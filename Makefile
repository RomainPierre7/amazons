GSL_PATH ?= /net/ens/renault/save/gsl-2.6/install
CFLAGS = -std=c99 -Wall -Wextra -fPIC -g3 -I$(GSL_PATH)/include 
LDFLAGS = -lm -lgsl -lgslcblas -ldl \
	-L$(GSL_PATH)/lib -L$(GSL_PATH)/lib64 \
	-Wl,--rpath=${GSL_PATH}/lib
SERVEUR = src/server.c src/server_aux.c src/game.c src/graph.c
TESTS = tst/test.c src/graph.c src/game.c src/server_aux.c tst/main_test.c tst/test_David.c tst/test_Georges.c 
LIBS = -lm -lgsl -lgslcblas
OBJECTS =  src/graph.o src/game.o 
CLIENTS = install/David.so install/Georges.so
all: build

build: server client
	rm src/*.o

src/%.o:src/%.c 
	gcc ${CFLAGS} -c -fPIC $^ -o $@

install/%.so:src/%.o ${OBJECTS}
	gcc ${CFLAGS} -shared -o $@ $^

server:
	gcc ${CFLAGS} -o install/server ${SERVEUR} -ldl ${LDFLAGS}

client: ${OBJECTS} ${CLIENTS}

alltests:
	gcc ${CFLAGS} -o install/alltests ${TESTS} -fprofile-arcs -ftest-coverage -lgcov ${LDFLAGS}
	./install/alltests

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all ./install/server install/David.so install/Georges.so

test: alltests

install: server client test
	rm src/*.o

run:
	./install/server install/David.so install/Georges.so

run_print_grid:
	./install/server install/David.so install/Georges.so -p -t g

pdf:
	pdflatex ./tex/rapport.tex
	evince rapport.pdf

doc:
	doxygen configdox

cleanTest:
	rm -f install/*.gcno install/*.gcda install/*.gcov install/alltests

clean: cleanTest
	@rm -rf *~ src/*~ install/* *.gcno *.gcov rapport.[pla]* rapport.toc *.gcda doc/*

.PHONY: client install test clean doc
