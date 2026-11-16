#include "../../incl/Server.hpp"

void Server::handle_who(Client* client, const std::vector<std::string>& args){


    if (!client->is_registered) {
        client->send_buffer += ": 451 :You have not registered\r\n";
        client->has_to_write = true;
        return;
    }

    if (args.empty()) {
        client->send_buffer += ": 461 WHO :Not enough parameters\r\n";
        client->has_to_write = true;
        return;
    }

    std::string channel_name = args[0];

    if (channel_name.empty() || channel_name[0] != '#') {
        client->send_buffer += ": 403 " + channel_name + " :No such channel\r\n";
        client->has_to_write = true;
        return;
    }

    std::map<std::string, Channel*>::iterator it = channels_map.find(channel_name);
     if (it == channels_map.end())
    {
        client->send_buffer += ": 403 " + channel_name + " :No such channel\r\n";
        client->has_to_write = true;
        return;
    }
    Channel *channel = it->second;

    const std::vector<Client*>& members = channel->getMembers();
    std::string names_msg = ": 353 " + client->nickname + " = " + channel_name + " :";
    for (size_t i = 0; i < members.size(); ++i) 
    {
        Client* m = members[i];
        if (channel->isOperator(m)) {
            names_msg += "@";
        }
        names_msg += m->nickname;
        if (i < members.size() - 1) {
            names_msg += " ";
        }
    }
    client->send_buffer += names_msg + "\r\n";
    client->send_buffer += ": 366 " + client->nickname + " " + channel_name + " :End of /NAMES list\r\n";
    client->has_to_write = true;
}
