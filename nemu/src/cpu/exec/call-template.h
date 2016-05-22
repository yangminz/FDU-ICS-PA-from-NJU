#include "cpu/exec/template-start.h"

#define instr call

static void do_execute () {
	/*idex (see helper.h) is already called in the expansion of make_instr_helper 
	 *(see exec/helper.h),
	 *with its returning value passed to exec() in src/cpu/exec.c 
	 *(exec() is expanded from its helper function)
	 *eventually to cpu_exec() in src/monitor/cpu-exec.c
	 */
#if DATA_BYTE == 4 
	cpu.esp -= 4;
	//Log("cpu.eip = 0x%x", cpu.eip);
	
	if(op_src->type == OP_TYPE_IMM){
		/*relative call*/
		MEM_W(cpu.esp, cpu.eip + 5);
		cpu.eip += op_src->val;
		print_asm(str(instr) " %x", cpu.eip + 5);
	} else {
		/*absolute call*/
		MEM_W(cpu.esp, cpu.eip + 2);
		cpu.eip = op_src->val - 2;
		print_asm(str(instr) " *%s", op_src->str);
	}
#else
		panic("please implement me!");
#endif
	
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(i)
make_instr_helper(rm)
#else
#error please check call.c to see whether this template has been included incorrectly
#endif


#include "cpu/exec/template-end.h"
