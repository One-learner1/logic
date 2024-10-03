all: syntax tree truth truth_table truth_tables

syntax: syntax.cpp
	g++ syntax.cpp -o syntax

tree: tree.cpp syntax.h
	g++ tree.cpp -o tree

truth: truth.cpp tree.h
	g++ truth.cpp -o  truth

truth_table: truth_table.cpp truth.h
	g++ truth_table.cpp -o truth_table

truth_tables: truth_tables.cpp truth_table.h
	g++ truth_tables.cpp -o truth_tables

clean:
	rm -fr `ls -l|awk '/^-..x/{print $$NF}'` *.o *.dSYM
	rm -fr *.pdf *.out *.log *.aux .*.swp
