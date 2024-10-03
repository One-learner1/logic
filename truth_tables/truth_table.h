/* Header file to be used for the "truth_tables.cpp" program */
#include "truth.h"

// function to ensure that the input to the out function is correct. size should be sizeof(variables) for the variables that was passed in as an argument.
int ready(formula* f, variable variables[], int size);

// function to produce the latex file containing the truth table.
void latex(formula* f, variable variables[], FILE* g, int size);

// Improved version of the above latex function
void improved_latex(formula* f, variable variables[], FILE* g, int size);

// function to print out a formula in latex (this just changes the operators to control sequences)
void print_latex(formula* f, FILE *g);

// function to print out an operator in its latex control sequence form
void print_op(char op, FILE *g);

int ready(formula* f, variable variables[], int size) {
        if (f == NULL)
                return 0;
        if (!syntax(f->expression))
                return 0;
        if (size / sizeof(variable) != MAX_VARIABLES) {
              return 0;
	}
        return 1;
}

void improved_latex(formula* f, variable variables[], FILE* g, int size) {
	if (g == nullptr) {
		printf("Error creating latex file.\n");
		exit(1);
	}
	if (!ready(f, variables, size))
		return;
	var_count = f->var_count;
	// for this altered latex function, the end will not ve f->var_count+1, but 
	// f->var_count+strlen(f->expression), as now each character in the formula will be treated as its own column. 
	// If the formula is atomic, it will still be set to f->var_count (1)
	int end = f->op == '\0'?f->var_count:f->var_count+strlen(f->expression);
	bool value;
	int columns = fprintf(g, "Truth table for:\n\\[");
	int count = 0; // this will be used to track the index of the character in f->expression we are adding to the table
	int precedence = -1; // will be used to test if a given index matches an operator with precedence
	print_latex(f, g);
	fprintf(g, "\\]\n");
	fprintf(g, "\\setlength\\tabcolsep{0pt}\n");
	fprintf(g, "\\begin{center}\n\\begin{tabular}{");
	// after each variable, we want a hspace of 10 pt. We don't want any horizontal space between 
	// the characters in the formula
	for (int i = 0; i < end; i++) {
		if (i < var_count) {
			fprintf(g, "c@{\\hspace{10pt}}");
		}
		else {
			fprintf(g, "c");
		}
	}
	fprintf(g, "}\n");
	for (int j = 0; j < end; j++) {
		if (j < f->var_count) {
			fprintf(g, "$%c$ ", variables[j].var);
		}
		else {
			fprintf(g, "$");
			if (correct_connective(f->expression[count]) || f->expression[count] == '~')
				print_op(f->expression[count], g);
			else
				fprintf(g, "%c", f->expression[count]);
                        fprintf(g, "$ ");
			count++;
                }
		 if (j != end-1)
                                fprintf(g, "& ");
	}
	fprintf(g, "\\\\\n");
	for (int i = 0; i < f->interpretations; i++) {
		count = 0;
                for (int j = 0; j < end; j++) {
                        if (j < f->var_count) {
                                fprintf(g, "$%d$ ", variables[j].values[i]);
                                if (j != end-1)
                                        fprintf(g, "& ");
                        }
			else {
				if (f->op_find(count, value, i)) {
					fprintf(g, "$%d$", value);
				}
				if (j != end-1)
					fprintf(g, " & "); 
				count++;
			}
		}
		fprintf(g, "\\\\\n");
	}
	// lastly, we print out a line for operator precedence
	count = 0;
	for (int j = 0; j < end; j++) {
		if (j < f->var_count && j != end-1)
			fprintf(g, "& ");
		else {
			precedence = f->precedence_find(count);
			if (precedence != -1) {
				fprintf(g, "$%d$", precedence);
			}
			if (j != end-1)
				fprintf(g, " & ");
			count++;
		}
	}
	fprintf(g, "\n\\end{tabular}\n\\end{center}\n\n");
}
				
	

void latex(formula* f, variable variables[], FILE* g, int size) {
	if (g == nullptr) {
		printf("Error creating latex file.\n");
		exit(1);
	}
	if (!ready(f, variables, size))
		return;
	// while we are working with the given formula, it will be necessary to set the global var_count to f->var_count, 
	// so that i_check works correclty
	var_count = f->var_count;
	int end = f->var_count+1; // will be set to var_count+1 if the formula is not atomic, and var_count if it is atomic
	char subtraction; // will be used for alignment when printing the truth table
        if (f->op == '\0')
                end = f->var_count;
	bool value; // when checking if an index matches any operators, if it does, this will be set to the value the operator
                   // has under the given interpretation
	int columns = f->var_count+1; // for the number of columns in the table
	// now we can start the truth table
	fprintf(g, "Truth table for:\n\\[");
	print_latex(f, g);
	fprintf(g, "\\]\n");
	fprintf(g, "\\begin{center}\n\\begin{tabular}{");
	for (int i = 0; i < columns; i++) { fprintf(g, "l"); }
	fprintf(g, "}\n");
	for (int j = 0; j < end; j++) {
                if (j < f->var_count) {
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
        for (int i = 0; i < f->interpretations; i++) {
                for (int j = 0; j < end; j++) {
                        if (j < f->var_count) {
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
						if (f->expression[k] != '(' && f->expression[k] != ')') {
							fprintf(g, "\\hspace{\\len%c}", f->expression[k]);
						}
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
	fprintf(g, "\\end{tabular}\n\\end{center}\n\n");
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
