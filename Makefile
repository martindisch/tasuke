.PHONY: all clean debug

all: tasuke

tasuke: tasuke.o
	gcc -Wall tasuke.o -o tasuke

debug: tasuke.o
	gcc -g -O0 -Wall tasuke.o -o tasuke

tasuke.o: tasuke.c
	gcc -c -Wall tasuke.c -o tasuke.o

clean:
	rm -f tasuke *.o
