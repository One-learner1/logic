#include "../truth_tables/truth_table.h"
#define MAX_AXIOMS 100 // for the maximum number of axioms that can be input in the input file
#define MAX_INFERENCE 100 // for the maximum number of inference rules that can be input in the input file
#define MAX_PREMISES 10 // for the maximum number of premises an inference rule can have
#define MAX_STEPS 100 // for the maximum number of steps in a proof

/* This is the class to store the axioms from the input file. For now, we are only considering axiom schemes, so each axiom will be a propositional formula that 
is a regular expression (standing for any propositional formula that matches the described form). */

class axiom {
public:
	axiom(char expression[]) :f{new formula(expression)} {} // construct an axiom for a given formula

	formula* f;
}

/* This is the class to store the inference rules from the input file. Each inference rules consists of the following: a set of premises (each 
propositional formulas); a conclusion (also a propositional formula); and a name for how we should refer to this rule. */
class inference_rule {
public:
	axiom(char* expression) :premises{new formula[MAX_PREMISES]}, conclusion{new formula}, name{expression} {} // construct an inference rule

	formula* premises;
	formula* conclusion;
	char* name;
}

/* This is the class to store a single line of a proof. Each line should consist of the following: a step; a formula; a deductive reason
(i.e., an inference rule and the previous lines of the proof used in the inference rule to obtain the formula); the deductive reason is made out of 
an array of steps, as well as an inference rule. */
class p_line {
public:
	p_line(int i, char* expression, int* steps, char* rule) :f{new formula(expression)}, 
								    step{i}, d_steps{steps}, d_rule{rule} {}

	formula* f;
	int step;
	int* d_steps; // to store the steps listed in the proof
	char* d_rule; // to store the name of the rule used. Set to 0 if no rule was used (i.e., if the formula is being reiterated from an eariler 
		    // step in the proof
}

/* This is a function to input axioms and inference rules from a given input file.
Any input errors will be handled in the function, and if any are encountered, the function will exit. */
void system_input(FILE* f, axiom axioms[], inference_rule i_rules[]);

/* This is a function to input a proof from a given input file. Any input errors will be handled in the function, 
and if any are encountered, the function will exit. */
void proof_input(FILE* f, p_line proof[]);

/* This functions checks whether a proof in a given system is valid. If it is invalid, the function will give a description of why it is invalid, and 
return false. If it is valid, it will return true. */
bool proof_check(p_line proof[], axiom axioms[], inference_rule i_rules[]); 

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Exactly one file name must be supplied.\n");
		exit(EXIT_FAILURE);
	}
	FILE* f;
	axiom axioms = new axiom[MAX_AXIOMS];
	inference_rule i_rules = new inference_rule[MAX_INFERENCE];
	p_line proof[MAX_STEPS];
	f = fopen(argv[1]);
	if (f == nullptr) {
		printf("Error opening input file: \"%s\"\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	// we input the axioms and inference rules
	system_input(f, axioms, i_rules);	
	// now we input the proof
	proof_input(f, proof); 
	// for now, the proof_check function will note if the proof is invalid, so we only need to print out in main the case when the proof is valid
	if (proof_check(proof, axioms, i_rules))
		printf("The proof is correct.\n");
}
