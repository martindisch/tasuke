.PHONY: all clean debug

all: tasuke

tasuke: tasuke.o tasklib.o
	gcc -Wall tasuke.o tasklib.o -o tasuke

debug: tasuke.o tasklib.o
	gcc -Wall -g -O0 tasuke.o tasklib.o -o tasuke

tasuke.o: tasuke.c
	gcc -Wall -c tasuke.c -o tasuke.o

tasklib.o: tasklib.c tasklib.h
	gcc -Wall -c tasklib.c -o tasklib.o

clean:
	rm -f tasuke *.o
