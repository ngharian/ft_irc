#include "../../incl/Server.hpp"

void Server::handle_invite(Client* client, const std::vector<std::string>& args)
{
    if (!client->is_registered) {
        client->send_buffer += ": 451 :You have not registered\r\n";
        client->has_to_write = true;
        return;
    }
    if (args.size() < 2) {
        client->send_buffer += ": 461 INVITE :Not enough parameters\r\n";
        client->has_to_write = true;
        return;
    }

    std::string target_nick = args[0];
    std::string channel_name = args[1];

    std::map<std::string, Channel*>::iterator it_ch = channels_map.find(channel_name);
    if (it_ch == channels_map.end()) {
        client->send_buffer += ": 403 " + channel_name + " :No such channel\r\n";
        client->has_to_write = true;
        return;
    }

    Channel* ch = it_ch->second;

    if (ch->isInviteOnly() && !ch->isOperator(client)) {
        client->send_buffer += ": 482 " + channel_name + " :You're not channel operator\r\n";
        client->has_to_write = true;
        return;
    }
    std::map<std::string, Client*>::iterator it_target = clients_str_map.find(target_nick);
    if (it_target == clients_str_map.end()) {
        client->send_buffer += ": 401 " + target_nick + " :No such nick/channel\r\n";
        client->has_to_write = true;
        return;
    }
    Client* target_client = it_target->second;
    if (ch->isMember(target_client)) {
        client->send_buffer += ": 443 " + target_nick + " " + channel_name + " :is already on channel\r\n";
        client->has_to_write = true;
        return;
    }
    ch->addInvited(target_client);
    std::string invite_msg_target = ":" + client->get_id() + " INVITE " + target_nick + " :" + channel_name + "\r\n";
    target_client->send_buffer += invite_msg_target;
    target_client->has_to_write = true;

    client->send_buffer += ": 341 " + client->nickname + " " + target_nick + " " + channel_name + "\r\n";
    client->has_to_write = true;
}