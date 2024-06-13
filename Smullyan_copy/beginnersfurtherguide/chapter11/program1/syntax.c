/* Program to check that a given expression really is a term with the only combinators as $S$ and $K$. For simplicity, all lower case letters are variables, and all upper case letters other 
than $S$ or $K$ are also variables. No other characters stand for variables, and there are no other constants. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LENGTH 100

int main(int argc, char* argv[]) {
	char* expression = (char*)malloc(MAX_LENGTH);
	if (argc != 2) {
		printf("An expression (maximum length of %d) must be supplied as an argument.\n", MAX_LENGTH);
		exit(1);
	}
	int paren_count = 0;
	int right_paren_encounter = 0; // to track whether the first right paren in a string of right paren's has been encountered
	int last_char = 0; // to track whether the preceeding character in the expression is not a parentheses
	expression = argv[1];
	if (strlen(expression) == 0) {
		printf("An expression must be entered.\n");
		exit(1);
	}
	if (strlen(argv[1]) > MAX_LENGTH) {
		printf("The expression must be at most %d characters.\n", MAX_LENGTH);
		exit(1);
	}
	for (int i = 0; i<(int)strlen(expression); i++) {
		// if the character is a left parenthesis, we need to ensure that it matches up with a right parenthesis
		if (expression[i] == '(') {
			int nested_paren_count = 1;
			paren_count++;
			for (int j = i; j<(int)strlen(expression); j++) {
				if (expression[j] == '(') {
					nested_paren_count++;
				}
				else if (expression[j] == ')') {
					nested_paren_count--;
					if (nested_paren_count == 0) {
						break;
					}
				}
			}
			if (nested_paren_count != 0) {
				printf("Incorrect syntax.\n");
				exit(1);
			}
		}
		else if (expression[i] == ')') {
			paren_count--;
			if (right_paren_encounter == 0) {
				right_paren_encounter = 1;
				if (last_char == 0) {
					printf("Incorrect syntax.\n");
					exit(1);
				}
			}
		}
		else {
			last_char = 1;
			right_paren_encounter = 0;
			// this ensures that the character is either a lower case or upper case letter
			if (!(((expression[i] >= 65 && expression[i] <= 90)) || (expression[i] >= 97 && expression[i] <= 122))) {
				printf("expression[i]: %d\n", expression[i]);
				printf("Character '%c' is not allowed.\n", expression[i]);
				exit(1);
			}
		}
	}
	if (paren_count != 0) {
		printf("Incorrect syntax.\n");
		exit(1);
	}
	printf("Correct syntax.\n");
}
		
