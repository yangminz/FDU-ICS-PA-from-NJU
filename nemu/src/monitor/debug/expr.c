#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ , NUM = 23

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */
	 {"[0-9]{1,}",	NUM},
	{" +",	NOTYPE},				// spaces
	{"\\+", '+'},					// plus
	{"\\-", '-'},
	{"\\*", '*'},
	{"\\/", '/'},
	{"\\(", '('},
	{"\\)", ')'},
	{"==", EQ}						// equal
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

static bool make_token(char *e, int * end) {
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
				 	printf("Length Over Flow!\nEnd the command!\n");
				 	break;
				 }
				switch(rules[i].token_type) {
					case NOTYPE: 
						nr_token -= 1;
						break;
					case NUM:
						tokens[nr_token].type = NUM;
						sprintf( tokens[nr_token].str, "%.*s", substr_len, substr_start );
						break;	
					case '+':
						tokens[nr_token].type = '+';
						sprintf( tokens[nr_token].str, "%.*s", substr_len, substr_start );
						break;
					case '-':
						tokens[nr_token].type = '-';
						sprintf( tokens[nr_token].str, "%.*s", substr_len, substr_start );
						break;
					case '*':
						tokens[nr_token].type = '*';
						sprintf( tokens[nr_token].str, "%.*s", substr_len, substr_start );
						break;
					case '/':
						tokens[nr_token].type = '/';
						sprintf( tokens[nr_token].str, "%.*s", substr_len, substr_start );
						break;
					case '(':
						tokens[nr_token].type = '(';
						sprintf( tokens[nr_token].str, "%.*s", substr_len, substr_start );
						break;
					case ')':
						tokens[nr_token].type = ')';
						sprintf( tokens[nr_token].str, "%.*s", substr_len, substr_start );
						break;
					default: panic("please implement me");
				}
				nr_token += 1;
				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}
	*end = nr_token;
	return true; 
}

static bool check_parentheses( int p, int q ){
	return false;
}

int eval( int p, int q ){
	if( p > q ){
		/* Bad expression */
		return -1;
	}
	else if( p == q ){
		/* Single token.
		 * For now this token should be a number
		 * Return the value of the number
		 */
		 int tmp;
		 sscanf( tokens[p].str, "%d", &tmp );
		 return tmp;
	}
	else if( check_parentheses( p, q ) == true ){
		/* The expression is surrounded by a matched pair of parentheses
		 * If that is the case, just throw away the paretheneses
		 */
		 return eval( p + 1, q - 1 );
	}
	else{
		/* We should do more things here */
		int op = q;
		int p_or_m = -256;
		int m_or_d = -256;
		for( ; op >= p; op -- ){
			if( tokens[op].type != NUM ){
				if( ( tokens[op].type == '+' || tokens[op].type == '-' ) && ( p_or_m == -256 ) )
					p_or_m  = op;
				if( ( tokens[op].type == '*' || tokens[op].type == '/' ) && ( m_or_d == -256 ) )
					m_or_d = op;
			}
		}
		op = ( p_or_m > -255 ) ? p_or_m : m_or_d;
		if( op < 0 ) assert(0);
		int val1 = eval( p, op - 1 );
		int val2 = eval( op + 1, q );
		switch( tokens[op].type ){
			case '+': return val1 + val2; break;
			case '-': return val1 - val2; break;
			case '*': return val1 * val2; break; 
			case '/': return val1 / val2; break;
			default: assert(0); 
		}
	}
}

uint32_t expr(char *e, bool *success) {
	int end;
	if(!make_token(e, &end)) {
		*success = false;
		return 0;
	}
	*success = true;
	/* TODO: Insert codes to evaluate the expression. */
	return eval( 0, end - 1 );
	panic("please implement me");
	return 0;
}