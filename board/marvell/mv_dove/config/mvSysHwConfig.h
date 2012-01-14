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
/*******************************************************************************
* mvSysHwCfg.h - Marvell system HW configuration file
*
* DESCRIPTION:
*       None.
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

#ifndef __INCmvSysHwConfigh
#define __INCmvSysHwConfigh

/****************************************/
/* Soc supporetd Units definitions	*/
/****************************************/
#undef MV_MEM_OVER_PEX_WA

#define MV_INCLUDE_PEX
#define MV_INCLUDE_GIG_ETH
#define MV_INCLUDE_USB
#define MV_INCLUDE_TWSI
#define MV_INCLUDE_NAND
#define MV_INCLUDE_UART
#define MV_INCLUDE_SPI
#define MV_INCLUDE_XOR
//#define MV_INCLUDE_CAMERA
//#define MV_INCLUDE_GPU
#define MV_INCLUDE_AUDIO
#define MV_INCLUDE_AC97
#define MV_INCLUDE_SDIO
#define MV_INCLUDE_RTC
#define MV_INCLUDE_PDMA
#define MV_INCLUDE_INTEG_SATA
#define MV_INCLUDE_CLK_PWR_CNTRL
#define MV_INCLUDE_PMU

/*********************************************/
/* Board Specific defines : On-Board devices */
/*********************************************/

/* DRAM ddim detection support */
#undef MV_INC_BOARD_DDIM
/* On-Board NAND Flash support */
#define MV_INC_BOARD_NAND_FLASH
/* On-Board SPI Flash support */
#define MV_INC_BOARD_SPI_FLASH
/* On-Board RTC */
#define MV_INC_BOARD_RTC


/************************************************/
/* U-Boot Specific				*/
/************************************************/
#define MV_INCLUDE_MONT_EXT

#if defined(MV_INCLUDE_MONT_EXT)
#define MV_INCLUDE_MONT_MMU
#define MV_INCLUDE_MONT_MPU
#if defined(MV_INC_BOARD_NOR_FLASH)
#define MV_INCLUDE_MONT_FFS
#endif
#define MV_INCLUDE_MONT_LOAD_NET
#endif


/* 
 *  System memory mapping 
 */

/* SDRAM: actual mapping is auto detected */
#define SDRAM_CS0_BASE  0x00000000
#define SDRAM_CS0_SIZE  _256M

#define SDRAM_CS1_BASE  0x10000000
#define SDRAM_CS1_SIZE  _256M

#define SDRAM_CS2_BASE  0x20000000
#define SDRAM_CS2_SIZE  _256M

#define SDRAM_CS3_BASE  0x30000000
#define SDRAM_CS3_SIZE  _256M

/* PEX */
#define PEX0_MEM_BASE 0x90000000
#define PEX0_MEM_SIZE _128M

#define PEX0_IO_BASE 0xF0000000
#define PEX0_IO_SIZE _8M

#define PEX1_MEM_BASE 0x98000000
#define PEX1_MEM_SIZE _128M

#define PEX1_IO_BASE 0xF0800000
#define PEX1_IO_SIZE _8M


#define DEVICE_SPI0_BASE 0xf8000000
#define DEVICE_CS0_BASE DEVICE_SPI0_BASE
#define DEVICE_CS0_SIZE _8M

#define DEVICE_SPI_BASE DEVICE_SPI0_BASE

#define DEVICE_SPI1_BASE 0xf9000000
#define DEVICE_CS1_BASE DEVICE_SPI1_BASE
#define DEVICE_CS1_SIZE _8M

#define DEVICE_BOOTROM_BASE 0xf4000000
#define DEVICE_CS2_BASE DEVICE_BOOTROM_BASE
#define DEVICE_CS2_SIZE _1M

#define DEVICE_CS3_BASE BOOTDEV_CS_BASE
#define DEVICE_CS3_SIZE BOOTDEV_CS_SIZE

#if !defined(MV_BOOTROM) && defined(MV_NAND_BOOT)
#define CONFIG_SYS_NAND_BASE 	BOOTDEV_CS_BASE
#else
#define CONFIG_SYS_NAND_BASE 	DEVICE_CS0_BASE
#endif

