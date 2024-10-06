/* This program inputs a file containing triples of an expression, a remove pattern (the thing to be removed), and a replace pattern (the 
thing to replace the remove pattern with). For each triple, 
the expression should come first, then the remove pattern, then the replace pattern. */

#include "replace.h"
#define MAX_REPLACEMENT 100 // the maximum number of formulas that we allow to be replaced

typedef struct replace_pattern {
	char expression[MAX_LENGTH];
	char remove[MAX_LENGTH];
	char replace[MAX_LENGTH];
} replace_pattern;

/* function to input the input file containing the triples (an expression, a remove pattern, and a replace pattern). This
function does not check for syntax (the actual single_replace and full_replace functions will). The function will return
the number of triples found if the file was input properly, and -1 if the file was not input properly. */
int replace_input(replace_pattern replace_patterns[], FILE* f);

/* Given an array of replace_pattern's (and the number of patterns), this function creates an output file consisting of all
the expressions in the array (so this will be used to output the updated expressions with replacement in this program) */
void replace_output(replace_pattern replace_patterns[], int num_replacement);

int main(int argc, char* argv[]) {
	FILE *f;
	int num_replacement = 0; // to store the number of expressions we will be replacing
	if (argc != 2) {
		puts("Exactly one input file must be supplied as an argument.");
		exit(1);
	}
	f = fopen(argv[1], "r");
	if (f == nullptr) {
		printf("Error opening input file: \"%s\"\n", argv[1]);
		exit(1);
	}
	replace_pattern replace_patterns[MAX_REPLACEMENT];	
	num_replacement = replace_input(replace_patterns, f);
	if (num_replacement == -1) {
		printf("Input error.\n");
		exit(1);
	}
	fclose(f);
	// loop to replace all of the inputted expressions
	for (int i = 0; i < num_replacement; i++)
		if (replace(replace_patterns[i].expression, replace_patterns[i].remove, replace_patterns[i].replace) == 0) {
			printf("Input error.\n");
			exit(1);
		} 
	// now we make the output file
	replace_output(replace_patterns, num_replacement);
}

int replace_input(replace_pattern replace_patterns[], FILE* f) {
	int ret_val = -1; // to store the return value of fscanf
	int num_replacement = 0;
	for (int i = 0; i < MAX_REPLACEMENT; i++) {
		// if fscanf neither reaches a triple nor EOF, then we have an input error
		ret_val = fscanf(f, "%s%s%s", replace_patterns[i].expression, replace_patterns[i].remove, replace_patterns[i].replace);
		if (ret_val == EOF)
			break;
		if (ret_val != 3)
			return -1;
		num_replacement++;
	}
	// if we have not yet reached the end of the file, there is an input error
	if (fgetc(f) != EOF)
		return -1;
	return num_replacement;
}

void replace_output(replace_pattern replace_patterns[], int num_replacement) {
	FILE* f = fopen("replace_output.txt", "r"); // we see if there is already a file called "replace_output.txt"
	if (f != nullptr) {
		printf("Error: \"replace_output.txt\" already exists.\n");
		exit(1);
	}
	fclose(f);
	f = fopen("replace_output.txt", "w");
	if (f == nullptr) {
		printf("Error opening output file: \"replace_output.txt\"\n");
		exit(1);
	}
	for (int i = 0; i < num_replacement; i++) {
		fprintf(f, "%s\n", replace_patterns[i].expression);
	}
	fclose(f);
}
