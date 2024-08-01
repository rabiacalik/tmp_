#include "../../includes/Server.hpp"

/*
Belirli bir kullanıcıyı kanaldan çıkarmak için kullanılır
sadece operator kanaldan kullanıcı atabilir
    /KICK <kanal> <kullanıcı> [sebep]

*/


void Server::Kick(std::vector<std::string>& params, Client& cli)
{
    //Kullanıcının kimlik kontrolü
    passChecker(cli);

    //en az kullanıcı ve kanal adı yer almalı
    if (params.size() < 2) {
        Utils::writeMessage(cli._cliFd, ERR_NEEDMOREPARAMS(cli._nick, params[0]));
        return ;
    }

    //Kullanıcı kendi kendini kanaldan çıkaramaz
    else if (params[1] == cli._nick) {
        Utils::writeMessage(cli._cliFd, ERR_CHANOPKICK(cli._nick, params[0]));
        return ;
    }

    else
    {
        int flag = 0;

        //kanallar gezilir
        for (chanIt it = _channels.begin(); it != _channels.end(); ++it) {
            //kanal adı listede varsa
            if (it->_name == params[0])
            {
                //komutu kullanan kullanıcı operator değilse hata
                if (cli._nick != it->_opNick) {
                    Utils::writeMessage(cli._cliFd, ERR_CHANOPRIVSNEEDED(cli._nick, params[0]));
                    return ;
                }
                // Kullanıcının kanaldan çıkarılması
                //kanaldaki kullanıcılar dolaşılır 
                for (cliIt it2 = it->_channelClients.begin(); it2 != it->_channelClients.end(); ++it2) {
                    flag = 1; //kanal bulundu

                    // aranan kullanıcı bulunur
                    if (it2->_nick == params[1]) {
                        std::string msg = "";
                        // eğer mesaj da komutla gelmişse o da mesaja eklenir
                        if (params.size() == 3)
                            msg = params[2];
                        Utils::writeMessage(it2->_cliFd, RPL_KICK(it->_opNick, params[0], params[1], msg));
                        //kullanıcı kanaldan çıkarılır
                        it->_channelClients.erase(it2);
                        //kullanıcı arayüzü güncellenir
                        showRightGui(cli, *it);
                        return ;
                    }
                }
            }
        }
        //kanal bulunamamışsa hata
        if (flag == 0)
            Utils::writeMessage(cli._cliFd, ERR_NOSUCHCHANNEL(cli._nick, params[0]));
        //kullanıcı bulunamamışsa hata
        else if (flag == 1)
            Utils::writeMessage(cli._cliFd, ": 431 " + cli._nick + " " + params[0] + " : " +  params[1] + " not in this channel\r\n");
    }
}
