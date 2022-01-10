#ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include <ctime>
#include <vector>
#include <map>
#include <string>

#define MAXMAILNUM 100

using namespace std;

class User{
	private:
		string  	    	name;
		string		    	passwd;
		pair<string,string> mailbox[MAXMAILNUM];
		int 		    	mailNum;
		int					violationCount;
		int					portNum;
		int					chatVersion;
		bool				isLogined;
		
	public:
		//Constructor
		User(string,string);
		User();
		
		//Get/Set functions
		string getName() const;
		string getPasswd() const;
		int    getMailNum();
		bool   getIsLogined();
		int    getPortNum();
		int	   getChatVersion();
		int    getViolationCount();
		void   setName(string);
		void   setPasswd(string);
		void   setLoginTime(time_t);
		void   setIsLogined(bool);
		void   setPortNum(int);
		void   setChatVersion(int);
		void   increaseViolationCount();

		//Commands
		string receiveMsg(string);
		string listMsg();

		//utility
		void   addMail(string,string);
		bool   checkMailExist(string);
};


#endif
