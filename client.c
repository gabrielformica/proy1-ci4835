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

void error();
void mfree();
void *reading_stdin();
pthread_mutex_t mutex;
char *username, *comfile;
bool cfile = false;
bool quit_request = false;

int main(int argc, char *argv[]) {
   signal(SIGINT,SIG_IGN);  //Ctrl-C is ignore

   int sockfd, portno, charsno;
   char opts, *host, buffer[256];
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
   if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
      error("Could not connect");
   }
	printf("-------------------\n");
   printf("-You are connected-\n");
	printf("-------------------\n");

   pthread_mutex_init(&mutex,NULL); //Initialize mutex

   pthread_t pthread_id;
   if (pthread_create(&pthread_id, NULL, reading_stdin, (void *)&sockfd)) {
      error("Could not create thread");
   }

   while(1) {
      bzero(buffer,256);
      charsno = recv(sockfd, buffer, 256,0);
      pthread_mutex_lock(&mutex);
      if (charsno < 0) {
         error("Error reading from sockets");
      } else if (charsno == 0) {
         close(sockfd);
         mfree(host);
         mfree(comfile);
         mfree(username);
         quit_request = true;
         /* printf ("Esperando join...\n"); */
         /* pthread_join(pthread_id, NULL); */
         /* printf ("Romp\n"); */

         exit(0);
      }
      
      printf ("%s\n",buffer);
      pthread_mutex_unlock(&mutex);
   }
	

   /*
     close(sockfd);
     free(host);
     free(comfile); 
     free(username);
   */
   free(username);
}

void error(char *msg) {
	perror(msg);
	exit(1);
}


void *reading_stdin(void *sockfd) {
   int sock = *(int *) sockfd;
   char message[256];
   FILE * fp;
   char * line;
   size_t len = 0;
   ssize_t read;

   bzero(message, 256);
   write(sock, username, strlen(username));
   if (cfile) {
      fp = fopen(comfile, "r");
      if ( fp  == NULL ) {
         perror("Error opening file");
      }
      
      while ((read = getline(&line, &len, fp)) != -1) {
         pthread_mutex_lock(&mutex);
         write(sock, line, 256);
         pthread_mutex_unlock(&mutex);
      }
      mfree(line);
      fclose(fp);
   }
      
   while(!quit_request) {
      fgets(message, 256, stdin);
      pthread_mutex_lock(&mutex);
      write(sock, message, 256);
      bzero(message, 256);
      pthread_mutex_unlock(&mutex);
   }
   pthread_exit(0);
}

void mfree(void * p) {
   if (p)
      free;
}
