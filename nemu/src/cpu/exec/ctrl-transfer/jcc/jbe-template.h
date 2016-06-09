#include "cpu/exec/template-start.h"

#define instr jbe

static void do_execute(){
	int32_t rel = op_src->val;
#if DATA_BYTE == 1
	rel = (int8_t) rel;
	print_asm(str(instr) " %x", cpu.eip + rel + 2);
#endif
	if( cpu.eflags.CF || cpu.eflags.ZF ){
		cpu.eip += rel;
	}
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"