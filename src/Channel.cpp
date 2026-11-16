
#include "../incl/Channel.hpp"
#include "../incl/Client.hpp"

Channel::Channel(const std::string& name) : name(name), topic(""), user_limit(0), is_invite_only(false), is_topic_protected(false) {
}

Channel::~Channel(void) {
}

const std::string& Channel::getName() const {
    return name;
}

const std::string& Channel::getTopic() const {
    return topic;
}

const std::vector<Client*>& Channel::getMembers() const {
    return members;
}

const std::vector<Client*>& Channel::getOperators() const {
    return operators;
}

const std::string& Channel::getPassword() const {
    return password;
}

int Channel::getUserLimit() const {
    return user_limit;
}

bool Channel::isInviteOnly() const {
    return is_invite_only;
}

bool Channel::isTopicProtected() const {
    return is_topic_protected;
}

bool Channel::isMember(Client* client) const {
    for (std::vector<Client*>::const_iterator it = members.begin(); it != members.end(); ++it) {
        if (*it == client) {
            return true;
        }
    }
    return false;
}

bool Channel::isOperator(Client* client) const {
    for (std::vector<Client*>::const_iterator it = operators.begin(); it != operators.end(); ++it) {
        if (*it == client) {
            return true;
        }
    }
    return false;
}

void Channel::addInvited(Client* client) {
    for (size_t i = 0; i < invited_clients.size(); ++i) {
        if (invited_clients[i] == client) {
            return;
        }
    }
    invited_clients.push_back(client);
}

bool Channel::isInvited(Client* client) const {
    for (size_t i = 0; i < invited_clients.size(); ++i) {
        if (invited_clients[i] == client) {
            return true;
        }
    }
    return false;
}

void Channel::setTopic(const std::string& new_topic) {
    topic = new_topic;
}

void Channel::setPassword(const std::string& new_password) {
    password = new_password;
}

void Channel::setInviteOnly(bool state) {
    is_invite_only = state;
}

void Channel::setTopicProtected(bool state) {
    is_topic_protected = state;
}

void Channel::setUserLimit(int limit) {
    user_limit = (limit < 0) ? 0 : limit;
}


void Channel::addMember(Client* client) {
    if (!isMember(client)) {
        members.push_back(client);
    }
}

void Channel::removeMember(Client* client) {
    for (std::vector<Client*>::iterator it = members.begin(); it != members.end(); ++it) {
        if (*it == client) {
            members.erase(it);
            break;
        }
    }
    removeOperator(client);
    removeInviteClients(client);
}

void Channel::addOperator(Client* client) {
    if (isMember(client) && !isOperator(client)) {
        operators.push_back(client);
    }
}

void Channel::removeOperator(Client* client) {
    for (std::vector<Client*>::iterator it = operators.begin(); it != operators.end(); ++it) {
        if (*it == client) {
            operators.erase(it);
	        std::vector<Client*>::iterator new_op = members.begin();
	        if (operators.size() == 0)
                addOperator(*new_op);
	        break ;
        }
    }
}

void Channel::removeInviteClients(Client* client) {
    for (std::vector<Client*>::iterator it = invited_clients.begin(); it != invited_clients.end(); ++it) {
        if (*it == client) {
            invited_clients.erase(it);
            break;
        }
    }
}
