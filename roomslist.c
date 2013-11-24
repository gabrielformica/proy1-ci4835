#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "roomslist.h"

#define DEFAULT "actual"

bool add_user(list l, char *roomname, char *username) {
	if (roomname == NULL) {
		perror("ERROR room name");
		exit(1);
	}
	if (username == NULL) {
		perror("ERROR user name ");
		exit(1);
	}
	if (l == NULL) {
		perror("ERROR passing empty list");
		exit(1);
	}	

	//Se busca el nombre de la sala	
	box *temp = l->first;
	
	while ((temp != NULL) && (strcmp(((room *)temp->elem)->name, roomname) != 0))
		temp = temp->next;

	//    si no encuentra la sala, retorna false
	if (temp == NULL)
		return false;

	//    si lo encuentra, busca al usuario
	box *temp2 = ((room *) temp->elem)->users->first;
	while ((temp2 != NULL) && (strcmp((char *) temp2->elem, username) != 0))
		temp2 = temp2->next;

	//				si el usuario no esta, lo agrega y retorna true
	if (temp2 == NULL)
		return add(((room *)temp->elem)->users, username);			
	//				si el usuario esta, lo agrega con nombre(i) y retornar true				
	return true;
}

bool add_room(list l, char *name) {
	if (l == NULL) {
		perror("ERROR passing empty list");
		exit(1);
	}

	if (name == NULL) {
		perror("ERROR room name");
		exit(1);
	}

	if (room_is_in(name,l))
		return false;

	room *temp;	
	if ((temp = (room *) malloc(sizeof(room))) == NULL) {
		perror("ERROR malloc room");
		return false;
	}
	
	temp->name = name;
	temp->users = create_list();
	return (add(l,temp));
}

list initialize_rooms(char *defname) {
	list rooms = create_list();
	if (defname == NULL) {
		if ((defname = (char *) malloc(sizeof(char)*strlen(DEFAULT))) == NULL) {
			perror("ERROR malloc defname");
			exit(1);
		}
		defname = DEFAULT;
	}
	
	if (! (add_room(rooms, defname))) 
		perror("ERROR adding room");

	return rooms;	
}

bool room_is_in(char *name, list l) {
	box *temp = l->first;
	while ((temp != NULL) && (strcmp(((room *)temp->elem)->name, name) != 0))
		temp = temp->next;

	return (temp != NULL);
}
