#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute() {
	DATA_TYPE dest = op_dest->val, src = op_src->val;
	DATA_TYPE result = dest - src;
	uint8_t low_order_8_bits = result & 0xff;
	int parity = 1;
	int i = 0;
	cpu.eflags.CF = (src > dest);

	for(i=0;i<8;i++){
		if(low_order_8_bits & 0x1){
			parity = !parity;
		}
		low_order_8_bits = low_order_8_bits >> 1;
	}
	cpu.eflags.PF = parity;
	cpu.eflags.ZF = !result;
	cpu.eflags.SF = MSB(result);
	cpu.eflags.OF = (MSB(dest) != MSB(src)) && (MSB(dest) != MSB(result));
	print_asm_template2();
}

make_instr_helper(i2a);
make_instr_helper(i2rm);
make_instr_helper(r2rm);
make_instr_helper(rm2r);
#if DATA_BYTE == 4 || DATA_BYTE == 2 
make_instr_helper(si2rm)
#endif

#include "cpu/exec/template-end.h"