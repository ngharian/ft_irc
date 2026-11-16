#include "../../incl/Server.hpp"

void Server::handle_ping(Client* client, const std::vector<std::string>& args)
{
    if (args.empty()) {
        client->send_buffer += ": 409 :No origin specified\r\n";
        client->has_to_write = true;
        return;
    }

    client->send_buffer += "PONG :" + args[0] + "\r\n";
    client->has_to_write = true;
}