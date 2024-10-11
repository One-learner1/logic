#include "../truth_tables/truth_table.h"
#define MAX_AXIOMS 100 // for the maximum number of axioms that can be input in the input file
#define MAX_INFERENCE 100 // for the maximum number of inference rules that can be input in the input file
#define MAX_PREMISES 10 // for the maximum number of premises an inference rule can have
#define MAX_STEPS 100 // for the maximum number of steps in a proof

/* This is the class to store the axioms from the input file. For now, we are only considering axiom schemes, so each axiom will be a propositional formula that 
is a regular expression (standing for any propositional formula that matches the described form), and also will include a name to reference that axiom */

class axiom {
public:
	axiom() :f(new formula), name(new char[MAX_LENGTH]) {}
	axiom(char expression[], char n[]) :f(new formula(expression)) {
			name = new char[MAX_LENGTH];
			strcpy(name, n); }; // construct an axiom for a given formula

	formula* f;
	char* name;
};

// this struct stores a set of axioms, as well as the number of axioms in the set
class axiom_set {
public:
	axiom aset[MAX_AXIOMS];
	int a_count;
};

/* This is the class to store the inference rules from the input file. Each inference rules consists of the following: a set of premises (each 
propositional formulas); a conclusion (also a propositional formula); a name for how we should refer to this rule; and a count for the 
number of premises. */
class inference_rule {
public:
	inference_rule() :premises(new formula[MAX_PREMISES]), conclusion(new formula), name(new char[MAX_LENGTH]) {}
	inference_rule(char* expression) :premises(new formula[MAX_PREMISES]), conclusion(new formula), name(expression), premise_count(0) {} // construct an inference rule

	formula* premises;
	formula* conclusion;
	char* name;
	int premise_count;
};

// this struct stores a set of inference rules, a well as the number of inference rules in the set
class inference_set {
public:
	inference_rule rules[MAX_INFERENCE];
	int i_count;
};

/* This is the class to store a single line of a proof. Each line should consist of the following: a step; a formula; a deductive reason
(i.e., an inference rule and the previous lines of the proof used in the inference rule to obtain the formula); the deductive reason is made out of 
an array of steps, as well as an inference rule. */
class p_line {
public:
	p_line() : f(new formula), d_steps(new int[MAX_STEPS]), d_rule(new char[MAX_LENGTH]) {}
	p_line(int i, char* expression, int* steps, char* rule) :f(new formula(expression)), 
								    step(i), d_steps(steps), d_rule(rule) {}

	formula* f;
	int step;
	int* d_steps; // to store the steps listed in the proof
	char* d_rule; // to store the name of the rule used. Set to 0 if no rule was used (i.e., if the formula is being reiterated from an eariler 
		    // step in the proof
};

/* This is a function to input axioms and inference rules from a given input file.
Any input errors will be handled in the function, and if any are encountered, the function will exit. 

The input file for the axioms should have the following form: The input file should be started first by the input for the 
axioms. This should be started by a line reading "START AXIOMS". After this, each axiom should be presented by a line consisting of 
the name to reference the axiom (this name may not be a string of numbers, as this would cause confusion when distinguishing axiom names from steps in the proof),
followed by the axiom. The input for the axioms should be ended by a line reading "END AXIOMS". 
For instance:
"START AXIOMS
S1 (Xv~X)
S2 (X=~~X)
S3 (X>(Y>X))
S4 (((X>Y)^(X>~Y))>~X)
END AXIOMS"
would be acceptable input for the axioms.

Next the input file needs to contain the input for the inference rules. This section should start with a line reading "START INFERENCE". After this, 
each inference rule should be presented two lines: the first contains the name of the inference rule, followed by the premises, while the second line 
contains the conclusion. The input for the inference rules should be ended by a line reading "END INFERENCE".
For instance:
"START INFERENCE
MP X, (X>Y)
   Y
MT ~Y, (X>Y)
   ~X
END INFERENCE"
would be acceptable input for the inference rules. */
void system_input(FILE* f, axiom_set* axioms, inference_set* i_set);

/* This is a function to input a proof from a given input file. Any input errors will be handled in the function, 
and if any are encountered, the function will exit. */
void proof_input(FILE* f, p_line proof[]);

/* This functions checks whether a proof in a given system is valid. If it is invalid, the function will give a description of why it is invalid, and 
return false. If it is valid, it will return true. */
bool proof_check(p_line proof[], axiom_set* axioms, inference_set* i_set); 

/* This function inputs the axioms */
void axiom_input(FILE* f, axiom_set* axioms);

/* This function inputs the inference rules */
void inference_input(FILE* f, inference_set* i_rules);

/* This function checks whether a given string is a string of only positive integers. If it is, it returns true, and returns false
otherwise. */
bool positive_string(char* string);

