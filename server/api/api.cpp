#include "api.h"

// get a username given a socket
string Api::getUserName(int sockfd)
{
    auto search = socketsToUsers.find(sockfd);

    if (search != socketsToUsers.end())
    {
        return search->second;
    }
    return "";
}

// get a single socket given a username
int Api::getSocket(string userName)
{
    auto search = usersToSockets.find(userName);

    if (search != usersToSockets.end())
    {
        return search->second;
    }

    return -1;
}

// lists all users for the socket that asked for it
// WHO command
void Api::listAllUsernames(int sockfd)
{
    map<string, int>::iterator it;
    char msg[] = "The users on this server are: ";
    string userName;
    int n;

    sendMessage(0, sockfd, msg);
    for (it = usersToSockets.begin(); it != usersToSockets.end(); it++)
    {
        if (it->second != sockfd)
        {
            userName = it->first;
            n = userName.length();

            char buffer[n + 1];

            // copy the string to an array so it can be sent
            strcpy(buffer, userName.c_str());

            sendMessage(0, sockfd, buffer);
        }
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

void Api::giveServerNewId()
{
    string str = "fortune -s";
    string fortuneQuote = exec("fortune -s");
    int randomIdNumber = rand() % 1000000 + 1;
    auto start = chrono::system_clock::now();
    time_t end_time = chrono::system_clock::to_time_t(start);
    idOfServer = to_string(randomIdNumber) + " " + fortuneQuote + " group23 " + "at " + ctime(&end_time);
}

// send a message to a single client
int Api::sendMessage(int from, int sockDest, char buffer[])
{
    string userName = getUserName(from);
    userName.erase(remove(userName.begin(), userName.end(), '\n'), userName.end());

    string concat = userName + ": " + arrayToString(buffer);

    int n = concat.length();

    char finalMessage[n + 1];

    // copy the string to an array so it can be sent
    strcpy(finalMessage, concat.c_str());

    return write(sockDest, finalMessage, strlen(finalMessage));
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

string Api::getServerId()
{
    return idOfServer;
}

string Api::exec(const char *cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
        throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get()))
    {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
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

void Api::leaveServer(int sockfd)
{
    // wut
    close(sockfd);
}

string Api::arrayToString(char arr[])
{
    string msg(arr);
    msg.erase(remove(msg.begin(), msg.end(), '\n'), msg.end());
    return msg;
}