#include "../../includes/Server.hpp"
/*
kullanıcının kanala katılmasını sağlar
/JOIN <channel>
/JOIN #gizli sifre123 -- şifresi varsa

    - kanal mevcut değilse otomatik oolarak oluşturulur ve kullanıcı operator yapılır
    - eğer kullanıcı gereksinimleri yada varsa şifreyi sağlarsa kanala alınır
    - bazı kanalların limiti olabilir

Local ve Küresel kanalları detaylı araştır !!!
Küresel Kanallar ( # ) Sunucular arasında paylaşılabilir ve sunucular arasındaki iletişime izin verir
Local Kanallar ( & ) Yalnızca tek bir sunucu ve sunucular arasında paylaşılmaz


*/

void Server::Join(std::vector<std::string>& params, Client& cli)
{
<<<<<<< HEAD
    if (cli._passChecked == 0){
        passChecker(cli);
        return;
    }
    if (params.empty()){
=======
    //şifre kontrolü yapılır
    if (cli._passChecked == 0)
    {
        passChecker(cli);
        return;
    }

    //parametre kontrolü
    if (params.empty())
    {
>>>>>>> 6c660cb8f14e021b1dd118fa0e0f2be671ce736e
        Utils::writeMessage(cli._cliFd, ERR_NEEDMOREPARAMS(cli._nick, "JOIN"));
        return;
    }
    std::vector<std::string> channels;
    std::string keys = "";
    std::stringstream ss(params[0]);
    std::string item;
<<<<<<< HEAD
    while (std::getline(ss, item, ',')){
        channels.push_back(item);
    }
    if (params.size() == 2){
=======

    //params[0] içindeki kanallar channels vektorune kaydedilir
    while (std::getline(ss, item, ','))
    {
        channels.push_back(item);
    }

    //iki elemanlıysa şifre de var demektir onlarda kayır edilir
    if (params.size() == 2)
    {
>>>>>>> 6c660cb8f14e021b1dd118fa0e0f2be671ce736e
        keys = params[1];
    }

    for (size_t i = 0; i < channels.size(); ++i)
    {
        std::string chan = channels[i];
        std::string key = "";
<<<<<<< HEAD

        if (!keys.empty()){
=======
        
        //keylerin ayrıştırılması yapılır
        if (!keys.empty())
        {
            //stringi stringstream a dönüştürür
>>>>>>> 6c660cb8f14e021b1dd118fa0e0f2be671ce736e
            std::stringstream keyStream(keys);
            std::string keyItem;  //virgülden ayrıştırılan keyler buraya yazılır
            //virgüle göre keyleri ayırır
            std::getline(keyStream, keyItem, ',');

            //bu kkısmı neden yaptığımızı anlamadım !!!
            if (i == 0 || keyStream.eof())
            {
                key = keyItem;
            }
        }
<<<<<<< HEAD
        if (chan.empty()){
            Utils::writeMessage(cli._cliFd, ERR_NEEDMOREPARAMS(cli._nick, "JOIN"));
            return;
        }
        if (chan[0] != '#' && chan[0] != '&'){
            Utils::writeMessage(cli._cliFd, ERR_INVCHANNAME);
            return;
        }
=======

        //kanal boş mu !! zaten var olmayan bir kanalı kontrol ediyorsak ne olacak
        if (chan.empty())
        {
            Utils::writeMessage(cli._cliFd, ERR_NEEDMOREPARAMS(cli._nick, "JOIN"));
            return;
        }

        //kanalın başında global local işaretlerinden biri olmalı
        if (chan[0] != '#' && chan[0] != '&')
        {
            Utils::writeMessage(cli._cliFd, ERR_INVCHANNAME);
            return;
        }

        //istemci belirtilen kanalda değilse
>>>>>>> 6c660cb8f14e021b1dd118fa0e0f2be671ce736e
        if (clientIsInThere(cli, chan) == 0)
        {
            //Kullanıcının nick nami varsa devam eder
            if (!cli._nick.empty())
            {
                //Belirli bir kanal zaten mevcutsa 1, yoksa 0
                int isThere = 0;
                // mevcut kanallar gezilir
                for (chanIt it = _channels.begin(); it != _channels.end(); ++it)
                {
                    //verilen kanal ismi kanal listesinde varsa
                    if (it->_name == chan)
                    {
                        //kanalın anahtarıyla girilen anahtar uyumlu  mu
                        if (it->_key == key)
                        {
                            //Kanalda kullanıcı limiti varsa ve kanal doluysa hata mesajı
                            if (it->_userLimit != 0 && it->_channelClients.size() >= it->_userLimit)
                            {
                                Utils::writeMessage(cli._cliFd, ERR_CHANNELISFULL(cli._nick, chan));
                                return;
                            }

                            //Kullanıcı kanala eklenir
                            it->_channelClients.push_back(cli);
<<<<<<< HEAD
                            //buna gerek var mı !!!
=======
                            //operator kullanıcı guncellenir GEREKLİ Mİ !!!
>>>>>>> 6c660cb8f14e021b1dd118fa0e0f2be671ce736e
                            it->_opNick = it->_channelClients[0]._nick;
                            //konsola ve kulllanıcıya mesaj y6azılır
                            //kullanıcı kanala girdi
                            Utils::writeMessage(cli._cliFd, RPL_JOIN(cli._nick, cli._ip, chan));
                            std::cout << PURPLE << "Client " << cli._nick << " has entered \'" << chan << "\'" << RESET << std::endl;
                            //tum kanal kullanıcılarına guncel kullanıcı listesi mesaj olarak iletilir.
                            showRightGui(cli, *it);
                        }
<<<<<<< HEAD
                        else{
=======
                        //girilen şifre yanlışsa hata
                        else
                        {
>>>>>>> 6c660cb8f14e021b1dd118fa0e0f2be671ce736e
                            Utils::writeMessage(cli._cliFd, ERR_BADCHANNELKEY(cli._nick, chan));
                        }
                        
                        isThere = 1;
                        break;
                    }
                }
                //Belirtilen kanal yoksa
                if (isThere == 0)
                {
                    Channel tmp;
                    tmp._name = chan;
                    if (!key.empty()){
                        tmp._key = key;
                    }
                    //yeni kanal oluşturulur ve kullanıcı eklenip operator yapılır
                    tmp._channelClients.push_back(cli);
                    tmp._opNick = tmp._channelClients[0]._nick;
                    //yeni kanal kanal listesine eklenir
                    _channels.push_back(tmp);

                    //Kullanıcı kanala girdi
                    Utils::writeMessage(cli._cliFd, RPL_JOIN(cli._nick, cli._ip, chan));
<<<<<<< HEAD
                    if (!key.empty()){
=======
                    //şifre ile kanal oluşturuldu yada şifresiz oluşturuldu mesajı
                    if (!key.empty())
                    {
>>>>>>> 6c660cb8f14e021b1dd118fa0e0f2be671ce736e
                        std::cout << PURPLE << "Channel " << chan << " created with " << key << RESET << std::endl;
                    }
                    else{
                        std::cout << PURPLE << "Channel " << chan << " created" << RESET << std::endl;
                    }

                    showRightGui(cli, tmp);
                }
            }
<<<<<<< HEAD
            else{
                Utils::writeMessage(cli._cliFd, "Set your nick before!\r\n");
            }
        }
        else{
=======
            //nicki yoksa hata
            else
            {
                Utils::writeMessage(cli._cliFd, "Set your nick before!\r\n");
            }
        }
        //istemci zaten kanaldaysa hata
        else
        {
>>>>>>> 6c660cb8f14e021b1dd118fa0e0f2be671ce736e
            Utils::writeMessage(cli._cliFd, "You are already in this channel\r\n");
        }
    }
}
