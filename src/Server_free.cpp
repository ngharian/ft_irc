
#include "../incl/Server.hpp"

void	Server::close_channel(Channel* ch)
{
	std::string channel_name = ch->getName();
	std::vector<Client*>	channel_members = ch->getMembers();
	std::vector<Client*>::iterator it = channel_members.begin();

	if (channels_map.find(channel_name) != channels_map.end())
		channels_map.erase(channel_name);
	while (it != channel_members.end())
	{
		if ((*it)->joined_channels.find(channel_name) != (*it)->joined_channels.end())
			(*it)->joined_channels.erase(channel_name);
		it++;
	}
	delete ch;
}

void	Server::check_channel_to_del(Client *client)
{
	std::map<std::string, Channel*>::iterator it = client->joined_channels.begin();
	std::vector<Client*>					  channel_members;
	
	while(it != client->joined_channels.end())
	{
		channel_members = it->second->getMembers();
		if (channel_members.size() > 0)
			it->second->removeMember(client);
		else
		{
			close_channel(it->second);
		}
		++it;
	}
}

void	Server::remove_client_ref(Client *client, int fd_client, std::string nick)
{
	std::map<std::string, Channel*>::iterator it = channels_map.begin();
	clients_fd_map.erase(fd_client);
	if (nick.empty() == 0)
		clients_str_map.erase(nick);
	while (it != channels_map.end())
	{
		if(it->second->isMember(client))
			it->second->removeMember(client);
		++it;
	}
}

void	Server::free_client(int fd_client)
{
	std::string			nick;
	Client*				client = clients_fd_map[fd_client];

	check_channel_to_del(client);
	nick = clients_fd_map[fd_client]->nickname;
	remove_client_ref(client, fd_client, nick);
	delete client;
}
