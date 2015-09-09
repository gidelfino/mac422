CC = gcc
CFLAGS = -g

all: ep1sh ep1

ep1sh: ep1sh.c
	$(CC) ep1sh.c -o ep1sh $(CFLAGS) -lreadline  
ep11: ep1.c
	$(CC) ep1.c -o ep11 $(CFLAGS) -pthread
ep12: ep1_cond.c
	$(CC) ep1_cond.c -o ep12 $(CFLAGS) -pthread
