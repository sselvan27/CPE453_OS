#include "schedulers.h"
#include "cpu.h"
#include "task.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


// Traverses to end of linked list to ensure FIFO
struct node *getNext(struct node **head){
	struct node *temp;
	struct node *end = NULL;
	temp = *head;

	while(temp->next != NULL)
	{
		temp = temp->next;
	}

	end = temp;
	delete(head, temp->task);

	return end;
}

// Run all scheduled tasks
void schedule(struct node **head)
{	struct node *curNode;
	Task *curTask;
	// Run tasks until there are no more tasks to run (empty head)
	while(*head != NULL)
	{
		// Pop off next node in linked list (FIFO)
		curNode = getNext(head);
		curTask = curNode->task;
		// Run task in full (for the entire CPU burst time)
		run(curTask, curTask->burst);
		printf("Task %s finished.\n", curTask->name);
	}
}


