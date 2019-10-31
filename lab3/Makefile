TARGET	 = multithreadsort
CC		 = gcc
CCFLAGS  = -std=c99 -pedantic -Wall
LDFLAGS  = -lm -pthread
SOURCES  = $(wildcard *.c)
INCLUDES = $(wildcard *.h)
OBJECTS  = $(SOURCES:.c=.o)

all:$(TARGET)

$(TARGET):$(OBJECTS)
	$(CC) -o $(TARGET) $(LDFLAGS) $(OBJECTS)

$(OBJECTS):$(SOURCES) $(INCLUDES)
	$(CC) -c $(CCFLAGS) $(SOURCES)

clean:
	rm -f $(TARGET) $(OBJECTS)