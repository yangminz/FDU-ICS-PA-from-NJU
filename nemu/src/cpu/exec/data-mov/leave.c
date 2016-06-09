#include "cpu/exec/helper.h"

make_helper(leave) {
	cpu.esp = cpu.ebp;
	if(ops_decoded.is_data_size_16){
		panic("please implement me!");
	}
	else {
		cpu.ebp = swaddr_read(cpu.esp, 4);
		cpu.esp += 4;
	}
	print_asm("leave");
	return 1;
}