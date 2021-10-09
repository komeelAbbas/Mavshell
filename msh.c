/*
Name : komeel Abbas
Student ID: 1001760310
*/


// The MIT License (MIT)
// 
// Copyright (c) 2016, 2017, 2021 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments
#define MAXCOMMANDS 15
#define MAXPIDS 15
int pidcount=0; // an int variable to keep track of the pids
int pids[15]; //an array to keep track of all the newly created pid values by the processes


int main()
{
  int count=0; // count variable to keep track of all the commands entered
  char *record[15]; //record of string
  pid_t showpids[MAXPIDS];
  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );

//the for loop dynamically allocate memory to the 15 records in the char pointers
int i;
  for(i=0;i<15;i++)
  {
        record[i]=(char*)malloc(MAX_COMMAND_SIZE); 
  }

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;    

    
    if (count<=14)
    {
            strncpy(record[count++],cmd_str,MAX_COMMAND_SIZE);
    }
    else
    {
        //storing upto 15 commands in the record counter 
        //i is incremented to move to the next address
        for(i=0; i<14; i++)
        {
            strncpy(record[i],record[i+1],MAX_COMMAND_SIZE);
        }
        strncpy(record[14], cmd_str, MAX_COMMAND_SIZE);
    }
    char *arr[1];
        if(cmd_str[0] == '!')
        {
            arr[0]= strdup(cmd_str);

            int x=atoi(&arr[0][1]);
            if(!(x<=count)||(x<1 && x>15))
                printf("Command not in history \n");
            else
            {
                if (count>=15)
                {
                    count--;
                    strcpy(cmd_str,record[x-2]);
                    strcpy(record[count++],cmd_str);
                }
                else
                {
                    count--;
                    strcpy(cmd_str,record[x-1]);
                    strcpy(record[count++],cmd_str);
                }
            }
        }

                                                           
    char *working_str  = strdup( cmd_str );                

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input strings with whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // Now print the tokenized input as a debug check

    //we check for comparision with command line prompt if it is equal to cd, then comparision is true
    if (token[0] == NULL)
    { 
       continue;     
    }

    else if (strcmp( "cd",token[0]) == 0 && token[1]!= NULL)
    {
        int y = chdir(token[1]);

        if (y == -1)
        {
            printf(": Command not found\n");
        }

    }
//token[0] is compared with command line input.
//if the pid value and command line input is equal pids values upto 15 is listed
    else if (strcmp(token[0], "showpids") == 0)
    {
        for (i = 0; i < pidcount; i++)
        {
             printf("%d: %d\n", i + 1, pids[i]);
        }
    }
// check command line input with quit and exit and if its true exit the program
    else if (strcmp(token[0], "quit") == 0 || strcmp(token[0], "exit") == 0)
    {
        exit(1);
    }

    ///token[0] is compared with command line input.
    //if the history and command line input is equal then it list history for upto or lesser than 15 commands

    else if (strcmp(token[0], "history") == 0)
    {
         for (i = 0; i < count; i++) 
         {
             printf("%d: %s\n", i + 1, record[i]);
         }
    }
    else //new process if if condition not met
    {
        pid_t pid = fork();
            
        if (pid = 0)
        {
            //if child process it will run and you can as namy NULLs on the end as wished
            //if command not found print error message
             int ret = execvp(token[0], &token[0]);
            if (ret == -1)
            {
                printf("%s: command not found\n", token[0]);
                break;
            }
        }

        //return child pid to the parent
         else
        {
                

            if (pidcount <= 14)
            {
                pids[pidcount++] = pid;
            }
            //if pids are greater than 15 they are added in an array
            else
            {
                for (k = 0; k < 14; k++)
                {
                    pidhistory[k] = pidhistory[k + 1];
                }
                //the new process's is added and it's pid is stored to the end of an array
                    pidhistory[14] = pid;
            }

                int status;
                //wait for child to finish first
                wait(&status);
        }
    }

    free( working_root );

  }
  return 0;
}