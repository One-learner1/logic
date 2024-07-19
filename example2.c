//*% $COM -o # %
//
// Turing machine, as in Turing's Computable Numbers paper
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//
// The second example in section 3
//
enum Config{
	END, B, O, Q, P, F
};
struct Tuple{
	enum Config config;
	const char *symbols;
	const char *operations;
	enum Config final;
} tuple[]={
B,	" ",	"Pe,R,Pe,R,P0,R,R,P0,L,L",	O,

O,	"1",	"R,Px,L,L,L",			O,
O,	"0",	"",				Q,

Q,	"01",	"R,R",				Q,
Q,	" ",	"P1,L",				P,

P,	"x",	"E,R",				Q,
P,	"e",	"R",				F,
P,	" ",	"L,L",				P,

F,	"ex01",	"R,R",				F,
F,	" ",	"P0,L,L",			O,
END
};
char name(enum Config c){
	switch(c){
	default:
		fprintf(stderr, "Bad config %d\n", c);
		exit(1);
	case B: return 'b';
	case O: return 'o';
	case Q: return 'q';
	case P: return 'p';
	case F: return 'f';
	}
}
#define	NTAPE	65536
#define	NSTEP	1024
int main(void){
	char tape[NTAPE];
	memset(tape, ' ', NTAPE);
	int loc=0;
	int hwm=0;
	enum Config config=B;
	for(int nstep=0;nstep!=NSTEP;nstep++){
		struct Tuple *t;
		for(t=tuple;;t++){
			if(t->config==END){
				printf("Nothing for state %d, input %c\n",
					config, tape[loc]);
				return 0;
			}
			if(t->config==config && strchr(t->symbols, tape[loc])!=0)
				break;
		}
		for(const char *op=t->operations;*op;op++) switch(*op){
		case ',':
			break;
		case 'P':
			op++;
			if(*op=='\0'){
				fprintf(stderr, "Bad P, t->operations=%s\n",
					t->operations);
				return 1;
			}
			tape[loc]=*op++;
			break;
		case 'E':
			tape[loc]=' ';
			break;
		case 'R':
			if(++loc==NTAPE){
				printf("Out of tape!\n");
				return 0;
			}
			if(loc>hwm) hwm=loc;
			break;
		case 'L':
			if(--loc<0){
				fprintf(stderr, "Ran off left end of tape\n");
				return 1;
			}
			break;
		}
		config=t->final;
		for(int i=0;i<=hwm;i++){
				putchar(tape[i]);
			if(i==loc)
				printf("[%c]", name(config));
		}
		putchar('\n');
	}
	printf("Stopped after %d steps, loc=%d, hwm=%d\n", NSTEP, loc, hwm);
	return 0;
}
