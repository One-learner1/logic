#include "algorithm.h"

int correct_syntax(char* expression) {
	int ret_val = 0;
	int first_index, second_index; // these track where the first and second terms end
	// if it is a single character that is either an uppercase or lowercase letter, it has correct syntax
	if (strlen(expression) == 1 && ((expression[0] >= 'a' && expression[0] <= 'z') || (expression[0] >= 'A' && expression[0] <= 'Z'))) {
		return 1;
	}
	// the next case is when the first character is a left paren and the last is a right paren
	else if (expression[0] == '(' && expression[strlen(expression)-1] == ')') {
		char* t1 = (char*)malloc((int)strlen(expression));
		char* t2 = (char*)malloc((int)strlen(expression));
		// calls function to find the terms t_1 and t_2
		t1 = term(expression, 1, &first_index);
		t2 = term(expression, first_index+1, &second_index);
		// we check that both the terms t1 and t2 were found
		if (t1 == NULL || t2 == NULL) {
			ret_val = 0;
		}
		// we also check that the end of the second term is the second to last character of the expression
		if (second_index != (int)strlen(expression)-2) {
			ret_val = 0;
		} 
		// now we ensure that both t1 and t2 are terms
		if (correct_syntax(t1) && correct_syntax(t2)) {
			ret_val = 1;
		}
		else {
			ret_val = 0;
		}
		return ret_val;
		free(t1);
		free(t2);
	}
	// otherwise the expression does not start with a valid character
	else {
		return 0;
	}
}

char* term(char* expression, int index, int* end_index) {
	char* term = (char*)malloc((int)strlen(expression));
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
		char* t1 = (char*)malloc((int)strlen(expression));
		char* t2 = (char*)malloc((int)strlen(expression));
		char* simplified = (char*)malloc(strlen(expression));
                char* first_simplified = (char*)malloc((int)strlen(expression)); // to store the simplified form of t1
		printf("before first_simplified: %p\n", first_simplified);
                char* second_simplified = (char*)malloc((int)strlen(expression)); // to store the simplified form of t2
                // we first find t1 and t2
                t1 = term(expression, 1, &first_index);
                t2 = term(expression, first_index+1, &second_index);
                // we obtain the simplified version of t1
                first_simplified = simplify(t1);
		printf("first_simplified after: %p string: %s\n", first_simplified, first_simplified);
                // if t2 is just a single character, then we let second_simplified equal it; otherwise we make a recursive call; also, if t2 is just three characters (meaning it is of the form 
		// (x)), we also simplify accordingly
                if (strlen(t2) == 1) {
                        strcpy(second_simplified, t2);
                        sprintf(simplified, "%s%s", first_simplified, second_simplified);
                }
                // in this case, note that this means the second term is surrounded by parentheses (it is different from the previous case)
                else {
                        second_simplified = simplify(t2);
                        sprintf(simplified, "%s(%s)", first_simplified, second_simplified);
                }
		free(t1);
		free(t2);
		free(first_simplified);
		free(second_simplified);
                return simplified;
        }
        // otherwise an error has occured
        else {
                printf("Syntactically incorrect expression.\n");
                exit(1);
        }
}

