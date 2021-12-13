#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "clientFunction.h"
#include "constain.h"

#define PORT 5550

int main(int argc, const char* argv[]) {
  int state = NOT_AUTH;
  // create a socket
  int network_socket;
  network_socket = socket(AF_INET, SOCK_STREAM, 0);

  // specify an address for the socket
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(PORT);
  server_address.sin_addr.s_addr = INADDR_ANY;

  int connection_status =
      connect(network_socket, (struct sockaddr*)&server_address,
              sizeof(server_address));
  // check for connection_status
  if (connection_status == -1) {
    printf("The connection has error\n\n");
  }

  int continuer = 1;

  if (connection_status == 0) {
    char response[256] = "\0";
    // receive data from the server
    recv(network_socket, &response, sizeof(response), 0);
    printf("%s\n", response);
    int sent_status = 0;

    while (continuer) {
      // print menu
      int choice;
      choice = getUserChoice(state);

      switch (choice) {
        case LOGIN:
          state = login(network_socket, state);
          break;

        case REGISTER:
          state = signup(network_socket, state);
          break;

        case LOGOUT:
          state = logout(network_socket, state);
          break;

        case JOIN_GAME:
          state = playgame(network_socket, state);
          break;

        case EXIT:
        default:
          // send q or Q to disconnect
          sent_status = send(network_socket, "q", sizeof("q"), 0);
          if (sent_status == -1) {
            printf("The data has error\n\n");
          }  // check for sent_status
          continuer = 0;
          break;
      }
    }
  }

    close(network_socket);
}
