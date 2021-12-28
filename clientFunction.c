#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "clientFunction.h"
#include "protocol.h"


void initMenu(){
    printf("-----------------------\n");
    printf("1. Login\n");
    printf("2. Register\n");
    printf("3. Exit\n");
    printf("-----------------------\n");
}

void authMenu(){
    printf("-----------------------\n");
    printf("1. Join game\n");
    printf("2. Logout\n");
    printf("3. See dashboard\n");
    printf("4. Exit\n");
    printf("-----------------------\n");

}

void answerMenu(){
    printf("-----------------------\n");
    printf("1. Answer\n");
    printf("2. Help\n");
    printf("3. End game\n");

}

//RESPONSE_CODE extractServerMessage(char* response, char *message) {
//  RESPONSE_CODE code;
//  char* token = strtok(response, "|");
//  code = token[0] - '0';
//  strcpy(message, strtok(NULL, "|"));
//
//  return code;
//}

void getString(char* label, char* des) {
  printf("%s", label);
  scanf("%[^\n]s", des);
  getchar();
}

void createMessage(char* buffer, int type, char* data1, char* data2) {
  switch (type) {
    case LOGIN:
        sprintf(buffer, "%d|%s|%s", type, data1, data2);
        break;
    case REGISTER:
      sprintf(buffer, "%d|%s|%s", type, data1, data2);
      break;
    case LOGOUT:
      sprintf(buffer, "%d", type);
      break;
      case JOIN_GAME:
          sprintf(buffer, "%d",type);
          break;
      case ANSWER:
          sprintf(buffer, "%d|%s", type, data1);
          break;
      case QUESTION_REQUEST:
          sprintf(buffer, "%d", type);
          break;
    default:
      break;
  }
}

int getUserChoice(int state) {
  // print menu
  switch(state){
      case NOT_AUTH:
      {
          initMenu();
          printf("Enter your choice: ");
          int choice;
          scanf("%d", &choice);
          getchar();
          switch (choice) {
              case 1:
                  return LOGIN;
                  break;
              case 2:
                  return REGISTER;
                  break;
              case 3:
              default:
                  return EXIT;
                  break;
          }
          break;
      }
      case AUTH:
      {
          authMenu();
          printf("Enter your choice: ");
          int choice;
          scanf("%d", &choice);
          getchar();
          switch (choice) {
              case 1:
                  return JOIN_GAME;
                  break;
              case 2:
                  return LOGOUT;
                  break;
              case 3:
                  return DASHBOARD;
                  break;
              case 4:
              default:
                  return EXIT;
                  break;
          }
          break;
      }
//      case IN_GAME:
//      {
//          break;
//      }
      default:
//          return NOT_AUTH;
          break;
  }

}

int login(int network_socket, int state) {
  printf("\n----------- Login --------------\n");
  char username[256] = "\0";
  char password[256] = "\0";
  char buffer[256] = "\0";
  char response[256] = "\0";
  int sent_status = 0;

  getString("Enter your username: ", username);
  if (strcmp(username, "q") == 0) {
    return state;
  }
  getString("Enter your password: ", password);
  // send data to the server
    sprintf(buffer, "%d|%s|%s", LOGIN, username, password);
  sent_status = send(network_socket, buffer, sizeof(buffer), 0);
  if (sent_status == -1) {
    printf("The data has error\n\n");
  }  // check for sent_status

  // receive data from the server
  int n = recv(network_socket, response, sizeof(response), 0);
  response[n] = '\0';
    printf("%s\n", response);
    char* token;
    token = strtok(response, "|");
    if (atoi(token)== LOGIN_SUCCESS){
      printf("Login success\n");
      return AUTH;
  } else {
//    return login(network_socket, state);
      return state;
  }

}
int signup(int network_socket, int state) {
  printf("\n----------- Register --------------\n");
  char username[256] = "\0";
  char password[256] = "\0";
  char re_password[256] = "\0";
  char buffer[256] = "\0";
  char response[256] = "\0";
  int sent_status = 0;
  getString("Enter your username: ", username);
  if (strcmp(username, "q") == 0) {
    return NOT_AUTH;
  }
  getString("Enter your password: ", password);
  getString("Re-enter your password: ", re_password);

  if (strcmp(password, re_password) != 0) {
    printf("The password is not match\n\n");
    return signup(network_socket, state);
  }
  createMessage(buffer, REGISTER, username, password);
  sent_status = send(network_socket, buffer, sizeof(buffer), 0);
  if (sent_status == -1) {
    printf("The data has error\n\n");
  }  // check for sent_status

  // receive data from the server
    int n = recv(network_socket, response, sizeof(response), 0);
    response[n] = '\0';
    printf("%s\n", response);
    char* token;
    token = strtok(response, "|");
    if (atoi(token) == REGISTER_SUCCESS) {
    return AUTH;
  } else {
    return state;
  }
}
int logout(int network_socket, int state) {
	printf("\n----------- Logout --------------\n");
  char buffer[256] = "\0";
  char response[256] = "\0";
  int sent_status = 0;
  createMessage(buffer, LOGOUT, NULL, NULL);
  sent_status = send(network_socket, buffer, sizeof(buffer), 0);
  if (sent_status == -1) {
    printf("The data has error\n\n");
  }

  // recevie data from the server
  recv(network_socket, &response, sizeof(response), 0);
}

