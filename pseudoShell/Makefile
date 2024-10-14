CC=gcc

all: pseudoShell

pseudoShell: command.o main.o
	$(CC) -o pseudoShell command.o main.o 

command.o: command.c command.h 
	$(CC) -c command.c 

main.o: main.c
	$(CC) -c main.c

clean:
	rm -f pseudoShell *.o