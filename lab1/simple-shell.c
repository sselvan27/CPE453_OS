/**
 * Authors: Kent Zhang, James Abundis
 * CSC 453 Operating Systems Lab 1 Spring 2019
 *
 * Simple shell interface program.
 *
 * Operating System Concepts - Tenth Edition
 * Copyright John Wiley & Sons - 2018
 */
#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


#define MAX_LINE		80 /* 80 chars per line, per command */
#define READ          0
#define WRITE         1

void parseInput(char* (*args)[MAX_LINE/2 + 1], int *argc);
int executeCommand(pid_t *pid, char *argv[41], int startpipe, int endpipe, int curr);
int executeRedirection(pid_t *pid, int rdindex, int option, char *file, char *arguments[41]);
static void closeFD(int curr, int wFD, int rFD, int endpipe);
static int validPipe(char *string);
int tokenizeCommands(char *command, int start, int end, int curr, int *result, int *waitflag);
int readCommandLine(char (*commandLine)[MAX_LINE]);
void createArrayArgv(char *commandLine, char* (*argv)[41], int *argc);
int pipeline(int argc, char **argv);


int main(void)
{
   char recentcommand[MAX_LINE];
   char commandline[MAX_LINE];
	char *args[MAX_LINE/2 + 1];	/* command line (of 80) has max of 40 arguments */
   int should_run = 1;
   int histflag = 0;
   int argc = 0;
		
    	while (should_run){   
         /* ask for prompt */
        	printf("osh>");
        	fflush(stdout);

         /* parse command line */
         readCommandLine(&commandline);

         /* check for exit function */
         if(!strcmp(commandline, "exit"))
         {
            exit(0);
         }

         /* check for history flag and if recent command not found */
         if (!strcmp(commandline, "!!") && !histflag)
         {
            printf("No recent command found\n");
            continue;
         }

         /* check for history flag and recent command is avail */
         if(!strcmp(commandline, "!!"))
         {
            /* create parsed array of arguments for recent command */
            createArrayArgv(recentcommand, &args, &argc);

            /* runs command/ commands (pipeline) */
            pipeline(argc, args);
         }else
         {
            /* create parsed array of arguments for new command */
            createArrayArgv(commandline, &args, &argc);

            /* runs command/ commands (pipeline) */
            pipeline(argc, args);

            /* copy new command into recent command buffer */
            memcpy(recentcommand, commandline, sizeof(commandline)); 
         }

         /* set history flag after new command */
         histflag = 1;
        
        	/**
         	 * After reading user input, the steps are:
         	 * (1) fork a child process
         	 * (2) the child process will invoke execvp()
         	 * (3) if command includes &, parent and child will run concurrently
         	 */
    	}
    
	return 0;
}

/* create child process to execute commands */
/* takes into account pipe location */
int executeCommand(pid_t *pid, char *argv[41], int startpipe, int endpipe, int curr)
{
   int fd[2];

   pipe(fd);

   if((*pid = fork()) < 0)
   {
      fprintf(stderr, "simple-shell: Invalid pipe\n");
      return -1;
   }
   else if(*pid == 0)
   {
      close(fd[READ]);
      /* check if command is at beginning of pipe */
      if(curr == 0 && startpipe == 1 && endpipe == 0)
      {
         dup2(fd[WRITE], STDOUT_FILENO);
      }
      /* check if command is somewhere in the middle of pipe */
      else if(curr != 0 && startpipe == 0 && endpipe == 0)
      {
         dup2(curr, STDIN_FILENO);
         dup2(fd[WRITE], STDOUT_FILENO);
      }
      /* check if command is at the end of pipe */
      else
         dup2(curr, STDIN_FILENO);

      if(execvp(argv[0], argv) == -1)
      {
         fprintf(stderr, "simple-shell: %s: Command not found\n", argv[0]);
         return -1;
      }
   }

   /* close necesasry file descriptors */
   closeFD(curr, fd[WRITE], fd[READ], endpipe);

   return fd[READ];
}

