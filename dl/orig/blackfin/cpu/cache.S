/*
 * Blackfin cache control code
 *
 * Copyright 2003-2008 Analog Devices Inc.
 *
 * Enter bugs at http://blackfin.uclinux.org/
 *
 * Licensed under the GPL-2 or later.
 */

#include <asm/linkage.h>
#include <config.h>
#include <asm/blackfin.h>

.text
/* Since all L1 caches work the same way, we use the same method for flushing
 * them.  Only the actual flush instruction differs.  We write this in asm as
 * GCC can be hard to coax into writing nice hardware loops.
 *
 * Also, we assume the following register setup:
 * R0 = start address
 * R1 = end address
 */
.macro do_flush flushins:req optflushins optnopins label

	R2 = -L1_CACHE_BYTES;

	/* start = (start & -L1_CACHE_BYTES) */
	R0 = R0 & R2;

	/* end = ((end - 1) & -L1_CACHE_BYTES) + L1_CACHE_BYTES; */
	R1 += -1;
	R1 = R1 & R2;
	R1 += L1_CACHE_BYTES;

	/* count = (end - start) >> L1_CACHE_SHIFT */
	R2 = R1 - R0;
	R2 >>= L1_CACHE_SHIFT;
	P1 = R2;

.ifnb \label
\label :
.endif
	P0 = R0;
	SSYNC; 
	LSETUP (1f, 2f) LC1 = P1;
1:
.ifnb \optflushins
	\optflushins [P0];
.endif
#if ANOMALY_05000443
.ifb \optnopins
2:
.endif
	\flushins [P0++];
.ifnb \optnopins
2:	\optnopins;
.endif
	nop;	
	nop;	
#else
2:	\flushins [P0++];
#endif
	SSYNC; 
	RTS;
.endm


/* Invalidate all instruction cache lines assocoiated with this memory area */
.section .l1.text
ENTRY(_blackfin_icache_flush_range)
	do_flush IFLUSH, , nop
ENDPROC(_blackfin_icache_flush_range)

/* Flush all cache lines assocoiated with this area of memory. */
ENTRY(_blackfin_icache_dcache_flush_range)
	do_flush FLUSH, IFLUSH
ENDPROC(_blackfin_icache_dcache_flush_range)

/* Throw away all D-cached data in specified region without any obligation to
 * write them back.  Since the Blackfin ISA does not have an "invalidate"
 * instruction, we use flush/invalidate.  Perhaps as a speed optimization we
 * could bang on the DTEST MMRs ...
 */

ENTRY(_blackfin_dcache_flush_invalidate_range)
	do_flush FLUSHINV
ENDPROC(_blackfin_dcache_flush_invalidate_range)

/* Flush all data cache lines assocoiated with this memory area */
ENTRY(_blackfin_dcache_flush_range)
	do_flush FLUSH, , , .Ldfr
ENDPROC(_blackfin_dcache_flush_range)

/* copied this routine from u-boot release r0 (very old). might help, because the method is different. Added NOPs as well. Alex. 200812 */
ENTRY(_blackfin_dcache_invalidate_range)
	R2 = -32;
	R2 = R0 & R2;
	P0 = R2;
	P1 = R1;
	CSYNC;
1:
	FLUSHINV[P0++];
	NOP;
	NOP;
	NOP;
	CC = P0 < P1(iu);
	IF CC JUMP 1b(bp);

	/*
	 * If the data crosses a cache line, then we'll be pointing to
	 * the last cache line, but won't have flushed/invalidated it yet, so do
	 * one more.
	 */
	FLUSHINV[P0];
	NOP;
	NOP;
	NOP;
	SSYNC;
	RTS;
ENDPROC(_blackfin_dcache_invalidate_range)

