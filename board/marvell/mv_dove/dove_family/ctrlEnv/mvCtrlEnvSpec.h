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

#ifndef __INCmvCtrlEnvSpech
#define __INCmvCtrlEnvSpech

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MV_ARM_SOC
#define SOC_NAME_PREFIX			"MV88F"

/* used for sync write reordering in bridge*/ 
/*#define MV_BRIDGE_SYNC_REORDER*/
#define MV_DDR_INCLUDE_DDRMC

/* units base and port numbers */
/* Only one actual TWSI unit - always return the same base address regardless of chanNum */
#define MV_SPI_REGS_OFFSET(unit)		(0x10600 | (unit << 14))
#define MV_TWSI_SLAVE_REGS_OFFSET(chanNum)	(0x11000)
#define MV_UART_CHAN_OFFSET(chanNum)		(0x12000 + (chanNum * 0x100))
#define MV_CPUIF_REGS_OFFSET			(0x20000)
#define MV_CNTMR_REGS_OFFSET			(0x20300)
#define MV_CESA_TDMA_REGS_OFFSET		(0x30000)
#define MV_CESA_REGS_OFFSET			(0x3D000)
#define MV_PEX_IF_REGS_OFFSET(pexIf)		(0x40000 + (pexIf * 0x40000))
#define MV_USB_REGS_OFFSET(dev)       		((dev) ? 0x51000:0x50000)
#define MV_XOR_REGS_OFFSET(unit)           	((unit) ? 0x60900:0x60800)
#define MV_ETH_REGS_OFFSET(port)		(((port) == 0) ? 0x72000 : 0x72000)
#define MV_SDIO_REGS_OFFSET(dev)		((dev) ? 0x90000:0x92000)
#define MV_CAM_SDIO_REGS_OFFSET			(0x90000)
#define MV_CAFE_REGS_OFFSET			(0x94000)
#define MV_SATA_REGS_OFFSET           		(0xA0000)
#define MV_AUDIO_REGS_OFFSET(unit)		((unit) ? 0xB4000:0xB0000)
#define MV_NFC_REGS_OFFSET			(0xC0000)
#define MV_PMU_REGS_OFFSET			(0xD0000)
#define MV_MISC_REGS_OFFSET			(0xD0000)
#ifdef CONFIG_DOVE_REV_Z0
#define MV_GPP_REGS_OFFSET(grp)			((grp) ? 0xD0420:0xD0400)
#else
#define MV_GPP_REGS_OFFSET(grp)			((grp) ? ((grp == 2) ? 0xE8400 : 0xD0420) : 0xD0400)
#endif /* CONFIG_DOVE_REV_Z0 */
#define MV_GPP_REGS_OFFSET_0			0xD0400
#define MV_RTC_REGS_OFFSET			(0xD8500)
#define MV_AC97_REGS_OFFSET			(0xE0000)
#define MV_PDMA_REGS_OFFSET			(0xE4000)
#define MV_DDR_REGS_OFFSET			(0x800000)

#define INTER_REGS_SIZE	 	 	_16M

/* This define describes the TWSI interrupt bit and location */
#define TWSI_CPU_MAIN_INT_CAUSE_REG		0x20200
#define TWSI0_CPU_MAIN_INT_BIT			(1<<11)

/* This define describes the TWSI Baud Rate */
#define TWSI_SPEED				100000

#ifdef CONFIG_DOVE_REV_Z0
#define MV_GPP_MAX_GROUP    		2
#else
#define MV_GPP_MAX_GROUP    		3
#endif /* CONFIG_DOVE_REV_Z0 */
#define MV_CNTMR_MAX_COUNTER 		2
#define MV_UART_MAX_CHAN		4
#define MV_XOR_MAX_UNIT         	2
#define MV_XOR_MAX_CHAN         	4 /* total channels for all units together */
#define MV_XOR_MAX_CHAN_PER_UNIT       	2 /* channels for units */
#define MV_SATA_MAX_CHAN	 	1

