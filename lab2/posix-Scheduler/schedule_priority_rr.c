#include "schedulers.h"
#include "list.h"
#include "cpu.h"
#include "task.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Traverses to end of linked list to ensure FIFO
struct node *getNext(struct node **head)
{
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

// Run round robin algorithm scheduler
void schedule_rr(struct node **head)
{
	struct node *curNode;
	Task *curTask;
	int quantum = 10;
	// Run tasks until there are no more tasks to run (empty head)
	while(*head != NULL)
	{
		//Pop off next node in linked list (FIFO)
		//curNode = getNext(head);
		curNode = *head;
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

// Create sub linked list for tasks of equal priority
void subQueue(struct node **head, int cur_priority)
{
	struct node *temp;
	struct node *newHead = NULL;
    temp = *head;

    while (temp != NULL)
    {
        if (temp->task->priority == cur_priority)
        {
        	// Add to subque for RR
        	insert(&newHead, temp->task);
        	// Delete from original queue
        	delete(head, temp->task);

        }
        temp = temp->next;
    }

    // Call to RR
    schedule_rr(&newHead);
}

// Get highest priority number from linked list
int getHighest(struct node **head)
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
	
	// Return highest priority task
	return highest->task->priority;
}

// Run all scheduled tasks
void schedule(struct node **head)
{	
	int curHighest;
	
	// Run tasks until there are no more tasks to run (empty head)
	while(*head != NULL)
	{
		// Get next highest priority job
		curHighest = getHighest(head);
		subQueue(head, curHighest);
	}
}