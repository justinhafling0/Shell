#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>


/*

References:
- (General)       :    http://www.rozmichelle.com/pipes-forks-dups/
- (Main Piping)   :    https://stackoverflow.com/questions/13000917/c-programming-fork-multiple-pipe
- (Redirection)   :    http://www.sarathlakshman.com/2012/09/24/implementation-overview-of-redirection-and-pipe-operators-in-shell



- http://web.eecs.utk.edu/~jplank/plank/classes/cs360/360/notes/Dup/lecture.html
- https://stackoverflow.com/questions/13801175/classic-c-using-pipes-in-execvp-function-stdin-and-stdout-redirection
- https://www.geeksforgeeks.org/implementing-lswc-command/
- https://stackoverflow.com/questions/916900/having-trouble-with-fork-pipe-dup2-and-exec-in-c/
- https://stackoverflow.com/questions/8389033/implementation-of-multiple-pipes-in-c

*/

#define TRUE 1
#define FALSE 0

#define DEBUG 0
#define MAX_SIZE 512

char prefix[] = "myshell$ ";
char breakCharacters[] = {' ', '\n', '\0', '|', '<', '>', '&', ' '};
int breakCharactersLength = 8;

_Bool inputReceiving = TRUE;
_Bool printShellPrompt = TRUE;

/*

TO-DO List

- Trailing Ampersand/Meta characters checking may screw things up.
- I don't know how to implement & / Duplicate the entire shell process.


CheckList
[X] Error Checking for Commands / General Error Checking

[X] Check for Random Debugging Messages.
[X] Free Memory
  - All memory up ubntil this point is being freed.

[X] Implement & (Ampersand);

BUGS
[✓] grep "c" doesn't work.

*/

//Function Declarations


void shellPrint();
int shellProcess(char *args[]);

char* getCommand(char inputLine[], char* command);
char** getArgs(char* trimmedInput, char* args[]);

char* trimWhiteSpace(char inputLine[]);
_Bool strCompare(char* command, char compare[]);

int getArgCount(char* args[]);
int getLength(char* input);
int* numCleaner(char** args, int* numberHelper);

/*

Main Class
- Gets Input from Command Line
- Sends Input to shellProcess Function

*/

int main(int argc, char** argv){
  if(argc > 1){
    if(strCompare(argv[1], "-n")){
      printShellPrompt = FALSE;
    }
  }
  if(printShellPrompt){
    shellPrint();
  }
  char inputLine[MAX_SIZE];

  while(inputReceiving){
    inputLine[0] = '\0';
    if(fgets(inputLine, MAX_SIZE, stdin) != NULL){
      char *trimmedInput = malloc(MAX_SIZE);
      trimmedInput = trimWhiteSpace(inputLine);
      if(trimmedInput[0] != '\n'){
        char **args = malloc(MAX_SIZE);
        if(DEBUG){
          printf("Original Line: {%s}\n", inputLine);
          printf("Trimmed Line:{%s}\n", trimmedInput);
        }
        args = getArgs(trimmedInput, args);
        int i = 0;

        int argAmount = getArgCount(args);
        /*
        if(*args[0] < 'A' || *args[0] > 'z'){
          //printf("-bash: syntax error near unexpected token \'%c\'\n", *args[0]);
          if(printShellPrompt){
            shellPrint();
          }
        }else{
          shellProcess(args);
          if(printShellPrompt){
            shellPrint();
          }
        }
        */

        shellProcess(args);
        if(printShellPrompt){
          shellPrint();
        }
        
        i = getArgCount(args);
        for(i; i >= 0; i--){
          if(DEBUG){
            printf("Freeing: %s\n", (args)[i]);
          }
          free(args[i]);
        }
        free(trimmedInput);
      }else{
        if(printShellPrompt){
          shellPrint();
        }
      }
    }else{
      printf("\n");
      exit(EXIT_FAILURE);
    }
  }
}

