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

   if ( temp  = get_room(rooms, roomname) != NULL  ) {
      write(sock, "Room already exists", 256);
      return;
   }
   temp = NULL;

   if (temp = add_room(rooms, roomname) == NULL) {
      write(sock, "Room already exists", 256);
      return;
   }
   
}
 



//funcion men $1
//


//funcion sus

