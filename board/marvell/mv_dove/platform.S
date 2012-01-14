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
#include "mvOsAsm.h"
#include <config.h>
#include <version.h>
#include "mvBoardEnvSpec.h"
#include "mvCtrlEnvSpec.h"
#include "mvCpuIfRegs.h"
#include "mvCtrlEnvAsm.h"
#include "mvSysHwConfig.h"
#include "gpp/mvGppRegs.h"
#if defined(MV_INC_BOARD_SPI_FLASH)
#include "spi/mvSpiSpec.h"
#endif


/*
  get_board_id - get board id from twsi eeprom
  input : r5 - board id data start offset in the eeprom
        
*/
.globl lowlevel_init

/************************************************/
/*              lowlevel_init                   *
/************************************************/

lowlevel_init:
	/* seven segment */
	ldr	r6, =0x00008000
	MV_DV_REG_WRITE_ASM(r6, r1, 0x000D0400)
#if 0
	/* Identify number of CS in DRAM DIMM (1 or 2) - now performed in board.c */
	ldr	r1, =0x12345678
	MV_MEM_WRITE_ASM (r1, r6, 0x20000000)
	eor	r1, r1, r1
	eor	r6, r6, r6
	MV_MEM_READ_ASM (r1, r6, 0x20000000)
	ldr	r6, =0x12345678
	eor	r1, r1, r6
	cmp	r1, #0
	beq	change_reg_base
	ldr	r1, =0x000D0000
	MV_DV_REG_WRITE_ASM (r1, r6, 0x00800110)
/*	ldr	r1, =0x00000001				*/
/*	MV_DV_REG_WRITE_ASM (r1, r6, 0x00800120)	*/

	ldr	r1, =0x00500000
delay:
	sub	r1, r1, #1
	cmp	r1, #0
	bne	delay
#endif	

#if defined(MV_WRITE_LEVELING_BY_UBOOT)
	/*** DDR3 Write Leveling ***/

	/* skip if we run from DRAM (during debug) */
	MV_MEM_READ_ASM (r1, r6, 0xC8010000)
	cmp	r1, #0
	bne	change_reg_base

	/* first, identify if this is DDR3 by reading sample at reset register 0xD0214 */
	/* and checking bit 28 */
	MV_DV_REG_READ_ASM (r1, r6, 0xD0214)
	ldr	r6, =0x10000000
	and	r1, r1, r6
	cmp	r1, #0
	beq	change_reg_base
	/* OK, this is DDR3, start Write Leveling algorithm */

	/* Force ODT always on */
	ldr	r1, =0xF1000001
	MV_DV_REG_WRITE_ASM (r1, r6, 0x00800770)

	/* Disable pulldown on PHY */
	ldr	r1, =0x00113100
	MV_DV_REG_WRITE_ASM (r1, r6, 0x00800200)

	/* Enable MC write level mode */
	ldr	r1, =0x00080044
	MV_DV_REG_WRITE_ASM (r1, r6, 0x00800090)

	/* Enable write leveling for CS0 */
	ldr	r1, =0x00000004
	MV_DV_REG_WRITE_ASM (r1, r6, 0x00800080)
	ldr	r1, =0x01000200
	MV_DV_REG_WRITE_ASM (r1, r6, 0x00800120)

	/* Use the following variables: */
	/* r1 - index, r2 - value, r4 - lane, r6 - status */
	ldr	r6, =0x0	/* status = 0 */
	ldr	r4, =0x0	/* lane = 0 */
	ldr	r7, =0x0	/* used to save results of write leveling */
	ldr	r8, =0x0	/* used to save results of write leveling */
lane_loop:
	ldr	r1, =0x0	/* index = 0 */
	ldr	r2, =0x0	/* value = 0 */

	ldr	r0, =0x80000000
	MV_DV_REG_WRITE_ASM (r0, r3, 0x00800250)
	ldr	r0, =0x00000100
	add	r0, r0, r4
	MV_DV_REG_WRITE_ASM (r0, r3, 0x00800E40)

status_loop:
	mov	r2, r1, LSL #0x10
	orr	r2, r2, r1
	MV_DV_REG_WRITE_ASM (r2, r3, 0x00800E50)
	ldr	r0, =0x80000000
	MV_DV_REG_WRITE_ASM (r0, r3, 0x00800250)
	MV_DV_REG_READ_ASM (r6, r3, 0x00800250)
	mov	r3, #0x1
	mov	r3, r3, LSL r4
	and	r6, r6, r3

	/* save results of write leveling */
	mov	r7, r1

	add	r1, r1, #1	/* index = index + 1 */
	cmp	r6, r3		/* while (status != (1 << lane)) */
	bne	status_loop
