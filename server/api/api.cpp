#include "api.h"

// get a username given a socket
string Api::getUserName(int sockfd)
{
    auto search = socketsToUsers.find(sockfd);

    if (search != socketsToUsers.end())
    {
        return search->second;
    }
}

// get a single socket given a username
int Api::getSocket(string userName)
{
    auto search = usersToSockets.find(userName);

    if (search != usersToSockets.end())
    {
        return search->second;
    }
}

// return all usernames
// WHO command
vector<string> Api::getAllUsernames()
{
}

// return all sockets that are connected
vector<int> Api::getAllSockets()
{
}

// link a username to a socket
void Api::addUserToList(string userName, int sockfd)
{
    usersToSockets[userName] = sockfd;
    socketsToUsers[sockfd] = userName;
}

// send a message to a single client
int Api::sendMessage(int sock, char buffer[])
{
    return write(sock, buffer, strlen(buffer));
}

// send a message to all clients
int Api::sendMessage(vector<int> sockets, char buffer[])
{
}
