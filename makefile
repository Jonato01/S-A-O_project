CFLAGS=-Wall -std=c89 -Wpedantic 
OPTIONS=


all: 	clear clean master nave porto meteo
clear:
	clear
run: master nave porto
	./master
master: master.o 
	    gcc master.o -o master 
nave:   nave.o
		gcc nave.o -o nave -lm
porto:  porto.o
		gcc porto.o -o porto
meteo:  meteo.o
		gcc meteo.o -o meteo

master.o:
		gcc -c $(CFLAGS) $(OPTIONS) master.c
nave.o:
		gcc -c $(CFLAGS) $(OPTIONS) nave.c
porto.o:
		gcc -c $(CFLAGS) $(OPTIONS) porto.c
meteo.o:
		gcc -c $(CFLAGS) $(OPTIONS) meteo.c
clean:
		rm -f *.o master nave porto meteo



