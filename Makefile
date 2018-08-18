.PHONY: all clean

all: tasuke

tasuke: tasuke.o
	gcc tasuke.o -o tasuke

tasuke.o: tasuke.c
	gcc -c tasuke.c -o tasuke.o

clean:
	rm -f tasuke *.o
