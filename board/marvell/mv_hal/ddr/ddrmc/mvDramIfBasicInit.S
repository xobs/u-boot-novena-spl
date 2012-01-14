/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

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
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer.

    *   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

    *   Neither the name of Marvell nor the names of its contributors may be
        used to endorse or promote products derived from this software without
        specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#define MV_ASMLANGUAGE
#include "mvCommon.h"
#include "mvOsAsm.h"

#include "mvSysDdrConfig.h"
#include "mvBoardEnvSpec.h"
#include "mvDramIfConfig.h"
#include "mvDramIfRegs.h"
#include "pci-if/pex/mvPexRegs.h"
#include "pci-if/pci/mvPciRegs.h"
#include "mvCtrlEnvSpec.h"
#include "mvCtrlEnvAsm.h"

/* defines */

#if !defined(MV_INC_BOARD_DDIM)
.globl dramBoot1
dramBoot1:
        .word   0

/******************************************************************************
*
*
*
*
*******************************************************************************/
#if defined(DB_88F6781)
/* DDR2 MT8HTF12864AY-800E1 333MHz */
#define STATIC_SDRAM_BANK0_SIZE			0x000D0001
#define STATIC_SDRAM_BANK1_SIZE			0x000D0000
#define STATIC_SDRAM_TIMING1	     		0x47160A28
#define STATIC_SDRAM_TIMING2	     		0x547502B2
#define STATIC_SDRAM_TIMING3	     		0x20C82EE2
#define STATIC_SDRAM_TIMING4	     		0x32825050
#define STATIC_SDRAM_TIMING5     		0x00100111
#define STATIC_SDRAM_TIMING6			0x00000000
#define STATIC_SDRAM_CTRL4			0x01014005
#define STATIC_SDRAM_CTRL6			0x00000201
#define STATIC_SDRAM_CTRL7			0x0200000A
#define STATIC_SDRAM_CONFIG0          		0x00042430
#define STATIC_SDRAM_CONFIG1          		0x0

#endif

	.globl _mvDramIfStaticInit
_mvDramIfStaticInit:

	mov     r11, LR     		/* Save link register */
	mov	r10, r2

#if 0
        /* If we boot from NAND jump to DRAM sddress */
        mov     r5, #1
        ldr     r6, =dramBoot1
        str     r5, [r6]                /* We started executing from DRAM */

        ldr     r6, dramBoot1
        cmp     r6, #0
        bne     1f
#endif

	/* set all dram configuration in temp registers */
	/* Write to SDRAM timing 1 register */
	ldr	r6, = STATIC_SDRAM_TIMING1
	MV_REG_WRITE_ASM(r6, r5, SDRAM_TIMING_1_REG)
	/* Write to SDRAM timing 2 register */
	ldr	r6, = STATIC_SDRAM_TIMING2
	MV_REG_WRITE_ASM(r6, r5, SDRAM_TIMING_2_REG)
	/* Write to SDRAM timing 3 register */
	ldr	r6, = STATIC_SDRAM_TIMING3
	MV_REG_WRITE_ASM(r6, r5, SDRAM_TIMING_3_REG)
	/* Write to SDRAM timing 4 register */
	ldr	r6, = STATIC_SDRAM_TIMING4
	MV_REG_WRITE_ASM(r6, r5, SDRAM_TIMING_4_REG)
	/* Write to SDRAM timing 5 register */
	ldr	r6, = STATIC_SDRAM_TIMING5
	MV_REG_WRITE_ASM(r6, r5, SDRAM_TIMING_5_REG)
	/* Write to SDRAM timing 6 register */
	ldr	r6, = STATIC_SDRAM_TIMING6
	MV_REG_WRITE_ASM(r6, r5, SDRAM_TIMING_6_REG)
        /* Write SDRAM bank 0 size register */
	ldr	r6, = STATIC_SDRAM_BANK0_SIZE
	MV_REG_WRITE_ASM(r6, r5, SDRAM_MEM_MAP_REG(0))
        /* Write SDRAM bank 1 size register */
	ldr	r6, = STATIC_SDRAM_BANK1_SIZE
	MV_REG_WRITE_ASM(r6, r5, SDRAM_MEM_MAP_REG(1))
        /* Write SDRAM ctrl 6 register */
	ldr	r6, = STATIC_SDRAM_CTRL6
	MV_REG_WRITE_ASM(r6, r5, SDRAM_CTRL_6_REG)
        /* Write SDRAM ctrl 7 register */
	ldr	r6, = STATIC_SDRAM_CTRL7
	MV_REG_WRITE_ASM(r6, r5, SDRAM_CTRL_7_REG)
        /* Write SDRAM config 0 register */
	ldr	r6, = STATIC_SDRAM_CONFIG0
	MV_REG_WRITE_ASM(r6, r5, SDRAM_CFG_REG(0))
        /* Write SDRAM config 1 register */
	ldr	r6, = STATIC_SDRAM_CONFIG1
	MV_REG_WRITE_ASM(r6, r5, SDRAM_CFG_REG(1))
        /* Write SDRAM ctrl 4 register */
	ldr	r6, = STATIC_SDRAM_CTRL4
	MV_REG_WRITE_ASM(r6, r5, SDRAM_CTRL_4_REG)

	mov 	sp, #0

        /* SDRAM init */
        ldr	r6, = SDRAM_USRCMD_INIT;
        MV_REG_WRITE_ASM(r6, r5, SDRAM_USR_CMD_REG)

	/* Wait for init done */
        ldr     r1, =(SDRAM_STATUS_REG)
       
        /* Make sure its back to normal operation */
