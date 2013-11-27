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
};

extern box *add_user(list, char *, void *);
extern box *add_room(list, char *);
bool room_is_in(char *, list);
extern list initialize_rooms(char *);
extern user_data *create_user_data(char *, int);
extern box *get_room(list,char *);

#endif
