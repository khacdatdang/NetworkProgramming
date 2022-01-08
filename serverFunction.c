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
#define BUFF_SIZE 1024

extern MYSQL* con;

void finish_with_error(MYSQL* con) {
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

void handle_message(char* message, int socket) {
  REQUEST_CODE type = message[0] - '0';
//  char server_message[200] = "\0";
  switch (type) {
    case LOGIN:
        printf("handle login\n");
        loginUser(message, socket);
        break;
    case REGISTER:
        printf("handle register\n");
        registerUser(message, socket);
        break;
    case LOGOUT:
        printf("Handle logout\n");
//        printf("%s\n", message);
        logoutUser(message, socket);
        break;
    case QUESTION_REQUEST: {
        printf("handle game play\n");
        sendQuestion(message, socket);
        break;
    }
    case HELP: {
        printf("handle help\n");
        helpAnswer(message, socket);
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
    case STOP:
    case BREAK:
    {
        printf("Handle score\n");
        calculateScore(message, socket, type);
        break;
    }
    default:
      break;
  }
}
void showDashboard(int socket){
    char query[BUFF_SIZE] = "\0";
    char serverMess[BUFF_SIZE] = "\0";
    char temp[100] = "\0";
    sprintf(query, "SELECT username, highScore from users ORDER BY highScore DESC");
//    printf("%s\n", query);
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
            sprintf(temp,"%d|%s|%s|",DASHBOARD_INFO, row[0], row[1]);
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
    char serverMess[BUFF_SIZE] = "\0";
    char query[200] = "\0";
    RESPONSE_CODE code;
    // Split message
    token = strtok(message, "|");
    token = strtok(NULL, "|");
    question_id = atoi(token);
    token = strtok(NULL, "|");
    strcpy(answer, token);
    printf("%s %d\n", answer, strlen(answer));

    // Query question and answer from database
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
        printf("True answer %s %d\n", row[6], strlen(row[6]));
        if (strcmp(row[6], answer) == 0){
            printf("%s\n", row[6]);
            code = ANSWER_CORRECT;
            sprintf(serverMess, "%d|Answer correct|",code);
        }
        else
        {
            code = END_GAME;
            sprintf(serverMess, "%d|Answer incorrect|",code);
        }
        send(socket, serverMess, strlen(serverMess), 0);
    }
}

int helpAnswer(char* message, int socket){
    char serverMess[BUFF_SIZE] = "\0";
    char* token;
    char query[BUFF_SIZE];
    char trueAnswer[2];
    token = strtok(message, "|");
    token = strtok(NULL, "|");
    int question_id = atoi(token);

    // Query question and answer from database
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
    } else {
        MYSQL_ROW row;
        row = mysql_fetch_row(result);
        strcpy(trueAnswer, row[6]);
    }
    RESPONSE_CODE code = HELP_SUCCESS;
    sprintf(serverMess, "%d|Answer of this question is %s|",code, trueAnswer);
    send(socket, serverMess, strlen(serverMess), 0);
}
int calculateScore(char* message, int socket, REQUEST_CODE code){
    char* token;
    char username[50];
    char serverMess[BUFF_SIZE] = "\0";
    int position ;
    int score;
    int highScore;
    char query[BUFF_SIZE] = "\0";
    // Get username and position
    token = strtok(message, "|");
    token = strtok(NULL, "|");
    strcpy(username, token);
    token = strtok(NULL,"|");
    position = atoi(token);

    if (code == STOP)
    {
        score = position ;
    }
    else
    {
            if (position == 0)
                score = 0;
            else if (position >= 1 && position < 5)
                score = 1;
            else if (position >= 5 && position < 10)
                score = 5;
            else if (position > 10 && position <15)
                score = 10;
            else if (position == 15)
                score = 15;
    }
    // Get username from database
    sprintf(query, "SELECT * from users where username='%s'",username);
    printf("%s\n", query);
    if (mysql_query(con, query)) {
        printf("Query fail\n");
        sprintf(serverMess, "%d|%s\n", QUERY_FAIL, mysql_error(con));
        send(socket, serverMess, strlen(serverMess), 0);
        return 0;
    }
    MYSQL_RES* result = mysql_store_result(con);
    printf("Number of row %d\n", mysql_num_rows(result) );
    MYSQL_ROW row;
    if (row = mysql_fetch_row(result)){
        printf("Score %s\n", row[3]);
        highScore = atoi(row[3]);
    }
    else
        printf("Cannot fetch\n");
//    return highScore;
    // Update highscore in database
    if (score > highScore){
        sprintf(query, "UPDATE users SET highScore = '%d' where username='%s'",score, username);
        printf("%s\n", query);
        if (mysql_query(con, query)) {
            printf("Query fail\n");
            sprintf(serverMess, "%d|%s\n", QUERY_FAIL, mysql_error(con));
            send(socket, serverMess, strlen(serverMess), 0);
            return 0;
        }
    }
    sprintf(serverMess, "%d|%d|", SCORE_INFO, score);
    send(socket, serverMess, strlen(serverMess), 0);

}
int sendQuestion(char* message, int socket){
    printf("Start send question \n");
    char serverMess[BUFF_SIZE] = "\0";
    char query[200] = "\0";
    int position;
    char temp[BUFF_SIZE];
    char* token;
    char question[BUFF_SIZE];
    int level;

    // Get position
    printf("%s\n", message);
    token = strtok(message, "|");
    token = strtok(NULL, "|");
    strcpy(temp, token);
    position = atoi(temp);
    printf("Position %d", position);
    // Get position to choose appropriate question
    if (position >=0 && position <5)
        level = 1;
    else if (position >= 5 & position < 10)
        level = 2;
    else level = 3;

    // Get question from database
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
    send(socket, serverMess, strlen(serverMess), 0 );

}
int registerUser(char* message, int socket) {
  char username[255] = "\0";
  char password[255] = "\0";
  char serverMess[BUFF_SIZE] = "\0";
  char query[BUFF_SIZE] = "\0";
  char* token;

  // Split message to get username and password
  token = strtok(message, "|");
  token = strtok(NULL, "|");
  strcpy(username, token);
  token = strtok(NULL, "|");
  strcpy(password, token);
  encryptPassword(password);

  // Check username is existed ?
  sprintf(query, "SELECT * FROM users WHERE username = '%s' ",
            username);
    if (mysql_query(con, query)) {
        sprintf(serverMess, "%d|%s\n", QUERY_FAIL, mysql_error(con));
        send(socket, serverMess, strlen(serverMess), 0);
        return 0;
    }
    MYSQL_RES* result = mysql_store_result(con);
    if (mysql_num_rows(result)){
        sprintf(serverMess, "%d|This username is existed\n", REGISTER_USERNAME_EXISTED);
        send(socket, serverMess, strlen(serverMess), 0);
        return 0;
    }
    else {
        // Insert new account into database
        sprintf(query, "INSERT INTO users (username, password) VALUES ('%s', '%s')",
                username, password);
        if (mysql_query(con, query)) {
            sprintf(serverMess, "%d|%s\n", QUERY_FAIL, mysql_error(con));
            send(socket, serverMess, strlen(serverMess), 0);
            return 0;
        }

        // Insert this account into signing in accounts
        sprintf(query, "INSERT INTO using_accounts (username) VALUES ('%s')",username);
        if (mysql_query(con, query)) {
            sprintf(serverMess, "%d|%s\n", QUERY_FAIL, mysql_error(con));
            send(socket, serverMess, strlen(serverMess), 0);
            return 0;
        }

        sprintf(serverMess, "%d|Successfully registered\n", REGISTER_SUCCESS);
        send(socket, serverMess, sizeof(serverMess), 0);
        return 1;
    }

}

