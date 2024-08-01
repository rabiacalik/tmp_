#include "../../includes/Server.hpp"

/*
CAP - Capabilities - yetkinlikler
İstemcilerin ve sunucuların belirli özellikleri destekleyip desteklemediklerini müzekare etmesini sağlar
    /CAP LS
        sunucunun desteklediği yetenekleri listeleyecektir
    /CAP NC
        no capability - yetkinlik yok 
    /CAP BOT
        bot işte
*/
void Server::Cap(std::vector<std::string>& params, Client& cli)
{
    if (params[0] == "LS") 
        cli._isCap = HEX;
    else if (params[0] == "BOT"){
        cli._isCap = BOT;
        _botFd = cli._cliFd;
    }
    else
        cli._isCap = NC;
}
