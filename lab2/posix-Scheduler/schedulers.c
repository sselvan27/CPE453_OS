#include "schedulers.h"
#include "cpu.h"
#include <string.h>
#include <stdlib.h>

void add(char *name, int priority, int burst, struct node **head)
{
	Task *newTask = malloc(sizeof(Task));
    newTask->name = name;
	newTask->priority = priority;
	newTask->burst = burst;

	insert(head, newTask);
}

