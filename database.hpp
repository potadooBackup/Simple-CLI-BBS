#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <iostream>
#include <vector>
#include <ctime>
#include <sstream>
#include <memory>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <algorithm>

#include "user.hpp"
#include "board.hpp"

#define MAXMEMBERNUM 30

using namespace std;

class Database{
	private:
		User	   		   members[MAXMEMBERNUM];
		int	       		   memberNum;
		vector<Board>	   boards;
		vector<string>     blacklist;
		vector<User*>      chatroomMembers;
		string			   chatHistory;
		int				   serialNumOfPost;
		int				   boardNum;
	public:
		//Constructor
		Database();
		
		//Get/Set functions
		int	       	   getMemberNum();
		User*          getMembers();
		string		   getChatHistory();
		vector<User*>  getChatRoomMembers();
		
		//Commands
		string	       listUser();
		string		   listBoard();
		string		   listPost(string);
		void		   createBoard(string, string);
		void		   createPost(string, string, string, string);
		string		   readPost(int);
		bool		   deletePost(int, string);
		bool		   updatePostTitle(int, string, string);
		bool		   updatePostContent(int, string, string);
		void		   commentPost(int, string, string);
		
		//Utility
		User* 	       loginQuery(string,string);
		User*	       checkIfNameExist(string);
		bool		   isInBlacklish(string);
		void	       addMember(string,string);
		bool		   isBoardExist(string);
		bool		   isPostExist(int);
		void		   addChatHistory(string);
		void           addChatRoomMembers(User*, int, int);
		void		   removeChatRoomMember(User*);
		void		   addToBlacklist(User*);
};



#endif