char** getInfo(){
  if(printShellPrompt){
    shellPrint();
  }
  char inputLine[MAX_SIZE];
  inputLine[0] = '\0';
  if(fgets(inputLine, MAX_SIZE, stdin) != NULL){
    char *trimmedInput = malloc(MAX_SIZE);
    //memset(trimmedInput, 0, MAX_SIZE);
//    printf("Here\n");
    trimmedInput = trimWhiteSpace(inputLine);
//    printf("String: %s\n", trimmedInput);
    if(trimmedInput[0] != '\n'){
//      printf("Here\n");
      char **args =  malloc(MAX_SIZE);
      args = getArgs(trimmedInput, args);
      //free(args);
      free(trimmedInput);
      return args;
      //shellProcess(args);
    }
  }else{
    exit(EXIT_FAILURE);
  }
}

int getArgCount(char* args[]){
  int returnN = 0;
  //printf("Arg: %s\n", args[0]);
  while(args[returnN] != NULL){
    //printf("Arg: %s\n", args[returnN]);
    returnN++;
  }
  return returnN;
}

int getLength(char* input){
  int j = 0;
  while(*input != '\0'){
    input++;
    j++;
  }
  return j;
}

_Bool strCompare(char* command, char compareString[]){
  int index = 0;
  while(*command != '\0' && compareString[index] != '\0'){
    if(*command != compareString[index]){
      return FALSE;
    }
    command++; index++;
  }
  return TRUE;

}

int* numCleaner(char** args, int* numberHelper){
  if(DEBUG){
    printf("Cleaning Number\n");
  }
  int i = 0;
  int argCount = getArgCount(args);

  int j = 0;
  int commandCounter = 1;
  char c;
  numberHelper[0] = 1;
  while(args[i] != NULL && i < argCount){
    numberHelper[commandCounter] +=1;
    j = 0;
    c = args[i][j];
    if(DEBUG){
      printf("C: %c\n", c);
    }
    while(c != '\0'){
        j++;
        numberHelper[commandCounter] +=1;
        if(c == '|' || c == '<' || c == '>' || c == '&' || c == '\n' || c == '\0'){
          commandCounter++;
          numberHelper[0] +=1;
        }
        c = args[i][j];
    }
    i++;
  }
  return numberHelper;

}

