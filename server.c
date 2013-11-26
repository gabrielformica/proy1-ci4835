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

typedef struct thread_data thread_data;
struct thread_data {
   int client_sock;
	list rooms; 
};

thread_data prepare_thread_data();

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
		thread_data tdata = prepare_thread_data(client_sock, rooms);
		
		if (pthread_create(&thread_id, NULL, connection_handler, 
												(void *) &tdata) ) {
			perror("could not create thread");
			exit(1);
		}
	}

	if (client_sock < 0) 
		error("ERROR accepting connection");
	return 0; 
}



thread_data prepare_thread_data(int client_sock, list rooms) {
	thread_data temp; 
	temp.client_sock = client_sock;
	temp.rooms = rooms;
	return temp;
}

void error(const char *msg) {
  perror(msg);
  exit(1);
}

void *connection_handler(void *td) {
	int sock = ((thread_data *) td)->client_sock;
	list rooms = ((thread_data *) td)->rooms;	
	char message[256];
	int length = strlen(message);
	int read_size;
	box *user = NULL;
	if ((read_size = recv(sock, message, length, 0)) > 0) {
		pthread_mutex_lock(&mutex);
		user = add_user(rooms, rooms->first->elem->name, message);	
		pthread_mutex_unlock(&mutex);
	}
	//funcion
		//Primer ciclo que pide el nombre de usuario
		//mutex lock
		//se agrega en el heap caja con usuario a la sala por defecto 
		//mutex unlock
	//
	while ((read_size = recv(sock, message, length, 0)) > 0) {
		
		pthread_mutex_lock(&mutex);
		//se parsea el comando
		//funcion que ejecuta la funcion correspondiente con el comando
		int i;
		for (i = 0; i < read_size; i++)
			client_message[i] = message[i];	
		client_message[read_size] = '\0';
		write(sock, client_message, strlen(client_message));	
		memset(client_message, 0, 2000);
		pthread_mutex_unlock(&mutex);
	}

}


//funcion sal 
	//imprime la rooms nada mas ese socket

//funcion usu
	//le imprime a ese usuario todos los usuarios conectados en el servidor

//funcion men $1
	//


//funcion sus

