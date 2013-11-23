#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(char *msg) {
	perror(msg);
	exit(0);
}



int main(int argc, char *argv[]) {

	int sockfd, portno, charno;
	char opts, *host, *username, *comfile, buffer[256];
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
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
		error("ERROR connecting");
	}
	printf("You are connected\n");

/*
	close(sockfd);
	free(host);
	free(comfile); 
	free(username);
*/
}
