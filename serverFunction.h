#ifndef _SERVER_FUNC_H
#define _SERVER_FUNC_H

#include "protocol.h"
STATE handle_message(Request*, int, STATE);
int registerUser(char*, int);
int loginUser(char*, int);
void encryptPassword(char*);
void finish_with_error(MYSQL* con);

#endif  // _SERVER_FUNC_H
