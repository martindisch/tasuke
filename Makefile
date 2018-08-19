CFLAGS = -Wall
DEBUG = -g -O0

.PHONY: all clean debug

all: tasuke

debug: CFLAGS += $(DEBUG)
debug: tasuke

tasuke: tasuke.o tasklib.o
	gcc $(CFLAGS) tasuke.o tasklib.o -o tasuke

tasuke.o: tasuke.c
	gcc -c $(CFLAGS) tasuke.c -o tasuke.o

tasklib.o: tasklib.c tasklib.h
	gcc -c $(CFLAGS) tasklib.c -o tasklib.o

clean:
	rm -f tasuke *.o
