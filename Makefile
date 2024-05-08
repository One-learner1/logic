%.pdf:
	pdflatex -interaction=nonstopmode %.tex > latex.txt || ~/Github/repositories/data_structs/error latex.txt stdout ~/Github/repositories/data_structs/flags.txt
