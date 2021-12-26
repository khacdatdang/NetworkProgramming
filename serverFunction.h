#ifndef _SERVER_FUNC_H
#define _SERVER_FUNC_H


STATE handle_message(char*, int, STATE);
int registerUser(char*, int);
int loginUser(char*, int);
void encryptPassword(char*);
void finish_with_error(MYSQL* con);
int playGame(int);
#endif  // _SERVER_FUNC_H
