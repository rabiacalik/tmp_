#include "../includes/Utils.hpp"

// Singleton deseni, belirli bir sınıftan birden fazla nesne oluşturulmasını önler ve tüm kod tabanı boyunca bu sınıfın tek örneğine erişim sağlar
Server* Server::singleton = NULL;  

Server::Server(): _botFd(0), _fdCount(0), flag(false) {}

Server::~Server() {}

// sadece bir tane server nesnesi oluşturulduğundan emin oluyor
Server* Server::getInstance()
{
    try {
        if (singleton == NULL) 
            singleton = new Server; 
        return singleton;
    } catch (std::exception & e) { 
        std::cerr << e.what() << std::endl;
        delete singleton;
        exit(1);
    }
}

// sunucu kapatılıyor. Fonksiyon ismi neden böyle !!!   
void Server::signalHandler(int number)
{
    std::cout << RED << "\nServer is shutting down!" << RESET << std::endl;
    delete singleton;
    close(singleton->_serverFd);
    exit(number);
}

void Server::initCommands()
{
    _commands["PASS"] = &Server::Pass;
    _commands["NICK"] = &Server::Nick;
    _commands["JOIN"] = &Server::Join;
    _commands["CAP"] = &Server::Cap;
    _commands["USER"] = &Server::User;
    _commands["WHO"] = &Server::Who;
    _commands["QUIT"] = &Server::Quit;
    _commands["PART"] = &Server::Part;
    _commands["PRIVMSG"] = &Server::Privmsg;
    _commands["NOTICE"] = &Server::Notice;
    _commands["KICK"] = &Server::Kick;
    _commands["MODE"] = &Server::Mode;
    _commands["TOPIC"] = &Server::Topic;
    _commands["INVITE"] = &Server::Invite;
    _commands["bot"] = &Server::Bot;
}

