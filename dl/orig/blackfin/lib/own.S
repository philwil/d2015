/*
 
 */

#include <config.h>
#include <asm/blackfin.h>
#include <asm/mach-common/bits/core.h>
#include <asm/mach-common/bits/dma.h>
#include <asm/mach-common/bits/pll.h>



	
	/* 	own code to store the value of the program counter in the return variable. 
		In the c-code create a global variable "unsigned long ownpc=0;"
		use "ownpc = catch_pc();" and watch "ownpc" to see the address. 
		This technique was used to check if functions were really accessed by a jump rather than inlined by the compiler */
.text
ENTRY(_catch_pc)
	r0 = rets;
#if ANOMALY_05000371
	NOP;
	NOP;
	NOP;
#endif
	rts;
ENDPROC(_catch_pc)



.text
ENTRY(_jump_call)
	sp += -12;

	[--sp] = rets;
	[--sp] = p2;

	p2 = r0;
	call(p2);
	p2 = [sp++];
	rets = [sp++];
	sp += 12;

	rts;
ENDPROC(_jump_call)



