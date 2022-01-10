#include "../header/user.hpp"

User::User(){};

User::User(string _name, string _passwd):
	name(_name),
	passwd(_passwd){
		mailNum = 0;
		isLogined = false;
		violationCount = 0;
		portNum = -1;
}

string User::getName() const{
	return this->name;
}

string User::getPasswd() const{
	return this->passwd;
}

int User::getMailNum(){
	return this->mailNum;
}

bool User::getIsLogined(){
	return this->isLogined;
}

int User::getChatVersion(){
	return this->chatVersion;
}

int User::getPortNum(){
	return this->portNum;
}

int User::getViolationCount(){
	return this->violationCount;
}

void User::addMail(string fromWho, string msg){
	string parsedMsg = msg.substr(1,msg.size()-2);
	this->mailbox[this->mailNum].first = fromWho;
	this->mailbox[this->mailNum].second = parsedMsg;
	mailNum++;
}

void User::setName(string userName){
	name = userName;
}

void User::setPasswd(string password){
	passwd = password;
}

void User::setIsLogined(bool _isLogined){
	this->isLogined = _isLogined;
}

void User::increaseViolationCount(){
	this->violationCount = this->violationCount + 1;
}

void User::setChatVersion(int _chatVersion){
	this->chatVersion = _chatVersion;
}

void User::setPortNum(int _portNum){
	this->portNum = _portNum;
}

string User::listMsg(){
	string returnBuff = "";
	map<string,int> hash;
	for(int i=0;i<this->mailNum;i++){
		if(hash.find(mailbox[i].first) == hash.end()){
			hash[mailbox[i].first] = 1;
		}
		else{
			hash[mailbox[i].first] ++;
		}
	}
	for(auto it : hash){
		string tmp;
		tmp = to_string(it.second) + " message from " + it.first + ".\n";
		returnBuff += tmp;
	}
	return returnBuff;	
}

bool User::checkMailExist(string sender){
	for(int i=0;i<mailNum;i++){
		if(mailbox[i].first == sender) return true;
	}
	return false;
}

string User::receiveMsg(string sender){
	string msg;
	bool found = false;
	for(int i=0;i<mailNum;i++){
		if(mailbox[i].first == sender && !found){
			msg = mailbox[i].second;
			found = true;
			mailbox[i].first = "";
			mailbox[i].second = "";
		}
		if(found && i+1<mailNum){
			mailbox[i].first = mailbox[i+1].first;
			mailbox[i].second = mailbox[i+1].second;
		}
	}
	mailNum--;
	return msg;
}
