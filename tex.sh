program='/Users/kat/Github/repositories/data_structs/error'
flags='/Users/kat/Github/repositories/data_structs/flags.txt'
output='latex.txt'
command="program output stdout flags"
pdflatex -interaction=nonstopmode $1 > output || command
