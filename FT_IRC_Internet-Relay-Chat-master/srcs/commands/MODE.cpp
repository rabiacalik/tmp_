#include "../../includes/Server.hpp"
/*
Kanalların ve kullanıcıların modlarını yönetmak için kullanılır
    /MODE <kanal> <+|-mod> [parametreler]☻
        - mod ekleme yada çıkarma
        - operatorler yapabilir

    +k <şifre> -- Kanala girmek için şifre gerektirir
    +l <sayı> -- kanalın kullanıcı limitini belirler
    +o: Kullanıcıyı kanal operatörü yapar., /MODE #general +o JohnDoe

    işlemlerin başarılı olması durumunda mesaj gitmiyor. düzenlenmeli mi !!!!
*/
void Server::Mode(std::vector<std::string>& params, Client& cli)
{
    passChecker(cli);
    if (params.size() == 1)
        return;
    if (params.size() < 1 || params.size() > 3) {
        Utils::writeMessage(cli._cliFd, ERR_NEEDMOREPARAMS(cli._nick, params[0]));
        return ;
    }
    if (!isChannelExist(params[0])) {
        Utils::writeMessage(cli._cliFd, ERR_NOSUCHCHANNEL(cli._nick, params[0]));
        return;
    }
    for (chanIt it = _channels.begin(); it != _channels.end(); ++it) {
        if (it->_name == params[0])
        {
            if (it->_opNick != cli._nick) {
                Utils::writeMessage(cli._cliFd, ERR_CHANOPRIVSNEEDED(cli._nick, params[0]));
                return ;
            }
            int flag = 0;
            modesOp(it, params, &flag);
            modesLimit(it, params, &flag);
            modesKey(it, params, &flag);
            //bilinmeyen mod ise hata
            if (!flag) {
                Utils::writeMessage(cli._cliFd, ERR_UNKNOWNMODE(cli._nick, params[0], params[1]));
                return ;
            }
        }
    }
}

//Kanal operatorlüğü atama işlemi
void Server::modesOp(chanIt& it, std::vector<std::string>& params, int* flag)
{
    //kanal operatorlüğü klenecek
    if (params[1] == "+o")
    {
        *flag = 1;//operator komutu verilmiş
        size_t flag2 = 0;
        for (cliIt it2 = it->_channelClients.begin(); it2 != it->_channelClients.end(); ++it2) {
            //Operator yapılacak kullanıcı bulunur
            if (it2->_nick == params[2])
            {
                //Kullanıcı zaten operator ise işlem sonlandırılır
                if (it2->_nick == it->_opNick)
                    return ;
                flag2 = 1; //kullanıcı kanalın bir parçasıysa
                size_t i;

                //Kullanıcı konumu bulunuyor (yukarıdaki for da yapılamazmıydı !!!)
                for (i = 0; i < it->_channelClients.size(); ++i) {
                    if (it->_channelClients[i]._nick == it2->_nick)
                        break ;
                }

                //Bulunan kullanıcı kanalı ilk eleman ile yer değiştirir. opnick kullanıcının takma adı olarak ayarlanır ve guncelleme yapılır
                Client tmp = it->_channelClients[i];
                it->_channelClients[i] = it->_channelClients[0];
                it->_channelClients[0] = tmp;
                it->_opNick = it->_channelClients[0]._nick;
                showRightGui(tmp, *it);
                return ;
            }
        }
        if (flag2 == 0) 
        {
            Utils::writeMessage(getOpFd(it->_opNick), "User is not in the channel\r\n");
            return ;
        }
    }
}

//Kanala bir kullanıcı limiti koyar
void Server::modesLimit(chanIt& it, std::vector<std::string>& params, int* flag)
{
    //limit modu mu kontrolü
    if (params[1] != "+l")
        return ;
    *flag = 1; //limit komutu verilmiş
    //kullanıcı limiti ayarlanır
    if (params[1] == "+l" && params.size() == 3)
        it->_userLimit = std::atoi(params[2].c_str());
    //sayı girilmemişse hata
    else if (params[1] == "+l" && params.size() != 3)
        std::cout << "Not enough paramaters for '+l' " << std::endl;
    Utils::writeMessage(getOpFd(it->_opNick), RPL_MODE(it->_opNick, params[0], "+l", params[2]));
    return ;
}

//Kanallara şifre eklemek yada kaldırmak için kullanılır
void Server::modesKey(chanIt& it, std::vector<std::string>& params, int* flag)
{
    //girdilerin olması gereken opsiyondan farklıysa hata
    if (params[1] != "+k" && params[1] != "-k")
        return ;
    if (params[1] == "+k" && params.size() != 3)
        return ;
    if(params[1] == "+k")
        it->_key = params[2];
    else if (params[1] == "-k")
        it->_key = "";
    *flag = 1;//işlem başarılı
}