/* Internal registers: size is defined in Controller environment */
#define INTER_REGS_BASE			0xF1000000
#define DOVE_SB_REGS_PHYS_BASE          INTER_REGS_BASE
#define DOVE_SB_REGS_VIRT_BASE          INTER_REGS_BASE
#define DOVE_CESA_PHYS_BASE		0xC8000000
#define DOVE_SCRATCHPAD_PHYS_BASE	DOVE_CESA_PHYS_BASE
#define DOVE_SCRATCHPAD_VIRT_BASE	DOVE_CESA_PHYS_BASE
#define DOVE_PMUSP_VIRT_BASE		0xFDC00000
#define DOVE_NB_REGS_VIRT_BASE		0xFE600000

#define CRYPT_ENG_BASE	0xFB000000
#define CRYPT_ENG_SIZE	_64K

/* PMU Address definitions */
#ifdef CONFIG_DOVE_REV_Z0
 #define PMU_CESA_SP_BASE              (DOVE_SB_REGS_VIRT_BASE + MV_PMU_REGS_BASE + 0xC000)
 #define PMU_SCRATCHPAD_EXT_BASE       (DOVE_SCRATCHPAD_VIRT_BASE + 0x10000)
 #define PMU_SCRATCHPAD_INT_BASE       (DOVE_SCRATCHPAD_VIRT_BASE + 0x10000)
 #define PMU_SCRATCHPAD_INT_BASE_PHYS  (DOVE_SCRATCHPAD_PHYS_BASE + 0x10000)
#else
 #define PMU_SCRATCHPAD_EXT_BASE       (DOVE_SCRATCHPAD_VIRT_BASE + 0xC000)
 #define PMU_SCRATCHPAD_INT_BASE       (DOVE_SB_REGS_VIRT_BASE + MV_PMU_REGS_BASE + 0xC000)
 #define PMU_SCRATCHPAD_INT_BASE_PHYS  (DOVE_SB_REGS_PHYS_BASE + MV_PMU_REGS_BASE + 0xC000)
#endif

/* DRAM detection stuff */
#define MV_DRAM_AUTO_SIZE

#define PCI_ARBITER_CTRL    /* Use/unuse the Marvell integrated PCI arbiter	*/
#undef	PCI_ARBITER_BOARD	/* Use/unuse the PCI arbiter on board			*/

/* Check macro validity */
#if defined(PCI_ARBITER_CTRL) && defined (PCI_ARBITER_BOARD)
	#error "Please select either integrated PCI arbiter or board arbiter"
#endif

/* Board clock detection */
#define TCLK_AUTO_DETECT    /* Use Tclk auto detection 		*/
#define SYSCLK_AUTO_DETECT	/* Use SysClk auto detection 	*/
#define PCLCK_AUTO_DETECT  /* Use PClk auto detection */
#define L2CLK_AUTO_DETECT  /* Use L2 Clk auto detection */

/************* Ethernet driver configuration ********************/

/* HW cache coherency configuration */
#define DMA_RAM_COHER	    NO_COHERENCY
#define INTEG_SRAM_COHER    MV_UNCACHED  /* Where integrated SRAM available */


/*********** Idma default configuration ***********/
#define UBOOT_CNTRL_DMA_DV     (ICCLR_DST_BURST_LIM_8BYTE | \
				ICCLR_SRC_INC | \
				ICCLR_DST_INC | \
				ICCLR_SRC_BURST_LIM_8BYTE | \
				ICCLR_NON_CHAIN_MODE | \
				ICCLR_BLOCK_MODE )


