
CC:=gcc

all:cmd

cmd: cmd.o
	$(CC) cmd.o -o cmd

cmd.o: cmd.c
	$(CC) -c cmd.c

clear: 
	rm -rf *.o cmd