int shellProcess(char*args[]){

  /*

  numberHelper:
    Index 0: Number of commands to execute.
    Index X: Number of arguments in that command number.

  */

  _Bool processingArgs = TRUE;
  _Bool foundMeta = FALSE;

  int argTracker = 0;
  int argCounter = getArgCount(args);

  int* numberHelper = calloc(sizeof(int), argCounter);
  numberHelper = numCleaner(args, numberHelper);

  int t;

  numberHelper[0]+=1;
  for(t = 0; t < numberHelper[0] + 1; t++){
    numberHelper[t] -=1;
  }

  char*** cleanArgs = calloc(numberHelper[0] * sizeof(char*), sizeof(char*));
  int j;
  int z;
  int cleanIndex = 0;
  char* metaCharacters = malloc(sizeof(char) * (numberHelper[0] + 1));

  /*

  This code block is just cleaning up the args one more time. It is formatting them perfectly to be used
  in the execvp commands. We also are creating an array of meta characters so we can reference them

  These are both very important steps because it allows us to ensure that our inputs are being piped
  properly.

  */

  for(j = 0; j < numberHelper[0]; j++){
    foundMeta = FALSE;
    cleanArgs[j] = calloc(numberHelper[j+1] + 1, sizeof(char));
  }

  int cleanX, cleanY;
  cleanX = 0;
  cleanY = 0;
  for(z = 0; z < argCounter; z++){
    if(*args[z] == '|' || *args[z] == '>' || *args[z] == '<' || *args[z] == '&'){
      if(DEBUG){
        printf("Found Meta\n");
      }
      metaCharacters[cleanX] = *args[z];
      cleanY++;
      cleanArgs[cleanX][cleanY] = NULL;
      cleanX++; cleanY = 0;
    }else{
      cleanArgs[cleanX][cleanY] = calloc(getLength(args[z]) * sizeof(char), sizeof(char));
      cleanArgs[cleanX][cleanY] = args[z];
      cleanY++;
    }
  }

  /*

  This is where we will handle the forking/piping/child handling processes.
  Here is the main logic of the shell that is required for this assignment.

  */
  int numPipes = numberHelper[0] - 1;
  int pipefds[2*numPipes];
  metaCharacters[numPipes +1] = '\0';


  int r;
  for( r = 0; r < numPipes; r++ ){
      if( pipe(pipefds + r*2) < 0 ){
          //perror and exit
      }
  }

  int status;

  int commandCounter = 0;
  pid_t pid, wpid;

  int new_pipe[2];
  int old_pipe[2];
  int tempPipe;

  _Bool foundAmpersand = FALSE;


  while(commandCounter <= numPipes){
    if(commandCounter < numberHelper[0]){
      pipe(new_pipe);
    }

    pid = fork();
    if(pid < 0){
      printf("ERROR: Could Not Fork\n");
    }else{
      //Child Stage
      if(pid == 0){
        if(DEBUG){
          printf("\n\n");
          printf("-------------------------\n");
          printf("Command Counter: %d\n", commandCounter);
          printf("Number of Pipes: %d\n", numPipes);
          printf("-------------------------\n");
          printf("\n\n");
        }

        //If Previous Commands Exist
        if(commandCounter > 0){
          tempPipe = (commandCounter-1) * 2;
          //printf("Pipe (Previous) [Read]: %d\n", tempPipe);
          if(dup2(pipefds[(commandCounter-1) * 2], 0) < 0){
             perror(" dup2");///j-2 0 j+1 1
             exit(EXIT_FAILURE);
          }
          close(pipefds[tempPipe]);
          close(pipefds[tempPipe+1]);
        }

        //If there is a next command, we need to do those pipes.
        if(commandCounter < numPipes){
          tempPipe = commandCounter * 2 + 1;
          if(metaCharacters[commandCounter] != '>' && metaCharacters[commandCounter] != '<'){
            if(dup2(pipefds[tempPipe], 1) < 0){
              perror("dup2");
              exit(EXIT_FAILURE);
            }
          }else if(metaCharacters[commandCounter] == '>'){
            int out = open(*cleanArgs[commandCounter+1], O_RDWR|O_CREAT, 0666); // Should also be symbolic values for access rights
            dup2(out,STDOUT_FILENO);
            close(out);
          }else if(metaCharacters[commandCounter] == '<'){
            int fd0 = open(*cleanArgs[commandCounter+1], O_RDONLY);
            dup2(fd0, STDIN_FILENO);
            close(fd0);
            if(commandCounter + 1 < numPipes){
              if(metaCharacters[commandCounter + 1] == '>'){
                int out = open(*cleanArgs[commandCounter+2], O_RDWR|O_CREAT, 0666); // Should also be symbolic values for access rights
                dup2(out,STDOUT_FILENO);
                close(out);
              }else{
                if(dup2(pipefds[tempPipe+2], 1) < 0){
                  perror("dup2");
                  exit(EXIT_FAILURE);
                }
              }
            }
          }
          close(pipefds[tempPipe]);
          close(pipefds[tempPipe-1]);
        }
        if(commandCounter > 0){
          if(metaCharacters[commandCounter-1] != '>' && metaCharacters[commandCounter-1] != '<'){
            if(execvp(cleanArgs[commandCounter][0], &cleanArgs[commandCounter][0]) < 0){
              printf("ERROR: Could Not execute command\n");
              exit(0);
            }
          }else{
            close(pipefds[tempPipe]);
            close(pipefds[tempPipe-1]);
            exit(0);
          }
        }else{
          if(execvp(cleanArgs[commandCounter][0], &cleanArgs[commandCounter][0]) < 0){
            printf("ERROR: Could Not execute command\n");
            exit(0);
          }
        }
      }else{
          // In parent Call
          if(DEBUG){
            printf("In parent\n");
          }

          if(numPipes >= 1){
            if(metaCharacters[numPipes-1] == '&' && !foundAmpersand){
              char** tArgs = malloc(MAX_SIZE);
              tArgs = getInfo();
              //printf("Command: %d\n", commandCounter);
              foundAmpersand = TRUE;
              shellProcess(tArgs);
              free(tArgs);
              //printf("Here\n");
            }
          }

          int e;
          e = commandCounter*2 + 1;
          wpid = waitpid(pid, &status, WUNTRACED);        //Waited for child to finish
          if(e < 2 * numPipes){
            close(pipefds[e]);
          }
          if(commandCounter > 1){
            close(pipefds[e-3]);
          }

        commandCounter++;                      // Increasing counter to know how many commands we execute.
      }
    }
  }

  free(numberHelper);
  free(metaCharacters);
  // FIX THIS SHITTY LINE OF CODE!!!!!
  //free(cleanArgs);
  return 0;
}

char* trimWhiteSpace(char inputLine[]){
  char* trimmedInput = calloc(MAX_SIZE, sizeof(char));
//  memset(trimmedInput, 0, getLength(trimmedInput));
  int trimmedCounter = 0;
  int inputLineCounter = 0;
  if(inputLine[0] == ' '){
    inputLineCounter++;
  }

  for(inputLineCounter; inputLineCounter < MAX_SIZE; inputLineCounter++){ // inputLine[inputLineCounter] != '\n'

     if(inputLineCounter >= 1){
      // Checking to see if we either find a double sspace, or a space before the newline character. This makes sure
      // That there are almost no errors later on in the program
      if(inputLine[inputLineCounter] == '\n' && inputLine[inputLineCounter-1] == ' '){
        trimmedInput--; *(trimmedInput) = '\0';
        trimmedInput-=trimmedCounter;
        trimmedInput++;

        return trimmedInput;
      }
      //Checking if we find 2 spaces in a row.
      if(inputLine[inputLineCounter] == ' ' && inputLine[inputLineCounter-1] == ' '){

      }else{
        //Checking if we find newline
        if(inputLine[inputLineCounter] == '\n'){
          break;
        }else{

          //If we didn't find any violations, we can just keep adding the old characters to the newly trimmed output.
          *trimmedInput = inputLine[inputLineCounter];
          trimmedInput++;
          trimmedCounter++;
        }
      }
    }else{
      *trimmedInput = inputLine[inputLineCounter]; trimmedInput++;
      trimmedCounter++;
    }
  }
//  printf("Trimmed\n");
  *trimmedInput = '\0';
  int x =0;

  for(x; x < trimmedCounter; x++){
    trimmedInput--;
  }

  return trimmedInput;
}


// I think getArgs is now bugfree and doing exactly what we want.
char** getArgs(char* trimmedInput, char* args[]){
  _Bool scanning = TRUE;
  int argCounter = 0;
  int argLength = 0;
  char *tempString = malloc(MAX_SIZE);;
  int tempStringLength = 0;
  int count = 0;
  int stop = getLength(trimmedInput);

  if(trimmedInput != NULL){
    while(scanning && count < stop+2){
      count++;
      int j = 0;
      for(j; j < breakCharactersLength; j++){
        if(*trimmedInput == breakCharacters[j] && (argLength > 0 || *trimmedInput != ' ')){
          if(argLength > 0){
            args[argCounter] = malloc(sizeof(char) * argLength + 1);
            *tempString = '\0';
            tempString -= argLength;
            int i = 0;
            for(i; i < argLength; i++){
              args[argCounter][i] = tempString[i];
            }
            args[argCounter][argLength] = '\0';
            if(*trimmedInput == '\n' || *trimmedInput == '\0'){
              scanning = FALSE;
            }
            argCounter++;
            argLength = 0;
          }
          if(*trimmedInput == '|' || *trimmedInput == '&' || *trimmedInput == '<' || *trimmedInput == '>'){
              args[argCounter] = malloc(sizeof(char) + 1);
              args[argCounter][0]= *trimmedInput;
              argCounter++;
              args[argCounter-1][1] = '\0';
              argLength = 0;
          }
          trimmedInput++;

        }else{
          if(j == (breakCharactersLength - 1)){
            if(*trimmedInput != ' ' && *trimmedInput != '\0'){
              *tempString = *trimmedInput;
              tempString++; argLength++;
            }
          }
        }
      }
      trimmedInput++;
    }
  }
  return args;
}

void shellPrint(){
  printf("%s", prefix);
}
