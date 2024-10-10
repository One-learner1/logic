#include "syntax.h"
#include <math.h>
#define MAX_VARIABLES 6 // for the maximum number of distinct variables in a foromula
#define MAX_INTERPRETATIONS 64 // should be 2^MAX_VARIABLES
int var_count = 0; // to store the number of variables in the formula

/* The formula class is meant to store a node on the sub-formula tree of a given expression.
The first constructor constructs an origin of the
tree, i.e., one where previous == NULL. Thus one needs to explicitly alter the previous (and start_index) when using
the constructor. The second constructor does the same as the first, but is passed in a value for the expression. */
class formula {
public:
        formula() { start_index = 0; previous = nullptr; }
        formula(char* f) {
                if (f == nullptr || strlen(f) == 0 || strlen(f) > MAX_LENGTH) {
                        printf("Constructor error in formula.\n");
                        exit(1);
                }
                start_index = 0; previous = nullptr; strcpy(expression, f);
        }

        inline int depth(); // This returns the depth of the node in the tree, with depth 0 for the origin
        inline void print(); // This prints out n number of tabs, where n is the depth of the node, followed by the
                                // formula stored in the node
	inline void print_tree(); // prints a sub-formula tree
	inline void print_tree(int i); // prints a sub-formula tree, and prints the ith truth value for each sub-formula
	inline int op_find(int index, bool& value, int i); // function to check if a given index matches any op_indeces of the formula 
						  // subformulas of f. If so, it assigns to value the value of that 
						  // operator under interpretation number i
	inline int precedence_find(int index); // for the check if a given index has an operator precedence. If so, the precedence is 
					       // returned. Otherwise -1 is returhed.
	inline int prec_init(int precedence); /// function for initializing the operator precedences
	inline bool sub_find(formula* sub); // function to see if sub is a sub-formula of this
	inline formula* start_find(int start_index); // function to find a sub-formula of this with the given start_index
					             // returns the pointer to that formula if found, nullptr otherwise

        char op; // to store the operator of the formula. If the formula is atomic, op will be set to '\0'
        char expression[MAX_LENGTH]; // to store the actual sub-formula contained in this node
        int op_index; // the index of the operator of this sub-formula. Set to -1 if atomic.
        int start_index; // the index where the sub-formula starts in the expression
        formula* previous; // a pointer to the parent of this node on the tree
        formula* next1; // a pointer to the first child of this node on the tree
        formula* next2; // a pointer to the second child of this node on the tree
                        /* next1 and next2 will be determined by the following rules. Let S be the sub-formula this node contains. If
                        S is atomic, then both next1 and next2 will be set to NULL. If S starts with a '~', then
                        next1 will be initialized, while next2 will be set to NULL. If S starts with a '(', then
                        both next1 and next2 will be initialized. */
	bool values[MAX_INTERPRETATIONS]; // to store the truth-values of the given sub-formula
	int interpretations; // should be equal to 2^n where n is the number of distinct variables in the expression
	int var_count; // to store the number of variables in expression
	int op_precedence; // to store the operator precedence
};
/* Here is an example of a formula node. Consider the formula "((X>Y)^Z)". The node where
the expression was "(X>Y)" would be such that op = '>', op_index = 3, start_index = 1, previous is the origin,
next1 is a node whose expression is "X", and next2 is a node whose expression is "Y". As another example, the
node where the expression is "Z" would be such that op = '\0', op_index = -1, start_index = 7,
previous is the origin, next1 = NULL and next2 = NULL. */

/* The list struct is a linked list meant to help construct the sub-formula tree. It works in the following way: Whenever
a sub-formula whose next2 is not set to NULL is encountered, we push next2 onto the list. Whenever we reach an atomic formula,
we go to the next formula on the linked list, and remove that formula from the list. For instance, if we encountered the sub-formula
"(X>Y)", next2 (which would end up containing the sub-formula "Y") would be pushed onto the list. As another example, if we
encountered an atomic formula "X", and the beginning of the linked list was node p, we would then move to node p. */
class list {
public:
        list() {}

        inline list* remove_beg(list* start); // pop and
        inline list* add_beg(list* start, formula* data); // push functions for the list class
	inline list* create_copy(); // to create a copy of a given list
	inline bool find_expression(char* expression); // to search for a given expression (in a formula) in the linked list.
						      // returns 1 if found, 0 if not found
	inline void print_list(); // function to print a list
	

        formula* f;
        list* next;
};

// Given a formula, creates its corresponding sub-formula tree
void tree(char* expression, formula* start, list* components);

/* This achieves the same result as tree, but in a different way. It is recursive, and does not require the use of 
the list class */
formula* alt_tree(char* expression, int index);

