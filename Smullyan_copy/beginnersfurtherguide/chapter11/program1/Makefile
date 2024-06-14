CC = gcc
CFLAGS += -O3 -g3 -W -Wall -D __DEBUG__
#CFLAGS += -O3 -g3 -W -Wall
LIBS += -lm

clean:
	rm -fr `ls -l|awk '/^-..x/{print $$NF}'` *.o *.dSYM
	rm -fr *.pdf *.out *.log *.aux .*.swp

syntax_simplify: syntax_simplify.o
	$(CC) $(CFLAGS) syntax_simplify.c -o syntax_simplify

simplify: simplify.o
	$(CC) $(CFLAGS) simplify.c -o simplify
