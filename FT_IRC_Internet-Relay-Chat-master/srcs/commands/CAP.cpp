#include "../../includes/Server.hpp"

//İstemcilerin ve sunucuların belirli özellikleri destekleyip desteklemediklerini müzekare etmesini sağlar
void Server::Cap(std::vector<std::string>& params, Client& cli)
{
    //LS(list) ise istemcinin sunucunun desteklediğiyetenekleri listelemesini ister
    if (params[0] == "LS")
        cli._isCap = HEX; //İstemcinin yetenek listesini desteklediğini belirtir
    //İstemci bir bottur
    else if (params[0] == "BOT")
    {
        cli._isCap = BOT;
        _botFd = cli._cliFd; //sunucunun bot istemcisini tanımasını sağlar
    }
    //No capability = yetenek yok
    else
        cli._isCap = NC;
}
