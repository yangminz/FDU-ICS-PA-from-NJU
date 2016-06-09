#include "cpu/exec/template-start.h"

#define instr call

static void do_execute () {
#if DATA_BYTE == 4 
	cpu.esp -= 4;
	
	if(op_src->type == OP_TYPE_IMM){
		MEM_W(cpu.esp, cpu.eip + 5);
		cpu.eip += op_src->val;
		print_asm(str(instr) " %x", cpu.eip + 5);
	}
#else
		panic("please implement me!");
#endif
	
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(i)
make_instr_helper(rm)
#endif


#include "cpu/exec/template-end.h"
