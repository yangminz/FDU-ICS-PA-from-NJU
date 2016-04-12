#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOT, PTR, NEG, MUL, DIV, ADD, SUB, EQ, NEQ, AND, OR, DEC, HEX, REG, LPA, RPA, NOTYPE = 256
	/* TODO: Add more token types */
};
int prior[16] = { 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 5, -3, -3, -3, -1, -2 };

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */
	{" +",	NOTYPE},				// spaces
	{"0x[0-9a-fA-F]{1,}",	HEX},
	{"\\$[a-zA-Z]{2,3}",	REG},
	{"[0-9]{1,}",	DEC},	
	{"\\(", LPA},
	{"\\)", RPA},
	{"\\+", ADD},					// plus
	{"\\-", SUB},
	{"\\*", MUL},
	{"\\/", DIV},
	{"==", EQ},						// equal
	{"!=", NEQ},
	{"&&", AND},
	{"\\|\\|", OR},
	{"\\!", NOT}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0; 

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			// #define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )
			// regexec is a function from Regular Expression library
			// to match regular expression
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array ``tokens''. For certain 
				 * types of tokens, some extra actions should be performed.
				 */
				 if( substr_len >= 32 ){
				 	printf("Length Over Flow!\nEnd the command!\n"); break;
				 }
				switch(rules[i].token_type) {
					case NOTYPE:
						nr_token -= 1; break;
					case HEX:
						tokens[nr_token].type = HEX;
						sprintf( tokens[nr_token].str, "%.*s", substr_len, substr_start ); break;
					case REG:
						tokens[nr_token].type = REG;
						sprintf( tokens[nr_token].str, "%.*s", substr_len, substr_start + 1 ); break;
					case DEC:
						tokens[nr_token].type = DEC;
						sprintf( tokens[nr_token].str, "%.*s", substr_len, substr_start ); break;	
					case ADD:    tokens[nr_token].type = ADD; break;
					case SUB:    tokens[nr_token].type = SUB; break;
					case MUL:    tokens[nr_token].type = MUL; break;
					case DIV:    tokens[nr_token].type = DIV; break;
					case LPA:    tokens[nr_token].type = LPA; break;
					case RPA:    tokens[nr_token].type = RPA; break;
					case EQ:    tokens[nr_token].type = EQ; break;
					case NEQ:    tokens[nr_token].type = NEQ; break;
					case AND:    tokens[nr_token].type = AND; break;
					case OR:    tokens[nr_token].type = OR; break;
					case NOT:    tokens[nr_token].type = NOT; break;
					default: panic("please implement me");
				}
				nr_token += 1; break;
			}
		}
		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}
	return true; 
}

static bool check_parentheses( int p, int q, bool * legal ){
	int i , legal_check = 0;
	bool match = false;
	if( tokens[p].type ==  LPA  && tokens[q].type ==  RPA )
		match = true;
	for( i = p; i <= q; i ++ ){
		if( tokens[i].type ==  LPA )
			legal_check ++;
		if( tokens[i].type ==  RPA )
			legal_check --;
		if( ( legal_check == 0 ) && ( i != q ) )
			match = false;
	}
	*legal = true;
	if( legal_check != 0 ) 
		*legal = false;
	return match;
}
uint32_t swaddr_read(swaddr_t, size_t);
int eval( int p, int q ){
	bool legal = true;
	if( p > q ){
		printf("160 Bad expression!\n");
		assert(0); 
	}
	else if( p == q ){
		/* Single token.
		 * For now this token should be a number
		 * Return the value of the number
		 */
		 int val = -256;
		 int i;
		 char * reg_name[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
		 switch ( tokens[p].type ){
		 	case HEX: sscanf( tokens[p].str, "%x", &val ); break;
		 	case DEC: sscanf( tokens[p].str, "%d", &val ); break;
		 	case REG: 
		 		for( i = 0; i < 8; i ++ ){
		 			if( strcmp( tokens[p].str, reg_name[i] ) == 0 ){
		 				val = cpu.gpr[i]._32; break;
		 			}
		 		} break;
		 	default: assert(0);
		 }
		 return val;
	}
	else if( check_parentheses( p, q, &legal ) == true ){
		/* The expression is surrounded by a matched pair of parentheses
		 * If that is the case, just throw away the paretheneses
		 */
		 return eval( p + 1, q - 1 );
	}
	else{
		/* We should do more things here */
		if( legal == false ) {
			printf("193 Bad expression!\n");
			assert(0);
		}
		//*******************************
		int prior_arr[32]; // find maximum in it
		int locate[32];
		int i, j, num;
		for( i = q, num = 0; i >= p; i -- ){
			if( prior[ tokens[i].type ] > -3 ){
				if( tokens[i].type == RPA ){
					for( j = i - 1; check_parentheses( j, i, &legal ) == false; j -- );
					i = j;
				}
				else{
					prior_arr[num] = prior[ tokens[i].type ];
					locate[num] = i;
					num ++;
				}				
			}
		}
		for( i = 0, j = 0; i < num; i ++ ){
			j = ( prior_arr[i] > prior_arr[j] ) ? i : j;
		}
		int op = locate[j];
		// ********************************
		int val2 = eval( op + 1, q );
		switch( tokens[op].type ){
			case ADD: return eval( p, op - 1 ) + val2; break;
			case SUB: return eval( p, op - 1 ) - val2; break;
			case MUL: return eval( p, op - 1 ) * val2; break; 
			case DIV: return eval( p, op - 1 ) / val2; break;
			case EQ: return (eval( p, op - 1 ) == val2); break;
			case NEQ: return (eval( p, op - 1 ) != val2); break;
			case AND: return (eval( p, op - 1 ) && val2); break;
			case OR: return (eval( p, op - 1 ) || val2); break;
			//**********************
			case NOT: return !(val2); break;
			case NEG: return -1 * val2; break;
			case PTR: return swaddr_read( val2, 4 ); break;
			default: assert(0); 
		}
	}
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
	*success = true;
	/* TODO: Insert codes to evaluate the expression. */
	int i;
	for( i = 0; i < nr_token; i ++ ){
		if( tokens[i].type == MUL && ( i == 0 || prior[ tokens[i - 1].type ] > -2 )  ) tokens[i].type = PTR;
		if( tokens[i].type == SUB && ( i == 0 || prior[ tokens[i - 1].type ] > -2 )  ) tokens[i].type = NEG;
	}
	return eval( 0, nr_token - 1 );
	panic("please implement me");
	return 0;
}