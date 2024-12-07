CC = gcc

# All targets to build
all : DuckBank

DuckBank : bank 

bank: bank.c string_parser.c
	$(CC) -o bank bank.c string_parser.c

clean: 
	rm -f bank 