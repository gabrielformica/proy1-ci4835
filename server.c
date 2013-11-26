#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "list.h"
#include "roomslist.h"

void error();
void *connection_handler();
pthread_mutex_t mutex;
char client_message[2000];

int sock001, sock002;
	
int main(int argc, char *argv[]) {

	/* declare */
	char opts;
	int sockfd, client_sock, portno;
	socklen_t clilen;
	char buffer[256];
	char *defname = NULL;
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	

	if (argc < 5) {
		fprintf(stderr,"ERROR, wrong number of arguments.\n");
		exit(1);
	} 

	while ((opts = getopt(argc, argv, ":p:s:")) != -1) {
		switch (opts) {
			case 's': 
				if ((defname = (char *) malloc(sizeof(char)*strlen(optarg))) == NULL) {
					perror("Error malloc");
					exit(1);	
				}
				defname = optarg;
				printf("Flag -s, with parameter %s\n",defname);
				break;
			case 'p':
				portno = atoi(optarg);
				printf("Flag -p, with parameter %d\n",portno);
				break;
			case '?':
	
				printf("ERROR with flags\n");
				break;
		}
	}


	list rooms = initialize_rooms(defname);
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		error("ERROR on binding");


	/* Aquí empiezan a hacerse los hilos */

	listen(sockfd,3);
	
	clilen = sizeof(cli_addr);

	pthread_t thread_id;

	pthread_mutex_init(&mutex,NULL);
	while (client_sock = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) {
		printf("Connection accepted\n");
		if (pthread_create(&thread_id, NULL, connection_handler, 
												(void *) &client_sock) ) {
			perror("could not create thread");
			exit(1);
		}
	}

	if (client_sock < 0) 
		error("ERROR accepting connection");
	return 0; 
}


void error(const char *msg) {
  perror(msg);
  exit(1);
}

void *connection_handler(void *socket_desc) {
	int sock = *(int *) socket_desc;
	int read_size;
	while ((read_size = recv(sock, client_message, 2000, 0)) > 0) {
		client_message[read_size] = '\0';
		write(sock, client_message, strlen(client_message));	
		write(sock001, client_message, strlen(client_message));
		memset(client_message, 0, 2000);
	}
}
