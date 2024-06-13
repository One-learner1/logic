#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LENGTH 100

/* Function to find the corresponding right parenthesis for a left paren in the expression */
int find_right(char* expression, int index) {
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

/* Function to remove a set of parenthesis from the expression (the left paren is indictaed by left, the right is indicated by the right) */
char* paren_remove(char* expression, int left, int right) {
	char* new_expression = (char*)malloc(MAX_LENGTH);
	char* temp1 = (char*)malloc(MAX_LENGTH);
	char* temp2 = (char*)malloc(MAX_LENGTH);
	char* temp3 = (char*)malloc(MAX_LENGTH);
	temp2 = &(expression[right])+1;
	if (temp2 == NULL) {
		printf("null\n");
	}
	memmove(temp3, &(expression[left+1]), (right-1)-left);
	sprintf(new_expression, "%s%s", temp2, temp3);
	if (left == 0) {
		return new_expression;
	}
	else {
		memmove(temp1, &(expression[0]), left);
		strlcat(temp1, new_expression, sizeof(new_expression));
		return temp1;
	}
}

// this function checks if any extraneous parentheses were encountered
int extraneous_check(char* expression, int* index) {
	int extraneous = 0;
	for (int i = 0; i < (int)strlen(expression); i++) {
                if (expression[i] == '(') { 
                        int right_paren = find_right(expression, i);
                        // this is the case where the expression contains only a single character
                        if (right_paren == i+2) {
				*index = i;
				return 1;
                        }
                        // in this case, we know that the expression contains another expression enclosed by parentheses
                        else {  
                                int second_right = find_right(expression, i+1);
                                if (second_right == right_paren-1) {
					*index = i;
					return 1;
                                }
                        }
                }
        }
	return extraneous;
}
	 

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("An expression must be entered.\n");
		exit(1);
	}
	char* expression = (char*)malloc(MAX_LENGTH);
	expression = argv[1];
	printf("Given expression: %s\n", expression);
	int index;
	while (extraneous_check(expression, &index) == 1) {
		expression = paren_remove(expression, index, find_right(expression, index));
		printf("expression: %s\n", expression);
	} 
	printf("Simplified expression: %s\n", expression);
}
				
