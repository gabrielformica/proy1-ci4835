/**
  * @file
  * @author Gabriel Formica <gabriel@ac.labf.usb.ve>
  * @author Melecio Ponte <melecio@ac.labf.usb.ve> 
  *
  * @section Descripcion
  *
  * Programa que imprime por niveles de izquierda a derecha los nodos
  * de un arbol binario completamente especificado
  */

#include <stdio.h>
#include <stdlib.h>
#include "list.h"


/**
  * Crea una cola vacia.
  * @param c Cola a crear.
  * @return La cola es una cola valida.
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
 * Anade un elemento al final de cola. 
 * @param c Cola en question.
 * @param n Elemento que se desea agregar.
 * @return La cola c, y c es una cola valida.
 */

box *add(list c, void *n) {
	box *temp;

	if ((temp = (box *) malloc(sizeof(box))) == NULL) {
		return NULL;
	}
	temp->elem = n;
	temp->next = NULL;
	if (c->size == 0) {
		c->first = temp;	
		++(c->size);
		c->last= temp;
	}
	else {
		(c->last)->next = temp;
		c->last = temp;
		++(c->size);
	}
	return temp;
}

/**
  * Desencola un elemento de la cola.
  * @param c Cola en question. La cola es una cola valida.
  * @return El primer elemento de la cola, o null si es vacia
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
/*
	if (c->size == 0) 
		return NULL;
	else {
	        void *n;  
		box *temp;
		n = (c->first)->elem;
		temp = c->first;  //Para no perder la direccion
		c->first = (c->first)->next;
		if (c->size == 1) { 
			c->first = NULL;
			c->last = NULL;
		}
		--(c->size);
		free(temp); //Se libera el espacio
	        return n;
	}
*/
}

/**
  * Devuelve el tamanio de la cola.
  * @param c Cola en question. La cola es una cola valida.
  * @return Tamanio de la cola.
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
