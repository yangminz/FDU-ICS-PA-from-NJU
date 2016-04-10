#ifndef __REG_H__
#define __REG_H__

#include "common.h"

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };

/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

typedef struct {
	union{
		union{
			uint32_t _32;
			uint16_t _16;
			uint8_t _8[2];
		} gpr[8];

	/* Do NOT change the order of the GPRs' definitions. */
		/* yangmin:
		 * Method to work this out:
		 * understand the construction of register in cpu
		 * (1)
		 * refer to nemu\src\cpu\reg.c to understand the meaning of tests in it
		 * e.g.
		 * assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
		 * reg_b is a marco exp below in this file: 
		 * #define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])
		 * thus we rewrite the test sentence:
		 * cpu.gpr[check_reg_index( R_AL ) & 0x3]._8[R_AL >> 2] == (sample[R_EAX] & 0xffff)
		 * And note that there is no expr like cpu.al, cpu.bx in reg.c
		 * So 16 bits and 8 bits registers are all in gpr[8]
		 * (2)
		 * note the following sentence:
		 * assert(sample[R_EAX] == cpu.eax);
		 * consider the sequence of assignment in reg.c:
		 * rand() -> sample[i] -> reg_l(i) == cpu.gpr[check_reg_index( i )]._32
		 * there is no assignment for cpu.eax
		 * So cpu.eax and the only assigned variable cpu.gpr[ whatever ]._32 must be unioned
		 */
		struct{
			uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
		};
	};

	swaddr_t eip;
	/* yangmin:
	 * typedef uint32_t swaddr_t;	--	from #include "common.h"
	 * register eip is the address of the instruction
	 * that cpu is going to execute
	 * For further understanding, refer to nemu\src\monitor\cpu_exec.c
	 */

} CPU_state;

extern CPU_state cpu;

static inline int check_reg_index(int index) {
	assert(index >= 0 && index < 8);
	return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])

extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];

#endif
