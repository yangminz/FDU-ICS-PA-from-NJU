#include "cpu/exec/template-start.h"

#define instr push

static void do_execute(){

#if DATA_BYTE == 4 || DATA_BYTE == 1
	//assert(op_src->type == OP_TYPE_REG);
	//printf("op_src->reg: 0x%d\n", op_src->reg);
	uint32_t val = op_src->val;
	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, val);
	//MEM_W(cpu.esp, op_src->val);
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
/*
make_helper(concat(push_rm_, SUFFIX)) {
	cpu.esp -= 4;
	swaddr_t addr = instr_fetch(eip + 1, 2);
	MEM_W(cpu.esp, MEM_R(addr));

	print_asm("push" str(SUFFIX) " 0x%x", addr);
	return 3;
}
*/

#include "cpu/exec/template-end.h"