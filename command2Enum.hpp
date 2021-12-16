#ifndef COMMAND2ENUM_HPP
#define COMMAND2ENUM_HPP

#include <iostream>
using namespace std;

enum Command{
	REGISTER,
	LOGIN,
	LOGOUT,
	WHOAMI,
	LISTUSER,
	EXIT,
	SEND,
	LISTMSG,
	RECEIVE,
	SPACE,
	CREATEBOARD,
	CREATEPOST,
	LISTBOARD,
	LISTPOST,
	READ,
	DELETEPOST,
	UPDATEPOST,
	COMMENT,
	ENTERCHATROOM,
	CHAT,
	ERROR
};

Command command2enum(string);



#endif
