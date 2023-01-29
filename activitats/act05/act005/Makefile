CC=gcc
CFLAGS=-c -Wall -O3 -Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wconversion
SOURCES=act05.c aux.c

OBJECTS=$(SOURCES:.c=.o)
EXECS=act05
RM=/bin/rm

.PHONY: all
all: $(SOURCES) $(EXECS)
	
$(EXECS): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@
	$(CC) $(CFLAGS) player.c -o player.o
	$(CC)  aux.o player.o -o player
	$(CC)  temp.c -o temp
	$(RM) *.o
	
.c.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	/bin/rm -rf *.o
	/bin/rm -rf $(EXECS) 
	$(RM) player
	$(RM) temp

.PHONY: run
run: $(EXECS)
	 ./$(EXECS)