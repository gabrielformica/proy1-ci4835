#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 


int main(int argc, char *argv[]) {
	char opts;
	while ((opts = getopt(argc, argv, ":h:p:n:a:")) != -1) {
		switch (opts) {
			case 'h': 
				printf("Flag -h, with parameter %s\n",optarg);
				break;
			case 'p': 
				printf("Flag -p, with paremeter %s\n",optarg);
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
