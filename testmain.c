#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "list.h"
#include "roomslist.h"

void error();

void addtenusers(list l, char* roomname) {
  int i;
  for (i = 0; i < 10; i++) {
    char *nombre = malloc(sizeof(char)*2);
    sprintf(nombre, "%d", i);
    printf ("Se intentara agregar usuario -%s-\n",nombre);
    
    if (!(add_user(l, roomname, nombre))) {
      printf ("---NO puedo agregar el usuario %d, marik\n",i);
      exit(1);
    }
    printf ("---Agregue usuario %s\n",nombre);
  }
}


void addtenrooms(list l) {
  int i;
  for (i = 0; i < 10; i++) {
    char *nombre = malloc(sizeof(char)*2);
    sprintf(nombre, "%d", i);
    printf ("Se intentara agregar sala -%s-\n",nombre);
    if (!(add_room(l,nombre))) {
      printf ("---Mira, no puedo agregar la sala -%s-\n",nombre);
      exit(1);
    }
    printf ("---Se agrego la sala %s\n",nombre);
  }
}
	
int main(int argc, char *argv[]) {
  list rooms = initialize_rooms(NULL);
  addtenrooms(rooms);
  printf ("cantidad de salas:  %d\n",get_size(rooms));

  addtenusers(rooms,"0");
  printf("tamanion de la sala 0: %d\n",get_size(((room *)rooms->first->next->elem)->users));
  
}
 
