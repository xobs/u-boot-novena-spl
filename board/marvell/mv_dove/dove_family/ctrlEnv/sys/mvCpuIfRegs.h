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


#ifndef __INCmvCpuIfRegsh
#define __INCmvCpuIfRegsh

/****************************************/
/* ARM Control and Status Registers Map */
/****************************************/
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "ctrlEnv/mvCtrlEnvRegs.h"

#define CPU_CONFIG_REG				(MV_CPUIF_REGS_BASE + 0x100)
#define CPU_CTRL_STAT_REG			(MV_CPUIF_REGS_BASE + 0x104)
#define CPU_RSTOUTN_MASK_REG			(MV_CPUIF_REGS_BASE + 0x108)
#define CPU_SYS_SOFT_RST_REG			(MV_CPUIF_REGS_BASE + 0x10C)
#define CPU_AHB_MBUS_CAUSE_INT_REG		(MV_CPUIF_REGS_BASE + 0x110)
#define CPU_AHB_MBUS_MASK_INT_REG		(MV_CPUIF_REGS_BASE + 0x114)
#define CPU_FTDLL_CONFIG_REG			(MV_CPUIF_REGS_BASE + 0x120)

#define AXI_P_D_CTRL_REG			(MV_MISC_REGS_BASE + 0x224)
#define CPU_CONTROL_REG				(MV_MISC_REGS_BASE + 0x25C)
#define CPU_CTRL_PMU_CPU_RST_EN_OFFS		1
#define CPU_CTRL_PMU_CPU_RST_EN_MASK		(0x1 << CPU_CTRL_PMU_CPU_RST_EN_OFFS)

/* Debug Register */

/***************************/
/* BOOTROM_ROUTINE_ERR_REG */
/***************************/
#define BOOTROM_ROUTINE_ERR_REG			(MV_MISC_REGS_BASE + 0x2D8)