/* This function checks whether a string is made up just of whitespace characters. Returns true if that is the case, and false 
otherwise. */
bool whitespace_string(char* string);

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Exactly one file name must be supplied.\n");
		exit(EXIT_FAILURE);
	}
	FILE* f;
	axiom_set* axioms = new axiom_set;
	inference_set* i_rules = new inference_set;
	p_line proof[MAX_STEPS];
	f = fopen(argv[1], "r");
	if (f == nullptr) {
		printf("Error opening input file: \"%s\"\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	// we input the axioms and inference rules
	system_input(f, axioms, i_rules);	
	printf("System input files have correct syntax.\n");
	// now we input the proof
	//proof_input(f, proof); 
	// for now, the proof_check function will note if the proof is invalid, so we only need to print out in main the case when the proof is valid
	//if (proof_check(proof, axioms, i_rules))
	//	printf("The proof is correct.\n");
	fclose(f);
}

bool positive_string(char* string) {
	if (string == NULL || strlen(string) == 0)
		return false;
	bool num = true;
	for (int i = 0; i < strlen(string); i++)
		num &= isdigit(string[i]);
	return num;
}

bool whitespace_string(char* string) {
	if (string == NULL || strlen(string) == 0)
		return false;
	bool whitespace = true;
	for (int i = 0; i < strlen(string); i++)
		whitespace &= isspace(string[i]);
	return whitespace;
}

void system_input(FILE* f, axiom_set* axioms, inference_set* i_rules) {
	if (f == nullptr) {
		printf("No input file provided to system input.\n");
		exit(EXIT_FAILURE);
	}
	if (axioms == nullptr) {
		printf("No axioms parameter provided to system input.\n");
		exit(EXIT_FAILURE);
	}
	if (i_rules == nullptr) {
		printf("No inference rules parameter provided to system input.\n");
		exit(EXIT_FAILURE);
	}
	// first we input the axioms
	axiom_input(f, axioms);
	// then we input the inference rules
	inference_input(f, i_rules);
}

void inference_input(FILE* f, inference_set* i_rules) {
	if (f == nullptr) {
		printf("No input file provided to system input.\n");
		exit(EXIT_FAILURE);	
	}
	if (i_rules == nullptr) {
		printf("No inference rules parameter provided to inferece input.\n");
		exit(EXIT_FAILURE);
	}
	char stringbuf[MAX_LENGTH*MAX_PREMISES];
	char name[MAX_LENGTH];
	char* pch;
	char t_string[MAX_LENGTH];
	int j = 0;
	// we find the next line in the input file that contains something other than a newline
	while (1) {
		if (fgets(stringbuf, MAX_LENGTH, f) == nullptr) {
			printf("The inference rules must be started by \"START INFERENCE\".\n");
			exit(EXIT_FAILURE);
		}
		if (!whitespace_string(stringbuf))
			break;
	}
	printf("CCC\n");
	stringbuf[strcspn(stringbuf, "\n")] = '\0';
	if (strcmp(stringbuf, "START INFERENCE") != 0) {
		printf("The axioms cannot be started by \"%s\"\n", stringbuf);
		printf("They must be started by \"START AXIOMS\"\n");
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < MAX_INFERENCE; i++, i_rules->i_count++) {
		if (fscanf(f, "%s", name) != 1 || fgets(stringbuf, MAX_LENGTH, f) == nullptr) {
			printf("The axioms must be ended by \"END AXIOMS\".\n");
			exit(EXIT_FAILURE);
                }
		stringbuf[strcspn(stringbuf, "\n")] = '\0';
		printf("i: %d name: %s stringbuf: %s\n", i, name, stringbuf);
		// we check to see if we have reached the end of the inference rules. We do the sscanf as the string containing
		// "INFERENCE" in "END INFERENCE" may also include space, so we sscanf from this string to just extract the
		// "INFERENCE"
		sscanf(stringbuf, "%s", t_string);
		printf("t_string: %s\n", t_string);
		if (!strcmp(name, "END") && !strcmp(t_string, "INFERENCE")) return;
		// we insure the name was input correctly
		if (strlen(name) == 0) {
			printf("Inference rule %d: no name provided.\n", i+1);
			exit(EXIT_FAILURE);
		}
                if (positive_string(name)) {
                        printf("Error with name \"%s\": Names cannot be strings only consisting of positive integers.\n", name);
                        exit(EXIT_FAILURE);
                }
		// we must ensure that stringbuf is of the following format: X, Y, Z, ... where X, Y, Z, are formulas.
		pch = strtok(stringbuf, ", \t");
		// if pch is already nullptr, we have an error
		if (pch == nullptr) {
			printf("Inference rule %d contains no premises.\n", i+1);
			exit(EXIT_FAILURE);
		}
		i_rules->rules[i] = inference_rule(name);
		j = 0;
		do {
			printf("pch: %s\n", pch);
			if (!syntax(pch)) {
				printf("\"%s\" does not have correct syntax.\n", pch);
				exit(EXIT_FAILURE);
			}
			strcpy(i_rules->rules[i].premises[j].expression, pch);
			i_rules->rules[i].premise_count++;
			if (i_rules->rules[i].premise_count > MAX_PREMISES) {
				printf("Inference rule %d has too many premises (maximum number is %d).\n", i+1, MAX_PREMISES);
				exit(EXIT_FAILURE);
			}
			pch = strtok(NULL, " ,\t");	
			j++;
		} while (pch != NULL);
		// now we input the conclusion of this inference rule
		if (fscanf(f, "%s", stringbuf) != 1) {
			printf("Inference rule %d does not have a conclusion.\n", i+1);
			exit(EXIT_FAILURE);
		}
		stringbuf[strcspn(stringbuf, "\n")] = '\0';
		if (!syntax(stringbuf)) {
			printf("Inference rule %d: \"%s\" is not correct syntax.\n", i+1, stringbuf);
			exit(EXIT_FAILURE);
		}
		strcpy(i_rules->rules[i].conclusion->expression, stringbuf); 
		printf("conclusion: %s\n", stringbuf);
	}
	// at this point we need to have reached a line reading "END INFERENCE"
	if (fgets(stringbuf, MAX_LENGTH, f) == nullptr) {
		printf("The inference rules must be ended by \"END INFERECE\".\n");
                exit(EXIT_FAILURE);
	}
	stringbuf[strcspn(stringbuf, "\n")] = '\0';
	if (strcmp(stringbuf, "END INFERENCE") != 0) {
		printf("The inference rules must be ended by \"END INFERECE\".\n");
                exit(EXIT_FAILURE);
	}
}

void axiom_input(FILE* f, axiom_set* axioms) {
	if (f == nullptr) {
		printf("No input file provided to axiom input.\n");
		exit(EXIT_FAILURE);
	}
	if (axioms == nullptr) {
		printf("No axioms parameter provided to axiom input.\n");
		exit(EXIT_FAILURE);
	}
	char stringbuf[MAX_LENGTH];
	char name[MAX_LENGTH];
	if (fgets(stringbuf, MAX_LENGTH, f) == nullptr) {
		printf("The axioms must be started by \"START AXIOMS\".\n");
		exit(EXIT_FAILURE);
	} 
	stringbuf[strcspn(stringbuf, "\n")] = '\0';
	if (strcmp(stringbuf, "START AXIOMS") != 0) {
		printf("AAA\n");
		printf("The axioms cannot be started by \"%s\"\n", stringbuf);
		printf("They must be started by \"START AXIOMS\"\n");
		exit(EXIT_FAILURE);
	}
	printf("stringbuf: %s\n", stringbuf);
	int i = 0;
	for (i = 0; i < MAX_AXIOMS; i++, axioms->a_count++) {
		if (fscanf(f, "%s%s", name, stringbuf) != 2) {
			printf("The axioms must be ended by \"END AXIOMS\".\n");
			exit(EXIT_FAILURE);
		}
		printf("i: %d name: %s stringbuf: %s\n", i, name, stringbuf);
		// we check if this possibly could end the string
		if (!strcmp(name, "END") && !strcmp(stringbuf, "AXIOMS")) return;
		// if the string was incorrect syntax, we have an error
		if (!syntax(stringbuf)) {
			printf("\"%s\" is incorrect syntax.\n", stringbuf);
			exit(EXIT_FAILURE);
		}
		// now we ensure that the name is not a string of positive integers
		if (strlen(name) == 0) {
			printf("Axiom %d: no name provided.\n", i+1);
			exit(EXIT_FAILURE);
		}
		
		if (positive_string(name)) {
			printf("Error with name \"%s\": Names cannot be strings only consisting of positive integers.\n", name);
			exit(EXIT_FAILURE);
		}
		//axioms->aset[i] = axiom(stringbuf, name);
	}
	printf("i: %d\n", i);
	exit(1);
	// at this point, we need to have reached a line with "END AXIOMS" on it
	if (fgets(stringbuf, MAX_LENGTH, f) == nullptr) {
		printf("The axioms must be ended by \"END AXIOMS\".\n");
                exit(EXIT_FAILURE);
	}
	stringbuf[strcspn(stringbuf, "\n")] = '\0';
	if (strcmp(stringbuf, "END AXIOMS") != 0) {
		printf("The axioms must be ended by \"END AXIOMS\".\n");
                exit(EXIT_FAILURE);
	}
}
