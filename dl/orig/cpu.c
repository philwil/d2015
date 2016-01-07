/*
 * U-boot - cpu.c CPU specific functions
 *
 * Copyright (c) 2005-2008 Analog Devices Inc.
 *
 * (C) Copyright 2000-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * Licensed under the GPL-2 or later.
 */

#include <common.h>
#include <command.h>
#include <asm/blackfin.h>
#include <asm/cplb.h>
#include <asm/mach-common/bits/core.h>
#include <asm/mach-common/bits/ebiu.h>
#include <asm/mach-common/bits/trace.h>

#include "cpu.h"
#include "serial.h"

ulong bfin_poweron_retx;

//#if defined (CONFIG_NMI_SYS)

u32 l1_text_size;
u32 l1_data_size;
//SDCS set up nmi_stack here

#ifndef CONFIG_NMI_STACK_SIZE
#define CONFIG_NMI_STACK_SIZE 512
#endif

//SDCS special nmi stack 13052011
int nmi_stack[CONFIG_NMI_STACK_SIZE];// __attribute__ ((l1_data));
int nmi_stackh; // set to point to stack head
int nmi_stackp; // set to point to stack end

extern char _stext_l1;
extern char _etext_l1;
extern char _sdata_l1;
extern char _edata_l1;
extern void bfin_run_nmi(void);
void bfin_evt_nmi(struct pt_regs *reg);
asmlinkage void evt_nmi(void);
asmlinkage void evt_nmi_sdcs(void);

/* master definition here */
nmi_t __attribute__ ((l1_data)) nmi_data;

asmlinkage void evt_12(void);
asmlinkage void evt_core(void);


//#endif

DECLARE_GLOBAL_DATA_PTR;

__attribute__ ((__noreturn__))
void cpu_init_f(ulong bootflag, ulong loaded_from_ldr)
{
#ifndef CONFIG_BFIN_BOOTROM_USES_EVT1
	/* Build a NOP slide over the LDR jump block.  Whee! */
	char nops[0xC];
	serial_early_puts("NOP Slide\n");
	memset(nops, 0x00, sizeof(nops));
	memcpy((void *)L1_INST_SRAM, nops, sizeof(nops));
#endif

	if (!loaded_from_ldr) {
		/* Relocate sections into L1 if the LDR didn't do it -- don't
		 * check length because the linker script does the size
		 * checking at build time.
		 */
		serial_early_puts("L1 Relocate\n");
//		extern char _stext_l1[], _text_l1_lma[], _text_l1_len[];
		extern char _text_l1_lma[], _text_l1_len[];
		memcpy(&_stext_l1, &_text_l1_lma, (unsigned long)_text_l1_len);
//		extern char _sdata_l1[], _data_l1_lma[], _data_l1_len[];
		extern char _data_l1_lma[],_data_l1_len[];
		memcpy(&_sdata_l1, &_data_l1_lma, (unsigned long)_data_l1_len);
	}
#if defined(__ADSPBF537__) || defined(__ADSPBF536__) || defined(__ADSPBF534__)
	/* The BF537 bootrom will reset the EBIU_AMGCTL register on us
	 * after it has finished loading the LDR.  So configure it again.
	 */
	else
		bfin_write_EBIU_AMGCTL(CONFIG_EBIU_AMGCTL_VAL);
#endif

	/* Save RETX so we can pass it while booting Linux */
	bfin_poweron_retx = bootflag;

#ifdef CONFIG_DEBUG_DUMP
	/* Turn on hardware trace buffer */
	bfin_write_TBUFCTL(TBUFPWR | TBUFEN);
#endif

#ifndef CONFIG_PANIC_HANG
	/* Reset upon a double exception rather than just hanging.
	 * Do not do bfin_read on SWRST as that will reset status bits.
	 */
	bfin_write_SWRST(DOUBLE_FAULT);
#endif

   //SDCS 13502011
   {
      int *nmi_ptr;
      nmi_stackh=&nmi_stack[0];
//      nmi_stackh=(int *) 0xFFB00100;
      nmi_stackp= nmi_stackh + ((CONFIG_NMI_STACK_SIZE-2) * sizeof(int));

//      nmi_stackp= &nmi_stack[CONFIG_NMI_STACK_SIZE-2];
      for (nmi_ptr = nmi_stackh ; nmi_ptr < nmi_stackp; nmi_ptr++) {
         *nmi_ptr=0xdeadbeef;
      }
      *nmi_ptr=0xbeadeffe;
   }
   serial_early_puts("Board init flash\n");
   board_init_f(bootflag);
}

