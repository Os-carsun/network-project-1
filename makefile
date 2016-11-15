all: myshell


#declare variables
CC = gcc

myshell: command.o
	$(CC) *.c -o myshell
command.o: comp.sh
	bash comp.sh

