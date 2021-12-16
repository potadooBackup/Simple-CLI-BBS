#include "database.hpp"

Database::Database(){
	memberNum = 0;
	boardNum = 0;
	serialNumOfPost = 0;
	chatHistory = "";
}

User* Database::loginQuery(string _name, string _passwd){
	for(int i=0; i<memberNum; i++){
		//cout<<members[i].getName()<<" "<<members[i].getPasswd()<<endl;
		if(members[i].getName() == _name && members[i].getPasswd() == _passwd){
			return &members[i];
		}
	}
	return NULL;
}

User* Database::checkIfNameExist(string _name){
	for(int i=0; i<memberNum; i++){
		if(members[i].getName() == _name) return &members[i];
	}
	return NULL;
}

void Database::addMember(string username, string password){
	members[memberNum].setName(username);
	members[memberNum].setPasswd(password);
	memberNum++;
}

void Database::addChatHistory(string chat){
	chatHistory += chat + "\n";
}

User* Database::getMembers(){
	return members;
}

int Database::getMemberNum(){
	return memberNum;
}

string Database::getChatHistory(){
	return chatHistory;
}

vector<User*> Database::getChatRoomMembers(){
	return chatroomMembers;
}

string Database::listUser(){
	string str = "";
	sort(members,members+memberNum,
			[](const auto& lhs, const auto& rhs) {return lhs.getName()<rhs.getName();});
	for(int i=0;i<memberNum;i++) str += members[i].getName() + "\n";
	return str;
}

bool Database::isBoardExist(string boardName){
	for(int i=0;i<this->boards.size();i++){
		if(boards[i].name == boardName) return true;
	}
	return false;
}

void Database::createBoard(string boardName, string moderator){
	unique_ptr<Board> newBoard = make_unique<Board>(++this->boardNum, boardName, moderator);
	this->boards.push_back(*newBoard);
}

void Database::createPost(string boardName, string title, string content, string author){
	time_t now = time(0);
	tm *local_time = localtime(&now);
	string postTime = to_string(1+local_time->tm_mon) + "/" + to_string(local_time->tm_mday);
	unique_ptr<Post> newPost = make_unique<Post>(++this->serialNumOfPost, 
												 title,
												 content,
												 author,
												 postTime);
	for(auto& board : boards){
		if(board.name == boardName){
			board.posts.push_back(*newPost);
		}
	}
}

string Database::listBoard(){
	string str = "";
	for(auto board : boards){
		str += to_string(board.index) + "\t" + board.name + "\t" + board.moderator + "\n";
	}
	return str;
}

string Database::listPost(string boardName){
	string str = "";
	for(auto board : boards){
		if(board.name == boardName){
			for(auto post : board.posts){
				str += to_string(post.index) + "\t" + post.title + "\t" + post.author + "\t" + post.postTime + "\n";
			}
		}
	}
	return str;
}

string Database::readPost(int serialNum){
	string str = "";
	for(auto board : boards){
		for(auto post : board.posts){
			if(post.index == serialNum){
				str += "Author: " + post.author + "\n";
				str += "Title: " + post.title + "\n";
				str += "Date: " + post.postTime + "\n";
				str += "--\n";
				str += post.content + "\n";
				str += "--\n";
				for(auto comment : post.comments){
					str += comment.first + ": " + comment.second + "\n";
				}
			}
		}
	}
	return str;
}

bool Database::isPostExist(int serialNum){
	for(auto board : boards){
		for(auto post : board.posts){
			if(post.index == serialNum)
				return true;
		}
	}
	return false;
}

bool Database::deletePost(int serialNum, string userName){
	for(auto& board : boards){
		for(auto postIt = board.posts.begin(); postIt != board.posts.end() ; postIt++){
			if(postIt->index == serialNum){
				if(postIt->author == userName){
					board.posts.erase(postIt);
					return true;
				}
				else{
					return false;
				}
			}
		}
	}
}

bool Database::updatePostTitle(int serialNum, string modified, string userName){
	for(auto& board : boards){
		for(auto& post : board.posts){
			if(post.index == serialNum){
				if(post.author == userName){
					post.title = modified;
					return true;
				}
				else{
					return false;
				}
			}
		}
	}
}

bool Database::updatePostContent(int serialNum, string modified, string userName){
	for(auto& board : boards){
		for(auto& post : board.posts){
			if(post.index == serialNum){
				if(post.author == userName){
					post.content = modified;
					return true;
				}
				else{
					return false;
				}
			}
		}
	}
}

void Database::commentPost(int serialNum, string comment, string userName){
	for(auto& board : boards){
		for(auto& post : board.posts){
			if(post.index == serialNum){
				pair<string,string> newComment = {userName, comment};
				post.comments.push_back(newComment);
			}
		}
	}
}

void Database::addChatRoomMembers(User* user, int port, int version){
	user->setPortNum(port);
	user->setChatVersion(version);
	chatroomMembers.push_back(user);
}

void Database::removeChatRoomMember(User* user){
	for(auto iter = chatroomMembers.begin(); iter != chatroomMembers.end(); iter++){
		if(*iter == user){
			chatroomMembers.erase(iter);
			return;
		}
	}
}

void Database::addblacklist(User* user){
	blacklist.push_back(user->getName());
}

bool Database::isInBlacklish(string name){
	for(auto str: blacklist){
		if(str==name) return true;
	}
	return false;
}