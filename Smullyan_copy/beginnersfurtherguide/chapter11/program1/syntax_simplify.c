/* Program to check that a given expression really is a term with the only combinators as $S$ and $K$. For simplicity, all lower case letters are variables, and all upper case letters other 
than $S$ or $K$ are also variables. No other characters stand for variables, and there are no other constants.
For readability, it is useful to use the abbreviated notation which was used in the book. It can be defined as follows:
or all constants and variables, the simplified form of x is x. For all terms t_1, the simplified form of (t_1x) is tx, where t is the simplified form of t_1, and for all
terms t_1 and expressions x1, the simplified form of (t1(x1)) (if it is a term) is t(x) where t and x are the simplified forms of t1 and (x1), respectively. This program
simplifies any given term according to these rules. It will be useful to note for another program that, for any variables or constants x, the simplified form of (x) is just x. */
/* This is also a program to "termify" a given simplified expression (according to the rules in the syntax and simplify program). Formally, we can define the process of termification as follows:
For any expressions x and y (assumed to be syntactically correct or simplified from syntactically correct terms), the termified form of xy is (xy). Also, the termified form of x is x.
The termified form of (x) is x. Then, for any expressions
x_1,...,x_n, if x is the termified form of x_1,...,x_n-1, then (xx_n) is the termified form of x_1,...,x_n. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LENGTH 10000

// this stores the sub-expressions of expression
typedef struct sub_expression {
        char* expression;
        int end_index;
} sub_expression;

/* Function to determine the syntactical correctness of a given expression */
int correct_syntax(char* expression);

/* Function to find the first term in the given expression */
char* term(char* expression, int index, int* end_index);

/* Function to find the right paren corresponding to a given left paren in an expression */
int right_paren(char* expression, int index);

/* Function to simplify an expression (assumed syntactically correct) according to the above rules */
char* simplify(char* expression);

/* Function to termify a given expression, if possible */
char* termify(char* expression, int recursive_calls);

/* Function to fully simplify a given expression; that is, a simplified expression is assumed to be passed in, and 
this function will simplify any simplifiable S or K's inside of the expression */
char* simplify_sk(char* expression);

/* Funtion to find n terms. Returns success or failure. */
int term_find(char* expression, int index, int n, sub_expression sub_expressions[]);

/* Function to return 1 if the end has been reached, and 0 otherwise */
int end(int i, int end);

int main(int argc, char* argv[]) {
	char* expression = (char*)malloc(MAX_LENGTH);
	if (argc != 2 || strlen(argv[1]) == 0) {
		printf("Exactly one expression must be supplied.\n");
		exit(1);
	}
	int ret_val = 0;
	ret_val = correct_syntax(argv[1]);
	if (ret_val == 1) {
		printf("The expression is syntactically correct.\n");
                expression = simplify(argv[1]);
                printf("The simplified expression is: %s\n", expression);
        }
	// if the initial expression is syntactically incorrect, we make an attempt to termify it, and see if the resulting expression is syntactically correct
	else if (ret_val == 0) {
		expression = termify(argv[1], 0);
		ret_val = correct_syntax(expression);
		if (ret_val == 1) {
			printf("The termified form of the expression is: %s\nIt is syntactically correct.\n", expression);
			// for later, we will need expression to be simplified
                        expression = simplify(expression);
		}
		else {
			printf("The expression is syntactically incorrect.\n");
		}
	}	
	// if the syntax has been found to be correct, then we see if there are any possibilities to use S or K simplification
	if (ret_val == 1) {
		expression = simplify_sk(expression);
		printf("The expression with S and K simplification is: %s\n", expression);
	}
	/*
	printf("The termified expression is: %s\n", termify(argv[1], 0));
	printf("The simplified expression is: %s\n", simplify_sk(termify(argv[1], 0)));
	*/
}