char* termify(char* expression, int recursive_calls) {
        int term_count = 0;
        int current_index = 0; // to track what the index of the expression is
	int end_index = 0;
	char* sub = (char*)malloc((int)strlen(expression));
        if (expression == NULL) {
                return NULL;
        }
        // in this case, expression is just a single character, meaning its termified form is the same 
        else if (strlen(expression) == 1) {
		//printf("single character: %s recursive calls: %d\n", expression, recursive_calls);
                return expression;
        }
	// in this case, the expression is enclosed by parentheses, meaning we make a recursive call
	else if (term(expression, 0, &end_index) != NULL && end_index == (int)strlen(expression)-1) {
		memmove(sub, &expression[1], strlen(expression)-2);
		return termify(sub, recursive_calls+1);
	}
	// in the last case, we know that there are multiple sub-expressions
        // in this case expression consists of sub-expressions
        else {
		char* termified = (char*)malloc((int)strlen(expression));
		sub_expression* sub_expressions = (sub_expression*)malloc((int)strlen(expression));
                for (int i = 0; i < (int)strlen(expression); i++) {
                        // we find the next sub-term
                        sub_expressions[i].expression = term(expression, current_index, &sub_expressions[i].end_index);
                        // if no term was found, an error occured
                        if (sub_expressions[i].expression == NULL) {
                                printf("Invalid expression.\n");
                                exit(1);
                        }
                        // otherwise, if a term was found, and it is at the end of the expression, we have found all of the sub expressions
                       else if (sub_expressions[i].end_index == (int)strlen(expression)-1) {
                                term_count++;
                                break;
                        }
                        else {
                                // we update current_index
                                current_index = sub_expressions[i].end_index+1;
                                term_count++;
                        }
                }
		char* second_termified = (char*)malloc((int)strlen(expression));
                char* first_termified = (char*)malloc((int)strlen(expression)); // this is the termified expression of the first n-1 sub expressions
                memmove(sub, &expression[0], sub_expressions[term_count-2].end_index+1);
                first_termified = termify(sub, recursive_calls+1);
		second_termified = termify(sub_expressions[term_count-1].expression, recursive_calls+1);
                sprintf(termified, "(%s%s)", first_termified, second_termified);
		return termified;
        }
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
	char* new_expression = (char*)malloc((int)strlen(expression));
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
                        char* previous_term = (char*)malloc((int)strlen(expression));
			char* next_term = (char*)malloc((int)strlen(expression));
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
char* x_eliminate(char* expression, char variable, char** return_expression) {
	//char* test_expression = (char*)malloc((int)strlen(expression)); // expression to test if the eliminate is correct
	if (expression == NULL || variable == '\0') {
		printf("An expression and a variable must be simplified.\n");
		exit(1);
	}
	// we make sure they are not empty
	if ((int)strlen(expression) == 0) {
		printf("The expressions must not be empty.\n");
		exit(1);
	}
	if (!(variable >= 'a' && variable <= 'z')) {
		printf("The second argument must be a variable.\n");
		exit(1);
	}
	// we first termify the given expression, and then ensure that it is syntactically correct
	expression = termify(expression, 0);
	if (!correct_syntax(expression)) {
		printf("Expression is syntactically incorrect.\n");
		exit(1);
	}
	expression = eliminate(expression, variable, return_expression);
	printf("Return expression: %s\n", expression);
	//printf("The expression with %c elimination is %s\n", variable, expression);
	// we check to see if there is enough memory for 
	return expression;
	/*
	sprintf(test_expression, "(%s%c)", expression, variable);
	printf("test_expression: %s\n", test_expression);
	printf("new expression: %s\n", expression);
	//printf("Check on elimination: %s=%s\n", test_expression, simplify_sk(test_expression));
	int ret_val = 0;
	ret_val = strcmp(expression, simplify_sk(test_expression));
	//if the initial strcmp did not come out correct, it could be that the resulting expression was simplified too far, and we need to check to see if the simplified form of the given expression
	// equals the simplified form of the test_expression. For example, suppose we had the x-eliminate E of Kxy. Then Ex=Kxy=x
	if (ret_val == 0) {
		ret_val = strcmp(simplify_sk(expression), simplify_sk(test_expression))?0:1;
	}
	printf("The %s eliminate was obtained.\n", ret_val?"correct":"incorrect");
	return expression;
	*/
}

char* eliminate(char* expression, char variable, char** return_expression) {
	if ((int)strlen(expression) == 1 && expression[0] == variable) {
		// we return the expression for I, but written in terms of S and K
		printf("AAA\n");
		strcpy(*return_expression, "((SK)K)");
		return *return_expression;
	}
	else if (!var_find(expression, variable)) {
		printf("BBB\n");
		printf("expression pointer: %p\n", expression);
		sprintf(*return_expression, "(K%s)", expression);
		printf("return_expression: %p\n", return_expression);
		return *return_expression;
	}
	// otherwise we know that the expression is enclosed by parentheses
	else {
		// we find the two sub-terms of the epxression
		char* term1 = (char*)malloc(MAX_LENGTH);
		char* term2 = (char*)malloc(MAX_LENGTH);
		int end_index1;
		int end_index2;
		printf("CCC\n");
		printf("before recursive call: return_expression: %p\n", return_expression);
		printf("variable: %c first_term: %s\n", variable, term(expression, 1, &end_index1));
		printf("before recursive call: term1: %p term2: %p\n", term1, term2);
		term1 = eliminate(term(expression, 1, &end_index1), variable, &term1);
		printf("after recursive call: term1: %s pointer: %p\n", term1, &term1);
		term2 = eliminate(term(expression, end_index1+1, &end_index2), variable, &term2);
		printf("second term: %s\n", term(expression, end_index1+1, &end_index2));
		printf("after recursive call: term2: %s pointer: %p\n", term2, &term2);
 		sprintf(*return_expression, "((S%s)%s)", term1, term2);
		printf("after recursive call: return_expression: %p\n", return_expression);
		free(term1);
		printf("CCC\n");
		free(term2);
		return *return_expression;
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
