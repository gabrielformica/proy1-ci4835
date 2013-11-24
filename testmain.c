#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "list.h"
#include "roomslist.h"

void error();
	
int main(int argc, char *argv[]) {
  list rooms = initialize_rooms(NULL);
  list rooms2 = initialize_rooms("elMac");
}
