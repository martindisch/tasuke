CFLAGS = -Wall -std=c11 -Wpedantic
DEBUG = -g -O0

.PHONY: all clean debug

all: tasuke

debug: CFLAGS += $(DEBUG)
debug: tasuke

tasuke: tasuke.o tasklib.o tasklist.o
	gcc $(CFLAGS) tasuke.o tasklib.o tasklist.o -o tasuke

tasuke.o: tasuke.c
	gcc -c $(CFLAGS) tasuke.c -o tasuke.o

tasklib.o: tasklib.c tasklib.h
	gcc -c $(CFLAGS) tasklib.c -o tasklib.o

tasklist.o: tasklist.c tasklist.h
	gcc -c $(CFLAGS) tasklist.c -o tasklist.o

clean:
	rm -f tasuke *.o