_sdramOpPoll1:        
        MV_REG_READ_ASM(r6, r1, SDRAM_USR_CMD_REG)
        cmp     r6, #SDRAM_STATUS_INIT_DONE
        bne     _sdramOpPoll1
1:
	mov 	r2, r10
	mov     PC, r11         	/* r11 is saved link register */

#else  /* #if !defined(MV_INC_BOARD_DDIM) */

.globl dramBoot1
dramBoot1:
        .word   0

/*******************************************************************************
* mvDramIfBasicInit - Basic initialization of DRAM interface
*
* DESCRIPTION:
*       The function will initialize the DRAM for basic usage. The function
*       will use the TWSI assembly API to extract DIMM parameters according
*       to which DRAM interface will be initialized.
*       The function referes to the following DRAM parameters:
*       1) DIMM is registered or not.
*       2) DIMM width detection.
*       3) DIMM density.
*
* INPUT:
*       r3 - required size for initial DRAM.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*       Note:
*       r4 holds I2C EEPROM address
*       r5 holds SDRAM register base address
*	r7 holds returned values
*       r8 holds SDRAM various configuration registers value.
*       r11 holds return function address.
*******************************************************************************/
/* Setting the offsets of the I2C registers */
#define NUM_OF_ROWS_OFFSET            3
#define NUM_OF_COLS_OFFSET            4
#define NUM_OF_RANKS		      5
#define SDRAM_WIDTH_OFFSET           13
#define NUM_OF_BANKS_OFFSET          17
#define SUPPORTED_CL_OFFSET          18
#define DIMM_TYPE_INFO_OFFSET        20         /* DDR2 only    */
#define SDRAM_MODULES_ATTR_OFFSET    21

#define DRAM_DEV_DENSITY_128M        0x080
#define DRAM_DEV_DENSITY_256M        0x100
#define DRAM_DEV_DENSITY_512M        0x200
       .globl _mvDramIfBasicInit
       .extern _i2cInit

_mvDramIfBasicInit:

        mov     r11, LR     		/* Save link register */

        mov     r5, #1
        ldr     r8, =dramBoot1
        str     r5, [r8]                /* We started executing from DRAM */

        /* If we boot from NAND jump to DRAM sddress */
        ldr     r8, dramBoot1
        cmp     r8, #0
        movne   pc, r11



        bl      _i2cInit                /* Initialize TWSI master             */

        /* Get default SDRAM Config values */
        MV_REG_READ_ASM (r8, r5, SDRAM_CONFIG_REG)
        bic     r8, r8, #SDRAM_DCFG_MASK

        /* Get registered/non registered info from DIMM */
	bl  	_is_Registered
        beq     nonRegistered

setRegistered:
        orr     r8, r8, #SDRAM_REGISTERED   /* Set registered bit(17)         */

nonRegistered:
	/* Get SDRAM width */
	bl 	_get_width

        orr     r6, r8, #SDRAM_DCFG_X16_DEV /* x16 devices  */
        cmp     r7, #16
        beq     setConfigReg

        orr     r6, r8, #SDRAM_DCFG_X8_DEV  /* x8 devices   */
        cmp     r7, #8
        beq     setConfigReg

        /* This is an error. return */
        b       exit_ddrAutoConfig

