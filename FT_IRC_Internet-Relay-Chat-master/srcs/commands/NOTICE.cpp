#include "../../includes/Server.hpp"
/*
Bir kullanıcıya bilgilendirici mesaj gönderir
    /NOTICE <hedef> <mesaj>
    /NOTICE user123 Merhaba, nasılsın?
    /NOTICE #genel Toplantı 5 dakika içinde başlayacak. (kanal ile kullanilabiliyormu dene!!!!)


*/
void Server::Notice(std::vector<std::string>& params, Client& cli)
{
    passChecker(cli);
    //bir kanal olmamalı
    if (params[0][0] == '#') {
        Utils::writeMessage(cli._cliFd, "Cannot notice a channel\r\n");
        return ;
    }
    //mesaj yer almalı
    if (params.size() < 2) {
        Utils::writeMessage(cli._cliFd, ERR_NEEDMOREPARAMS(cli._nick, "NOTICE"));
        return ;
    }
    //kullanıcı kendisine mesaj gönderemez
    if (cli._nick == params[0]) {
        Utils::writeMessage(cli._cliFd, "Cannot notice yourself\r\n");
        return ;
    }

    //mesaj gidecek kullanıc ıbulunur ve mesaj hazırlanır
    size_t flag = 0;
    for (cliIt it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->_nick == params[0])
        {
            if (params[1][0] == ':')
                getAfterColon(params);
            flag = 1;

            //Bu son kısmı anlayamadım !!!
            it->_messageBox.push_back(RPL_NOTICE(cli._nick, params[0], params[1]));
            FD_SET(it->_cliFd, &_writeFds);//yazılabilir dosya tanımlayıcısına eklenir
            break;
        }
    }
    //kullanıcı bulunamazsa
    if (flag == 0) {
        Utils::writeMessage(cli._cliFd, ERR_NOSUCHNICK);
    }
}
