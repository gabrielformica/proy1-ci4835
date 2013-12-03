/**
  * @file
  * @author Gabriel Formica <gabriel@ac.labf.usb.ve>
  * @author Melecio Ponte <melecio@ac.labf.usb.ve> 
  *
  * @section Descripcion
  *
  * Main program of schat server
  */

#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <ctype.h>
#include "list.h"
#include "roomslist.h"
#define DEFAULT "actual"
#define MAX_PORT_NUM 65535
#define MAX_PACK_SIZE 256

void error();
void sal();
void usu();
void cre();
void subscribe();
void men();
void unsubscribe_all();
void des();
void sus();
void fue();
void eli();
void broadcast_to_users();
void *connection_handler();
user_data *wait_username();
pthread_mutex_t mutex;
list rooms;   
list connected_users;

typedef struct Thread_data Thread_data;

struct Thread_data {
   int client_sock;
   list subscribed_rooms; 
};

Thread_data prepare_thread_data();

int main(int argc, char *argv[]) {
   char opts;
   int sockfd, client_sock, portno;
   socklen_t clilen;
   char buffer[MAX_PACK_SIZE];
   char *defname = NULL;
   struct sockaddr_in serv_addr, cli_addr;
   int n;
   if (argc > 5) {
      printf ("Too many arguments\n");
      exit(1);
   }

   if (argc < 3) {    //port is needed 
      fprintf(stderr, "Too few arguments.\n");
      exit(1);
   } 

   while ((opts = getopt(argc, argv, ":p:s:")) != -1) {
      switch (opts) {
      case 's': 
         if ((defname = (char *) malloc(sizeof(char)*strlen(optarg))) == NULL)
            error("Malloc failed");
         defname = optarg;
         break;
      case 'p':
         portno = atoi(optarg);
         if (portno < 1 || portno > MAX_PORT_NUM) {
            printf ("Port entered is an invalid port number.\n");
            exit(1);
         }
         break;
      case '?':
         printf("Urecognized option. Try again.\n");
			exit(1);
      }
   }

   if (defname == NULL) {  
      if ((defname = (char *) malloc(sizeof(char)*strlen(DEFAULT))) == NULL)
         error("Malloc failed");
      defname = DEFAULT;
   }

   rooms = initialize_rooms(defname);
   connected_users = create_list();
   if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
      error("Open socket failed");
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);

   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
   	error("Binding failed");


   /* Accept connections and create threads*/

   listen(sockfd, 3);
   clilen = sizeof(cli_addr);
   pthread_t thread_id;
   pthread_mutex_init(&mutex, NULL);  
   while (client_sock = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) {
      int flag = 1;
      int result = setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, 
														(char *) &flag, sizeof(int));
      printf("Connection accepted\n");
      Thread_data tdata = prepare_thread_data(client_sock, create_list());
		
      if (pthread_create(&thread_id, NULL, connection_handler, 
                         (void *) &tdata)) {
         perror("could not create thread");
      }
   }
}

/**
  * Task that thread of connection is running 
  * @param tdata: the thread data
  */

