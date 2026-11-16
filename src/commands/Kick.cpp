#include "../../incl/Server.hpp"

void Server::handle_kick(Client* client, const std::vector<std::string>& args)
{
    if (!client->is_registered) {
        client->send_buffer += ": 451 :You have not registered\r\n";
        client->has_to_write = true;
        return;
    }
    if (args.size() < 2) {
        client->send_buffer += ": 461 KICK :Not enough parameters\r\n";
        client->has_to_write = true;
        return;
    }

    std::string channel_name = args[0];
    std::string target_nick  = args[1];

    std::map<std::string, Channel*>::iterator it = channels_map.find(channel_name);
    if (it == channels_map.end()) {
        client->send_buffer += ": 403 " + channel_name + " :No such channel\r\n";
        client->has_to_write = true;
        return;
    }

    Channel* ch = it->second;
    if (!ch->isMember(client) || !ch->isOperator(client)) {
        client->send_buffer += ": 482 " + channel_name + " :You're not channel operator\r\n";
        client->has_to_write = true;
        return;
    }

    std::map<std::string, Client*>::iterator itc = clients_str_map.find(target_nick);
    if (itc == clients_str_map.end()) {
        client->send_buffer += ": 401 " + target_nick + " :No such nick/channel\r\n";
        client->has_to_write = true;
        return;
    }
    Client* target = itc->second;
    if (!ch->isMember(target)) {
        client->send_buffer += ": 441 " + target_nick + " " + channel_name + " :They aren't on that channel\r\n";
        client->has_to_write = true;
        return;
    }

    std::string comment = (args.size() > 2 ? args[2] : "");
    std::string kick_msg = ":" + client->get_id() + " KICK " + channel_name + " " + target_nick;
    if (!comment.empty()) 
        kick_msg += " :" + comment;
    kick_msg += "\r\n";

    broadcastToChannel(ch, kick_msg, NULL);
    ch->removeMember(target);
    target->joined_channels.erase(channel_name);
}
