#include "../../incl/Server.hpp"

static bool     is_special_char(char c)
{
    return (c == '[' || c == ']' || c == '\\'
            || c == '`' || c == '^' || c == '{' || c == '|' || c == '}');
}

static bool verif_synthaxe(const std::string& nickname)
{
    if (nickname.empty() || nickname.length() > 9)
        return (false);
    if (!std::isalpha(static_cast<unsigned char>(nickname[0])) && !is_special_char(nickname[0]))
        return(false);
    for (size_t i = 1; i < nickname.size(); ++i)
    {
        unsigned char c = static_cast<unsigned char>(nickname[i]);
        if (!std::isalnum(c) && !is_special_char(c) && c != '-')
            return (false);
    }
    return (true);
}

void Server::handle_nick(Client* client, const std::vector<std::string>& args)
{
	if (!client->is_authenticated) {
        client->send_buffer += ": 451 :You have not registered\r\n";
        client->has_to_write = true;
        return;
    }
    if (args.empty()) {
        client->send_buffer += ": 431 :No nickname given\r\n";
        client->has_to_write = true;
        return;
    }

    std::string new_nick = args[0];
    if (!verif_synthaxe(new_nick)) {
        client->send_buffer += ": 432 " + new_nick + " :Erroneous nickname\r\n";
        client->has_to_write = true;
        return;
    }

    if (clients_str_map.find(new_nick) != clients_str_map.end()) {
        client->send_buffer += ": 433 " + new_nick + " :Nickname is already in use\r\n";
        client->has_to_write = true;
        return;
    }

    if (!client->nickname.empty()) {
        client->send_buffer += "change nickname to " + new_nick + "!\r\n";
        client->has_to_write = true;
        std::string msg = client->get_id() + " has change nickname to " + new_nick + "!\r\n";
        for (std::map<std::string, Channel*>::iterator it = channels_map.begin(); it != channels_map.end(); it++)
        {
            if (it->second->isMember(client))
                broadcastToChannel(it->second , msg, client);
        }
        clients_str_map.erase(client->nickname);
    }
    
    client->nickname = new_nick;
    clients_str_map[new_nick] = client;
    check_registration(client);
}