setConfigReg:
        mov     r8, r6
        ldr     r6, =SDRAM_CONFIG_DV
        orr     r8, r8, r6              /* Add default settings */
        mov     r6, r8                  /* Do not swap r8 content */
        MV_REG_WRITE_ASM (r6, r5, SDRAM_CONFIG_REG)

        /* Set maximum CL supported by DIMM */
	bl	_get_CAL

        /* r7 is DIMM supported CAS (e.g: 3 --> 0x1C)                         */
        clz     r6, r7
        rsb     r6, r6, #31     /* r6 = the bit number of MAX CAS supported   */

        /* Check the DDR version */
        tst     r8, #SDRAM_DTYPE_DDR2
        bne     casDdr2

casDdr1:
        ldr	r7, =3		/* stBurstDel field value	*/
	ldr     r8, =0x52       /* Assuming MAX CL = 1.5        */
        cmp     r6, #1          /* If CL = 1.5 break            */
        beq     setModeReg

        ldr	r7, =3		/* stBurstDel field value	*/
	ldr     r8, =0x22       /* Assuming MAX CL = 2          */
        cmp     r6, #2          /* If CL = 2 break              */
        beq     setModeReg

        ldr	r7, =4		/* stBurstDel field value	*/
	ldr     r8, =0x62       /* Assuming MAX CL = 2.5        */
        cmp     r6, #3          /* If CL = 2.5 break            */
        beq     setModeReg

        ldr	r7, =4		/* stBurstDel field value	*/
	ldr     r8, =0x32       /* Assuming MAX CL = 3          */
        cmp     r6, #4          /* If CL = 3 break              */
        beq     setModeReg

        ldr	r7, =5		/* stBurstDel field value	*/
	ldr     r8, =0x42       /* Assuming MAX CL = 4          */
        cmp     r6, #6          /* If CL = 4 break              */
        b       setModeReg

        b       exit_ddrAutoConfig      /* This is an error !!  */

casDdr2:
        ldr	r7, =4		/* stBurstDel field value	*/
	ldr     r8, =0x32      /* Assuming MAX CL = 3           */
        cmp     r6, #3          /* If CL = 3 break              */
        beq     casDdr2Cont

        ldr	r7, =5		/* stBurstDel field value	*/
	ldr     r8, =0x42      /* Assuming MAX CL = 4           */
        cmp     r6, #4          /* If CL = 4 break              */
        beq     casDdr2Cont

        /* CL 5 currently unsupported. We use CL 4 instead      */
        ldr	r7, =5		/* stBurstDel field value	*/
	ldr     r8, =0x42      /* Assuming MAX CL = 5           */
        cmp     r6, #5          /* If CL = 5 break              */
        beq     casDdr2Cont

        b       exit_ddrAutoConfig      /* This is an error !!  */
casDdr2Cont:
        /* Write recovery for auto-precharge relevant only in DDR2 */
        orr     r8, r8, #0x400   /* Default value */

setModeReg:
        /* The CPU must not attempt to change the SDRAM Mode register setting */
        /* prior to DRAM controller completion of the DRAM initialization     */
        /* sequence. To guarantee this restriction, it is recommended that    */
        /* the CPU sets the SDRAM Operation register to NOP command, performs */
        /* read polling until the register is back in Normal operation value, */
        /* and then sets SDRAM Mode register to it's new value.               */

	/* write 'nop' to SDRAM operation */
        mov     r6, #0x5                 /* 'NOP' command              */
        MV_REG_WRITE_ASM (r6, r5, SDRAM_OPERATION_REG)

        /* poll SDRAM operation. Make sure its back to normal operation       */
_sdramOpPoll1:
        ldr     r6, [r5]
        cmp     r6, #0                          /* '0' = Normal SDRAM Mode    */
        bne     _sdramOpPoll1

        /* Now its safe to write new value to SDRAM Mode register             */
        MV_REG_WRITE_ASM (r8, r5, SDRAM_MODE_REG)

        /* Make the Dunit write the DRAM its new mode                         */
        mov     r6, #0x3                 /* Mode Register Set command  */
        MV_REG_WRITE_ASM (r6, r5, SDRAM_OPERATION_REG)

        /* poll SDRAM operation. Make sure its back to normal operation       */
_sdramOpPoll2:
        ldr     r6, [r5]
        cmp     r6, #0                          /* '0' = Normal SDRAM Mode    */
        bne     _sdramOpPoll2

	/* Set Dunit control register according to max CL detected	      */
	/* If we use registered DIMM, add 1 to stBurstDel		      */
        MV_REG_READ_ASM (r6, r5, SDRAM_CONFIG_REG)
	tst	r6, #SDRAM_REGISTERED
	beq	setDunitReg
	add	r7, r7, #1

