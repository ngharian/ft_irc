#include "../../incl/Server.hpp"

void Server::handle_quit(Client* client, const std::vector<std::string>& args)
{
    std::string reason = (args.empty() ? "Client Quit" : args[0]);
    std::string quit_msg = ":" + (client->nickname.empty() ? std::string("*") : client->get_id())
                         + " QUIT :" + reason + "\r\n";

    for (std::map<std::string, Channel*>::iterator it = channels_map.begin(); it != channels_map.end(); ++it) {
        Channel* ch = it->second;
        if (ch->isMember(client)) {
            broadcastToChannel(ch, quit_msg, client);
            ch->removeMember(client);
        }
    }
    close_connection(client->socket);
}
