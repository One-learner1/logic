#include "algorithm.h"

int main(int argc, char* argv[]) {
	char variable = argv[1][0];
	char* expression = (char*)malloc(MAX_LENGTH);
	char* return_expression = (char*)malloc(MAX_LENGTH);
	expression = argv[2];
	expression = eliminate(expression, variable, &return_expression);
	printf("The eliminate is: %s\n", expression);
}
	
