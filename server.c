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
void cre();
void subscribe();
void men();
void des();
void sus();
void fue();
void broadcast_to_users();
void *connection_handler();
user_data *wait_username();
pthread_mutex_t mutex;
char client_message[2000];


list rooms;   
list users_connected;


typedef struct thread_data thread_data;
struct thread_data {
   int client_sock;
   list subscribed_rooms; 
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

   rooms = initialize_rooms(defname);
   users_connected = create_list();
   add_room(rooms,"A");
    
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
      thread_data tdata = prepare_thread_data(client_sock, create_list());
		
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

void *connection_handler(void *td) {
   int sock = ((thread_data *) td)->client_sock;
   list subscribed_rooms = ((thread_data *) td)->subscribed_rooms;	
   char aux[30];
   char message[256];
   memset(message, 0, 256);

   user_data *user = wait_username(rooms, sock);  //user points to the box of the user
   add(users_connected, user);
   printf("este es el nombre de usuario: -%s-\n", ((char *) user->name));
   add(subscribed_rooms, rooms->first);	
   printf("primera sala: -%s-\n", ((room *) ((box *)subscribed_rooms->first->elem)->elem)->name);
   int read_size;
   while ((read_size = recv(sock, message, 256, 0)) > 0) {
      pthread_mutex_lock(&mutex);
      if (read_size < 3) {
      }
      else if ((message[0] == 's') && (message[1] == 'u') && (message[2] == 's')) {
         printf("holaa\n");
         int i	= 0;	
         memset(aux, 0, 30);
         while  (message[i+4] != '\n') {
            aux[i] = message[i+4];
            i++;
         }
         printf("este es largo %d y te vas a suscribir a: -%s-\n",read_size, aux);
         sus(subscribed_rooms, aux, user);
      }
      else if ((message[0] == 's') && (message[1] == 'a') && (message[2] == 'l')) {
         sal(sock);
      } 
      else if ((message[0] == 'm') && (message[1] == 'e') && (message[2] == 'n')) {
         int i	= 0;	
         memset(aux, 0, 30);
         while  (message[i+4] != '\n') {
            aux[i] = message[i+4];
            i++;
         }
         men(user, subscribed_rooms, aux);
      }
      else if ((message[0] == 'u') && (message[1] == 's') && (message[2] == 'u')) {
         usu(sock); 
      }
      else if ((message[0] == 'd') && (message[1] == 'e') && (message[2] == 's')) {
         des(subscribed_rooms, user);
      }
      else if ((message[0] == 'c') && (message[1] == 'r') && (message[2] == 'e')) {
         int i	= 0;	
         memset(aux, 0, 30);
         while  (message[i+4] != '\n') {
            aux[i] = message[i+4];
            i++;
         }
         cre(sock,aux);
      }
      else if ((message[0] == 'e') && (message[1] == 'l') && (message[2] == 'i')) {
         //	message = "Mandaste ELI";
         write(sock, message, strlen(client_message));	
      }
      else if ((message[0] == 'f') && (message[1] == 'u') && (message[2] == 'e')) {
         fue(sock, subscribed_rooms, user);
      }
      memset(message, 0, strlen(message));
      pthread_mutex_unlock(&mutex);
   }
}

thread_data prepare_thread_data(int client_sock, list l) {
   thread_data temp; 
   temp.client_sock = client_sock;
   temp.subscribed_rooms = l;
   return temp;
}

void error(const char *msg) {
   perror(msg);
   exit(1);
}

void sus(list subs_rooms, char *roomname, user_data *ud) {
   add_user(rooms, roomname, ud);
   add(subs_rooms, get_room(rooms, roomname));
}

void des(list subs_rooms, user_data *ud) {
   box *temp = rooms->first;
   while (temp != NULL) {	
      del_user(rooms, ((room *) temp->elem)->name, ud);
      temp = temp->next;
   }
   destroy(subs_rooms);
}

user_data *wait_username(list rooms, int socket) {
   user_data *ud = NULL;
   if ((ud = malloc(sizeof(user_data))) == NULL) {
      perror("Error malloc");
   }
   char buffer[40];
   bzero(buffer,40);
   int read_size;
   box *user = NULL;
   if ((read_size = recv(socket, buffer, 40, 0)) > 0) {
      pthread_mutex_lock(&mutex);
      char *user_name;
      if ((user_name =  malloc(sizeof(char)*read_size)) == NULL) {
         perror("ERRRO malloc");
      }
      int i;
      for (i = 0; i < read_size; i++) 
         user_name[i] = buffer[i];
      ud = create_user_data(user_name, socket);	
      user = add_user(rooms, ((room *)rooms->first->elem)->name, ud);	
      pthread_mutex_unlock(&mutex);
   }
   return ud;	
}


void sal(int sock) {
   write(sock, "---Lista de salas---",256);
   box *temp = rooms->first;
   while (temp != NULL) {
      printf("ESTAS SON LAS SALASSS : %s\n",((room *) temp->elem)->name);
      write(sock, ((room *) temp->elem)->name, 256);
      temp = temp->next;
   }
}

void broadcast_to_users(userslist users, char *msg) {
   box *temp = users->first;
   while (temp != NULL) {
      user_data *ud = (user_data *) temp->elem;
      write(ud->socket, msg, 256); 
      temp = temp->next;	
   }
}

void men(user_data *user, list subs_rooms, char *msg) {
   char *buffer = malloc(sizeof(char)*256);
   box *temp = subs_rooms->first;	
   while (temp != NULL) {
      memset(buffer, 0, 256);
      int i;
      strcpy(buffer, user->name);
      strcat(buffer, " (from room \'");	
      strcat(buffer, ((room *) ((box *) temp->elem)->elem)->name);
      strcat(buffer, "\') says: ");
      strcat(buffer, msg);
		
      broadcast_to_users(((room *) ((box *) temp->elem)->elem)->users, buffer);
      temp = temp->next;
   }
   free(buffer);
}

void usu(int sock) {
   box *temp = users_connected->first;	
   while (temp != NULL) {
      write(sock, ((user_data *) temp->elem)->name, 256);
      temp = temp->next;
   }
}

void cre(int sock, char *roomname) {
   box *temp;

   if ((temp  = get_room(rooms, roomname)) != NULL) {
      write(sock, "Room already exists", 256);
      return;
   }
   temp = NULL;

   if ((temp = add_room(rooms, roomname)) == NULL) {
      write(sock, "Room already exists", 256);
      return;
   }
   
}


void fue(int sock, list sub_rooms, user_data *ud) {
   box *temp = users_connected->first;
   des(sub_rooms, ud);
   del(users_connected,ud);
   free(ud);
   write(sock, "See you later", 256);
   close(sock);
   return;      
}
