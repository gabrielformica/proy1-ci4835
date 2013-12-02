/**
  * @file
  * @author Gabriel Formica <gabriel@ac.labf.usb.ve>
  * @author Melecio Ponte <melecio@ac.labf.usb.ve> 
  *
  * @section Descripcion
  *
  * List of rooms that implements generic lists
  */

#include "list.h"

#ifndef ROOMSLIST_H
#define ROOMSLIST_H

typedef list userslist;
typedef struct room room;
typedef struct user_data user_data;

struct room {
	char *name;
	userslist users;
};

struct user_data {
   char *name;
   int socket;
   list subscribed_rooms;
};


/**
  * Initialize the list of rooms
  * @param defname: the name of the first room (default room)
  * @return A valid list of rooms 
  */

extern list initialize_rooms(char *);

/**
  * Add a user to a room
  * @param l: list of rooms
  * @param roomname: name of the room 
  * @param user: pointer to the user data
  * @return If exist a room with the same name of roomname, return a 
  *         pointer to the box that points to the user data.
  *         Return NULL in any other case
  */

extern box *add_user(list, char *, void *);

/**
  * Add a room
  * @param l: list of rooms
  * @param name: name of the room 
  * @return Pointer to the box that points to the room data
  */

extern box *add_room(list, char *);

/**
  * Create and initialize an user data structure
  * @param name: name of the user (client)
  * @param socket: socket of the user (client)
  * @return The pointer to the user data structure
  */

extern user_data *create_user_data(char *, int);

/**
  * Check if a room is in a list of rooms
  * @param name: name of the room 
  * @param l: list of rooms
  * @return True, if the room exist into the list. False in any other case
  */

bool room_is_in(char *, list);

/**
  * Get a room 
  * @param l: list of rooms
  * @param roomname: name of the room 
  * @return The pointer to the box that points to room structure
  */

extern box *get_room(list,char *);

/**
  * Delete the box that points to a user
  * @param l: list of rooms
  * @param roomname: name of the room 
  * @param user: pointer to the user data structure
  * @return True, it the box is deleted. False in any other case
  */

extern bool del_user(list, char*, void *);

/**
  * Get the users of a room
  * @param r: pointer to the room
  * @return The users added to that room
  */

extern list get_userslist(room *r);

/**
  * Get the socket number of an user 
  * @param ud: pointer to the user_data structure 
  * @return The socket number of the user
  */

extern int get_socket(user_data *);

/**
  * Get the list subscribed_rooms of user_data *
  * @param ud: pointer to user_data
  * @return The list of subscribed_rooms
  */

extern list get_subscribed_rooms(user_data *); 


/**
  * Get the name of an user 
  * @param ud: pointer to the user_data structure 
  * @return The socket number of the user
  */

extern char *get_name(user_data *);

#endif
