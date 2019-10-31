#include "allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int main(int argc, char *argv[])
{
    char prompt[MAX_LINE];
    char *args[4];
    int count = 0;

    checkArgs(argc, argv);

    // assume arguments are correct
    // create head of empty block size
    s_block *head = initBlock(atoi(argv[1]));

    // main loop
    while (1){
        printf("allocator>");
        fflush(stdout);

        getLine(&prompt);
        tokenizePrompt(prompt, &args, &count);
        execute(args, count, &head, atoi(argv[1]));
    }

    return 0;
}

// check for correct arguments
void checkArgs(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s [memory size]\n", argv[0]);
        exit(1);
    }
}

// initialize a new block of memory
s_block *initBlock(int size)
{
    s_block *head = NULL;
    head = malloc(sizeof(s_block));

    // set empty block size
    head->size = size;
    head->free = 1;
    head->prev = NULL;
    head->next = NULL;

    return head;
}

// parse user input for command line
int getLine(char (*prompt)[MAX_LINE])
{
   char line[MAX_LINE];
   int c, errflag, i;
   errflag = i = 0;

    while(1){
        c = getchar();

        if(c == '\n' || c == EOF)
        {
         line[i] = '\0';
         break;
        }
        else
         line[i] = c;

        if(i == MAX_LINE)
         errflag = 1;

        i++;
   }

   if(errflag){
      fprintf(stderr, "allocator: command line too long max char 80\n");
      return 1;
   }

   strcpy(*prompt, line);

   return 0;
}

// Create array of arguments from user input
int tokenizePrompt(char *prompt, char* (*args)[4], int *count)
{
   char *argument;
   int i = 0;

   argument = strtok(prompt, " ");

   while(argument != NULL){
        (*args)[i++] = argument;
        argument = strtok(NULL, " ");

        if (i > 4){
            printf("Too many arguments\n");
            return 1;
        }
    }

    *count = i;

   return 0;
}

// Initialize new node and insert into given position
void insertBlock(int size, char * p_name, int offset, s_block **head, s_block **temp)
{
   s_block *newBlock = initBlock(size);
   newBlock->free = 0;
   strcpy(newBlock->p_name, p_name);
   newBlock->offset = offset;

   //Check for case that we are inserting at start of list
   if((*temp)->prev != NULL){
        //Set the previous memory block of the empty block to the previous of the newly inserted block 
        newBlock->prev = (*temp)->prev;
        (*temp)->prev->next = newBlock;
   }
   // Need to make the inserting memory block the head, if inserting at front
   else{
        *head = newBlock;
   }

   // Check the case that the current process fits exactly into empty block
   if(size == (*temp)->size){

        // Check for the case that we are inserting at end of list
        if((*temp)->next != NULL){
         newBlock->next = (*temp)->next;
         (*temp)->next->prev = newBlock;
        }
        /* Free empty block from memory as it is now filled */
        free(*temp);
   } 
   else{
        newBlock->next = (*temp);
        (*temp)->prev = newBlock;

        //Adjust size and offset of temp
        (*temp)->offset += size;
        (*temp)->size -= size;
   }
}

// Correct offsets in a given linked list of memory
void modifyOffsets(s_block **head)
{
    int offset = 0;
    s_block *temp = *head;

    while(temp != NULL){
        temp->offset = offset;
        offset += temp->size;
        temp = temp->next;
    }
}

// Case statements for various commands: Request, Release, Compaction, Statistics
void execute(char *args[4], int count, s_block **head, int max_size)
{
    if(!strcmp(args[0], "RQ")){
        executeRequest(args, count, head);
    }
    else if(!strcmp(args[0], "RL")){
        executeRelease(args, count, head);
    }
    else if(!strcmp(args[0], "C")){
        executeCompaction(count, head, max_size);
    }
    else if(!strcmp(args[0], "STAT")){
        executeStat(count, head);
    }
    else if(!strcmp(args[0], "X")){
        exit(1);
    }
    else{
        printf("Invalid command\n");
    }
}

