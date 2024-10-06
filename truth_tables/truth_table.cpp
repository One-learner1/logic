/* Program to print out a truth table for a given formula. It will print out a version of the truth table in stdout, as well 
as well as writing a latex file (titled "truth_table.tex") containing a latex version of the truth table. */ 
#include "truth.h"

// function to print the truth table on stdout
void out(formula* f, variable variables[]);

// function to ensure that the input to the out function is correct
int ready(formula* f, variable variables[]);

// function to produce the latex file containing the truth table
void latex(formula* f, variable variables[]);

// function to print out a formula in latex (this just changes the operators to control sequences)
void print_latex(formula* f, FILE *g);

// function to print out an operator in its latex control sequence form
void print_op(char op, FILE *g);

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Exactly one formula must be provided.\n");
		exit(1);
	}
	if (!syntax(argv[1])) {
                printf("Invalid syntax.\n");
                exit(1);
        }
        formula* f = new formula(argv[1]);
        int i;
        list* components = nullptr;
        tree(argv[1], f, components);
        variable variables[MAX_VARIABLES];
        variables->init_vars(variables);
        variable_find(argv[1], variables);
        f->interpretations = (int)pow(2, var_count);
        variable_values(variables, f->interpretations, var_count);
	for (int j = 0; j < f->interpretations; j++) {
                formula_values(f, variables, j);
        }
	out(f, variables);
	latex(f, variables);
}

int ready(formula* f, variable variables[]) {
	if (f == NULL)
		return 0;
	if (!syntax(f->expression))
		return 0;
	//if (sizeof(variables) / sizeof(variable) != MAX_VARIABLES)
	//	return 0;
	return 1;
}

void out(formula* f, variable variables[]) {
	int end = var_count+1; // will be set to var_count+1 if the formula is not atomic, and var_count if it is atomic
	if (f->op == '\0')
                end = var_count;
	if (!ready(f, variables))
		return;
	bool value; // when checking if an index matches any operators, if it does, this will be set to the value the operator 
		   // has under the given interpretation
	// we first print out the variables and the formula
	for (int j = 0; j < end; j++) {
		if (j < var_count)
			printf("%c ", variables[j].var);
		else {
			printf("%s", f->expression);
		}
	}
	printf("\n");
	// Otherwise we print out the truth values
	for (int i = 0; i < f->interpretations; i++) {
		for (int j = 0; j < end; j++) {
			if (j < var_count)
				printf("%d ", variables[j].values[i]);
			else {
				for (int k = 0; k < (int)strlen(f->expression); k++) {
					// we check if the current index matches any operators
					if (f->op_find(k, value, i)) {
						printf("%d", value);
					}
					else {
						printf(" ");
					}
				}
			}
		}
		printf("\n");
	}
}

