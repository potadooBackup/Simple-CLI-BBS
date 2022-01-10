#include "../header/command2Enum.hpp"

Command command2enum(string cmd){
	if (cmd == "register") return REGISTER;
	else if (cmd == "login") return LOGIN;
	else if (cmd == "logout") return LOGOUT;
	else if (cmd == "whoami") return WHOAMI;
	else if (cmd == "list-user") return LISTUSER;
	else if (cmd == "exit") return EXIT;
	else if (cmd == "send") return SEND;
	else if (cmd == "list-msg") return LISTMSG;
	else if (cmd == "receive") return RECEIVE;
	else if (cmd == "create-board") return CREATEBOARD;
	else if (cmd == "create-post") return CREATEPOST;
	else if (cmd == "list-post") return LISTPOST;
	else if (cmd == "list-board") return LISTBOARD;
	else if (cmd == "read") return READ;
	else if (cmd == "delete-post") return DELETEPOST;
	else if (cmd == "update-post") return UPDATEPOST;
	else if (cmd == "comment") return COMMENT;
	else if (cmd == "enter-chat-room") return ENTERCHATROOM;
	else if (cmd == "chat") return CHAT;
	else return ERROR;
}
