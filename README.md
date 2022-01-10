# Multiuser CLI BBS
<span style="color:grey">*Intro. to Network Programming, lectured by Chun-Ying Huang* </span>

In this project, I designed a Bulletin Board System(BBS) server. This program runs in single process and use **select()** to handle multiple connections (users). Client connect to the server by TCP connection, while chat with UDP connection and special packet form.

## Environment

ubuntu 20.04  
g++ 14

## Program Structure  

![image](https://github.com/potadooweii/Simple-CLI-BBS/blob/master/imgs/structure%231.png)

## Features
All exceptions are handled.
### Basics
* **register \<username\> \<password\>**  
   Register with username and password. 
* **login \<username\> \<password\>**  
  Login with username and password.
* **logout**  
  Logout account.
* **whoami**  
  Show your username. 
* **list-user**  
  List all users in BBS.
* **exit**  
  Close connection.
### Private Message
*  **send \<username> \<message>**  
leave message to a user.
* **list-msg**  
list the message state of Message Box.
* **receive \<username>**  
 Show the first message from <username>, and pop the message.

### Board and Post

* **create-board \<name>**  
Create a board which named <name\>.

* **create-post \<board-name> --title \<title> --content \<content>**  
Create a post which title is <title\> and content is <content\>.

* **list-board**  
List all boards in BBS.
* **list-post <board-name>**  
List all posts in a board named <board-name\>
* **read <post-S/N\>**  
Show the post which S/N is \<post-S/N>.

* **delete-post <post-S/N\>**  
Delete the post which S/N is <post-S/N\>.

* **update-post <post-S/N> --title/content <new>**  
Update the post which S/N is <post-S/N>.

### Chat Room

* **enter-chat-room \<port\> \<version\>**  
Enter a chat room.
* **chat \<message\>**  
send the messages by UDP packets.

#### message format (version 1)
```
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 (bits)
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|      flag     |    version    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|         length of name        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                               |
+                               +
|                               |
+              name             +
|                               |
+                               +
|                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|       length of message       |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                               |
+                               +
|                               |
+            message            +
|                               |
+                               +
|                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

| Field Name            | Size                       | Description                         |
| --------------------- | -------------------------- | ----------------------------------- |
| **flag**              | 8 bits                     | it must be `0x1`                    |
| **version**           | 8 bits                     | it must be `0x1`                    |
| **length of name**    | 16 bits                    | length of message in **big endian** |
| **name**              | `strlen(name)` * 8 bits    |                                     |
| **length of message** | 16 bits                    | length of message in **big endian** |
| **message**           | `strlen(message)` * 8 bits |                                     |




#### message format (version 2)
```
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 (bits)
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|      flag     |    version    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                               |
+                               +
|                               |
+                               +
|                               |
+                               +
|              name             |
+               +-+-+-+-+-+-+-+-+
|               |   separator   |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                               |
+                               +
|                               |
+                               +
|                               |
+                               +
|            message            |
+               +-+-+-+-+-+-+-+-+
|               |   separator   |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

| Field Name    | Size                               | Description                            |
| ------------- | ---------------------------------- | -------------------------------------- |
| **flag**      | 8 bits                             | its must be `0x1`                     |
| **version**   | 8 bits                             | it must be `0x2`                     |
| **name**      | `strlen(base64(name))` * 8 bits    | name encoded in base64    |
| **separator** | 8 bits                             | it must be `'\n'`                    |
| **message**   | `strlen(base64(message))` * 8 bits | message encoded in base64 |
| **separator** | 8 bits                             | its value is `'\n'`                    |

#### chat history
When a user joins the public (enter-chat-room) chat room, the server should immediately encapsulate all messages in chat history in the format of <chat_history> and sends it to the client in the TCP connection.

#### content filtering
To keep our chat room a place filling with joy and happiness, some keywords should not be used. Below is the filtering list of those keywords. The server must mask each matched word in <message\> sent by the `chat` command by replacing the word with a string of `*` characters of equal length.   
**Hard coded filtering list in C/C++ program file.**
