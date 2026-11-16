#include "../../incl/Server.hpp"
#include <cstdlib>

static bool    isnumber(std::string str)
{
    int i = -1;
    while(str[++i])
    {
        if (!isdigit(str[i]))
            return(0);
    }
    return(1);
}

bool display_mode(const std::vector<std::string>& args, Channel *ch, Client *client)
{
    std::string target = args[0];

    if (args.size() == 1) {
        std::string modes = "+";
        if (ch->isInviteOnly())
            modes += "i";
        if (ch->isTopicProtected())
            modes += "t";
        if (!ch->getPassword().empty())
            modes += "k";
        if (ch->getUserLimit() > 0)
            modes += "l";
        client->send_buffer += ": 324 " + client->nickname + " " + target + " " + modes + "\r\n";
        client->has_to_write = true;
        return 1;
    }

    if (!ch->isOperator(client)) {
        client->send_buffer += ": 482 " + target + " :You're not channel operator\r\n";
        client->has_to_write = true;
        return 1;
    }
    return 0;

}

void handleInvite(Server *server, bool add_mode, std::string mode_msg, Channel *ch)
{
    std::string mm;

    ch->setInviteOnly(add_mode);
    if (add_mode)
        mm = "+i";
    else
        mm = "-i";
    mode_msg += " " + mm + "\r\n";
    server->broadcastToChannel(ch, mode_msg, NULL);
}

void handleTopic(Server *server, bool add_mode, std::string mode_msg, Channel *ch)
{
    std::string mm;

    ch->setTopicProtected(add_mode);
    if (add_mode)
        mm = "+t";
    else
        mm = "-t";
    mode_msg += " " + mm + "\r\n";
    server->broadcastToChannel(ch, mode_msg, NULL);
}

bool handleKey(Server *server, bool add_mode, size_t arg_idx, const std::vector<std::string>& args, Client *client, Channel *ch, std::string mode_msg)
{
    if (add_mode) {
        if (arg_idx >= args.size()) {
            client->send_buffer += ": 461 MODE :Not enough parameters\r\n";
            client->has_to_write = true;
            return 1;
        }
        ch->setPassword(args[arg_idx]);
        mode_msg += " +k " + args[arg_idx] + "\r\n";
        arg_idx++;
    }
    else
    {
        ch->setPassword("");
        mode_msg += " -k\r\n";
    }
    server->broadcastToChannel(ch, mode_msg, NULL);
    return 0;
}

bool handleOperators(Server *server, bool add_mode, size_t arg_idx, const std::vector<std::string>& args, Client *client, Channel *ch, 
    std::map<std::string, Client*> clients_str_map, std::string mode_msg)
{
    std::string target = args[0];
    std::string mm;

    if (arg_idx >= args.size()) {
        client->send_buffer += ": 461 MODE :Not enough parameters\r\n";
        client->has_to_write = true;
        return 1;
    }
    std::map<std::string, Client*>::iterator it_o = clients_str_map.find(args[arg_idx]);
    if (it_o == clients_str_map.end()) {
        client->send_buffer += ": 401 " + args[arg_idx] + " :No such nick/channel\r\n";
        client->has_to_write = true;
        arg_idx++;
        return 1;
    }
    Client* target_client = it_o->second;
    if (!ch->isMember(target_client)) {
        client->send_buffer += ": 441 " + args[arg_idx] + " " + target + " :They aren't on that channel\r\n";
        client->has_to_write = true;
        arg_idx++;
        return 1;
    }
    if (add_mode)
        ch->addOperator(target_client);
    else
        ch->removeOperator(target_client);
    if (add_mode)
        mm = "+o ";
    else
        mm = "-o ";
    mode_msg += " " + mm + args[arg_idx] + "\r\n";
    arg_idx++;
    server->broadcastToChannel(ch, mode_msg, NULL);
    return 0;
}

bool handleLimit(Server *server, bool add_mode, size_t arg_idx, const std::vector<std::string>& args, Client *client, char m, Channel *ch, std::string mode_msg)
{
    if (add_mode) {
        if (arg_idx >= args.size()) {
            client->send_buffer += ": 461 MODE :Not enough parameters\r\n";
            client->has_to_write = true;
            return 1;
        }
        int lim = std::atoi(args[arg_idx].c_str());
        if (lim <= 0 || !isnumber(args[arg_idx])) {
            client->send_buffer += ": 472 " + std::string(1, m) + " :is unknown mode char to me\r\n";
            client->has_to_write = true;
            arg_idx++;
            return 1;
        }
        ch->setUserLimit(lim);
        mode_msg += " +l " + args[arg_idx] + "\r\n";
        arg_idx++;
    }
    else
    {
        ch->setUserLimit(0);
        mode_msg += " -l\r\n";
    }
    server->broadcastToChannel(ch, mode_msg, NULL);
    return 0;
}

void Server::handle_mode(Client* client, const std::vector<std::string>& args)
{
    if (!client->is_registered) {
        client->send_buffer += ": 451 :You have not registered\r\n";
        client->has_to_write = true;
        return;
    }
    if (args.empty()) {
        client->send_buffer += ": 461 MODE :Not enough parameters\r\n";
        client->has_to_write = true;
        return;
    }

    std::string target = args[0];
    if (target.empty() || target[0] != '#') {
        client->send_buffer += ": 472 " + target + " :is unknown mode char to me\r\n";
        client->has_to_write = true;
        return;
    }

    std::map<std::string, Channel*>::iterator it = channels_map.find(target);
    if (it == channels_map.end()) {
        client->send_buffer += ": 403 " + target + " :No such channel\r\n";
        client->has_to_write = true;
        return;
    }
    Channel* ch = it->second;
    
    if (display_mode(args, ch, client))
        return ;
    std::string mode_str = args[1];
    bool add_mode = (mode_str[0] == '+');
    size_t arg_idx = 2;

    for (size_t i = 1; i < mode_str.size(); ++i) {
        char m = mode_str[i];
        std::string mode_msg = ":" + client->get_id() + " MODE " + target;
        std::string mode_char_str(1, m);
        
        if (m == 'i')
            handleInvite(this , add_mode, mode_msg, ch);
        else if (m == 't')
            handleTopic(this , add_mode, mode_msg, ch);
        else if (m == 'k')
        {
            if (handleKey(this , add_mode, arg_idx, args, client, ch, mode_msg))
                continue ;
        }
        else if (m == 'o')
        {
            if (handleOperators(this , add_mode, arg_idx, args, client, ch, clients_str_map, mode_msg))
                continue ;
        }
        else if (m == 'l')
        {
           if (handleLimit(this , add_mode, arg_idx, args, client, m, ch, mode_msg))
                continue ;
        }
        else {
            client->send_buffer += ": 472 " + mode_char_str + " :is unknown mode char to me\r\n";
            client->has_to_write = true;
        }
    }
}
