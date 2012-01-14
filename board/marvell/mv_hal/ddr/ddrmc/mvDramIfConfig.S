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

/*******************************************************************************
* mvDramIfBasicAsm.s
*
* DESCRIPTION:
*       Memory full detection and best timing configuration is done in
*       C code. C runtime environment requires a stack. This module API
*       initialize DRAM interface chip select 0 for basic functionality for
*       the use of stack.
*       The module API assumes DRAM information is stored in I2C EEPROM reside
*       in a given I2C address MV_BOARD_DIMM0_I2C_ADDR. The I2C EEPROM
*       internal data structure is assumed to be orgenzied in common DRAM
*       vendor SPD structure.
*       NOTE: DFCDL values are assumed to be already initialized prior to
*       this module API activity.
*
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

/* includes */
#define MV_ASMLANGUAGE
#include "mvOsAsm.h"
#include "mvSysDdrConfig.h"
#include "mvDramIfRegs.h"
#include "mvDramIfConfig.h"
#include "pci-if/pex/mvPexRegs.h"
#include "pci-if/pci/mvPciRegs.h"
#include "mvCtrlEnvSpec.h"
#include "mvCtrlEnvAsm.h"
#include "mvCommon.h"

/* defines  */

/* locals   */
.data
.globl _mvDramIfConfig

.text

/*******************************************************************************
* _mvDramIfConfig - Basic DRAM interface initialization.
*
* DESCRIPTION:
*       The function will initialize the following DRAM parameters using the
*       values prepared by mvDramIfDetect routine. Values are located
*       in predefined registers.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/

_mvDramIfConfig:

        /* Save register on stack */
	cmp	sp, #0
	beq	no_stack_s
save_on_stack:
        stmdb	sp!, {r1, r2, r3, r4, r7, r11}
no_stack_s:

	/* Write to SDRAM timing 1 register */
        ldr     r1, =(DRAM_BUF_REG1)
        ldr     r4, [r1]
        ldr     r1, =(SDRAM_TIMING_1_REG)
        str     r4, [r1]

	/* Write to SDRAM timing 2 register */
        ldr     r1, =(DRAM_BUF_REG2)
        ldr     r4, [r1]
        ldr     r1, =(SDRAM_TIMING_2_REG)
        str     r4, [r1]

	/* Write to SDRAM timing 3 register */
        ldr     r1, =(DRAM_BUF_REG3)
        ldr     r4, [r1]
        ldr     r1, =(SDRAM_TIMING_3_REG)
        str     r4, [r1]

	/* Write to SDRAM timing 4 register */
        ldr     r1, =(DRAM_BUF_REG4)
        ldr     r4, [r1]
        ldr     r1, =(SDRAM_TIMING_4_REG)
        str     r4, [r1]

	/* Write to SDRAM timing 5 register */
        ldr     r1, =(DRAM_BUF_REG5)
        ldr     r4, [r1]
        ldr     r1, =(SDRAM_TIMING_5_REG)
        str     r4, [r1]

	/* Write to SDRAM timing 6 register */
        ldr     r1, =(DRAM_BUF_REG6)
        ldr     r4, [r1]
        ldr     r1, =(SDRAM_TIMING_6_REG)
        str     r4, [r1]

        /* Write SDRAM bank 0 size register */
        ldr     r1, =(DRAM_BUF_REG0)
        ldr     r4, [r1]
        ldr     r1, =(SDRAM_MEM_MAP_REG(0))
        str     r4, [r1]

        /* Write SDRAM ctrl 6 register */
        ldr     r1, =(DRAM_BUF_REG7)
        ldr     r4, [r1]
        ldr     r1, =(SDRAM_CTRL_6_REG)
        str     r4, [r1]

        /* Write SDRAM ctrl 7 register */
        ldr     r1, =(DRAM_BUF_REG8)
        ldr     r4, [r1]
        ldr     r1, =(SDRAM_CTRL_7_REG)
        str     r4, [r1]

        /* Write SDRAM config 0 register */
        ldr     r1, =(DRAM_BUF_REG9)
        ldr     r4, [r1]
        ldr     r1, =(SDRAM_CFG_REG(0))
        str     r4, [r1]

        /* Write SDRAM config 1 register */
        ldr     r1, =(DRAM_BUF_REG9)
        ldr     r4, [r1]
        ldr     r1, =(SDRAM_CFG_REG(1))
        str     r4, [r1]

        /* Write SDRAM ctrl 4 register */
        ldr     r1, =(DRAM_BUF_REG10)
        ldr     r4, [r1]
        ldr     r1, =(SDRAM_CTRL_4_REG)
        str     r4, [r1]

        /* SDRAM init */
        ldr     r1, = SDRAM_USRCMD_INIT;
        ldr     r1, =(SDRAM_USR_CMD_REG)
        str     r4, [r1]

	/* Wait for init done */
        ldr     r1, =(SDRAM_STATUS_REG)
       
        /* Make sure its back to normal operation */
_sdramOpPoll1:        
        ldr     r4, [r1]
        cmp     r4, #SDRAM_STATUS_INIT_DONE
        bne     _sdramOpPoll1

        /* Write SDRAM mode register */
        ldr     r1, = (SDRAM_USRCMD_MRS_INIT | SDRAM_USRCMD_CS0_MASK | SDRAM_USRCMD_CS1_MASK);
        ldr     r1, =(SDRAM_USR_CMD_REG)
        str     r4, [r1]

        /* Write SDRAM extended mode register 1*/
        ldr     r1, = (SDRAM_USRCMD_EMRS1_INIT | SDRAM_USRCMD_CS0_MASK | SDRAM_USRCMD_CS1_MASK);
        ldr     r1, =(SDRAM_USR_CMD_REG)
        str     r4, [r1]

        /* Write SDRAM extended mode register 2*/
        ldr     r1, = (SDRAM_USRCMD_EMRS2_INIT | SDRAM_USRCMD_CS0_MASK | SDRAM_USRCMD_CS1_MASK);
        ldr     r1, =(SDRAM_USR_CMD_REG)
        str     r4, [r1]

        /* Write SDRAM extended mode register 3*/
        ldr     r1, = (SDRAM_USRCMD_EMRS3_INIT | SDRAM_USRCMD_CS0_MASK | SDRAM_USRCMD_CS1_MASK);
        ldr     r1, =(SDRAM_USR_CMD_REG)
        str     r4, [r1]

        /* Save register on stack */
	cmp	sp, #0
	beq	no_stack_l
load_from_stack:
        ldmia	sp!, {r1, r2, r3, r4, r7, r11}
no_stack_l:

        mov     pc, lr

