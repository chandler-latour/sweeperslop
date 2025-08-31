#include "ll.h"

//assumption: ALL LINKED LIST NODES ARE DEFINED IN THE HEAP
//therefore they must be freed when we're done with them.

t_sll * newSLL(void * dp, void * ffp()) {
	t_sll * ll = malloc(sizeof(t_sll));
	assert(ll != NULL);
	ll->data = dp;
	ll->f_freedata = ffp();
	ll->next = NULL;
	return ll;
}

void freeNextSSL(t_sll * prior) {
	t_sll * target = yoinkNextSLL(prior);
	emptySLL(target);
	free(target);
}

void emptySLL(t_sll * target) {
	assert(target != NULL);
	assert(target->f_freedata != NULL);
	if (target->data == NULL) {
		return;
	}
	target->f_freedata(target->data);
	target->f_freedata = NULL;
	target->data = NULL;
}


void insertSLL(t_sll * newbie, t_sll * target) {
	assert(newbie != NULL);
	assert(target != NULL);
	assert(newbie->next == NULL);
	if (target->next != NULL) {
		newbie->next = target->next;
	}
	target->next = newbie;
}

t_sll * yoinkNextSLL(t_sll * prior) {
	assert(prior != NULL);
	assert(prior->next != NULL);
	t_sll * target = prior->next;
	prior->next = target->next;
	target->next = NULL;
	return target;
}



