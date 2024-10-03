all: replace

replace: replace.cpp ../truth_tables/truth_table.h
	g++ replace.cpp -o replace

clean:
	rm -fr `ls -l|awk '/^-..x/{print $$NF}'` *.o *.dSYM
	rm -fr *.pdf *.out *.log *.aux .*.swp
