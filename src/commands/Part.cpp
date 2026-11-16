#include "../../incl/Server.hpp"

void Server::handle_part(Client* client, const std::vector<std::string>& args)
{
    if (!client->is_registered) {
        client->send_buffer += ": 451 :You have not registered\r\n";
        client->has_to_write = true;
        return;
    }
    if (args.empty()) {
        client->send_buffer += ": 461 PART :Not enough parameters\r\n";
        client->has_to_write = true;
        return;
    }
    std::string channel_name = args[0];
    std::map<std::string, Channel*>::iterator it = channels_map.find(channel_name);
    if (it == channels_map.end()) {
        client->send_buffer += ": 403 " + channel_name + " :No such channel\r\n";
        client->has_to_write = true;
        return;
    }
    Channel* ch = it->second;
    if (!ch->isMember(client)) {
        client->send_buffer += ": 442 " + channel_name + " :You're not on that channel\r\n";
        client->has_to_write = true;
        return;
    }
    std::string msg = ":" + client->get_id() + " PART " + channel_name + "\r\n";
    ch->removeMember(client);
    client->joined_channels.erase(channel_name);
    broadcastToChannel(ch, msg, NULL);

     if (ch->getMembers().size() == 0)
        close_channel(ch);
}
