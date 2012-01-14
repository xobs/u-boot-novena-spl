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

#include "mv_os.h"


/********/
/* MISC */
/********/
#define INTER_REGS_BASE					0xD0000000
#define BOOTROM_ROUTINE_ERR_REG				0xD02D8
#define SAMPLE_RESET					0xD0214
/*******/
/* DDR */
/*******/
#define DRAM_STATUS_REG					0xD08001B0
#define DRAM_INIT_DONE_MASK				0x1
#define DRAM_PAD_ODT_CTRL_REG				0x800770
#define DRAM_PHY_CTRL10_REG				0x800200
#define DRAM_CTRL2_REG					0x800090
#define DRAM_CTRL1_REG					0x800080
#define DRAM_USR_CMD_REG				0x800120
#define DRAM_CTRL15_REG					0x800250
#define DRAM_CTRL_WL_SLCT_REG				0x800E40
#define DRAM_CTRL_WL_CTRL10_REG				0x800E50

/**********/
/* TIMERS */
/**********/
#define TIMERS_CTRL_REG					0x20300
#define TIMER0_ENABLE_OFFS				0x0
#define TIMER0_ENABLE_MASK				(0x1 << TIMER0_ENABLE_OFFS)
#define TIMER0_AUTO_OFFS				0x1
#define TIMER0_AUTO_MASK				(0x1 << TIMER0_ENABLE_OFFS)
#define TIMER0_RELOAD_REG				0x20310
#define TIMER0_VALUE_REG				0x20314

/*******/
/* PMU */
/*******/
#define PMU_MPP_CTRL_REG				0xD0210
#define PMU_SIG_SLCT_REG				0xD802C

/*
 * Delay in us
 */
MV_VOID uDelay(MV_U32 delay)
{
	/* From TCLK calculate the cycles needed for the requested us delay */
	MV_U32 cycles = (166 * delay); /* Assuming 166Mhx Tclk */
	
	/* Set the Timer value */
	MV_REG_WRITE(TIMER0_VALUE_REG, cycles);

	/* Enable the Timer */
	MV_REG_BIT_SET(TIMERS_CTRL_REG, TIMER0_ENABLE_MASK); 

	/* loop waiing for the timer to expire */
	while (MV_REG_READ(TIMER0_VALUE_REG)) {}  
}

/*
 * Perform DDR3 Write Leveling on the specified CS #
 */