// Checks the syntax of the formula
int syntax(char* expression);

// To check if a given value of i corresponds to any interpretation for a given var_count
int i_check(int i, int vars);

// prints n tabs
void tabs(int n);

int syntax(char* expression) {
        if (expression == nullptr || strlen(expression)==0) {
                return 0;
        }
        if (strlen(expression) > MAX_LENGTH) {
                return 0;
        }
        if (!correct(expression)) {
                return 0;
        }
        return 1;
}

// note: this function should not be used. it is old
void tree(char* expression, formula* start, list* components) {
        if (!syntax(expression))
                return;
        int end_index1;
        formula* f = start;
        while (1) {
                if (f->expression[0] == '~') {
                        f->next2 = nullptr;
                        f->next1 = new formula;
                        f->next1->start_index = f->start_index+1;
                        f->next1->previous = f;
                        potential(expression, f->next1->start_index, f->next1->expression);
                        f->op = '~';
                        f->op_index = f->start_index;
                        //f->print();
                        f = f->next1;
                }
                else if (f->expression[0] == '(') {
                        //printf("CCC\n");
                        f->next1 = new formula;
                        f->next2 = new formula;
                        f->next1->start_index = f->start_index+1;
                        //printf("DDD\n");
                        potential(expression, f->next1->start_index, end_index1, f->next1->expression);
                        //printf("f: %s sub1: %s\n", f->expression, f->next1->expression);
                        f->op = expression[end_index1];
                        f->op_index = end_index1;
                        f->next2->start_index = end_index1+1;
                        f->next1->previous = f->next2->previous = f;
                        potential(expression, f->next2->start_index, f->next2->expression);
                        //printf("sub2: %s\n", f->next2->expression);
                        components = components->add_beg(components, f->next2);
                        //f->print();
                        f = f->next1;
                }
                else {
                        //printf("BBB\n");
                        f->op = '\0';
                        f->op_index = -1;
                        f->next1 = f->next2 = nullptr;
                        //f->print();
                        if (components == nullptr) {
                                break;
                        }
                        else {
                                f = components->f;
                                components = components->remove_beg(components);
                        }
                }
        }
}

formula* alt_tree(char* expression, int index) {
	if (!syntax(expression)) {
		printf("Invalid syntax.\n");
		exit(1);
	}
	if (index < 0 || index > strlen(expression)-1) {
		printf("Invalid index.\n");
		printf("index: %d\n", index);
		exit(1);
	}
	formula* f = new formula;
	potential(expression, index, f->expression);
	f->start_index = index;
	// this is the case for an atomic formula
	if (strlen(f->expression) == 1) {
		f->op = '\0';
		f->op_index = -1;
		f->next1 = f->next2 = nullptr;
		return f;
	}
	else if (expression[index] == '~') {
		f->next1 = alt_tree(expression, index+1);
		f->next2 = nullptr;
		f->next1->previous = f;
		f->op = '~';
		f->op_index = index;
		return f;
	}
	else {
		f->next1 = alt_tree(expression, index+1);
		f->op_index = index+strlen(f->next1->expression)+1;
		f->op = expression[f->op_index];
		f->next2 = alt_tree(expression, f->op_index+1);
		f->next1->previous = f->next2->previous = f;
		return f;
	}
}
	

inline list* list::remove_beg(list* start) {
        if (start == nullptr) {
                printf("Underflow error.\n");
                exit(1);
        }
        list* ptr;
        ptr = start->next;
        free(start);
        return ptr;
}

inline list* list::add_beg(list* start, formula* data) {
        list* new_node = (list*)malloc(sizeof(list));
        new_node->f = data;
        if (start == nullptr) {
                start = new_node;
                new_node->next = nullptr;
                return start;
        }
        new_node->next = start;
        start = new_node;
        return start;
}

inline int formula::depth() {
        int i;
        formula* g = this;
        if (g == nullptr)
                return 0;
        for (i = 0; g->previous != nullptr; i++) {
                g = g->previous;
        }
        return i;
}

inline void formula::print() {
        tabs(this->depth());
        printf("%s\n", this->expression);
}

void tabs(int n) {
        for (int i = 0; i < n; i++) {
                printf("\t");
        }
}

inline void formula::print_tree() {
	if (this->previous != nullptr)
		return;
	list* components = NULL;
	formula* g = this;
	while (1) {
		g->print();
		if (g->next1 == nullptr && g->next2 == nullptr) {
			if (components == NULL)
				break;
			g = components->f;
			components = components->remove_beg(components);
		}
		else if (g->next2 == nullptr && g->next1 != nullptr) {
			g = g->next1;
		}
		else if (g->next1 != nullptr && g->next2 != nullptr) {
			components = components->add_beg(components, g->next2);
			g = g->next1;
		}
		// otherwise (if g->next1 == nullptr && g->next2 != nullptr) we have an error
		else
			return;
	}
}

