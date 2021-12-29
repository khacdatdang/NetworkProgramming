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
int position = 0;
int helpTimes = 2;
char  questions[16][1024];
char UserName[100];
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
        printf("State :%d\n", state);
//        if (state == NOT_AUTH) {
//            sprintf(server_message, "%d|You must login first\n", JOIN_GAME_FAIL);
//            send(socket, server_message, sizeof(server_message), 0);
//            return state;
//        } else {
            if (loadGame(socket))
                return IN_GAME;
            else return AUTH;
//        }
        break;
    }
    case QUESTION_REQUEST: {
        printf("handle game play\n");
//        if (state != IN_GAME) {
//            sprintf(server_message, "%d|You must login first\n", JOIN_GAME_FAIL);
//            send(socket, server_message, sizeof(server_message), 0);
//            return state;
//        } else {
            if (playGame(socket))
                return IN_GAME;
            else return AUTH;
//        }
        break;
    }
    case DASHBOARD:{
        printf("Handle dashboard\n");
        showDashboard(socket);
        return AUTH;
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
int loadGame(int socket){
    char query[200] = "\0";
    char serverMess[1024] = "\0";
    int index = 0;
    for (int i = 1 ; i <= 3 ; i++){
        sprintf(query, "SELECT * from questions where level = %d order by RAND() limit 5",i);
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
            MYSQL_ROW  row;
            while ( row = mysql_fetch_row(result)){
                sprintf(questions[index],"%s|%s|%s|%s|%s|%s\n", row[6],row[1], row[2], row[3], row[4], row[5]);
//                printf("%s\n", questions[index]);
                index += 1;
            }
        }
    }
    sprintf(serverMess, "%d|%s\n", JOIN_GAME_SUCCESS, "Load game success\n");
    send(socket, serverMess, strlen(serverMess), 0);
    return 1;
}
int playGame(int socket){
    printf("Start send question \n");
    printf("Position %d\n", position);
    char serverMess[1024] = "\0";
    char query[200] = "\0";
    char client_message[100] = "\0";
    char temp[1024];
    char* token;
    char trueAnswer[2], question[1024];
    int sendBytes, highScore;
    REQUEST_CODE type;

    // Seperate true answer and question
    strcpy(temp, questions[position]);
    token = strtok(temp, "|");
    strcpy(trueAnswer, token);
    printf("True answer %s\n", trueAnswer);
    token = strtok(NULL, "+");
    printf("Question: %s\n", token);
    strcpy(question, token);

    // Store and send question
    sprintf(serverMess, "%d|%s", QUESTION, question);
    sendBytes = send(socket, serverMess, strlen(serverMess), 0 );

    // Receive choice from client after client received question
    int n = recv(socket, client_message, 100, 0);
    client_message[n] = '\0';
    if (n <= 0)
        printf("Cannot recieve\n");
    type = atoi(strtok(client_message, "|"));
    printf("Request code %d\n", type);

    switch (type) {
        case ANSWER: {
            char* token;
            token = strtok(NULL, "|");
            printf("Answer %s\n", token);
            if (strcmp(trueAnswer, token) == 0){
                printf("Answer true\n");
                sprintf(serverMess, "%d|%s",ANSWER_CORRECT, "You answer correct");
                send(socket, serverMess, strlen(serverMess), 0 );
                position += 1 ;
                return 1;
            }
            else {
                int highScore = getHighScore(socket);
                int score = calculateScore(type, position);
                if (score > highScore)
                {
                    sprintf(query, "UPDATE users SET highScore = %d where username='%s'",score,UserName);
                    if (mysql_query(con, query)) {
                        printf("Query fail\n");
                        sprintf(serverMess, "%d|%s\n", QUERY_FAIL, mysql_error(con));
                        send(socket, serverMess, strlen(serverMess), 0);
                        return 0;
                    }
                }
                sprintf(serverMess, "%d|%s %d", END_GAME, "Game end. Your score is :",score);
                send(socket, serverMess, strlen(serverMess), 0);
                position = 0;
                return 0;
            }
            break;
        }
        case HELP: {
            if (helpTimes > 0){
                sprintf(serverMess, "%d|%s %s\n", HELP_SUCCESS, "Use help success. This question's answer is:",trueAnswer);
                send(socket, serverMess, strlen(serverMess), 0 );
                position += 1 ;
                helpTimes--;
                return  1;
            }
            else {
                sprintf(serverMess, "%d|%s\n", HELP_FAIL, "Help fail");
                send(socket, serverMess, strlen(serverMess), 0 );
                return 1;
            }
            break;
        }
        case STOP: {
            int highScore = getHighScore(socket);
            int score = calculateScore(type, position);
            if (score > highScore)
            {
                sprintf(query, "UPDATE users SET highScore = %d where username='%s'",score,UserName);
                if (mysql_query(con, query)) {
                    printf("Query fail\n");
                    sprintf(serverMess, "%d|%s\n", QUERY_FAIL, mysql_error(con));
                    send(socket, serverMess, strlen(serverMess), 0);
                    return 0;
                }
            }
            sprintf(serverMess, "%d|%s|%d", END_GAME, "Game end. Your score is :",score);
            send(socket, serverMess, strlen(serverMess), 0);
            position = 0;
            return 0;
            break;
        }
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
    sprintf(serverMess, "%d|%s\n", QUERY_FAIL, mysql_error(con));
    send(socket, serverMess, strlen(serverMess), 0);
    return 0;
  }
  char server_message[100] = "\0";
  sprintf(server_message, "%d|Successfully registered\n", REGISTER_SUCCESS);
    strcpy(UserName,username );
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
  strcpy(UserName,username );
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

int getHighScore(int socket){
    char query[200] = "\0";
    char serverMess = "\0";
    int highScore;
    sprintf(query, "SELECT * from users where username='%s'",UserName);
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
    return highScore;
}