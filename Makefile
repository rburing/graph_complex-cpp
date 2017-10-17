CC=g++
CFLAGS=-std=c++11 -O3 -pedantic -Wall -Wextra -I${HOME}/include # -Werror
LDFLAGS=

.PHONY: all

all: obj\
	bin\
	bin/test

bin:
	mkdir bin

obj:
	mkdir obj

bin/%: obj/%.o obj/graph.o
	$(CC) -o $@ $< obj/graph.o $(LDFLAGS)
	
obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf obj
	rm -rf bin
