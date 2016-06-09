#include "cpu/exec/template-start.h"

#define instr sub

static void do_execute() {
	DATA_TYPE dest = op_dest->val, src = op_src->val;
	DATA_TYPE result = dest - src;
	OPERAND_W(op_dest, result);
	uint8_t low_order_8_bits = result & 0xff;
	int parity = 1;
	int i = 0;
	cpu.eflags.CF = (src > dest);
	cpu.eflags.SF = MSB(result);
	cpu.eflags.OF = (MSB(dest) != MSB(src)) && (MSB(dest) != MSB(result));
	cpu.eflags.PF = parity;
	cpu.eflags.ZF = !result;
	for(i=0;i<8;i++){
		if(low_order_8_bits & 0x1){
			parity = !parity;
		}
		low_order_8_bits = low_order_8_bits >> 1;
	}
	print_asm_template2();
}

make_instr_helper(rm2r)
make_instr_helper(r2rm)
#if DATA_BYTE == 4 || DATA_BYTE == 2 
make_instr_helper(si2rm)
make_instr_helper(i2rm)
#endif

#include "cpu/exec/template-end.h"
