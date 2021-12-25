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
#define PORT 5550

GtkWidget *window; // Window
GtkLabel *mylabel; // Label

GtkWidget *button1;
GtkWidget *button2;
GtkWidget *button3;

GtkWidget *label1;
GtkWidget *label2;
GtkEntry *entry1;
GtkEntry *entry2;

int network_socket;
STATE state = NOT_AUTH;

void exit_app()
{
	printf("Exit app \n"); // Not neccesary
	gtk_main_quit(); // Command to quit a GTK program
}

int login_clicked() {
    Request *request = (Request*) malloc (sizeof(Request));
    Response *response = (Response*) malloc (sizeof(Response));

    // create a socket
    
    //network_socket = socket(AF_INET, SOCK_STREAM, 0);

    //char response[256] = "\0";
        // receive data from the server
//    recv(network_socket, &response, sizeof(response), 0);
    char res[100] ;
    // receive data from the server
    int n = recv(network_socket, res, sizeof(res), 0);
    if (n <= 0)
        printf("Cannot receive \n");
    printf("Response from server \n");
    printf("%s\n", res);
    //int sent_status = 0;


  printf("\n----------- Login --------------\n");
  char username[256] = "\0";
  char password[256] = "\0";
  int sent_status = 0;

  strcpy(username,gtk_entry_get_text(entry1));
  strcpy(password,gtk_entry_get_text(entry2));

  printf ("%s\n",username);
  printf ("%s\n",password);

  if (strcmp(username, "q") == 0) {
    return state;
  }
  //getString("Enter your password: ", password);
  // send data to the server
  request->code = LOGIN;
  sprintf(request->message, "%s|%s", username, password);
  sent_status = sendRequest(network_socket, request, sizeof(Request), 0);
  if (sent_status == -1) {
    printf("The data has error\n\n");
  }
//    printf("Sent %d\n", sent_status);// check for sent_status

  // receive data from the server
  receiveResponse(network_socket, response, sizeof(Response), 0);
  printf("%s\n", response->message);
  if (response->code == LOGIN_SUCCESS) {
    return AUTH;
  } else {
      return state;
  }
    return 1;

}
void button1_clicked()
{
    GtkBuilder *builder;
	printf("Log In \n");

	builder = gtk_builder_new(); // Create GTK UI Builder
	gtk_builder_add_from_file(builder, "login.glade", NULL); // Load our UI file

	// Assign the Variables
	window = GTK_WIDGET(gtk_builder_get_object(builder, "MyWindow")); // Load our window named MyWindow
	mylabel = GTK_LABEL(gtk_builder_get_object(builder, "MyLabel")); // Load our label named MyLabel

    label1 = GTK_WIDGET(gtk_builder_get_object(builder, "Label1"));
    label2 = GTK_WIDGET(gtk_builder_get_object(builder, "Label2"));

    entry1 = GTK_ENTRY(gtk_builder_get_object(builder, "entry1"));
    entry2 = GTK_ENTRY(gtk_builder_get_object(builder, "entry2"));

    button2 = GTK_WIDGET(gtk_builder_get_object(builder, "button2"));

//    char res[100] ;
//    // receive data from the server
//    int n = recv(network_socket, res, sizeof(res), 0);
//    if (n <= 0)
//        printf("Cannot receive \n");
//    printf("Response from server \n");
//    printf("%s\n", res);

    // Essential for a GTK based program
	gtk_builder_connect_signals(builder, NULL); 
	g_object_unref(builder);
	
	gtk_widget_show_all(window); // Show our window

}

int main(int argc, const char* argv[]) {

    // create a socket

    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    GtkBuilder *builder; // GTK Builder variable
	gtk_init(&argc, &argv); // Start GTK

	builder = gtk_builder_new(); // Create GTK UI Builder
	gtk_builder_add_from_file(builder, "homepage.glade", NULL); 

    // specify an address for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int connection_status =
            connect(network_socket, (struct sockaddr *) &server_address,
                    sizeof(server_address));
    // check for connection_status
    if (connection_status == -1) {
        printf("The connection has error\n\n");
    }




    window = GTK_WIDGET(gtk_builder_get_object(builder, "MyWindow")); // Load our window named MyWindow
	mylabel = GTK_LABEL(gtk_builder_get_object(builder, "MyLabel")); // Load our label named MyLabel
    
    button1 = GTK_WIDGET(gtk_builder_get_object(builder, "button1")); 

    gtk_builder_connect_signals(builder, NULL); 
	g_object_unref(builder);

    gtk_widget_show_all(window); // Show our window
	gtk_main(); // Run our program


    /*int continuer = 1;

    if (connection_status == 0) {
        char response[256] = "\0";
        // receive data from the server
        recv(network_socket, &response, sizeof(response), 0);
        printf("%s\n", response);
        int sent_status = 0;


        while (continuer) {
            // print menu

            switch (state) {
                case NOT_AUTH: {
                    initMenu();
                    printf("Enter your choice: ");
                    int choice;
                    scanf("%d", &choice);
                    getchar();
                    switch (choice) {
                        case 1:
                            state = login(network_socket, state);
                            break;
                        case 2:
                            state = signup(network_socket, state);
                            break;
                        case 3:
                        default:
                            sent_status = send(network_socket, "q", sizeof("q"), 0);
                            if (sent_status == -1) {
                                printf("The data has error\n\n");
                            }  // check for sent_status
                            continuer = 0;
                            break;
                            break;
                    }
                    break;
                    
                   
                }
                 authMenu();
                    printf("Enter your choice: ");
                    int choice;
                    scanf("%d", &choice);
                    getchar();
                    switch (choice) {
                        case 1:
                            state = playgame(network_socket, state);
                            break;
                        case 2:
                            state = logout(network_socket, state);
                            break;
                        case 3:
                            printf("Feature is developing\n");
                            break;
                        case 4:
                        default:
                            sent_status = send(network_socket, "q", sizeof("q"), 0);
                            if (sent_status == -1) {
                                printf("The data has error\n\n");
                            }  // check for sent_status
                            continuer = 0;
                            break;
                    }
                    break;
                }
                case IN_GAME: {
                    break;
                }
                default:
                    break;
            }
        }
    }*/
    close(network_socket);
}
