#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "list.h"
#include "roomslist.h"

void error();
	
int main(int argc, char *argv[]) {

	/* declare */
	char opts;
	int sockfd, newsockfd, portno;
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
				printf("Flag -h, with parameter %s\n",defname);
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
	
	
/*****

*/


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

	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, 
							(struct sockaddr *) &cli_addr, 
							&clilen);
	if (newsockfd < 0) 
		error("ERROR on accept");
	bzero(buffer,256);
	n = read(newsockfd,buffer,255);
	if (n < 0) error("ERROR reading from socket");
	printf("Here is the message: %s\n",buffer);
	n = write(newsockfd,"I got your message",18);
	if (n < 0) error("ERROR writing to socket");
	close(newsockfd);
	close(sockfd);
	return 0; 
}


void error(const char *msg)
{
  perror(msg);
  exit(1);
}