inline void formula::print_tree(int i) {
	if (this->previous != nullptr)
                return;
        list* components = nullptr;
        formula* g = this;
	if (!i_check(i, this->var_count))
		return;
        while (1) {
                g->print();
		tabs(g->depth());
		printf("%d\n", g->values[i]);
                if (g->next1 == nullptr && g->next2 == nullptr) {
                        if (components == nullptr)
                                break;
                        g = components->f;
                        components = components->remove_beg(components);
                }
                else if (g->next2 == nullptr && g->next1 != nullptr) {
                        g = g->next1;
                }
                else if (g->next1 != nullptr && g->next2 != nullptr) {
                        components = components->add_beg(components, g->next2);
                        g = g->next1;
                }
                // otherwise (if g->next1 == nullptr && g->next2 != nullptr) we have an error
                else
                        return;
        }
}			

int i_check(int i, int vars) {
	if (i < 0 || i > pow(2, vars)) {
		return 0;
        }
	return 1;
}	

inline int formula::op_find(int index, bool& value, int i) {
	list* components = nullptr;
	formula* g = this;
	if (!i_check(i, this->var_count)) {
		return 0;
	}
	while (1) {
		if (g->op_index == index) {
			value = g->values[i];
			return 1;
		}
		if (g->next1 == nullptr && g->next2 == nullptr) {
                        if (components == nullptr)
                                break;
                        g = components->f;
                        components = components->remove_beg(components);
                }
                else if (g->next2 == nullptr && g->next1 != nullptr) {
                        g = g->next1;
                }
                else if (g->next1 != nullptr && g->next2 != nullptr) {
                        components = components->add_beg(components, g->next2);
                        g = g->next1;
                }
                // otherwise (if g->next1 == nullptr && g->next2 != nullptr) we have an error
                else
                        return 0;
	}
	return 0;
}

inline int formula::prec_init(int precedence) {
	// if this is an atomic formula, we return the same precedence
	if (this->op_index == -1) {
		// we return precedence-1 here for the following reason: this case occurs if this contains 
		// an atomic formula. If we were to return precedence, consider the following example. Say we 
		// have the sub-formula "(X>~Y)", and are passed in a precedence of n. Then we would pass in 
		// n+1 to find the precedence of X, but it has no precedence, so we would return n+1, and then pass in 
		// n+2 to find the precedence of "~Y", which would yield a precedence of n+2, even though ~Y should
		// have a precedence of n+1. Returing precedence-1 solves this issue. 
		return precedence-1;
	}
	this->op_precedence = precedence;
	precedence = this->next1->prec_init(precedence+1);
	if (this->next2 == nullptr)
		return precedence;
	precedence = this->next2->prec_init(precedence+1);
	return precedence;
}

inline int formula::precedence_find(int index) {
	int ret_val = -1;
	if (this->op_index == index)
		return this->op_precedence;
	if (this->next1 != nullptr) {
		ret_val = this->next1->precedence_find(index);
		if (ret_val != -1 || this->next2 == nullptr)
			return ret_val;
		ret_val = this->next2->precedence_find(index);
		return ret_val;
	}
	// otherwise we have an atomic formula
	else {
		return -1;
	}
}

inline bool formula::sub_find(formula* sub) {
	if (this == sub)
		return true;
	if (this->next1 == nullptr)
		return false;
	if (this->next1->sub_find(sub))
		return true;
	if (this->next2 == nullptr)
		return false;
	return this->next2->sub_find(sub);
}

inline formula* formula::start_find(int start_index) {
	formula* ret_val = nullptr;
	if (this->start_index == start_index)
		return this;
	if (this->next1 == nullptr)
		return nullptr;
	ret_val = this->next1->start_find(start_index);
	if (ret_val != nullptr)
		return ret_val;
	if (this->next2 == nullptr)
		return nullptr;
	return this->next2->start_find(start_index);
}

inline list* list::create_copy() {
	list* copy = new list;
	list* l = this;
	while (l != nullptr) {
		copy = copy->add_beg(copy, l->f);
		l = l->next;
	}
	return copy;
}

inline bool list::find_expression(char* expression) {
	if (expression == nullptr)
		return false;
	list* l = this;
	while (l != nullptr) {
		if (l->f != nullptr && !strcmp(expression, l->f->expression))
			return true;
		l = l->next;
	}
	return false;
}

inline void list::print_list() {
	list* l = this;
	while (l != nullptr && l->f != nullptr) {
		printf("%s\n", l->f->expression);
		l = l->next;
	}
}
