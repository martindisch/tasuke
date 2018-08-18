.PHONY: all clean debug

all: tasuke

tasuke: tasuke.o
	gcc tasuke.o -o tasuke

debug: tasuke.o
	gcc -g -O0 tasuke.o -o tasuke

tasuke.o: tasuke.c
	gcc -c tasuke.c -o tasuke.o

clean:
	rm -f tasuke *.o
