#include <arpa/inet.h>
#include <errno.h>
#include <libgen.h>
#include <mysql/mysql.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "protocol.h"
#include "serverFunction.h"

extern MYSQL* con;

void finish_with_error(MYSQL* con) {
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

STATE handle_message(char* message, int socket, STATE state) {
  REQUEST_CODE type = message[0] - '0';
  char server_message[200] = "\0";
  int position = 0;
  switch (type) {
    case LOGIN:
      if (state != NOT_AUTH) {
        sprintf(server_message, "%d|You are already logged in\n", USERNAME_IS_SIGNIN);
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
        sprintf(server_message, "%d|You are already logged in\n", USERNAME_IS_SIGNIN);
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
        sprintf(server_message, "%d|Log out successful\n", LOGOUT_SUCCESS);
        send(socket, server_message, sizeof(server_message), 0);
        return NOT_AUTH;
      } else {
        sprintf(server_message, "%d|You must login or register first\n",
                LOGOUT_FAIL);
        send(socket, server_message, sizeof(server_message), 0);
        return state;
      }
    case JOIN_GAME: {
        printf("Handle join game\n");
        if (state == NOT_AUTH) {
            sprintf(server_message, "%d|You must login first\n", JOIN_GAME_FAIL);
            send(socket, server_message, sizeof(server_message), 0);
            return state;
        } else {
            playGame(socket);
            return AUTH;
        }
        break;
    }

    default:
      return state;
      break;
  }
}
int playGame(int socket){
    printf("Start send question \n");
    int position = 0;
    char serverMess[1024] = "\0";
    char query[200] = "\0";

    if (position >=0 && position < 5){
        sprintf(query, "SELECT * from questions where level = 1");
    } else if (position < 10){
        sprintf(query, "SELECT * from questions where level = 2");
    }
    else {
        sprintf(query, "SELECT * from questions where level = 3");
    }

    if (mysql_query(con, query)) {
        sprintf(serverMess, "%d|%s\n", QUERY_FAIL, mysql_error(con));
        send(socket, serverMess, strlen(serverMess), 0);
        return 0;
    }
    MYSQL_RES* result = mysql_store_result(con);
    char trueanswer;
    if (mysql_num_rows(result) == 0) {
        sprintf(serverMess, "%d|Question not found\n", QUERY_FAIL);
        send(socket, serverMess, strlen(serverMess), 0);
        return 0;
    }
    else {
        // Send question
        MYSQL_ROW row = mysql_fetch_row(result);
        sprintf(serverMess, "%d|%s|%s|%s|%s|%s\n", QUESTION, row[1], row[2], row[3], row[4], row[5]);
        send(socket, serverMess, strlen(serverMess), 0);
        //Get choose answer
        trueanswer = row[6];
    }
//    char client_message[100] = "\0";
//    recv(socket, client_message, 100, 0);
//    REQUEST_CODE type = client_message[0] - '0';
//    switch (type) {
//        case ANSWER: {
//            char* token;
//            char answer;
//            token = strtok(client_message, "|");
//            token = strtok(NULL, "|");
//            answer = token[0];
//            if (answer == trueanswer)
//                position += 1 ;
//           break;
//        }
//        case HELP: {
//            break;
//        }
//        case STOP: {
//            break;
//        }
//    }

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
    sprintf(serverMess, "%d|%s\n", QUERY_FAIL, mysql_error(con));
    send(socket, serverMess, strlen(serverMess), 0);
    return 0;
  }
  char server_message[100] = "\0";
  sprintf(server_message, "%d|Successfully registered\n", REGISTER_SUCCESS);
  send(socket, server_message, sizeof(server_message), 0);
  return 1;
}

int loginUser(char* message, int socket) {
    printf("Start handle login\n");
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
    printf("%s %s\n",username, password);
  char query[200] = "\0";

  sprintf(query, "SELECT * from users where username='%s'",username);
  if (mysql_query(con, query)) {
    sprintf(serverMess, "%d|%s\n", QUERY_FAIL, mysql_error(con));
    send(socket, serverMess, strlen(serverMess), 0);
    return 0;
  }
  MYSQL_RES* result = mysql_store_result(con);
  if (mysql_num_rows(result) == 0) {
    sprintf(serverMess, "%d|Invalid username or password\n", USERNAME_NOTFOUND);
    send(socket, serverMess, strlen(serverMess), 0);
    return 0;
  }
  else {
      MYSQL_ROW row = mysql_fetch_row(result);
      if (strcmp(row[2], password))
      {
//          printf("%s\n", row[0]);
          sprintf(serverMess, "%d|Password is incorrect\n", PASSWORD_INCORRECT);
          send(socket, serverMess, strlen(serverMess), 0);
          return 0;
      }
  }
  char server_message[100] = "\0";
  sprintf(server_message, "%d|Successfully logged in\n", LOGIN_SUCCESS);
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