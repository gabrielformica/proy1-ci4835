/**
  * @file
  * @author Gabriel Formica <gabriel@ac.labf.usb.ve>
  * @author Melecio Ponte <melecio@ac.labf.usb.ve> 
  *
  * @section Descripcion
  *
  * List of rooms that implements generic lists
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "roomslist.h"


/**
  * Initliaze the list of rooms
  * @param defname: the name of the first room (default room)
  * @return A valid list of rooms 
  */

list initialize_rooms(char *defname) {
	list rooms = create_list();

	if (! (add_room(rooms, defname))) 
		perror("ERROR adding room");

	return rooms;	
}

/**
  * Add a user to a room
  * @param l: list of rooms
  * @param roomname: name of the room 
  * @param user: pointer to the user data
  * @return If exist a room with the same name of roomname, return a 
  *         pointer to the box that points to the user data.
  *         Return NULL in any other case
  */

box *add_user(list l, char *roomname, void *user) {
	if (roomname == NULL) {
		perror("ERROR room name");
		exit(1);
	}
	if (user == NULL) {
		perror("ERROR user name ");
		exit(1);
	}
	if (l == NULL) {
		perror("ERROR passing empty list");
		exit(1);
	}	

	box *temp = get_room(l, roomname);
	if (temp == NULL) //if it doesnt exist, return null
		return NULL;

	return add(((room *)temp->elem)->users, user);			
}

/**
  * Add a room
  * @param l: list of rooms
  * @param name: name of the room 
  * @return Pointer to the box that points to the room data
  */

box *add_room(list l, char *name) {
	if (l == NULL) {
		perror("ERROR passing empty list");
		exit(1);
	}

	if (name == NULL) {
		perror("ERROR room name");
		exit(1);
	}

	if (room_is_in(name,l))
		return NULL;

	room *temp;	
	if ((temp = (room *) malloc(sizeof(room))) == NULL) {
		perror("ERROR malloc room");
		return NULL;
	}

	temp->name = name;
	temp->users = create_list();
	return (add(l,temp));
}

/**
  * Create and initialize an user data structure
  * @param name: name of the user
  * @param socket: socket of the user (client)
  * @return The pointer to the user data structure
  */

user_data *create_user_data(char *name, int socket) {
	user_data *ud;
	if ((ud = malloc(sizeof(user_data))) == NULL) {
		perror("ERROR malloc");	
	}	
	ud->name = name;	
	ud->socket = socket;
}

/**
  * Check if a room is in a list of rooms
  * @param name: name of the room 
  * @param l: list of rooms
  * @return True, if the room exist into the list. False in any other case
  */

bool room_is_in(char *name, list l) {
	return (get_room(l, name) != NULL);
}

/**
  * Get a room 
  * @param l: list of rooms
  * @param roomname: name of the room 
  * @return The pointer to the box that points to room structure
  */

box *get_room(list l, char *roomname) {
	box *temp = l->first;
	while ((temp != NULL) && (strcmp(((room *)temp->elem)->name, roomname) != 0))
		temp = temp->next;
	return temp;
}

/**
  * Delete the box that points to a user
  * @param l: list of rooms
  * @param roomname: name of the room 
  * @param user: pointer to the user data structure
  * @return True, it the box is deleted. False in any other case
  */

bool del_user(list l, char *roomname, void *user) {
	box *temp = get_room(l, roomname);
	return del(((room *) temp->elem)->users, user);
}

/**
  * Get the list subscribed_rooms of user_data *
  * @param ud: pointer to user_data
  * @return the list of subscribed_rooms
  */

list get_subscribed_rooms(user_data * ud) {
	return ud->subscribed_rooms;
}

/**
  * Get the users of a room
  * @param r: pointer to the room
  * return: The users added to that room
  */

list get_userslist(room *r) {
	return r->users;
}
