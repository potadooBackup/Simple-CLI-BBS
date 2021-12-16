#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <vector>
#include <string>
#include <ctime>
using namespace std;

class Post{
    friend class Board;
    friend class Database;
    public:
        Post(){}
        Post(int _index, string _title, string _content, string _author, string _postTime):
            index(_index),title(_title),content(_content),author(_author),postTime(_postTime){}
    private:
        int    index; 
        string title;
        string content;
        string author;
        string postTime;
        vector<pair<string, string>> comments;
};

class Board{
    friend class Database;
    public:
        Board(){}
        Board(int _index, string _name, string _moderator):
            index(_index),name(_name), moderator(_moderator){}
    private:
        int          index;
        string       name;
        string       moderator;
        vector<Post> posts;
};

#endif