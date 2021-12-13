#ifndef _SERVER_FUNC_H
#define _SERVER_FUNC_H


int handle_message(char*, int, int);
int registerUser(char*, int);
int loginUser(char*, int);
void encryptPassword(char*);
void finish_with_error(MYSQL* con);

#endif  // _SERVER_FUNC_H
