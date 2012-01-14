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

#ifndef __INCmvCtrlEnvRegsh
#define __INCmvCtrlEnvRegsh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "ctrlEnv/mvCtrlEnvSpec.h"

#define MV_CPUIF_REGS_BASE			(MV_CPUIF_REGS_OFFSET + INTER_REGS_BASE)
#define MV_MISC_REGS_BASE			(MV_MISC_REGS_OFFSET + INTER_REGS_BASE)

/* CV Support */
#define PEX0_MEM0 	PEX0_MEM
#define PEX1_MEM1 	PEX1_MEM
#define PCI0_MEM0	PEX0_MEM
#define PCI1_MEM1	PEX1_MEM

/* Controller revision info */
#define PCI_CLASS_CODE_AND_REVISION_ID			    0x008
#define PCCRIR_REVID_OFFS				    0		/* Revision ID */
#define PCCRIR_REVID_MASK				    (0xff << PCCRIR_REVID_OFFS)

/* Controler environment registers offsets */

/* Power Managment Control */ 

#define POWER_MNG_CTRL_REG			(MV_CPUIF_REGS_BASE + 0x11C)

/* Software Reset Control Register */
#define SW_RESET_CTRL_REG			(MV_MISC_REGS_BASE + 0x030)

#define SRC_GBE_SW_RESET_BIT			2

/* Clock Gating Control Register */
#define CLOCK_GATING_CTRL_REG			(MV_MISC_REGS_BASE + 0x038)

#define CGC_USBENCLOCK_OFFS(port)		(0 + (port))
#define CGC_USBENCLOCK_MASK(port)		(1 << CGC_USBENCLOCK_OFFS(port))
#define CGC_USBENCLOCK_EN(port)			(1 << CGC_USBENCLOCK_OFFS(port))
#define CGC_USBENCLOCK_DIS(port)		(0 << CGC_USBENCLOCK_OFFS(port))

#define CGC_GEENCLOCK_OFFS			2
#define CGC_GEENCLOCK_MASK			(1 << CGC_GEENCLOCK_OFFS)
#define CGC_GEENCLOCK_EN			(1 << CGC_GEENCLOCK_OFFS)
#define CGC_GEENCLOCK_DIS			(0 << CGC_GEENCLOCK_OFFS)

#define CGC_SATAENCLOCK_OFFS			3
#define CGC_SATAENCLOCK_MASK			(1 << CGC_SATAENCLOCK_OFFS)
#define CGC_SATAENCLOCK_EN			(1 << CGC_SATAENCLOCK_OFFS)
#define CGC_SATAENCLOCK_DIS			(0 << CGC_SATAENCLOCK_OFFS)

#define CGC_PEXENCLOCK_OFFS(port)		(4 + (port))
#define CGC_PEXENCLOCK_MASK(port)		(1 << CGC_PEXENCLOCK_OFFS(port))
#define CGC_PEXENCLOCK_EN(port)			(1 << CGC_PEXENCLOCK_OFFS(port))
#define CGC_PEXENCLOCK_DIS(port)		(0 << CGC_PEXENCLOCK_OFFS(port))

#define CGC_SDIOENCLOCK_OFFS(port)		(8 + (port))
#define CGC_SDIOENCLOCK_MASK(port)		(1 << CGC_SDIOENCLOCK_OFFS(port))
#define CGC_SDIOENCLOCK_EN(port)		(1 << CGC_SDIOENCLOCK_OFFS(port))
#define CGC_SDIOENCLOCK_DIS(port)		(0 << CGC_SDIOENCLOCK_OFFS(port))

#define CGC_NANDENCLOCK_OFFS			10
#define CGC_NANDENCLOCK_MASK			(1 << CGC_NANDENCLOCK_OFFS)
#define CGC_NANDENCLOCK_EN			(1 << CGC_NANDENCLOCK_OFFS)
#define CGC_NANDENCLOCK_DIS			(0 << CGC_NANDENCLOCK_OFFS)

#define CGC_CAMENCLOCK_OFFS			11
#define CGC_CAMENCLOCK_MASK			(1 << CGC_CAMENCLOCK_OFFS)
#define CGC_CAMENCLOCK_EN			(1 << CGC_CAMENCLOCK_OFFS)
#define CGC_CAMENCLOCK_DIS			(0 << CGC_CAMENCLOCK_OFFS)

#define CGC_ADENCLOCK_OFFS(port)		(12 + (port))
#define CGC_ADENCLOCK_MASK(port)		(1 << CGC_ADENCLOCK_OFFS(port))
#define CGC_ADENCLOCK_EN(port)			(1 << CGC_ADENCLOCK_OFFS(port))
#define CGC_ADENCLOCK_DIS(port)			(0 << CGC_ADENCLOCK_OFFS(port))

#define CGC_GPUENCLOCK_OFFS			17
#define CGC_GPUENCLOCK_MASK			(1 << CGC_GPUENCLOCK_OFFS)
#define CGC_GPUENCLOCK_EN			(1 << CGC_GPUENCLOCK_OFFS)
#define CGC_GPUENCLOCK_DIS			(0 << CGC_GPUENCLOCK_OFFS)

