#pragma once

class Client; class Channel //niye böyle tanımlı tam tersi olmalı mı !!!!
{
    public:

        size_t _userLimit;
        std::string _name;
        std::string _topic;
        std::string _key;
        std::string _opNick; //operator kullanıcının nicki
        std::vector<Client> _channelClients;
        Channel() : _userLimit(0), _name(""), _topic(""), _key(""), _opNick("") {}

        std::vector<int> getFds() const;
};
