#include "cpu/exec/template-start.h"

#define instr test

static void do_execute () {
	DATA_TYPE result = op_dest->val & op_src->val;
	uint8_t low_order_8_bits = result & 0xff;

	cpu.eflags.ZF = !result;
	cpu.eflags.SF = MSB(result);
	cpu.eflags.OF = 0;
	
	int parity = 1;
	int i = 0;
	cpu.eflags.CF = 0;
	for(i=0;i<8;i++){
		if(low_order_8_bits & 0x1){
			parity = !parity;
		}
		low_order_8_bits = low_order_8_bits >> 1;
	}
	cpu.eflags.PF = parity;

	print_asm_template2();
}

make_instr_helper(r2rm)
make_instr_helper(i2a)
make_instr_helper(i2rm)

#include "cpu/exec/template-end.h"