MV_VOID ddr3_wl(MV_U32 cs_num, MV_U32 cs_copy)
{
	MV_U32 cs, i, j;
	MV_U32 reg;
	MV_U32 wl_val;
	MV_U32 odt;
	MV_U32 pldwn;
	MV_U32 ddr_ctrl2;

	/* Force ODT always ON */
	odt = MV_REG_READ(DRAM_PAD_ODT_CTRL_REG);
	reg = (odt & ~0x03000000);			/* clear 0xD0800770[25:24] ==> Controller Side Termination Disabled */
	reg &= ~0x0000000F;
	reg |= 0x0000000A;				/* set 0xD0800770[3:0] = 0xA ==> DDR Termination enable, CS#0,1 */
	MV_REG_WRITE(DRAM_PAD_ODT_CTRL_REG, reg);

	/* Disable pulldown on Phy */
	pldwn = MV_REG_READ(DRAM_PHY_CTRL10_REG);
	reg = (pldwn & ~0x0000001F);			/* clear 0xD0800200[4:0] ==> Disale pull down on all signal */
	MV_REG_WRITE(DRAM_PHY_CTRL10_REG, reg);

	/* Enter MC WL mode */
	ddr_ctrl2 = MV_REG_READ(DRAM_CTRL2_REG);
	reg = (ddr_ctrl2 | 0x00000004); 		/* set 0xD0800090[2] ==> Enable MC WL mode */
	MV_REG_WRITE(DRAM_CTRL2_REG, reg);

	/* WL per CS */
	for (cs=0; cs<cs_num; cs++)
	{
		wl_val = 0;

		/* Enable Qoff for non-active CS */
		MV_REG_WRITE(DRAM_CTRL1_REG, 0x00000008);			/* set 0xD0800080[3] ==> Qoff enabled */
		MV_REG_WRITE(DRAM_USR_CMD_REG, ((0x02000000 >> cs) | 0x200)); 	/* Issue LMR1 write */

		/* Wait for MRS to finish - delay min 40 DRAM CC for MRS */
		uDelay(2);

		/* Enable write leveling for active CS */
		MV_REG_WRITE(DRAM_CTRL1_REG, 0x00000004);			/* set 0xD0800080[2] ==> Enable DDR WL mode */
		MV_REG_WRITE(DRAM_USR_CMD_REG, ((0x01000000 << cs) | 0x200));	/* Issue LMR1 write */

		/* Wait for MRS to finish - delay min 40 DRAM CC for MRS */
		uDelay(2);

		/* Loop on all 4 lanes */
		for (i=0; i<4; i++)
		{
			/* Select appropriate Shadow register per lane */
			MV_REG_WRITE(DRAM_CTRL_WL_SLCT_REG, ((0x100 << cs) + i)); 
			for (j=0; j<0x80; j++)
			{
				MV_REG_WRITE(DRAM_CTRL_WL_CTRL10_REG, ((j << 16) | j));
				MV_REG_WRITE(DRAM_CTRL15_REG, 0x80000000);
				reg = MV_REG_READ(BOOTROM_ROUTINE_ERR_REG);	// delay
				reg = MV_REG_READ(BOOTROM_ROUTINE_ERR_REG);	// delay
				reg = (MV_REG_READ(DRAM_CTRL15_REG) & (0x1 << i));
				if (!reg)
					break;
			}

			for (; j<0x80; j++)
			{
				MV_REG_WRITE(DRAM_CTRL_WL_CTRL10_REG, ((j << 16) | j));
				MV_REG_WRITE(DRAM_CTRL15_REG, 0x80000000);
				reg = MV_REG_READ(BOOTROM_ROUTINE_ERR_REG);	// delay
				reg = MV_REG_READ(BOOTROM_ROUTINE_ERR_REG);	// delay
				reg = (MV_REG_READ(DRAM_CTRL15_REG) & (0x1 << i));
				if (reg)
					break;
			}

			/* Save result to be written in SRAM for Uboot */
			wl_val |= (j << (i*8));
		}

		/* Save the result on the SRAM */
		MV_MEMIO_LE32_WRITE((0xC8010000 + (cs * 0x4)), wl_val);
		/* Save the same result on the SRAM for cs1 as well */
		if (cs_copy)
			MV_MEMIO_LE32_WRITE((0xC8010000 + ((cs+1) * 0x4)), wl_val);
	}	

	/* Disable Qoff */
	MV_REG_WRITE(DRAM_CTRL1_REG, 0x0);		/* clear 0xD0800080[3] ==> Qoff disabled */
	MV_REG_WRITE(DRAM_USR_CMD_REG, 0x01000200); 	/* Issue LMR1 write for CS0 */

	/* Wait for MRS to finish - delay min 40 DRAM CC for MRS */
	uDelay(2);

	/* Disable WL mode */
	MV_REG_WRITE(DRAM_CTRL1_REG, 0x0);		/* clear 0xD0800080[2] ==> Disable WL mode */
	MV_REG_WRITE(DRAM_USR_CMD_REG, 0x02000200); 	/* Issue LMR1 write for CS1 */

	/* Wait for MRS to finish - delay min 40 DRAM CC for MRS */
	uDelay(2);

	/* Exit MC WL mode */
	MV_REG_WRITE(DRAM_CTRL2_REG, ddr_ctrl2);

	/* Restore original pulldown on Phy */
	MV_REG_WRITE(DRAM_PHY_CTRL10_REG, pldwn);

	/* Restore original ODT configuration */
	MV_REG_WRITE(DRAM_PAD_ODT_CTRL_REG, odt);
}

/*
 * Write the DDR3 Write Leveling values on the specified CS #
 */
