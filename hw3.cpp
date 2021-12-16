#include <iostream>
#include <unistd.h>
#include <sys/ipc.h>
#include <csignal>
#include <map>
#include <sys/select.h>
#include <sys/time.h>
#include "user.h"
#include "server.h"
#include "database.h"


#define DEBUG 0
Server  *server;
using namespace std;

void ctrlcHandler(int sig){
	if(server!=NULL) delete server;
	exit(0);
}

int main(int argc, char* argv[]){
	signal(SIGINT, ctrlcHandler);

	if(argc != 2){
		cout<<"Server usage: ./hw3 [port Number]"<<endl;
		return 0;
	}
	server = new Server();
	int listenfd = server->setupTCP(atoi(argv[1]));
	int udpfd = server->setupUDP(atoi(argv[1]));

	fd_set my_set;   //all fd i'm using
	fd_set wk_set;   //working set

	FD_ZERO(&my_set);
	FD_SET(listenfd, &my_set);
	FD_SET(udpfd, &my_set);
	int max_fd = max(listenfd, udpfd);
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 20;
	map<int,User*> userHash;
	map<User*,int> userHashInv;

	while(1){
		memcpy(&wk_set,&my_set,sizeof(my_set));
		int rc = select(max_fd+1, &wk_set, NULL, NULL, &timeout);     //recv

		int tmp_max = max_fd;
		for(int i = 0; i < max_fd+1; i++){
			if(FD_ISSET(i, &wk_set)){
				if(i == listenfd){
					int connfd = server->acceptConnection();
					FD_SET(connfd, &my_set);
					userHash[connfd] = NULL;
					server->sendWelcomeMsg(connfd);
					server->sendCmlPrompt(connfd);
					if(connfd > max_fd) tmp_max = connfd;
				}
				else if(i == udpfd){
					User* user = server->recvChatPackage();
					if(user != NULL){
						vector<string> tmp;
						tmp.push_back("logout");
						bool noUse;
						server->doCommand(user, tmp, userHashInv[user], noUse);
						server->sendCmlPrompt(userHashInv[user]);
					}
					//receive chat
						//1. parse the package
						//2. deal with word filter
						//3. broadcast
						//4. sendcmlprompt 
				}
				else{
					int recvlen = 0;
					#if DEBUG
					cout<<"I'm fd: "<<i<<endl;
					if(userHash[i] != NULL) cout<<"As user :"<<userHash[i]->getName()<<endl; else cout<<"No user logined!"<<endl;
					#endif
					bool isExisted = false;
					userHash[i] = server->recvCommand(userHash[i], i, recvlen, isExisted);
					userHashInv[userHash[i]] = i;
					if(isExisted || recvlen == 0){
						cout<<"I'm fd: "<<i<<", and i'm out!\n";
						FD_CLR(i, &my_set);
						if(i == max_fd){
							for(int j=0;j<max_fd;j++){
								if(FD_ISSET(j, &my_set)) tmp_max = j;
							}
						}
					}
					else server->sendCmlPrompt(i);
				}
			}
		}
		max_fd = tmp_max;
	}

	return 0;
}
