#include "../../incl/Server.hpp"
#include <cctype>

bool check_alphanum(std::string str)
{
    for (size_t i = 1; i < str.size(); ++i)
    {
        if (!std::isalnum(static_cast<unsigned char>(str[i])))
            return true;
    }
    return false;
}

void Server::handle_join(Client* client, const std::vector<std::string>& args)
{
    if (!client->is_registered) {
        client->send_buffer += ": 451 :You have not registered\r\n";
        client->has_to_write = true;
        return;
    }

    if (args.empty()) {
        client->send_buffer += ": 461 JOIN :Not enough parameters\r\n";
        client->has_to_write = true;
        return;
    }

    std::string channel_name = args[0];
    std::string password = (args.size() > 1) ? args[1] : "";

    if (channel_name.empty() || channel_name[0] != '#') {
        client->send_buffer += ": 403 " + channel_name + " :No such channel\r\n";
        client->has_to_write = true;
        return;
    }

    if (check_alphanum(channel_name)) {
        client->send_buffer += ": 476 " + channel_name + " :Invalid channel name\r\n";
        client->has_to_write = true;
        return;
    }

    std::map<std::string, Channel*>::iterator it = channels_map.find(channel_name);
    Channel* channel = NULL;

    if (it == channels_map.end()) {
        try {
            channel = new Channel(channel_name);
        }
        catch (const std::bad_alloc &e) {
            client->send_buffer += ": 437 " + client->nickname + " #"
                                + channel_name + " :Ressource temporarily unavailable\r\n";
            client->has_to_write = true;
            return ;
        }
        channels_map[channel_name] = channel;
        channel->addMember(client);
        client->joined_channels[channel_name] = channel;
        channel->addOperator(client);
    } 
    else {
        channel = it->second;

        if (channel->isMember(client)) {
            return;
        }
        if (channel->isInviteOnly() && !channel->isInvited(client)) {
            client->send_buffer += ": 473 " + channel_name + " :Cannot join channel (+i)\r\n";
            client->has_to_write = true;
            return;
        }
        if (!channel->getPassword().empty() && channel->getPassword() != password) {
            client->send_buffer += ": 475 " + channel_name + " :Cannot join channel (+k)\r\n";
            client->has_to_write = true;
            return;
        }
        if (channel->getUserLimit() > 0 && channel->getMembers().size() >= static_cast<long unsigned int>(channel->getUserLimit())) {
            client->send_buffer += ": 471 " + channel_name + " :Cannot join channel (+l)\r\n";
            client->has_to_write = true;
            return;
        }
        if (channel->isInviteOnly() && channel->isInvited(client))
            channel->removeInviteClients(client);
        channel->addMember(client);
        client->joined_channels[channel_name] = channel;
    }

    std::string join_msg = ":" + client->get_id() + " JOIN :" + channel_name + "\r\n";
    broadcastToChannel(channel, join_msg, NULL);
}
