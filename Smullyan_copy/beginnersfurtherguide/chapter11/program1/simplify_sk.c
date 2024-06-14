/* This program simplifies an expression (assumed either syntactically correct or the simplified form of a syntactically correct expression) using the rules for S and K: 
Sxyz=xz(yz) and Kxy=x. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LENGTH 100

// this stores the sub-expressions of expression
typedef struct sub_expression {
        char* expression;
        int end_index;
} sub_expression;

/* Funtion to find n terms. Returns success or failure. */
int term_find(char* expression, int index, int n, sub_expression sub_expressions[]) {
	int current_index = index;
	for (int i = 0; i < n; i++) {
		sub_expressions[i].expression = term(expression, current_index+1, &sub_expressions[i].end_index);
		// if no term was found, we cannot simplify
		if (sub_expressions[i].expression == NULL) {
			return 0;
		}
		current_index = sub_expressions[i].end_index+1;
        }
	return 1;
}

int main(int argc, char* argv[]) {
	char* expression = (char*)malloc(MAX_LENGTH);
	if (argc != 2 || strlen(argv[1]) == 0) {
		printf("An expression must be supplied.\n");
		exit(1);
	}
	expression = argv[1];
	// loop to remove all simplifiable instances of S and K
	int index;
	int first_end = 0;
	int second_end = 0;
	char* simplified = (char*)malloc(MAX_LENGTH);
	sub_expression sub_expressions[3];
	while (1) {
		int current_index;
		int done = 0;
		int too_few = 0; // to track whether there are too few variables to simplify
		for (int i = 0; i < strlen(expression); i++) {
			current_index = i+1;
			char* previous_term = (char*)malloc(MAX_LENGTH);
			char* expanded = (char*)malloc(MAX_LENGTH); // this stores the expression obtained after using either the S or K rule
			if (expression[i] == 'S') {
				if (term_find(expression, i+1, 3, sub_expressions) == 0) {
					break;
				}
				// there are two cases: either S is the first character of the expression, or it isn't
				if (i == 0) {
					sprintf(expression, "%s%s(%s%s)%s", sub_expressions[0].expression, sub_expressions[2].expression, sub_expressions[1].expression,
								sub_expressions[2].expression, &expression[sub_expressions[3].end_index+1]);
					
				}
				else {
					memmove(previous_term, expression, i);
					sprintf(expression, "%s%s%s(%s%s)%s", previous_term, sub_expressions[0].expression, sub_expressions[2].expression, sub_expressions[1].expression,
                                                                sub_expressions[2].expression, &expression[sub_expressions[3].end_index+1]);	
				break;
			}
			else if (expression[i] == 'K') {
				if (term_find(expression, i+1, 2, sub_expressions) == 0) {
					break;
				}
				if (i == 0) {
					sprintf(expression, "%s%s", sub_expressions[0].expression, &expression[sub_expressions[2].end_index+1]);
				}
				else {
					memove(previous_term, expression, i);
					sprintf(expression, "%s%s%s", previous_term, sub_expressions[0].expression, &expression[sub_expressions[2].end_index+1]);
				}
				break;
			}
			if (i == strlen(expression)-1) {
				done = 1;
				break;
			}
		}
	}
}		
