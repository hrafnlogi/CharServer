#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <iostream>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}
void *receieveMsg(void *sock)
{
    int sockfd = *((int *)sock);

    char buffer[256];
    int len;
    while ((len = recv(sockfd, buffer, 256, 0)) > 0)
    {
        buffer[len] = '\0';
        std::cout << std::string(buffer) << std::endl;
        memset(buffer, '\0', sizeof(buffer));
    }
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr; // Socket address structure
    struct hostent *server;
    pthread_t receiveThread; // thread for listening to incoming messages from server

    char buffer[256];
    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    portno = atoi(argv[2]); // Read Port No from command line

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // Open Socket

    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(argv[1]); // Get host from IP

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET; // This is always set to AF_INET

    // Host address is stored in network byte order
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);

    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    // create thread that will listen for incoming messages
    pthread_create(&receiveThread, NULL, receieveMsg, &sockfd);

    // Write to socket
    while (1)
    {
        //printf("> ");
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        n = write(sockfd, buffer, strlen(buffer));

        if (n < 0)
            error("ERROR writing to socket");
    }

    pthread_join(receiveThread, NULL);
    close(sockfd);
    return 0;
}
