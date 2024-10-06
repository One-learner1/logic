/* A program to be given two arguments: a formula and an int i. The program will produce the sub-formula tree, and for each 
sub-formula, print out its truth value under the ith interpretation. It also produces a table of the variables in the formula, with their 
corresponding truth values. */
#include "tree.h"
int interpretations = 0; // to store the number of interpretations

// class for holding a variable, as well as its array of truth values
class variable {
public:
	variable() { var = '\0'; }
	
	inline int max_check(variable variables[]); // to check if an array of variables is full
	inline int num_vars(variable variables[]); // to return the number of variables in a given array of variables 
	inline void right_shift(variable variables[], int i); // to right shift an array of variables from the ith index
	inline void init_vars(variable variables[]); // initializes an array of variables

	char var; // to store the variable
	bool values[MAX_INTERPRETATIONS];
};

/* Function given an array of variables and a variable. Checks to see if the variable is in the array. Returns the 
index of where it is stored if it is found, -1 otherwise. */
int variable_check(variable variables[], char var);

/* Function to insert a new variable in alphabetical order into an array of variables. The given array is assumed to already 
be in alphabetical order. */ 
void var_insert(variable variables[], char var);

/* Function to, given an expression, find all of the distinct variables in the expression, and 
put them into the variables array. Will check if the formula contains too many variables. */
void variable_find(char* expression, variable variables[]);

/* Function to, given an array of variables, compute the truth values of those variables (according to 
the number of variables in the array). */
void variable_values(variable variables[]);

/* Function to assign the truth values of the sub-formulas for the ith interpretation. */
void formula_values(formula* f, variable variables[], int i);

int main(int argc, char* argv[]) {
	if (argc != 3) {
		printf("A formula and an int must be supplied (formula first, then the int).\n");
		exit(1);
	}
	if (!syntax(argv[1])) {
		printf("Invalid syntax.\n");
		exit(1);
	}
	formula* f = new formula(argv[1]);
	int i;
	list* components = nullptr;
	tree(argv[1], f, components);
	variable variables[MAX_VARIABLES];
	variables->init_vars(variables);
	variable_find(argv[1], variables);
	interpretations = (int)pow(2, var_count);
	// this ensures that the second argument was input properly
	i = atoi(argv[2]);
	if (!i_check(i, var_count) || strlen(argv[2]) != 1) {
		printf("i was input incorrectly.\n");
		exit(1);
	}
	variable_values(variables);
	// loops to print out a table of the truth values for the variables
	for (int j = 0; j < var_count; j++) {
		printf("%c ", variables[j].var);
	}
	printf("\n");
	for (int j = 0; j < interpretations; j++) {
		for (int k = 0; k < var_count; k++) {
			printf("%d ", variables[k].values[j]);
		}
		printf("\n");
	}
	for (int j = 0; j < interpretations; j++) {
		formula_values(f, variables, j);
	}
	f->print_tree(i-1); 
}	

inline int variable::max_check(variable variables[]) {
	if (variables[MAX_VARIABLES-1].var != '\0') {
		return 1;
	}
	return 0;
}

inline int variable::num_vars(variable variables[]) {
	int i;
	for (i = 0; i < MAX_VARIABLES; i++) {
		if (variables[i].var == '\0')
			break;
	}
	return i;
}

void variable_find(char* expression, variable variables[]) {
	if (!syntax(expression))
		return;
	for (int i = 0; i < strlen(expression); i++) {
		if (expression[i] >= 'A' && expression[i] <= 'Z') {
			if (variable_check(variables, expression[i]) == -1) {
				if (var_count++ >= MAX_VARIABLES) {
					printf("%d exceeds maximum number %d of variables.\n", var_count, MAX_VARIABLES);
					exit(1);
				}
				var_insert(variables, expression[i]);
			}
		}
	}
}

void var_insert(variable variables[], char var) {
	if (variables->max_check(variables))
		return;
	int var_num = variables->num_vars(variables);
	for (int i = 0; i < MAX_VARIABLES; i++) {
		if (i >= var_num) {
			variables[i].var = var;
			break;
		}
		else if (i == var_num) {
			variables[i+1].var = var;
			break;
		}
		else if (variables[i].var < var && var < variables[i+1].var) {
			variables->right_shift(variables, i+1);
			variables[i+1].var = var;
			break;
		}
		else if (variables[i].var == var)
			return;
	}
}

inline void variable::right_shift(variable variables[], int i) {
	if (i >= MAX_VARIABLES-1 || i < 0)
		return;
	for (int j = MAX_VARIABLES-1; j > i; j--) {
		variables[j] = variables[j-1];
	}
}

void variable_values(variable variables[]) {
	bool value = true;
	int count = 1;
	for (int i = 0; i < var_count; i++) {
		for (int j = 0; j < interpretations; j++, count++) {
			variables[i].values[j] = value;
			if (count == (int)pow(2, var_count-(i+1))) {
				count = 0;
				value = !value;
			}
		}
	}
}

int variable_check(variable variables[], char var) {
	for (int i = 0; i < MAX_VARIABLES; i++) {
		if (variables[i].var == var)
			return i;
	}
	return -1;
}

void formula_values(formula* f, variable variables[], int i) {
	int var_place;
	switch (f->op) {
	case '\0':
		var_place = variable_check(variables, f->expression[0]);
		if (var_place == -1) {
			printf("Error in assigning truth values.\n");	
			exit(1);
		}
		f->values[i] = variables[var_place].values[i];
		break;
	case '~':
		formula_values(f->next1, variables, i);
		f->values[i] = !f->next1->values[i];
		break;
	case '^':
		formula_values(f->next1, variables, i);
		formula_values(f->next2, variables, i);
		f->values[i] = f->next1->values[i] && f->next2->values[i];
		break;
	case 'v':
		formula_values(f->next1, variables, i);
		formula_values(f->next2, variables, i);
		f->values[i] = f->next1->values[i] || f->next2->values[i];
		break;
	case '=':
		formula_values(f->next1, variables, i);
		formula_values(f->next2, variables, i);
		f->values[i] = (f->next1->values[i] && f->next2->values[i]) || (!f->next1->values[i] && !f->next2->values[i]);
		break;
	case '>':
		formula_values(f->next1, variables, i);
                formula_values(f->next2, variables, i);
		f->values[i] = !f->next1->values[i] || f->next2->values[i];
		break;
	default:
		printf("Error in assigning truth values. Unkown operator '%c'.\n", f->op);
		exit(1);
	}
}

inline void variable::init_vars(variable variables[]) {
	for (int i = 0; i < MAX_VARIABLES; i++) {
		variables[i].var = '\0';
	}
}
