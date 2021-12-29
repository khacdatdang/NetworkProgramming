#ifndef _CLIENT_FUNC_H
#define _CLIENT_FUNC_H
void initMenu();
void authMenu();
void answerMenus();
int getUserChoice(int);
void getString(char*, char*);
//int extractServerMessage(char*, );
void createMessage(char*, int, char*, char*);

int login(int, int);
int signup(int, int);
int logout(int, int);
int playgame(int,int);
int dashboard(int,int);
#endif  // _CLIENT_FUNC_H