#ifdef CONFIG_DOVE_REV_Z0 
#define MV_TWSI_MAX_CHAN		1
#else
#define MV_TWSI_MAX_CHAN		3
#endif /* CONFIG_DOVE_REV_Z0 */

/* channel ranges for twsi units */
#define MV_TWSI_UNIT_SOC_MIN_CHANNEL		0
#define MV_TWSI_UNIT_SOC_MAX_CHANNEL            2
#define MV_TWSI_UNIT_CAM_MIN_CHANNEL         MV_TWSI_UNIT_SOC_MAX_CHANNEL+1
#define MV_TWSI_UNIT_CAM_MAX_CHANNEL	     MV_TWSI_UNIT_CAM_MIN_CHANNEL 
/* which twsi units are included */
#define MV_TWSI_UNIT_INCLUDE_SOC        1
#define MV_TWSI_UNIT_INCLUDE_CAM	1
/* channel definitions */
#define MV_TWSI_CHAN_CAM	MV_TWSI_UNIT_CAM_MAX_CHANNEL

#define MV_6781_MPP_MAX_GROUP		2
#define MV_6781_MPP_MAX_MODULE		2

#define MV_DRAM_MAX_CS      		2

/* This define describes the maximum number of supported PCI\PCIX Interfaces*/
#define MV_PCI_MAX_IF		0
#define MV_PCI_START_IF		0

/* This define describes the maximum number of supported PEX Interfaces 	*/
#define MV_INCLUDE_PEX0
#define MV_DISABLE_PEX_DEVICE_BAR 
#define MV_PEX_MAX_IF		2
#define MV_PEX_START_IF		MV_PCI_MAX_IF

/* This define describes the maximum number of supported PCI Interfaces 	*/
#define MV_PCI_IF_MAX_IF   	(MV_PEX_MAX_IF+MV_PCI_MAX_IF)

#define MV_ETH_MAX_PORTS		1

#define MV_IDMA_MAX_CHAN    		0

#define MV_USB_MAX_PORTS		2

#define MV_6781_NAND			1

#define MV_6781_SDIO			1

#define MV_6781_AUDIO			1

#define MV_AUDIO_MAX_UNITS		2

#define MV_6781_TDM			0

#define MV_DEVICE_MAX_CS      		4

/* Others */
#define PEX_HOST_BUS_NUM(pciIf)		(pciIf)
#define PEX_HOST_DEV_NUM(pciIf)		0
/* CESA version #2: One channel, 2KB SRAM, TDMA */
#define MV_CESA_VERSION		 	2
#define MV_CESA_SRAM_SIZE               2*1024

#define MV_ETH_VERSION 			4
#define MV_ETH_PORT_SGMII          	{ MV_FALSE }
#define MV_ETH_MAX_RXQ                 8
#define MV_ETH_MAX_TXQ                 8

/* This define describes the the support of USB 	*/
#define MV_USB_VERSION  		1

#define MV_INCLUDE_SDRAM_CS0
#define MV_INCLUDE_SDRAM_CS1
#define MV_INCLUDE_SDRAM_CS2

#define MV_INCLUDE_DEVICE_CS0
#define MV_INCLUDE_DEVICE_CS1
#define MV_INCLUDE_DEVICE_CS2
#define MV_INCLUDE_DEVICE_CS3


#define MV_TARGET_IS_DRAM(target)   \
                            ((target >= SDRAM_CS0) && (target <= SDRAM_CS3))

#define MV_TARGET_IS_PEX0(target)   \
                            ((target >= PEX0_MEM) && (target <= PEX0_IO))

#define MV_TARGET_IS_PEX1(target)   \
                            ((target >= PEX1_MEM) && (target <= PEX1_IO))

#define MV_TARGET_IS_PEX(target) (MV_TARGET_IS_PEX0(target) || MV_TARGET_IS_PEX1(target))

#define MV_TARGET_IS_DEVICE(target) \
                            ((target >= DEVICE_CS0) && (target <= DEVICE_CS3))


#ifndef MV_ASMLANGUAGE