// Execute request command: Insert block of memory based on type of algorithm
int executeRequest(char **args, int argc, s_block **head)
{   
    char req_type;
    int size;
    char *p_name;
    if (argc != 4){
        printf("Usage: RQ [Name] [Size] [Type]\n");
        return 1;
    } 
    
    p_name = args[1];
    req_type = args[3][0];
    size = atoi(args[2]);

    // Make new block for process
    switch(req_type){
       case 'F':
          firstFit(size, p_name, head);
          break;
       case 'B':
          bestFit(size, p_name, head);
          break;
       case 'W':
          worstFit(size, p_name, head);
          break;
       default :
          printf("Allocation types: 'F','B','W'\n");
    }     
    return 0;
}

// Execute release command: Free given process in linked list
int executeRelease(char **args, int argc, s_block **head)
{
   char *p_name;
   s_block *temp = *head;
   if (argc != 2){
      printf("Usage: RL [Name]\n");
      return 1;
   }

   p_name = args[1];
   
   while(temp != NULL){
        if(!strcmp(p_name, temp->p_name)){
            //Case for deleting at beginning of linked list and non empty right adjacent block
            if(temp->prev == NULL && temp->next->free == 0){
                temp->free = 1;
            }
            //Case for deleting at beginning of linked list and empty right adjacent block
            else if(temp->prev == NULL && temp->next->free == 1){
                // Next is now the head of the linked list, set prev to null
                temp->next->prev = NULL;
                //Adjust offset and empty block size
                temp->next->offset -= temp->size;
                temp->next->size += temp->size;
                //Make next empty block the new head
                *head = temp->next;
                free(temp);  
            }
            // Case for deleting at end of list and non empty left adjacent
            // block
            else if(temp->next == NULL && temp->prev->free == 0){
                temp->free = 1;        
            }
            //Case for deleting at end of linked list and empty left
            //adjacent block
            else if(temp->next == NULL && temp->prev->free == 1){
                // Fix links
                temp->prev->next = NULL;
                temp->prev->size += temp->size;
                free(temp);
            }
            // Case for left adjacent empty memory block
            // In this case we join removed process to left empty block
            else if(temp->prev->free == 1 && temp->next->free == 0){
                // Adjust links of previous empty block
                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;

                //Increase previous empty block size to account for deletion
                temp->prev->size = temp->prev->size + temp->size;
                // Free released process from memory
                free(temp);
                
            }
            // Case for right adjacent memory block
            // In this case we join removed process to right empty block
            else if(temp->prev->free == 0 && temp->next->free == 1){
                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;
                //Increase next empty block size to account for deletion
                temp->next->size = temp->next->size + temp->size;
                // Adjust the offset to be the offset of the deleted process
                temp->next->offset = temp->offset;
                free(temp);
            }
            // Case for both adjacent blocks are empty
            // In this case we join left, deleted process, right into one empty block
            else if(temp->prev->free == 1 && temp->next->free == 1){
                //Adjust links
                temp->prev->next = temp->next->next;
                temp->next->next->prev = temp->prev;

                //Joining blocks to left empty block, adjust size
                temp->prev->size += (temp->size + temp->next->size);
                
                //Free deleted process block and right adjacent block
                free(temp);
                free(temp->next);
            }
            // Case when both adjacent blocks are filled
            else{
                // Mark memory block as free, remove process name
                temp->free = 1;
            }
            return 0;
        }
        else{
            temp = temp->next;
        }
   }
   
   printf("Process not found in memory\n");
   
   return 1;
}

