#include <mysql/mysql.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "constain.h"
#include "serverFunction.h"

extern MYSQL* con;

void finish_with_error(MYSQL* con) {
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

int handle_message(char* message, int socket, int state) {
  int type = message[0] - '0';
  char server_message[100] = "\0";
  switch (type) {
    case LOGIN:
      if (state != NOT_AUTH) {
        sprintf(server_message, "%d|S|You are already logged in\n", LOGIN);
        send(socket, server_message, sizeof(server_message), 0);
        return state;
      }
      if (loginUser(message, socket)) {
        return AUTH;
      } else {
        return NOT_AUTH;
      }
      break;
    case REGISTER:
      if (state != NOT_AUTH) {
        sprintf(server_message, "%d|S|You are already logged in\n", REGISTER);
        send(socket, server_message, sizeof(server_message), 0);
        return state;
      }
      if (registerUser(message, socket)) {
        return AUTH;
      } else {
        return NOT_AUTH;
      }
      break;
    case LOGOUT:
      if (state != NOT_AUTH) {
        sprintf(server_message, "%d|S|Log out successful\n", LOGOUT);
        send(socket, server_message, sizeof(server_message), 0);
        return NOT_AUTH;
      } else {
        sprintf(server_message, "%d|F|You must login or register first\n",
                LOGOUT);
        send(socket, server_message, sizeof(server_message), 0);
        return state;
      }
    default:
      return state;
      break;
  }
}

int registerUser(char* message, int socket) {
  char username[100] = "\0";
  char password[100] = "\0";
  char serverMess[100] = "\0";
  char* token;
  token = strtok(message, "|");
  token = strtok(NULL, "|");
  strcpy(username, token);
  token = strtok(NULL, "|");
  strcpy(password, token);
  encryptPassword(password);

  char query[200] = "\0";

  sprintf(query, "INSERT INTO users (username, password) VALUES ('%s', '%s')",
          username, password);
  if (mysql_query(con, query)) {
    sprintf(serverMess, "%d|F|%s\n", REGISTER, mysql_error(con));
    send(socket, serverMess, strlen(serverMess), 0);
    return 0;
  }
  char server_message[100] = "\0";
  sprintf(server_message, "%d|S|Successfully registered\n", REGISTER);
  send(socket, server_message, sizeof(server_message), 0);
  return 1;
}

int loginUser(char* message, int socket) {
  char username[100] = "\0";
  char password[100] = "\0";
  char serverMess[100] = "\0";
  char* token;
  token = strtok(message, "|");
  token = strtok(NULL, "|");
  strcpy(username, token);
  token = strtok(NULL, "|");
  strcpy(password, token);
  encryptPassword(password);

  char query[200] = "\0";

  sprintf(query, "SELECT * from users where username='%s' and password='%s'",
          username, password);
  if (mysql_query(con, query)) {
    sprintf(serverMess, "%d|F|%s\n", LOGIN, mysql_error(con));
    send(socket, serverMess, strlen(serverMess), 0);
    return 0;
  }
  MYSQL_RES* result = mysql_store_result(con);
  if (mysql_num_rows(result) == 0) {
    sprintf(serverMess, "%d|F|Invalid username or password\n", LOGIN);
    send(socket, serverMess, strlen(serverMess), 0);
    return 0;
  }
  char server_message[100] = "\0";
  sprintf(server_message, "%d|S|Successfully logged in\n", LOGIN);
  send(socket, server_message, sizeof(server_message), 0);
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