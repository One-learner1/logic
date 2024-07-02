/* Program to check that an expression is syntactically correct, and then simplify it */
#include "algorithm.h"

int main(int argc, char* argv[]) {
	char* expression = (char*)malloc(MAX_LENGTH);
	if (argc != 2) {
		printf("An expression must be supplied.\n");
		exit(1);
	}
	if (strlen(argv[1]) == 0) {
		printf("The expresison must not be empty.\n");
		exit(1);
	}
	expression = termify(argv[1], 0);
	printf("termified form: %s\n", expression);
	if (!correct_syntax(expression)) {
		printf("The expression is syntactically incorrect.\n");
		exit(1);
	}
	printf("The termified form of the expression is: %s\n", expression);
	printf("It is syntactically correct.\n");
	expression = simplify_sk(expression);
	printf("The expression with S and K simplification is: %s\n", expression);
}
	
