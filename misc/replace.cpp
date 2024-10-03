#include "../truth_tables/truth_table.h"
// compares the variables of two formulas. If they have the same number of distinct variables, 0 is returned. If string1 has more 
// variables, 1 is returned. If string2 has more any variables that string1 does not contain, -1 is returned.
int var_comp(char string1[], char string2[]);

/* this functions checks if the expression stored in sub is a replaceable string (with respce to the remove string). If it 
is, it replaces it with the corresponding remove string in the main expression, and returns true. Otherwise returns false. */
bool replaceable(formula* main, formula* sub, char replace, char remove[]);

/* This functions replaces all regular expressions (must be syntactically correct formulas) that are derived from remove in main->expression 
with the corresponding replace string. The basic rule is that the only sub-formulas of main that will be replaced are those from the initial formula (these 
sub-formulas must be correctly stored in the original_subs linked list). off_limits acts as holding which of those sub-formnulas have already been replaced.

Here is an example of what would happen in the function. Say we are given a main formula with expression 
"(X^(X>Y))", a sub formula with expression "(X>Y)", a remove of "(X>Y)" (it does not need to contain 'X' and 'Y'; it could be, say, "(A>B)"), 
and a replace of "(~XvY)". "(X>Y") would be found on the original_subs linked list, and would not be found on the off_limits linked list. Thus, because 
it matches the regular expression pattern of "(X>Y)", "(X>Y)" would be replaced with "(~XvY)", resulting in main->expression being 
"(X^(~XvY))". Then "(X>Y)" would be added onto off_limits, and a recursive call would be made for "~X" (which would be next1->expression of 
the updated sub) and then another recursive call for "Y" (which would be next2->expression of the updated sub). 

This example would work in a similar way if main->expresison was "(A^(A>B))", sub->expression was "(A>B)", 
a remove of "(X>Y"), and a replace of "(~XvY)". In that case, main->expression would be changed to "(A^(~AvB))". */ 
void replace(formula* main, formula* sub, list* off_limits, list* original_subs, char replace[], char remove[]);

// this functions creates a linked list of all the sub-formulas of a given formula f
list* formula_list(formula* f, list* linked_list);

/* this function replaces all occurences of a form of remove with replace. For instance, if given a main->expression of 
"((X=Y)=(Y=X))", a replace of "((A>B)^(B>A))" and a remove of "(A=B)", main->expression would first be changed to 
"(((X=Y)>(Y=X))^((Y=X)>(X=Y)))" and then eventually to
"((((X>Y)^(Y>X))>((Y>X)^(X>Y)))^(((Y>X)^(X>Y))>((X>Y)^(Y>X))))" */
void full_replace(formula* main, formula* current, list* original_subs, char replace[], char remove[]);

int main() {
	char remove[MAX_LENGTH]; // the type of expression to be replaced
	char replace[MAX_LENGTH]; // the type of expression to replace remove with
	formula* f = new formula;
	printf("What is the formula?\n");
	scanf("%s", f->expression)
	f = alt_tree(f->expression, 0);
	printf("What will be replaced?\n");
	scanf("%s", remove);
	printf("What will replace it?\n");
	scanf("%s", replace);
	list* original_subs = formula_list(f);
	full_replace(f, original_subs, replace, remove);
	printf("The updated formula is:\n%s\n", f->expression);
}

void full_replace(formula* main, formula* current, list* original_subs, char replace[], char remove[]) {
	if (main == nullptr || current == nullptr)
		return;
	int start_index = current->start_index;
	replace(main, current, original_subs, replace, remove);
	// because the main formula may have changed, we need to find the potentially updated current
	current = main->start_find(start_index);
	if (current->next1 == nullptr)
		return;
	replace(main, current->next1, original_subs, replace, remove);
	current = main->start_find(start_index);
	if (current->next2 == nullptr)
		return;
	replace(main, current->next2, original_subs, replace, remove);
} 

list* formula_list(formula* f, linked_list) {
	// if we have made no recursive calls, we must ensure that linked_list starts out as nullptr (or else there might already be 
	// other things on the list, which we don't want
	if (f->previous == nullptr && linked_list != nullptr)
		return nullptr;
	linked_list = linked_list->add_beg(linked_list, f);
	if (f->next1 == nullptr)
		return linked_list;
	linked_list = formula_list(f->next1, linked_list);
	if (f->next2 = nullptr)
		return linked_list;
	return formula_list(f->next2, linked_list);
}

