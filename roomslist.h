#include "list.h"

#ifndef ROOMSLIST_H
#define ROOMSLIST_H


typedef list userslist;
typedef struct room room;

struct room {
	char *name;
	userslist users;
};

extern box *add_user(list, char *, char *);
extern box *add_room(list, char *);
bool room_is_in(char *, list);
extern list initialize_rooms(char *);

#endif