setDunitReg:
        ldr     r6, =SDRAM_DUNIT_CTRL_LOW_DV
        orr	r6, r6, r7, LSL #SDRAM_ST_BURST_DEL_OFFS
        MV_REG_WRITE_ASM (r6, r5, SDRAM_DUNIT_CTRL_REG)


        /* DIMM density configuration*/
        /* Density = (1 << (rowNum + colNum)) * dramWidth * dramBankNum       */
Density:
	bl 	_getDensity
	mov 	r8, r7
        mov     r8, r8, LSR #20 /* Move density 20 bits to the right  */
                                /* For example 0x10000000 --> 0x1000 */

        mov     r6, #0x00
        cmp     r8, #DRAM_DEV_DENSITY_128M
        beq     densCont

        mov     r6, #0x10
        cmp     r8, #DRAM_DEV_DENSITY_256M
        beq     densCont

        mov     r6, #0x20
        cmp     r8, #DRAM_DEV_DENSITY_512M
        beq     densCont

        /* This is an error. return */
        b       exit_ddrAutoConfig

densCont:
        MV_REG_WRITE_ASM (r6, r5, SDRAM_ADDR_CTRL_REG)

        /* Config DDR2 registers (Extended mode, ODTs and pad calibration)    */
        MV_REG_READ_ASM (r8, r5, SDRAM_CONFIG_REG)
        tst     r8, #SDRAM_DTYPE_DDR2
        beq     _extModeODTEnd


	/* Set DDR Extended Mode register for working with CS[0]	      */
        /* write 'nop' to SDRAM operation */
        mov     r6, #0x5                 /* 'NOP' command              */
        MV_REG_WRITE_ASM (r6, r5, SDRAM_OPERATION_REG)

        /* poll SDRAM operation. Make sure its back to normal operation       */
_sdramOpPoll3:
        ldr     r6, [r5]
        cmp     r6, #0                          /* '0' = Normal SDRAM Mode    */
        bne     _sdramOpPoll3

        /* Now its safe to write new value to SDRAM Extended Mode register    */
        ldr	r6, =DDR_SDRAM_EXT_MODE_CS0_DV
        MV_REG_WRITE_ASM (r6, r5, SDRAM_EXTENDED_MODE_REG)

        /* Make the Dunit write the DRAM its new extended mode                */
        mov     r6, #0x4                /* Extended Mode Register Set command */
        MV_REG_WRITE_ASM (r6, r5, SDRAM_OPERATION_REG)

        /* poll SDRAM operation. Make sure its back to normal operation       */
_sdramOpPoll4:
        ldr     r6, [r5]
        cmp     r6, #0                          /* '0' = Normal SDRAM Mode    */
        bne     _sdramOpPoll4

	/* ODT configuration is done for single bank CS[0] only		      */
        /* Config DDR2 On Die Termination (ODT) registers */
        ldr     r6, =DDR2_ODT_CTRL_LOW_CS0_DV
        MV_REG_WRITE_ASM (r6, r5, DDR2_SDRAM_ODT_CTRL_LOW_REG)

        ldr     r6, =DDR2_ODT_CTRL_HIGH_CS0_DV
        MV_REG_WRITE_ASM (r6, r5, DDR2_SDRAM_ODT_CTRL_HIGH_REG)

        ldr     r6, =DDR2_DUNIT_ODT_CTRL_CS0_DV
        MV_REG_WRITE_ASM (r6, r5, DDR2_DUNIT_ODT_CONTROL_REG)


        /* we will check what device we are running and perform
        Initialization according to device value */

_extModeODTEnd:

setTimingReg:
        /* Set default Timing parameters */
        MV_REG_READ_ASM (r8, r5, SDRAM_CONFIG_REG)
        tst     r8, #SDRAM_DTYPE_DDR2
        bne     ddr2TimeParam

ddr1TimeParam:
        ldr     r6, =DDR1_TIMING_LOW_DV
        MV_REG_WRITE_ASM (r6, r5, SDRAM_TIMING_CTRL_LOW_REG)
        ldr     r6, =DDR1_TIMING_HIGH_DV
        MV_REG_WRITE_ASM (r6, r5, SDRAM_TIMING_CTRL_HIGH_REG)
        b       timeParamDone