MV_VOID ddr3_wl_set(MV_U32 cs_idx, MV_U32 dly_4l)
{
	MV_U32 i, dly;

	/* Loop on all 4 lanes */
	for (i=0; i<4; i++)
	{
		/* extract the delay for the specific lane */		
		dly = ((dly_4l >> (i*8)) & 0xFF);
		dly |= (dly << 16);

		/* Select appropriate Shadow register per lane */
		MV_REG_WRITE(DRAM_CTRL_WL_SLCT_REG, ((0x100 << cs_idx) + i)); 		

		/* Set the appropriate value */
		MV_REG_WRITE(DRAM_CTRL_WL_CTRL10_REG, dly);
	}
}

/*
 * Execute DDR3 WL if needed
 */
MV_VOID ddr3_wl_exec(MV_U8 typ)
{
	switch (typ)
	{
		case 0x01:
			ddr3_wl(0x1, 0);
			break;
		case 0x02:
			ddr3_wl(0x2, 0);
			break;
		case 0x03:
			ddr3_wl(0x1, 1);
			ddr3_wl_set(0x1, MV_MEMIO_LE32_READ(0xC8010000));	// copy CS#0 to CS#1
			break;

		default:
			break;
	};
}

/*
 * Perfomr the M_RESET and CKE workaround for the Standby
 */
MV_VOID standby_wa(MV_U8 m_reset_mpp, MV_U8 cke_mpp, MV_U8 wa_delay)
{
	MV_U32 reg, reg1;

	reg = MV_REG_READ(PMU_MPP_CTRL_REG);
	reg1 = MV_REG_READ(PMU_SIG_SLCT_REG);	

	if (wa_delay) 
		uDelay(wa_delay * 2000);

	/* Convert M_RESET MPP to PMU domain and LOW - Asserted */
	if (m_reset_mpp) /* W/A should be implemented on PMU MPP */
	{
		reg |= (0x1 << (m_reset_mpp-1));
		reg1 |= (0x1 << ((m_reset_mpp-1) * 4));

		MV_REG_WRITE(PMU_MPP_CTRL_REG, reg);
		MV_REG_WRITE(PMU_SIG_SLCT_REG, reg1);
	
		uDelay(300);

		/* Release M_RESET MPP to be pulled up - Deasserted */
		reg1 &= ~(0xF << ((m_reset_mpp-1) * 4));
		reg1 |= (0x2 << ((m_reset_mpp-1) * 4));
		MV_REG_WRITE(PMU_SIG_SLCT_REG, reg1);

		/* Wait 600us before initialization */
		uDelay(600);
	}

	/* Convert CKE MPP to PMU domain and release it to Dove control */
	if (cke_mpp) /* W/A should be implemented on PMU MPP */
	{
		reg |= (0x1 << (cke_mpp-1));
		reg1 |= (0x1 << ((cke_mpp-1) * 4));
	

		MV_REG_WRITE(PMU_MPP_CTRL_REG, reg);
		MV_REG_WRITE(PMU_SIG_SLCT_REG, reg1);	
	}
}

void ddr3_wl2cs(MV_U32 ddr_wl_type, MV_U32 m_reset_mpp, MV_U32 cke_mpp, MV_U32 wa_delay)
{
	MV_U32 reg;

	/* Perform DDR3 workaround */
	standby_wa((MV_U8)m_reset_mpp, (MV_U8)cke_mpp, (MV_U8)wa_delay);

	/* Trigger DDR initialization */
	MV_REG_WRITE(DRAM_USR_CMD_REG, 0x1);
		
	/* Poll init done bit */
	do
	{
		reg = ((MV_MEMIO_LE32_READ(DRAM_STATUS_REG)) & DRAM_INIT_DONE_MASK);
	} while (reg == 0x0);

	reg = MV_REG_READ(SAMPLE_RESET);

	/* Check of DDR3 Write Leveling and execute it if needed */
	if ((ddr_wl_type) && (reg & 0x10000000))
		ddr3_wl_exec(ddr_wl_type);
}
