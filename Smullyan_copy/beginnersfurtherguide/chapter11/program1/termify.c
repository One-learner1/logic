/* This is a program to "termify" a given simplified expression (according to the rules in the syntax and simplify program). Formally, we can define the process of termification as follows: 
For any expressions x and y (assumed to be syntactically correct or simplified from syntactically correct terms), the termified form of xy is (xy). Also, the termified form of x is x. 
Then, for any expressions 
x_1,...,x_n, if x is the termified form of x_1,...,x_n-1, then (xx_n) is the termified form of x_1,...,x_n. */ 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LENGTH 100

// this stores the sub-expressions of expression
typedef struct sub_expression {
	char* expression;
	int end_index;
} sub_expression;

/* Function to termify a given simplified expression */
char* termify(char* expression);

int main(int argc, char* argv[]) {
	char* expression = (char*)malloc(MAX_LENGTH);
	if (argc != 2 || strlen(argv[1] == 0)) {
		printf("An expression must be supplied.\n");
		exit(1);
	}
	// first we make sure that it is not already termified! That is, if it is syntactically correct, it is already a termified term
	if (correct_syntax(argv[1])) {
		expression = argv[1];
	}
	else {
		expression = termify(argv[1]);
	}
	printf("The termified expression is %s.\n", expression);
}

char* termify(char* expression) {
	sub_expression sub_expressions[MAX_LENGTH];
	int term_count = 0;
	int current_index = 0; // to track what the index of the expression is
	char* termified = (char*)malloc(MAX_LENGTH);
	if (expression == NULL) {
		return NULL;
	}
	// in this case, expression is just a single character, meaning its termified form is the same
	else if (strlen(expression) == 1) {
		return expression;
	}
	// in this case expression consists of sub-expressions
	else {
		for (int i = 0; i < MAX_LENGTH; i++) {
			// we find the next sub-term
			sub_expressions[i].expression = term(expression, current_index, &sub_expressions[i].end_index);
			// if no term was found, an error occured
			if (sub_expressions[i].expression == NULL) {
				printf("Invalid expression.\n");
				exit(1);
			}
			// otherwise, if a term was found, and it is at the end of the expression, we have found all of the sub expressions
			else if (sub_expressions[i].end_index == strlen(expression)) {
				term_count++:
				break;
			}
			else {
				// we update current_index
				current_index = sub_expressions[i].end_index+1;
				term_count++;
			}
		}
	}
	// if there were only two terms x and y, the termified expression is (xy)
	if (term_count == 2) {
		sprintf(termified, "(%s%s)", sub_expressions[0].expression, sub_expressions[1].epxression);
	}
	// otherwise we need to make a recursive call
	else {
		char* first_termified = (char*)malloc(MAX_LENGTH); // this is the termified expression of the first n-1 sub expressions
		char* sub = (char*)malloc(MAX_LENGTH); // to store the string of the first n-1 sub expressions
		memmove(sub, expression, sub_expressions[term_count-2].end_index+1);
		first_termified = termify(sub);
		sprintf(termified, "(%s%s)", first_termified, sub_expressions[term_count-1].expression);
	}
	return termified;
}