ddr2TimeParam:
        ldr     r6, =DDR2_TIMING_LOW_DV
        MV_REG_WRITE_ASM (r6, r5, SDRAM_TIMING_CTRL_LOW_REG)
        ldr     r6, =DDR2_TIMING_HIGH_DV
        MV_REG_WRITE_ASM (r6, r5, SDRAM_TIMING_CTRL_HIGH_REG)

timeParamDone:
        /* Open CS[0] window to requested size and enable it. Disable other   */
	/* windows 							      */
        ldr	r6, =SCBAR_BASE_MASK
        sub     r3, r3, #1
        and	r3, r3, r6
	orr	r3, r3, #1	/* Enable bank */
        MV_REG_WRITE_ASM (r3, r5, SDRAM_SIZE_REG(0))
        ldr	r6, =0
        MV_REG_WRITE_ASM (r6, r5, SDRAM_SIZE_REG(1))
        MV_REG_WRITE_ASM (r6, r5, SDRAM_SIZE_REG(2))
        MV_REG_WRITE_ASM (r6, r5, SDRAM_SIZE_REG(3))

exit_ddrAutoConfig:
        mov     PC, r11         /* r11 is saved link register */


/***************************************************************************************/
/*       r4 holds I2C EEPROM address
 *       r7 holds I2C EEPROM offset parameter for i2cRead and its --> returned value
 *       r8 holds SDRAM various configuration registers value.
 *	r13 holds Link register
 */
/**************************/
_getDensity:
	mov     r13, LR                            /* Save link register */

        mov     r4, #MV_BOARD_DIMM0_I2C_ADDR       /* reading from DIMM0      */
        mov     r7, #NUM_OF_ROWS_OFFSET            /* offset  3               */
        bl      _i2cRead
        mov     r8, r7                             /* r8 save number of rows  */

        mov     r4, #MV_BOARD_DIMM0_I2C_ADDR       /* reading from DIMM0      */
        mov     r7, #NUM_OF_COLS_OFFSET            /* offset  4               */
        bl      _i2cRead
        add     r8, r8, r7                         /* r8 = number of rows + number of col */

        mov     r7, #0x1
        mov     r8, r7, LSL r8                     /* r8 = (1 << r8)          */

        mov     r4, #MV_BOARD_DIMM0_I2C_ADDR       /* reading from DIMM0      */
        mov     r7, #SDRAM_WIDTH_OFFSET            /* offset 13 */
        bl      _i2cRead
        mul     r8, r7, r8

        mov     r4, #MV_BOARD_DIMM0_I2C_ADDR       /* reading from DIMM0      */
        mov     r7, #NUM_OF_BANKS_OFFSET           /* offset 17               */
        bl      _i2cRead
        mul     r7, r8, r7

	mov     PC, r13

/**************************/
_get_width:
	mov     r13, LR                 /* Save link register */

        /* Get SDRAM width (SPD offset 13) */
        mov     r4, #MV_BOARD_DIMM0_I2C_ADDR /* reading from DIMM0            */
        mov     r7, #SDRAM_WIDTH_OFFSET
        bl      _i2cRead                /* result in r7                       */

	mov     PC, r13

/**************************/
_get_CAL:
	mov     r13, LR                 /* Save link register */

        /* Set maximum CL supported by DIMM */
        mov     r4, #MV_BOARD_DIMM0_I2C_ADDR /* reading from DIMM0            */
        mov     r7, #SUPPORTED_CL_OFFSET     /* offset  18 */
        bl      _i2cRead

	mov     PC, r13

/**************************/
/* R8 - sdram configuration register.
 * Return value in flag if no-registered then Z-flag is set
 */
_is_Registered:
	mov     r13, LR                 /* Save link register */

        /* Get registered/non registered info from DIMM */
        tst     r8, #SDRAM_DTYPE_DDR2
        bne     regDdr2

regDdr1:
        mov     r4, #MV_BOARD_DIMM0_I2C_ADDR /* reading from DIMM0            */
        mov     r7, #SDRAM_MODULES_ATTR_OFFSET
        bl      _i2cRead                /* result in r7                       */
        tst     r7, #0x2
	b	exit
regDdr2:
        mov     r4, #MV_BOARD_DIMM0_I2C_ADDR /* reading from DIMM0            */
        mov     r7, #DIMM_TYPE_INFO_OFFSET
        bl      _i2cRead                /* result in r7                       */
        tst     r7, #0x11               /* DIMM type = regular RDIMM (0x01)   */
                                        /* or Mini-RDIMM (0x10)               */
exit:
        mov     PC, r13


#endif
