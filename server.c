#include <mysql/mysql.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <errno.h>


#define PORT 5550
#define BACKLOG 20

#include "serverFunction.h"
#include "protocol.h"

MYSQL* con;
void sig_chld(int signo);

void echo(int sockfd);

int main(){
    con = mysql_init(NULL);

    if (con == NULL) {
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);
    }

    if (mysql_real_connect(con, "localhost", "root", "", "project", 0, NULL, 0) ==
        NULL) {
        finish_with_error(con);
    }

    int listen_sock, conn_sock; /* file descriptors */
    struct sockaddr_in server; /* server's address information */
    struct sockaddr_in client; /* client's address information */
    pid_t pid;
    int sin_size;


    int num;
    if ((listen_sock=socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
        printf("socket() error\n");
        return 0;
    }

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY puts your IP address automatically */

    if(bind(listen_sock, (struct sockaddr*)&server, sizeof(server))==-1){
        perror("\nError: ");
        return 0;
    }

    if(listen(listen_sock, BACKLOG) == -1){
        perror("\nError: ");
        return 0;
    }

    /* Establish a signal handler to catch SIGCHLD */
    signal(SIGCHLD, sig_chld);

//

    while(1){
        sin_size=sizeof(struct sockaddr_in);
        if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client, &sin_size))==-1){
            if (errno == EINTR)
                continue;
            else{
                perror("\nError: ");
                return 0;
            }
        }

        /* For each client, fork spawns a child, and the child handles the new client */
        pid = fork();

        /* fork() is called in child process */
        if(pid  == 0){
            close(listen_sock);
            printf("You got a connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */
            printf("Hello\n");
            echo(conn_sock);
            exit(0);
        }

        /* The parent closes the connected socket since the child handles the new client */
        close(conn_sock);
    }
    mysql_close(con);
    close(listen_sock);
    return 0;
}
void sig_chld(int signo){
    pid_t pid;
    int stat;

    /* Wait the child process terminate */
    while((pid = waitpid(-1, &stat, WNOHANG))>0)
        printf("\nChild %d terminated\n",pid);
}

void echo(int sockfd) {
  STATE state = NOT_AUTH;
  Request *request = (Request*) malloc (sizeof(Request));
//  Response *response = (Response*) malloc (sizeof(Response));
  int read_len = 0;
  char server_message[100] = "Hello from server\n";
  int send_status = 0;
  send_status = send(sockfd, server_message, sizeof(server_message), 0);

  char client_message[100] = "\0";
  while ((receiveRequest(sockfd, request, sizeof(Request), 0)) > 0) {
    request->message[strlen(request->message)] = '\0';
    if (request->code == EXIT) {
      printf("Client disconnected\n");
      break;

    }
      printf("Recieved\n");
    state = handle_message(request, sockfd, state);

    memset(client_message, 0, sizeof(client_message));
  }
  close(sockfd);
}