void *connection_handler(void *td) {
   int sock = ((Thread_data *) td)->client_sock;
   list subscribed_rooms = ((Thread_data *) td)->subscribed_rooms;	
   char msg[MAX_PACK_SIZE];
   memset(msg, 0, MAX_PACK_SIZE);

   user_data *user = wait_username(rooms, sock);  
   user->subscribed_rooms = subscribed_rooms;
   add(connected_users, user);
   add(subscribed_rooms, rooms->first);	
   int read_size;
   while ((read_size = recv(sock, msg, MAX_PACK_SIZE, 0)) > 0) {
      pthread_mutex_lock(&mutex);
      if (read_size < 3) {
      }
      else if ((strlen(msg) >= 6) &&
					(msg[3] == ' ') && 
					(msg[0] == 's') && (msg[1] == 'u') && (msg[2] == 's')) {
			memmove(msg, msg+4, 252);
			msg[strlen(msg)-1] = '\0';   //replace new line
         sus(msg, user);
      }
      else if ((strlen(msg) == 4) &&
					(msg[0] == 's') && (msg[1] == 'a') && (msg[2] == 'l')) {
         sal(sock);
      } 
      else if ((strlen(msg) >= 6) &&  //because of new line
					(msg[3] == ' ') &&
					(msg[0] == 'm') && (msg[1] == 'e') && (msg[2] == 'n')) {
			memmove(msg, msg+4, 252);
			msg[strlen(msg)-1] = '\0';   //replace new line
         men(user, subscribed_rooms, msg);
      }
      else if ((strlen(msg) == 4) &&
					(msg[0] == 'u') && (msg[1] == 's') && (msg[2] == 'u')) {
         usu(sock); 
      }
      else if ((strlen(msg) == 4) &&
					(msg[0] == 'd') && (msg[1] == 'e') && (msg[2] == 's')) {
         des(subscribed_rooms, user);
      }
      else if ((strlen(msg) >= 6) &&
					(msg[3] == ' ') && 
					(msg[0] == 'c') && (msg[1] == 'r') && (msg[2] == 'e')) {
			memmove(msg, msg+4, 252);
			msg[strlen(msg)-1] = '\0';   //replace new line
         cre(sock, msg);
      }
      else if ((strlen(msg) >= 6) &&
					(msg[3] == ' ') && 	
					(msg[0] == 'e') && (msg[1] == 'l') && (msg[2] == 'i')) {
			memmove(msg, msg+4, 252);
			msg[strlen(msg)-1] = '\0';   //replace new line
         eli(msg, sock, user);
      }
      else if ((strlen(msg) == 4) &&
					(msg[0] == 'f') && (msg[1] == 'u') && (msg[2] == 'e')) {
         fue(subscribed_rooms, user);
      }
      else if ((strlen(msg) == 4) &&
					(msg[0] == 'h') && (msg[1] == 'l') && (msg[2] == 'p')) {
			memset(msg, 0, MAX_PACK_SIZE);
			strcat(msg, "-----------\n");
			strcat(msg, "The valids commands formats are:\n");
			strcat(msg, "sus <room name>\n");
			strcat(msg, "cre <room name>\n");
			strcat(msg, "eli <room name>\n");
			strcat(msg, "sal\n");
			strcat(msg, "usu\n");
			strcat(msg, "des\n");
			strcat(msg, "fue\n");
			strcat(msg, "-----------");
			write(sock, msg, MAX_PACK_SIZE);
      }
		else {
			memset(msg, 0, MAX_PACK_SIZE);
			strcat(msg, "Unrecognized option.");
			strcat(msg, " Try `hlp' for more information.");
			write(sock,	msg, MAX_PACK_SIZE);
		}
      memset(msg, 0, MAX_PACK_SIZE);
      pthread_mutex_unlock(&mutex);
   }
}


/**
  * Prepare the thread data 
  * @param client_sock: socket of client
  * @param l: list of subscribed rooms of client
  * @return An initialized thread data
  */

Thread_data prepare_thread_data(int client_sock, list l) {
   Thread_data temp; 
   temp.client_sock = client_sock;
   temp.subscribed_rooms = l;
   return temp;
}

/**
  * Send an error message and terminate the program
  * @param msg: msg to be sended
  */

void error(const char *msg) {
   perror(msg);
   exit(1);
}

/**
  * Subscribe a new user to a chatroom.
  * @param ud: the data from the user that wants to subscribe to a chatroom.
  * @param roomname: the name of the chatroom to which ud wants to subscribe.
  * @return Anything.
  */

void sus(char *roomname, user_data *ud) {
   list subs_rooms = ud->subscribed_rooms;
   if (get_room(rooms, roomname) == NULL) {
      write(get_socket(ud), 
				"Sorry, this room does not exist", 
				MAX_PACK_SIZE);
      return;
   }
	if (is_in(get_room(rooms, roomname), subs_rooms)) {
		write(get_socket(ud), 
				"You are already subscribed to this room", 
				MAX_PACK_SIZE);
		return;
	}
   add_user(rooms, roomname, ud);
   add(subs_rooms, get_room(rooms, roomname));

	char *buffer;
	if ((buffer = malloc(sizeof(char)*256)) == NULL) {
		perror("Malloc failed");
		write(get_socket(ud), "You are now subscribed to the room!", 256);
		return;
	}
	memset(buffer, 0, 256);
	strcat(buffer, "You are now subscribed to the room '");	
	strcat(buffer, roomname);
	strcat(buffer,"'!");
	write(get_socket(ud), buffer, 256);
	free(buffer);
}