/* Error Code , 0 - Pass, Non-Zero - Fail*/
#define BRE_ERR_CODE_OFFS				0
#define BRE_ERR_CODE_MASK				(0xFF << BRE_ERR_CODE_OFFS)
#define NO_ERR              				(0x00 << BRE_ERR_CODE_OFFS)
#define ERROR_INVALID_HEADER_ID				(0x11 << BRE_ERR_CODE_OFFS)
#define ERROR_INVALID_HDR_CHKSUM			(0x12 << BRE_ERR_CODE_OFFS)
#define ERROR_INVALID_IMG_CHKSUM			(0x13 << BRE_ERR_CODE_OFFS)
#define ERROR_UNSUPPORTED_EXT_HDR			(0x14 << BRE_ERR_CODE_OFFS)
#define ERROR_INVALID_EXT_HDR_CHKSUM			(0x15 << BRE_ERR_CODE_OFFS)
#define ERROR_BUSY			   		(0x17 << BRE_ERR_CODE_OFFS)
#define ERROR_LINK			   		(0x18 << BRE_ERR_CODE_OFFS)
#define ERROR_DMA			   		(0x19 << BRE_ERR_CODE_OFFS)
#define ERROR_PIO			   		(0x1a << BRE_ERR_CODE_OFFS)
#define ERROR_TIMEOUT					(0x1b << BRE_ERR_CODE_OFFS)
#define ERROR_UNSUPPORTED_BOOT 				(0x1d << BRE_ERR_CODE_OFFS)
#define ERROR_ALIGN_SIZE				(0x21 << BRE_ERR_CODE_OFFS)
#define ERROR_ALIGN_SRC					(0x22 << BRE_ERR_CODE_OFFS)
#define ERROR_ALIGN_DEST				(0x23 << BRE_ERR_CODE_OFFS)
#define ERROR_INVALID_BOOTROM_CHKSUM			(0x26 << BRE_ERR_CODE_OFFS)
#define ERROR_NAND_READ_FAIL				(0x30 << BRE_ERR_CODE_OFFS)
#define ERROR_NAND_NOT_READY				(0x31 << BRE_ERR_CODE_OFFS)
#define ERROR_NAND_IMAGE_NOT_ALIGN_256			(0x32 << BRE_ERR_CODE_OFFS)
#define ERROR_NAND_ECC_ERROR				(0x33 << BRE_ERR_CODE_OFFS)
#define ERROR_NAND_TIMEOUT				(0x34 << BRE_ERR_CODE_OFFS)
#define ERROR_NAND_INVALID_OPTION			(0x35 << BRE_ERR_CODE_OFFS)
#define ERROR_NAND_EXCESS_BAD_BLKS			(0x36 << BRE_ERR_CODE_OFFS)
#define ERROR_NAND_BAD_BLK				(0x37 << BRE_ERR_CODE_OFFS)
#define ERROR_I2C_READ_ERROR				(0x40 << BRE_ERR_CODE_OFFS)
#define ERROR_I2C_WRITE_ERROR				(0x41 << BRE_ERR_CODE_OFFS)
#define ERROR_I2C_START_ERROR				(0x42 << BRE_ERR_CODE_OFFS)
#define ERROR_I2C_STOP_ERROR				(0x43 << BRE_ERR_CODE_OFFS)
#define ERROR_I2C_NO_HDR_FOUND_ERROR			(0x44 << BRE_ERR_CODE_OFFS)
/* Routine Number */
#define BRE_RTN_NUM_OFFS				8
#define BRE_RTN_NUM_MASK				(0xF << BRE_RTN_NUM_OFFS)
#define BRE_RTN_UART_BOOT				(0x1 << BRE_RTN_NUM_OFFS)
#define BRE_RTN_SATA_BOOT				(0x4 << BRE_RTN_NUM_OFFS)
#define BRE_RTN_SPI_BOOT				(0x7 << BRE_RTN_NUM_OFFS)
#define BRE_RTN_NAND_BOOT				(0x9 << BRE_RTN_NUM_OFFS)
#define BRE_RTN_EXCEP					(0xb << BRE_RTN_NUM_OFFS)
#define BRE_RTN_EXEC					(0xc << BRE_RTN_NUM_OFFS)
#define BRE_RTN_I2C					(0xd << BRE_RTN_NUM_OFFS)
#define BRE_RTN_PEX_BOOT				(0xe << BRE_RTN_NUM_OFFS)
#define BRE_RTN_PM_RESUME				(0xf << BRE_RTN_NUM_OFFS)
/* Bit that says the DRAM is initiated */
#define BRE_DRAM_INIT_OFFS				12
#define BRE_DRAM_INIT_MASK				(0x1 << BRE_DRAM_INIT_OFFS)
/* Bit that says the redundant image was tried */
#define BRE_REDUNDANT_IMG_OFFS				13
#define BRE_REDUNDANT_IMG_MASK				(0x1 << BRE_REDUNDANT_IMG_OFFS)
/* Retry */
#define BRE_RTN_RETRY_OFFS				16
#define BRE_RTN_RETRY_MASK				(0xF << BRE_RTN_RETRY_OFFS)
#define BRE_MAX_RETRY					(0xF << BRE_RTN_RETRY_OFFS)
#define BRE_RTN_TRACE_OFFS				20
#define BRE_RTN_TRACE_MASK				(0xFFFU << BRE_RTN_TRACE_OFFS)
#define TRACE_INIT					0x100
#define TRACE_MAIN					0x200
#define TRACE_EXEC					0x300
#define TRACE_DRAM					0x400
#define TRACE_EXTENTION					0x500
#define TRACE_UART					0x600
#define TRACE_I2C					0x700
#define TRACE_PEX					0x800
#define TRACE_SPI					0x900
#define TRACE_NAND					0xa00
#define TRACE_SATA					0xb00
#define TRACE_RDRW_RWG					0xc00
#define TRACE_NAND_ECC					0xd00
#define TRACE_PM_RESUME					0xe00


/* ARM Configuration register */
/* CPU_CONFIG_REG (CCR) */

/* Reset vector location */
#define CCR_VEC_INIT_LOC_OFFS			1
#define CCR_VEC_INIT_LOC_MASK			BIT1
/* reset at 0x00000000 */
#define CCR_VEC_INIT_LOC_0000			(0 << CCR_VEC_INIT_LOC_OFFS)
/* reset at 0xFFFF0000 */
#define CCR_VEC_INIT_LOC_FF00			(1 << CCR_VEC_INIT_LOC_OFFS)


