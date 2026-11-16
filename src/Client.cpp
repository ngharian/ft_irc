
#include "../incl/Client.hpp"

Client::Client(void)
{
	username = "";
	nickname = "";
	password = "";
	socket = -1;
	has_to_write = 0;
	is_registered = 0;
	is_authenticated = 0;
}

Client::~Client(void)
{
	close(socket);
}

std::string	Client::get_id(void)
{
	std::string	id = nickname;
	
	id += "!~" + username + "@" + hostname;
	return (id);
}
