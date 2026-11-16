#include "../../incl/Server.hpp"


void Server::handle_privmsg(Client* client, const std::vector<std::string>& args)
{
    if (!client->is_registered) {
        client->send_buffer += ": 451 :You have not registered\r\n";
        client->has_to_write = true;
        return;
    }

    if (args.size() < 2) {
        client->send_buffer += ": 461 PRIVMSG :Not enough parameters\r\n";
        client->has_to_write = true;
        return;
    }

    std::string recipient = args[0];
    std::string message;
    for (std::vector<std::string>::const_iterator it = args.begin() + 1; it != args.end(); it++)
    {
        message += *it;
	message += " ";
    }

    if (recipient[0] == '#') {
        std::map<std::string, Channel*>::iterator it = channels_map.find(recipient);
        if (it == channels_map.end()) {
            client->send_buffer += ": 403 " + recipient + " :No such channel\r\n";
            client->has_to_write = true;
            return;
        }
        
        Channel* channel = it->second;

        if (client->joined_channels.find(recipient) == client->joined_channels.end()) {
            client->send_buffer += ": 442 " + recipient + " :You're not on that channel\r\n";
            client->has_to_write = true;
            return;
        }
        const std::vector<Client*>& members = channel->getMembers();
        for (std::vector<Client*>::const_iterator it_members = members.begin(); it_members != members.end(); ++it_members) {
            Client* member = *it_members;
            if (member->socket != client->socket) {
                member->send_buffer += ":" + client->get_id() + " PRIVMSG " + recipient + " :" + message + "\r\n";
                member->has_to_write = true;
            }
        }
    }
    else {
        std::map<std::string, Client*>::iterator it = clients_str_map.find(recipient);
        if (it == clients_str_map.end()) {
            client->send_buffer += ": 401 " + recipient + " :No such nick/channel\r\n";
            client->has_to_write = true;
            return;
        }
        Client* recipient_client = it->second;
        recipient_client->send_buffer += ":" + client->get_id() + " PRIVMSG " + recipient + " :" + message + "\r\n";
        recipient_client->has_to_write = true;
    }
}
