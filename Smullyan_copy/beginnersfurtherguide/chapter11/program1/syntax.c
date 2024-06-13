/* Program to check that a given expression really is a term with the only combinators as $S$ and $K$. For simplicity, all lower case letters are variables, and all upper case letters other 
than $S$ or $K$ are also variables. No other characters stand for variables, and there are no other constants. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LENGTH 100

/* Function to determine the syntactical correctness of a given expression */
int correct_syntax(char* expression);

/* Function to find the first term in the given expression */
char* term(char* expression, int index, int* end_index);

/* Function to find the right paren corresponding to a given left paren in an expression */
int right_paren(char* expression, int index);

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Exactly one expression must be supplied.\n");
		exit(1);
	}
	int ret_val;
	ret_val = correct_syntax(argv[1]);
	printf("The expression is syntactically %s.\n", ret_val?"correct":"incorrect");
}

int correct_syntax(char* expression) {
	char* t1 = NULL;
	char* t2 = NULL;
	int first_index, second_index; // these track where the first and second terms end
	// if it is a single character that is either an uppercase or lowercase letter, it has correct syntax
	if (strlen(expression) == 1 && ((expression[0] >= 65 && expression[0] <= 90) || (expression[0] >= 97 && expression[0] <= 122))) {
		return 1;
	}
	// the next case is when the first character is a left paren and the last is a right paren
	else if (expression[0] == '(' && expression[strlen(expression)-1] == ')') {
		// calls function to find the terms t_1 and t_2
		t1 = term(expression, 1, &first_index);
		t2 = term(expression, first_index+1, &second_index);
		// we check that both the terms t1 and t2 were found
		if (t1 == NULL || t2 == NULL) {
			return 0;
		}
		// we also check that the end of the second term is the second to last character of the expression
		if (second_index != (int)strlen(expression)-2) {
			return 0;
		} 
		// now we ensure that both t1 and t2 are terms
		if (correct_syntax(t1) && correct_syntax(t2)) {
			return 1;
		}
		else {
			return 0;
		}
	}
	// otherwise the expression does not start with a valid character
	else {
		return 0;
	}
}

char* term(char* expression, int index, int* end_index) {
	char* term = (char*)malloc(MAX_LENGTH);
	int right;
	if (expression[index] != '(') {
		term[0] = expression[index];
		*end_index = index; 
		return term;
	}
	// otherwise we know it is a left paren, and we need to find a right paren 
	right = right_paren(expression, index);
	if (right == -1) {
		return NULL;
	}
	else {
		strncpy(term, &expression[index], (right+1)-index);
		*end_index = right;
		return term;
	}
}

int right_paren(char* expression, int index) {
	int paren_count = 0;
	for (int i = index; i < (int)strlen(expression); i++) {
		if (expression[i] == '(') {
			paren_count++;
		}
		else if (expression[i] == ')') {
			paren_count--;
			if (paren_count == 0) {
				return i;
			}
		}
	}
	return -1;
}
