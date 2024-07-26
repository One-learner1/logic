//*% $COM -o # %
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
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
// Turing warns (bottom of pate 236) about infinite expansion, but I
// think if you only expand functions with literal arguments (guaranteed
// to produce literal results) you should never run into trouble unless
// you have a recursive definition. (The real problem is that m-functions
// have no facility for conditional evaluation, and thus no way to stop
// a runaway recursion.
//
typedef struct Name Name;
typedef struct MConfig MConfig;
typedef struct Config Config;
//
// Having two things names Function and FnConfig is stupid.
//
typedef struct Function Function;
typedef	struct Param Param;
typedef struct FnConfig FnConfig;
struct Name{
	char *name;
	//
	// Only one of fn and config should be non-zero
	// fn is a configuration function.
	// config is the first of a list of MConfigs describing
	// what to do when the machine is in this configuration.
	//
	Function *fn;
	MConfig *config;
	int expand;		// if the name is a function call that
				// has yet to be expanded
	Name *next;
};
struct Config{
	char *symbols;
	char *operations;
	Name *final;
	Config *next;
};
struct MConfig{
	Name *name;
	Config *config;
	MConfig *next;
};
struct FnConfig{
	//
	// Exactly one of symname and symbols should be non-null
	//
	Name *symname;
	char *symbols;
	char *operations;
	Name *final;		// might be a call
	FnConfig *next;
};
struct Param{
	Name *name;
	char *value;
	Param *next;
};
struct Function{
	Name *name;
	Param *params;
	FnConfig *configs;
};
MConfig *mconfig;	// the machine's list of  m-configs
Name *namelist;		// list of all names
Name *enamelist;	// points to the last name on namelist or 0 if none.
char line[8192];
char token[8192];
Name *def=0;
char *inp;
int lineno;
int nerror;
char *infile;
//
// Print an error message. It might be nice if this accepted
// printf-like arguments.
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
	fprintf(stderr, " on line %d of %s\n", lineno, infile);
	//
	// Then print the line.
	//
	for(char *s=line;*s!='\n' && *s!='\0';s++) putc(*s, stderr);
	putc('\n', stderr);
	//
	// Then indicate the position at which the message
	// was issued.
	//
	for(char *s=line;s!=inp && *s!='\n' && *s!='\0';s++)
		putc(*s=='\t'?'\t':' ', stderr);
	fprintf(stderr, "^\n");
	nerror++;
}
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
MConfig *newMConfig(Name *name, Config *config, MConfig *next){
	MConfig *m=emalloc(sizeof(MConfig));
	m->name=name;
	m->config=config;
	m->next=next;
	return m;
}
Config *newConfig(char *symbols, char *operations, Name *final, Config *next){
	Config *c=emalloc(sizeof(Config));
	c->symbols=symbols;
	c->operations=operations;
	c->final=final;
	c->next=next;
	return c;
}
Param *newParam(Name *name, char *value, Param *next){
	Param *p=emalloc(sizeof(Param));
	p->name=name;
	p->value=value;
	p->next=next;
	return p;
}
FnConfig *newFnConfig(Name *symname, char *symbols, char *operations, Name *final, FnConfig *next){
	FnConfig *f=emalloc(sizeof(FnConfig));
	f->symname=symname;
	f->symbols=symbols;
	f->operations=operations;
	f->final=final;
	f->next=next;
	return f;
}
Function *newFunction(Name *name, Param *params, FnConfig *configs){
	Function *f=emalloc(sizeof(Function));
	f->name=name;
	f->params=params;
	f->configs=configs;
	return f;
}
Name *newName(char *name, Name *next){
	Name *n=emalloc(sizeof(Name));
	n->name=name;
	n->next=next;
	return n;
}
//
// Find the name in the symbol table
//
Name *lookup(char *s){
	Name *n;
	for(n=namelist;n;n=n->next) if(strcmp(n->name, s)==0) return n;
	//
	// Not found, need to create a new name and add it to the namelist
	//
	n=newName(strdup(s), 0);
	if(enamelist!=0) enamelist->next=n;
	else namelist=n;
	enamelist=n;
	return n;
}
int alpha(int c){
	return 'a'<=c && c<='z' || 'A'<=c && c<='Z';
}
int idchar(int c){
	return alpha(c) || '0'<=c && c<='9' || c=='\'';
}
void skipwhite(void){
	while(*inp==' ' || *inp=='\t' || *inp=='\n') inp++;
	//
	// Comments run from # to end of line
	//
	if(*inp=='#') while(*inp!='\0') inp++;
}
void skipid(void){
	while(idchar(*inp)) inp++;
}
//
// Save the token starting at start and ending at inp,
// squishing out all white space
//
void savetoken(char *start){
	char *tokp=token;
	for(char *s=start;s!=inp;s++){
		if(*s!=' ' && *s!='\t' && *s!='\n') *tokp++=*s;
	}
	*tokp='\0';
}
//
// get the next (top-level) token from the line.
// Token types are:
//	FNDEF	the word def followed by name(params)
//	NAME	just a naked name
//	CALL	name(args). An arg can be a name or a string or a nested call
//	STRING	a quoted string -- either an input list or an operation list
//	EOL	end of line
//	ERROR	something else
// This doesn't try too hard to understand the internals of CALLs.
// Someone else will be responsible for that.
//
//
// Caller is expected to save inp
enum{
	FNDEF, NAME, CALL, STRING, EOL, ERROR
};
int gettoken(void){
	token[0]='\0';
	skipwhite();
	char *tokp=inp;
	if(alpha(*inp)){
		skipid();
		//
		// If we see "def", it must be followed by
		// a function header
		//
		if(inp-tokp==3 && strncmp(tokp, "def", 3)==0){
			skipwhite();
			tokp=inp;
			skipid();
			skipwhite();
			if(*inp!='(') return ERROR;
			inp++;
			for(;;){
				skipwhite();
				if(!alpha(*inp)) return ERROR;
				skipid();
				skipwhite();
				if(*inp==')'){
					inp++;
					savetoken(tokp);
					return FNDEF;
				}
				if(*inp!=',') return ERROR;
				inp++;
			}
		}
		//
		// If we see "else", pretend that we saw the
		// empty string, which we use in the symbol field
		// to mean any character at all.
		//
		if(inp-tokp==4 && strncmp(tokp, "else", 4)==0){
			strcpy(token, "\"\"");
			return STRING;
		}
		skipid();
		skipwhite();
		if(*inp!='('){
			savetoken(tokp);
			return NAME;
		}
		int level=1;
		inp++;
		while(level!=0){
			if(*inp=='\0') return ERROR;
			if(*inp=='(') level++;
			if(*inp==')') --level;
			inp++;
		}
		savetoken(tokp);
		return CALL;
	}
	else if(*inp=='"'){
		inp++;
		while(*inp!='"'){
			if(*inp=='\0') return ERROR;
			if(*inp=='\\') inp++;
			inp++;
		}
		inp++;
		savetoken(tokp);
		return STRING;
	}
	else if(*inp!='\0') return ERROR;
	//
	// Don't advance past EOL -- if someone calls again,
	// just return EOL again
	//
	return EOL;
}
MConfig *getmconfig(char *token){
	Name *name=lookup(token);
	if(name->fn==0 && name->config==0) return newMConfig(name, 0, 0);
	error("name %s already in use", name->name);
	return 0;
}
//
// Read a config with the given name and add it to
// the machines MConfig list
//
Config *getconfig(void){
	int t=gettoken();
	if(t==EOL) return 0;
	if(t!=STRING){
		error("mconfig must specify symbols");
		return 0;
	}
	char *symbols=strdup(token);	// should strip quotes
	if(gettoken()!=STRING){
		error("mconfig must specify operations");
		return 0;
	}
	char *operations=strdup(token);	// should check syntax
	t=gettoken();
	if(t!=NAME && t!=CALL){
		error("mconfig must specify final config\n");
		return 0;
	}
	Name *final=lookup(token);
	if(gettoken()!=EOL)
		error("junk past the end of a config\n");
	return newConfig(symbols, operations, final, 0);
}
//
// Crack a Function header
//
Function *getfunction(char *token){
	Name *name;
	char *s=token;
	while(idchar(*s)) s++;
	char id[8192];
	strncpy(id, token, s-token);
	name=lookup(id);
	if(*s!='('){
		error("Bad function header %s", token);	// should never happen
		exit(1);
	}
	s++;
	Param *params=0;
	Param *eparams=0;
	for(;;){
		char *t=s;
		while(idchar(*t)) t++;
		strncpy(id, s, t-s);
		Param *p=newParam(lookup(id), 0, 0);
		if(eparams) eparams->next=p;
		else params=p;
		eparams=p;
		s=t;
		if(*s!=',') break;
		s++;
	}
	if(*s!=')' || s[1]!='\0'){
		error("Bad function header %s", token);	// should never happen
		exit(1);
	}
	return newFunction(name, params, 0);
}
FnConfig *getfnconfig(void){
	Name *symname=0;
	char *symbols=0;
	int t=gettoken();
	if(t==EOL) return 0;
	if(t==NAME)
		symname=lookup(token);
	else if(t==STRING)
		symbols=strdup(token);
	else{
		error("config in function def must specify symbols (or a parameter name)");
		return 0;
	}
	if(gettoken()!=STRING){
		error("config in function def must specify operations");
		return 0;
	}
	char *operations=strdup(token);
	t=gettoken();
	Name *final;
	if(t==NAME || t==CALL)
		final=lookup(token);
	else{
		error("config in function def must specify final");
		return 0;
	}
	if(gettoken()!=EOL)
		error("junk at the end of config in function");
	return newFnConfig(symname, symbols, operations, final, 0);
}
//
// Read a config (possibly with embedded parameter names) and
// add it to fn
//
void input(char *file){
	infile=file;
	FILE *in=fopen(file, "r");
	if(in==0){
		perror(file);
		exit(1);
	}
	Function *function=0;
	FnConfig *efnconfig=0;
	MConfig *mconfig=0;
	Config *econfig=0;
	for(lineno=1;fgets(line, sizeof line, in)!=0;lineno++){
		inp=line;
		int t;

		if(line[0]==' ' || line[0]=='\t' || line[0]=='#'){
			//
			// Two legal possibilities at this point:
			//	a blank line, which we should ignore
			//	another config for an mconfig or a function.
			skipwhite();
			if(*inp=='\n' || *inp=='\0')
				continue;
			if(function){
				FnConfig *f=getfnconfig();
				if(efnconfig)
					efnconfig->next=f;
				else
					function->configs=f;
				efnconfig=f;
			}
			else if(mconfig){
				Config *c=getconfig();
				if(econfig)
					econfig->next=c;
				else
					mconfig->config=c;
				econfig=c;
			}
			else
				error("orphan continuation line");
		}
		else{
			t=gettoken();
			switch(t){
			default:
				error("unknown token. Cannot happen!!\n");
				exit(1);
			case EOL:
				break;
			case FNDEF:
				mconfig=0;
				function=getfunction(token);
				function->configs=getfnconfig();
				efnconfig=function->configs;
				break;
			case CALL:	// Maybe this is an error?
			case NAME:
				function=0;
				mconfig=getmconfig(token);
				mconfig->config=getconfig();
				econfig=mconfig->config;
				break;
			case STRING:
				error("Not expecting a string here");
				break;
			case ERROR:
				error("unrecognized token");
				break;
			}
		}
	}
	fclose(in);
}
int main(int argc, char *argv[]){
	if(argc!=2){
		fprintf(stderr, "Usage: %s machine-file\n", argv[0]);
		return 1;
	}
	input(argv[1]);
	if(nerror!=0) return 1;
	return 0;
}
