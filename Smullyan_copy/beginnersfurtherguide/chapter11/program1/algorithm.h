#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LENGTH 1000000

// this stores the sub-expressions of expression
typedef struct sub_expression {
        char* expression;
        int end_index;
} sub_expression;

/* Function to preform the elimination, given an expression and a variable */
char* x_eliminate(char* expression, char variable);
                
/* The function to actually do the elimination */
char* eliminate(char* expression, char variable);
        
/* Function to determine if the variable occurs in the given expression */
int var_find(char* expression, char variable);

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
