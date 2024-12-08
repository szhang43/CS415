CC = gcc

# All targets to build
all : DuckBank

DuckBank : bank 

bank : bank.c 
	$(CC) -pthread -lpthread -o bank bank.c 

clean: 
	rm -f bank 