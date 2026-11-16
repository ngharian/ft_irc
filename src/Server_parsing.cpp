
#include "../incl/Server.hpp"

void	Server::check_carriage(Client* current_client)
{
	size_t pos;
	std::string request;

	while (true)
	{
		pos = current_client->recv_buffer.find("\r\n", 0);
		if (pos == std::string::npos)
			break ;
		request = current_client->recv_buffer.substr(0, pos + 2);
		current_client->recv_buffer.erase(0, pos + 2);
		if (parse_request(request, current_client)
			|| current_client->recv_buffer.length() < 1)
			break;
	}
}

bool	Server::parse_request(std::string request, Client* current_client)
{
	std::istringstream iss(request);
    std::string command;
    std::vector<std::string> args;
    std::string arg;
	std::string err_msg;

	if (request.length() > 512) {
		err_msg = ": 417 " + command + " :Input line was too long\r\n";
        current_client->send_buffer += err_msg;
		current_client->has_to_write = true;
	}

	iss >> command;

    while (iss >> arg) {
    	args.push_back(arg);
	}
	std::map<std::string, CommandHandler>::iterator it = commands_map.find(command);
    if (it != commands_map.end()) {
        (this->*(it->second))(current_client, args);
    }
	else {
        err_msg = ": 421 " + command + " :Unknown command\r\n";
        current_client->send_buffer += err_msg;
        current_client->has_to_write = true;
    }
	if (command == "QUIT")
		return(1);
	return (0);
}

void Server::check_registration(Client* client) {
    if (client->is_authenticated && !client->nickname.empty() && !client->username.empty() && !client->is_registered) {
        client->is_registered = true;
        client->send_buffer += ": 001 " + client->nickname + " :Welcome to the ft_irc network, " + client->nickname + "!\r\n";
        client->has_to_write = true;
    }
}