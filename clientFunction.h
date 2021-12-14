#ifndef _CLIENT_FUNC_H
#define _CLIENT_FUNC_H
#include "protocol.h"

void initMenu();
void authMenu();
//int getUserChoice(int);
void getString(char*, char*);
STATE extractServerMessage(Response*);
void createMessage(char*, int, char*, char*);

int login(int, int);
int signup(int, int);
int logout(int, int);
int playgame(int,int);
#endif  // _CLIENT_FUNC_H
