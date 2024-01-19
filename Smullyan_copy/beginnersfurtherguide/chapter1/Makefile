PDF=
PDF+=answers.pdf

all: answers.pdf

%.pdf:  %.tex
	pdflatex -file-line-error -interaction=nonstopmode $< | grep -v /usr/local/texlive/
	@rm -fr *.out *.log *.aux

clean:
	@rm -fr `ls -l|awk '/^-..x/{print $$NF}'` *.o *.dSYM
	@rm -fr *.pdf *.out *.log *.aux .*.swp
