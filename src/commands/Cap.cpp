
#include "../../incl/Server.hpp"

void Server::handle_cap(Client* client, const std::vector<std::string>& args)
{
	if (args.empty())
		return;
	if (args[0] == "LS") {
	    client->send_buffer += ": CAP * LS :\r\n";
        client->has_to_write = true;
    }
}