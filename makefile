CFLAGS=-Wall -std=c89 -pedantic 
OPTIONS=


all: 	master nave porto

master: master.o 
	    gcc master.o -o master 
nave:   nave.o
		gcc nave.o -o nave -lm
porto:  porto.o
		gcc porto.o -o porto

master.o:
		gcc -c $(CFLAGS) $(OPTIONS) master.c
nave.o:
		gcc -c $(CFLAGS) $(OPTIONS) nave.c
porto.o:
		gcc -c $(CFLAGS) $(OPTIONS) porto.c
clean:
		rm -f *.o master nave porto 