/**
  * Unsubscribe an user of all of the chatrooms. Auxiliar function for des
  * @param ud: the data from the user that wants to desubscribe.
  * @param subs_rooms: list of rooms the user is subscribed to.
  * @return Anything.
  */


void unsubscribe_all(list subs_rooms, user_data *ud) {
   box *temp = rooms->first;
   while (temp != NULL) {	
      del_user(rooms, ((room *) temp->elem)->name, ud);
      temp = temp->next;
   }
   destroy(subs_rooms);  //Delete all subscriptions
}

/**
  * Unsubscribe an user of all of the chatrooms and send a message to the user.
  * @param ud: the data from the user that wants to desubscribe.
  * @param subs_rooms: list of rooms the user is subscribed to.
  * @return Anything.
  */

void des(list subs_rooms, user_data *ud) {
	unsubscribe_all(subs_rooms, ud);
	write(get_socket(ud), 
			"You have been unsubscribed of all of the rooms.", 
			256);
}


/**
  * Waits for an user to send its username through a socket.
  * @param rooms: list of chatrooms.
  * @param socket: socket through which the username will be sent.
  * @return A user_data structure containing the data from the new user.
  */

user_data *wait_username(list rooms, int socket) {
   user_data *ud = NULL;
   if ((ud = malloc(sizeof(user_data))) == NULL) {
      perror("Malloc failed");
		write(socket, "server: There was an error in the server.", MAX_PACK_SIZE);
		close(socket); //end of connection
		return NULL;
   }
   char buffer[MAX_PACK_SIZE];
   bzero(buffer, MAX_PACK_SIZE);
   int read_size;
   box *user = NULL;
   if ((read_size = recv(socket, buffer, MAX_PACK_SIZE, 0)) > 0) {
      pthread_mutex_lock(&mutex);
      char *user_name;
      if ((user_name =  malloc(sizeof(char)*read_size)) == NULL) {
         perror("Malloc failed");
			write(socket, "server: There was an error in the server.", MAX_PACK_SIZE);
			close(socket); //end of connection
			return NULL;
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

/**
  * Lists all of the chatrooms on the server.
  * @param socket: socket through which the list of chatrooms wil be sent.
  * @return Anything.
  */

void sal(int socket) {
   write(socket, "---These are all the rooms---", MAX_PACK_SIZE);
   box *temp = rooms->first;
	char *buffer;
	if ((buffer = malloc(sizeof(char)*MAX_PACK_SIZE)) == NULL) {
		perror("Malloc failed");
		write(socket, 
				"server: Sorry, problems with server. Try again.", 
				MAX_PACK_SIZE);
	} 
	else {
		while (temp != NULL) {
			memset(buffer, 0, MAX_PACK_SIZE);
			strcat(buffer, "* ");
			strcat(buffer, ((room *) temp->elem)->name);
			write(socket, buffer, MAX_PACK_SIZE);
			temp = temp->next;
		}
		write(socket, "-----------------------------", MAX_PACK_SIZE);
		free(buffer);
	}
}

/**
  * Broadcasts a message to all users from a list.
  * @param users: List of users to which the message will be broadcasted.
  * @param msg: The message to be broadcasted.
  * @return Anything.
  */

void broadcast_to_users(userslist users, char *msg) {
   box *temp = users->first;
   while (temp != NULL) {
      user_data *ud = (user_data *) temp->elem;
      write(ud->socket, msg, MAX_PACK_SIZE); 
      temp = temp->next;	
   }
}

/**
  * Sends a message to all subscribed rooms.
  * @param user: Data from the user sending the message.
  * @param msg: The message to be sent.
  * @param subs_rooms: List of rooms the user is subscribed to.
  * @return Anything.
  */

void men(user_data *user, list subs_rooms, char *msg) {
   char *buffer;
	if ((buffer = malloc(sizeof(char)*MAX_PACK_SIZE)) == NULL) {
		perror("Malloc failed");
		write(get_socket(user),
				"Server: there was an error. Please, resend your message.",
				MAX_PACK_SIZE);
		return;
	}
   box *temp = subs_rooms->first;	
   while (temp != NULL) {
      memset(buffer, 0, MAX_PACK_SIZE);
      int i;
      strcpy(buffer, user->name);
      strcat(buffer, " (from room \'");	
      strcat(buffer, ((room *) ((box *) temp->elem)->elem)->name);
      strcat(buffer, "\') says: ");
      strcat(buffer, msg);
      broadcast_to_users(((room *) ((box *) temp->elem)->elem)->users, buffer);
      temp = temp->next;
   }
   memset(buffer, 0, MAX_PACK_SIZE);
   free(buffer);
}

/**
  * Sends a list of all the users subscried to the server.
  * @param socket: Socket through which the list of users will be sent.
  * @return Anything.
  */

void usu(int socket) {
   write(socket, "---These are all the users connected---", MAX_PACK_SIZE);
   box *temp = connected_users->first;	
	char *buffer;
	if ((buffer = malloc(sizeof(char)*MAX_PACK_SIZE)) == NULL) {
		perror("Malloc failed");
		write(socket, 
				"server: Sorry, problems with server. Try again.", 
				MAX_PACK_SIZE);
	} 
	else {
		while (temp != NULL) {
			memset(buffer, 0, MAX_PACK_SIZE);
			strcat(buffer, "* ");
			strcat(buffer, get_name((user_data *) temp->elem)); 
			write(socket, buffer, MAX_PACK_SIZE);
			temp = temp->next;
		}
		write(socket, "-----------------------------", MAX_PACK_SIZE);
		free(buffer);
	}
}

/**
  * Creates a new chatroom.
  * @param roomname: The name of the room that will be created.
  * @param socket: Socket through which some information will be sent.
  * @return Anything.
  */

void cre(int socket, char *roomname) {
	char *name;
	if ((name = malloc(sizeof(char)*strlen(roomname))) == NULL) {
		perror("Malloc failed");
		write(socket,
				"Server: there was an error. Please, resend your message.",
				MAX_PACK_SIZE);
		return;
	}
	strcpy(name, roomname);
   box *temp;

   if ((temp  = get_room(rooms, name)) != NULL) {
      write(socket, "Room already exists", MAX_PACK_SIZE);
      return;
   }
   temp = NULL;

   if ((temp = add_room(rooms, name)) == NULL) {
      write(socket, "Problem creating room", MAX_PACK_SIZE);
      return;
   }
}

/**
  * Ends the connection for an user.
  * @param socket: Socket through which some information will be sent.
  * @param subs_rooms: List of rooms the user is subscribed to.
  * @param user: Data from the user ending the connection.
  * @return Anything.
  */

void fue(list sub_rooms, user_data *ud) {
	int socket = get_socket(ud);
   box *temp = connected_users->first;
   unsubscribe_all(sub_rooms, ud);
   del(connected_users, ud);
   free(ud);
   write(socket, "See you later", MAX_PACK_SIZE);
   close(socket);
   return;      
}

/**
  * Deletes a chatroom from the server.
  * @param roomname: The name of the room that will be deleted.
  * @param socket: Socket through which some information will be sent.
  * @return Anything.
  */

void eli(char *roomname, int socket) {
	box *temp_room = get_room(rooms, roomname);
	if (temp_room == NULL) {
		write(socket, "You can't delete a room that doesn't exist", MAX_PACK_SIZE);
		return;
	}
	if (temp_room == rooms->first) {
		write(socket, "You just can't delete the default room", MAX_PACK_SIZE);
		return;
	}

	box *temp_cu = connected_users->first;
	//delete subscriptions to the rooms of every user
	while (temp_cu != NULL) { 
		del(get_subscribed_rooms(temp_cu->elem), temp_room);
		temp_cu = temp_cu->next;
	}

	//destroy list of users in the room
	list userslist = get_userslist(temp_room->elem);
	destroy(userslist);

	//delete the room
	del(rooms, temp_room->elem);
}
