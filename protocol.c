//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "protocol.h"
#include <sys/socket.h>
int receiveRequest(int socket, Request *buff, int size, int flags)
{
    int n;
    n = recv(socket, buff, size, flags);
    if (n < 0)
        perror("Error: ");
    return n;
}

int sendRequest(int socket, Request *buff, int size, int flags)
{
    int n;

    n = send(socket, buff, size, flags);
    if (n < 0)
        perror("Error: ");
    return n;
}

int sendResponse(int socket, Response *msg, int size, int flags)
{
    int n;
    n = send(socket, msg, size, flags);
    if (n < 0)
        perror("Error: ");
    if (n == 0)
        printf("Cannot send\n");
    return n;
}

int receiveResponse(int socket, Response *msg, int size, int flags)
{
    int n;
    n = recv(socket, msg, size, flags);
    if (n < 0)
        perror("Error: ");
    if (n == 0)
        printf("Cannot recieve\n");
    return n;
}

void setMessageResponse(Response *msg)
{
    switch (msg->code)
    {
//            case SYNTAX_ERROR:
//                strcpy(msg->message, "Syntax error ");
//                break;
//            case INVALID_OPERATION:
//                strcpy(msg->message, "Invalid operation ");
//                break;
        case USERNAME_NOTFOUND:
            strcpy(msg->message, "Cannot find account ");
            break;
        case USERNAME_BLOCKED:
            strcpy(msg->message, "Account is blocked ");
            break;
        case USERNAME_IS_SIGNIN:
            strcpy(msg->message, "Login only one account ");
            break;
        case PASSWORD_CORRECT:
            strcpy(msg->message, "Login successful ");
            break;
        case PASSWORD_INCORRECT:
            strcpy(msg->message, "Password incorrect ");
            break;
        case PASSWORD_INCORRECT_THREE_TIMES:
            strcpy(msg->message, "Password is incorrect. Account is blocked ");
            break;
        case LOGOUT_SUCCESS:
            strcpy(msg->message, "Logout successful ");
            break;
        case LOGOUT_FAIL:
            strcpy(msg->message, "You must log in first ");
            break;
        case REGISTER_SUCCESS:
            strcpy(msg->message, "Register successfull ");
            break;
        case REGISTER_USERNAME_EXISTED:
            strcpy(msg->message, "Username is existed ");
            break;
        case LOGIN_SUCCESS:
            strcpy(msg->message, "Login successful ");
            break;
//            case PASSWORD_CORRECT_BUT_ACCOUNT_IS_SIGNINED_IN_ORTHER_CLIENT:
//                strcpy(msg->message, "Account is signin in orhter client ");
//                break;
//            case ANSWER_IS_CORRECT:
//                strcpy(msg->message, "The answer is correct ");
//                break;
//            case ANSWER_IS_INCORRECT:
//                strcpy(msg->message, "The answer is incorrect \nEnd game");
//                break;
//            case ANSWER_IS_INVALID:
//                strcpy(msg->message, "The answer is invalid ");
//                break;
//            case USER_USED_HINT_SUCCESS:
//                strcpy(msg->message, "User used hint success! ");
//                break;
//            case USER_USED_HINT_FAIL:
//                strcpy(msg->message, "User used hint fail! You have used up the suggestions ");
//                break;
//            case GAME_READY:
//                strcpy(msg->message, "Game ready ");
//                break;
//            case GAME_NOT_READY:
//                strcpy(msg->message, "Waiting orther player... ");
//                break;
//            case TOPIC_USER_DONT_CHOOSE_LEVEL:
//                strcpy(msg->message, "");
//                break;
//            case GAME_END_WIN:
//                strcpy(msg->message, "End game.\nYou are champion ");
//                break;
//            case GAME_IS_PLAYING_DONT_LOG_IN:
//                strcpy(msg->message, "\nGame is playing!! You can't login \n");
//                break;
        default:
            strcpy(msg->message, "Exception ");
            break;
    }

}

//void setOpcodeRequest(Request *request, char *input)
//{
//    char code[100], data[100];
////    splitMessage(input, code, data);
//    strcpy(request->message, data);
//    if (strcmp(code, "SIGNIN") == 0)
//        request->code = SIGNIN;
//    else if (strcmp(code, "REGISTER") == 0)
//        request->code = REGISTER;
//    else if (strcmp(code, "LOGOUT") == 0)
//        request->code = LOGOUT;
//    else if (strcmp(code, "ANSWER") == 0)
//        request->code = ANSWER;
//    else if (strcmp(code, "HELP") == 0)
//        request->code = HELP;
//}
// Created by dangkhacdat on 14/12/2021.
//

