/*
 * core.S - trampoline default exceptions/interrupts to C handlers
 *
 * Copyright (c) 2005-2007 Analog Devices Inc.
 * Licensed under the GPL-2 or later.
 */

#include <asm/blackfin.h>
#include <asm/entry.h>

.text

/* core timer entry point  */
ENTRY(_evt_core)
	SAVE_ALL_SYS
	r0 = sp;	/* stack frame pt_regs pointer argument ==> r0 */
	sp += -12;
/*	call _bfin_core; */
	sp += 12;
	RESTORE_ALL_SYS
	rti;
ENDPROC(_evt_core)

	/* evt12 entry point   dont think this is used SDCS 1205 */
ENTRY(_evt_12)
	SAVE_ALL_SYS
	r0 = sp;	/* stack frame pt_regs pointer argument ==> r0 */
	sp += -12;
/*        call _bfin_evt12; */
	call _bfin_evt_nmi; 
	sp += 12;
	RESTORE_ALL_SYS
	rti;
ENDPROC(_evt_12)

