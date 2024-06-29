/* This is a program to impliment the algorithm to produce an x-eliminate of a simple term, as described on page 219 of the book */
#include "syntax_simplify.h"

/* The function to actually do the elimination */
char* eliminate(char* expression, char variable);

/* Function to determine if the variable occurs in the given expression */
int var_find(char* expression, char variable); 

int main(int argc, char* argv[]) {
	char* expression = (char*)malloc(MAX_LENGTH);
	char* test_expression = (char*)malloc(MAX_LENGTH); // expression to test if the eliminate is correct
	if (argc != 3) {
		printf("An expression and a variable must be simplified.\n");
		exit(1);
	}
	// we make sure they are not empty
	if ((int)strlen(argv[1]) == 0 || (int)strlen(argv[2]) == 0) {
		printf("The expressions must not be empty.\n");
		exit(1);
	}
	if ((int)strlen(argv[2]) != 1 || !(argv[2][0] >= 'a' && argv[2][0] <= 'z')) {
		printf("The second argument must be a variable.\n");
		exit(1);
	}
	// we first termify the given expression, and then ensure that it is syntactically correct
	expression = termify(argv[1], 0);
	if (!correct_syntax(expression)) {
		printf("Expression is syntactically incorrect.\n");
		exit(1);
	}
	expression = eliminate(expression, argv[2][0]);
	printf("The expression with %s elimination is %s\n", argv[2], expression);
	sprintf(test_expression, "(%s%c)", expression, argv[2][0]);
	printf("Check on elimination: %s=%s\n", test_expression, simplify_sk(test_expression));
	int ret_val = 0;
	ret_val = strcmp(argv[1], simplify_sk(test_expression));
	// if the initial strcmp did not come out correct, it could be that the resulting expression was simplified too far, and we need to check to see if the simplified form of the given expression
	// equals the simplified form of the test_expression. For example, suppose we had the x-eliminate E of Kxy. Then Ex=Kxy=x
	if (ret_val == 0) {
		ret_val = strcmp(simplify_sk(argv[1]), simplify_sk(test_expression))?0:1;
	}
	printf("The %s eliminate was obtained.\n", ret_val?"correct":"incorrect");
}

char* eliminate(char* expression, char variable) {
	char* eliminated_expression = (char*)malloc(MAX_LENGTH);
	if ((int)strlen(expression) == 1 && expression[0] == variable) {
		// we return the expression for I, but written in terms of S and K
		return "((SK)K)";
	}
	else if (!var_find(expression, variable)) {
		sprintf(eliminated_expression, "(K%s)", expression);
		return eliminated_expression;
	}
	// otherwise we know that the expression is enclosed by parentheses
	else {
		// we find the two sub-terms of the epxression
		char* term1 = (char*)malloc(MAX_LENGTH);
		char* term2 = (char*)malloc(MAX_LENGTH);
		int end_index1;
		int end_index2;
		term1 = eliminate(term(expression, 1, &end_index1), variable);
		term2 = eliminate(term(expression, end_index1+1, &end_index2), variable);
 		sprintf(eliminated_expression, "((S%s)%s)", term1, term2);
		return eliminated_expression;
	} 
}

int var_find(char* expression, char variable) {
	for (int i = 0; i < (int)strlen(expression); i++) {
		if (expression[i] == variable) {
			return 1;
		}
	}
	return 0;
}
