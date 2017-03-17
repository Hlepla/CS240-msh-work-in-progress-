// msh.c
// Created by Hayden Lepla
// Assignment 3

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

typedef struct history_node
{
  char *entry;
 struct history_node *next;
}his_list;

char *getcommand(); // receives input from user
char **parse_command(char *buff); // takes char buffer and parses the arguments
void fork_func(char **tok_array); // once parsed, we run our arguments to the exec function
char *read_file();
void sub_func();
void alias();
void change_PATH();
void history();
void pipe_func();
void store_list(char **buff, his_list *);

his_list *head = NULL;
his_list *curr = NULL;

int main()
{
 getcommand();

  return EXIT_SUCCESS;
}

char *getcommand()
{
  int command;
  char temp_char;
  head = malloc(sizeof(his_list)); // create the head of the list

  if(head == NULL)
    {
      return (void *)1;
    }


  do
    {
      char *buff = malloc(sizeof(char) * 256); // buffer size of 256
      int i=0;

      printf("\n?: "); // print the prompt for the shell

      while((temp_char = getchar()) != 0xa) // grab argument until the enter key is pushed
    {
      buff[i]=temp_char; // assign our buffer with the arguments entered

      if(strcmp(buff,"exit") == 0) // exit upon the users request
        {
          exit(EXIT_SUCCESS);
        }

      else if(strcmp(buff,"read")==0) // read from file subroutine
        {
          char *ptr_buff="\0", **tok_buff;

          read_file();
          buff = "\0";
        }
      else if(strcmp(buff, "^")==0) // *causes seg fault right now*
        {
          sub_func();
          buff = "\0";
        }
      else if(strcmp(buff,"history")==0)
        {
          history();
          buff = "\0";
        }

      else
        {
          i++;
        }

    }

  parse_command(buff);

    }while(1);

}
char **parse_command(char *buff)
{
  char *token;
  char *space = " "; // parse the arguments between whitespace
  char **tok_array = malloc(256 * sizeof(char *)); // dynamically allocate a size of 256 for token array
  int i=0;

  token = strtok(buff,space); // grab the first token and separate by whitespace

  while(token != NULL)
    {
      tok_array[i] = token; // store our tokens into the array
      token = strtok(NULL,space); // reset each token
      i++;
    }

  tok_array[i] = NULL; // make sure to end the array with NULL

  store_list(tok_array,head); // store our input commands

  fork_func(tok_array);
}

void fork_func(char **tok_array)
{
  pid_t catch;
  int *child_status;

  catch = fork(); // create the child process

  if(catch == -1) // check for bad fork
    {
       printf("An error has occurred...\n");
       exit(EXIT_FAILURE);
    }

  else if(catch == 0) // child process
     {
       if(execvp(tok_array[0],tok_array) < 0) // run the system call to run our command and argument. If return value is < 0, print error.
         {
           printf("%s\n",strerror(2)); // print the error message for missing directory/file
         }

       exit(0);
     }

  else // parent code
    {
      catch = wait(child_status); // wait so we dont create zombies
    }
}

char *read_file()
{
  FILE *fp ;
  char *buff = malloc(sizeof(char) * 256) ,read_char;

  fp = fopen("mshrc","r"); // allow us to read our mshrc file

  if(fp==NULL)
    {
      printf("Error, could not open the file...\n");
      exit(0) ;
    }

  while(1)
    {

      buff[0] = '\0';
      int i=0;

      while((read_char=fgetc(fp)) != 0xa)
        {
          if(read_char == EOF) // exit with ctrl-D
            return 0;

          buff[i]= read_char;
          //printf("Whats in buff: %s and %c\n",buff, buff[i]);
          i++;
        }

      read_char = '\0';
      buff[i] = '\0';

      parse_command(buff);
      sleep(1);


    }

  fclose(fp);
}

void sub_func()
{
  printf("hiya\n");
  return;
}

void history()
{
  int i=1;

  while(curr != NULL)
    {
      printf("%d.%s \n",i,curr->entry);
      curr = curr->next;
      i++;
    }
}

void store_list(char **buff, his_list *head)
{
  his_list * newnode = malloc(sizeof(his_list)); 

  if(newnode == NULL) // check for errors
    {
      printf("unable to allocate memory for new node\n");
      exit(-1);
    }

  newnode->entry = *buff;
  newnode->next = NULL;

  if(head->next == NULL) // check for first insertion
    {
      head->next = newnode;
      printf("added at the beginning\n");
    }

  else
    {
      curr = head;

      while(curr->next != NULL)
        {
          curr = curr->next;
        }
      curr->next = newnode;
      printf("added new node\n");
      curr->next = NULL;
    }

  return;
}
