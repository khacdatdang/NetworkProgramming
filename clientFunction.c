#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <gtk/gtk.h> 

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

STATE extractServerMessage(Response* response) {
    switch (response->code) {
        case LOGIN_SUCCESS:
            return AUTH;
        case JOIN_GAME_SUCCESS:
            return IN_GAME;
        case LOGOUT_SUCCESS:
            return NOT_AUTH;
        default:
            return NOT_AUTH;

    }
}

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
    default:
      break;
  }
}

//int getUserChoice(int state) {
//  // print menu
//  switch(state){
//      case NOT_AUTH:
//      {
//          initMenu();
//          printf("Enter your choice: ");
//          int choice;
//          scanf("%d", &choice);
//          getchar();
//          switch (choice) {
//              case 1:
//                  return LOGIN;
//                  break;
//              case 2:
//                  return REGISTER;
//                  break;
//              case 3:
//              default:
//                  return EXIT;
//                  break;
//          }
//          break;
//      }
//      case AUTH:
//      {
//          authMenu();
//          printf("Enter your choice: ");
//          int choice;
//          scanf("%d", &choice);
//          getchar();
//          switch (choice) {
//              case 1:
//                  return JOIN_GAME;
//                  break;
//              case 2:
//                  return LOGOUT;
//                  break;
//              case 3:
//                  return DASHBOARD;
//                  break;
//              case 4:
//              default:
//                  return EXIT;
//                  break;
//          }
//          break;
//      }
//      case IN_GAME:
//      {
//          break;
//      }
//      default:
//          break;
//  }
//
//}

int signup(int network_socket, int state) {
  printf("\n----------- Register --------------\n");
  Request *request = (Request*) malloc (sizeof(Request));
  Response *response = (Response*) malloc (sizeof(Response));
  char username[256] = "\0";
  char password[256] = "\0";
  char re_password[256] = "\0";
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

  request->code = REGISTER;
  sprintf(request->message, "%s|%s", username, password);
  sent_status = sendRequest(network_socket, request, sizeof(Request), 0);
  if (sent_status == -1) {
    printf("The data has error\n\n");
  }  // check for sent_status

  // receive data from the server
  receiveResponse(network_socket, response, sizeof(Response), 0);
    printf("%s\n", response->message);
  if (response->code == REGISTER_SUCCESS) {
    return AUTH;
  } else {
    return state;
  }
}
int logout(int network_socket, int state) {
	printf("\n----------- Logout --------------\n");
    Request *request = (Request*) malloc (sizeof(Request));
    Response *response = (Response*) malloc (sizeof(Response));
  int sent_status = 0;
  request->code = LOGOUT;
    sent_status = sendRequest(network_socket, request, sizeof(Request), 0);

  if (sent_status == -1) {
    printf("The data has error\n\n");
  }

  // recevie data from the server
    receiveResponse(network_socket, response, sizeof(Response), 0);
    printf("%s\n", response->message);
    if (response->code == LOGOUT_SUCCESS) {
        return NOT_AUTH;
    } else {
        return state;
    }
}

int playgame(int network_socket, int state) {
    printf("---------GAME START---------\n");
    return 1;
}