int loginUser(char* message, int socket) {
    printf("Start handle login\n");
  char username[255] = "\0";
  char password[255] = "\0";
  char serverMess[BUFF_SIZE] = "\0";
  char* token;
    char query[BUFF_SIZE] = "\0";
    // Split message to get username and password
  token = strtok(message, "|");
  token = strtok(NULL, "|");
  strcpy(username, token);
  token = strtok(NULL, "|");
  strcpy(password, token);
  encryptPassword(password);
//    printf("%s %s\n",username, password);

    // Query to validate account
    // Check username
  sprintf(query, "SELECT * from users where username='%s'",username);
  printf("%s\n", query);
  if (mysql_query(con, query)) {
    sprintf(serverMess, "%d|%s\n", QUERY_FAIL, mysql_error(con));
    send(socket, serverMess, strlen(serverMess), 0);
    return 0;
  }
  MYSQL_RES* result = mysql_store_result(con);
  if (mysql_num_rows(result) == 0) {
    sprintf(serverMess, "%d|Invalid username\n", USERNAME_NOTFOUND);
    send(socket, serverMess, strlen(serverMess), 0);
    return 0;
  }
  else {
      // Check password
      MYSQL_ROW row = mysql_fetch_row(result);
      if (strcmp(row[2], password))
      {
          sprintf(serverMess, "%d|Password is incorrect\n", PASSWORD_INCORRECT);
          send(socket, serverMess, strlen(serverMess), 0);
          return 0;
      }
      else {
          // Check account is signing in other device
          char server_message[100] = "\0";
          char temp[512];
          sprintf(query, "SELECT * from using_accounts where username='%s'",username);
          if (mysql_query(con, query)) {
              sprintf(serverMess, "%d|%s\n", QUERY_FAIL, mysql_error(con));
              send(socket, serverMess, strlen(serverMess), 0);
              return 0;
          }
          MYSQL_RES* result = mysql_store_result(con);
          if (mysql_num_rows(result) == 0){
              // Push account into signing in account table
              sprintf(query, "INSERT INTO using_accounts (username) VALUES ('%s')",username);
              mysql_query(con, query);
              sprintf(server_message, "%d|Successfully logged in\n", LOGIN_SUCCESS);
              send(socket, server_message, sizeof(server_message), 0);
              return 1;
          }
          else {
              sprintf(server_message, "%d|Your account is signing in other device\n", USERNAME_IS_SIGNIN);
              send(socket, server_message, sizeof(server_message), 0);
              return 0;
          }

      }
  }

}
int logoutUser(char* message, int socket){
    printf("Start handle logout\n");
    char username[255] = "\0";
    char server_message[BUFF_SIZE] = "\0";
    char* token;
    char query[300] = "\0";

    // Split message to get username
    token = strtok(message, "|");
    token = strtok(NULL, "|");
    strcpy(username, token);

    // Delete in database
    sprintf(query, "DELETE FROM using_accounts where username='%s'",username);
    if (mysql_query(con, query)) {
        sprintf(server_message, "%d|%s\n", QUERY_FAIL, mysql_error(con));
        send(socket, server_message, strlen(server_message), 0);
        return 0;
    }
    sprintf(server_message, "%d|\n", LOGOUT_SUCCESS);
    send(socket, server_message, strlen(server_message), 0);

    return 1;
}
void encryptPassword(char* password) {
  for (int i = 0; i < strlen(password); i++) {
    if ((int)password[i] > i) {
      password[i] = password[i] - i;
    }
  }
}
