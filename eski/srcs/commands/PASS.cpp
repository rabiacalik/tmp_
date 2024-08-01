#include "../../includes/Server.hpp"
/*
İstemcinin sunucuya bağlanmadan önce bir parola belirlemesini yada doğrulamasına olanak tanır
_passChecker burada ayarlanır sunucuya bağlandığımızı belirtir
*/
void Server::Pass(std::vector<std::string>& params, Client& cli)
{
    //sadece bir parametre(şifre) içermelidir
    if (params.size() != 1)
        Utils::writeMessage(cli._cliFd, ERR_NEEDMOREPARAMS(params[0], _password));
    //daha önce parola kontrolu yapılmamış olmalı
    else if (cli._passChecked == 1)
        Utils::writeMessage(cli._cliFd, ERR_ALREADYREGISTRED);
    //girilen şifre sunucunun şifresi olmalı
    else if (params[0] == _password)
    {
        cli._passChecked = 1;
        Utils::writeMessage(cli._cliFd, Utils::welcome());
    }
    passChecker(cli); //gerek var mı kontrole az önce bir yaptık !!!!
}
