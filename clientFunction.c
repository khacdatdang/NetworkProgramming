#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "clientFunction.h"
#include "constain.h"


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

int extractServerMessage(char* message, int type) {
  int status = FAILURE;
  char* token = strtok(message, "|");
  if (type != token[0] - '0') {
    printf("Wrong recv message\n");
    return status;
  }

  token = strtok(NULL, "|");
  if (token[0] - 'S' == 0 || token[0] - 's' == 0) {
    status = SUCCESS;
  }
  token = strtok(NULL, "|");
  if (token != NULL) {
    printf("%s\n", token);
  }
  return status;
}

void getString(char* label, char* des) {
  printf("%s", label);
  scanf("%[^\n]s", des);
  getchar();
}

void createMessage(char* buffer, int type, char* data1, char* data2) {
  switch (type) {
    case LOGIN:
    case REGISTER:
      sprintf(buffer, "%d|%s|%s", type, data1, data2);
      break;
    case LOGOUT:
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
      case IN_GAME:
      {
          break;
      }
      default:
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
  createMessage(buffer, LOGIN, username, password);
  sent_status = send(network_socket, buffer, sizeof(buffer), 0);
  if (sent_status == -1) {
    printf("The data has error\n\n");
  }  // check for sent_status

  // receive data from the server
  recv(network_socket, &response, sizeof(response), 0);
  if (extractServerMessage(response, LOGIN) == SUCCESS) {
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
  recv(network_socket, &response, sizeof(response), 0);
  if (extractServerMessage(response, REGISTER) == SUCCESS) {
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
  if (extractServerMessage(response, LOGOUT) == SUCCESS) {
    return NOT_AUTH;
  } else {
    return state;
  }
}

int playgame(int network_socket, int state) {
    printf("---------GAME START---------\n");


}
