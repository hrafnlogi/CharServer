#ifndef API_H
#define API_H

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
#include <vector>

using namespace std;

class Api
{
private:
  // map client usernames to their sockets
  map<string, int> usersToSockets;
  // map sockets to usernames
  map<int, string> socketsToUsers;

public:
  string getUserName(int sockfd);
  int getSocket(string userName);
  void listAllUsernames(int sockfd);
  vector<string> getAllUserNames();
  vector<int> getAllSockets();

  void addUserToList(string userName, int sockfd);

  int sendMessage(int from, int sockDest, char buffer[]);
  int sendMessageToAll(int from, char buffer[]);

  string receiveMessage(int sockfd);
};

#endif