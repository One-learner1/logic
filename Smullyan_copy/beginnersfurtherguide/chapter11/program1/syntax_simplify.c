/* Program to check that a given expression really is a term with the only combinators as $S$ and $K$. For simplicity, all lower case letters are variables, and all upper case letters other 
than $S$ or $K$ are also variables. No other characters stand for variables, and there are no other constants.
For readability, it is useful to use the abbreviated notation which was used in the book. It can be defined as follows:
For all constants and variables, the simplified form of x is x. For all terms t1, the simplified form of (t1x) is tx, where t is the simplified form of t1, and for all
terms t1 and expressions x1, the simplified form of (t1(x1)) (if it is a term) is t(x) where t and x are the simplified forms of t1 and (x1), respectively. This program
simplifies any given term according to these rules. It will be useful to note for another program that, for any variables or constants x, the simplified form of (x) is just x. */
/* QUESTION: If I replace 
char* second_simplified = (char*)malloc(MAX_LENGTH); by 
char* second_simplified = NULL; or
char* simplified = (char*)malloc(MAX_LENGTH); by
char* simplified = NULL;
a segmentation fault occurs. second_simplified is the destination of a strcpy, and simplified is the destination of a sprintf. Why does this happen? */
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

/* Function to simplify an expression (assumed syntactically correct) according to the above rules */
char* simplify(char* expression);

int main(int argc, char* argv[]) {
	char* expression = (char*)malloc(MAX_LENGTH);
	if (argc != 2 || strlen(argv[1]) == 0) {
		printf("Exactly one expression must be supplied.\n");
		exit(1);
	}
	int ret_val;
	ret_val = correct_syntax(argv[1]);
	printf("The expression is syntactically %s.\n", ret_val?"correct":"incorrect");
	// if the formula is syntactically correct, we simplify it
	if (ret_val == 1) {
		expression = simplify(argv[1]);
		printf("The simplified expression is: %s\n", expression);
	}
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

char* simplify(char* expression) {
        char* t1 = NULL;
        char* t2 = NULL;
        char* simplified = (char*)malloc(MAX_LENGTH);
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
        else if (expression[0] == '(') {
                char* first_simplified = NULL; // to store the simplified form of t1
                char* second_simplified = (char*)malloc(MAX_LENGTH); // to store the simplified form of t2
                // we first find t1 and t2
                t1 = term(expression, 1, &first_index);
                t2 = term(expression, first_index+1, &second_index);
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
                printf("Syntactically incorrect expression.\n");
                exit(1);
        }
}