int playgame(int network_socket, int state) {
    printf("---------GAME START---------\n");
    char buffer[256] = "\0";
    char response[256] = "\0";
    int sent_status = 0;
    int recv_bytes;
    RESPONSE_CODE type ;

    createMessage(buffer, JOIN_GAME, NULL, NULL);
    sent_status = send(network_socket, buffer, sizeof(buffer), 0);
    if (sent_status <= 0) {
        printf("The data has error\n\n");
    }
    recv_bytes = recv(network_socket, response, sizeof(response), 0);
    response[recv_bytes] = '\0';
    printf("%s\n", response);
    do {
        printf("Start sending request\n");
        createMessage(buffer, QUESTION_REQUEST, NULL, NULL);
        sent_status = send(network_socket, buffer, sizeof(buffer), 0);
        if (sent_status <= 0) {
            printf("The data has error\n\n");
        }
        recv_bytes = recv(network_socket, response, sizeof(response), 0);
        response[recv_bytes] = '\0';
//        printf("%s\n", response);
        printf("Question \n");
        printf("%s\n", response);
        char *token = strtok(response, "|");
        type = atoi(token);
//        printf("Response type %d\n", type);

        answerMenu();
        int choice;
        char answer[2];
        printf("Enter your choice \n");
        scanf("%d", &choice);
        switch (choice) {
            case 1: {
                printf("Enter your answer\n");
                getchar();
                scanf("%s", answer);
//                printf("Answer %s", answer);
                sprintf(buffer, "%d|%s", ANSWER, answer);
//                printf("Send message %s\n", buffer);
                sent_status = send(network_socket, buffer, sizeof(buffer), 0);
                if (sent_status <= 0) {
                    printf("The data has error\n\n");
                }
                int n = recv(network_socket, response, sizeof(response), 0);
                response[n] = '\0';
                printf("%s\n", response);
                type = atoi(strtok(response, "|"));
                break;
            }
            case 2: {
                sprintf(buffer, "%d|", HELP);
                sent_status = send(network_socket, buffer, sizeof(buffer), 0);
                if (sent_status <= 0) {
                    printf("The data has error\n\n");
                }
                int n = recv(network_socket, response, sizeof(response), 0);
                response[n] = '\0';
                type = atoi(strtok(response, "|"));
                printf("%s\n", strtok(NULL, "+"));
                break;
            }
            case 3:{
                sprintf(buffer, "%d|", STOP);
                sent_status = send(network_socket, buffer, sizeof(buffer), 0);
                if (sent_status <= 0) {
                    printf("The data has error\n\n");
                }
                int n = recv(network_socket, response, sizeof(response), 0);
                response[n] = '\0';
                printf("%s\n", response);
                type = atoi(strtok(response, "|"));
                break;
            }
        }
    } while (type != END_GAME);

    return AUTH;
}
