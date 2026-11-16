
#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <sstream>
# include <vector>
# include <map>
# include <sys/socket.h>
# include <netinet/in.h>
# include <poll.h>
# include <unistd.h>
# include <cstdlib>
# include <cstring>
# include <csignal>
# include "./Channel.hpp"
# include "./Client.hpp"

class Server
{
private:
	const short								port;
	const char								*password;
	int										fd;
	bool									is_running;
	struct sockaddr_in						addr;
	std::vector<pollfd>						poll_vector;
	std::map<std::string, Client*> 			clients_str_map;
	std::map<int, Client*> 					clients_fd_map;
	std::map<std::string, Channel*>			channels_map;
	size_t									poll_index;
	
	typedef void (Server::*CommandHandler)(Client*, const std::vector<std::string>&);
    	std::map<std::string, CommandHandler>	commands_map;

	int				new_client(void);
	void			free_client(int client_fd);
	int				read_from_client(int client_fd);
	int				send_to_client(int client_fd);
	void			close_connection(int client_fd);
	void			check_carriage(Client* client);
	bool			parse_request(std::string request, Client* current_client);
	void			init_commands(); 
	void 			check_registration(Client* client);
	
	//commandes
	void 	handle_nick(Client* client, const std::vector<std::string>& args);
    void 	handle_user(Client* client, const std::vector<std::string>& args);
    void 	handle_ping(Client* client, const std::vector<std::string>& args);
	void	handle_pass(Client* client, const std::vector<std::string>& args);
	void	handle_join(Client* client, const std::vector<std::string>& args);
	void	handle_privmsg(Client* client, const std::vector<std::string>& args);
	void	handle_cap(Client* client, const std::vector<std::string>& args);
	void	handle_quit(Client* client, const std::vector<std::string>& args);
	void	handle_part(Client* client, const std::vector<std::string>& args);
	void	handle_kick(Client* client, const std::vector<std::string>& args);
	void	handle_topic(Client* client, const std::vector<std::string>& args);
	void	handle_mode(Client* client, const std::vector<std::string>& args);
	void	handle_invite(Client* client, const std::vector<std::string>& args);
	void	handle_who(Client* client, const std::vector<std::string>& args);
	
	void check_channel_to_del(Client* client);
	void close_channel(Channel *channel);
	void remove_client_ref(Client* client, int fd_client, std::string nick);

public:
	Server(int port, char *password);
	~Server(void);
	
	void 	broadcastToChannel(Channel* channel, const std::string& msg, Client* exclude);
	int		launch(void);
	int		run(void);
	void	kill(void);

};

# endif

