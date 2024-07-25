#include "../includes/Server.hpp"

//Nick te mevcut - sunucudaki istemciler arasında bu nickName sahip biri varsa 1 doner, yoksa 0
int Server::isNickExist(std::string const& nick)
{
    for (cliIt it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->_nick == nick)
            return 1;
    }
    return 0;
}

//Bir istemciyi sunucudan bağlantıdan çıkarır
void Server::kickClient(cliIt& it)
{
    // neden -3 !!!
    std::cout << RED << "Client " << it->_cliFd - 3  << " disconnected!" << RESET << std::endl;

    //istemci fd_set kütüphanelerşnden çıkarılır
    FD_CLR(it->_cliFd, &_readFds);
    FD_CLR(it->_cliFd, &_writeFds);
    //istemcinin dosya tanımlayıcısını kapatır. istemciye olan fiziksel bağlantıyı keser
    close(it->_cliFd);
    //istemci listeden çıkarılır
    _clients.erase(it);
}

//Şifre doğrulaması yapılmamışsa istemci kapatılır
void Server::passChecker(Client& client)
{
    // henüz şifre doğrulaması yapılmamışsa
    if (client._passChecked == 0)
    {
        std::cout << RED << client._cliFd << " is not logged in." << RESET << std::endl;
        //istemcinin bağlantısı kapatıılır
        for (cliIt it = _clients.begin(); it != _clients.end(); ++it) {
            if (client._cliFd == it->_cliFd)
            {
                Utils::writeMessage(client._cliFd, ERR_PASSWDMISMATCH);
                kickClient(it);
                break;
            }
        }
    }
}

//(:) yı params tan çıkarır ve sıralar
void Server::getAfterColon(std::vector<std::string>& params)
{
    //genellikle (:) yada gereksiz bir karakter temizlenir !!!
    params[1].erase(0, 1);
    //parametreleri sıralar
    for (size_t i = 2; i < params.size(); ++i)
    {
        params[1] += " " + params[i];
    }
}

//_channels da belirtilen kanal adını arar ve referans döner
Channel& Server::getChannel(std::string const& channelName)
{
    chanIt it;
    for (it = _channels.begin(); it != _channels.end(); ++it)
    {
        if (it->_name == channelName) {
            break;
        }
    }
    return *it;
}

//Belirtilen chanelname nin var olup olmadığına bakar, yukarıdaki ile cok benzer!!!!
int Server::isChannelExist(std::string const& channelName)
{
    for (chanIt it = _channels.begin(); it != _channels.end(); ++it)
    {
        if (it->_name == channelName)
            return 1;
    }
    return 0;
}

//İstemcinin belirtilen kanalda olup olmadığına bakar
int Server::clientIsInThere(Client& client, std::string const& chanName)
{
    for (chanIt it = _channels.begin(); it != _channels.end(); ++it)
    {
        if (it->_name == chanName)
        {
            for (cliIt it2 = it->_channelClients.begin(); it2 != it->_channelClients.end(); ++it2)
            {
                //neden nick kontrolü yapılıyor!!!!
                if (it2->_nick == client._nick)
                    return (1);
            }
        }
    }
    return (0);
}

//Verilen kullanıcı adının clients te mevcut olup olmadığını kontrol eder
int Server::getOpFd(std::string const& opName)
{
    for (cliIt it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->_nick == opName)
            return (it->_cliFd);
    }
    return (0);
}

//Kanalın istemcilerini ve operatörünü mesaj halinde tüm istemcilere gönderir
void Server::showRightGui(Client &cli, Channel &tmp)
{
    std::string msg;
    if (tmp._name.empty())
        return;
    for(std::vector<Client>::iterator it = tmp._channelClients.begin() ; it != tmp._channelClients.end(); ++it)
    {
        if (it->_cliFd == getOpFd(tmp._opNick))
            msg += "@";
        msg += (*it)._nick + " ";
    }
    Utils::writeAllMessage(tmp.getFds(), RPL_NAMREPLY(cli._nick, tmp._name, msg));
}

void Server::setPort(size_t const& port)
{
    _port = port;
}

void Server::setPassword(std::string const& password)
{
    _password = password;
}

void Server::printStatus()
{
    std::cout << CYAN << "Server running on this pc " << RESET << std::endl;
    std::cout << CYAN << "Password: " << _password << RESET << std::endl;
    std::cout << CYAN << "Port: " << _port << RESET << std::endl;
}
