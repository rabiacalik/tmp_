#include "../includes/Server.hpp"

// client fd lerini vectore kaydeder ve dondurur
std::vector<int> Channel::getFds() const
{
    std::vector<int> file;

    for (std::vector<Client>::const_iterator it = _channelClients.begin(); it != _channelClients.end(); ++it)
    {
        file.push_back(it->_cliFd);
    }
    return file;
}
