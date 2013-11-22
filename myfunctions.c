#include "myfunctions.h"
#include <stdlib.h>


void allocate(void *x, int size) {
  if ((x = malloc(size)) == NULL) {
    perror("Error malloc");
    exit(1);
  }
}
