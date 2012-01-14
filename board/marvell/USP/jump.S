/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File in accordance with the terms and conditions of the General 
Public License Version 2, June 1991 (the "GPL License"), a copy of which is 
available along with the File in the license.txt file or by writing to the Free 
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or 
on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY 
DISCLAIMED.  The GPL License provides additional details about this warranty 
disclaimer.

*******************************************************************************/
#define MV_ASMLANGUAGE
#include "mvSysHwConfig.h"
#include "mvBoardEnvSpec.h"
#include "mvOsAsm.h"
#include "pci-if/pci/mvPciRegs.h"
#include "pci-if/pex/mvPexRegs.h"
#include "mvCtrlEnvSpec.h"
#include "mvCtrlEnvAsm.h"
#include "sys/mvCpuIfRegs.h"

.section	".reset_vector_sect",#alloc, #execinstr
jumpStart:
#if defined(MV_88F6082) || defined(MV_88F6183) || defined(DB_88F5181_OLD) || defined(DB_FPGA) || \
	defined(MV88F6281) || defined(MV88F6192) || defined(MV88F6180)  || defined(MV_88F6183L)
#if defined(__BE)
	/* disable I-Cache */
	.word 0x100f11ee /* mrc     15, 0, r0, cr1, cr0, {0} */
	.word 0x010ac0e3 /* bic     r0, r0, #4096   ; 0x1000 */
	.word 0x0700c0e3 /* bic     r0, r0, #7      ; 0x7 */
	.word 0x020080e3 /* orr     r0, r0, #2      ; 0x2 */
	.word 0x100f01ee /* mcr     15, 0, r0, cr1, cr0, {0} */
	/* disable L2 prefetch */
	.word 0x110f3fee /* mrc p15, 1, r0, c15, c1 */
	.word 0x010480e3 /* orr r0, r0, #(1<<24) */
	.word 0x110f2fee /* mcr p15, 1, r0, c15, c1 */
/* convert CPU to big endian */
	.word 0x100f11ee /* mrc p15, 0, r0, c1, c0 */
	.word 0x800080e3 /* orr r0, r0, #0x80 */
	.word 0x100f01ee /* mcr p15, 0, r0, c1, c0 */
	nop;nop;nop;nop;
	nop;nop;nop;nop;

#endif
#endif

	/* Check if we booted from DRAM. If we did someone already      */
	/* initialize the DRAM controller                               */

	adr	r4, jumpStart		/* r4 <- current position of code   */
	ldr	r5, =~0xff
	and	r4, r4, r5
	ldr     r5, __start		/* r5 <- linker results for _start   */
	ldr     r2, _jumpStart		/* r2 <- linker results reset vector */
	sub	r8, r2, r5		/* r8 <- (reset vector address - start address) */
	sub	r8, r4, r8		/* r8 <- absolute address to jump to */
					/* r8 <- (current code address -      */
	ldr	sp, =0
	ldr	lr, =0 
	ldr	r5, =CONFIG_SYS_RESET_ADDRESS 	/* test if we run from flash or RAM */
	cmp     r4, r5                  /* don't reloc during debug         */
	beq     romBoot

	mov     r5, #1
	ldr     r4, =dramBoot
	str     r5, [r4]                /* We started executing from DRAM */
romBoot:

device_cont:

	mov lr, r8
	mov pc, lr



__start:
	.word  _start

.globl dramBoot
dramBoot:
	.word   0

_jumpStart:
	.word   .reset_vector_sect

.section	".dummy",#alloc, #execinstr
	.long 0xffffffff
	.long 0xffffffff
	.long 0xffffffff
	.long 0xffffffff
