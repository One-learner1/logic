/* Program to, given a list of variables x1,...,x_n and an expression W made only out of those variables (and constants), find the 
x1,...,xn eliminate of W */ 
#include "algorithm.h"
int variable_count; // global variable to store the number of variables

/* Function to read in the list of variables */
void read_variables(char* variables, char variable_array[], int found_variables[]);

/* Function to ensure that the given expression only consists of the given variables */
void variable_check(char* expression, int found_variables[]);

int main(int argc, char* argv[]) {
	char* expression = (char*)malloc(MAX_LENGTH);
	char variables[MAX_LENGTH];
	int found_variables[26]; // array to track what variables have been entered
	if (argc != 3) {
		printf("A list of variables and an expression must be supplied.\n");
		exit(1);
	}
	if (argv[1] == NULL || argv[2] == NULL) {
		printf("A list of variables and an expression must be supplied.\n");
		exit(1);
	}
	if (strlen(argv[1]) == 0 || strlen(argv[2]) == 0) {
		printf("A list of variables and an expression must be supplied.\n");
		exit(1);
	}
	printf("CCC\n");
	printf("argv[2]: %s\n", argv[2]);
	printf("termified form: %s\n", termify(argv[2], 0));
	expression = termify(argv[2], 0);
	printf("DDD\n");
	// we termify the expression, and make sure it is syntactically correct
	if (!correct_syntax(expression)) {
		printf("Incorrect syntax.\n");
		exit(1);
	}
	// we initialize the found_variables array
	for (int i = 0; i < 26; i++) {
		found_variables[i] = 0;
	}
	printf("BBB\n");
	// function to read the variables out of argv[1]
	read_variables(argv[1], variables, found_variables);
	// we know check that all of the variables in the expression are found in the list of variables
	variable_check(argv[2], found_variables);
	// now that we have ensured correct input, we successively eliminate the variables
	printf("AAA\n");
	for (int i = variable_count-1; i >= 0; i--) {
		printf("i: %d expression: %s variable: %c\n", i, expression, variables[i]);
		expression = x_eliminate(expression, variables[i]);
	}
	printf("The eliminate is %s\n", expression);
}

void read_variables(char* variables, char variable_array[], int found_variables[]) {
	char character;
	if (variables == NULL) {
		printf("A string of variables must be supplied.\n");
		exit(1);
	}
	for (int i = 0; i < (int)strlen(variables); i++) {
		character = variables[i];
		if (!(character >= 'a' && character <= 'z')) {
			printf("%c is not a variable.\n", character);
			exit(1);
		}
		// otherwise we update the variable array
		variable_count++;
		variable_array[i] = character;
		// we do this as atoi('a')-97=0
		found_variables[character-97] = 1; 
	}
}

void variable_check(char* expression, int found_variables[]) {
	if (expression == NULL) {
		printf("The expression must not be empty.\n");
		exit(1);
	}
	//printf("AAA\n");
	for (int i = 0; i < (int)strlen(expression); i++) {
		// if the character is a variable, we need to be sure that it was listed
		if (expression[i] >= 'a' && expression[i] <= 'z') {
			//printf("character: %c\n", expression[i]);
			//printf("index: %d\n", expression[i]-97);
			if (found_variables[expression[i]-97] != 1) {
				printf("The expression must only contain variables in the list.\n");
				exit(1);
			}
		}
	}
}
