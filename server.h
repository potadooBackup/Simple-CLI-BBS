#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include "database.h"
#include "user.h"
#include "command2Enum.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include <unistd.h>
#include <regex>

#include <fstream>


#define MAXLINE 200
#define LISTENQ 10
#define MAXCLINUM 10

#define FILEDEBUG

using namespace std;

struct Header{
    unsigned char flag;
    unsigned char version;
} __attribute__((packed));

struct Data{
    unsigned short len;
    unsigned char data[0];
} __attribute__((packed));

class Server{
	private:
		int	  	    listenfd;
		int			udpfd;
		sockaddr_in	servaddr;
		Database*	database;
		int		    shmid;
	public:
		//Constructor
		Server(Database*, int);
		Server();
		~Server();
		
		//Socket Related
		int     setupTCP(int);		//socket setup, bind, listen
		int     setupUDP(int);
		int     acceptConnection();
		void    closeParentConnection();
		
		//BBS Server Action
		void    sendCmlPrompt(int);
		void    sendWelcomeMsg(int);
		User*   recvCommand(User*, int, int&, bool&);
		User*   doCommand(User*, vector<string>&, int, bool&);
		User*	recvChatPackage();
		string	chatFilter(string);
		void	broadcast(sockaddr_in, const char*, const char*, int);

		//utility
		string parseMsg(vector<string>&);
		bool   isOpsLegal(Command, vector<string>&);
		string starGenerator(string);
		string base64_encode(string);
		string base64_decode(string);

		#ifdef FILEDEBUG
		ofstream fout;
		#endif
};


#endif
