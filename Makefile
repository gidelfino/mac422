CC = gcc
CFLAGS = -g

all: ep1sh ep1

ep1sh: ep1sh.c
	$(CC) ep1sh.c -o ep1sh $(CFLAGS) -lreadline  
ep1: ep1.c
	$(CC) ep1.c -o ep1 $(CFLAGS) -pthread