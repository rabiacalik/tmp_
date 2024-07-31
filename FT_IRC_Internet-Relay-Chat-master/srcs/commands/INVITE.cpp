#include "../../includes/Server.hpp"

/*
İstemcinin bir kanala kullanıcı davet etmesi için kullanılır
/INVITE komutunu çağıran kullanıcı "bob" kullanıcısını "#general" kanalına davet eder    
    - INVITE bob #general
"bob" a şu mesaj gider
    - :alice!alice@host INVITE bob :#general
(Farklı kullanıcılar oluşurarak bunları dene !!!)

Hata Mesajları
ERR_NOSUCHNICK (401): Davet edilen kullanıcı adı geçersiz veya mevcut değilse.
ERR_NOSUCHCHANNEL (403): Kanal adı geçersiz veya mevcut değilse.
ERR_CHANOPRIVSNEEDED (482): Kanal operatörü olmanız gerektiğini belirten hata.
ERR_USERONCHANNEL (443): Kullanıcı zaten belirtilen kanalda bulunuyorsa.

kanala davet işlemini sadece o kanalın operatorü yapabilir
*/

void Server::Invite(std::vector<std::string>& params, Client& cli)
{
    //yetkili mi
    passChecker(cli);
    //Belirtilen kanal mevcut mu
    if (isChannelExist(params[1]) == 0) {
        Utils::writeMessage(cli._cliFd, ERR_NOSUCHCHANNEL(params[1], params[0]));
        return ;
    }
    size_t flag = 0; //Kullanıcı bulun du mu flag kullanımına gerek var mıydı !!
    for (cliIt it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->_nick == params[0]) //davet edilecek kullanıcı ile listedeki nickler karşılaştırılır
        {
            flag = 1;
            Channel chan = getChannel(params[1]); //ilgili kanal alınır
            //davet eden kullanıcı kanalın operatötü değilse hata mesajı iletilir
            if (cli._nick != chan._opNick) {
                Utils::writeMessage(cli._cliFd, ERR_CHANOPRIVSNEEDED(cli._nick, params[1]));
                return ;
            }
            Utils::writeMessage(it->_cliFd, RPL_INVITE(cli._nick, cli._ipAddr, chan._name, params[1]));
        }
    }
    if (flag == 0)
        Utils::writeMessage(cli._cliFd, ERR_NOSUCHNICK);
}
