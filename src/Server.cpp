
#include "../incl/Server.hpp"
#include <fcntl.h>
#include <unistd.h>

extern bool g_running;

Server::Server(int port, char *password) : port(port), password(password)
{
	fd = 0;
	init_commands();
}

Server::~Server(void)
{
	if (fd >= 3)
		close(fd);
	kill();
}

void Server::init_commands(){
	commands_map["NICK"] = &Server::handle_nick;
   	commands_map["USER"] = &Server::handle_user;
	commands_map["PASS"] = &Server::handle_pass;
	commands_map["PING"] = &Server::handle_ping;
	commands_map["JOIN"] = &Server::handle_join;
	commands_map["PRIVMSG"] = &Server::handle_privmsg;
	commands_map["CAP"] = &Server::handle_cap;
	commands_map["QUIT"] = &Server::handle_quit;
	commands_map["PART"] = &Server::handle_part;
	commands_map["KICK"] = &Server::handle_kick;
	commands_map["TOPIC"] = &Server::handle_topic;
	commands_map["MODE"] = &Server::handle_mode;
	commands_map["INVITE"] = &Server::handle_invite;
	commands_map["WHO"] = &Server::handle_who;
}

int	Server::launch(void)
{
	int	opt = 1;
	pollfd	listen_poll;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 3)
		return(std::cerr << "Error while creating the socket" << std::endl, 1);
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
		return(std::cerr << "Error while using fcntl()" << std::endl, 1);
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR /*| SO_REUSEPORT(!TEST!)*/, &opt, sizeof(opt)))
		return(std::cerr << "Error while setting socket option" << std::endl, 1);

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		return(std::cerr << "Error: bind failed" << std::endl, 1);
	if (listen(fd, 3) < 0)
		return(std::cerr << "Error while starting to listen" << std::endl, 1);
	
	listen_poll.fd = fd;
	listen_poll.events = POLLIN;
	listen_poll.revents = 0;
	poll_vector.push_back(listen_poll);
	return (0);
}

int	Server::run(void)
{
	int	activity;
	int	current_client;

	while (true)
	{
		activity = poll(poll_vector.data(), poll_vector.size(), -1);
		if (activity < 0 && g_running)
		{
			std::cerr << "Error: poll" << std::endl;
			return (1);
		}
		if(poll_vector[0].revents & POLLIN && g_running)
		{
			if (new_client())
				continue;
		}
		poll_index = 1;
		while (poll_index < poll_vector.size())
		{
			current_client = poll_vector[poll_index].fd;
			if (g_running == false)
			{
				clients_fd_map[current_client]->send_buffer.clear();
				clients_fd_map[current_client]->send_buffer += ": 001 : Closing server. Goodbye !\r\n";
				clients_fd_map[current_client]->has_to_write = 1;
			}
			if((poll_vector[poll_index].revents & POLLOUT)
				&& (clients_fd_map[current_client]->has_to_write))
			{
				if (send_to_client(current_client))
					close_connection(current_client);
			}
			if (poll_vector[poll_index].revents & POLLIN && g_running)
			{
				if (read_from_client(current_client))
					close_connection(current_client);
			}
			++poll_index;
		}
		if (g_running == false)
			break;
	}
	return (0);
}

void	Server::kill(void)
{
	std::map<std::string, Channel*>::iterator it = channels_map.begin();
	std::map<std::string, Channel*>::iterator tmp;

	for (size_t i = 1; i < poll_vector.size(); ++i)
		free_client(poll_vector[i].fd);
	while (it != channels_map.end())
	{
		tmp = it;
		++it;
		close_channel(tmp->second);
	}
	std::cout << "Server closed!" << std::endl;
}

static void append_and_flag(Client* c, const std::string& msg) {
    c->send_buffer += msg;
    c->has_to_write = 1;
}

void Server::broadcastToChannel(Channel* channel, const std::string& msg, Client* exclude) {
    const std::vector<Client*>& members = channel->getMembers();
    for (std::vector<Client*>::const_iterator it = members.begin(); it != members.end(); ++it) {
        Client* m = *it;
        if (exclude && m == exclude) continue;
        append_and_flag(m, msg);
    }
}
