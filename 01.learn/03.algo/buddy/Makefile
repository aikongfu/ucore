OBJS=buddy2_test.o buddy2.o
CC=gcc
CFLAGS=-c -Wall -g

test:$(OBJS)
	$(CC) $(OBJS) -o test
main.o : buddy2.c
	$(CC) buddy2.c $(CFLAGS)  -o buddy2.o
tool1.o: buddy2_test.c
	$(CC) buddy2_test.c $(CFLAGS) -o buddy2_test.o

clean: 
	rm *.o test -rf
