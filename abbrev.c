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
// Turing warns (bottom of page 236) about infinite expansion, but I
// think if you only expand functions with literal arguments (guaranteed
// to produce literal results) you should never run into trouble unless
// you have a recursive definition. (The real problem is that m-functions
// have no facility for conditional evaluation, and thus no way to stop
// a runaway recursion.)
//
typedef struct Name Name;
typedef struct MConfig MConfig;
typedef struct Config Config;
typedef struct Function Function;
typedef	struct Param Param;
typedef struct FnConfig FnConfig;
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
	// Only one of fn and config should be non-zero
	// fn is a configuration function.
	// config is the first of a list of MConfigs describing
	// what to do when the machine is in this configuration.
	//
	Function *fn;
	MConfig *config;	// If non-zero, has already been expanded.
	Name *next;
};
Name *newName(char *name, Function *fn, MConfig *config, Name *next){
	Name *n=emalloc(sizeof(Name));
	n->name=name;
	n->fn=fn;
	n->config=config;
	n->next=next;
	return n;
}
struct Config{
	char *symbols;		// might be better if this were a bitmask
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
struct MConfig{
	Name *name;
	Config *config;
	MConfig *next;
};
MConfig *newMConfig(Name *name, Config *config, MConfig *next){
	MConfig *m=emalloc(sizeof(MConfig));
	m->name=name;
	m->config=config;
	m->next=next;
	return m;
}
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
FnConfig *newFnConfig(Name *symname, char *symbols, char *operations, Name *final, FnConfig *next){
	FnConfig *f=emalloc(sizeof(FnConfig));
	f->symname=symname;
	f->symbols=symbols;
	f->operations=operations;
	f->final=final;
	f->next=next;
	return f;
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
	Param *params;
	FnConfig *configs;
};
Function *newFunction(Name *name, Param *params, FnConfig *configs){
	Function *f=emalloc(sizeof(Function));
	f->name=name;
	f->params=params;
	f->configs=configs;
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
Name *enamelist=0;		// last name on namelist, or 0 if none.
Unexpanded *unexpanded=0;	// list of unexpanded calls
Unexpanded *eunexpanded=0;	// end of the unexpanded list, or 0 if none.
//
// Used by input
//
char line[8192];	// the line currently being parsed
int lineno;		// line number of the current line
char token[8192];	// token most recently read
char *inp;		// next character to be scanned on the line
int nerror;		// total number of error messages
char *infile;		// name of the input file
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
	fprintf(stderr, " on line %d of %s\n", lineno, infile);
	//
	// Then print the line and indicate the position at which the message
	// was issued.
	//
	fprintf(stderr, "%s\n", line);
	for(char *s=line;s!=inp && *s!='\n' && *s!='\0';s++)
		putc(*s=='\t'?'\t':' ', stderr);
	fprintf(stderr, "^\n");
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
	n=newName(strdup(s), 0, 0, 0);
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
}
void skipid(void){
	while(idchar(*inp)) inp++;
}
void skipstring(void){
	if(*inp!='"'){
		error("Can't happen: skipstring called on non-string");
		exit(1);
	}
	inp++;
	while(*inp!='"'){
		if(*inp=='\0'){
			error("end of line in string");
			return;
		}
		if(*inp=='\\') inp++;
		inp++;
	}
}
//
// Save the token starting at start and ending at inp,
// squishing out all white space.
// Except we need to preserve white space in STRINGs
//
void savetoken(char *start){
	char *tokp=token;
	if(*start=='"')
		for(char *s=start;s!=inp;s++) *tokp++=*s;
	else
		for(char *s=start;s!=inp;s++)
			if(*s!=' ' && *s!='\t' && *s!='\n') *tokp++=*s;
	*tokp='\0';
}
void copyid(char *dst, char *src, char *esrc){
	int len=esrc-src;
	memcpy(dst, src, len);
	dst[len]='\0';
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
		skipwhite();
		if(*inp!='('){
			savetoken(tokp);
			return NAME;
		}
		int level=1;
		inp++;
		while(level!=0){
			skipwhite();
			if(*inp=='(') level++;
			else if(*inp==')') --level;
			else if(*inp=='"') skipstring();
			else if(*inp!=',' && !idchar(*inp)) return ERROR;
			inp++;
		}
		savetoken(tokp);
		return CALL;
	}
	else if(*inp=='"'){
		skipstring();
		if(*inp!='"'){
			error("Can't happen: no terminating \" on string");
			exit(1);
		}
		inp++;
		savetoken(tokp);
		return STRING;
	}
	else if(*inp=='#'){
		while(*inp!='\0') inp++;
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
	if(name->fn!=0 || name->config!=0){
		error("name %s already in use", name->name);
		return 0;
	}
	MConfig *m=newMConfig(name, 0, 0);
	name->config=m;
	return m;
}
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
		error("mconfig must specify final config");
		return 0;
	}
	Name *final=lookup(token);
	if(gettoken()!=EOL)
		error("junk past the end of a config");
	Config *c=newConfig(symbols, operations, final, 0);
	if(t==CALL){
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
	if(name->fn || name->config){
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
	for(;;){
		char *t=s;
		while(idchar(*t)) t++;
		copyid(id, s, t);
		Param *p=newParam(lookup(id), 0, 0);
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
	name->fn=newFunction(name, params, 0);
	return name->fn;
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
		error("config in function def must specify symbols "
			"(or a parameter name)");
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
// Read a skeleton table file.
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

		if(line[0]==' ' || line[0]=='\t'){
			//
			// Two legal possibilities at this point:
			//	a blank line, which we should ignore
			//	another config for an mconfig or a function.
			skipwhite();
			if(*inp=='\0') continue;	// nothing on line
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
				error("Can't happen: unknown token");
				exit(1);
			case EOL: break;	// empty line
			case FNDEF:
				mconfig=0;
				function=getfunction(token);
				function->configs=getfnconfig();
				efnconfig=function->configs;
				break;
			case CALL:
				//
				// Not sure this is actually an error.
				//
				error("not expecting a call here");
				break;
			case NAME:
				function=0;
				mconfig=getmconfig(token);
				mconfig->config=getconfig();
				econfig=mconfig->config;
				if(start==0) start=mconfig->name;
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
		if(alpha(*srcp)){
			//
			// copy the name into dst
			//
			namep=dstp;
			while(idchar(*srcp)) *dstp++=*srcp++;
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
// Expand a FnConfig, producing a Config.
// 
Config *expandconfig(FnConfig *fc, Param *param){
	char *symbols;
	//
	// Symbols either come from a literal in the FnConfig
	// or by looking up a parameter.
	// Someone should have checked that the parameter is
	// legit earlier, but no one did, so we have to do the check here.
	//
	// We also have to theck that the parameter value is a legit
	// string. I don't think there's anywhere else we could have done
	// that.
	//
	if(fc->symbols) symbols=fc->symbols;
	else{
		symbols=findparam(fc->symname->name, param);
		if(symbols==0){
			error("Can't expand symbols -- no param %s",
				fc->symname);
			return 0;
		}
		if(symbols[0]!='"'){
			error("Can't expand symbols -- param %s=%s is not "
				"a string", fc->symname->name, symbols);
			return 0;
		}
	}
	char *operations;
	operations=fc->operations;
	//
	// Expand final.
	// First make a copy of fc->final->name with any
	// parameters substituted (that's the job of substitute).
	//
	Name *final;
	char buf[8192];
	substitute(buf, fc->final->name, param);
	//
	// What if, at this point, buf contains a string rather than a
	// name or a call? I think it might be possible for that to happen
	// if the FnConfig's final is a param that has a string value. But
	// I don't think there's any plausible way that an m-config should have
	// a quoted string for a name.
	//
	final=lookup(buf);
	//
	// Then check whether the result is legit -- something that
	// expands to the header of a fn definition is questionable. (There's
	// no real need for this, but it's pretty bogus.)
	//
	if(final->fn!=0){
		error("trying to expand %s, but that's just wrong",
			final->name);
		return 0;
	}
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
//
// Process a call to an m-function.
// All this parsing and reparsing of CALLs makes me think
// we should do it once and for all (during input) and
// attach some sort of parse tree to the CALL in the
// symbol table.
//
void expand(Name *name){
	if(name->config!=0) return;	// already expanded.
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
	if(fname->fn==0){
		error("Undefined function %s in call %s", fname->name, call);
		return;
	}
	//
	// crack arguments starting at callp and set each
	// Param::value to the corresponding argument.
	//
	Param *paramp=fname->fn->params;
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
			if(*callp=='"'){
				//
				// Soak up a quoted string -- in particular
				// don't count parentheses inside the string.
				// This almost duplicates code in skipstring
				// which is annoying and should be fixed.
				//
				callp++;
				while(*callp!='"'){
					//
					// This can't happen because
					// input has already verified
					// that every string has a
					// closing quote
					//
					if(*callp=='\0'){
						error("Can't happen: end of "
							"token in string while "
							"expanding %s",
							name->name);
						exit(1);
					}
					if(*callp=='\\') callp++;
					callp++;
				}
			}
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
	// do the expansion for each FnConfig in fc->fn
	//
	name->config=newMConfig(name, 0, 0);
	Config *econfig=0;
	for(FnConfig *fc=fname->fn->configs;fc;fc=fc->next){
		Config *c=expandconfig(fc, fname->fn->params);
		if(econfig==0)
			name->config->config=c;
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
void show(FILE *f){
	if(start)
		fprintf(f, "# start %s\n", start->name);
	else
		fprintf(f, "# No start symbol!\n");
	for(Name *n=namelist;n!=0;n=n->next){
		if(n->config!=0){
			fprintf(f, "%s\n", n->name);
			for(Config *c=n->config->config;c!=0;c=c->next){
				fprintf(f, "\t%s", c->symbols);
				fprintf(f, "\t%s", c->operations);
				fprintf(f, "\t%s\n", c->final->name);
			}
		}
#if 0
		else if(n->fn!=0){
			fprintf(f, "def %s", n->name);
			int sep='(';
			for(Param *p=n->fn->params;p!=0;p=p->next){
				fprintf(f, "%c%s", sep, p->name->name);
				sep=',';
			}
			fprintf(f, ")\n");
			for(FnConfig *fn=n->fn->configs;fn!=0;fn=fn->next){
				if(fn->symname!=0)
					fprintf(f, "\t%s", fn->symname->name);
				else
					fprintf(f, "\t%s", fn->symbols);
				fprintf(f, "\t%s", fn->operations);
				fprintf(f, "\t%s\n", fn->final->name);
			}
		}
#endif
	}
}
int main(int argc, char *argv[]){
	if(argc!=2){
		fprintf(stderr, "Usage: %s machine-file\n", argv[0]);
		return 1;
	}
	input(argv[1]);
	if(nerror!=0) return 1;
	expandall();
	show(stdout);
	return 0;
}
