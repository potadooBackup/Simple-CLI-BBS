#ifndef COMMAND2ENUM_H
#define COMMAND2ENUM_H

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
