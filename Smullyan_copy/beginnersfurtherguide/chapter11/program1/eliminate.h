/* Header file for the elimination functions */
#include "syntax_simplify.h"

/* Function to preform the elimination, given an expression and a variable */
char* x_eliminate(char* expression, char* variable);

/* The function to actually do the elimination */
char* eliminate(char* expression, char variable);

/* Function to determine if the variable occurs in the given expression */
int var_find(char* expression, char variable);
