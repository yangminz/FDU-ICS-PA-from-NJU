#include "cpu/exec/template-start.h"

#define instr add

static void do_execute() {

	DATA_TYPE dest = op_dest->val, src = op_src->val;
	DATA_TYPE result = dest + src;
	OPERAND_W(op_dest, result);
	
	cpu.eflags.ZF = !result;
	cpu.eflags.SF = MSB(result);
	cpu.eflags.OF = (MSB(dest) == MSB(src)) && (MSB(dest) != MSB(result));
	cpu.eflags.CF = (result < dest);
	
	uint8_t low8bits = result & 0xff;
	int parity = 1;
	int i = 0;
	for(i=0;i<8;i++){
		if(low8bits & 0x1){
			parity = !parity;
		}
		low8bits = low8bits >> 1;
	}
	cpu.eflags.PF = parity;
	print_asm_template2();
}

make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(si2rm)

#include "cpu/exec/template-end.h"