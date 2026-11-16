#include "../../incl/Server.hpp"

void Server::handle_topic(Client* client, const std::vector<std::string>& args)
{
    if (!client->is_registered) {
        client->send_buffer += ": 451 :You have not registered\r\n";
        client->has_to_write = true;
        return;
    }
    if (args.empty()) {
        client->send_buffer += ": 461 TOPIC :Not enough parameters\r\n";
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

    if (args.size() == 1) {
        if (ch->getTopic().empty())
            client->send_buffer += ": 331 " + client->nickname + " " + channel_name + " :No topic is set\r\n";
        else
            client->send_buffer += ": 332 " + client->nickname + " " + channel_name + " :" + ch->getTopic() + "\r\n";
        client->has_to_write = true;
        return;
    }
    
    if (ch->isTopicProtected() && !ch->isOperator(client)) {
        client->send_buffer += ": 482 " + channel_name + " :You're not channel operator\r\n";
        client->has_to_write = true;
        return;
    }

    std::string new_topic = args[1];
    ch->setTopic(new_topic);

    std::string topic_msg = ":" + client->get_id() + " TOPIC " + channel_name + " :" + new_topic + "\r\n";
    broadcastToChannel(ch, topic_msg, NULL);
}