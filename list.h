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
  * @param elem: the element it is desired to add
  * @return The pointer to the added element
  */

extern box *add(list,void *);

/**
  * Look if an element is in a list
  * @param elem: the element it is desired to find
  * @param l: The list. It is a valid list
  * @return True if the element is in list. False in any other case
  */


extern bool is_in(void * elem, list l);

/**
  * Get the amount of elements in the list
  * @param The list. It is a valid list
  * @return The amount of elements in the list
  */

extern int get_size(list);

/**
  * Delete an element of the list
  * @param The list. It is a valid list
  * @return True if the element is deleted. False in any other case
  */

extern bool del(list,void *);

/**
  * Destroy the list. 
  * @param l: The list. It is a valid list
  * @return Anything, but the list is initialized
  */

extern void destroy(list);


#endif
