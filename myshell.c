#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 100
#define DELIM " \t\r\n\a"

char **getArgs(char *command,int *waitFlag){
  int argsNum = 0;
  char *arg;
  char **args = malloc(BUFFER_SIZE * sizeof(char*));
  if (!args) {exit(EXIT_FAILURE);}

  arg = strtok(command, DELIM);
  while (arg != NULL) {
    if(strncmp(arg, "&", 1) == 0){
      *waitFlag=1;
    }else{
      args[argsNum] = arg;
      argsNum++;
    }
    arg = strtok(NULL, DELIM);
  }
  args[argsNum] = NULL;
  return args;
}

int execute(char **args,int waitFlag){
  pid_t pid;
  pid = fork();

  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    printf("Forking have failed (error number %d)\n",pid);
  } else if(waitFlag==0) {
    // Parent process
     while(wait(NULL)!=pid);
  }
  return 1;
}

int main(void)
{
    close(2);
    dup(1);
    char command[BUFFER_SIZE];
    char **args;
    int waitFlag=0;
    
    while (1)
    {
        fprintf(stdout, "my-shell> ");
        memset(command, 0, BUFFER_SIZE);
        fgets(command, BUFFER_SIZE, stdin);
        if(strncmp(command, "exit", 4) == 0)
        {
            break;
        }
        args = getArgs(command,&waitFlag);
        execute(args,waitFlag);
        waitFlag=0;
        free(args); 
    }
    return 0;
}
