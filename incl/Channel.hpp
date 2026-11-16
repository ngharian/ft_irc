
#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
#include <vector>
#include "Client.hpp"

class Client;

class Channel
{
private:
	std::string name;
	std::string topic;
	std::string password;
	std::vector<Client *> members;
	std::vector<Client *> operators;
	int user_limit;
	bool is_invite_only;
	bool is_topic_protected;
	std::vector<Client *> invited_clients;
public:
	Channel(const std::string &name);
	~Channel(void);

	 // Getters
    const std::string& getName() const;
    const std::string& getTopic() const;
	const std::vector<Client*>& getMembers() const;
	const std::vector<Client*>& getOperators() const;
	const std::string& getPassword() const;
	int getUserLimit() const;

    // Setters
    void setTopic(const std::string& new_topic);
    void setPassword(const std::string& new_password);
	
	// Méthodes pour gérer les membres et opérateurs
    void addMember(Client* client);
    void removeMember(Client* client);
    void addOperator(Client* client);
    void removeOperator(Client* client);
    void removeInviteClients(Client* client);

    // Méthodes pour les modes
    void setInviteOnly(bool state);
    void setTopicProtected(bool state);
    void setUserLimit(int limit);

    bool isMember(Client* client) const;
    bool isOperator(Client* client) const;
    bool isInviteOnly() const;
    bool isTopicProtected() const;
    bool isInvited(Client* client) const;
    void addInvited(Client *client);
};

#endif

