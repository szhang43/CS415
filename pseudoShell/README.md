# README - Pseudo-Shell Project

# Pseudo-Shell: A Single-threaded UNIX System Command-line Interface

## Project Overview

This project implements a simplified version of a UNIX shell, called "Pseudo-Shell". It's a single-threaded, synchronous, command-line interface that allows users to perform a limited set of functions centered around file system navigation, manipulation, and I/O.

## Features

- Two operational modes: Interactive and File mode
- Implementation of basic UNIX commands: ls, pwd, mkdir, cd, cp, mv, rm, cat
- Error handling for incorrect syntax and parameters
- Command chaining using ';' separator

## File Structure

- main.c: Contains the main function and core shell logic
- command.c: Implements the UNIX-like commands
- command.h: Header file for command functions
- string_parser.c: Contains functions for parsing input strings
- string_parser.h: Header file for string parsing functions
- Makefile: For compiling the project

## Compilation

To compile the project, run:

```
make
```

## Usage

Run in interactive mode:

```
./pseudo-shell
```

Run in file mode:

```
./pseudo-shell -f &lt;filename&gt;
```

## Supported Commands

- ls: List directory contents
- pwd: Print working directory
- mkdir <name>: Create a new directory
- cd <directory>: Change current directory
- cp <src> <dst>: Copy files
- mv <src> <dst>: Move files
- rm <filename>: Remove files
- cat <filename>: Display file contents
- exit: Exit the shell

## Error Handling

The shell handles errors such as incorrect parameters, missing commands, and syntax errors. Error messages are displayed when these occur.

## Notes

- This project uses system calls for command implementation
- Memory management is handled using malloc() and free()
- The project has been tested with Valgrind to ensure no memory leaks

## Author

Sophia Zhang

## Course Information

CIS 415 - Operating Systems, Fall 2024

Professor Allen Malony
