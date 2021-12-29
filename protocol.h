//
// Created by dangkhacdat on 14/12/2021.
//
#ifndef NETWORKPROGRAMMING_PROTOCOL_H
#define NETWORKPROGRAMMING_PROTOCOL_H

typedef enum {
    LOGIN,
    REGISTER,
    LOGOUT,
    JOIN_GAME,
    QUESTION_REQUEST,
    DASHBOARD,
    ANSWER,
    HELP,
    STOP,
    EXIT
} REQUEST_CODE;


typedef enum {
    QUERY_FAIL,
    USERNAME_NOTFOUND,
    USERNAME_BLOCKED,
    USERNAME_IS_SIGNIN,
    LOGIN_SUCCESS,

    PASSWORD_CORRECT,
    PASSWORD_INCORRECT,
    PASSWORD_INCORRECT_THREE_TIMES,

    LOGOUT_SUCCESS,
    LOGOUT_FAIL,
    JOIN_GAME_FAIL,
    JOIN_GAME_SUCCESS,

    QUESTION,
    ANSWER_CORRECT,
    HELP_SUCCESS,
    HELP_FAIL,

    DASHBOARD_INFO,

    REGISTER_SUCCESS,
    REGISTER_USERNAME_EXISTED,
    END_GAME,
} RESPONSE_CODE;


typedef enum {
    NOT_AUTH,
    AUTH,
    IN_GAME,
} STATE;

//int receiveRequest(int socket, Request *buff, int size, int flags);
//int sendRequest(int socket, Request *buff, int size, int flags);
//
//int sendResponse(int socket, Response *msg, int size, int flags);
//int receiveResponse(int socket, Response *msg, int size, int flags);
//
//// set message response
//void setMessageResponse(Response *msg);
////void readMessageResponse(Response *msg);
//
////set opcode request
//void setOpcodeRequest(Request *request, char *input);


#endif //NETWORKPROGRAMMING_PROTOCOL_H
