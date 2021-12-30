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
//int position = 0;
//int helpTimes = 2;
//char  questions[16][1024];
//char UserName[100];

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
      loginUser(message, socket);
      break;
    case REGISTER:
        registerUser(message, socket);
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
          break;
    case QUESTION_REQUEST: {
        printf("handle game play\n");
        sendQuestion(message, socket);
        break;
    }
    case ANSWER: {
        printf("Handle answer \n");
        answerQuestion(message, socket);
        break;
    }
    case DASHBOARD:{
        printf("Handle dashboard\n");
        showDashboard(socket);
        break;

    }
    default:
      return state;
      break;
  }
}
void showDashboard(int socket){
    char query[200] = "\0";
    char serverMess[1024] = "\0";
    char temp[1024] = "\0";
    sprintf(query, "SELECT username, highScore from users ORDER BY highScore DESC");
    printf("%s\n", query);
    if (mysql_query(con, query)) {
        sprintf(serverMess, "%d|%s\n", QUERY_FAIL, mysql_error(con));
        send(socket, serverMess, strlen(serverMess), 0);
    }
    MYSQL_RES* result = mysql_store_result(con);
    if (mysql_num_rows(result) == 0) {
        sprintf(serverMess, "%d|Cannot show dashboard\n", QUERY_FAIL);
        send(socket, serverMess, strlen(serverMess), 0);
    }
    else {
        MYSQL_ROW  row;
        while ( row = mysql_fetch_row(result)){
            sprintf(temp,"%d|%s|%s|\n",DASHBOARD_INFO, row[0], row[1]);
            strcat(serverMess, temp);
        }
        printf("%s\n", serverMess);
    }
    send(socket, serverMess, strlen(serverMess), 0);
}

int answerQuestion(char* message, int socket){
    int question_id;
    char answer[2], trueAnswer[2];
    char* token;
    char serverMess[1024] = "\0";
    char query[200] = "\0";
    RESPONSE_CODE code;
    char temp[1024];

    token = strtok(message, "|");
    token = strtok(NULL, "|");
    question_id = atoi(token);
    token = strtok(NULL, "|");
    strcpy(answer, token);

    sprintf(query, "SELECT * from questions where id = %d",question_id);
    printf("%s\n", query);
    if (mysql_query(con, query)) {
        sprintf(serverMess, "%d|%s\n", QUERY_FAIL, mysql_error(con));
        send(socket, serverMess, strlen(serverMess), 0);
        return 0;
    }
    MYSQL_RES* result = mysql_store_result(con);
    if (mysql_num_rows(result) == 0) {
        sprintf(serverMess, "%d|Question not found\n", QUERY_FAIL);
        printf("Cannot find questions \n");
        send(socket, serverMess, strlen(serverMess), 0);
        return 0;
    }
    else {
        MYSQL_ROW row;
        row = mysql_fetch_row(result);
//        sprintf(question, "%d|%s|%s|%s|%s|%s\n", atoi(row[0]), row[1], row[2], row[3], row[4], row[5]);
        if (strcmp(row[6], answer) == 0){
            code = ANSWER_CORRECT;
            sprintf(serverMess, "%d|Answer correct|");
        }
        else
        {
            code = END_GAME;
            sprintf(serverMess, "%d|Answer incorrect|");
        }

        send(socket, serverMess, strlen(serverMess), 0);
    }

}

int sendQuestion(char* message, int socket){
    int position;
    printf("Start send question \n");
    char serverMess[1024] = "\0";
    char query[200] = "\0";

    char temp[1024];
    char* token;
    char trueAnswer[2], question[1024];
    int sendBytes, highScore;
    int level;
    REQUEST_CODE type;

    // Get position
    printf("%s\n", message);
    token = strtok(message, "|");
    token = strtok(NULL, "|");
    strcpy(temp, token);
    position = atoi(temp);
    printf("Position %d", position);
    // Query
    if (position >=0 && position <5)
        level = 1;
    else if (position >= 5 & position < 10)
        level = 2;
    else level = 3;

    sprintf(query, "SELECT * from questions where level = %d order by RAND() limit 1",level);
    printf("%s\n", query);
    if (mysql_query(con, query)) {
        sprintf(serverMess, "%d|%s\n", QUERY_FAIL, mysql_error(con));
        send(socket, serverMess, strlen(serverMess), 0);
        return 0;
    }
    MYSQL_RES* result = mysql_store_result(con);
    if (mysql_num_rows(result) == 0) {
        sprintf(serverMess, "%d|Question not found\n", QUERY_FAIL);
        printf("Cannot find questions \n");
        send(socket, serverMess, strlen(serverMess), 0);
        return 0;
    }
    else {
        MYSQL_ROW row;
        row = mysql_fetch_row(result);
        sprintf(question, "%d|%s|%s|%s|%s|%s\n", atoi(row[0]), row[1], row[2], row[3], row[4], row[5]);
    }
    // Store and send question
    sprintf(serverMess, "%d|%s|", QUESTION, question);
    sendBytes = send(socket, serverMess, strlen(serverMess), 0 );

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
//    strcpy(UserName,username );
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
    printf("%s\n", query);
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

int calculateScore(REQUEST_CODE code, int position){
    switch (code) {
        case STOP:
            return position;
            break;
        case ANSWER:
            if (position == 0)
                return 0;
            else if (position >= 1 && position < 5)
                return 1;
            else if (position >= 5 && position < 10)
                return 5;
            else if (position > 10 && position <15)
                return 10;
            else if (position == 15)
                return 15;
            break;
        default:
            return 0 ;
            break;
    }
}

//int getHighScore(int socket){
//    char query[200] = "\0";
//    char serverMess[100] = "\0";
//    int highScore;
//    sprintf(query, "SELECT * from users where username='%s'",UserName);
//    printf("%s\n", query);
//    if (mysql_query(con, query)) {
//        printf("Query fail\n");
//        sprintf(serverMess, "%d|%s\n", QUERY_FAIL, mysql_error(con));
//        send(socket, serverMess, strlen(serverMess), 0);
//        return 0;
//    }
//    MYSQL_RES* result = mysql_store_result(con);
//
//    printf("Number of row %d\n", mysql_num_rows(result) );
//    MYSQL_ROW row;
//    if (row = mysql_fetch_row(result)){
//        printf("Score %s\n", row[3]);
//        highScore = atoi(row[3]);
//    }
//    else
//        printf("Cannot fetch\n");
//    return highScore;
//}