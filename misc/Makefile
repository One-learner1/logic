all: test

test: test.cpp replace.h
	g++ test.cpp -o test


clean:
	rm -fr `ls -l|awk '/^-..x/{print $$NF}'` *.o *.dSYM
	rm -fr *.pdf *.out *.log *.aux .*.swp
