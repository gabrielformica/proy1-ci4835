/**
  * @file
  * @author Gabriel Formica <gabriel@ac.labf.usb.ve>
  * @author Melecio Ponte <melecio@ac.labf.usb.ve> 
  *
  * @section Descripcion
  *
  * Generic structure of simple linked list 
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
  * Create an empty list
  * @param None.
  * @return Empty list
  */

extern list create_list();

/**
  * Add an element to end of the list
  * @param l: the list
  * @param n: Element that is going to be added
  * @return the list, and it is a valid list
  */

extern box *add(list,void *);

/**
  * Delete an element of the list
  * @param The list. It is a valid list
  * @return true if the element is deleted, false in other case
  */

extern bool del(list,void *);

/**
  * Get the amount of elements in the list
  * @param The list. It is a valid list
  * @return the amount of elements in the list
  */

extern int get_size(list);

#endif
