CC=gcc
CFLAGS=-Wall -Wextra -Werror -g

SOURCES=$(wildcard src/*.c src/middlewares/*.c)
OBJECTS=$(SOURCES:.c=.o)
DEPS=$(SOURCES:.c=.d) $(wildcard test/*.d)
EXECUTABLE=server

all: $(EXECUTABLE)

# Include automatically generated dependency files
-include $(DEPS)

# Generate object files and dependency files
src/%.o: src/%.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(DEPS) test/*.test

test: $(patsubst test/%.test.c,test/%.test,$(wildcard test/*.test.c))

test/%.test: test/%.test.c $(OBJECTS)
	$(CC) $(CFLAGS) -MMD $< $(filter-out src/main.o,$(OBJECTS)) -o $@

.PHONY: all clean test