#define CCR_AHB_ERROR_PROP_OFFS			2
#define CCR_AHB_ERROR_PROP_MASK			BIT2
/* Erros are not propogated to AHB */
#define CCR_AHB_ERROR_PROP_NO_INDICATE		(0 << CCR_AHB_ERROR_PROP_OFFS)
/* Erros are propogated to AHB */
#define CCR_AHB_ERROR_PROP_INDICATE		(1 << CCR_AHB_ERROR_PROP_OFFS)


#define CCR_ENDIAN_INIT_OFFS			3
#define CCR_ENDIAN_INIT_MASK			BIT3
#define CCR_ENDIAN_INIT_LITTLE			(0 << CCR_ENDIAN_INIT_OFFS)
#define CCR_ENDIAN_INIT_BIG			(1 << CCR_ENDIAN_INIT_OFFS)


#define CCR_INCR_EN_OFFS			4
#define CCR_INCR_EN_MASK			BIT4
#define CCR_INCR_EN				BIT4


#define CCR_NCB_BLOCKING_OFFS			5			
#define CCR_NCB_BLOCKING_MASK			(1 << CCR_NCB_BLOCKING_OFFS)
#define CCR_NCB_BLOCKING_NON			(0 << CCR_NCB_BLOCKING_OFFS)
#define CCR_NCB_BLOCKING_EN			(1 << CCR_NCB_BLOCKING_OFFS)

#define CCR_CPU_2_MBUSL_TICK_DRV_OFFS		8
#define CCR_CPU_2_MBUSL_TICK_DRV_MASK		(0xF << CCR_CPU_2_MBUSL_TICK_DRV_OFFS)
#define CCR_CPU_2_MBUSL_TICK_SMPL_OFFS		12
#define CCR_CPU_2_MBUSL_TICK_SMPL_MASK		(0xF << CCR_CPU_2_MBUSL_TICK_SMPL_OFFS)
#define CCR_ICACH_PREF_BUF_ENABLE		BIT16
#define CCR_DCACH_PREF_BUF_ENABLE		BIT17

/* Ratio options for CPU to DDR for 6281/6192/6180 */
#define CPU_2_DDR_CLK_1x3	    4
#define CPU_2_DDR_CLK_1x4	    6

/* Ratio options for CPU to DDR for 6281 only */
#define CPU_2_DDR_CLK_2x5	    3

/* Ratio options for CPU to DDR for 6192/6180 */
#define CPU_2_DDR_CLK_1x2	    2

/* Ratio options for CPU to DDR for 6180 only */
#define CPU_2_DDR_CLK_1x3_1	    0xB
#define CPU_2_DDR_CLK_1x4_1	    0xD

/* Default values for CPU to Mbus-L DDR Interface Tick Driver and 	*/
/* CPU to Mbus-L Tick Sample fields in CPU config register		*/

#define TICK_DRV_1x1	0
#define TICK_DRV_1x2	0
#define TICK_DRV_1x3	1
#define TICK_DRV_1x4	2
#define TICK_SMPL_1x1	0
#define TICK_SMPL_1x2	1
#define TICK_SMPL_1x3	2
#define TICK_SMPL_1x4	3

#define CPU_2_MBUSL_DDR_CLK_1x2						\
		 ((TICK_DRV_1x2  << CCR_CPU_2_MBUSL_TICK_DRV_OFFS) | 	\
		  (TICK_SMPL_1x2 << CCR_CPU_2_MBUSL_TICK_SMPL_OFFS))
#define CPU_2_MBUSL_DDR_CLK_1x3						\
		 ((TICK_DRV_1x3  << CCR_CPU_2_MBUSL_TICK_DRV_OFFS) | 	\
		  (TICK_SMPL_1x3 << CCR_CPU_2_MBUSL_TICK_SMPL_OFFS))
#define CPU_2_MBUSL_DDR_CLK_1x4						\
		 ((TICK_DRV_1x4  << CCR_CPU_2_MBUSL_TICK_DRV_OFFS) | 	\
		  (TICK_SMPL_1x4 << CCR_CPU_2_MBUSL_TICK_SMPL_OFFS))

/* ARM Control and Status register */
/* CPU_CTRL_STAT_REG (CCSR) */


/*
This is used to block PCI express\PCI from access Socrates/Feroceon GP
while ARM boot is still in progress
*/

