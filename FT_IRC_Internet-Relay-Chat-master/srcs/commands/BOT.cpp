#include "../../includes/Server.hpp"

void Server::Bot(std::vector<std::string>& params, Client& cli)
{
    //Kullanıcının kimlik doğrulaması yapılmışmı
    passChecker(cli);

    //Takma adı yoksa önce takma ad oluştur diye meaj yollar
    if (cli._nick == "") {
        Utils::writeMessage(cli._cliFd, "Set your nickname first\r\n");
        return ;
    }
    //Parametre kontrolü
    if (params[0] == "") {
        Utils::writeMessage(cli._cliFd, ERR_NEEDMOREPARAMS(cli._nick, "BOT"));
        return ;
    }

    //Botun sunucuya bağlımı kontrolü
    if (_botFd == 0) {
        Utils::writeMessage(cli._cliFd, "Bot is not connected\r\n");
        return ;
    }
    //Kullanıcıdan alınann parametreleri mesaj haline getirir.
    std::string msg = "";
    for (size_t i = 0; i < params.size(); ++i) {
        msg += params[i] + " ";
    }
    //belirtilen formatta gönderilir
    Utils::writeMessage(_botFd, "bot " + cli._nick + " " + msg + "\n");
}
