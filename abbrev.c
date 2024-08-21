//*% $COM -O0 -o # %
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#define	INCR	8192	// how much tape to add when we reach the right end.
int nstep=-1;		// How many steps to run the machine. -1 means forever.
//
// An ordinary tuple consists of
//	config	the configuration the machine is in, currently
//	symbols	a string of symbols it might see at a location on the tape
//	operations	a string of operations to perform on seeing one
//			of the tuple's symbols while in the given config.
//	final		which config to change the machine into.
//
// Skeleton tables use a sort of macro-expansion to specify configurations.
// For example f(S,B,a) is a function yielding a configuration with
// arguments S, B and a.
// In general:
//	Config functions start with any letter and are followed by (
//		and a list of arguments, ending with ).
//	Arguments begining with a capital letter are variables.
//	Literal symbol sets are represented by strings with C syntax.
//	Arguments beginning with a lower case letter are literal configs,
//	that may or may not be followed by (...) where ... is a list
//	of literal configs and symbol sets.
//
// Turing's first example on page 236 might look like this in my notation.
// This is my notation, not Turing's, which involves single letter names
// in Fraktur, Greek and italic fonts, possibly with subscripts and primes.
//
//	config		symbols	behavior	final config
//
//	f(C,B,a)	"e"	"L"		f1(C,B,a)
//	f(C,B,a)	else	"L"		f(C,B,a)
//
//	f1(C,B,a)	a			C
//	f1(C,B,a)	" "	"R"		f2(C,B,a)
//	f1(C,B,a)	else	"R"		f1(C,B,a)
//
//	f2(C,B,a)	a			C
//	f2(C,B,a)	" "	"R"		B
//	f2(C,B,a)	else	"R"		f1(C,B,a)
//
// If the abbreviated table mentioned config f(s,t,"x") (or produced
// it as an evaluation of a function), this would add these rules:
//
//	f(s,t,"x")	"e"	"L"		f1(s,t,"x")
//	f(s,t,"x")	else	"L"		f(s,t,"x")
//
//	f1(s,t,"x")	"x"			s
//	f1(s,t,"x")	" "	"R"		f2(s,t,"x")
//	f1(s,t,"x")	else	"R"		f1(s,t,"x")
//
//	f2(s,t,"x")	"x"			s
//	f2(s,t,"x")	" "	"R"		t
//	f2(s,t,"x")	else	"R"		f1(s,t,"x")
//
// From the paper: Our interpretation rule then is this. We are given
// the names of the m-configurations of the machine, mostly expressed
// in terms of m-functions. We are also given skeleton tables. All we
// want is the complete table for the m-configurations of the machine.
// This is obtained by repeated substitution in the skeleton tables.
//
// Turing warns (bottom of page 236) about infinite expansion, but I
// think if you only solve6n1(); functions with literal arguments (guaranteed
// to produce literal results) you should never run into trouble unless
// you have a recursive definition. (The real problem is that m-functions
// have no facility for conditional evaluation, and thus no way to stop
// a runaway recursion.)
//
// Note that Turing uses phrases like "not e", "None", etc. to describe
// sets of symbols. We probably don't want to go that far in emulating
// Turing's notation, though it occasionally requires some rewriting
// of his definitions. (For example, his definition of f1 on page 236
// whose symbols are "a", "not a" and "None", have to be reordered
// to be "a", " " and "else" because we have no way of knowing which
// symbols are included in "not a".
//
// We use the ascii space character in a cell to
// indicate Turing's blank (initially) or erased cells.
//
// It might be better to do away with all the quotation marks (").
// Then this notation would look more like Turing's. 
//
// Turing overloads functions on their parameter count.
// To accomodate this we need to have a list of functions on each name,
// and an indicator of the parameter count. Then at each expansion,
// we need to pick the variant with the appropriate parameter count.
//
typedef struct Name Name;
typedef struct Config Config;
typedef struct Function Function;
typedef	struct Param Param;
typedef struct Unexpanded Unexpanded;
//
// Just like malloc, except prints a message and exits on error
//
void *emalloc(int n){
	void *v;

	v=malloc(n);
	if(v==0){
		fprintf(stderr, "Can't malloc\n");
		exit(1);
	}
	return v;
}
struct Name{
	char *name;
	//
	// At most one of fns and mconfig should be non-zero.
	// fns is a list of configuration functions,each with a
	// different parameter count.
	// configs is the first of a list of Configs describing
	// what to do when the machine is in this state.
	//
	int index;	// state number used by compiler, 0 if not yet assigned
	Function *fns;
	Config *config;	// If non-zero, has already been expanded.
	Name *next;
};
Name *newName(char *name, Function *fns, Config *config, int index, Name *next){
	Name *n=emalloc(sizeof(Name));
	n->name=name;
	n->index=index;
	n->fns=fns;
	n->config=config;
	n->next=next;
	return n;
}
//
// Naming is confused. An MConfig the name of a state.
// It has an associated list of Configs that determine what
// happens when the machine is in this MConfig and sees
// one of the given symbols.
//
struct Config{
	char *symbols;
	char *operations;
	Name *final;
	Config *next;
};
Config *newConfig(char *symbols, char *operations, Name *final, Config *next){
	Config *c=emalloc(sizeof(Config));
	c->symbols=symbols;
	c->operations=operations;
	c->final=final;
	c->next=next;
	return c;
}
struct Param{
	Name *name;
	char *value;
	Param *next;
};
Param *newParam(Name *name, char *value, Param *next){
	Param *p=emalloc(sizeof(Param));
	p->name=name;
	p->value=value;
	p->next=next;
	return p;
}
struct Function{
	Name *name;
	int nparams;
	Param *params;
	Config *configs;
	Function *next;
};
Function *newFunction(Name *name, int nparams, Param *params, Config *configs, Function *next){
	Function *f=emalloc(sizeof(Function));
	f->name=name;
	f->nparams=nparams;
	f->params=params;
	f->configs=configs;
	f->next=next;
	return f;
}
struct Unexpanded{
	Name *name;
	Unexpanded *next;
};
Unexpanded *newUnexpanded(Name *name, Unexpanded *next){
	Unexpanded *u=emalloc(sizeof(Unexpanded));
	u->name=name;
	u->next=next;
	return u;
}
Name *start=0;			// the state in which the machine starts
Name *namelist=0;		// list of all names
Unexpanded *unexpanded=0;	// list of unexpanded calls
Unexpanded *eunexpanded=0;	// end of the unexpanded list, or 0 if none.
int nstate;			// # of state names so far
//
// Used by input
//
char line[8192];	// the line currently being parsed
int lineno;		// line number of the current line
char token[8192];	// token most recently read
char *inp;		// next character to be scanned on the line
int nerror;		// total number of error messages
char *infile;		// name of the input file
int parsing;		// true during input
char *allsymbols=0;	// every symbol that might appear. Used in getconfig
//
// Print an error message. Use it like printf.
// Msg should not have any newlines!
//
void error(char *msg, ...){
	va_list args;

	//
	// First print the message
	//
	fprintf(stderr, "Error: ");
	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
	//
	// If we're not parsing, we've lost file/line information
	// and should suppress printing them. We could store
	// line/file information with functions & m-configs
	// to allow non-parsing messages to suggest input locations.
	//
	if(parsing){
		fprintf(stderr, " on line %d of %s\n", lineno, infile);
		//
		// Then print the line and indicate the position at
		// which the message
		// was issued.
		//
		fprintf(stderr, "%s\n", line);
		for(char *s=line;s!=inp && *s!='\n' && *s!='\0';s++)
			putc(*s=='\t'?'\t':' ', stderr);
		fprintf(stderr, "^\n");
	}
	else
		fprintf(stderr, "\n");
	nerror++;
}
//
// Find the name in the symbol table.
//
Name *lookup(char *s){
	Name *n;
	for(n=namelist;n;n=n->next) if(strcmp(n->name, s)==0) return n;
	//
	// Not found, need to create a new name and add it to the namelist
	//
	n=newName(strdup(s), 0, 0, 0, namelist);
	namelist=n;
	return n;
}
int alpha(int c){
	return 'a'<=c && c<='z' || 'A'<=c && c<='Z';
}
int idchar(int c){
	return alpha(c) || '0'<=c && c<='9' || c=='\'';
}
int iswhite(int c){
	return c==' ' || c=='\t' || c=='\n';
}
void skipwhite(void){
	while(iswhite(*inp)) inp++;
}
void skipid(void){
	while(idchar(*inp)) inp++;
}
//
// Save the token starting at start and ending at inp,
// squishing out all unescaped white space.
//
void savetoken(char *start){
	char *tokp=token;
	for(char *s=start;s!=inp;s++){
		if(*s=='\\' && s+1!=inp){
			s++;
			*tokp++=*s;
		}
		else if(!iswhite(*s)) *tokp++=*s;
	}
	*tokp='\0';
}
void copyid(char *dst, char *src, char *esrc){
	int len=esrc-src;
	memcpy(dst, src, len);
	dst[len]='\0';
}
//
// get the next entry from the line.
// An entry is a string of non-white characters (a backslash quotes any
// character, including white space) with balanced parentheses (which may
// enclose white space, which is ignored). A right paren
// unmatched by a left paren is a syntax error.
//
int gettoken(void){
	skipwhite();
	if(*inp=='\0') return 0;
	char *start=inp;
	int level=0;
	while(level>0 || !iswhite(*inp)){
		if(*inp=='(') level++;
		if(*inp==')'){
			if(level==0){
				error("Unbalanced parentheses");
				inp++;		// skip over the bad paren.
				return 0;
			}
			--level;
		}
		if(*inp=='\\'){
			inp++;
			if(*inp=='\0'){
				error("backslash at end of line");
				return 0;
			}
		}
		inp++;
	}
	char *tokp=token;
	for(char *s=start;s!=inp;s++){
		if(!iswhite(*s)){
			if(*s=='\\') s++;
			*tokp++=*s;
		}
	}
	*tokp='\0';
	return 1;	
}
//
// Check what kind of token we have.
// These overlap, so there are separate routines checking whether the token
// could be each type. They will be called as appropriate.
//
int iskw(char *kw){
	return strcmp(token, kw)==0;
}
int isname(void){
	char *s=token;
	if(*s=='$') s++;	// not sure this is ok.
	if(!alpha(*s)) return 0;
	while(idchar(*s)) s++;
	return *s=='\0';
}
//
// A header is a name followed by a parenthesis-enclosed, comma-separated
// list of names.
//
int isheader(void){
	char *s=token;
	if(!alpha(*s)) return 0;
	while(idchar(*s)) s++;
	if(*s!='(') return 0;
	s++;
	for(;;){
		if(!alpha(*s)) return 0;
		while(idchar(*s)) s++;
		if(*s==')') return s[1]=='\0';
		if(*s!=',') return 0;
		s++;
	}
}
//
// A call is a name followed by a string of balanced parentheses.
// Balance is guaranteed by gettoken(), so we just check that the
// last character is a right paren.
//
int iscall(void){
	char *s=token;
	if(*s=='$') s++;	// not sure this is ok
	if(!alpha(*s)) return 0;
	while(idchar(*s)) s++;
	if(*s!='(') return 0;
	while(s[1]!='\0') s++;
	return *s==')';
}
//
// Return a copy of src with c substituted for every '*'.
//
char *starsubst(char *src, int c){
	char *dst=strdup(src);
	for(char *dp=dst;*dp!='\0';dp++) if(*dp=='*') *dp=c;
	return dst;
}
Config *getconfig(int expandcall){
	if(!gettoken()) return 0;
	char *symbols=strdup(token);
	if(!gettoken()){
		error("config must specify operations");
		return 0;
	}
	char *operations=strdup(token);	// should check syntax
	if(!gettoken()){
		error("config must specify final config");
		return 0;
	}
	int call=iscall();
	if(!call && !isname()){
		error("final config must be a name or a call");
		return 0;
	}
	Name *final=lookup(token);
	if(gettoken())
		error("junk past the end of a config");
	//
	// If symbols is * and final also contains a *,
	// we make a list of configs,
	// one for each of the machine's symbols.
	// (In the case where final contains no *, the interpreter takes a *
	// in symbols to mean any non-blank and in operations to mean the
	// scanned symbol.  The only reason to do it this way is to avoid
	// proliferating states when the situation can be handled easily
	// in the interpreter.)
	//
	if(strcmp(symbols, "*")==0 && strchr(final->name, '*')==0){
		Config *clist=0;
		Config *eclist=0;
		if(allsymbols==0){
			error("no symbols declaration");
			return 0;
		}
		for(char *sym=allsymbols;*sym!='\0';sym++){
			char *finalstring=starsubst(final->name, *sym);
			Name *substfinal=lookup(finalstring);
			free(finalstring);
			Config *c=newConfig(starsubst(symbols, *sym),
					    starsubst(operations, *sym),
					    substfinal,
					    0);
			if(eclist!=0) eclist->next=c;
			else clist=c;
			eclist=c;
			if(expandcall && call){
				Unexpanded *u=newUnexpanded(substfinal, 0);
				if(eunexpanded==0) unexpanded=u;
				else eunexpanded->next=u;
				eunexpanded=u;
			}
		}
		free(symbols);
		free(operations);
		return clist;
	}
	Config *c=newConfig(symbols, operations, final, 0);
	if(expandcall && call){
		Unexpanded *u=newUnexpanded(final, 0);
		if(eunexpanded==0) unexpanded=u;
		else eunexpanded->next=u;
		eunexpanded=u;
	}
	return c;
}
//
// Crack a Function header
//
Function *getfunction(char *token){
	Name *name;
	char *s=token;
	while(idchar(*s)) s++;
	char id[8192];
	copyid(id, token, s);
	name=lookup(id);
	//
	// This check doesn't actually matter.
	// We could delete this check if we need
	// to overload a function name as an
	// m-config name. I hope that doesn't
	// come up in the paper.
	//
	if(name->config){
		error("Name %s already in use", name->name);
		return 0;
	}

	if(*s!='('){
		error("Can't happen: bad function header %s", token);
		exit(1);
	}
	s++;
	Param *params=0;
	Param *eparams=0;
	int nparams=0;
	for(;;){
		char *t=s;
		while(idchar(*t)) t++;
		copyid(id, s, t);
		Param *p=newParam(lookup(id), 0, 0);
		nparams++;
		if(eparams) eparams->next=p;
		else params=p;
		eparams=p;
		s=t;
		if(*s!=',') break;
		s++;
	}
	if(*s!=')' || s[1]!='\0'){
		error("Can't happen: bad function header %s", token);
		exit(1);
	}
	for(Function *f=name->fns;f!=0;f=f->next){
		if(f->nparams==nparams){
			error("Function %s with %d params already defined", name->name, nparams);
			Param *next;
			for(Param *p=params;p!=0;p=next){
				next=p->next;
				free(p);
			}
			return 0;
		}
	}
	Function *fn=newFunction(name, nparams, params, 0, name->fns);
	name->fns=fn;
	return fn;
}
//
// Return the last in the argument list of configs or 0 if the argument is 0.
//
Config *last(Config *c){
	if(c==0) return 0;
	while(c->next!=0) c=c->next;
	return c;
}
//
// Read a skeleton table file.
//
void input(char *file){
	infile=file;
	FILE *in=fopen(file, "r");
	if(in==0){
		perror(file);
		exit(1);
	}
	Name noname;
	noname.name="*gok*";
	noname.fns=0;
	noname.config=0;
	noname.next=0;
	Function *function=0;
	Function nofunction;
	nofunction.name=&noname;
	nofunction.nparams=0;
	nofunction.configs=0;
	nofunction.next=0;
	Name *statename=0;
	Config *econfig=0;
	for(lineno=1;fgets(line, sizeof line, in)!=0;lineno++){
		inp=line;
		char *comment=strchr(line, '#');
		if(comment) *comment='\0';

		if(iswhite(*inp)){	// continuation of statename/function def
			//
			// Two legal possibilities at this point:
			//	a blank line, which we should ignore
			//	a config for an statename or a function.
			skipwhite();
			if(*inp=='\0') continue;	// nothing on line
			if(function==0 && statename==0)
				error("orphan continuation line");
			else{
				Config *c=getconfig(statename!=0);
				if(c!=0){
					if(econfig) econfig->next=c;
					else if(function) function->configs=c;
					else statename->config=c;
					econfig=last(c);
				}
			}
		}
		else{
			//
			// The line is unindented, so it must be
			// an include, a def or an statename (which starts with
			// and unadorned name).
			//
			if(!gettoken()) continue;
			if(iskw("include")){
				skipwhite();
				if(*inp!='"'){
					error("bad include");
					continue;
				}
				inp++;
				char *includename=inp;
				while(*inp!='"' && *inp!='\0'){
					if(*inp=='\\' && inp[1]!='\0') inp++;
					inp++;
				}
				if(*inp!='"'){
					error("unterminated include file name");
					return;
				}
				savetoken(includename);
				//
				// Recursive call.
				// Save important globals first,
				// do the call and restore them after.
				// (We're done with line, token and inp at this
				// point, so we don't need to save and restore
				// them.) This would all be mildly easier in
				// C++.
				//
				char *savefile=file;
				int savelineno=lineno;
				input(token);
				file=savefile;
				lineno=savelineno;
				continue;
			}
			if(iskw("def")){
				if(!gettoken())
					error("function def must name "
						"a function");
				else if(!isheader())
					error("function def must have a name "
						"and parameters (saw %s)",
						token);
				else{
					statename=0;
					function=getfunction(token);
					//
					// on error, we want to continue
					// reading the definition and stash
					// it someplace innocuous
					//
					if(function==0)
						function=&nofunction;
					// works even if getconfig returns 0
					function->configs=getconfig(0);
					econfig=last(function->configs);
				}
			}
			else if(iskw("symbols")){
				if(!gettoken())
					error("symbols line must a "
						"symbol list");
				else if(allsymbols!=0)
					error("can only specify symbols once");
				else{
					allsymbols=strdup(token);
				}
			}
			else if(isname()){
				Name *configname=lookup(token);
				nstate++;
				configname->index=nstate;
				function=0;
				statename=lookup(token);
				if(statename->fns!=0 || statename->config!=0){
					error("name %s already in use", statename->name);
					continue;
				}
				// works even if getconfig returns 0
				statename->config=getconfig(1);
				econfig=last(statename->config);
				if(start==0) start=configname;
			}
			else if(token[0]!='\0')
				error("unrecognized line, token '%s'", token);
		}
	}
	fclose(in);
}
char *findparam(char *name, Param *param){
	for(Param *p=param;p!=0;p=p->next)
		if(strcmp(name, p->name->name)==0) return p->value;
	return 0;
}
//
// Copy src to dst. Whenever we see a param in the copy,
// substitute in its value. This has a danger of overflowing
// the dst buffer if substituted values are too long!
// Should check...
//
void substitute(char *dst, char *src, Param *param){
	char *srcp=src;
	char *dstp=dst;
	char *namep=0;
	for(;;){
		if(*srcp=='$'){
			//
			// copy the name into dst
			//
			namep=dstp;
			srcp++;
			if(!alpha(*srcp)){
				error("$ must be followed by an identifier "
					"in %s", src);
				continue;
			}
			while(idchar(*srcp)) *dstp++=*srcp++;
			if(*srcp=='.') srcp++;
			//
			// look for a substitution
			//
			char namebuf[8192];
			int nname=dstp-namep;
			memcpy(namebuf, namep, nname);
			namebuf[nname]='\0';
			char *subst=findparam(namebuf, param);
			if(subst!=0){
				//
				// Overwrite the name in dst with its expansion.
				//
				dstp=namep;
				while(*subst) *dstp++=*subst++;
			}
		}
		else if(*srcp=='\0') break;
		else *dstp++=*srcp++;
	}
	*dstp='\0';
}
//
// Expand a Config
// 
Config *expandconfig(Config *fc, Param *param){
	char buf[8192];		// results of substitution will go here
	char *symbols;
	//
	// Symbols come from substituting into fc's symbols.
	//
	substitute(buf, fc->symbols, param);
	symbols=strdup(buf);
	char *operations;
	substitute(buf, fc->operations, param);
	operations=strdup(buf);
	//
	// Expand final.
	// First make a copy of fc->final->name with any
	// parameters substituted (that's the job of substitute).
	//
	Name *final;
	substitute(buf, fc->final->name, param);
	final=lookup(buf);
	//
	// If final is a call that has yet to be expanded,
	// put it on the unexpanded list for later processing.
	//
	if(strchr(buf, '(')!=0 && final->config==0){
		Unexpanded *u=newUnexpanded(final, 0);
		if(eunexpanded)
			eunexpanded->next=u;
		else
			unexpanded=u;	// I don't think this can happen!
		eunexpanded=u;
	}
	//
	// All done. Allocate a Config, fill in it's fields
	// and return it.
	//
	return newConfig(symbols, operations, final, 0);
}
int argcount(char *call){
	char *c=call;
	while(idchar(*c)) c++;
	if(*c!='('){
		error("Can't happen: non-call %s in argcount", call);
		exit(1);
	}
	c++;
	int argc=1;
	int level=1;
	while(level!=0){
		if(*c=='\0') goto Error;
		if(level==1 && *c==',') argc++;
		if(*c=='(') level++;
		if(*c==')') --level;
		c++;
	}
	return argc;
Error:
	error("Can't happen: ungrammatical call %s in argcount", call);
	exit(1);
	return 0;	// not reached
}
//
// Process a call to an m-function.
// All this parsing and reparsing of CALLs makes me think
// we should do it once and for all (during input) and
// attach some sort of parse tree to the CALL in the
// symbol table.
//
void expand(Name *name){
	if(name->config!=0) return;	// already expanded.
	nstate++;
	name->index=nstate;
	char call[8192];
	//
	// We copy the call into a local buffer, because we
	// want to divide it into parts by replacing some punctuation
	// with '\0' and it would be bad to deface the original.
	//
	strcpy(call, name->name);
	//
	// Find the end of the m-function name in the call
	//
	char *callp=call;
	while(idchar(*callp)) callp++;
	if(*callp!='('){
		error("Can't happen: expand sees non-call %s", name->name);
		exit(1);
	}
	*callp++='\0';
	//
	// Get the function definition
	//
	Name *fname=lookup(call);
	int nargs=argcount(name->name);
	Function *fn;
	for(fn=fname->fns;fn!=0;fn=fn->next)
		if(fn->nparams==nargs)
			break;
	if(fn==0){
		error("Undefined function %s (with %d paramss) in call %s",
			name, nargs, call);
		return;
	}
	//
	// crack arguments starting at callp and set each
	// Param::value to the corresponding argument.
	//
	Param *paramp=fn->params;
	int done=0;
	while(!done){
		//
		// At this point we're pointing at the next argument
		// to crack.
		//
		if(paramp==0){	// all parameters set already
			error("too many arguments in call %s",
				name->name);
			return;
		}
		//
		// Scan down to the next comma or right paren that's
		// not nested inside parentheses or inside a string.
		//
		char *value=callp;
		int level=0;
		while(*callp!=',' && *callp!=')' || level>0){
			if(*callp=='\0'){
				error("Can't happen: premature end of call %s",
					name->name);
				exit(1);
			}
			if(*callp=='(') level++;
			if(*callp==')') --level;
			callp++;
		}
		if(*callp==')') done=1;
		//
		// Finally, we get to set paramp->value to the scanned value.
		//
		*callp='\0';
		callp++;
		paramp->value=value;
		paramp=paramp->next;
	}
	if(paramp!=0){
		error("not enough arguments in call %s", name->name);
		return;
	}
	//
	// The call was already parsed enough in input(.) to guarantee
	// that it's well-formed
	//
	if(*callp!='\0'){
		error("Can't happen: junk off end of call %s", name->name);
		exit(1);
	}
	//
	// do the expansion for each Config in fc->fn
	//
	name->config=0;
	Config *econfig=0;
	for(Config *fc=fn->configs;fc;fc=fc->next){
		Config *c=expandconfig(fc, fn->params);
		if(econfig==0)
			name->config=c;
		else
			econfig->next=c;
		econfig=c;
	}
}
//
// Go through the list of unexpanded calls and expand them, one by one.
// Note that as we remove calls from the list, new calls may be added (to
// the end) by expansion. In pathological cases, this could cause expansion
// not to terminate. In the absence of a way to detect expansion loops,
// maybe we should have an ad hoc limit (settable!) on how many expansions
// we're willing to do.
//
void expandall(void){
	while(unexpanded!=0){
		expand(unexpanded->name);
		Unexpanded *next=unexpanded->next;
		free(unexpanded);
		unexpanded=next;
	}
	eunexpanded=0;
}
void showstring(FILE *f, char *str){
	for(char *s=str;*s!='\0';s++){
		if(iswhite(*s)) fprintf(f, "\\");
		putc(*s, f);
	}
}
void showconfigs(FILE *f, Config *config){
	for(Config *c=config;c!=0;c=c->next){
		fprintf(f, "\t");
		showstring(f, c->symbols);
		fprintf(f, "\t");
		showstring(f, c->operations);
		fprintf(f, "\t");
		showstring(f, c->final->name);
		fprintf(f, "\n");
	}
}
void show(FILE *f){
	if(start)
		fprintf(f, "# start %s (state # %d)\n", start->name, start->index);
	else
		fprintf(f, "# No start symbol!\n");
	//
	// Print all the defs first, then all the MConfigs
	//
	for(Name *n=namelist;n!=0;n=n->next){
		if(n->fns!=0){
			for(Function *fn=n->fns;fn!=0;fn=fn->next){
				fprintf(f, "def ");
				showstring(f, n->name);
				int sep='(';
				for(Param *p=fn->params;p!=0;p=p->next){
					fprintf(f, "%c", sep);
					showstring(f, p->name->name);
					sep=',';
				}
				fprintf(f, ")\n");
				showconfigs(f, fn->configs);
			}
		}
	}
	printf("----------\n");
	//
	// Should sort these in index order
	//
	for(Name *n=namelist;n!=0;n=n->next){
		if(n->config!=0){
			printf("# state %d\n", n->index);
			showstring(f, n->name);
			showconfigs(f, n->config);
		}
	}
}
void checkchar(int c){
	if(c!= '_' && c!='~' && c!='*' && strchr(allsymbols, c)==0)
		error("symbol '%c' not in symbols \"%s\"", c, allsymbols);
}
//
// Verify that allsymbols includes all the symbols of the machine.
//
void checkallsymbols(void){
	//
	// It's not actually an error for there to by no symbols
	// declaration. It's only wrong to 
	if(allsymbols==0) return;
	for(Name *name=namelist;name!=0;name=name->next) if(name->config){
		for(Config *c=name->config;c!=0;c=c->next){
			for(char *s=c->symbols;*s!='\0';s++)
				checkchar(*s);
			for(char *s=c->operations;*s!='\0';s++){
				if(*s=='P'){
					//
					// Someone should report an error if P
					// is not followed by a character.
					// That should happen in 
					// Let execute do it for now.
					//
					s++;
					if(*s=='\0') break;
					checkchar(*s);
				}
			}
		}
	}
}
void execute(char *initfile){
	int ntape=INCR;
	char *tape=emalloc(ntape*sizeof(char));
	memset(tape, '_', ntape*sizeof(char));
	Name *state=start;
	if(state==0){
		error("no start state!");
		return;
	}
	int loc=0;
	int hwm=0;
	//
	// print the initial tape, loc and state
	//
	printf("%.*s\n", hwm, tape);
	for(int i=0;i!=loc;i++) putchar(' ');
	printf("^%s\n", state->name);
	for(int step=0;step!=nstep;step++){
		if(state->config==0){
			error("Undefined m-config %s", state->name);
			return;
		}
		Config *config;
		int scannedloc=tape[loc];
		for(config=state->config;config!=0;config=config->next){
			char *sym=config->symbols;
			if(*sym=='~'){
				sym++;
				int found=0;
				while(*sym){
					if(*sym==scannedloc) found=1;
					sym++;
				}
				if(!found) goto FoundSymbol;
			}
			else{
				for(;*sym!='\0';sym++)
					if(*sym=='*' && scannedloc!='_'
					|| *sym==scannedloc)
						goto FoundSymbol;
			}
		}
		//
		// If we fall out of the loop, no config matched.
		//
		error("Config for symbol %c not found in state %s",
			scannedloc, state->name);
		return;
	FoundSymbol:
		//
		// At this point, config has a match.
		//
		// Execute the operations, enter the new state, print the tape.
		// Right now this ignores operations it doesn't understand.
		// We need to have some symbol indicating no operations to
		// do, because input won't let the string be empty.
		//
		for(char *o=config->operations;*o!='\0';o++) switch(*o){
		default:	break;
		case 'N': break;	// no operation
		case 'H':	// Halt. Not one of Turing's actions.
			return;
		case 'P':
			o++;
			if(*o=='\0'){
				error("malformed operations %s in state %s",
					config->operations, state->name);
				return;
			}
			tape[loc]=*o=='*'?scannedloc:*o;
			break;
		case 'E':
			tape[loc]='_';
			break;
		case 'L':
			if(loc==0){
				error("Moved left from the left end of"
					" the tape in state %s operations %s",
					state->name, config->operations);
				return;
			}
			--loc;
			break;
		case 'R':
			if(loc==ntape-1){
				tape=realloc(tape, (ntape+INCR)*sizeof(char));
				if(tape==0){
					error("Can't grow tape to %d cells\n",
						ntape+INCR);
					exit(1);
				}
				memset(tape+ntape, '_', INCR);
				ntape+=INCR;
			}
			loc++;
			if(loc>hwm) hwm=loc;
			break;
		}
		state=config->final;
		//
		// Print the tape and indicate the loc and the new state
		//
		printf("%.*s\n", hwm, tape);
		for(int i=0;i!=loc;i++) putchar(' ');
		printf("^%s\n", state->name);
	}
	printf("Stopped after %d steps\n", nstep);
}
void compile(void){
}
int main(int argc, char *argv[]){
	char *tapefile=0;	// initial contents of the tape, if nonzero
	int docompile=0;	// compile the TM into input for the Universal machine
	int i=1;
	while(i<argc && argv[i][0]=='-'){
		switch(argv[i][1]){
		default:
		Usage:
			fprintf(stderr,
				"Usage: %s [-s nstep] [-c] [-r tape] "
				"machine-file\n",
				argv[0]);
			return 1;
		case 's':
			if(argv[i][2]!='\0'){
				nstep=atoi(argv[i]+2);
				break;
			}
			if(i+1>=argc) goto Usage;
			nstep=atoi(argv[i+1]);
			i++;
			break;
		case 'c':
			docompile=1;
			break;
		case 'r':
			if(argv[i][2]!='\0'){
				tapefile=argv[i]+2;
				break;
			}
			if(i+1>=argc) goto Usage;
			tapefile=argv[i+1];
			i++;
			break;
		}
		i++;
	}
	if(i!=argc-1) goto Usage;
	parsing=1;
	input(argv[argc-1]);
	parsing=0;
	if(nerror!=0) return 1;
	expandall();
	if(nerror!=0) return 1;
	checkallsymbols();
	if(nerror!=0) return 1;
	if(docompile) compile();
	show(stdout);
	printf("=======================\n");
	execute(tapefile);
	return nerror!=0;
}
