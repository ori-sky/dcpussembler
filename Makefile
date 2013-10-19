CC=gcc
CFLAGS=-g -c -Wall --std=c99
LDFLAGS=-g
SOURCES=main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=dcpussembler

all: $(SOURCES) $(EXECUTABLE)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@