//#if defined (CONFIG_NMI_SYS)
//#error NMI_SYS
int nmi_init(void)
{
   serial_early_puts2("Board init NMI\n");


   l1_text_size = (&_etext_l1 - &_stext_l1);
   l1_data_size = (&_edata_l1 - &_sdata_l1);
   nmi_data.myloc = (void *)&nmi_data;
   nmi_data.global = (void *)gd;

   nmi_data.l1_data_size = l1_data_size;
   nmi_data.l1_text_size = l1_text_size;
   nmi_data.nmi_trig = 0;
   nmi_data.evt2 = evt_nmi_sdcs;
   nmi_data.nmi_run = (void *)bfin_run_nmi;
   nmi_data.nmi_stack = nmi_stackh;

   return 0;
}
//#endif

int exception_init(void)
{
	bfin_write_EVT3(trap);
	return 0;
}

int core_init(void)
{
  	bfin_write_EVT6(evt_core);
	// enable the interrupt corresponding to 6 at the core level
	//*pIMASK |= (1<<6);
	bfin_write_SIC_IMASK0(bfin_read_SIC_IMASK0() |(1<<6) );


	return 0;
}

int core_exit(void)
{
	// disable the interrupt corresponding to 6 at the core level
//	*pIMASK &= ~(1<<6);
	bfin_write_SIC_IMASK0(bfin_read_SIC_IMASK0() | ~(1<<6) );

	bfin_write_EVT6(evt_default);
	return 0;
}

int evt12_init(void)
{
//	bfin_write_EVT12(evt_default);
        bfin_write_EVT12(evt_12); // in core.S
	// enable the interrupt corresponding to 12 at the core level
	//*pIMASK |= (1<<12);
	bfin_write_SIC_IMASK0(bfin_read_SIC_IMASK0() |(1<<12) );
	return 0;
}

int evt12_exit(void)
{
	// disable the interrupt corresponding to 12 at the core level
	//*pIMASK &= ~(1<<12);
	bfin_write_SIC_IMASK0(bfin_read_SIC_IMASK0() | ~(1<<12) );

	bfin_write_EVT12(evt_default);
	return 0;
}

extern long nmi_p3_save;

void save_p3(void)    //sdcs 120511
{
  asm("[--sp] = P0;");
  asm("[--sp] = R0;");
  asm("P0.l = _nmi_p3_save;");
  asm("P0.h = _nmi_p3_save;");
  asm("R0 = P3;");
  asm("[P0] = r0;");
  asm("R0 = [sp++];");
  asm("P0 = [sp++];");
  return ;
}

int irq_init(void)
{
     save_p3();   //SDCS 120511

#ifdef SIC_IMASK0
	bfin_write_SIC_IMASK0(0);
	bfin_write_SIC_IMASK1(0);
# ifdef SIC_IMASK2
	bfin_write_SIC_IMASK2(0);
# endif
#elif defined(SICA_IMASK0)
	bfin_write_SICA_IMASK0(0);
	bfin_write_SICA_IMASK1(0);
#else
	bfin_write_SIC_IMASK(0);
#endif
	/* Set up a dummy NMI handler if needed.  */
	if (CONFIG_BFIN_BOOT_MODE == BFIN_BOOT_BYPASS || ANOMALY_05000219)
		bfin_write_EVT2(evt_nmi);	/* NMI */
	bfin_write_EVT2(evt_nmi_sdcs);	/* NMI */
	bfin_write_EVT5(evt_default);	/* hardware error */
	bfin_write_EVT6(evt_default);	/* core timer */
	bfin_write_EVT7(evt_default);
	bfin_write_EVT8(evt_default);
	bfin_write_EVT9(evt_default);
	bfin_write_EVT10(evt_default);
	bfin_write_EVT11(evt_default);
	bfin_write_EVT12(evt_default);
	bfin_write_EVT13(evt_default);
	bfin_write_EVT14(evt_default);
	bfin_write_EVT15(evt_default);
	bfin_write_ILAT(0);
	CSYNC();
	/* enable hardware error irq */
	irq_flags = 0x3f;
	local_irq_enable();
	return 0;
}
