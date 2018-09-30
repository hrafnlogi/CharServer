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

// lists all users for the socket that asked for it
// WHO command
void Api::listAllUsernames(int sockfd)
{
    map<string, int>::iterator it;
    string userName;
    int n;

    for (it = usersToSockets.begin(); it != usersToSockets.end(); it++)
    {
        userName = it->first;
        n = userName.length();

        char buffer[n + 1];

        // copy the string to an array so it can be sent
        strcpy(buffer, userName.c_str());

        sendMessage(0, sockfd, buffer);
    }
}

// returns all userNames that are connected
vector<string> Api::getAllUserNames()
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
int Api::sendMessage(int from, int sockDest, char buffer[])
{
    return write(sockDest, buffer, strlen(buffer));
}

// send a message to all clients
int Api::sendMessageToAll(int from, char buffer[])
{
    map<string, int>::iterator it;
    for (it = usersToSockets.begin(); it != usersToSockets.end(); it++)
    {
        int receiverSockfd = it->second;
        // don't want to send the to the client who sent the message
        if (receiverSockfd != from)
        {
            sendMessage(from, receiverSockfd, buffer);
        }
    }
}

// read message from client
string Api::receiveMessage(int sockfd)
{
    int nBytes, MAXMSG = 512;
    char buffer[MAXMSG];
    bzero(buffer, MAXMSG);

    nBytes = read(sockfd, buffer, MAXMSG);

    if (nBytes < 0)
    {
        perror("read");
        exit(EXIT_FAILURE);
    }
    else if (nBytes == 0)
    {
        // end of file
        // return -1
    }
    else
    {
        // return message
        return string(buffer);
    }
}

// checks if the sequence of the ports are correct
bool Api::validPorts(vector<int> ports)
{
    int secondLastElement = ports.rbegin()[1];
    int lastElement = ports.rbegin()[0];
    if (secondLastElement != 23001)
    {
        return false;
    }

    if (lastElement != 23002)
    {
        return false;
    }

    return true;
}
