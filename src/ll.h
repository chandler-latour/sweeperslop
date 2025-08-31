//a file to add singly-linked lists to the program
#ifndef SLL
#define SLL

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct s_sll {
	void * data;
	void (*f_freedata)(void *);
	struct s_sll * next;
} t_sll;


/*
 * This function mallocs a singly linked list node.
 * It needs a pointer to data to point to, and a pointer to the function
 * used to free that data, assuming the data is defined in the heap.
 */

t_sll * newSLL(void *, void *(void *));


/*
 * This function accepts a pointer to a singly-linked list node,
 * and frees the one that comes AFTER THAT NODE. This is done because
 * a singly-linked list node can't reference the previous one. 
 * Here because it's entirely obligatory, but 
 */ 
void freeNextSLL(t_sll *);

/*
 * This function "empties" a SSL node instead of freeing it.
 * This means the node is left entirely as is, but the data inside is freed and set to
 * a void pointer. This way, it can be re-assigned to something else.
 * Use this function to cut down on freeing and mallocing and freeing repeatedly.
 */
void emptySLL (t_sll *);


/*
 * Inserts an existing SLL node into a list.
 * First argument is the new data to insert,
 * second argument is the target to be appended to.
 * this function doesn't care if the LL node has data in it or not,
 * but does assert that both arguments are non-NULL, and that 
 * the "next" pointer of the new node IS NULL.
 */

void insertSLL (t_sll *, t_sll *);

/*
 * Yoinks a singly linked list out of the tree without freeing or emptying it
 * does this to the one AFTER the argument, not the argument itself, so as to protect
 * the pointers and make sure they're still set up right.
 * great if you have one item in one linked list and want to move it to another list, 
 * because this function sets target->next to NULL and kills the original pointer pointing to it.
 * returns a pointer to the one that got yoinked!
 */
t_sll * yoinkNextSLL (t_sll *);

#endif
