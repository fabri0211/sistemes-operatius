ossh: main.o parse.o log.o
		gcc -o ossh main.o parse.o log.o 

main.o: main.c parse.h
	gcc -c main.c

parse.o: parse.c parse.h log.h
	gcc -c parse.c

log.o: log.c
	gcc -c log.c

clean:
	rm -f *.o ossh