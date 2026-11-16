#include "../../incl/Server.hpp"

void Server::handle_user(Client* client, const std::vector<std::string>& args)
{
	if (!client->is_authenticated) {
        client->send_buffer += ": 451 :You have not registered\r\n";
        client->has_to_write = true;
        return;
    }
    if (!client->username.empty()) {
        client->send_buffer += ": 462 :You may not reregister\r\n";
        client->has_to_write = true;
        return;
    }
    if (args.size() < 4) {
        client->send_buffer += ": 461 USER :Not enough parameters\r\n";
        client->has_to_write = true;
        return;
    }
    client->username = args[0];
    check_registration(client);
}