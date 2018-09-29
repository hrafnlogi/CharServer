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

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int readClientMessage(int fileDescriptor)
{
    int MAXMSG = 512;
    char buffer[MAXMSG];
    int nBytes;
    bzero(buffer, MAXMSG);

    nBytes = read(fileDescriptor, buffer, MAXMSG);

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
        return 0;
    }
}

// creates a socket for a client to connect to given a port and returns
// the fd to be used
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

int main(int argc, char *argv[])
{
    int sockfd, portNumber;
    socklen_t clientLength;
    char buffer[256];
    struct sockaddr_in clientName;
    fd_set active_fd_set, read_fd_set;
    int n, i;

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
        // block until input arrives on one or more active sockets(clients connected)
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
                }
                else
                {
                    // message arriving on an already connected socket
                    if (readClientMessage(i) < 0)
                    {
                        close(i);
                        FD_CLR(i, &active_fd_set);
                    }
                }
            }
        }
    }
}