#define CGC_AC97ENCLOCK_OFFS			21
#define CGC_AC97ENCLOCK_MASK			(1 << CGC_AC97ENCLOCK_OFFS)
#define CGC_AC97ENCLOCK_EN			(1 << CGC_AC97ENCLOCK_OFFS)
#define CGC_AC97ENCLOCK_DIS			(0 << CGC_AC97ENCLOCK_OFFS)

#define CGC_PDMAENCLOCK_OFFS			22
#define CGC_PDMAENCLOCK_MASK			(1 << CGC_PDMAENCLOCK_OFFS)
#define CGC_PDMAENCLOCK_EN			(1 << CGC_PDMAENCLOCK_OFFS)
#define CGC_PDMAENCLOCK_DIS			(0 << CGC_PDMAENCLOCK_OFFS)

#define CGC_XORENCLOCK_OFFS(port)		(23 + (port))
#define CGC_XORENCLOCK_MASK(port)		(1 << CGC_XORENCLOCK_OFFS(port))
#define CGC_XORENCLOCK_EN(port)			(1 << CGC_XORENCLOCK_OFFS(port))
#define CGC_XORENCLOCK_DIS(port)		(0 << CGC_XORENCLOCK_OFFS(port))

/* Controler environment registers offsets */
#define MPP_CONTROL_REG0			(MV_MISC_REGS_BASE + 0x200)
#define MPP_CONTROL_REG1			(MV_MISC_REGS_BASE + 0x204)
#define MPP_CONTROL_REG2			(MV_MISC_REGS_BASE + 0x208)
#define MPP_CONTROL_REG3			(MV_MISC_REGS_BASE + 0x20C)
#define MPP_GENERAL_CONTROL_REG			(MV_MISC_REGS_BASE + 0x210)
#define MPP_SAMPLE_AT_RESET_REG0		(MV_MISC_REGS_BASE + 0x214)
#define MPP_SAMPLE_AT_RESET_REG1		(MV_MISC_REGS_BASE + 0x218)
#define MPP_AUDIO_UNIT_CONTROL_REG		(MV_MISC_REGS_BASE + 0x230)
#define MPP_SSCG_CONFIG_REG			(MV_MISC_REGS_BASE + 0x2B4)
#define MPP_PEX_CONTROL_REG			(MV_MISC_REGS_BASE + 0x2E0)
#define MPP_GENERAL_CONFIG_REG			(MV_MISC_REGS_BASE + 0x1803C)

#define MSAR_BOOT_MODE_OFFS                     0
#define MSAR_BOOT_MODE_MASK                     (0x1F << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_SPI_NO_BOOTROM		(0x0 << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_SPI_3_CYCLES			(0x1 << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_SPI_4_CYCLES			(0x2 << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_SATA				(0x3 << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_PEX_END_POINT			(0x4 << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_NAND_1			(0x5 << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_NAND_2			(0x6 << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_NAND_3			(0x7 << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_NAND_4			(0x8 << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_NAND_5			(0x9 << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_NAND_6			(0xA << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_NAND_7			(0xB << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_NAND_8			(0xC << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_NAND_9			(0xD << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_UART0				(0xE << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_UART1				(0xF << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_DEBUG_UART0			(0x10 << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_DEBUG_UART1			(0x11 << MSAR_BOOT_MODE_OFFS)
/* 0x12 till 0x13 are reserved */
#define MSAR_BOOT_NAND_10			(0x14 << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_NAND_11			(0x15 << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_NAND_12			(0x16 << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_NAND_13			(0x17 << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_NAND_14			(0x18 << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_NAND_15			(0x19 << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_NAND_16			(0x1A << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_NAND_17			(0x1B << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_NAND_18			(0x1C << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_NAND_19			(0x1D << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_NAND_20			(0x1E << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_NAND_21			(0x1F << MSAR_BOOT_MODE_OFFS)

#define MSAR_TCLCK_OFFS				23
#define MSAR_TCLCK_MASK				(0x3 << MSAR_TCLCK_OFFS)
#define MSAR_TCLCK_166				(0x0 << MSAR_TCLCK_OFFS)
#define MSAR_TCLCK_125				(0x1 << MSAR_TCLCK_OFFS)

#define MSAR_CPUCLCK_OFFS			5
#define MSAR_CPUCLCK_MASK			(0xF << MSAR_CPUCLCK_OFFS)

#define MSAR_DDRCLCK_RTIO_OFFS			12
#define MSAR_DDRCLCK_RTIO_MASK			(0xF << MSAR_DDRCLCK_RTIO_OFFS)

#define MSAR_L2CLCK_RTIO_OFFS			9
#define MSAR_L2CLCK_RTIO_MASK			(0x7 << MSAR_L2CLCK_RTIO_OFFS)

#ifndef MV_ASMLANGUAGE

#if defined(CONFIG_DOVE_REV_Z0)