int correct_syntax(char* expression) {
	char* t1 = (char*)malloc(MAX_LENGTH);
	char* t2 = (char*)malloc(MAX_LENGTH);
	int first_index, second_index; // these track where the first and second terms end
	// if it is a single character that is either an uppercase or lowercase letter, it has correct syntax
	if (strlen(expression) == 1 && ((expression[0] >= 'a' && expression[0] <= 'z') || (expression[0] >= 'A' && expression[0] <= 'Z'))) {
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
	if (expression == NULL || index > (int)strlen(expression)-1) {
		return NULL;
	}
	else if (expression[index] != '(') {
		// if it is a right paren, we haven't found a term
		if (expression[index] == ')') {
			return NULL;
		}
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
        char* t1 = (char*)malloc(MAX_LENGTH);
        char* t2 = (char*)malloc(MAX_LENGTH);
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
	// if the expression is just three characters long (of the form (x)), then the simplified form is just x
	else if (strlen(expression) == 3) {
		simplified[0] = expression[1];
		return simplified;
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
                // if t2 is just a single character, then we let second_simplified equal it; otherwise we make a recursive call; also, if t2 is just three characters (meaning it is of the form 
		// (x)), we also simplify accordingly
                if (strlen(t2) == 1) {
                        strcpy(second_simplified, t2);
                        sprintf(simplified, "%s%s", first_simplified, second_simplified);
                }
		else if (strlen(t2) == 3) {
			second_simplified[0] = t2[1];
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

char* termify(char* expression, int recursive_calls) {
        sub_expression sub_expressions[MAX_LENGTH];
        int term_count = 0;
        int current_index = 0; // to track what the index of the expression is
	int stop; // for when the sub_expressions should stop
	int start; // for where the sub_expressions should start
	int sub_length = 0; // to track the length of the sub-expression
        char* termified = (char*)malloc(MAX_LENGTH);
        if (expression == NULL) {
                return NULL;
        }
        // in this case, expression is just a single character, meaning its termified form is the same 
        else if (strlen(expression) == 1) {
		//printf("single character: %s recursive calls: %d\n", expression, recursive_calls);
                return expression;
        }
	// in this case, expression is three characters, and is of the form (x)
	else if (strlen(expression) == 3 && (expression[0] == '(' && expression[strlen(expression)-1] == ')')) {
		termified[0] = expression[1];
		return termified;
	}
        // in this case expression consists of sub-expressions
        else {
		int end_index;
		// if the whole expression itself is something enclosed in parentheses, we handle the case differently
		if (term(expression, 0, &end_index) != 0 && end_index == (int)strlen(expression)-1) {
			start = 1;
			current_index = 1;
			stop = strlen(expression)-2;
		}
		else {
			start = 0;
			stop = strlen(expression)-1;
		}
                for (int i = 0; i < MAX_LENGTH; i++) {
                        // we find the next sub-term
                        sub_expressions[i].expression = term(expression, current_index, &sub_expressions[i].end_index);
                        // if no term was found, an error occured
                        if (sub_expressions[i].expression == NULL) {
                                printf("Invalid expression.\n");
                                exit(1);
                        }
                        // otherwise, if a term was found, and it is at the end of the expression, we have found all of the sub expressions
                        else if (sub_expressions[i].end_index == stop) {
                                term_count++;
                                break;
                        }
                        else {
                                // we update current_index
                                current_index = sub_expressions[i].end_index+1;
                                term_count++;
                        }
                }
        }
        // if there were only two terms x and y, the termified expression is (mn) where m is the termified form of x and n is the termified form of y
        if (term_count == 2) {
                sprintf(termified, "(%s%s)", termify(sub_expressions[0].expression, recursive_calls+1), termify(sub_expressions[1].expression, recursive_calls+1));
        }
        // otherwise we need to make a recursive call
        else {
		char* second_termified = (char*)malloc(MAX_LENGTH);
                char* first_termified = (char*)malloc(MAX_LENGTH); // this is the termified expression of the first n-1 sub expressions
                char* sub = (char*)malloc(MAX_LENGTH); // to store the string of the first n-1 sub expressions
		if (term_count == 1) {
			memmove(sub, &expression[start], sub_expressions[0].end_index);
                        first_termified = termify(sub, recursive_calls+1);
			sprintf(termified, "%s", first_termified);
			return termified;
		}
		if (start == 1) {
			sub_length = sub_expressions[term_count-2].end_index;
		}
		else {
			sub_length = sub_expressions[term_count-2].end_index+1;
		}
                memmove(sub, &expression[start], sub_length);
                first_termified = termify(sub, recursive_calls+1);
		second_termified = termify(sub_expressions[term_count-1].expression, recursive_calls+1);
                sprintf(termified, "(%s%s)", first_termified, second_termified);
        }
        return termified;
}

int term_find(char* expression, int index, int n, sub_expression sub_expressions[]) {
        int current_index = index;
        for (int i = 0; i < n; i++) {
                sub_expressions[i].expression = term(expression, current_index, &sub_expressions[i].end_index);
                // if no term was found, we cannot simplify
                if (sub_expressions[i].expression == NULL) {
                        return 0;
                }
                current_index = sub_expressions[i].end_index+1;
        }
        return 1;
}

char* simplify_sk(char* expression) {
	// loop to remove all simplifiable instances of S and K
	int done = 0;
        sub_expression sub_expressions[3];
	char* new_expression = (char*)malloc(MAX_LENGTH);
	int end_index;
        while (1) {
		int next_index;
		if (done == 1) {
			break;
		}
		// we first termify the expression, and then simplify the resulting term
		expression = simplify(termify(expression, 0));
		int last_index = (int)strlen(expression)-1; // to track the last index of the expression
                for (int i = 0; i < (int)strlen(expression); i++) {
                        char* previous_term = (char*)malloc(MAX_LENGTH);
			char* next_term = (char*)malloc(MAX_LENGTH);
                        if (expression[i] == 'S') {
                                if (term_find(expression, i+1, 3, sub_expressions) == 0) {
					done = end(i, last_index);
					continue;
                                }
				next_index = sub_expressions[2].end_index+1;
				// if the next index is out of the array, next_term is set to NULL. Otherwise it is set to the address of the next index of the expression
				if (next_index > (int)strlen(expression)-1) {
					next_term = NULL;
				}
				else {
					next_term = &expression[next_index];
				}
                                // there are two cases: either S is the first character of the expression, or it isn't
				// if it is not the first character, we need to ensure that it is the first character of some sub-term of the expression; that is, 
				// the previous index starts a term, and S is part of it
                                if (i != 0) {
					if (!(term(expression, i-1, &end_index) && i<end_index)) {
						done = end(i, last_index);
						continue;
					}					
                                        memmove(previous_term, expression, i);
					// if next_term is null, we do not append it to the expression
					if (next_term == NULL) {
						sprintf(new_expression, "%s%s%s(%s%s)", previous_term, sub_expressions[0].expression, sub_expressions[2].expression, sub_expressions[1].expression,
                                                                sub_expressions[2].expression);
					}
					else {
						sprintf(new_expression, "%s%s%s(%s%s)%s", previous_term, sub_expressions[0].expression, sub_expressions[2].expression, sub_expressions[1].expression, 
								sub_expressions[2].expression, next_term);
					}
					expression = new_expression;
				}
				else {
					// if next_term is null, we do not append it to the expression
					if (next_term == NULL) {
						sprintf(new_expression, "%s%s(%s%s)", sub_expressions[0].expression, sub_expressions[2].expression, sub_expressions[1].expression,
                                                                sub_expressions[2].expression);
					}
					else {
					sprintf(new_expression, "%s%s(%s%s)%s", sub_expressions[0].expression, sub_expressions[2].expression, sub_expressions[1].expression,
                                                                sub_expressions[2].expression, next_term);
					}
					expression = new_expression;
                                }
                                break;
                        }
                        else if (expression[i] == 'K') {
                                if (term_find(expression, i+1, 2, sub_expressions) == 0) {
					done = end(i, last_index);
					continue;
                                }
                                if (i == 0) {
                                        sprintf(expression, "%s%s", sub_expressions[0].expression, &expression[sub_expressions[1].end_index+1]);
                                }
                                else {
					if (!(term(expression, i-1, &end_index) && i<end_index)) {
						done = end(i, last_index);
						continue;
                                        }
                                        memmove(previous_term, expression, i);
                                        sprintf(expression, "%s%s%s", previous_term, sub_expressions[0].expression, &expression[sub_expressions[1].end_index+1]);
                                }
                                break;
                        }
			if (i == (int)strlen(expression)-1) {
				done = 1;
				break;
			}
		}
	}
	return expression;
}

int end(int i, int end) {
	if (i == end) {
		return 1;
	}
	return 0;
}
