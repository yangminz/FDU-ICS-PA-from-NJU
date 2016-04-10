#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

#include <malloc.h>

enum {
	NOTYPE = 256, EQ , DEC = 23

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */
	 {"[0-9]{1,}",	DEC},
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
// used for the whole codeevaluate 

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
				switch(rules[i].token_type) {
					case NOTYPE: break;
					case DEC:
						tokens[nr_token].type = DEC;
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

	return true; 
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
	int i;
	for( i = 0; i < 32; i ++ )
		printf("%s", tokens[i].str );
	/* TODO: Insert codes to evaluate the expression. */
	// use tokens[] to evaluate
	panic("please implement me");
	return 0;
}