/* CPU clock for 6781Z0. 0->Resereved */
#define MV_CPU_CLCK_TBL { 	0,		0, 		0, 		1200000000,	\
			     	1067000000, 	933000000,	800000000,	667000000,	\
			     	533000000, 	0,		0,		0,		\
			     	0,		0,		0,		0}
/* DDR clock RATIO for 6781Z0. {0,0}->Reserved */
#define MV_DDR_CLCK_RTIO_TBL	{\
	{1, 1}, {3, 2}, {2, 1}, {5, 2}, \
	{3, 1}, {7, 2}, {4, 1}, {9, 2}, \
	{5, 1}, {11, 2}, {6, 1}, {13, 2}, \
	{7, 1}, {15, 2}, {8, 1}, {10, 1} \
}

/* L2 clock RATIO for 6781Z0. {0,0}->Reserved */
#define MV_L2_CLCK_RTIO_TBL	{\
	{0, 0}, {3, 2}, {2, 1}, {5, 2}, \
	{3, 1}, {7, 2}, {4, 1}, {0, 0} \
}

#elif defined (CONFIG_DOVE_REV_Y0) || defined(CONFIG_DOVE_REV_Y1)

/* CPU clock for 6781Y0. 0->Resereved */
#define MV_CPU_CLCK_TBL { 	0,		0, 		0, 		0,		\
			     	0,		1000000000, 	933000000,	933000000,	\
				800000000,     	800000000, 	800000000,	1067000000,	\
				667000000,	533000000,	400000000,	333000000	}

/* DDR clock RATIO for 6781Y0. {0,0}->Reserved */
#define MV_DDR_CLCK_RTIO_TBL	{\
	{1, 1}, {0, 0}, {2, 1}, {0, 0}, \
	{3, 1}, {0, 0}, {4, 1}, {0, 0}, \
	{5, 1}, {0, 0}, {6, 1}, {0, 0}, \
	{7, 1}, {0, 0}, {8, 1}, {10, 1} \
}

/* L2 clock RATIO for 6781Y0. {0,0}->Reserved */
#define MV_L2_CLCK_RTIO_TBL	{\
	{1, 1}, {0, 0}, {2, 1}, {0, 0}, \
	{3, 1}, {0, 0}, {4, 1}, {0, 0} \
}

#else /* CONFIG_DOVE_REV_X0 */

/* CPU clock for 6781X0. 0->Resereved */
#define MV_CPU_CLCK_TBL { 	0,		0, 		0, 		0,		\
			     	0,		1000000000, 	933000000,	933000000,	\
				800000000,     	800000000, 	800000000,	1067000000,	\
				667000000,	533000000,	400000000,	333000000	}

/* DDR clock RATIO for 6781X0. {0,0}->Reserved */
#define MV_DDR_CLCK_RTIO_TBL	{\
	{1, 1}, {0, 0}, {2, 1}, {5, 2}, \
	{3, 1}, {0, 0}, {4, 1}, {0, 0}, \
	{5, 1}, {0, 0}, {6, 1}, {0, 0}, \
	{7, 1}, {0, 0}, {8, 1}, {10, 1} \
}

/* L2 clock RATIO for 6781X0. {0,0}->Reserved */
#define MV_L2_CLCK_RTIO_TBL	{\
	{1, 1}, {0, 0}, {2, 1}, {0, 0}, \
	{3, 1}, {0, 0}, {4, 1}, {0, 0} \
}


#endif /* CONFIG_DOVE_REV_Z0 */


/* These macros help units to identify a target Mbus Arbiter group */
#define MV_PCI_DRAM_BAR_TO_DRAM_TARGET(bar)   0

#define	MV_TARGET_IS_AS_BOOT(target) ((target) == (sampleAtResetTargetArray[((MV_REG_READ(MPP_SAMPLE_AT_RESET_REG0)\
						 & MSAR_BOOT_MODE_MASK) >> MSAR_BOOT_MODE_OFFS)]))


#define MV_CHANGE_BOOT_CS(target)	((target) == DEV_BOOCS)?\
					sampleAtResetTargetArray[((MV_REG_READ(MPP_SAMPLE_AT_RESET_REG0)\
						 & MSAR_BOOT_MODE_MASK) >> MSAR_BOOT_MODE_OFFS)]\
					:(target)
					
#define TCLK_TO_COUNTER_RATIO   1   /* counters running in Tclk */

#define BOOT_TARGETS_NAME_ARRAY {       \
    SPI0,          	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    BOOT_ROM_CS, 	\
    TBL_TERM,         	\
}


/* For old competability */
#define MV_BOOTDEVICE_INDEX   	0


#define PCI_IF0_MEM0		PEX0_MEM
#define PCI_IF0_IO		PEX0_IO
#define PCI_IF1_MEM0		PEX1_MEM
#define PCI_IF1_IO		PEX1_IO

#define PCI_IO(pciIf)		(PEX0_IO + 2*(pciIf))	
#define PCI_MEM(pciIf, memNum)	(PEX0_MEM + 2*(pciIf))


#endif /* MV_ASMLANGUAGE */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCmvCtrlEnvRegsh */
