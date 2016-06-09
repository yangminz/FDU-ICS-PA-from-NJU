#include "cpu/exec/template-start.h"

#define instr jmp

static void do_execute(){
	cpu.eip += (DATA_TYPE_S) op_src->val;
	print_asm(str(instr) " %x", cpu.eip + DATA_BYTE + 1);	
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"