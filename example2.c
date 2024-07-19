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
#define	END	'\0'
#define	START	'b'
#define	NTAPE	65536
#define	NSTEP	1024
struct Tuple{
	char config;
	const char *symbols;
	const char *operations;
	char final;
} tuple[]={
//	config	symbols	operations			final
	'b',	" ",	"Pe,R,Pe,R,P0,R,R,P0,L,L",	'o',
	
	'o',	"1",	"R,Px,L,L,L",			'o',
	'o',	"0",	"",				'q',
	
	'q',	"01",	"R,R",				'q',
	'q',	" ",	"P1,L",				'p',
	
	'p',	"x",	"E,R",				'q',
	'p',	"e",	"R",				'f',
	'p',	" ",	"L,L",				'p',
	
	'f',	"ex01",	"R,R",				'f',
	'f',	" ",	"P0,L,L",			'o',
	END
};
int main(void){
	char tape[NTAPE];
	memset(tape, ' ', NTAPE);
	int loc=0;
	int hwm=0;
	char config=START;
	printf(" \n");
	printf("%c\n", config);
	for(int nstep=0;nstep!=NSTEP;nstep++){
		struct Tuple *t;
		for(t=tuple;;t++){
			if(t->config==END){
				printf("Nothing for state %c, input %c\n",
					config, tape[loc]);
				return 0;
			}
			if(t->config==config
			&& strchr(t->symbols, tape[loc])!=0)
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
		for(int i=0;i<=hwm;i++) putchar(tape[i]);
		putchar('\n');
		for(int i=0;i!=loc;i++) putchar(' ');
		printf("%c\n", config);
	}
	printf("Stopped after %d steps, loc=%d, hwm=%d\n", NSTEP, loc, hwm);
	return 0;
}