#define CCSR_PCI_ACCESS_OFFS			0
#define CCSR_PCI_ACCESS_MASK			BIT0
#define CCSR_PCI_ACCESS_ENABLE			(0 << CCSR_PCI_ACCESS_OFFS)
#define CCSR_PCI_ACCESS_DISBALE			(1 << CCSR_PCI_ACCESS_OFFS)

#define CCSR_ARM_RESET				BIT1
#define CCSR_SELF_INT				BIT2
#define CCSR_BIG_ENDIAN				BIT3


/* RSTOUTn Mask Register */
/* CPU_RSTOUTN_MASK_REG (CRMR) */

#define CRMR_PEX_RST_OUT_OFFS			0
#define CRMR_PEX_RST_OUT_MASK			BIT0
#define CRMR_PEX_RST_OUT_ENABLE			(1 << CRMR_PEX_RST_OUT_OFFS)
#define CRMR_PEX_RST_OUT_DISABLE		(0 << CRMR_PEX_RST_OUT_OFFS)

#define CRMR_WD_RST_OUT_OFFS			1
#define CRMR_WD_RST_OUT_MASK			BIT1
#define CRMR_WD_RST_OUT_ENABLE			(1 << CRMR_WD_RST_OUT_OFFS)
#define CRMR_WD_RST_OUT_DISBALE			(0 << CRMR_WD_RST_OUT_OFFS)			

#define CRMR_SOFT_RST_OUT_OFFS			2
#define CRMR_SOFT_RST_OUT_MASK			BIT2
#define CRMR_SOFT_RST_OUT_ENABLE		(1 << CRMR_SOFT_RST_OUT_OFFS)
#define CRMR_SOFT_RST_OUT_DISBALE		(0 << CRMR_SOFT_RST_OUT_OFFS)

/* System Software Reset Register */
/* CPU_SYS_SOFT_RST_REG (CSSRR) */

#define CSSRR_SYSTEM_SOFT_RST			BIT0

/* AHB to Mbus Bridge Interrupt Cause Register*/
/* CPU_AHB_MBUS_CAUSE_INT_REG (CAMCIR) */

#define CAMCIR_ARM_SELF_INT			BIT0
#define CAMCIR_ARM_TIMER0_INT_REQ		BIT1
#define CAMCIR_ARM_TIMER1_INT_REQ		BIT2
#define CAMCIR_ARM_WD_TIMER_INT_REQ		BIT3


/* AHB to Mbus Bridge Interrupt Mask Register*/
/* CPU_AHB_MBUS_MASK_INT_REG (CAMMIR) */

#define CAMCIR_ARM_SELF_INT_OFFS		0
#define CAMCIR_ARM_SELF_INT_MASK		BIT0
#define CAMCIR_ARM_SELF_INT_EN			(1 << CAMCIR_ARM_SELF_INT_OFFS)
#define CAMCIR_ARM_SELF_INT_DIS			(0 << CAMCIR_ARM_SELF_INT_OFFS)


#define CAMCIR_ARM_TIMER0_INT_REQ_OFFS		1
#define CAMCIR_ARM_TIMER0_INT_REQ_MASK		BIT1
#define CAMCIR_ARM_TIMER0_INT_REQ_EN		(1 << CAMCIR_ARM_TIMER0_INT_REQ_OFFS) 
#define CAMCIR_ARM_TIMER0_INT_REQ_DIS		(0 << CAMCIR_ARM_TIMER0_INT_REQ_OFFS)

#define CAMCIR_ARM_TIMER1_INT_REQ_OFFS		2
#define CAMCIR_ARM_TIMER1_INT_REQ_MASK		BIT2
#define CAMCIR_ARM_TIMER1_INT_REQ_EN		(1 << CAMCIR_ARM_TIMER1_INT_REQ_OFFS) 
#define CAMCIR_ARM_TIMER1_INT_REQ_DIS		(0 << CAMCIR_ARM_TIMER1_INT_REQ_OFFS) 

