Discussing the implementation of my shell.

  Read:
    Read line from terminal and store in character array. This is the most basic element of the shell.

  Parse:
    For this exercise, parsing input was arguably one of the most important elements of the shell implementation. My program had 2 basic
    levels of parsing. The initial level of parsing is to call trimWhiteSpace(), then getArgs().

      trimWhiteSpace():
      - I do this step first to prevent any issues with reading inputs that have random spacing between arguments.
      - I used this step because at first I didn't think we were allowed to use <string.h>.
      - Removes double spacing, trailing spacing, and spacing before the first argument/command.

      getArgs();
      - This function is basically implementing strtok(). Again, I didn't understand that we were allowed to use the <string.h>, so This
      part is relatively unnecessary if I were to reimplement the shell.

    My program actually contains a second layer of parsing that I realized I needed later on into the assignment. It is an extra layer
    that is actually within my shellProcess() function that occurs before any forking of children.

      cleanArgs[][][]:
      - This structure essentially stores all the args needed to call the functions very easily. It starts storing based on metaCharacters
      to ensure that each command is on a line.

      metaCharacters[]:
      - List of metaCharacters in order which was critical for our implementation of piping and other input/output redirection.

    Execute:
      This stage of the shell is responsible for everything that actually happens in a shell that you see. This is where we implement the ability
      to execute the commands as well as interpret the various metaCharacters.

      Adjustments are made based on the number of commands executed, the number of commands that need to be executed, and where the metaCharacters
      occur. We need to be able to look ahead of the metaCharacters list to check for scenarios like cat < x > y. We need to know early
      that we need to also redirect output essentially 2 stages in advanced.

      '&'
        In order to implement the ampersand, we check before any processing/forking if the final metaCharacter is an ampersand. If it is, we call
        getInfo() and then send the information back to the shellProcess() function so that it can execute commands like:
            sleep 10 &
            ls
        without experiencing any issues.

    Loop:
      Keeps looping until Ctrl+D, Ctrl+C, or Ctrl+Z or other various ways of terminating the program.
