/**
  * @file
  * @author Gabriel Formica <gabriel@ac.labf.usb.ve>
  * @author Melecio Ponte <melecio@ac.labf.usb.ve> 
  *
  * @section Descripcion
  *
  * Client code of the chat
  */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>
#include <stdbool.h>
#include <signal.h>
#define MAX_PACK_SIZE 256

pthread_mutex_t mutex;
char *username, *comfile;
bool cfile = false;
bool quit_request = false;

void INThandler(int);
void error();
void *reading_stdin();

int main(int argc, char *argv[]) {

   signal(SIGINT, INThandler);  //Ctrl-C is ignore

   int sockfd, portno, charsno;
   char opts, *host, buffer[MAX_PACK_SIZE];
   struct sockaddr_in serv_addr;	
   struct hostent *server;

   
   while ((opts = getopt(argc, argv, ":h:p:n:a:")) != -1) {
      switch (opts) {
         int size;
      case 'h': 
         if ((host = (char *) malloc(sizeof(char)*strlen(optarg))) == NULL) {
            error("Malloc failed");
         }
         host = optarg;
         break;
      case 'p':
         portno = atoi(optarg);
         break;
      case 'n': 
         size = sizeof(char)*strlen(optarg);
         if ((username = (char *) malloc(size)) == NULL) {
            error("Malloc failed");
         }
         username = optarg;
         break;
      case 'a': 
         size = sizeof(char)*strlen(optarg);
         if ((comfile = malloc(size)) == NULL) {
            error("Malloc failed");
         }
         comfile = optarg;
         cfile = true;
         break;
      case '?':
         printf("Unrecognized option. Try again.\n");
			exit(1);
      }
   }
   if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
      error("Could not open socket");

   if ((server = gethostbyname(host)) == NULL) 
      error("No such host");
	
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
   serv_addr.sin_port = htons(portno);

	//Connecting
   if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      error("Could not connect");
   }
	printf("-------------------\n");
   printf("-You are connected-\n");
	printf("-------------------\n");

   pthread_mutex_init(&mutex, NULL); //Initialize mutex

   pthread_t pthread_id;
   if (pthread_create(&pthread_id, NULL, reading_stdin, (void *)&sockfd)) {
      error("Could not create thread");
   }

   while(1) {
      bzero(buffer, MAX_PACK_SIZE);
      charsno = recv(sockfd, buffer, MAX_PACK_SIZE, 0);
      pthread_mutex_lock(&mutex);
      if (charsno < 0) {
         error("Error reading from sockets");
      } 
		else if (charsno == 0) {
			break;	
      }
      printf ("\n%s\n", buffer);
      pthread_mutex_unlock(&mutex);
   }

	close(sockfd);
}

/**
  * Send an error message and terminate the program
  * @param msg: msg to be sended
  */

void error(char *msg) {
	perror(msg);
	exit(1);
}

/**
  * Reads from standard input.
  * @param sockfd: Socket to which the input will be written.
  * @return Nothing.
  */

void *reading_stdin(void *sockfd) {
   int sock = *(int *) sockfd;
   char message[MAX_PACK_SIZE];
   FILE *fp;
   char *line;
   size_t len = 0;
   ssize_t read;

   bzero(message, MAX_PACK_SIZE);
   write(sock, username, strlen(username));
   if (cfile) {
      fp = fopen(comfile, "r");
      if ( fp  == NULL ) {
         perror("Error opening file");
      }
      
      while ((read = getline(&line, &len, fp)) != -1) {
         pthread_mutex_lock(&mutex);
         write(sock, line, MAX_PACK_SIZE);
         pthread_mutex_unlock(&mutex);
      }
      free(line);
      fclose(fp);
   }
      
   while(!quit_request) {
      fgets(message, MAX_PACK_SIZE, stdin);
      pthread_mutex_lock(&mutex);
      write(sock, message, MAX_PACK_SIZE);
      bzero(message, MAX_PACK_SIZE);
      pthread_mutex_unlock(&mutex);
   }
   pthread_exit(0);
}


/**
  * Handles the Ctrl-C signal.
  * @param sig: Integer with signal number.
  * @return Nothing.
  */

void INThandler(int sig) {
   printf (" is not the correct way to quit.\n"
           "Use \"fue\" instead\n");
}
