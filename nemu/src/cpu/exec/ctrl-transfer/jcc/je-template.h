#include "cpu/exec/template-start.h"

#define instr je

static void do_execute(){
	int32_t rel = op_src->val;
	// printf("0x%x\n", rel);
#if DATA_BYTE == 1
	rel = (int32_t) (int16_t) (int8_t) rel;
	print_asm(str(instr) " %x", cpu.eip + rel + 2);
#elif DATA_BYTE == 2
	rel = (int32_t) (int16_t) rel;
	print_asm(str(instr) " %x", cpu.eip + rel + 5);
#elif DATA_BYTE == 4
	rel = (int32_t) rel;
	print_asm(str(instr) " %x", cpu.eip + rel + 6);
#else
#error unknown DATA_BYTE
#endif
	if(cpu.eflags.ZF){
		cpu.eip += rel;
	}
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"