/* This enumerator defines the Marvell Units ID      */ 
typedef enum _mvUnitId
{
    DRAM_UNIT_ID,
    PEX_UNIT_ID,
    ETH_GIG_UNIT_ID,
    USB_UNIT_ID,
    XOR_UNIT_ID,
    SATA_UNIT_ID,
    UART_UNIT_ID,
    NAND_UNIT_ID,
    SPI_UNIT_ID,
    CAMERA_UNIT_ID,
    GPU_UNIT_ID,
    AUDIO_UNIT_ID,
    AC97_UNIT_ID,
    SDIO_UNIT_ID,
    PDMA_UNIT_ID,
    MAX_UNITS_ID

}MV_UNIT_ID;

/* This enumerator defines the Marvell controller target ID      */ 
typedef enum _mvTargetId
{
    DRAM_TARGET_ID  = 0,    /* Port 0 -> DRAM interface         */
    DEV_TARGET_ID   = 1,    /* Port 1 -> SPI	 		*/
    CRYPT_TARGET_ID = 3,    /* Port 3 -> Crypto Engine		*/
    PEX0_TARGET_ID  = 4,    /* Port 4 -> PCI Express0 		*/
    PEX1_TARGET_ID  = 8,    /* Port 8 -> PCI Express1 		*/
    NAND_FLASH_ID   = 0xC, 
    PMU_SRAM_ID     = 0xD, 
    MAX_TARGETS_ID
} MV_TARGET_ID;

/* This enumerator describes the Marvell controller possible devices that   */
/* can be connected to its device interface.                                */
typedef enum _mvDevice
{
#if defined(MV_INCLUDE_DEVICE_CS0)
	DEV_CS0 = 0,    /* Device connected to dev CS[0]    */
#endif
#if defined(MV_INCLUDE_DEVICE_CS1)
	DEV_CS1 = 1,        /* Device connected to dev CS[1]    */
#endif
#if defined(MV_INCLUDE_DEVICE_CS2)
	DEV_CS2 = 2,        /* Device connected to dev CS[2]    */
#endif
#if defined(MV_INCLUDE_DEVICE_CS3)
	DEV_CS3 = 3,        /* Device connected to dev CS[2]    */
#endif
#if defined(MV_INCLUDE_DEVICE_CS4)
	DEV_CS4 = 4,        /* Device connected to BOOT dev    */
#endif
	MV_DEV_MAX_CS = MV_DEVICE_MAX_CS
}MV_DEVICE;


/* This enumerator described the possible Controller paripheral targets.    */
/* Controller peripherals are designated memory/IO address spaces that the  */
/* controller can access. They are also referred to as "targets".           */
typedef enum _mvTarget
{
    TBL_TERM = -1, 	/* non valid target, used as targets list terminator	*/
    SDRAM_CS0,     	/* DDR SDRAM Chip Select 0			*/  
    SDRAM_CS1,     	/* DDR SDRAM Chip Select 1			*/  
    SDRAM_CS2,     	/* DDR SDRAM Chip Select 2			*/
    SDRAM_CS3,     	/* DDR SDRAM Chip Select 3			*/
    PEX0_MEM,		/* PCI Express 0 Memory				*/
    PEX0_IO,		/* PCI Express 0 IO				*/
    PEX1_MEM,		/* PCI Express 1 Memory				*/
    PEX1_IO,		/* PCI Express 1 IO				*/
    INTER_REGS,     	/* Internal registers                           */  
    SPI0,	     	/* SPI0						*/  
    SPI1,	      	/* SPI1						*/  
    BOOT_ROM_CS,	/* BOOT_ROM_CS					*/
    DEV_BOOCS,     	/* DEV_BOOCS					*/
    CRYPT_ENG,      	/* Crypto Engine				*/
    MAX_TARGETS

} MV_TARGET;

