#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
/*========================================================*/
/* PA2.2                                                  */
/*========================================================*/
#include <elf.h>

void cpu_exec(uint32_t);

/* We use the ``readline'' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_si( char * args ){
	int n = atoi(args);
	if( n <= -1 ){
		printf("Error!\n");
		return -1;
	}
	int i=0;
	for(; i<n; i++){
		cpu_exec(1);
	}
	return 1;
}
// ******** src/monitor/debug/watchpoint.c
void query_wp();
static int cmd_info( char * args ){
	if( strcmp((const char * )args, "r" ) == 0 ){
		int i;
		char * name_32[8]= {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
		for(i = 0; i < 4; i ++) {
			printf("%s\t0x%0x\t%d\n", name_32[i], cpu.gpr[i]._32, cpu.gpr[i]._32 );
		}
		for(i = 4; i < 6; i ++) {
			printf("%s\t0x%0x\t0x%0x\n", name_32[i], cpu.gpr[i]._32, cpu.gpr[i]._32 );
		}
		for(i = 6; i < 8; i ++) {
			printf("%s\t0x%0x\t%d\n", name_32[i], cpu.gpr[i]._32, cpu.gpr[i]._32 );
		}
		printf("eip\t0x%0x\t%d\n", cpu.eip, cpu.eip );
	}
	else if( strcmp( (const char * )args, "w" ) == 0 ){
		query_wp();
	}
	return 1;
}
// ******** include/monitor/monitor.h
uint32_t swaddr_read(swaddr_t, size_t);
static int cmd_x( char * args ){
	char * _num_ = strtok(args, " ");
	int num = atoi( _num_ );
	if( num == 0 ){
		printf("Error!\n");
		return -1;
	}
	char * expr = _num_ + strlen( _num_ ) + 1;
	int addr;
	sscanf( expr, "%x", &addr );

	printf("0x%08x:\t", addr );
	int i = 0;
	for( ; i < num; i ++ ){
		printf("0x");
		printf("%02x", swaddr_read(addr + 3, 1) );
		printf("%02x", swaddr_read(addr + 2, 1) );
		printf("%02x", swaddr_read(addr + 1, 1) );
		printf("%02x", swaddr_read(addr , 1) );
		printf("\t");
		addr += 4;
	}
	printf("\n");
	return 1;
}
// ******** src/monitor/debug/expr.c
uint32_t expr(char *e, bool *success);

static int cmd_p( char * args ){
	bool if_succ;
	uint32_t result = expr( args, &if_succ );
	if( if_succ == false ){
		printf("Error!\n");
		return 0;
	}
	printf("%d\n", result );
	return 1;
}
// ******** src/monitor/debug/watchpoint.c
WP * new_wp();
static int cmd_w( char * args ){
	bool if_success;
	WP * nwp = new_wp();
	if( strlen( args ) > 32 ){
		printf("String Len Overflow!\n");
		return 0;
	}
	strcpy( nwp->args, args );
	nwp->old_val = expr( args , &if_success);
	return 0;
}
// ******** src/monitor/debug/watchpoint.c
bool delete_wp( int num );
static int cmd_d( char * args ){
	int num = atoi( args );
	if( !delete_wp( num ) ) 
		printf("Delete Index Error!\n");;
	return 0;
}

/*========================================================*/
/* PA2.2                                                  */
/*========================================================*/
typedef struct {
	swaddr_t prev_ebp;
	swaddr_t ret_addr;
	uint32_t args[4];
} PartOfStackFrame;
// Frome nemu/src/monitor/debug/elf.c
extern char * get_eipFunc(uint32_t eip);

static int cmd_bt(){	
	PartOfStackFrame frame[32];
	//printf("  0x%08x \n", cpu.eip);
	int i = 0;
	uint32_t ebp = cpu.ebp, eip = cpu.eip;
	while( ebp != 0 && eip != 0 && i <32 ){
		char * func = get_eipFunc(eip);
		//printf("%s\n", func);
		printf("#%d  0x%08x in %s ()\n", i, eip, func);
		frame[i].prev_ebp = swaddr_read(ebp, 4);
		frame[i].ret_addr = swaddr_read(ebp + 4, 4);
		ebp = frame[i].prev_ebp;
		eip = frame[i].ret_addr;	
		i++;
	}
	return 0;
}
/*========================================================*/
/* END OF PA2.2                                           */
/*========================================================*/

static int cmd_help(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },

	/* TODO: Add more commands */
	{ "si", "Execute singular steps", cmd_si },
	{ "info", "Fetch information of registers and mointor point", cmd_info },
	{ "x", "Scan the memory", cmd_x },
	{ "p", "Get the value of expression", cmd_p},
	{ "w", "Set watchpoint", cmd_w},
	{ "d", "Delete watchpoint", cmd_d},
	{ "bt", "Print Chain of Stack Frame", cmd_bt},
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
