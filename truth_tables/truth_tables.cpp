/* A program to input a file of propositional formulas, and produce a latex file containing truth tables for all of them. The 
input file should consists of formulas (separated by new lines). No output in stdout will be produced.
The output file that this program will produce will be titled "truth_tables.tex". */
#include "truth_table.h"
#define MAX_FORMULAS 100
int formula_count = 0;

// Function to input the formulas from the input file, and ensure they are all syntactically correct
void input(formula* formulas[], FILE* f, variable variables[MAX_FORMULAS][MAX_VARIABLES]);

int main() {
	FILE *f, *g;
	char input_file[100];
	printf("What is the name of the input file?\n");
	scanf("%s", input_file);
	f = fopen(input_file, "r");
	if (f == nullptr) {
		printf("Error opening input file: \"%s\"\n", input_file);
		exit(1);
	}
	formula* formulas[MAX_FORMULAS];
	for (int i = 0; i < MAX_FORMULAS; i++) {
		formulas[i] = new formula;
	}
	variable variables[MAX_FORMULAS][MAX_VARIABLES];
	input(formulas, f, variables);
	g = fopen("truth_tables.tex", "w");
	if (g == nullptr) {
		printf("Error opening output file: \"%s\"\n", "truth_tables.tex");
		exit(1);
	}
	// now we begin the latex file 
	fprintf(g, "\\documentclass{article}\n\\usepackage{amsmath}\n\\usepackage{amssymb}\n\\usepackage{calc}\n\\usepackage{amsfonts}\n");
        fprintf(g, "\\newcommand*{\\oldneg}{\\mathord{\\sim}}\n");
        fprintf(g, "\\title{Truth Tables}\n\\begin{document}\n\\maketitle\n\n");
        // these next lines of code define lengths to hold the widths of the operators, 1, and 0, which will help with alignment
	/*
        for (int i = 0; i < 7; i++) {
                fprintf(g, "\\newlength{\\len%c}\n", i+97);
        } */
	/*
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
	*/
	// now we run a loop to successively print out the truth tables for each formula
	for (int i = 0; i < formula_count; i++) {
		fprintf(g, "\n\\section*{Truth Table %d}\n", i+1);	
		improved_latex(formulas[i], variables[i], g, sizeof(variables[i]));
	}
	fprintf(g, "\n\\end{document}");
}

void input(formula* formulas[], FILE* f, variable variables[MAX_FORMULAS][MAX_VARIABLES]) {
	char expression[MAX_LENGTH];
	int count = 0;
	while (fgets(expression, MAX_LENGTH, f) != nullptr && formula_count < MAX_FORMULAS) {
		var_count = 0;
		// the next line removes the new line
		expression[strcspn(expression, "\n")] = 0;
		if (!syntax(expression)) {
			printf("\"%s\" does not have correct syntax.\n", expression);
			exit(1);
		}
		list* components = nullptr;
		strcpy(formulas[formula_count]->expression, expression);
		formulas[formula_count] = alt_tree(expression, 0);
		formulas[formula_count]->prec_init(1);
		variables[formula_count]->init_vars(variables[formula_count]);
		variable_find(expression, variables[formula_count]);
		formulas[formula_count]->interpretations = (int)pow(2, var_count);
		formulas[formula_count]->var_count = var_count;
		variable_values(variables[formula_count], formulas[formula_count]->interpretations); 
		for (int j = 0; j < formulas[formula_count]->interpretations; j++) {
			formula_values(formulas[formula_count], variables[formula_count], j);
		}
		delete components;
		formula_count++;
	}
	// if we haven't reached the end of the file by now, there was too many formulas
	if (fscanf(f, "%s", expression) != EOF) {
		printf("Error: input file contains too many formulas.\n"); 
		exit(1);
	}
}
