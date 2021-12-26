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

#define PORT 9999
#define BACKLOG 20
#define BUFF_SIZE 1024

#include "protocol.h"
#include "serverFunction.h"

MYSQL* con;

/* Handler process signal*/
void sig_chld(int signo);

/*
 * Receive and echo message to client
 * [IN] sockfd: socket descriptor that connects to client
 */
void echo(int sockfd);

// Remember to use -pthread when compiling this server's source code
void* connection_handler(void*);

int main(int argc, char* argv[]) {
    con = mysql_init(NULL);

    if (con == NULL) {
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);
    }

    if (mysql_real_connect(con, "localhost", "root", "", "test", 0, NULL, 0) ==
        NULL) {
        finish_with_error(con);
    }

    int listen_sock, conn_sock; /* file descriptors */
    struct sockaddr_in server;  /* server's address information */
    struct sockaddr_in client;  /* client's address information */
    pid_t pid;
    int sin_size;

    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) ==
        -1) { /* calls socket() */
        printf("socket() error\n");
        return 0;
    }

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr =
            htonl(INADDR_ANY); /* INADDR_ANY puts your IP address automatically */

    if (bind(listen_sock, (struct sockaddr*)&server, sizeof(server)) == -1) {
        perror("\nError: ");
        return 0;
    }

    if (listen(listen_sock, BACKLOG) == -1) {
        perror("\nError: ");
        return 0;
    }

    /* Establish a signal handler to catch SIGCHLD */
    signal(SIGCHLD, sig_chld);

    while (1) {
        sin_size = sizeof(struct sockaddr_in);
        if ((conn_sock = accept(listen_sock, (struct sockaddr*)&client,
                                (socklen_t*)&sin_size)) == -1) {
            if (errno == EINTR)
                continue;
            else {
                perror("\nError: ");
                return 0;
            }
        }
        /* For each client, fork spawns a child, and the child handles the new
         * client */
        pid = fork();

        /* fork() is called in child process */
        if (pid == 0) {
            close(listen_sock);
            printf("You got a connection from %s\n",
                   inet_ntoa(client.sin_addr)); /* prints client's IP */
            echo(conn_sock);
            exit(0);
        }

        /* The parent closes the connected socket since the child handles the new
         * client */
        close(conn_sock);
    }
    close(listen_sock);
    mysql_close(con);
    return 0;
}

void sig_chld(int signo) {
    pid_t pid;
    int stat;

    /* Wait the child process terminate */
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("\nChild %d terminated\n", pid);
}

void echo(int sockfd) {
    char buff[BUFF_SIZE];
    int bytes_received;
    int state = NOT_AUTH;
    // recv file name
    bytes_received = recv(sockfd, buff, BUFF_SIZE, 0);
    if (bytes_received < 0) {
        perror("recv: ");
        return;
    }
    buff[bytes_received] = '\0';
    printf("%s\n", buff);
    state = handle_message(buff, sockfd, state);

    close(sockfd);
}
