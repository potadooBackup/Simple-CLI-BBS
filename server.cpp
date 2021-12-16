#include "server.h"
using namespace std;

Server::Server(){
	this->database = new Database();
}

Server::Server(Database* _database, int _shmid):
	database(_database),
	shmid(_shmid){}

Server::~Server(){
	delete this->database;
	cout<<"Destructed!\n";
}

int Server::setupTCP(int port){
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family	 = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port	 = htons(port);

	bind(listenfd, (sockaddr*) &servaddr, sizeof(servaddr));

	listen(listenfd, LISTENQ);

	int enable = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	//setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int));

	return listenfd;
}

int Server::setupUDP(int port){
	udpfd = socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family	 = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port	 = htons(port);

	bind(udpfd, (sockaddr*) &servaddr, sizeof(servaddr));

	int enable = 1;
	setsockopt(udpfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	//setsockopt(udpfd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int));
	return udpfd;
}

int Server::acceptConnection(){
	int connfd = accept(this->listenfd, (sockaddr*) NULL, NULL);
	int enable = 1;
	setsockopt(connfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	//setsockopt(connfd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int));
	cout<<"Connection Acceptted!"<<endl;
	return connfd;
}

void Server::closeParentConnection(){
	close(this->listenfd);
}

void Server::sendWelcomeMsg(int connfd){
	const char welcomeMsg[] = "********************************\n"
			          "** Welcome to the BBS server. **\n"
			          "********************************\n";
	send(connfd, welcomeMsg, strlen(welcomeMsg), 0);
}

void Server::sendCmlPrompt(int connfd){
	char cmlPrompt[] = "% ";
	send(connfd, cmlPrompt, strlen(cmlPrompt), 0);
}

User* Server::recvCommand(User* user, int connfd, int &recvlen, bool &isExisted){
	char buff[MAXLINE];
	recvlen = recv(connfd, buff, 1024, 0);
	buff[recvlen] = '\0';

	stringstream ss(buff);
	vector<string> ops;
	string tmp = "";
	while(ss>>tmp) ops.push_back(tmp);
	if(ops.size() == 0) return user;
	
	user = this->doCommand(user, ops, connfd, isExisted);
	
	//clear up
	memset(buff,'\0',sizeof(buff));
	ss.clear();
	ss.str("");
	ops.clear();

	return user;
}

User* Server::doCommand(User* user, vector<string>& ops, int connfd, bool &isExisted){
	string msgBuff = "";
	switch(command2enum(ops[0])){
		case REGISTER:
			{
				//register <username> <password>
				if(ops.size()!=3){
					msgBuff = "Usage: register <username> <password>\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				string username = ops[1];
				string password = ops[2];
				
				if(database->checkIfNameExist(username)!=NULL){
					msgBuff = "Username is already used.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
				}

				else{
					database->addMember(username, password);	
					msgBuff = "Register successfully.\n";
					if(send(connfd, msgBuff.c_str(), msgBuff.size(), 0)<0)
						perror("Send Error: ");
				}

				break;
			}
		case LOGIN:
			{ //login <username> <password>
				if(ops.size()!=3){
					msgBuff = "Usage: login <username> <password>\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				string username = ops[1];
				string password = ops[2];
				if(user != NULL && user->getIsLogined() == true){
					msgBuff = "Please logout first.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				user = database->checkIfNameExist(username);
				if(user != NULL && database->isInBlacklish(username) == true){
					msgBuff = "We don't welcome " + username + "!\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}

				user = database->loginQuery(username, password);
				if(user != NULL){
					if(user->getIsLogined() == false ){
						user->setIsLogined(true);
						msgBuff = "Welcome, " + username + ".\n";
						send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
						break;
					}
					else{
						user = NULL;
						msgBuff = "Please logout first.\n";
						send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
						break;
					}
				}
				else{
					msgBuff = "Login failed.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				
			}
		case LOGOUT:
			{
				if(ops.size()!=1){
					msgBuff = "Usage: logout\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}

				//logout
				if(user == NULL){
					msgBuff = "Please login first.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				else{
					user->setIsLogined(false);
					msgBuff = "Bye, " + user->getName() + ".\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					user = NULL;
				}

				break;
			}
		case WHOAMI:
			{
				//whoami	
				if(user == NULL){
					msgBuff = "Please login first.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				else{
					msgBuff = user->getName() + "\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
				}
				break;
			}
		case LISTUSER:
			{
				//list-user
				msgBuff = this->database->listUser();
				cout<<msgBuff;
				send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
				break;
			}	
		case EXIT:
			{
				if(ops.size()!=1){
					msgBuff = "Usage: exit\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}

				//exit
				if(user != NULL && user->getIsLogined() == true){
					msgBuff = "Bye, " + user->getName() + ".\n";
                    send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					user->setIsLogined(false);
                    user = NULL;
				}
				shutdown(connfd,SHUT_RDWR);
				close(connfd);
				isExisted = true;
				break;
			}
		case SEND:
			{
				//send <username> <message>
				
				if(ops.size()<3){
					msgBuff = "Usage: send <username> <message>\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				
				string receiverName = ops[1];
				vector<string> preMsg {ops.begin()+2,ops.end()};
				string msg = parseMsg(preMsg);
				
				if(msg.empty()){
					msgBuff = "Usage: send <username> <message>\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				
				if(user == NULL){
					msgBuff = "Please login first.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				else{
					User* receiver = this->database->checkIfNameExist(receiverName);
					if(receiver == NULL){
						msgBuff = "User not existed.\n";
						send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					}
					else{
						receiver->addMail(user->getName(),msg);
					}
				}

				break;
			}
		case LISTMSG:
			{
				//list-msg
				if(user == NULL){
					msgBuff = "Please login first.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				else{
					if(user->getMailNum() == 0){
						msgBuff = "Your message box is empty.\n";
						send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					}
					else{
						msgBuff = user->listMsg();
						send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					}
				}
				break;
			}
		case RECEIVE:
			{
				//receive <username>
				if(ops.size()!=2){
					msgBuff = "Usage: receive <username>\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				string sender = ops[1];
								
				if(user == NULL){
					msgBuff = "Please login first.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				else{
					if(database->checkIfNameExist(sender) == NULL){
						msgBuff = "User not existed.\n";
						send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
						break;
					}
					if(user->checkMailExist(sender)){
						msgBuff = user->receiveMsg(sender) + "\n";
						send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					}
				}
			
				break;
			}
		case CREATEBOARD:
			{
				if(ops.size()!=2){
					msgBuff = "Usage: create-board <name>\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				if(user == NULL){
					msgBuff = "Please login first.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				string boardName = ops[1];
				if(!database->isBoardExist(boardName)){
					database->createBoard(boardName, user->getName());
					msgBuff = "Create board successfully.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				else{
					msgBuff = "Board already exists.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
			}
		case CREATEPOST:
			{
				if(!isOpsLegal(CREATEPOST,ops)){
					msgBuff = "Usage: create-post <board-name> --title <title> --content <content>\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}

				if(user == NULL){
					msgBuff = "Please login first.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}

				string boardName, title, content;
				bool titleOccurred, contentOccurred;
				titleOccurred = contentOccurred = false;
				title = content = "";
				boardName = ops[1];
				
				for(int i=2;i<ops.size();i++){
					if(ops[i] == "--title") {titleOccurred = true; contentOccurred = false; continue;}
					if(ops[i] == "--content") {contentOccurred = true; titleOccurred = false; continue;}
					if(titleOccurred && !contentOccurred) title += ops[i] + " ";
					if(!titleOccurred && contentOccurred) content += ops[i] + " ";
				}
				content = regex_replace(content, regex("<br>"), "\n");

				if(database->isBoardExist(boardName)){
					database->createPost(boardName, title, content, user->getName());
					msgBuff = "Create post successfully.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				else{
					msgBuff = "Board does not exist.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
			}
		case LISTBOARD:
			{
				if(ops.size()!=1){
					msgBuff = "Usage: list-board\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}

				string boardstr = database->listBoard();
				msgBuff = "Index\tName\tModerator\n" + boardstr;
				send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
				break;
			}
		case LISTPOST:
			{
				if(ops.size()!=2){
					msgBuff = "Usage: list-post <board-name>\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				string boardName = ops[1];
				if(database->isBoardExist(boardName)){
					string poststr = database->listPost(boardName);
					msgBuff = "S/N\tTitle\tAuthor\tDate\n" + poststr;
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				else{
					msgBuff = "Board does not exist.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
			}
		case READ:
			{
				if(ops.size()!=2){
					msgBuff = "Usage: read <post-S/N>\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				int serialNum;
				stringstream ss;
				ss << ops[1];
				ss >> serialNum;
				if(database->isPostExist(serialNum)){
					msgBuff =  database->readPost(serialNum);
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				else{
					msgBuff = "Post does not exist.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
			}
		case DELETEPOST:
			{
				if(ops.size()!=2){
					msgBuff = "Usage: delete-post <post-S/N>\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				if(user == NULL){
					msgBuff = "Please login first.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				int serialNum;
				stringstream ss;
				ss << ops[1];
				ss>> serialNum;
				if(database->isPostExist(serialNum)){
					if(database->deletePost(serialNum,user->getName())){
						msgBuff =  "Delete successfully.\n";
						send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
						break;
					}
					else{
						msgBuff = "Not the post owner.\n";
						send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
						break;
					}
				}
				else{
					msgBuff = "Post does not exist.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
			}
		case UPDATEPOST:
			{
				if(!isOpsLegal(UPDATEPOST,ops)){
					msgBuff = "Usage: update-post <post-S/N> --title/content <new>\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}

				int serialNum;
				stringstream ss;
				ss << ops[1];
				ss>> serialNum;

				string modified = "";
				for(int i=3;i<ops.size();i++){
					modified += ops[i] + " ";
				}
				modified = regex_replace(modified, regex("<br>"), "\n");

				if(database->isPostExist(serialNum)){
					if(ops[2] == "--title"){
						if(database->updatePostTitle(serialNum, modified, user->getName())){
							msgBuff =  "Update successfully.\n";
							send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
							break;
						}
						else{
							msgBuff = "Not the post owner.\n";
							send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
							break;
						}
					}
					else if(ops[2] == "--content"){
						if(database->updatePostContent(serialNum, modified, user->getName())){
							msgBuff =  "Update successfully.\n";
							send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
							break;
						}
						else{
							msgBuff = "Not the post owner.\n";
							send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
							break;
						}
					}
				}
				else{
					msgBuff = "Post does not exist.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
			}
		case COMMENT:
			{
				if(ops.size()<3){
					msgBuff = "Usage: comment <post-S/N> <comment>\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				if(user == NULL){
					msgBuff = "Please login first.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				int serialNum;
				stringstream ss;
				ss << ops[1];
				ss >> serialNum;

				string comment = "";
				for(int i=2;i<ops.size();i++){
					comment += ops[i] + " ";
				}

				if(database->isPostExist(serialNum)){
					database->commentPost(serialNum, comment, user->getName());
					msgBuff =  "Comment successfully.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				else{
					msgBuff = "Post does not exist.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				
			}
		case ENTERCHATROOM:
			{
				if(ops.size() != 3){
					msgBuff = "Usage: enter-chat-room <port> <version>\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				int portNum;
				int version;
				stringstream ss("");
				ss << ops[1]; ss<<" "; ss << ops[2];
				ss >> portNum; ss >> version;
				cout<<portNum<<" "<<version<<endl;
				if(portNum < 1 || portNum > 65535){
					msgBuff = "Port "+ ops[1] + " is not valid.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				if(version != 1 && version != 2){
					msgBuff = "Version " + ops[2] + " is not supported.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				if(user == NULL){
					msgBuff = "Please login first.\n";
					send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
					break;
				}
				database->addChatRoomMembers(user, portNum, version);
				string chatHistory = database->getChatHistory();
				msgBuff = "Welcome to public chat room.\nPort:" + ops[1] + "\nVersion:" + ops[2] + "\n" + chatHistory;
				send(connfd, msgBuff.c_str(), msgBuff.size(), 0);
			}
		default:
			break;
	}
	return user;
}

string Server::parseMsg(vector<string>& preMsg){
	int len = preMsg.size();
	if(preMsg[0][0] != '\"' || preMsg[len-1][preMsg[len-1].size()-1] != '\"')
		return "";
	string msg = "";
	for(int i=0;i<len;i++){
		//if(i == 0)     preMsg[i] = preMsg[i].substr(1,preMsg[i].size()-1);
		if(i == len-1) preMsg[i] = preMsg[i].substr(0,preMsg[i].size()-1);
		msg += preMsg[i] + " ";
	}
	return msg;	
}

bool Server::isOpsLegal(Command cmd, vector<string>& ops){
	switch(cmd){
		case CREATEPOST:
			{
				if(ops.size() < 6) return false;
				bool isTitle, isContent, isBoardName;
				isTitle = isContent = isBoardName = false;
				if(ops[1] != "--title" && ops[1] != "--content") isBoardName = true;
				for(int i=2;i<ops.size();i++){
					if(ops[i] == "--title") isTitle = true;
					if(ops[i] == "--content") isContent = true;
				}
				if(isTitle && isContent && isBoardName) return true;
				else return false;
			}
		case UPDATEPOST:
			{
				if(ops.size() < 4) return false;
				bool isEither = false;
				if(ops[2] == "--title" || ops[2] == "--content") isEither = true;
				return isEither;
			}
		default:
			return false;
	}
}

User* Server::recvChatPackage(){
	unsigned char buf[4096];
	sockaddr_in	cliaddr;
	socklen_t	len;
	int n = recvfrom(udpfd, buf, 4096, 0,(sockaddr*) &cliaddr, &len);
	
	unsigned char* cname;
	unsigned char* cmsg;

	cname = (unsigned char*) malloc(1024);
	cmsg = (unsigned char*) malloc(1024);

	Header *ph = (Header*) buf;
	unsigned char flag = ph->flag;
	unsigned char version = ph->version;

	if(version == 0x01){
		Data *pd1 = (Data*) (buf + sizeof(Header));
		uint16_t name_len = ntohs(pd1->len);
		memcpy(cname, pd1->data, name_len);
		cname[name_len] = 0;
		Data *pd2 = (Data*) (buf + sizeof(Header) + sizeof(Data) + name_len);
		uint16_t msg_len = ntohs(pd2->len);
		memcpy(cmsg, pd2->data, msg_len);
		cmsg[msg_len] = 0;
	}
	else if(version == 0x02){
		sscanf((char*)buf+16,"%s\n%s\n", cname, cmsg);
	}

	string name((char*)cname);
	string msg((char*)cmsg);

	if(version == 0x02){
		name = base64_decode(name);
		msg = base64_decode(msg);
	}

	string filteredMsg = chatFilter(msg);
	string chatSentence = name + ":" + filteredMsg;

	User* user = database->checkIfNameExist(name);
	if(filteredMsg != msg){
		user->increaseViolationCount();
		if(user->getViolationCount() == 3){
			//logout
			database->removeChatRoomMember(user);
			database->addblacklist(user);
		}
		else user = nullptr;
	}else user = nullptr;

	cmsg = (unsigned char*) filteredMsg.c_str();
	database->addChatHistory(chatSentence);
	broadcast(cliaddr, cname, cmsg, n);
	//sendto(udpfd, buf, n, 0, (sockaddr*) &cliaddr, len);
	return user;
}

string Server::chatFilter(string input){
	regex vowel_re("how|you|or|pek0|tea|ha|kon|pain|Starburst Stream");
	string filtered = regex_replace(input, vowel_re, "[$&]");
	bool flag = false;
	
	for(int i=0; i<filtered.size(); i++){
		if(filtered[i] == '['){
			flag = true;
			filtered.erase(i,1);
		}
		if(filtered[i] == ']'){
			flag = false;
			filtered.erase(i,1);
			i--;
		}
		if(flag) filtered.replace(i,1,"*");
	}
	return filtered;
}

void Server::broadcast(sockaddr_in cliaddr, unsigned char* cname, unsigned char* cmsg, int n){
	for(auto cli : database->getChatRoomMembers()){
		((sockaddr_in *) &cliaddr)->sin_port = htons(cli->getPortNum());
		// ofstream fout;
		// fout.open("1.txt");
		// fout<< cli->getPortNum() << endl;
		// fout.close();
		if(cli->getChatVersion() == 1){
			unsigned char buf[4096];

			uint16_t name_len = (uint16_t)strlen((char*)cname);
			uint16_t msg_len = (uint16_t)strlen((char*)cmsg);

			Header *ph = (Header*) buf;
			Data *pd1 = (Data*) (buf + sizeof(Header));
			Data *pd2 = (Data*) (buf + sizeof(Header) + sizeof(Data) + name_len);
			ph->flag = 0x01;
			ph->version = 0x01;
			pd1->len = htons(name_len);
			memcpy(pd1->data, cname, name_len);
			pd2->len = htons(msg_len);
			memcpy(pd2->data, cmsg, msg_len);

			sendto(udpfd, buf, n, 0, (sockaddr*) &cliaddr, sizeof(cliaddr));
		}
		else if(cli->getChatVersion() == 2){
			char buf[4096];
			string msg = base64_decode(string((char*) cmsg));
			string name = base64_decode(string((char*) cname));
			sprintf(buf, "\x01\x02%s\n%s\n", name.c_str(), msg.c_str());
		
			sendto(udpfd, buf, n, 0, (sockaddr*) &cliaddr, sizeof(cliaddr));
		}
	}
	
}

string Server::starGenerator(string str){
	int n = str.size();
	string stars("");
	for(int i=0; i<n; i++){
		stars+="*";
	}
	return stars;
}

string Server::base64_encode(const string in) {

    std::string out;

    int val = 0, valb = -6;
    for (unsigned char c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val>>valb)&0x3F]);
            valb -= 6;
        }
    }
    if (valb>-6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val<<8)>>(valb+8))&0x3F]);
    while (out.size()%4) out.push_back('=');
    return out;
}

string Server::base64_decode(const string in) {

    string out;

    vector<int> T(256,-1);
    for (int i=0; i<64; i++) T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

    int val=0, valb=-8;
    for (unsigned char c : in) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val>>valb)&0xFF));
            valb -= 8;
        }
    }
    return out;
}