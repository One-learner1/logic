/* For readability, it is useful to use the abbreviated notation which was used in the book. It can be defined as follows:
For all constants and variables, the simplified form of x is x. For all terms t1, the simplified form of (t1x) is tx, where t is the simplified form of t1, and for all 
terms t1 and expressions x1, the simplified form of (t1(x1)) (if it is a term) is t(x) where t and x are the simplified forms of t1 and (x1), respectively. This program 
simplifies any given term according to these rules. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LENGTH 100

/* Function to simplify an expression (assumed syntactically correct) according to the above rules */
char* simplify(char* expression);

int main(int argc, char* argv[]) {
	char* expression = (char*)malloc(MAX_LENGTH);
	if (arc != 2 || strlen(argv[1]) == 0) {
		printf("An expression must be supplied.\n");
		exit(1);
	}
	printf("The given expression is: %s\n", argv[1]);
	expression = simplify(argv[1]);
	printf("The simplified expression is: %s\n", expression);
}

char* simplify(char* expression) {
	char* t1 = NULL;
	char* t2 = NULL;
	char* simplified = NULL;
	int first_index, second_index;
	// if the expression is null, an error has occured
	if (expression == NULL) {
		printf("The expression cannot be null.\n");
		exit(1);
	}
	// if the expression is one character long, then the simplified form is just the expression itself
	else if (strlen(expression) == 1) {
		return expression;
	}
	// otherwise the expression must start with a left paren
	else if (expression[1] == '(') {
		char* first_simplified = NULL; // to store the simplified form of t1
		char* second_simplified = NULL; // to store the simplified form of t2
		// we first find t1 and t2
		t1 = term(expression, 1, &first_index);
		t2 = term(expression, first_index+1, &second_index)
		// we obtain the simplified version of t1
		first_simplified = simplify(t1);
		// if t2 is just a single character, then we let second_simplified equal it; otherwise we make a recursive call
		if (strlen(t2) == 1) {
			strcpy(second_simplified, t2);
			sprintf(simplified, "%s%s", first_simplified, second_simplified);
		}
		// in this case, note that this means the second term is surrounded by parentheses (it is different from the previous case)
		else {
			second_simplified = simplify(t2);
			sprintf(simplified, "%s(%s)", first_simplified, second_simplified);
		}
		return simplified;
	}
	// otherwise an error has occured
	else {
		pritnf("Syntactically incorrect expression.\n");
		exit(1);
	}
}
