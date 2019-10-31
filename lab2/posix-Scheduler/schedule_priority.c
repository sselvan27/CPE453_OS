#include "schedulers.h"
#include "cpu.h"
#include "task.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Get highest priority task from linked list
struct node *getHighest(struct node **head)
{
	struct node *temp;
	struct node *highest = NULL;

	temp = *head;
	
	while(temp != NULL)
	{
		if(highest == NULL)
		{
			// Highest priority job is undefined, replace with temp
			highest = temp;
		}
		else
		{
			// If the current job is higher priority than highest, replace
			// highest with temp
			if(temp->task->priority >= highest->task->priority)
			{
				highest = temp;
			}
		}
		temp = temp->next;
	}
	// Delete highest job from linked list
	delete(head, highest->task);
	// Return new copy of highest job
	return highest;
}

// Run all scheduled tasks
void schedule(struct node **head)
{	struct node *curNode;
	Task *curTask;
	// Run tasks until there are no more tasks to run (empty head)
	while(*head != NULL)
	{
		// Get next highest priority job
		curNode = getHighest(head);
		curTask = curNode->task;
		// Run task in full (for the entire CPU burst time)
		run(curTask, curTask->burst);
		printf("Task %s finished.\n", curTask->name);
	}
}