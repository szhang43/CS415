CC=gcc

all: lab2 

lab2: command.o main.o
	$(CC) -o lab2 command.o main.o 

command.o: command.c command.h 
	$(CC) -c command.c 

main.o: main.c
	$(CC) -c main.c

clean:
	rm -f lab2 *.o
