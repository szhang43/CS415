CC = gcc

# All targets to build
all: mcp

# Target mcp to build part1, iobound, and cpubound
mcp: part1 part2 iobound cpubound

# Compile and build part1 executable
part1: part1.c
	$(CC) -o part1 part1.c

# Compile and build part2 executable 
part2: part2.c 
	$(CC) -o part2 part2.c

# Compile and build iobound executable
iobound: iobound.c
	$(CC) -o iobound iobound.c

# Compile and build cpubound executable
cpubound: cpubound.c
	$(CC) -o cpubound cpubound.c

# Clean up executables
clean:
	rm -f part1 part2 iobound cpubound