// Execute statistics command: Print current status of memory in each process
int executeStat(int argc, s_block **head)
{
    s_block *temp = *head;
    if (argc != 1){
        printf("Usage: STAT\n");
        return 1;
    }
    while(temp != NULL){
        if(temp->free == 1)
        {
            if(temp->prev != NULL)
            {
                printf("Addresses [%d:%d] Unused\n",
                temp->offset + 1, temp->offset + temp->size);
            }
            else{
                printf("Addresses [%d:%d] Unused\n", temp->offset, temp->offset + temp->size);
            }

        }
        else{

            if(temp->prev != NULL)
            {
                printf("Addresses [%d:%d] Process %s\n",
                temp->offset + 1, temp->offset + temp->size, temp->p_name);
            }
            else{
                printf("Addresses [%d:%d] Process %s\n",temp->offset, temp->offset + temp->size, temp->p_name);
            }
        }
        temp = temp->next;
    }

    return 0;
}

// Execute compaction command: Combine holes of memory and shift it to the end of linked list
int executeCompaction(int argc, s_block **head, int max_size)
{
    s_block *temp = *head;
    int free_space = 0;

    while(temp != NULL){
        if(temp->free){
            free_space += temp->size;
            // Case for unused block at the beginning
            if (temp->prev == NULL){
                temp->next->prev = NULL;
                *head = temp->next;
                free(temp);
            }
            // Case for unused block in the middle of two processes
            else if (temp->prev != NULL && temp->next != NULL){
                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;
                free(temp);
            }
            // Case for unused block at the end
            else if (temp->next == NULL){
                temp->prev->next = NULL;
                free(temp);
            }
        }
        temp = temp->next; 
    }
    modifyOffsets(head);

    temp = *head;
    // Iterate to end of linked list and add unused block of memory
    while(temp->next != NULL){
        temp = temp->next;
    }
    s_block *newBlock = initBlock(free_space);
    newBlock->prev = temp;
    newBlock->offset = max_size - free_space;
    temp->next = newBlock;

    return 0;
}

// First fit algorithm: insert block of memory into first available sized block
int firstFit(int size, char * p_name, s_block **head)
{
   s_block *temp = *head;
   int offset = 0;
   while(temp != NULL){
        // Check if process can fit in current block
        if(size <= temp->size && temp->free == 1){
         insertBlock(size, p_name, offset, head, &temp);
         return 0;
        }
        offset += temp->size;
        // Go to next block in memory
        temp = temp->next;
      
   }
   printf("Insufficient memory, request rejected\n");

   return 1;

}

// Best fit algorithm: Insert block of memory into minimalist sized free block
int bestFit(int size, char * p_name, s_block **head)
{
    s_block *temp = *head;
    int minSize = INT_MAX;
    int offset = 0;

    // Search for minimum size block that fits process
    while(temp != NULL){
        // Check if process can fit in current block
        if (size <= temp->size && temp->free == 1 && temp->size < minSize){
        // Check if current block is less than minimum size block
            minSize = temp->size;
        }
        // Go to next block in memory
        temp = temp->next;
    }

    // check if minimum size block can fit process
    if (size > minSize){
        printf("Insufficient memory, request rejected\n");
        return 1;
    }

    temp = *head;

    // Search for block with the minimum size block
    while(temp != NULL){
        if (temp->size == minSize && temp->free){
            insertBlock(size, p_name, offset, head, &temp); 
            return 0;
        }

        offset += temp->size;
        // Go to next block in memory
        temp = temp->next;
    }

    return 1;
}

// Worst fit algorithm: Insert block of memory into largest available block
int worstFit(int size, char *p_name, s_block **head)
{
    s_block *temp = *head;
    int offset = 0;
    int max_size = 0;
  
    while(temp != NULL){
        // Check for free block that has sufficient memmory
        // If larger than max chunk size replace max chunk size
        if(temp->free && temp->size >= size && temp->size > max_size){
            max_size = temp->size;
        }
        temp = temp->next;
    }

    if(max_size == 0){
        printf("Insufficent memory, request rejected\n");
        return 1;
    }

    //Have temp point to head
    temp = *head;

    while(temp != NULL){
        if(temp->size == max_size && temp->free){
            insertBlock(size, p_name, offset, head, &temp);
            return 0;
        }
        offset += temp->size;
        temp = temp->next;
         
    }
   
    return 1;
}
