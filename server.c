#include <netinet/tcp.h>
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
void sal();
void usu();
void *connection_handler();
box *wait_username();
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
       int flag = 1;
       int result = setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY,
                               (char *) &flag, sizeof(int));
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
	printf("este es el socket: -%d-\n",sock);
	printf("este es el defname: -%s-\n", ((room *)rooms->first->elem)->name);
	printf("");
	char message[256];
	/*
	if ((message = malloc(sizeof(char)*256)) == NULL) {
		perror("ERROR malloc message");
	}
	*/
	memset(message, 0, 256);
	
	box *user = NULL;

	user = wait_username(rooms, message, sock);  //user points to the box of the user
	printf("este es el nombre de usuario: -%s-\n", ((char *) user->elem));
    box *current_room = rooms->first;	
	int read_size;
	while ((read_size = recv(sock, message, 256, 0)) > 0) {
		
		pthread_mutex_lock(&mutex);
		if (read_size < 3) {
		}
		else if ((message[0] == 's') && (message[1] == 'u') && (message[2] == 's')) {
			printf("HOLAAAAAA \n");
		}
		else if ((message[0] == 's') && (message[1] == 'a') && (message[2] == 'l')) {
           //message = "Mandaste SAL";
           sal(rooms, sock);
        } 
        else if ((message[0] == 'm') && (message[1] == 'e') && (message[2] == 'n')) {
		//	message = "Mandaste MEN";
			write(sock, message, strlen(client_message));	
		}
		else if ((message[0] == 'u') && (message[1] == 's') && (message[2] == 'u')) {
		//	message = "Mandaste USU";
           usu(rooms, sock);
		}
		else if ((message[0] == 'd') && (message[1] == 'e') && (message[2] == 's')) {
		//	message = "Mandaste DES";
			write(sock, message, strlen(client_message));	
		}
		else if ((message[0] == 'c') && (message[1] == 'r') && (message[2] == 'e')) {
		//	message = "Mandaste CRE";
			write(sock, message, strlen(client_message));	
		}
		else if ((message[0] == 'e') && (message[1] == 'l') && (message[2] == 'i')) {
		//	message = "Mandaste ELI";
			write(sock, message, strlen(client_message));	
		}
		else if ((message[0] == 'f') && (message[1] == 'u') && (message[2] == 'e')) {
	//		message = "Mandaste FUE";
			write(sock, message, strlen(client_message));	
		}
		//funcion que ejecuta la funcion correspondiente con el comando
		/*
		int i;
		for (i = 0; i < read_size; i++)
			client_message[i] = message[i];	
		client_message[read_size] = '\0';
		write(sock, client_message, strlen(client_message));	
		memset(client_message, 0, 2000);
		*/
		memset(message, 0, strlen(message));
		pthread_mutex_unlock(&mutex);
	}

}

box *wait_username(list rooms, char *str, int sock) {
	char buffer[40];
	bzero(buffer,40);
	int read_size;
	box *user = NULL;
	if ((read_size = recv(sock, buffer, 40, 0)) > 0) {
		pthread_mutex_lock(&mutex);
		char *user_name;
		if ((user_name =  malloc(sizeof(char)*read_size)) == NULL) {
			perror("ERRRO malloc");
		}
		int i;
		for (i = 0; i < read_size; i++) 
			user_name[i] = buffer[i];
		user = add_user(rooms, ((room *)rooms->first->elem)->name, user_name);	
		pthread_mutex_unlock(&mutex);
	}
	return user;	
}


void sal(list l, int sock) {
   if ((l == NULL) || (get_size(l) == 0)) {
      write(sock, "There are no rooms available\n", 50);
      return;
   }
   write(sock, "---Lista de salas---",256);
   box *temp = l->first;
   while (temp != NULL) {
      write(sock, ((room *) temp->elem)->name, 10);
      temp = temp->next;
   }

}

//funcion usu
	//le imprime a ese usuario todos los usuarios conectados en el servidor
void usu(list l, int sock) {
   box *temp = l->first;
   write(sock, "---Lista de usuarios conectados el servidor---",256);
   while (temp != NULL) {
      box *temp2 = ((room *) temp->elem)->users->first;
      if (temp2 == NULL) {
         temp = temp->next;
         continue;
      }
      char b[256];
      while (temp2 != NULL) {
         int len = strlen(((char *) temp2->elem));
         bzero(b,256);
         int i;
         for (i = 0; i < len; i++)
            b[i] = ((char *) temp2->elem)[i];
         write(sock, b, 256);
         temp2 = temp2->next;
      }
      temp = temp->next;
   }
   
}


//funcion men $1
	//


//funcion sus

