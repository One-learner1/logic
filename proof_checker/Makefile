all: propositional

propositional: propositional.o ../truth_tables/truth_table.h
	g++ -Wc++11-extensions -Wall -Werror -std=c++0x propositional.cpp -o propositional

clean:
	rm -fr `ls -l|awk '/^-..x/{print $$NF}'` *.o *.dSYM
	rm -fr *.pdf *.out *.log *.aux .*.swp
