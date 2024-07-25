#include "../../includes/Bot.hpp"

Bot* Bot::singleton = NULL;

//Bot sınıfı oluşturulur
Bot* Bot::getInstance()
{
    try {
        if (singleton == NULL) 
            singleton = new Bot;
        return singleton;
    } catch (std::exception & e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
}

Bot::~Bot()
{
    if (singleton != NULL)
        delete singleton;
    singleton = NULL;
    close(_fd);
}

void Bot::setPasword(std::string const& password)
{
    _password = password;
}

void Bot::setPort(int port)
{
    _port = port;
}

void Bot::createSocket()
{
    //soket oluşturulur
    if ((_fd = socket(AF_INET, SOCK_STREAM, 0)) <= 0) 
        throw std::runtime_error("socket() failed");
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr)); //sıfır ile doldurur
    
    addr.sin_family = AF_INET; //IPv4
    addr.sin_port = htons(_port);
    
    //ip adresini string den binary e çevirir
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    //belirtilen adrese bağlanmaya çalışır
    if (::connect(_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
        throw std::runtime_error("connect() failed");

    std::cout << RED << "LAZ ZİYA " << BLUE << "Bot connected to server" << RESET << std::endl;//Laz ziya mı !!!
}

void Bot::run()
{
    int login = 0;

    while (1)
    {
        //Giriş yapılmamışsa giriş yapılır ve mesaj yazdırılır
        if (!login)
        {
            std::string msg = "CAP BOT\nPASS " + _password + "\nNICK " + RED "LAZ ZİYA BOT\n";
            Utils::writeMessage(_fd, msg);
            login = 1;
        }
        char buffer[1024];
        int bytes_received = recv(_fd, buffer, sizeof(buffer), 0); //sunucudan veri alıp kaydeder
        buffer[bytes_received] = '\0';
        std::string tmp = buffer;

        if (bytes_received <= 0)
            throw std::runtime_error("recv() failed");

        std::stringstream ss(tmp);
        std::string ctr;
        ss >> ctr;

        if (ctr == "bot")
        {
            ss >> tmp; 
            ss >> ctr; 
            std::string msg = ctr;
            while (ss >> ctr)
                msg += " " + ctr;
            std::string writeCmd = "NOTICE * " + tmp + " " + msg + "\r\n";
            Utils::writeMessage(_fd, writeCmd);
        }
    }
}

void Bot::manageBot(int port, std::string const& password)
{
    setPasword(password);
    setPort(port);
    createSocket();
    run();
}