/* handles redirection of command outputs to file */
/* uses file as input for commands */
int executeRedirection(pid_t *pid, int rdindex, int option, char *file, char *arguments[41])
{
   int fd;

   if((*pid = fork()) < 0)
   {
      fprintf(stderr, "simple-shell: Invalid pipe\n");
      return -1;
   }
   else if(*pid == 0)
   {
      /* output > */
      if(option == 1)
      {
         if((fd = open(file, O_TRUNC | O_CREAT | O_WRONLY, 0666)) == -1)
         {
            fprintf(stderr, "simple-shell: Unable to open file for input\n");
            return -1;
         }
         dup2(fd, STDOUT_FILENO);
         close(fd);
      }

      /* input < */
      if(option == 2)
      {
         if((fd = open(file, O_RDONLY, 0666)) == -1)
         {
            fprintf(stderr, "simple-shell: Unable to open file for output\n");
            return -1;
         }
         dup2(fd, STDIN_FILENO);
         close(fd);
      }

      if(execvp(arguments[0], arguments) == -1)
      {
         fprintf(stderr, "simple-shell: %s: Command not found\n", arguments[0]);
         return -1;
      }
   }

   waitpid(*pid, 0, 0);
   return 0;
}

/* close neccesary file descriptors */
static void closeFD(int curr, int wFD, int rFD, int endpipe)
{
   if(curr != 0)
   {
      close(curr);
   }

   close(wFD);

   if(endpipe == 1)
   {
      close(rFD);
   }
}

/* parse arguments for each command and check for redirection */
/* checks for '&', if user wants to run parent and child concurrently */
int tokenizeCommands(char *command, int start, int end, int curr, int *result, int *waitflag)
{
   pid_t pid;
   int j, rdindex, redirection, option;
   char *arguments[41], *argv;
   j = redirection = option = 0;

   argv = strtok(command, " ");
   while(argv != NULL)
   {
      if(!strcmp(argv, ">"))
      {
         option = 1;
         redirection = 1;
         rdindex = j;
         break;
      }
      else if(!strcmp(argv, "<"))
      {
         option = 2;
         redirection = 1;
         rdindex = j;
         break;
      }
      else if(!strcmp(argv, "&"))
      {
         *waitflag = 1;
      }
      else
      {
         arguments[j++] = argv;
      }

      if(j == 41)
      {
         fprintf(stderr, "cshell: %s: Too many arguments", arguments[0]);
         return -1;
      }
      argv = strtok(NULL, " ");
   }

   arguments[j] = NULL;

   if(redirection)
   {
      argv = strtok(NULL, " ");

      if(executeRedirection(&pid, rdindex, option, argv, arguments) == -1)
         return -1;
   }
   else
   {
      if((*result = executeCommand(&pid, arguments, start, end, curr)) == -1)
         return -1;
   }

   return pid;
}

/* parse user input for command line */
int readCommandLine(char (*commandLine)[MAX_LINE])
{
   char line[MAX_LINE];
   int c, errflag, i;
   errflag = i = 0;

   while(1)
   {
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

   if(errflag)
   {
      fprintf(stderr, "simple-shell: Command line too long\n");
      return 1;
   }

   strcpy(*commandLine, line);

   return 0;
}

/* parse command line into array of arguments */
/* assumes maximum number of commands is 10 */
void createArrayArgv(char *commandLine, char* (*argv)[41], int *argc)
{
   char *argument;
   int count = 0;

   argument = strtok(commandLine, "|");
   
   while(argument != NULL)
   {
      if(!validPipe(argument))
      {
         fprintf(stderr, "simple-shell: Invalid pipe\n");
         break;
      }

      if(count == 10)
      {
         fprintf(stderr, "simple-shell: Too many commands\n");
         break;
      }

      (*argv)[count++] = argument;

      argument = strtok(NULL, "|");
   }
   (*argv)[count] = NULL;

   *argc = count;
}

/* error checking for pipeline */
static int validPipe(char *string)
{
   int i, result;
   result = 0;

   for(i = 0; i < strlen(string); i++)
   {
      if(string[i] != ' ')
      {
         result = 1;
      }
   }

   return result;
}

/* loop through all commands and parses them individually */
int pipeline(int argc, char **argv)
{
   pid_t pid;
   int i, curr, start, end, result, waitflag;

   result = curr = end = i = waitflag = 0;
   start = 1;

   for(i = 0; i < argc; i++)
   {
      if(i == (argc - 1))
      {
         end = 1;
      }

      curr = result;

      if((pid = tokenizeCommands(argv[i], start, end, curr, &result, &waitflag)) == -1)
      {
         return 1;
      }

      start = 0;
   }

   if(!waitflag)
   {
      waitpid(pid, 0, 0);
   }

   return 0;
}