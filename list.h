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

#ifndef LIST_H 
#define LIST_H 

typedef enum { false, true } bool;
typedef struct box box;
typedef struct head head;

struct box {
	void *elem;
	box *next;
};

struct head {
	int size;
	box *first;
	box *last;
};

typedef struct head *list;


/**
  * Crea una cola vacia.
  * @param c Cola a crear.
  * @return La cola es una cola valida.
  */

extern list create_list();


/**
  * Anade un elemento al final de cola. 
  * @param c Cola en question.
  * @param n Elemento que se desea agregar.
  * @return La cola c, y c es una cola valida.
  */

extern box *add(list,void *);

/**
  * Desencola un elemento de la cola.
  * @param c Cola en question. La cola es una cola valida.
  * @return El primer elemento de la cola, o null si es vacia
  */

extern void *del_elem(list,void *);

/**
  * Devuelve el tamanio de la cola.
  * @param c Cola en question. La cola es una cola valida.
  * @return Tamanio de la cola.
  */

extern int get_size(list);
#endif