/* CPU address decode table. Note that table entry number must match its    */
/* winNum enumerator. For example, table entry '4' must describe Deivce CS0 */
/* winNum which is represent by DEVICE_CS0 enumerator (4).                  */
#define MV_CPU_IF_ADDR_WIN_MAP_TBL	{				\
/*     base low      base high     size       	   WinNum     enable */ \
	{{SDRAM_CS0_BASE ,    0,      SDRAM_CS0_SIZE} ,0xFFFFFFFF,DIS}, \
	{{SDRAM_CS1_BASE ,    0,      SDRAM_CS1_SIZE} ,0xFFFFFFFF,DIS}, \
	{{SDRAM_CS2_BASE ,    0,      SDRAM_CS2_SIZE} ,0xFFFFFFFF,DIS}, \
	{{SDRAM_CS3_BASE ,    0,      SDRAM_CS3_SIZE} ,0xFFFFFFFF,DIS}, \
	{{PEX0_MEM_BASE  ,    0,      PEX0_MEM_SIZE } ,0x0       ,EN},	\
	{{PEX0_IO_BASE   ,    0,      PEX0_IO_SIZE  } ,0x2       ,EN},	\
	{{PEX1_MEM_BASE  ,    0,      PEX1_MEM_SIZE } ,0x1       ,EN},	\
	{{PEX1_IO_BASE   ,    0,      PEX1_IO_SIZE  } ,0x3       ,EN},	\
	{{INTER_REGS_BASE,    0,      INTER_REGS_SIZE},0x8       ,EN},	\
	{{DEVICE_CS0_BASE,    0,      DEVICE_CS0_SIZE},0x4	 ,EN},	\
	{{DEVICE_CS1_BASE,    0,      DEVICE_CS1_SIZE},0x5	 ,DIS},	\
	{{DEVICE_CS2_BASE,    0,      DEVICE_CS2_SIZE},0x5	 ,EN},	\
	{{BOOTDEV_CS_BASE,    0,      BOOTDEV_CS_SIZE},0x6	 ,EN},	\
	{{CRYPT_ENG_BASE,     0,      CRYPT_ENG_SIZE} ,0x7  	 ,EN},	\
    /* Table terminator */\
    {{TBL_TERM, TBL_TERM, TBL_TERM}, TBL_TERM,TBL_TERM}			\
};

#define MV_CACHEABLE(address) ((address) | 0x80000000)
#if 0
#define _1K		0x00000400
#define _2K		0x00000800
#define _4K		0x00001000
#define _8K		0x00002000
#define _16K		0x00004000
#define _32K		0x00008000
#define _64K		0x00010000
#define _128K		0x00020000
#define _256K		0x00040000
#define _512K		0x00080000

#define _1M		0x00100000
#define _2M		0x00200000
#define _3M		0x00300000
#define _4M		0x00400000
#define _5M		0x00500000
#define _6M		0x00600000
#define _7M		0x00700000
#define _8M		0x00800000
#define _9M		0x00900000
#define _10M		0x00a00000
#define _11M		0x00b00000
#define _12M		0x00c00000
#define _13M		0x00d00000
#define _14M		0x00e00000
#define _15M		0x00f00000
#define _16M		0x01000000

#define _32M		0x02000000
#define _64M		0x04000000
#define _128M		0x08000000
#define _256M		0x10000000
#define _512M		0x20000000

#define _1G		0x40000000
#define _2G		0x80000000
#endif

#if defined(MV_BOOTSIZE_256K)

#define BOOTDEV_CS_SIZE  (256 << 10) /* _256K */

#elif defined(MV_BOOTSIZE_512K)

#define BOOTDEV_CS_SIZE (512 << 10) /* _512K */

#elif defined(MV_BOOTSIZE_4M)

#define BOOTDEV_CS_SIZE (4 << 20) /* _4M */

#elif defined(MV_BOOTSIZE_8M)

#define BOOTDEV_CS_SIZE (8 << 20) /* _8M */

#elif defined(MV_BOOTSIZE_16M)

#define BOOTDEV_CS_SIZE (16 << 20) /* _16M */

#elif defined(MV_BOOTSIZE_32M)

#define BOOTDEV_CS_SIZE (32 << 20) /* _32M */

#elif defined(MV_BOOTSIZE_64M)

#define BOOTDEV_CS_SIZE (64 << 20) /* _64M */

#elif defined(MV_NAND_BOOT)

#define BOOTDEV_CS_SIZE (512 << 10) /* _512K */

#else

#define Error MV_BOOTSIZE undefined

#endif                                               

#define BOOTDEV_CS_BASE	((0xFFFFFFFF - BOOTDEV_CS_SIZE) + 1)


#define MV_CESA_MAX_BUF_SIZE	1600

#endif /* __INCmvSysHwConfigh */
