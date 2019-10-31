#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lwp.h"

context ptable[PROCESS_LIM];
int process_counter = 0;

tid_t lwp_create(lwpfun fun, void * arguments, size_t ssize){
	context ptEntry;
	tid_t *stackptr, *baseptr, *stack;

	if (process_counter >= PROCESS_LIM){
		return -1;
	}

	// Allocate stack
	stack = malloc(sizeof(tid_t) * ssize);

	// Create process table entry
	ptEntry.tid = ++process_counter;
	ptEntry.stack = stack;
	ptEntry.stacksize = ssize;
	ptEntry.stackptr = NULL;


	// pointer to thread stack
	stackptr = stack + ssize;

	// add argument to stack
	*stackptr = (tid_t) arguments;

	// decrement stack pointer
	stackptr--;

	// add function to stack
	*stackptr = (tid_t) fun;

	// decrement stack pointer
	stackptr--;

	// add register space
	*stackptr = REG_SPACE;

	// save base pointer position
	baseptr = stackptr;

	// decrement stack pointer
	stackptr--;

	// save 6 registers
	int i = 0;
	while(i < 6){
		*stackptr = i + 1;
		stackptr--;
		i++;
	}

	// update base pointer
	*stackptr = (tid_t) baseptr;

	// update current stack pointer of entry
	ptEntry.stackptr = stackptr;

	// Insert entry into process table
	ptable[ptEntry.tid - 1] = ptEntry;

	// Add to queue here for scheduler

	return ptEntry.tid;

}