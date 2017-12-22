// HEADER FILES -----------------------------------------------
#ifndef HEAD
#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fstream>
#include <string.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <vector>
#include <sstream>
#include <errno.h>
#include <fcntl.h>
#include <ctime>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
// #include <gcrypt.h>
#include <stdio.h>
#include "md5.h"
#include <bits/stdc++.h>
using namespace std;

// DATA STRUCTURES --------------------------------------------

struct distributedHashTable {
	unordered_map<string, string> DHT;
};

struct routingTable {
	vector< pair<string,int> > neighbour_set;
	vector< pair<string,int> > leaf_set;
	map<string,vector<pair<string,int> > >routing_set;
};

struct connectionInfo {
	string IP;
	int port;
};

static set<pair<string, int> > uniqueNSet;
extern connectionInfo connection;
extern string nodeID;
extern routingTable node;
extern distributedHashTable dht;

// FUNCTION PROTOTYPES-----------------------------------------
void *startServer(void *connection);
void *handleJoinThread(void *remaining);
void handleJoin(string remaining);
void connectToNetwork(string ip, int port, int toDo);
void printHashTable();
void printNeighbourSet();
// string md5(string a); 

extern string message;

enum ToDo {NOTHING=1, LEAF_SET=2, ROUTING_TABLE=3, NEIGHBOUR_SET=4, QUIT=5, DEATH=6, SHUTDOWN = 7};
enum ToWhere {SELF = 8, LSP = 9, RTP = 10, NSP = 11, SEND = 12};
enum FromWhere {SELFG = 16, LSG = 17, RTG = 18, NSG = 19};

#endif
