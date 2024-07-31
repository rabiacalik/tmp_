#pragma once

#define NC 0 //normal client
#define HEX 1 //belirli bir özellik
#define BOT 2 //bot istemci

class Client
{
    public:

        int _cliFd; //istemcinin dosya tanımlayıcısı
        int _port; //istemcinin baglandığı port
        int _isCap; //hangi yeteneklere sahip / nc-bot-hex
        int _passChecked; //istemcinin şifresi dogrulanmış mı
        char _ipAddr[INET_ADDRSTRLEN]; //istemcini ip adresi - ınet_addrstrlen ipv4 adresini saklamak için gereken char dizisi
        std::string _buffer;
        std::string _user;
        std::string _host;
        std::string _ip;
        std::string _realName;
        std::string _nick;
        //mesaj hemen gitmeyebilir o yuzden kaybolmaması için kaydedilir bu kısmı detaylı bak !!!!
        std::vector<std::string> _messageBox;

        Client(): _isCap(0), _passChecked(0) {}
};