int var_comp(char string1[], char string2[]) {
	char variable variables1[MAX_VARIABLES]; // to store the variables in string1
	char variable variables2[MAX_VARIABLES]; // to store the variables in string2
	int i;
	bool same = true;
	variable_find(string1, variables1);
	variable_find(string2, variables2);
	for (i = 0; i < MAX_LENGTH && isupper(variables2[i]); i++) {
		// if the variable in the current index of varibles2 is not in variables1, we return -1
		if (variable_check(variables1, variables2[i]) == -1)
			return -1;
		if (variables1[i] != variables2[i])
			same = false;
	}
	// if we have not returned in the loop, we know that string2 does not have any variables that string1 does not 
	// contain. Now we see if they contain the same exact variables
	if (i == MAX_LENGTH || !isupper(variables1[i]))
		return same?0:1;
	// otherwise string1 has more variables
	return 1;
}
	

bool replaceable(formula* main, formula* sub, char replace[], char remove[]) {
	if (sub == nullptr || main == nullptr)
		return false;
	if (sub->start_index < 0 || sub->start_index >= strlen(main->expression))
		return false;
	// we ensure that the thing to replace with has no more variables than the thing to remove
	if (var_comp(replace, remove) > 0)
		return false;
	bool used[26]; // this stores whether or not a regular expression in remove has already been found
	char expressions[26][MAX_LENGTH];
	int start_index = sub->start_index;
	int end_index; // to be passed in to potential
	int count = 0; // to track the index we are at in the sub-formula
	char replace_with[MAX_LENGTH]; // if the sub formula can be replaced, what it will be replaced with will be stored here
	char temp_expression[MAX_LENGTH]; // to store the potential sub-formulas of the sub-formula
	for (int i = 0; i < strlen(remove); i++) {
		// if the character in remove is not standing for a regular expression (i.e., either a connective or parenthesis)
		// we ensure that this character is the same as that in the sub expression
		if (!isupper(remove[i]) {
			if (sub->expression[i] != remove[i])
				return false;
			count++;
		}
		// otherwise we have a regular expression
		// we first check if the regular expression has already been used
		if (used[remove[i]-65) {
			// if the potential formula of the sub-formula at the current index does not match the 
			// stored regular expression, we return false;
			potential(sub->expression, count, end_index, temp_expression); 
			if (strcmp(expression[remove[i]-65], temp_expression) != 0)
				return false;
			// othwerwise we concatenate the expression onto the thing we will replace with
			strcat(replace_with, temp_expression);
			count += strlen(temp_expression);
			continue;
		}
		// othwerise we add the current expression to the expressions array
		potential(sub->expression, count, end_index, expressions[remove[i]-65]);
		used[remove[i]-65] = true;
	}
	// if we haven't reached the end of the sub formula, we return false
	if (count != strlen(sub->expression))
		return false;
	// if the replaced string would be too long (or less than 0), we return false
	if ((strlen(main->expression)-strlen(sub->expression))+strlen(replace_with) < 0 ||
	     (strlen(main->expression)-strlen(sub->expression))+strlen(replace_with) > MAX_LENGTH)
		return false;
	// otherwise we need to replace the formula
	strcpy(temp_expression, &main->expression[start_index+strlen(sub->expression));
	strcpy(&main->expression[start_index], replace_with);
	strcpy(&main->expression[start_index+strlen(replace_with)], temp_expression);
	return true;
}
	

void replace(formula* main, formula* sub, list* off_limits, list* original_subs, char replace[], char remove[]) {
	int index = -1;
	int start_index = sub->start_index; // to store the start index of the given sub_formula
	list* alt_offlimits = new list;
	if (formula == nullptr || sub == nullptr)
		return;
	// if sub is not found to be a sub_formula of main formula or original formula, we return
	if (!main->sub_find(sub))
		return; // we should not be returning here
	// if this was in the original formula, and is not off limits, then we see if we can replace the current sub formula
	if (original_subs->find_expression(sub->expression) && !off_limits->find_exppression(sub->expression)) {
		ret_val = replacable(main, sub, replace, remove);
		if (ret_val > 0) {
			off_limits = add_beg(off_limits, sub);
			num_offlimits++;
			index = sub->start_index;
			main = altree(main->expression, 0);	
			sub = main->start_find(start_index);
		}
	}
	if (sub->next1 == nullptr)
		return;
	alt_offlimits = off_limits->copy();
	replace(main, sub->next1, alt_offlimits, original_subs, replace, remove);
	sub = main->start_find(start_index);
	if (sub->next2 == nullptr)
		return;
	replace(main, sub->next2, off_limits, original_subs, freplace, remove);
}
