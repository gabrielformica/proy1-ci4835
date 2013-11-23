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
#include "myfunctions.h"

/**
  * Crea una cola vacia.
  * @param c Cola a crear.
  * @return La cola es una cola valida.
  */

list create_list(list c) {
	c = allocate(c, sizeof(list));
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

list add(list c, void *n) {
	box *temp;

	temp = allocate(temp, sizeof(box));
	temp->elem = n;
	temp->next = NULL;
	if (c->size == 0) {
		c->first = temp;	
		++(c->size);
		c->last= temp;
		return c;
	}
	else {
		(c->last)->next = temp;
		c->last = temp;
		++(c->size);
	}
	return c;
}

/**
  * Desencola un elemento de la cola.
  * @param c Cola en question. La cola es una cola valida.
  * @return El primer elemento de la cola, o null si es vacia
  */

void *del_elem(list c, void *elem) {
	return NULL;
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

int get_size(list c){
	return c->size;
}
