#include "../../incl/Server.hpp"

void Server::handle_pass(Client* client, const std::vector<std::string>& args)
{
    if (client->is_authenticated) {
        client->send_buffer += ": 462 :You may not reregister\r\n";
        client->has_to_write = true;
        return;
    }
    if (args.empty()) {
        client->send_buffer += ": 461 PASS :Not enough parameters\r\n";
        client->has_to_write = true;
        return;
    }
    if (args[0] == this->password) {
        client->is_authenticated = true;
    } else {
        client->send_buffer += ": 464 :Password incorrect\r\n";
        client->has_to_write = true;
    }
}