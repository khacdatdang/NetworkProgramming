#ifndef _SERVER_FUNC_H
#define _SERVER_FUNC_H
#include "protocol.h"

void handle_message(char*, int);
int registerUser(char*, int);
int loginUser(char*, int);
int logoutUser(char*, int);
int loadGame(int);
void encryptPassword(char*);
void finish_with_error(MYSQL* con);
int sendQuestion(char*, int);
int answerQuestion(char* message, int socket);
int helpAnswer(char* message, int socket);
int calculateScore(char*, int, REQUEST_CODE);
void showDashboard(int socket);
int getHighScore(int socket);
#endif  // _SERVER_FUNC_H
