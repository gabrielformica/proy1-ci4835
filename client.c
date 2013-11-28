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

void error();
void mfree();
void *reading_stdin();
pthread_mutex_t mutex;
char *username;
bool cfile = false;
char *comfile;

int main(int argc, char *argv[]) {
       

   int sockfd, portno, charsno;
   char opts, *host, buffer[256];
   struct sockaddr_in serv_addr;	
   struct hostent *server;

        
   while ((opts = getopt(argc, argv, ":h:p:n:a:")) != -1) {
      switch (opts) {
         int size;
      case 'h': 
         if ((host = (char *) malloc(sizeof(char)*strlen(optarg))) == NULL) {
            perror("Error malloc host");
         }
         host = optarg;
         printf("Flag -h, with parameter %s\n",host);
         break;
      case 'p':
         portno = atoi(optarg);
         printf("Flag -p, with parameter %d\n",portno);
         break;
      case 'n': 
         size = sizeof(char)*strlen(optarg);
         if ((username = (char *) malloc(size)) == NULL) {
            perror("Error malloc username");
            exit(1);
         }
         username = optarg;
         printf("Flag -n, with paremeter %s\n",username);
         break;
      case 'a': 
         size = sizeof(char)*strlen(optarg);
         if ((comfile = malloc(size)) == NULL) {
            perror("Error malloc comfile");
            exit(1);
         }
         comfile = optarg;
         printf("Flag -a, with paremeter %s\n",comfile);
         cfile = true;
         break;
      case '?':
         printf("ERROR with flags\n");
      }
   }

   if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
      error("ERROR opening socket");

   if ((server = gethostbyname(host)) == NULL) 
      error("ERROR no such host");
	
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
   serv_addr.sin_port = htons(portno);

   /* Se hace la conexion */
   if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
      error("ERROR connecting");
   }
   printf("You are connected\n");

   /* se inicia el mutex */
   pthread_mutex_init(&mutex,NULL);

   pthread_t pthread_id;
   if (pthread_create(&pthread_id, NULL, reading_stdin, (void *)&sockfd)) {
      perror("Could not create thread");
      exit(1);
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
         exit(0);
      }
      
      if (strcmp(buffer, "~~end") == 0)
         exit(0);
      printf ("%s\n",buffer);
      pthread_mutex_unlock(&mutex);
   }
	

   /*
     close(sockfd);
     free(host);
     free(comfile); 
     free(username);
   */
}

void error(char *msg) {
	perror(msg);
	exit(0);
}


void *reading_stdin(void *sockfd) {
   int sock = *(int *) sockfd;
   char message[256];
   FILE * fp;
   char * line;
   size_t len = 0;
   ssize_t read;
   
   write(sock, username, strlen(username));
   if (cfile) {
      fp = fopen(comfile, "r");
      if ( fp  == NULL ) {
         perror("Error opening file");
      }
      
      while ((read = getline(&line, &len, fp)) != -1) {
         /* line[read-1] = 0; */
         pthread_mutex_lock(&mutex);
         write(sock, line, 256);
         pthread_mutex_unlock(&mutex);
         /* printf ("Line of size: %d\n", read); */
         /* printf ("Line: -%s- \n", line); */
      }
      if (line)
         free(line);
   }
      
   while(1) {
      fgets(message,256,stdin);
      pthread_mutex_lock(&mutex);
      write(sock,message,256);
      bzero(message,256);
      pthread_mutex_unlock(&mutex);
   }
}

void mfree(void * p) {
   if (p)
      free;
}
