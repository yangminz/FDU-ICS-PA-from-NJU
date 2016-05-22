#include "cpu/exec/template-start.h"

#define instr sub

static void do_execute() {
	/*reminder: let the parameters "passed" between functions be unsigned 
	 *(although they are not really passed, but rather global variables)*/
	DATA_TYPE dest = op_dest->val, src = op_src->val;
	DATA_TYPE result = dest - src;
	OPERAND_W(op_dest, result);
	uint8_t low_order_8_bits = result & 0xff;
	int parity = 1;
	int i = 0;
	//printf("DATA_BYTE:%d\n", DATA_BYTE);
	cpu.eflags.CF = (src > dest);

	for(i=0;i<8;i++){
		if(low_order_8_bits & 0x1){
			parity = !parity;
		}
		low_order_8_bits = low_order_8_bits >> 1;
	}
	cpu.eflags.PF = parity;
	//printf("cpu.eflags.PF: %d\n", cpu.eflags.PF);
	/*no need to update AF in nemu*/
	cpu.eflags.ZF = !result;
/*
#if DATA_BYTE == 4
#define SIGN_OF(name) (((name) & 0x80000000)== 0x80000000)		
#elif DATA_BYTE == 2
#define SIGN_OF(name) (((name) & 0x8000)== 0x8000)
	Log("DATA_BYTE is 2!");
#else
#error DATA_BYTE is not 2 or 4
#endif
	cpu.eflags.SF = SIGN_OF(result);
	cpu.eflags.OF = (SIGN_OF(dest) != SIGN_OF(src)) && (SIGN_OF(dest) != SIGN_OF(result));
#undef SIGN_OF
*/

	cpu.eflags.SF = MSB(result);
	cpu.eflags.OF = (MSB(dest) != MSB(src)) && (MSB(dest) != MSB(result));
	print_asm_template2();
}

make_instr_helper(rm2r)
make_instr_helper(r2rm)
#if DATA_BYTE == 4 || DATA_BYTE == 2 
make_instr_helper(si2rm)
make_instr_helper(i2rm)
#endif

#include "cpu/exec/template-end.h"
