CC=gcc

all: pseudo-shell

pseudo-shell: command.o main.o string_parser.o
	$(CC) -o pseudo-shell command.o main.o string_parser.o 

command.o: command.c command.h 
	$(CC) -c command.c 

main.o: main.c string_parser.h
	$(CC) -c main.c

string_parser.o: string_parser.c string_parser.h
	$(CC) -c string_parser.c 

clean:
	rm -f pseudo-shell *.o
