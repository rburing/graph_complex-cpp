CC=g++
CFLAGS=-std=c++11 -O3 -pedantic -Wall -Wextra -I${HOME}/include # -Werror
LDFLAGS=
GINAC_LDFLAGS=-lcln -lginac -L${HOME}/lib 
NAUTY_CFLAGS=-I${HOME}/src/nauty26r10
NAUTY_LDFLAGS=-L${HOME}/src/nauty26r10 -l:nauty.a

.PHONY: all

all: obj\
	bin\
	bin/test\
	bin/generate_graphs\
	bin/contracting_differential\
	bin/expanding_differential\
	bin/reduce_mod_skew\
	bin/rank_nullity\
	bin/kernel\
	bin/substitute_relations\
	bin/orient

bin:
	mkdir bin

obj:
	mkdir obj

bin/generate_graphs:
	cp src/generate_graphs.py bin/generate_graphs
	chmod +x bin/generate_graphs

bin/%: obj/%.o obj/graph.o
	$(CC) -o $@ $< obj/graph.o $(LDFLAGS) $(NAUTY_LDFLAGS) $(GINAC_LDFLAGS)
	
obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) $(NAUTY_CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf obj
	rm -rf bin
