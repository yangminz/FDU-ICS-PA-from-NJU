#include "cpu/exec/helper.h"

static void do_ret() {
	cpu.eip = swaddr_read(cpu.esp, 4);
	cpu.esp += 4;
	print_asm("ret");
}

make_helper(ret) {
	static int len = 1;
	do_ret(cpu.eip);
	cpu.eip -= len;
	return len;
}
