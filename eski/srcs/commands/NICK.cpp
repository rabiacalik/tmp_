#include "../../includes/Server.hpp"

/*
Kullanıcıların takma adlarını değiştirmesini sağlar
    /NICK newNick
bir nickname farklı kanallarda aynı isme sahip olabilir mi !!!
*/
//Takma adın belirli kurallara uyup uymadığına bakar
int Server::CheckNickDepends(std::vector<std::string>& params, Client& cli)
{
    if (cli._isCap == NC) {// neden nc !!!
        passChecker(cli);
    }
    //bir parametre girilmeli, nick name başkası sahip olmamalı, belirtilen boyutu geçmemeli
    if (!(params.size() == 1 && !isNickExist(params[0]) && !params[0].empty() && params[0].size() <= 9))
    {
        //Hata lı durumlar burada işlenir
        if (params.size() != 1)
            Utils::writeMessage(cli._cliFd, ERR_NEEDMOREPARAMS(cli._nick, params[0]));
        else if (params[0].size() > 9)
            Utils::writeMessage(cli._cliFd, ERR_NONICKNAMEGIVEN);
        else if (params[0].empty())
            Utils::writeMessage(cli._cliFd, ERR_NICKNAMEEMPTY(cli._nick));
        else
            Utils::writeMessage(cli._cliFd, ERR_NICKNAMEINUSE(params[0]));
        return (0);
    }
    return (1);
}

void Server::Nick(std::vector<std::string>& params, Client& cli)
{
    //Nick geçerli ise
    if (!CheckNickDepends(params, cli))
        return ;

    //eski takma ad saklanır
    std::string oldNick = cli._nick;
    //nick güncellenir
    cli._nick = params[0];

    //tum kanallar içerisindeki kullanıclar gezilir ve takma ad değişikliği yapan operator kullanıcı ise operator adı guncellenir
    for (chanIt it = _channels.begin(); it != _channels.end(); ++it) {
        for (cliIt cit = it->_channelClients.begin(); cit != it->_channelClients.end(); ++cit) {
            if (oldNick == cit->_nick)
            {
                if (oldNick == it->_opNick)
                    it->_opNick = params[0];
                cit->_nick = params[0];
                break;
            }
        }
        //kullanıcıya mesaj gideerr
        Utils::writeMessage(cli._cliFd, RPL_NICK(oldNick, cli._user, cli._ipAddr, params[0]));
        //arayuz guncellenir
        showRightGui(cli, *it);
    }
    //kullanıcıtya mesaj gider
    Utils::writeMessage(cli._cliFd, RPL_NICK(oldNick, cli._user, cli._ipAddr, params[0]));
}