/* end of status_loop */

#if defined(MV_DRAM_2CS_DIMM)
	/* use the same value for CS1: */
	ldr	r0, =0x00000200
	add	r0, r0, r4
	MV_DV_REG_WRITE_ASM (r0, r3, 0x00800E40)
	MV_DV_REG_WRITE_ASM (r2, r3, 0x00800E50)
#endif /* MV_DRAM_2CS_DIMM */

	/* save results of write leveling */
	mov	r9, r4, LSL #0x3
	orr	r8, r8, r7, LSL r9

	add	r4, r4, #1	/* lane = lane + 1 */
	cmp	r4, #4
	bne	lane_loop	/* loop until lane == 4 */
/* end of lane_loop */

	/* save results of write leveling in Crypto SRAM */
	MV_MEM_WRITE_ASM (r8, r0, 0xC8010000)

	/* Disable write leveling for CS0 */
	ldr	r1, =0x00000000
	MV_DV_REG_WRITE_ASM (r1, r6, 0x00800080)
	ldr	r1, =0x01000200
	MV_DV_REG_WRITE_ASM (r1, r6, 0x00800120)

	/* Disable MC write level mode */
	ldr	r1, =0x00080000
	MV_DV_REG_WRITE_ASM (r1, r6, 0x00800090)

	/* Enable pulldown on PHY */
	ldr	r1, =0x0011311C
	MV_DV_REG_WRITE_ASM (r1, r6, 0x00800200)

#if defined(MV_DRAM_2CS_DIMM)
	/* Enable ODT during read/write */
	ldr	r1, =0x0100000A
	MV_DV_REG_WRITE_ASM (r1, r6, 0x00800770)
#else
	/* Enable ODT during read/write */
	ldr	r1, =0x01000002
	MV_DV_REG_WRITE_ASM (r1, r6, 0x00800770)
#endif

#endif /* MV_WRITE_LEVELING_BY_UBOOT */

change_reg_base:
	/* change reg base to 0xf1000000 */
	ldr     r4, =MV_REGS
        MV_DV_REG_WRITE_ASM(r4, r1, 0x20080)

	/* Setting NB reg base to 0xf1800000 */
	MV_REG_READ_ASM (r6, r1, CPU_CONTROL_REG)
	ldr	r1, =0xFFF007FF
	and	r6, r6, r1
	ldr	r1, =0x000F1800
	orr	r6, r6, r1
	MV_REG_WRITE_ASM (r6, r1, CPU_CONTROL_REG)

	/* Setting AXI Bridge address mapping to 0xf1800000 */
	MV_REG_READ_ASM (r6, r1, AXI_P_D_CTRL_REG)
	ldr	r1, =0x007FFFFF
	and	r6, r6, r1
	ldr	r1, =0xF1800000
	orr	r6, r6, r1
	MV_REG_WRITE_ASM (r6, r1, AXI_P_D_CTRL_REG)

	/* Setting memory controller internal registers to F1800000 */
	MV_DV_REG_READ_ASM (r6, r1, 0x00800010)
	ldr	r1, =0x0000FFFF
	and	r6, r6, r1
	ldr	r1, =0xF1800000
	orr	r6, r6, r1
	MV_DV_REG_WRITE_ASM (r6, r1, 0x00800010)

        /* save Link Registers */
	mov	r2, lr

	bl      _i2cInit

        /* Initialize BUS-L to DDR configuration parameters     */
        /* Must be done prior to DDR operation                  */
#if !defined(MV_BOOTROM)
        /* bl      _mvCpuIfPreInit */
#endif

#if defined(MV_INC_BOARD_SPI_FLASH)
	/* configure the Prescale of SPI clk Tclk = 166MHz */
	MV_REG_READ_ASM (r6, r1, MV_SPI_IF_CONFIG_REG(0))
	and	r6, r6, #~MV_SPI_CLK_PRESCALE_MASK
	orr	r6, r6, #0x14
	MV_REG_WRITE_ASM (r6, r1, MV_SPI_IF_CONFIG_REG(0))
#endif

#if !defined(MV_INC_BOARD_DDIM)
#if !defined(MV_BOOTROM)

        /* Call DRAM static initialization */
	bl	_mvDramIfStaticInit
#else
	b	done
#endif
#else /* #if !defined(MV_INC_BOARD_DDIM) */

        /* Call DRAM basic initialization to allow C runtime stack */
	bl	_mvDramIfBasicInit 

	b	done
#endif /* #if !defined(MV_INC_BOARD_DDIM) */
done:
	ldr	r6, =0x0000C000
	MV_REG_WRITE_ASM(r6, r1, GPP_DATA_OUT_REG_0)

	mov	lr, r2	
	mov pc, lr
