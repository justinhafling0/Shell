###############################################################################
# CS410 Fall 2020 - Lab 1
#
# This is how you can leave comments in your Makefile
# Some special-purpose variables:
#
# $@ : name of the target (lhs of the first line of each rule)
# $^ : dependecies of the target (rhs of the first line of each rule)
#
# - You can use any shell command available such as gcc, ar, cp, rm, echo, etc.
# - If you don't want make to show the command itself add a @ before the command name
#   e.g., echo "Hello" ---output--> echo "Hello"\n Hello
#         However, @echo "Hello" ---output---> Hello
#
###############################################################################

myshell: 
	gcc -o myshell myshell.c


clean:
	rm -f *.o *.so *.a myshell
