#include "schedulers.h"
#include "cpu.h"
#include "task.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Get shortest task from linked list
struct node *getShortest(struct node **head)
{
	struct node *temp;
	struct node *shortest = NULL;

	temp = *head;
	
	while(temp != NULL)
	{
		if(shortest == NULL)
		{
			// Shortest job is undefined, current node is set to shortest
			shortest = temp;
		}
		else
		{
			// If the current job is shorter than the shortest job, replace
			// shortest with temp
			if(temp->task->burst <= shortest->task->burst)
			{
				shortest = temp;

			}
		}
		temp = temp->next;
	}
	// Delete shortest job from linked list
	delete(head, shortest->task);
	// Return new copy of shortest job
	return shortest;

}

// Run all scheduled tasks
void schedule(struct node **head)
{	struct node *curNode;
	Task *curTask;
	// Run tasks until there are no more tasks to run (empty head)
	
	while(*head != NULL)
	{
		
		// Pop off shortest job in linked list
		curNode = getShortest(head);
		curTask = curNode->task;
		// Run task in full (for the entire CPU burst time)
		run(curTask, curTask->burst);
		printf("Task %s finished.\n", curTask->name);
	}
}