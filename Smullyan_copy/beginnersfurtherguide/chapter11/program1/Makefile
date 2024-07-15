CC = gcc
CFLAGS += -O3 -g3 -W -Wall -D __DEBUG__
#CFLAGS += -O3 -g3 -W -Wall
LIBS += -lm

clean:
	rm -fr `ls -l|awk '/^-..x/{print $$NF}'` *.o *.dSYM
	rm -fr *.pdf *.out *.log *.aux .*.swp

syntax_simplify: syntax_simplify.o
	$(CC) $(CFLAGS) syntax_simplify.c -o syntax_simplify

x_eliminate: syntax_simplify.o x_eliminate.o syntax_simplify.h
	$(CC) $(CFLAGS) x_eliminate.c syntax_simplify.o -o x_eliminate

eliminate: syntax_simplify.o eliminate.o syntax_simplify.h eliminate.h
	$(CC) $(CFLAGS) eliminate.c syntax_simplify.o -o eliminate

simplify: simplify.o algorithm.o algorithm.h
	$(CC) $(CFLAGS) simplify.c algorithm.o -o simplify

algorithm: algorithm.o algorithm.h
	$(CC) $(CFLAGS) algorithm.c -o algorithm

convert: algorithm.o convert.o algorithm.h
	$(CC) $(CFLAGS) convert.c algorithm.o -o convert
