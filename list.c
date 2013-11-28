/**
  * @file
  * @author Gabriel Formica <gabriel@ac.labf.usb.ve>
  * @author Melecio Ponte <melecio@ac.labf.usb.ve> 
  *
  * @section Descripcion
  *
  * Generic structure of simple linked list 
  */

#include <stdio.h>
#include <stdlib.h>
#include "list.h"


/**
  * Create an empty list
  * @param None.
  * @return Empty list
  */

list create_list() {
	list c;
	if ((c = malloc(sizeof(head))) == NULL) {
		perror("ERROR creating list:");
		exit(1);
	}
	c->size = 0;
	c->first = NULL;
	c->last = NULL;
	return c;
}

/**
  * Add an element to end of the list
  * @param l: the list
  * @param n: Element that is going to be added
  * @return the list, and it is a valid list
  */

box *add(list l, void *n) {
	box *temp;

	if ((temp = (box *) malloc(sizeof(box))) == NULL) {
		return NULL;
	}
	temp->elem = n;
	temp->next = NULL;
	if (l->size == 0) {
		l->first = temp;	
		++(l->size);
		l->last= temp;
	}
	else {
		(l->last)->next = temp;
		l->last = temp;
		++(l->size);
	}
	return temp;
}

/**
  * Delete an element of the list
  * @param The list. It is a valid list
  * @return true if the element is deleted, false in other case
  */


bool del(list l, void *elem) {
	box *temp = l->first;
	if ((temp != NULL) && (temp->elem == elem)) {
		if (l->size == 1) {
			l->first = NULL;
			l->last = NULL;
		}
		else {
			l->first = l->first->next;
		}
		free(temp);
		l->size = l->size -1;
		return true;
	}
	while ((temp != NULL) && (temp->next != NULL)) {
		if (temp->next->elem == elem)	{
			box *temp2 = temp->next;
			if (l->last == temp2) {
				l->last = temp; 
			}
			temp->next = temp->next->next;
			free(temp2);
			(l->size)--;
			return true;
		}			
		temp = temp->next;	
	}
	return false;
}

/**
  * Get the amount of elements in the list
  * @param The list. It is a valid list
  * @return the amount of elements in the list
  */

int get_size(list c) {
	return c->size;
}

void destroy(list l) {
	box *temp = l->first;
	if (temp == NULL) 
		return;
	while (temp->next != NULL) {
		box *temp2 = temp->next;
		free(temp);
		temp = temp2;
	}	
	l->first = NULL;
	l->last = NULL;
	l->size = 0;
	free(temp);
}