//socket oluşturup gerekli ayarlamaları yapar
void Server::createSocket()
{
    //ipV4 ve TCP ile bir socket oluşturulur ve fd ye kaydedilir
    if ((_serverFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        throw std::runtime_error("Socket");
    //F_SETFL flag ayarlar. bloklanmayan moda ayarlar 
    fcntl(_serverFd, F_SETFL, O_NONBLOCK);
    const int enable = 1;
    // adres yediden kullanımını açar
    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) 
        throw std::runtime_error("Setsockopt");
}

void Server::bindSocket(size_t const & port)
{
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // soketi belirli bir adrese ve porta bağlar
    if (bind(_serverFd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
        throw std::runtime_error("Bind");
    // dinleme yapmaya başlar 
    if (listen(_serverFd, SOMAXCONN) < 0)
        throw std::runtime_error("Listen");
}

//  sunucunun yeni bağlantıları kabul etmesini ve bu bağlantıları yönetmesini sağlar.
void Server::acceptRequest()
{
    Client tmp;
    sockaddr_in cliAddr;
    socklen_t cliSize = sizeof(cliAddr);

    // bekleyen baglantı ısteklerı kabul edılır
    tmp._cliFd = accept(_serverFd, (sockaddr *)&cliAddr, &cliSize);

    // soketin bloklanmayan moda gecirilmesi
    fcntl(tmp._cliFd, F_SETFL, O_NONBLOCK);
    if (tmp._cliFd <= 0) 
        throw std::runtime_error("Accept failed"); 

    // port adresi kaydedilir
    tmp._port = ntohs(cliAddr.sin_port);
    // istemcinin ıp adresinş metine cevirip tmp._ipAddr ye kopyalar
    inet_ntop(AF_INET, &(cliAddr.sin_addr), tmp._ipAddr, INET_ADDRSTRLEN);

    // yeni istemci soketini _readfds e ekler
    FD_SET(tmp._cliFd, &_readFds);
    std::cout << GREEN << "New client connected!" << RESET << std::endl;
    _fdCount++;
    _clients.push_back(tmp);
}

// (str) boşlıklara göre ayırır
// stringin ilk kelimesini komut olarak belirler (cmd)
// sonraki kelimeleri parametre (params)
// (ret) haritasına doldurur
std::map<std::string, std::vector<std::string> > Server::getParams(std::string const& str)
{
    std::stringstream ss(str);
    std::string tmp;
    std::map< std::string, std::vector<std::string> > ret;
    std::vector<std::string> params;
    ss >> tmp; // ilk kelime okunur
    std::string cmd;
    while (1)
    {
        cmd = tmp;
        if (ret.find(cmd) != ret.end()) // komut haritada zaten varsa ret doner
            return ret;
        // Her komutun kendi parametreleri olması gerektiğinden, yeni komut için sıfırlanır. 
        params.clear();
        ss >> tmp;
        // commands içerisinde yoksa params a ekler
        while (_commands.find(tmp) == _commands.end())
        {
            params.push_back(tmp);
            ss >> tmp;
            if (ss.eof()) // dosya sonuysa end of file ret doner
            {
                ret[cmd] = params;
                return ret;
            }
        }
        // Temel amaç, her komutun en az bir parametreye sahip olmasını sağlamaktır.
        if (ss.eof())
        {
            params.push_back("");
            ret[cmd] = params;
            return ret;
        }
        if (params.empty()) // bu iki if kosuluna gerek var mı !!!!!
            params.push_back(""); 
        ret[cmd] = params;
    }
    return ret;
}

void Server::commandHandler(std::string& str, Client& cli)
{
    std::map<std::string, std::vector<std::string> > params = getParams(str);

    // bilinmeyen komutlar için hata mesajı verir
    for (std::map<std::string, std::vector<std::string> >::iterator it = params.begin(); it != params.end(); ++it)
    {
        if (_commands.find(it->first) == _commands.end())
        {
            Utils::writeMessage(cli._cliFd, "421 : " + it->first + " :Unknown command!\r\n");
            std::cout << RED << it->first << " command not found!" << RESET << std::endl;
        }
        // bilinen bir komutsa gerekliişlevi yapar
        else
            (this->*_commands[it->first])(it->second, cli);
    }
}

//sunucunun istemcilerden gelen veri okuma olaylarını yönetir. İstemcilerden veri okur, gelen veriyi işler ve gerekli durumlarda komutları yürütür.
void Server::readEvent(int* state)
{
    for (cliIt it = _clients.begin(); it != _clients.end(); ++it)
    {
        //istemcinin dosya tanımlayıcısının (_cliFd) _readFdsSup kümesinde olup olmadığı kontrol edilir.
        if (FD_ISSET(it->_cliFd, &_readFdsSup))
        {
            // verinin başarıyla okunduğunu veya işlenmeye başlandığını belirtir.
            *state = 0;

            //it->_cliFd dosya tanımlayıcısından okuma yapar ve readed e atar
            int readed = read(it->_cliFd, _buffer, 1024);
            //istemci bağlantısının kapandığını veya bir hata olduğunu gösterir.
            if (readed <= 0)
            {
                // neden bu işlemler yapılıyor !!!
                std::vector<std::string> tmp;
                tmp.push_back("");
                (this->*_commands["QUIT"])(tmp, *it);
            }
            else
            {
                // _buffer tamponunun sonuna 0 eklenir c-string oluşturulur (neden!!!!)
                _buffer[readed] = 0;
                std::string tmp = _buffer;

                // komutun tanımlandığını belirtir.
                if (tmp == "\n")
                {
                    *state = 0; break;
                }
                // veri eksik yada tanımlanmamışsa. veri bir sonraki okuma işlemini bekleyecektir.
                if (tmp[tmp.length() - 1] != '\n')
                {
                    it->_buffer += tmp;
                    *state = 0; break;
                }
                // okunan veri yeni satır karakteri ile bitiyorsa, bu veri tamamlanmış bir komut satırıdır.
                else
                {
                    it->_buffer = it->_buffer + tmp;
                }
                // buffer içeriği konsola yazdırılır
                std::cout << YELLOW << it->_buffer << RESET;
                commandHandler(it->_buffer, *it);
                it->_buffer.clear();
            }
            break;
        }
    }
}

//Dosya tanımlayıcı kümelerini başlatır ve sunucu soketini okumak için ayarlar
void Server::initFds()
{
    //FD_ZERO = select sistem çağrısı ile kullanılan dosya tanımlayıcı kümelerini (fd_set) sıfırlamak için kullanılır. 
    //kümeyi sıfırlar, izlenmesi gereken okuma olaylarını içerecek şekilde başlatılır.
    FD_ZERO(&_readFds);
    //kümeyi sıfırlar, izlenmesi gereken yazma olaylarını içerecek şekilde başlatılır.
    FD_ZERO(&_writeFds);
    //Bu küme, _readFds kümesini geçici olarak kopyalamak ve kullanmak için başlatılır.
    FD_ZERO(&_readFdsSup);
    //_writeFds kümesini geçici olarak kopyalamak ve kullanmak için başlatılır.
    FD_ZERO(&_writeFdsSup);
    // sunucuya ait dosya tanımlayıcısını okuma olaylarının izleneceği dosya tanımlayıcılarını tutatn fd_set yapısına(readfds) e ekler
    FD_SET(_serverFd, &_readFds);
}

//sunucu ile istemcile arasındaki yazma olaylarını yönetir
void Server::writeEvent()
{
    // sunucuya bağlı tüm istemciler dolaşılır
    for (cliIt it = _clients.begin(); it != _clients.end(); ++it)
    {
        //fd nin belirtilen fs_set(writeFdSup) da olup olmadığı kontrol edilir
        if (FD_ISSET(it->_cliFd, &_writeFdsSup))
        {
            //istemciye mesaj gönderilir neden ilk mesaj gidiyor !!!
            int writed = write(it->_cliFd, it->_messageBox[0].c_str(), it->_messageBox[0].size());
            it->_messageBox.erase(it->_messageBox.begin());
            if (it->_messageBox.empty())
                FD_CLR(it->_cliFd, &_writeFds);
            if (writed <= 0)
            {
                std::vector<std::string> tmp;
                tmp.push_back("");
                (this->*_commands["QUIT"])(tmp, *it);
            }
            break ;
        }
    }
}

//sunucunun ana döngüsünü yönetir
void Server::run()
{
    // select çağrısının durumunu ve diğer işlemleri izlemek için kullanılır
    int state = 0;

    initFds();

    //sunucunun sürekli olarak çalışmasını sağlar
    while (1)
    {
        //dosya tanımlayıcı kümeleri kopyalanır. select çağrısı kümeleri değiştirebileceğinden bu işlem yapılmaktadır
        _readFdsSup = _readFds;
        _writeFdsSup = _writeFds;

        // belirtilen dosya tanımlayısı kümeerindeki olaylar dinlenir. olaylar gerçekleşene kadar bekler. fdCount+4 kontrol edilecek max fd sayısı
        state = select(_fdCount + 4, &_readFdsSup, &_writeFdsSup, NULL, 0);
        if (state == 0)
            throw std::runtime_error("Select");

        //_serverFd _readFdsSup içerisindeyse bu sunucunun yeni bir bağlantı isteği aldığı anlamına gelir 
        if (FD_ISSET(_serverFd, &_readFdsSup)) {
            acceptRequest(); // bağlantı kabul edilir
            state = 0; continue;  // state sıfırlanır ve döngünün başına dönülür.
        }

        // select herhangi bir olay bildirmişse
        if (state) {
            readEvent(&state);  //istemciden veri okunur
            if (state == 0)     // eğer sıfır olarak ayarlanmışsa döngünün başına dönülür
                continue;
        }

        // yazma olayının işlenmesi
        if (state) {
            writeEvent();
            state = 0;
            continue;
        }
    }
}

//sunucunun başlatılmasından ve çalıştırılmasından sorumlu 
void Server::manageServer(size_t const& port, std::string const& password)
{
    setPort(port);
    setPassword(password);
    initCommands();
    createSocket();
    bindSocket(port);
    printStatus();
    run();
}
