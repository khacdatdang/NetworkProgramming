#include <mysql/mysql.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


#include "serverFunction.h"
#include "protocol.h"

extern MYSQL* con;

void finish_with_error(MYSQL* con) {
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

STATE handle_message(Request* request, int socket, STATE state) {
    Response *response = (Response*) malloc (sizeof(Response));
  switch (request->code) {
    case LOGIN:
      if (state != NOT_AUTH) {
          response->code = USERNAME_IS_SIGNIN;
          setMessageResponse(response);
        sendResponse(socket, response, sizeof(Response), 0);
        return state;
      }
      if (loginUser(request->message, socket)) {
        return AUTH;
      } else {
        return state;
      }
      break;
    case REGISTER:
      if (state != NOT_AUTH) {
          response->code = USERNAME_IS_SIGNIN;
          setMessageResponse(response);
          sendResponse(socket, response, sizeof(Response), 0);
          return state;
      }
      if (registerUser(request->message, socket)) {
        return AUTH;
      } else {
        return NOT_AUTH;
      }
      break;
    case LOGOUT:
        if (state != NOT_AUTH) {
            response->code = LOGOUT_SUCCESS;
            setMessageResponse(response);
            sendResponse(socket, response, sizeof(Response), 0);
            return NOT_AUTH;
        }
//      if (state != NOT_AUTH) {
//        sprintf(server_message, "%d|S|Log out successful\n", LOGOUT);
//        send(socket, server_message, sizeof(server_message), 0);
//        return NOT_AUTH;
//      }
        else {
            response->code = LOGOUT_FAIL;
            setMessageResponse(response);
            sendResponse(socket, response, sizeof(Response), 0);
            return state;
        }
        break;
    default:
      return state;
      break;
  }
}

int registerUser(char* message, int socket) {
    Response *response = (Response*) malloc (sizeof(Response));
    char username[100] = "\0";
  char password[100] = "\0";
  char* token;
  token = strtok(message, "|");
  strcpy(username, token);
  token = strtok(NULL, "|");
  strcpy(password, token);
  encryptPassword(password);

  char query[200] = "\0";

  sprintf(query, "INSERT INTO users (username, password) VALUES ('%s', '%s')",
          username, password);
  if (mysql_query(con, query)) {
    sprintf(response->message, "%s\n", mysql_error(con));
    sendResponse(socket, response, sizeof(Response), 0);
    return 0;
  }
  response->code = REGISTER_SUCCESS;
    setMessageResponse(response);
  sendResponse(socket, response, sizeof(Response), 0);
  return 1;
}

int loginUser(char* message, int socket) {
  char username[100] = "\0";
  char password[100] = "\0";
  Response* response = (Response*) malloc(sizeof(Response));
  char* token;
  token = strtok(message, "|");
  strcpy(username, token);
  token = strtok(NULL, "|");
  strcpy(password, token);
  encryptPassword(password);

  char query[200] = "\0";

  sprintf(query, "SELECT * from users where username='%s' and password='%s'",
          username, password);
  if (mysql_query(con, query)) {
    sprintf(response->message, "%d|F|%s\n", LOGIN, mysql_error(con));
    send(socket, response, sizeof(Response), 0);
    return 0;
  }
  MYSQL_RES* result = mysql_store_result(con);
  if (mysql_num_rows(result) == 0) {
    response->code = USERNAME_NOTFOUND;
    setMessageResponse(response);
    sendResponse(socket, response, sizeof(Response), 0);
    return 0;
  }
  response->code = LOGIN_SUCCESS;
  setMessageResponse(response);
  sendResponse(socket, response, sizeof(Response), 0);
  return 1;
}

void encryptPassword(char* password) {
  int encrypt = 0;
  for (int i = 0; i < strlen(password); i++) {
    if ((int)password[i] > i) {
      password[i] = password[i] - i;
    }
  }
}