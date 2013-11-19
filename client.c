#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void allocate(void *x, int size) {
  if ((x = malloc(size)) == NULL) {
    perror("Error malloc");
    exit(1);
  }
}


int main(int argc, char *argv[]) {
	char opts;
    char *portno;
	while ((opts = getopt(argc, argv, ":h:p:n:a:")) != -1) {
		switch (opts) {
			case 'h': 
				printf("Flag -h, with parameter %s\n",optarg);
				break;
			case 'p':
              allocate(portno,40);
              portno[3] = 48;
              
              printf("Flag -p, with paremeter %d\n", portno[3]);
				break;
			case 'n': 
				printf("Flag -n, with paremeter %s\n",optarg);
				break;
			case 'a': 
				printf("Flag -a, with paremeter %s\n",optarg);
				break;
			case '?':
				printf("ERROR with flags\n");
		}
	}
}