#define CAMCIR_ARM_WD_TIMER_INT_REQ_OFFS 	3
#define CAMCIR_ARM_WD_TIMER_INT_REQ_MASK 	BIT3
#define CAMCIR_ARM_WD_TIMER_INT_REQ_EN	 	(1 << CAMCIR_ARM_WD_TIMER_INT_REQ_OFFS) 
#define CAMCIR_ARM_WD_TIMER_INT_REQ_DIS	 	(0 << CAMCIR_ARM_WD_TIMER_INT_REQ_OFFS) 

/* CPU FTDLL Config register (CFCR) fields */
#define CFCR_FTDLL_ICACHE_TAG_OFFS		0
#define CFCR_FTDLL_ICACHE_TAG_MASK		(0x7F << CFCR_FTDLL_ICACHE_TAG_OFFS)
#define CFCR_FTDLL_DCACHE_TAG_OFFS		8
#define CFCR_FTDLL_DCACHE_TAG_MASK		(0x7F << CFCR_FTDLL_DCACHE_TAG_OFFS)
#define CFCR_FTDLL_OVERWRITE_ENABLE		(1 << 15)
/* For Orion 2 D2 only */
#define CFCR_MRVL_CPU_ID_OFFS			16
#define CFCR_MRVL_CPU_ID_MASK			(0x1 << CFCR_MRVL_CPU_ID_OFFS)
#define CFCR_ARM_CPU_ID				(0x0 << CFCR_MRVL_CPU_ID_OFFS)
#define CFCR_MRVL_CPU_ID			(0x1 << CFCR_MRVL_CPU_ID_OFFS)
#define CFCR_VFP_SUB_ARC_NUM_OFFS		7
#define CFCR_VFP_SUB_ARC_NUM_MASK		(0x1 << CFCR_VFP_SUB_ARC_NUM_OFFS)
#define CFCR_VFP_SUB_ARC_NUM_1			(0x0 << CFCR_VFP_SUB_ARC_NUM_OFFS)
#define CFCR_VFP_SUB_ARC_NUM_2			(0x1 << CFCR_VFP_SUB_ARC_NUM_OFFS)


/*******************************************/
/* Main Interrupt Controller Registers Map */
/*******************************************/

#define CPU_MAIN_INT_CAUSE_REG			(MV_CPUIF_REGS_BASE + 0x200)
#define CPU_MAIN_IRQ_MASK_REG			(MV_CPUIF_REGS_BASE + 0x204)
#define CPU_MAIN_FIQ_MASK_REG			(MV_CPUIF_REGS_BASE + 0x208)
#define CPU_ENPOINT_MASK_REG			(MV_CPUIF_REGS_BASE + 0x20C)
#define CPU_MAIN_INT_CAUSE_HIGH_REG		(MV_CPUIF_REGS_BASE + 0x210)
#define CPU_MAIN_IRQ_MASK_HIGH_REG		(MV_CPUIF_REGS_BASE + 0x214)
#define CPU_MAIN_FIQ_MASK_HIGH_REG		(MV_CPUIF_REGS_BASE + 0x218)
#define CPU_ENPOINT_MASK_HIGH_REG		(MV_CPUIF_REGS_BASE + 0x21C)


/*******************************************/
/* ARM Doorbell Registers Map		   */
/*******************************************/

#define CPU_HOST_TO_ARM_DRBL_REG		(MV_CPUIF_REGS_BASE + 0x400)
#define CPU_HOST_TO_ARM_MASK_REG		(MV_CPUIF_REGS_BASE + 0x404)
#define CPU_ARM_TO_HOST_DRBL_REG		(MV_CPUIF_REGS_BASE + 0x408)
#define CPU_ARM_TO_HOST_MASK_REG		(MV_CPUIF_REGS_BASE + 0x40C)


/* CPU control register map */
/* Set bits means value is about to change according to new value */
#define CPU_CONFIG_DEFAULT_MASK         	(CCR_VEC_INIT_LOC_MASK  | CCR_AHB_ERROR_PROP_MASK)      

#define CPU_CONFIG_DEFAULT                      (CCR_VEC_INIT_LOC_FF00  )
                 
/* CPU Control and status defaults */
#define CPU_CTRL_STAT_DEFAULT_MASK              (CCSR_PCI_ACCESS_MASK)
                                                                        

#define CPU_CTRL_STAT_DEFAULT                   (CCSR_PCI_ACCESS_ENABLE)

#endif /* __INCmvCpuIfRegsh */

