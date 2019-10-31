#include "schedulers.h"
#include "cpu.h"
#include "task.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Traverses to end of linked list to ensure FIFO
struct node *getNext(struct node **head){
	struct node *temp;
	struct node *end;
	temp = *head;

	while(temp->next != NULL)
	{
		temp = temp->next;
	}
	end = temp;

	return end;
}

// Run all scheduled tasks
void schedule(struct node **head)
{	struct node *curNode;
	Task *curTask;
	int quantum = 10;
	// Run tasks until there are no more tasks to run (empty head)
	while(*head != NULL)
	{
		// Pop off next node in linked list (FIFO)
		curNode = getNext(head);
		curTask = curNode->task;
		// Run task in full (for the entire CPU burst time)
		if(curTask->burst > quantum)
		{
			run(curTask, quantum);
			curNode->task->burst -= quantum;
			// Delete partially processed task from linked list
			delete(head, curNode->task);
			// Re-insert at the end of linked list for later processing
			insert(head, curNode->task);
		}
		else
		{
			run(curTask, curTask->burst);
			printf("Task %s finished.\n", curTask->name);
			delete(head, curNode->task);
	    }
	}
}