/* I was ultimately not able to find a great way of printing this out in latex. The main problem was alignment (as is 
apparent from testing the program. Specifcally, the problem can be highlighted with the following example. Suppose we are 
printing out the truth table for "(X>Y)". Then we would have a column with "(X>Y)" in the first row, and in the 
next rows, we would need to print out the value of the ">" so that it aligns with the ">". For instance, the first row 
should print a "1" right beneath the ">". I tried to accomplish this by basically setting a bunch of macros to the lengths of all 
the control sequences for the connectives, the parentheses, and the upper case letters, and then, as we go along printing out 
the formula, using \hspace to print the desired space. The reason why this doesn't work is because the control sequences 
for the connectives will themselves print out extra space. For instance, "(X\supset Y)" will print out a bit of space 
between X and \supset, and \supset and Y. However, this extra space is not printed out for the negation, so I did not try to add any extra 
space for that case. 
I was not able to exactly how much extra space this is for the control sequences of each 
connective. However, the latex seems to be a rough approximation for at least relatively small formulas. */ 
void latex(formula* f, variable variables[]) {
	FILE *g = fopen("truth_table.tex", "w");
	if (g == nullptr) {
		printf("Error creating latex file.\n");
		exit(1);
	}
	if (!ready(f, variables))
		return;
	int end = var_count+1; // will be set to var_count+1 if the formula is not atomic, and var_count if it is atomic
	char subtraction; // will be used for alignment when printing the truth table
        if (f->op == '\0')
                end = var_count;
	bool value; // when checking if an index matches any operators, if it does, this will be set to the value the operator
                   // has under the given interpretation
	int columns = var_count+1; // for the number of columns in the table
	fprintf(g, "\\documentclass{article}\n\\usepackage{amsmath}\n\\usepackage{amssymb}\n\\usepackage{calc}\n\\usepackage{amsfonts}\n");
	fprintf(g, "\\newcommand*{\\oldneg}{\\mathord{\\sim}}\n");
	fprintf(g, "\\title{Truth Table}\n\\begin{document}\n\\maketitle\n\n");
	// these next lines of code define lengths to hold the widths of the operators, 1, and 0, which will help with alignment
	for (int i = 0; i < 7; i++) {
		fprintf(g, "\\newlength{\\len%c}\n", i+97);
	}
	fprintf(g, "\n");
	fprintf(g, "\\settowidth{\\lena}{$0$}\n");
	fprintf(g, "\\settowidth{\\lenb}{$1$}\n");
	fprintf(g, "\\settowidth{\\lenc}{$\\supset$}\n");
	fprintf(g, "\\settowidth{\\lend}{$\\equiv$}\n");
	fprintf(g, "\\settowidth{\\lene}{$\\vee$}\n");
	fprintf(g, "\\settowidth{\\lenf}{$\\wedge$}\n");
	fprintf(g, "\\settowidth{\\leng}{$\\oldneg$}\n");
	for (int i = 0; i < 26; i++) {
		fprintf(g, "\\newlength{\\len%c}\n", i+65);
		fprintf(g, "\\settowidth{\\len%c}{$%c$}\n", i+65, i+65);
	}
	fprintf(g, "\\newlength{\\leftlen}\n\\newlength{\\rightlen}\n");
	fprintf(g, "\\settowidth{\\leftlen}{$($}\n\\settowidth{\\rightlen}{$)$}\n");
	// no we can start the truth table
	fprintf(g, "Truth table for:\n\\[");
	print_latex(f, g);
	fprintf(g, "\\]\n");
	fprintf(g, "\\begin{center}\n\\begin{tabular}{");
	for (int i = 0; i < columns; i++) { fprintf(g, "l"); }
	fprintf(g, "}\n");
	for (int j = 0; j < end; j++) {
                if (j < var_count) {
                        fprintf(g, "$%c$ ", variables[j].var);
			if (j != end-1)
				fprintf(g, "& ");
		}
                else {
			fprintf(g, "$");
			print_latex(f, g);
			fprintf(g, "$ ");
                }
        }
        fprintf(g, "\\\\\n");
        // Otherwise we print out the truth values
        for (int i = 0; i < f->interpretations; i++) {
                for (int j = 0; j < end; j++) {
                        if (j < var_count) {
                                fprintf(g, "$%d$ ", variables[j].values[i]);
				if (j != end-1)
					fprintf(g, "& ");
			}
                        else {
				fprintf(g, "$");
                                for (int k = 0; k < (int)strlen(f->expression); k++) {
                                        // we check if the current index matches any operators
                                        if (f->op_find(k, value, i)) {
                                                fprintf(g, "%d", value);
						if (value == 0) subtraction = 'a';
						else subtraction = 'b';
						switch (f->expression[k]) {
						case '=':
							fprintf(g, "\\hspace{\\lend}");
							break;
						case '>':
							fprintf(g, "\\hspace{\\lenc}");
							break;
						case '~':
							fprintf(g, "\\hspace{\\leng-\\len%c}", subtraction);
							break;
						case '^':
							fprintf(g, "\\hspace{\\lenf}");
							break;
						case 'v':
							fprintf(g, "\\hspace{\\lene}");
							break;
						default:
							break;
						}	
						
                                        }
                                        else {
						if (f->expression[k] != '(' && f->expression[k] != ')')
							fprintf(g, "\\hspace{\\len%c}", f->expression[k]);
						else if (f->expression[k] == '(') {
							fprintf(g, "\\hspace{\\leftlen}");
						}
						else {
							fprintf(g, "\\hspace{\\rightlen}");
						}
                                        }
                                }
				fprintf(g, "$");
                        }
                }
                fprintf(g, "\\\\\n");
        }
	fprintf(g, "\\end{tabular}\n\\end{center}\n\n\\end{document}");
}

void print_latex(formula* f, FILE *g) {
	for (int i = 0; i < strlen(f->expression); i++) {
		if (correct_connective(f->expression[i]) || f->expression[i] == '~')
			print_op(f->expression[i], g);
		else {
			fprintf(g, "%c", f->expression[i]);
		}
	}
}

void print_op(char op, FILE *g) {
	switch (op) {
	case '~':
		fprintf(g, "\\oldneg ");
		break;
	case 'v':
		fprintf(g, "\\vee ");
		break;
	case '^':
		fprintf(g, "\\wedge ");
		break;
	case '>':
		fprintf(g, "\\supset ");
		break;
	case '=':
		fprintf(g, "\\equiv ");
		break;
	default:
		return;
	}
}	