#define TARGETS_DEF_ARRAY	{			\
    {0x00,DRAM_TARGET_ID}, /* DDR_SDRAM */		\
    {0x00,DRAM_TARGET_ID}, /* DDR_SDRAM */		\
    {0x00,DRAM_TARGET_ID}, /* DDR_SDRAM */		\
    {0x00,DRAM_TARGET_ID}, /* DDR_SDRAM */		\
    {0xE8,PEX0_TARGET_ID}, /* PEX0_MEM */		\
    {0xE0,PEX0_TARGET_ID}, /* PEX0_IO */		\
    {0xE8,PEX1_TARGET_ID}, /* PEX1_MEM */		\
    {0xE0,PEX1_TARGET_ID}, /* PEX1_IO */		\
    {0xFF,	    0xFF}, /* INTER_REGS */		\
    {0xFE,DEV_TARGET_ID},  /* SPI0 */			\
    {0xFB,DEV_TARGET_ID},  /* SPI1 */			\
    {0xFD,DEV_TARGET_ID},  /* BOOT_ROM_CS */		\
    {0xFE,DEV_TARGET_ID},  /* DEV_BOOCS */		\
    {0x00,CRYPT_TARGET_ID} /* CRYPT_ENG */		\
}


#define TARGETS_NAME_ARRAY	{	\
    "SDRAM_CS0",    /* DDR_SDRAM CS0 */	\
    "SDRAM_CS1",    /* DDR_SDRAM CS1 */	\
    "SDRAM_CS2",    /* DDR_SDRAM CS2 */	\
    "SDRAM_CS3",    /* DDR_SDRAM CS3 */	\
    "PEX0_MEM",	    /* PEX0_MEM */	\
    "PEX0_IO",	    /* PEX0_IO */	\
    "PEX1_MEM",	    /* PEX1_MEM */	\
    "PEX1_IO",	    /* PEX1_IO */	\
    "INTER_REGS",   /* INTER_REGS */	\
    "SPI0",	    /* SPI0 */		\
    "SPI1",	    /* SPI1 */		\
    "BOOT_ROM_CS",  /* BOOT_ROM_CS */	\
    "DEV_BOOTCS",   /* DEV_BOOCS */	\
    "CRYPT_ENG"	    /* CRYPT_ENG */	\
}

/* For old competability */
#define SPI_CS			SPI0
#define DEVICE_CS0		SPI0  
#define DEVICE_CS1  		SPI1 
#define DEVICE_CS2  		CRYPT_ENG 
#define DEVICE_CS3  		DEV_BOOCS

#define START_DEV_CS   		DEV_CS0
#define DEV_TO_TARGET(dev)	((dev) + DEVICE_CS0)

#endif

/* We use the following registers to store DRAM interface pre configuration   */
/* auto-detection results						      */
/* IMPORTANT: We are using mask register for that purpose. Before writing     */
/* to units mask register, make sure main maks register is set to disable     */
/* all interrupts.                                                            */
#define DRAM_BUF_REG0	(MV_MISC_REGS_BASE + 0x100)	/* sdram bank 0 size 	*/
#define DRAM_BUF_REG1	(MV_MISC_REGS_BASE + 0x104)	/* sdram timing 1 	*/
#define DRAM_BUF_REG2   (MV_MISC_REGS_BASE + 0x108)	/* sdram timing 2 	*/
#define DRAM_BUF_REG3	(MV_MISC_REGS_BASE + 0x10C)	/* sdram timing 3 	*/          
#define DRAM_BUF_REG4	(MV_MISC_REGS_BASE + 0x110)	/* sdram timing 4	*/
#define DRAM_BUF_REG5	(MV_MISC_REGS_BASE + 0x114)	/* sdram timing 5	*/
#define DRAM_BUF_REG6	(MV_MISC_REGS_BASE + 0x118)	/* sdram timing 6	*/
#define DRAM_BUF_REG7	(MV_MISC_REGS_BASE + 0x11C)	/* sdram ctrl 6		*/
#define DRAM_BUF_REG8	(MV_MISC_REGS_BASE + 0x120)	/* sdram ctrl 7       	*/
#define DRAM_BUF_REG9	(MV_MISC_REGS_BASE + 0x124)	/* sdram config 0	*/
#define DRAM_BUF_REG10	(MV_MISC_REGS_BASE + 0x128)	/* sdram ctrl 4		*/

#endif /* __INCmvCtrlEnvSpech */
