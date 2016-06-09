#include "cpu/exec/template-start.h"

#define instr push

static void do_execute(){

#if DATA_BYTE == 4 || DATA_BYTE == 1
	uint32_t val = op_src->val;
	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, val);
	print_asm_template1();
#else
	panic("please implement me!");
#endif

}

make_instr_helper(i)
make_instr_helper(rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"