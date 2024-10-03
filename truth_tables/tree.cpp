/* Program to create a sub-formula tree of a given propositional formula. For instance, when given the 
formula "((X>Y)^Z)", the program would output
((X>Y)^Z)
	(X>Y)
		X
		Y
	Z
or if given "~(Xv((Y=Z)^V))" the program would output
~(Xv((Y=Z)^V))
	(Xv((Y=Z)^V))
		X
		((Y=Z)^V)
			(Y=Z)
				Y
				Z
			V
*/

#include "syntax.h"
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

	formula* f;
	list* next;
};

// Given a formula, creates its corresponding sub-formula tree
void tree(char* expression, formula* start, list* components);

// Checks the syntax of the formula
int syntax(char* expression);

// prints n tabs
void tabs(int n);

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Exactly one formula must be supplied.\n");
		exit(1);
	}
	if (!syntax(argv[1])) {
		printf("Invalid syntax.\n");
		exit(1);
	}
	formula* f = new formula(argv[1]);
	list* components = nullptr;
	tree(argv[1], f, components);
}

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

void tree(char* expression, formula* start, list* components) {
	if (!syntax(expression))
		return;
	int current_index = 0;
	int end_index1, end_index2;
	formula* f = start;
	while (1) {
		if (f->expression[0] == '~') {
			f->next2 = nullptr;
			f->next1 = new formula;
			f->next1->start_index = f->start_index+1;
			f->next1->previous = f;
			potential(expression, f->next1->start_index, end_index1, f->next1->expression);
			f->op = '~';
			f->op_index = f->start_index;
			f->print();
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
			potential(expression, f->next2->start_index, end_index2, f->next2->expression);
			//printf("sub2: %s\n", f->next2->expression);
			components = components->add_beg(components, f->next2);
			f->print();
			f = f->next1;
		}
		else {
			//printf("BBB\n");
			f->op = '\0';
			f->op_index = -1;
			f->next1 = f->next2 = nullptr;
			f->print();
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
