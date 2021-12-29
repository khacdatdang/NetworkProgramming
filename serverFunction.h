#ifndef _SERVER_FUNC_H
#define _SERVER_FUNC_H
#include "protocol.h"

STATE handle_message(char*, int, STATE);
int registerUser(char*, int);
int loginUser(char*, int);
int loadGame(int);
void encryptPassword(char*);
void finish_with_error(MYSQL* con);
int playGame(int);
int calculateScore(REQUEST_CODE, int );
void showDashboard(int socket);
int getHighScore(int socket);
#endif  // _SERVER_FUNC_H
