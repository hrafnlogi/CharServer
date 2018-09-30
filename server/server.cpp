#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <iostream>
#include <arpa/inet.h>
#include <map>

// include the api we created
#include "api/api.h"

using namespace std;

void error(const char *msg);

int analyzeMessage(int sockfd, Api *api);

int createSocket(uint16_t port);

int main(int argc, char *argv[])
{
    string userName;
    int sockfd, portNumber;
    socklen_t clientLength;
    char buffer[256];
    struct sockaddr_in clientName;
    fd_set active_fd_set, read_fd_set;
    int n, i;
    // map client usernames to their sockets
    map<string, int> usersToSockets;
    // map sockets to usernames
    map<int, string> socketsToUsers;

    // TESTING API
    Api *api = new Api();

    //

    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    portNumber = atoi(argv[1]);

    sockfd = createSocket(portNumber);

    // Listen on socket for traffic
    if (listen(sockfd, 5) < 0)
    {
        perror("select");
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&active_fd_set);
    FD_SET(sockfd, &active_fd_set);

    while (1)
    {
        // block until input arrives on one or more active sockets
        read_fd_set = active_fd_set;
        if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }

        for (i = 0; i < FD_SETSIZE; ++i)
        {
            if (FD_ISSET(i, &read_fd_set))
            {
                if (i == sockfd)
                {
                    // connection request on original socket
                    int newSockFd;
                    clientLength = sizeof(clientName);
                    newSockFd = accept(sockfd, (struct sockaddr *)&clientName, &clientLength);

                    if (newSockFd < 0)
                    {
                        perror("accept");
                        exit(EXIT_FAILURE);
                    }
                    fprintf(stderr,
                            "Server: connect from host %s, port %hd.\n",
                            inet_ntoa(clientName.sin_addr),
                            ntohs(clientName.sin_port));

                    // sets the new socket as active in the active_fd_set
                    FD_SET(newSockFd, &active_fd_set);

                    char test[] = "Choose username:";
                    api->sendMessage(sockfd, newSockFd, test);
                    cout << "listening for username" << endl;
                    // listen for username
                    userName = api->receiveMessage(newSockFd);
                    cout << "USER: " << userName;
                    // map username to its socket. <USERNAME, SOCKET>
                    api->addUserToList(userName, newSockFd);

                    // print out commands that are available here
                }
                else
                {
                    //
                    // message arriving on an already connected socket
                    if (analyzeMessage(i, api) < 0)
                    {
                        close(i);
                        FD_CLR(i, &active_fd_set);
                    }
                }
            }
        }
    }
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int analyzeMessage(int sockfd, Api *api)
{
    int n, nBytes, MAXMSG = 512;
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
        return -1;
    }
    else
    {
        // message read
        fprintf(stderr, "Server: got message: %s", buffer);
        string checkCommand = string(buffer);
        cout << "is this a command?: " << checkCommand << endl;

        if (checkCommand.find("WHO") != string::npos)
        {
            // list all usersnames
            cout << "Listing all userNames for socket: " << sockfd << endl;
            api->listAllUsernames(sockfd);
        }
        else if (checkCommand.find("MSG ALL") != string::npos)
        {
            cout << "Sending to all clients.." << endl;
            // send to the user: Message to everybody: <MSG> and read into buffer
            // and send
            //api->sendMessageToAll(sockfd, buffer);
        }
        else
        {
            cout << "No command selected, sending to everybody" << endl;
        }

        // remove when all commands are beginning to work
        api->sendMessageToAll(sockfd, buffer);

        return 0;
    }
}

// creates a socket
int createSocket(uint16_t port)
{
    int sock;
    struct sockaddr_in name;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
        error("ERROR opening socket");

    bzero((char *)&name, sizeof(name));

    name.sin_family = AF_INET;
    name.sin_addr.s_addr = INADDR_ANY;
    name.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *)&name,
             sizeof(name)) < 0)
        error("ERROR on binding");

    return sock;
}