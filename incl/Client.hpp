
#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <unistd.h>
# include <iostream>
#include "Channel.hpp"
#include <map>

class Channel;

class Client
{	
public:	
	std::string	password;
	std::string	username;
	std::string	nickname;
	std::string	hostname;
	int			socket;
	int			has_to_write;
	int			is_registered;
	bool		is_authenticated;
	std::string	recv_buffer;
	std::string	send_buffer;

	std::map<std::string, Channel*> joined_channels;
	
	Client(void);
	~Client(void);

	std::string get_id();
};

#endif

