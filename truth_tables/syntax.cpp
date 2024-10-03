/* Program to check the syntactical correctness of a propositional formula. Rules for well-formed formulas (WFF's) are as follows:
1: All upper case letters are WFF's
2: For any WFF P, ~P is a WFF
3: For any WFFs P and Q, (P^Q) is a WFF
4: For any WFFs P and Q, (PvQ) is a WFF
5: For any WFFs P and Q, (P=Q) is a WFF
6: For any WFFs P and Q, (P>Q) is a WFF
A formula is a WFF if and only if it is a WFF as a consequence of the six rules above.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LENGTH 50

/* Function to attempt to find a right paren matching a given left paren, and return the index of the right paren */
int right_paren(char* formula, int index);

/* Function to return the index of the end of a string of negations */
int negation(char* formula, int index);

/* Function to check the correctness of a given formula */
int correct(char* formula);

/* Function to check if a given symbol matches any of the allowed connectives (other than ~) */
int correct_connective(char connective);

/* Function to return a "potential" formula, given an index of a larger expression. For example, if 
we were given the formula "((X>Y)^X)" and index 1, it would return "(X>Y)", and if given the same formula 
with index 7, it would return "X". If it were given the (incorrect) formula 
"(Xy>Z)" and index 1, it would return "Xy". If no potential formula is found (or if the index goes past the length of the 
expression), NULL is returned. This function also updates the index directly after the end of the potential formula. */
int potential(char* formula, int index, int& end_index, char term[]); 

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Exactly one formula must be supplied.\n");
		exit(1);
	}
	if (argv[1] == nullptr || strlen(argv[1]) == 0) {
		printf("Error: no formula provided.\n");
		exit(1);
	}
	if (strlen(argv[1]) > MAX_LENGTH) {
		printf("Formula can be at most %d characters.\n", MAX_LENGTH);
		exit(1);
	}
	printf("The formula is %s.\n", correct(argv[1])?"syntactically correct":"syntactically incorrect");
}

int correct(char* formula) {
	if (formula == nullptr)
		return 0;
	if ((int)strlen(formula) == 1)
		return formula[0] >= 'A' && formula[0] <= 'Z';
	int end_index1, end_index2;
	char sub1[MAX_LENGTH];
	char sub2[MAX_LENGTH];
	for (int i = 0; i < MAX_LENGTH; i++) {
		sub1[i] = sub2[i] = '\0';
	}
	if (formula[0] == '~') {
		if (!potential(formula, 1, end_index1, sub1))
			return 0;
		if (end_index1 != (int)strlen(formula))
			return 0;
		if (!correct(sub1))
			return 0;
		
		return 1;
	}
	else if (formula[0] == '(') {
		char connective;
		if (!potential(formula, 1, end_index1, sub1))
			return 0;
		if (!correct(sub1))
			return 0;
		connective = formula[end_index1];
		if (!potential(formula, end_index1+1, end_index2, sub2))
			return 0;
		if (!correct(sub2))
			return 0;
		if (!correct_connective(connective))
			return 0;
		if (end_index2 != (int)strlen(formula)-1)
			return 0;
		return 1;
	}
	else {
		return 0;
	}
}
		
int right_paren(char* formula, int index) {
	int paren_count = 0;
	if (formula == nullptr)
		return -1;
	if (index >= (int)strlen(formula) || index < 0)
		return -1;
	for (int i = index; i < (int)strlen(formula); i++) {
		if (formula[i] == '(')
			paren_count++;
		if (formula[i] == ')') {
			paren_count--;
			if (paren_count == 0)
				return i;	
		}
	}
	return -1;
}
	
int negation(char* formula, int index) {
	if (formula == nullptr)
		return -1;
	if (index >= strlen(formula) || index < 0)
		return -1;
	int i = 0;
	for (i = index; i<(int)strlen(formula) && formula[i] == '~'; i++);
	return i;
}

int potential(char* formula, int index, int& end_index, char term[]) {
	if (formula == nullptr)
		return 0;
	if (index >= strlen(formula) || index < 0)
		return 0;
	int current_index = index;
	current_index = negation(formula, index);
	if (current_index == -1)
		return 0;
	if (formula[current_index] != '(') {
		strncpy(term, &formula[index], (current_index-index)+1);
		end_index = current_index+1;
	}
	else {
		int right = right_paren(formula, current_index);
		if (right == -1)
			return 0;
		end_index = right+1;
		strncpy(term, &formula[index], (right-index)+1);
	}
	return 1;
}
	

int correct_connective(char connective) {
	switch (connective) {
	case '>':
		return 1;
	case '^':
		return 1;
	case '=':
		return 1;
	case 'v':
		return 1;
	default:
		return 0;
	}
}
