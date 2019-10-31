#include <stdlib.h>

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#define MAX_LINE 80

typedef struct block {
	int size;
	int offset;
	struct block *prev;
	struct block *next;
	int free;
	char p_name[30];
} s_block;

// HELPER FUNCTIONS
void checkArgs(int argc, char *argv[]);
s_block *initBlock(int size);
int getLine(char (*prompt)[MAX_LINE]);
int tokenizePrompt(char *prompt, char* (*args)[4], int *count);
void insertBlock(int size, char * p_name, int offset, s_block **head, s_block **temp);
void modifyOffsets(s_block **head);

// EXECUTE COMMAND FUNCTIONS
void execute(char *args[4], int count, s_block **head, int max_size);
int executeRequest(char **args, int argc, s_block **head);
int executeRelease(char **args, int argc, s_block **head);
int executeStat(int argc, s_block **head);
int executeCompaction(int argc, s_block **head, int max_size);

// REQUEST ALGORITHMS
int firstFit(int size, char * p_name, s_block **head);
int bestFit(int size, char * p_name, s_block **head);
int worstFit(int size, char *p_name, s_block **head);

#endif
