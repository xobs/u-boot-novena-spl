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


/* includes */
#include "mvCommon.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "../mvDramIf.h"
#include "mvDramIfRegs.h"
#include "mvDramIfConfig.h"
#include "mvDramIfConfig.h"
#include "mvDramIfRegs.h"


#ifdef MV_DEBUG
#define DB(x) x
#else
#define DB(x)
#endif

/* DRAM bank presence encoding */
#define BANK_PRESENT_CS0			0x1
#define BANK_PRESENT_CS0_CS1			0x3
#define BANK_PRESENT_CS0_CS2			0x5
#define BANK_PRESENT_CS0_CS1_CS2		0x7
#define BANK_PRESENT_CS0_CS2_CS3		0xd
#define BANK_PRESENT_CS0_CS2_CS3_CS4		0xf

/* locals   */
static MV_BOOL sdramIfWinOverlap(MV_TARGET target, MV_ADDR_WIN *pAddrWin);
#if defined(MV_INC_BOARD_DDIM)
static void sdramDDrOdtConfig(MV_DRAM_BANK_INFO *pBankInfo);
static MV_U32 sdramCtrlReg4Calc(MV_U32 minCas, MV_U32 busClk);
static MV_U32 sdramConfigRegCalc(MV_DRAM_BANK_INFO *pBankInfo);
static MV_U32 minCasCalc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32 busClk, 
						 MV_U32 forcedCl);
static MV_U32 sdramTimingReg1Calc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32 minCas, MV_U32 busClk);
static MV_U32 sdramTimingReg2Calc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32 minCas, MV_U32 busClk);
static MV_U32 sdramTimingReg3Calc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32 minCas, MV_U32 busClk);
static MV_U32 sdramTimingReg4Calc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32 minCas, MV_U32 busClk);
static MV_U32 sdramTimingReg5Calc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32 minCas, MV_U32 busClk);
static MV_U32 sdramTimingReg6Calc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32 minCas, MV_U32 busClk);

/*******************************************************************************
* mvDramIfDetect - Prepare DRAM interface configuration values.
*
* DESCRIPTION:
*       This function implements the full DRAM detection and timing 
*       configuration for best system performance.
*       Since this routine runs from a ROM device (Boot Flash), its stack 
*       resides on RAM, that might be the system DRAM. Changing DRAM 
*       configuration values while keeping vital data in DRAM is risky. That
*       is why the function does not preform the configuration setting but 
*       prepare those in predefined 32bit registers (in this case IDMA 
*       registers are used) for other routine to perform the settings.
*       The function will call for board DRAM SPD information for each DRAM 
*       chip select. The function will then analyze those SPD parameters of 
*       all DRAM banks in order to decide on DRAM configuration compatible 
*       for all DRAM banks.
*       The function will set the CPU DRAM address decode registers.
*       Note: This routine prepares values that will overide configuration of
*       mvDramBasicAsmInit().
*       
* INPUT:
*       forcedCl - Forced CAL Latency. If equal to zero, do not force.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_STATUS mvDramIfDetect(MV_U32 forcedCl)
{
	MV_U32 retVal = MV_OK;	/* return value */
	MV_DRAM_BANK_INFO bankInfo[MV_DRAM_MAX_CS];
	MV_U32  busClk, size, base = 0, i, temp, deviceW, dimmW;
	MV_U8	minCas;
	MV_DRAM_DEC_WIN dramDecWin;

	dramDecWin.addrWin.baseHigh = 0;

	busClk = mvBoardSysClkGet();
	
	if (0 == busClk)
	{
		mvOsPrintf("Dram: ERR. Can't detect system clock! \n");
		return MV_ERROR;
	}
	
	/* Close DRAM banks except bank 0 (in case code is excecuting from it...) */
#if defined(MV_INCLUDE_SDRAM_CS1)
	for(i= SDRAM_CS1; i < MV_DRAM_MAX_CS; i++)
		mvCpuIfTargetWinEnable(i, MV_FALSE);
#endif

	/* we will use bank 0 as the representative of the all the DRAM banks,  */
	/* since bank 0 must exist.                                             */	
	for(i = 0; i < MV_DRAM_MAX_CS; i++)
	{ 
		/* if Bank exist */
		if(MV_OK == mvDramBankInfoGet(i, &bankInfo[i]))
		{
			/* check it isn't SDRAM */
			if(bankInfo[i].memoryType == MEM_TYPE_SDRAM)
			{
				mvOsPrintf("Dram: ERR. SDRAM type not supported !!!\n");
				return MV_ERROR;
			}
			/* All banks must support registry in order to activate it */
			if(bankInfo[i].registeredAddrAndControlInputs != 
			   bankInfo[0].registeredAddrAndControlInputs)
			{
				mvOsPrintf("Dram: ERR. different Registered settings !!!\n");
				return MV_ERROR;
			}

			/* Init the CPU window decode */
			/* Note that the size in Bank info is in MB units 			*/
			/* Note that the Dimm width might be different then the device DRAM width */
			temp = MV_REG_READ(SDRAM_CTRL_4_REG);
			
			deviceW = ((temp & SDRAM_CTRL4_DWIDTH_MASK) == SDRAM_CTRL4_DWIDTH_16BIT )? 16 : 32;
			dimmW = bankInfo[i].dataWidth - (bankInfo[i].dataWidth % 16);
			size = ((bankInfo[i].size << 20) / (dimmW/deviceW)); 

			/* We can not change DRAM window settings while excecuting  	*/
			/* code from it. That is why we skip the DRAM CS[0], saving     */
			/* it to the ROM configuration routine	*/
			if(i == SDRAM_CS0)
			{
				MV_U32 sizeToReg;
				
				/* Translate the given window size to register format */
				sizeToReg = ctrlSizeToReg(size, SCSR_SIZE_ALIGNMENT);

				/* Size parameter validity check. */
				if (-1 == sizeToReg)
				{
					mvOsPrintf("mvCtrlAddrDecToReg: ERR. Win %d size invalid.\n"
							   ,i);
					return MV_BAD_PARAM;
				}
                
				/* Size is located at upper 16 bits */
				sizeToReg <<= SCSR_SIZE_OFFS;

				/* enable it */
				sizeToReg |= SCSR_WIN_EN;

				MV_REG_WRITE(DRAM_BUF_REG0, sizeToReg);
			}
			else
			{
				dramDecWin.addrWin.baseLow = base;
				dramDecWin.addrWin.size = size;
				dramDecWin.enable = MV_TRUE;
				
				if (MV_OK != mvDramIfWinSet(SDRAM_CS0 + i, &dramDecWin))
				{
					mvOsPrintf("Dram: ERR. Fail to set bank %d!!!\n", 
							   SDRAM_CS0 + i);
					return MV_ERROR;
				}
			}
			
			base += size;

			/* update the suportedCasLatencies mask */
			bankInfo[0].suportedCasLatencies &= bankInfo[i].suportedCasLatencies;

		}
		else
		{
			if( i == 0 ) /* bank 0 doesn't exist */
			{
				mvOsPrintf("Dram: ERR. Fail to detect bank 0 !!!\n");
				return MV_ERROR;
			}
			else
			{
				DB(mvOsPrintf("Dram: Could not find bank %d\n", i));
				bankInfo[i].size = 0;     /* Mark this bank as non exist */
			}
		}
	}

	/* calculate minimum CAS */
	minCas = minCasCalc(&bankInfo[0], busClk, forcedCl);
	if (0 == minCas) 
	{
		mvOsOutput("Dram: Warn: Could not find CAS compatible to SysClk %dMhz\n",
				   (busClk / 1000000));

		if (MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DTYPE_DDR3)
		{
			minCas = DDR3_CL_6; /* Continue with this CAS */
			mvOsPrintf("Set default CAS latency 6\n");
		}
		else if (MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DTYPE_DDR2)
		{
			minCas = DDR2_CL_4; /* Continue with this CAS */
			mvOsPrintf("Set default CAS latency 4\n");
		}
		else
		{
			minCas = DDR1_CL_3; /* Continue with this CAS */
			mvOsPrintf("Set default CAS latency 3\n");
		}
	}

	/* calc SDRAM_CONFIG_REG  and save it to temp register */
	temp = sdramConfigRegCalc(&bankInfo[0]);
	if(-1 == temp)
	{
		mvOsPrintf("Dram: ERR. sdramConfigRegCalc failed !!!\n");
		return MV_ERROR;
	}
	MV_REG_WRITE(DRAM_BUF_REG9, temp);

	/* calc SDRAM_CONTROL_REG4  and save it to temp register */ 
	temp = sdramCtrlReg4Calc(minCas, MV_U32 busClk);
	if(-1 == temp)
	{
		mvOsPrintf("Dram: ERR. sdramCtrlReg4Calc failed !!!\n");
		return MV_ERROR;
	}
	MV_REG_WRITE(DRAM_BUF_REG10, temp);

	/* calc SDRAM_TIMING_REG_1  and save it to temp register */
	temp = sdramTimingReg1Calc(&bankInfo[0], minCas, busClk);
	if(-1 == temp)
	{
		mvOsPrintf("Dram: ERR. sdramTimingReg1Calc failed !!!\n");
		return MV_ERROR;
	}
	MV_REG_WRITE(DRAM_BUF_REG1, temp);

	/* calc SDRAM_TIMING_REG_2  and save it to temp register */
	temp = sdramTimingReg2Calc(&bankInfo[0], minCas, busClk);
	if(-1 == temp)
	{
		mvOsPrintf("Dram: ERR. sdramTimingReg2Calc failed !!!\n");
		return MV_ERROR;
	}
	MV_REG_WRITE(DRAM_BUF_REG2, temp);

	/* calc SDRAM_TIMING_REG_3  and save it to temp register */
	temp = sdramTimingReg3Calc(&bankInfo[0], minCas, busClk);
	if(-1 == temp)
	{
		mvOsPrintf("Dram: ERR. sdramTimingReg2Calc failed !!!\n");
		return MV_ERROR;
	}
	MV_REG_WRITE(DRAM_BUF_REG3, temp);

	/* calc SDRAM_TIMING_REG_4  and save it to temp register */
	temp = sdramTimingReg4Calc(&bankInfo[0], minCas, busClk);
	if(-1 == temp)
	{
		mvOsPrintf("Dram: ERR. sdramTimingReg2Calc failed !!!\n");
		return MV_ERROR;
	}
	MV_REG_WRITE(DRAM_BUF_REG4, temp);

	/* calc SDRAM_TIMING_REG_5  and save it to temp register */
	temp = sdramTimingReg5Calc(&bankInfo[0], minCas, busClk);
	if(-1 == temp)
	{
		mvOsPrintf("Dram: ERR. sdramTimingReg2Calc failed !!!\n");
		return MV_ERROR;
	}
	MV_REG_WRITE(DRAM_BUF_REG5, temp);

	/* calc SDRAM_TIMING_REG_6  and save it to temp register */
	temp = sdramTimingReg6Calc(&bankInfo[0], minCas, busClk);
	if(-1 == temp)
	{
		mvOsPrintf("Dram: ERR. sdramTimingReg2Calc failed !!!\n");
		return MV_ERROR;
	}
	MV_REG_WRITE(DRAM_BUF_REG6, temp);

	/* Config DDR3/2 On Die Termination (ODT) registers */
	if ( (MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DTYPE_DDR3) ||
		(MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DTYPE_DDR2) )
	{
		sdramDDrOdtConfig(bankInfo);
	}
	
	/* Note that DDR SDRAM Address/Control and Data pad calibration     */
	/* settings is done in mvSdramIfConfig.s                            */

	return retVal;
}

/*******************************************************************************
* minCasCalc - Calculate the Minimum CAS latency which can be used.
*
* DESCRIPTION:
*	Calculate the minimum CAS latency that can be used, base on the DRAM
*	parameters and the SDRAM bus Clock freq.
*
* INPUT:
*	busClk    - the DRAM bus Clock.
*	pBankInfo - bank info parameters.
*
* OUTPUT:
*       None
*
* RETURN:
*       The minimum CAS Latency. The function returns 0 if max CAS latency
*		supported by banks is incompatible with system bus clock frequancy.
*
*******************************************************************************/
static MV_U32 minCasCalc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32 busClk, 
						 MV_U32 forcedCl)
{
	MV_U32 count = 1, j;
	MV_U32 busClkPs = 1000000000 / (busClk / 1000);  /* in ps units */
	MV_U32 startBit, stopBit;
	
	/*     DDR 1:
			*******-******-******-******-******-******-******-******* 
			* bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 * 
			*******-******-******-******-******-******-******-******* 
	CAS	=	* TBD  |  4   | 3.5  |   3  | 2.5  |  2   | 1.5  |   1  * 
			*********************************************************/
	
	/*     DDR 2:
			*******-******-******-******-******-******-******-******* 
			* bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 * 
			*******-******-******-******-******-******-******-******* 
	CAS	=	* TBD  |  6   |  5   |  4   |  3   |  2   | TBD  | TBD  * 
			*********************************************************/
	/*     DDR 3:
			*******-******-******-******-******-******-******-******* 
			* bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 * 
			*******-******-******-******-******-******-******-******* 
	CAS	=	* TBD  | 10   |  9   |  8   |  7   |  6   |  5   | TBD  * 
			*********************************************************/
	
	
	/* If we are asked to use the forced CAL */
	if (forcedCl)
	{
		mvOsPrintf("DRAM: Using forced CL %d.%d\n", (forcedCl / 10), (forcedCl % 10));
	
		if (MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DTYPE_DDR3)
		{
			if (forcedCl == 50)
				pBankInfo->suportedCasLatencies = 0x02;
			else if (forcedCl == 60)
				pBankInfo->suportedCasLatencies = 0x04;
			else if (forcedCl == 70)
				pBankInfo->suportedCasLatencies = 0x08;
			else if (forcedCl == 80)
				pBankInfo->suportedCasLatencies = 0x10;
			else if (forcedCl == 90)
				pBankInfo->suportedCasLatencies = 0x20;
			else if (forcedCl == 100)
				pBankInfo->suportedCasLatencies = 0x40;
			else
			{
				mvOsPrintf("Forced CL %d.%d not supported. Set default CL 6\n", 
						   (forcedCl / 10), (forcedCl % 10));
				pBankInfo->suportedCasLatencies = 0x4;
			}
		}
		else if (MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DTYPE_DDR2)
		{
			if (forcedCl == 30)
				pBankInfo->suportedCasLatencies = 0x08;
			else if (forcedCl == 40)
				pBankInfo->suportedCasLatencies = 0x10;
			else if (forcedCl == 50)
				pBankInfo->suportedCasLatencies = 0x20;
			else
			{
				mvOsPrintf("Forced CL %d.%d not supported. Set default CL 4\n", 
						   (forcedCl / 10), (forcedCl % 10));
				pBankInfo->suportedCasLatencies = 0x10;
			}
		}
		else
		{
			if (forcedCl == 15)
				pBankInfo->suportedCasLatencies = 0x02;
			else if (forcedCl == 20)
				pBankInfo->suportedCasLatencies = 0x04;
			else if (forcedCl == 25)
				pBankInfo->suportedCasLatencies = 0x08;
			else if (forcedCl == 30)
				pBankInfo->suportedCasLatencies = 0x10;
			else if (forcedCl == 40)
				pBankInfo->suportedCasLatencies = 0x40;
			else
			{
				mvOsPrintf("Forced CL %d.%d not supported. Set default CL 3\n", 
						   (forcedCl / 10), (forcedCl % 10));
				pBankInfo->suportedCasLatencies = 0x10;
			}
		}
	
		return pBankInfo->suportedCasLatencies;        
	}   
	
	/* go over the supported cas mask from Max Cas down and check if the 	*/
	/* SysClk stands in its time requirments.								*/
	
	
	DB(mvOsPrintf("Dram: minCasCalc supported mask = %x busClkPs = %x \n",
								pBankInfo->suportedCasLatencies,busClkPs ));
	for(j = 7; j > 0; j--)
	{
		if((pBankInfo->suportedCasLatencies >> j) & BIT0 )
		{
			/* Reset the bits for CL incompatible for the sysClk            */
			switch (count)
			{
				case 1: 
					if (pBankInfo->minCycleTimeAtMaxCasLatPs > busClkPs) 
						pBankInfo->suportedCasLatencies &= ~(BIT0 << j);
					count++;
					break;
				case 2: 
					if (pBankInfo->minCycleTimeAtMaxCasLatMinus1Ps > busClkPs)
						pBankInfo->suportedCasLatencies &= ~(BIT0 << j);
					count++;
					break;
				case 3: 
					if (pBankInfo->minCycleTimeAtMaxCasLatMinus2Ps > busClkPs)
						pBankInfo->suportedCasLatencies &= ~(BIT0 << j);
					count++;
					break;
				default: 
					pBankInfo->suportedCasLatencies &= ~(BIT0 << j);
					break;
			}
		}
	}
	
	DB(mvOsPrintf("Dram: minCasCalc support = %x (after SysCC calc)\n",
				  pBankInfo->suportedCasLatencies ));
	
	/* SDRAM DDR1 controller supports CL 1.5 to 3.5 */
	/* SDRAM DDR2 controller supports CL 3 to 5     */
	/* SDRAM DDR3 controller supports CL 5 to 10     */
	if (MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DTYPE_DDR3)
	{
		startBit = 1;   /* DDR3 support CL start with CL5 (bit 1) */
		stopBit  = 6;   /* DDR3 support CL stops with CL10 (bit 6)   */
	}
	else if (MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DTYPE_DDR2)
	{
		startBit = 3;   /* DDR2 support CL start with CL3 (bit 3) */
		stopBit  = 5;   /* DDR2 support CL stops with CL5 (bit 5) */
	}
	else
	{
		startBit = 1;   /* DDR1 support CL start with CL1.5 (bit 3) */
		stopBit  = 4;   /* DDR1 support CL stops with CL3 (bit 4)   */
	}
	
	for(j = startBit; j <= stopBit ; j++)
	{
		if((pBankInfo->suportedCasLatencies >> j) & BIT0 )
		{
			DB(mvOsPrintf("Dram: minCasCalc choose CAS %x \n",(BIT0 << j)));
			return (BIT0 << j);
		}
	}
	
	return 0; 
}

/*******************************************************************************
* sdramConfigRegCalc - Calculate sdram config register
*
* DESCRIPTION: Calculate sdram config register optimized value based
*			on the bank info parameters.
*
* INPUT:
*	pBankInfo - sdram bank parameters
*
* OUTPUT:
*       None
*
* RETURN:
*       sdram config reg value.
*
*******************************************************************************/
static MV_U32 sdramConfigRegCalc(MV_DRAM_BANK_INFO *pBankInfo)
{
	MV_U32 sdramConfig = 0;
	MV_U32 populateBanks = 0;
	
	populateBanks = sdramPopulateBanks(pBankInfo);

	sdramConfig = MV_REG_READ(SDRAM_CFG_REG(csNum));
	
	/* Set new value to RTT field */
	sdramConfig  &= SDRAM_CFG_RTT_MASK;
	/* DDR3 RTT */
	if (MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DTYPE_DDR3)
	{
		switch(populateBanks)
		{
			case(BANK_PRESENT_CS0):
			case(BANK_PRESENT_CS0_CS1):
				sdramConfig  |= SDRAM_CFG_RTT_120;
				break;
			default:
				mvOsPrintf("sdramRTT: Invalid DRAM bank presence\n");
				return;
		} 
	}
	/* DDR2 RTT */
	else if (MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DTYPE_DDR2)
	{
		switch(populateBanks)
		{
			case(BANK_PRESENT_CS0):
			case(BANK_PRESENT_CS0_CS1):
				sdramConfig  |= SDRAM_CFG_DDR2_RTT_150;
				break;
			default:
				mvOsPrintf("sdramRTT: Invalid DRAM bank presence\n");
				return;
		} 
	}

	/* DDR3 DRAM number of bank is 8*/
	if (MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DTYPE_DDR3)
	{
		sdramConfig |= SDRAM_CFG_8_BANK_NUM;
		mvOsPrintf("DRAM num of bank: 8.\n");
	}
	else
	{
		/* DDR2 num of bank is as in SPD/4 */
		sdramConfig &= ~SDRAM_CFG_BANK_NUM_MASK;
		sdramConfig |= ((pBankInfo->numOfBanksOnEachDevice) << (SDRAM_CFG_BANK_NUM_OFFS - 2));
		mvOsPrintf("DRAM num of bank: 0x%x.\n",
			((pBankInfo->numOfBanksOnEachDevice) << (SDRAM_CFG_BANK_NUM_OFFS - 2)));
	}
	
	/* DRAM number of row address*/
	sdramConfig &= ~SDRAM_CFG_ROW_NUM_MASK;
	sdramConfig |= ((pBankInfo->numOfRowAddr - 0x10) << (SDRAM_CFG_ROW_NUM_OFFS));
	mvOsPrintf("DRAM num of row: 0x%x.\n",
			((pBankInfo->numOfRowAddr - 0x10) << (SDRAM_CFG_ROW_NUM_OFFS)) );
	/* DRAM number of col address*/
	sdramConfig &= ~SDRAM_CFG_COL_NUM_MASK;
	sdramConfig |= ((pBankInfo->numOfColAddr - 0x7) << (SDRAM_CFG_COL_NUM_OFFS));
	mvOsPrintf("DRAM num of col: 0x%x.\n",
			((pBankInfo->numOfColAddr - 0x7) << (SDRAM_CFG_COL_NUM_OFFS)) );

	DB(mvOsPrintf("Dram: sdramConfigRegCalc set sdramConfig to 0x%x\n",
				  sdramConfig));
	
	return sdramConfig;  
}

/*******************************************************************************
* sdramCtrlReg4Calc - Calculate sdram mode register
*
* DESCRIPTION: Calculate sdram mode register optimized value based
*			on the bank info parameters and the minCas.
*
* INPUT:
*	minCas	  - minimum CAS supported. 
*       busClk    - Bus clock
*
* OUTPUT:
*       None
*
* RETURN:
*       sdram mode reg value.
*
*******************************************************************************/
static MV_U32 sdramCtrlReg4Calc(MV_U32 minCas, MV_U32 busClk)
{
	MV_U32 sdramCtrl4;
	MV_U32 cwl = 0;
	
	sdramCtrl4 = MV_REG_READ(SDRAM_CTRL_4_REG);
	/* Clear CAS Latency field */
	sdramCtrl4 &= ~SDRAM_CTRL4_CL_MASK;
	
	mvOsPrintf("DRAM CAS Latency ");
	if (sdramCtrl4 & SDRAM_CTRL4_DTYPE_DDR3)
	{
		switch (minCas)
		{
			case DDR3_CL_5: 
				sdramCtrl4 |= SDRAM_DDR3_CL_5;
				mvOsPrintf("5.\n");
				break;
			case DDR3_CL_6: 
				sdramCtrl4 |= SDRAM_DDR3_CL_6;
				mvOsPrintf("6.\n");
				break;
			case DDR4_CL_7: 
				sdramCtrl4 |= SDRAM_DDR3_CL_7;
				mvOsPrintf("7.\n");
				break;
			case DDR5_CL_8: 
				sdramCtrl4 |= SDRAM_DDR3_CL_8;
				mvOsPrintf("8.\n");
				break;
			case DDR5_CL_9: 
				sdramCtrl4 |= SDRAM_DDR3_CL_9;
				mvOsPrintf("9.\n");
				break;
			case DDR5_CL_10: 
				sdramCtrl4 |= SDRAM_DDR3_CL_10;
				mvOsPrintf("10.\n");
				break;
			case DDR5_CL_10: 
				sdramCtrl4 |= SDRAM_DDR3_CL_11;
				mvOsPrintf("11.\n");
				break;
			default:
				mvOsPrintf("\nsdramCtrlReg4Calc ERROR: Max. CL out of range\n");
				return -1;
		}
		
		/* JEDEC Calc CWL DDR3 Only */
		if ( busClk =< 400000000)
			cwl = 0;
		else if ( (busClk > 400000000) && (busClk =< 533333333) )
			cwl = 1;
		else if ( (busClk > 533333333) && (busClk =< 666666667) )
			cwl = 2;
		else if ( (busClk > 666666667) && (busClk =< 800000000) )
			cwl = 3;
	}
	else if (sdramCtrl4 & SDRAM_CTRL4_DTYPE_DDR2)
	{
		switch (minCas)
		{
			case DDR2_CL_3: 
				sdramCtrl4 |= SDRAM_DDR2_CL_3;
				mvOsPrintf("3.\n");
				break;
			case DDR2_CL_4: 
				sdramCtrl4 |= SDRAM_DDR2_CL_4;
				mvOsPrintf("4.\n");
				break;
			case DDR2_CL_5: 
				sdramCtrl4 |= SDRAM_DDR2_CL_5;
				mvOsPrintf("5.\n");
				break;
			case DDR2_CL_6: 
				sdramCtrl4 |= SDRAM_DDR2_CL_6;
				mvOsPrintf("6.\n");
				break;
			default:
				mvOsPrintf("\nsdramCtrlReg4Calc ERROR: Max. CL out of range\n");
				return -1;
		}
	}
	else	/* DDR1 */
	{
		DB(mvOsPrintf("\nsdramCtrlReg4Calc register DDR1 TODO\n"));
	}
	
	DB(mvOsPrintf("\nsdramCtrlReg4Calc register 0x%x\n", sdramMode ));

	return sdramMode;
}

/*******************************************************************************
* sdramExtModeRegCalc - Calculate sdram Extended mode register
*
* DESCRIPTION: 
*		Return sdram Extended mode register value based
*		on the bank info parameters and bank presence.
*
* INPUT:
*	pBankInfo - sdram bank parameters
*
* OUTPUT:
*       None
*
* RETURN:
*       sdram Extended mode reg value.
*
*******************************************************************************/
static MV_U32 sdramExtModeRegCalc(MV_DRAM_BANK_INFO *pBankInfo)
{
	MV_U32 populateBanks = 0;
	int bankNum;
	if (MV_REG_READ(SDRAM_CONFIG_REG) & SDRAM_DTYPE_DDR2)
	{
	/* Represent the populate banks in binary form */
	for(bankNum = 0; bankNum < MV_DRAM_MAX_CS; bankNum++)
	{
		if (0 != pBankInfo[bankNum].size)
		{
				populateBanks |= (1 << bankNum);
			}
		}
	
		switch(populateBanks)
		{
			case(BANK_PRESENT_CS0):
				return DDR_SDRAM_EXT_MODE_CS0_DV;
		
			case(BANK_PRESENT_CS0_CS1):
				return DDR_SDRAM_EXT_MODE_CS0_DV;
		
			case(BANK_PRESENT_CS0_CS2):
				return DDR_SDRAM_EXT_MODE_CS0_CS2_DV;
		
			case(BANK_PRESENT_CS0_CS1_CS2):
				return DDR_SDRAM_EXT_MODE_CS0_CS2_DV;
		
			case(BANK_PRESENT_CS0_CS2_CS3):
				return DDR_SDRAM_EXT_MODE_CS0_CS2_DV;
		
			case(BANK_PRESENT_CS0_CS2_CS3_CS4):
				return DDR_SDRAM_EXT_MODE_CS0_CS2_DV;
		
			default:
				mvOsPrintf("sdramExtModeRegCalc: Invalid DRAM bank presence\n");
				return -1;
		} 
	}
	return 0;
}
                                                                    
/*******************************************************************************
* sdramTimingReg1Calc - Calculate sdram timing register 1
*
* DESCRIPTION: 
*       This function calculates sdram timing register 1
*       optimized value based on the bank info parameters and the minCas.
*
* INPUT:
*	pBankInfo - sdram bank parameters
*       busClk    - Bus clock
*
* OUTPUT:
*       None
*
* RETURN:
*       sdram timing reg 1 value.
*
*******************************************************************************/
static MV_U32 sdramTimingReg1Calc(MV_DRAM_BANK_INFO *pBankInfo, 
                                                MV_U32 minCas, MV_U32 busClk)
{
	MV_U32 tCcd  = 0;
	MV_U32 tRtp = 0;
	MV_U32 tWtr = 0;
	MV_U32 tRc = 0;
	MV_U32 tRefi = 0;
	
	MV_U32 bankNum;
	
	busClk = busClk / 1000000;    /* In MHz */
	
	/* figure out the memory refresh internal */
	switch (pBankInfo.refreshInterval & 0xf)
	{
		case 0x0: /* refresh period is 15.625 usec */
				tRefi = 15625;
				break;
		case 0x1: /* refresh period is 3.9 usec  	*/
				tRefi = 3900;
				break;
		case 0x2: /* refresh period is 7.8 usec 	*/
				tRefi = 7800;
				break;
		case 0x3: /* refresh period is 31.3 usec	*/
				tRefi = 31300;
				break;
		case 0x4: /* refresh period is 62.5 usec	*/
				tRefi = 62500;
				break;
		case 0x5: /* refresh period is 125 usec 	*/
				tRefi = 125000;
				break;
		default:  /* refresh period undefined 					*/
				mvOsPrintf("Dram: ERR. DRAM refresh period is unknown!\n");
				return -1;
    	}

	/* Now the refreshPeriod is in register format value */
	tRefi = (busClk * tRefi) / 1000;
	
	DB(mvOsPrintf("Dram: sdramTimingReg1Calc calculated refresh interval %0x\n", 
				  tRefi));
	
	tRc  = pBankInfo.minActivateToActiveCmd;
	tWtr = pBankInfo.minWriteToReadCmdDelay;
	tRtp = pBankInfo.minReadToPrechCmdDelay;

	if (MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DTYPE_DDR3)
	{
		/* JEDEC min reqeirments tccd = 4 */
		tCcd  = 4;
		DB(mvOsPrintf("tCcd = %d ", tCcd));

		/* Extract clock cycles from time parameter. We need to round up    */
		tRc = ((busClk * tRc) / 1000) + (((busClk * tRc) % 1000) ? 1 : 0);
		DB(mvOsPrintf("tRc = %d ", tRc));
		tWtr = ((busClk * tWtr) / 1000) + (((busClk * tWtr) % 1000) ? 1 : 0);
		/* JEDEC min reqeirments tWtr = 4 */
		if (tWtr < 4)
			tWtr = 4;
		DB(mvOsPrintf("tWtr = %d ", tWtr));
		tRtp = ((busClk * tRtp) / 1000) + (((busClk * tRtp) % 1000) ? 1 : 0);
		/* JEDEC min reqeirments tRtp = 4 */
		if (tRtp < 4)
			tRtp = 4;
		DB(mvOsPrintf("tRtp = %d ", tRtp));
	}
	else if (MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DTYPE_DDR2)
	{
		/* Extract timing (in ns) from SPD value. We ignore the tenth ns    */
		/* part by shifting the data two bits right.                        */
		tWtr  = tWtr  >> 2;  /* For example 0x50 -> 20ns                    */
		tRtp = tRtp >> 2;
		tRc = tRc >> 2;
	
		/* Extract clock cycles from time parameter. We need to round up    */
		tWtr = ((busClk * tWtr) / 1000) + (((busClk * tWtr) % 1000) ? 1 : 0);
		/* JEDEC min reqeirments tWtr = 2 */
		if (tWtr < 2)
			tWtr = 2;
		DB(mvOsPrintf("tWtr = %d ", tWtr));
		tRtp = ((busClk * tRtp) / 1000) + (((busClk * tRtp) % 1000) ? 1 : 0);
		/* JEDEC min reqeirments tRtp = 2 */
		if (tRtp < 2)
			tRtp = 2;
		DB(mvOsPrintf("tRtp = %d ", tRtp));
		tRc = ((busClk * tRc) / 1000) + (((busClk * tRc) % 1000) ? 1 : 0);
		DB(mvOsPrintf("tRc = %d ", tRc));
		/* JEDEC min reqeirments tccd = 2 */
		tCcd  = 2;
		DB(mvOsPrintf("tCcd = %d ", tCcd));

	
	
	}
	else
	{    
		if ((200 == busClk) || ((100 == busClk) && (DDR1_CL_1_5 == minCas)))
		{
			tWtr = 2;
		}
		else
		{
			tWtr = 1;
		}
		
		tRtp = 2; /* Must be set to 0x1 (two cycles) when using DDR1 */
	}
	
	DB(mvOsPrintf("tWtr = %d\n", tWtr));
	
	return ((tCcd << SDRAM_TIMING1_TCCD_OFFS)|
			(tRtp << SDRAM_TIMING1_TRTP_OFFS)|
			(tWtr << SDRAM_TIMING1_TWTR_OFFS)|
			(tRc << SDRAM_TIMING1_TRC_OFFS)	 |
			(tRefi << SDRAM_TIMING1_TREFI_OFFS));
}

/*******************************************************************************
* sdramTimingReg2Calc - Calculate sdram timing register 2
*
* DESCRIPTION: 
*       This function calculates sdram timing register 2
*       optimized value based on the bank info parameters and the minCas.
*
* INPUT:
*	pBankInfo - sdram bank parameters
*       busClk    - Bus clock
*
* OUTPUT:
*       None
*
* RETURN:
*       sdram timing reg 2 value.
*
*******************************************************************************/
static MV_U32 sdramTimingReg2Calc(MV_DRAM_BANK_INFO *pBankInfo, 
                                                MV_U32 minCas, MV_U32 busClk)
{
	MV_U32 tRp  = 0;
	MV_U32 tRrd = 0;
	MV_U32 tRcd = 0;
	MV_U32 tWr  = 0;
	MV_U32 tRfc = 0;
	MV_U32 tMrd = 0;
	
	MV_U32 bankNum;
	
	busClk = busClk / 1000000;    /* In MHz */
	
	tRp  = pBankInfo.minRowPrechargeTime;
	tRrd = pBankInfo.minRowActiveToRowActive;
	tRcd = pBankInfo.minRasToCasDelay;
	tWr  = pBankInfo.minWriteRecoveryTime;
	tRfc = pBankInfo.minRefreshToActiveCmd;

	if (MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DTYPE_DDR3)
	{
		/* Extract clock cycles from time parameter. We need to round up        */
		tRp  = ((busClk * tRp)  / 1000) + (((busClk * tRp)  % 1000) ? 1 : 0);
		DB(mvOsPrintf("tRp = %d ", tRp));
		tRrd = ((busClk * tRrd) / 1000) + (((busClk * tRrd) % 1000) ? 1 : 0);
		/* JEDEC min reqeirments tRrd = 4 */
		if (tRrd < 4)
			tRrd = 4;
		DB(mvOsPrintf("tRrd = %d ", tRrd));
		tRcd = ((busClk * tRcd) / 1000) + (((busClk * tRcd) % 1000) ? 1 : 0);
		DB(mvOsPrintf("tRcd = %d ", tRcd));
		/* JEDEC min reqeirments tWr = 15nSec */
		tWr  = 0xF;
		tWr  = ((busClk * tWr)  / 1000) + (((busClk * tWr)  % 1000) ? 1 : 0);
		DB(mvOsPrintf("tWr = %d ", tWr));
		tRfc  = ((busClk * tRfc)  / 1000) + (((busClk * tRfc)  % 1000) ? 1 : 0);
		DB(mvOsPrintf("tRfc = %d ", tRfc));
		/* JEDEC min reqeirments tMrd = 4 */
		tMrd  = 0x4;
		DB(mvOsPrintf("tMrd = %d ", tMrd));
	}
	else if (MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DTYPE_DDR2)
	{
	
		/* Extract timing (in ns) from SPD value. We ignore the tenth ns part.  */
		/* by shifting the data two bits right.                                 */
		tRp  = tRp  >> 2;    /* For example 0x50 -> 20ns                        */
		tRrd = tRrd >> 2;
		tRcd = tRcd >> 2;
		tWr  = tWr  >> 2;

		/* Extract clock cycles from time parameter. We need to round up        */
		tRp  = ((busClk * tRp)  / 1000) + (((busClk * tRp)  % 1000) ? 1 : 0);
		/* Micron work around for 133MHz */
		if (busClk == 133)
			tRp += 1;
		DB(mvOsPrintf("tRp = %d ", tRp));
		tRrd = ((busClk * tRrd) / 1000) + (((busClk * tRrd) % 1000) ? 1 : 0);
		/* JEDEC min reqeirments tRrd = 2 */
		if (tRrd < 2)
			tRrd = 2;
		DB(mvOsPrintf("tRrd = %d ", tRrd));
		tRcd = ((busClk * tRcd) / 1000) + (((busClk * tRcd) % 1000) ? 1 : 0);
		DB(mvOsPrintf("tRcd = %d ", tRcd));
		tWr  = ((busClk * tWr)  / 1000) + (((busClk * tWr)  % 1000) ? 1 : 0);
		DB(mvOsPrintf("tWr = %d ", tWr));
		tRfc  = ((busClk * tRfc)  / 1000) + (((busClk * tRfc)  % 1000) ? 1 : 0);
		DB(mvOsPrintf("tRfc = %d ", tRfc));
		/* JEDEC min reqeirments tMrd = 2 */
		tMrd  = 0x2;
		DB(mvOsPrintf("tMrd = %d ", tMrd));
	}
	else
	{    
		tWr  = ((busClk*SDRAM_TWR) / 1000) + (((busClk*SDRAM_TWR) % 1000)?1:0);
		tRtp = 2; /* Must be set to 0x1 (two cycles) when using DDR1 */
		if (pBankInfo[0].deviceDensity == _1G)
		{
			tRfc = SDRAM_TRFC_1G;
		}
		else
		{
			if (200 == busClk)
			{
				tRfc = SDRAM_TRFC_64_512M_AT_200MHZ;
			}
			else
			{
				tRfc = SDRAM_TRFC_64_512M;
			}
		}
		tRfc  = ((busClk * tRfc)  / 1000) + (((busClk * tRfc)  % 1000) ? 1 : 0);
		DB(mvOsPrintf("tRfc = %d ", tRfc));
	}
	
	return ((tRp << SDRAM_TIMING2_TRP_OFFS)	|
			(tRrd << SDRAM_TIMING2_TRRD_OFFS)	|
			(tRcd << SDRAM_TIMING2_TRCD_OFFS)	|
			(tWr << SDRAM_TIMING2_TWR_OFFS)		|
			(tRfc << SDRAM_TIMING2_TRFC_OFFS)	|
			(tMrd << SDRAM_TIMING2_TMRD_OFFS));
}

/*******************************************************************************
* sdramTimingReg3Calc - Calculate sdram timing register 3
*
* DESCRIPTION: 
*       This function calculates sdram timing register 3
*       optimized value based on the bank info parameters and the minCas.
*
* INPUT:
*	pBankInfo - sdram bank parameters
*       busClk    - Bus clock
*
* OUTPUT:
*       None
*
* RETURN:
*       sdram timing reg 3 value.
*
*******************************************************************************/
static MV_U32 sdramTimingReg3Calc(MV_DRAM_BANK_INFO *pBankInfo, 
                                                MV_U32 minCas, MV_U32 busClk)
{
	MV_U32 tMod  = 0;
	MV_U32 tXsrd = 0;
	MV_U32 tXsnr = 0;
	MV_U32 tXards  = 0;
	MV_U32 tXp = 0;
	
	MV_U32 bankNum;
	
	busClk = busClk / 1000000;    /* In MHz */
	
	/* JEDEC min reqeirments tXsnr = MAX(5tCK, tRfc + 10nSec) */
	tXsnr = pBankInfo.minRefreshToActiveCmd + 10;
	tXsnr  = ((busClk * tXsnr)  / 1000) + (((busClk * tXsnr)  % 1000) ? 1 : 0);
	DB(mvOsPrintf("tXsnr = %d ", tXsnr));

	if (MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DTYPE_DDR3)
	{
		/* Extract clock cycles from time parameter. We need to round up        */
		/* JEDEC min reqeirments tMod = MAX(12tCK, 15nSec) */
		tMod  = ((busClk * 15)  / 1000) + (((busClk * 15)  % 1000) ? 1 : 0);
		tMod = MV_MAX(tMod, 12);
		DB(mvOsPrintf("tMod = %d ", tMod));
		/* JEDEC min reqeirments tXsrd = 512 tCK */
		tXsrd = 512;
		DB(mvOsPrintf("tXsrd = %d ", tXsrd));
		/* JEDEC min reqeirments tXards = MAX(10tCK, 24nSec) */
		tXards  = ((busClk * 24)  / 1000) + (((busClk * 24)  % 1000) ? 1 : 0);
		tXards = MV_MAX(tXards, 10);
		DB(mvOsPrintf("tXards = %d ", tXards));
		/* JEDEC min reqeirments tXp = MAX(3tCK, 7.5nSec) */
		tXp  = ((busClk * 7.5)  / 1000) + (((busClk * 7.5)  % 1000) ? 1 : 0);
		tXp = MV_MAX(tXp, 3);
		DB(mvOsPrintf("tXp = %d ", tXp));
	}
	else if (MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DTYPE_DDR2)
	{
	
		/* Extract clock cycles from time parameter. We need to round up        */
		/* JEDEC min reqeirments tMod = 2tCK */
		tMod = 2;
		DB(mvOsPrintf("tMod = %d ", tMod));
		/* JEDEC min reqeirments tXsrd = 200 tCK */
		tXsrd = 200;
		DB(mvOsPrintf("tXsrd = %d ", tXsrd));
		/* JEDEC min reqeirments tXards = 200 and 267 6
		 * 	 			  333 7 and 400 8*/
		switch (busClk)
		{
		case 200:
		case 267:
			tXards = 6;
			break;
		case 333:
			tXards = 7;
			break;
		case 400:
			tXards = 8;
			break;
		}
		DB(mvOsPrintf("tXards = %d ", tXards));
		/* JEDEC min reqeirments tXp = 2 tCK */
		tXp = 2;
		DB(mvOsPrintf("tXp = %d ", tXp));
	}
	else
	{    
		/* TODO */
		DB(mvOsPrintf("LP DDR1"));
	}
	
	return ((tMod << SDRAM_TIMING3_TMOD_OFFS)	|
			((tXsnr >> 8) << SDRAM_TIMING3_TXSNR_EXT_OFFS)	|
			(tXsrd << SDRAM_TIMING3_TXSRD_OFFS)	|
			(tXsnr << SDRAM_TIMING3_TXSNR_OFFS)		|
			(tXards << SDRAM_TIMING3_TXARDS_OFFS)	|
			(tXp << SDRAM_TIMING3_TXP_OFFS));
}

/*******************************************************************************
* sdramTimingReg4Calc - Calculate sdram timing register 4
*
* DESCRIPTION: 
*       This function calculates sdram timing register 4
*       optimized value based on the bank info parameters and the minCas.
*
* INPUT:
*	pBankInfo - sdram bank parameters
*       busClk    - Bus clock
*
* OUTPUT:
*       None
*
* RETURN:
*       sdram timing reg 4 value.
*
*******************************************************************************/
static MV_U32 sdramTimingReg4Calc(MV_DRAM_BANK_INFO *pBankInfo, 
                                                MV_U32 minCas, MV_U32 busClk)
{
	MV_U32 tCke  = 0;
	MV_U32 init_cnt = 0;
	MV_U32 tRwd = 1;
	MV_U32 rst_cnt  = 0;
	MV_U32 init_cnt_nop = 0;
	
	MV_U32 bankNum;
	
	busClk = busClk / 1000000;    /* In MHz */

	/* Extract clock cycles from time parameter. We need to round up
	 * init_cnt should be 200uSec */
	init_cnt  = ((busClk * 200) / 1000) + (((busClk * 200)  % 1000) ? 1 : 0);
	init_cnt = (init_cnt * 1000) >> 10;
	DB(mvOsPrintf("init_cnt = %d ", init_cnt));

	/* DDR3 only rst_cnt should be 100nSec */
	rst_cnt  = ((busClk * 100) / 1000) + (((busClk * 100)  % 1000) ? 1 : 0);
	DB(mvOsPrintf("rst_cnt = %d ", rst_cnt));

	if (MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DTYPE_DDR3)
	{
		/* Extract clock cycles from time parameter. We need to round up        */
		/* JEDEC min reqeirments tCke = MAX(3 tCK, 800 7.5nSec
		 * 	 			  	   1066 5.625
		 * 	 			  	   1333 5.625
		 * 	 			  	   1600 5) */
		switch (busClk)
		{
		case 400:
			tCke = 7500;
			break;
		case 533:
		case 666:
			tCke = 5625;
			break;
		case 800:
			tCke = 5000;
			break;
		}
		tCke  = ((busClk * tCke)  / 1000000) + (((busClk * tCke)  % 1000000) ? 1 : 0);
		tMod = MV_MAX(tCke, 3);
		DB(mvOsPrintf("tCke = %d ", tCke));

	 	/* init_cnt should be 500uSec */
		init_cnt_nop  = ((busClk * 500) / 1000) + (((busClk * 500)  % 1000) ? 1 : 0);
		init_cnt_nop = (init_cnt_nop * 1000) >> 10;
		DB(mvOsPrintf("init_cnt_nop = %d ", init_cnt_nop));
	}
	else if (MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DTYPE_DDR2)
	{
	
		/* Extract clock cycles from time parameter. We need to round up        */
		/* JEDEC min reqeirments tCke = 3tCK */
		tCke = 3;
		DB(mvOsPrintf("tCke = %d ", tCke));

	 	/* init_cnt should be 400nSec */
		init_cnt_nop  = ((busClk * 400) / 1000) + (((busClk * 400)  % 1000) ? 1 : 0);
		init_cnt_nop = init_cnt_nop >> 10;
		DB(mvOsPrintf("init_cnt_nop = %d ", init_cnt_nop));
	}
	else
	{    
		/* TODO */
		DB(mvOsPrintf("LP DDR1"));
	}
	
	return ((tCke << SDRAM_TIMING4_TCKE_OFFS)	|
			(init_cnt << SDRAM_TIMING4_INIT_COUNT_OFFS)	|
			(tRwd << SDRAM_TIMING4_TRWD_EXT_OFFS)	|
			(rst_cnt << SDRAM_TIMING4_REST_COUNT_OFFS)		|
			(init_cnt_nop << SDRAM_TIMING4_INIT_COUNT_NOP_OFFS));
}

/*******************************************************************************
* sdramTimingReg5Calc - Calculate sdram timing register 5
*
* DESCRIPTION: 
*       This function calculates sdram timing register 5
*       optimized value based on the bank info parameters and the minCas.
*
* INPUT:
*	pBankInfo - sdram bank parameters
*       busClk    - Bus clock
*
* OUTPUT:
*       None
*
* RETURN:
*       sdram timing reg 5 value.
*
*******************************************************************************/
static MV_U32 sdramTimingReg5Calc(MV_DRAM_BANK_INFO *pBankInfo, 
                                                MV_U32 minCas, MV_U32 busClk)
{
	MV_U32 tRas  = 0;
	MV_U32 tFaw = 0;
	MV_U32 tCcd_ccs = 1;
	MV_U32 page_size = 0;
	
	busClk = busClk / 1000000;    /* In MHz */

	/* Extract clock cycles from time parameter. We need to round up */

        tRas = pBankInfo.minRasPulseWidth;
	tRas  = ((busClk * tRas) / 1000) + (((busClk * tRas)  % 1000) ? 1 : 0);
	DB(mvOsPrintf("tRas = %d ", tRas));

	/* page size is 2^COL * device_width/8 */
	page_size  = 1 << pBankInfo.numOfColAddr;
	page_size  *=  (pBankInfo.sdramWidth/8);
	DB(mvOsPrintf("page_size = %d ", page_size));

	if (MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DTYPE_DDR3)
	{
		/* Extract clock cycles from time parameter. We need to round up        */
		tFaw  = pBankInfo.minFourActiveWinDelay;
		tFaw  = ((busClk * tFaw) / 1000) + (((busClk * tFaw)  % 1000) ? 1 : 0);
		DB(mvOsPrintf("tFaw = %d ", tFaw));
	}
	else if (MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DTYPE_DDR2)
	{
	
		/* Extract clock cycles from time parameter. We need to round up        */
		/* JEDEC min reqeirments tFaw = 37.5n 1K page size
		 *  				50n 2K page size */
		switch(page_size)
		{
		case _1K:
			tFaw = 375;
			break;
		case _2K:
			tFaw = 500;
			break;
		}
		tFaw  = ((busClk * tFaw) / 10000) + (((busClk * tFaw)  % 10000) ? 1 : 0);
		DB(mvOsPrintf("tFaw = %d ", tFaw));
	}
	else
	{    
		/* TODO */
		DB(mvOsPrintf("LP DDR1"));
	}
	
	return ((tRas << SDRAM_TIMING5_TRAS_OFFS)	|
			(tFaw << SDRAM_TIMING5_TFAW_OFFS)	|
			(tCcd_ccs << SDRAM_TIMING5_TCCD_CCS_EXT_OFFS));
}
/*******************************************************************************
* sdramTimingReg6Calc - Calculate sdram timing register 6
*
* DESCRIPTION: 
*       This function calculates sdram timing register 6
*       optimized value based on the bank info parameters and the minCas.
*
* INPUT:
*	pBankInfo - sdram bank parameters
*       busClk    - Bus clock
*
* OUTPUT:
*       None
*
* RETURN:
*       sdram timing reg 6 value.
*
*******************************************************************************/
static MV_U32 sdramTimingReg6Calc(MV_DRAM_BANK_INFO *pBankInfo, 
                                                MV_U32 minCas, MV_U32 busClk)
{
	/* JEDEC min reqeirments tZqcs= 64 tCK */
	MV_U32 tZqcs  = 64;
	/* JEDEC min reqeirments tZqoper= 256 tCK */
	MV_U32 tZqoper = 256;
	/* JEDEC min reqeirments tZqinit= 512 tCK */
	MV_U32 tZqinit = 512;
	
	return ((tZqcs << SDRAM_TIMING6_TZQCS_OFFS)	|
			(tZqoper << SDRAM_TIMING6_TZQOPER_OFFS)	|
			(tZqinit << SDRAM_TIMING5_TZQINIT_OFFS));
}

/*******************************************************************************
* sdramPopulateBanks - Return SDRAM bank population.
*
* DESCRIPTION: 
*		
* INPUT:
*		pBankInfo - bank info parameters.
*
* OUTPUT:
*       None
*
* RETURN:
*       0x0 - 0xF bitmap for populated bank.
*******************************************************************************/
static MV_U32 sdramPopulateBanks(MV_DRAM_BANK_INFO *pBankInfo)
{
	MV_U32 populateBanks = 0;
	int bankNum;
	
	/* Represent the populate banks in binary form */
	for(bankNum = 0; bankNum < MV_DRAM_MAX_CS; bankNum++)
	{
		if (0 != pBankInfo[bankNum].size)
		{
				populateBanks |= (1 << bankNum);
		}
	}
	
	return populateBanks;
}
	
/*******************************************************************************
* sdramDDrOdtConfig - Set DRAM DDR3/2 On Die Termination registers.
*
* DESCRIPTION: 
*       This function config DDR2 On Die Termination (ODT) registers.
*	ODT configuration is done according to DIMM presence:
*	
*       Presence	  Ctrl Low    Ctrl High  Dunit Ctrl   Ext Mode  
*	CS0	         0x84210000  0x00000000  0x0000780F  0x00000440 
*	CS0+CS1          0x84210000  0x00000000  0x0000780F  0x00000440 
*	CS0+CS2	    	 0x030C030C  0x00000000  0x0000740F  0x00000404 
*	CS0+CS1+CS2	 0x030C030C  0x00000000  0x0000740F  0x00000404 
*	CS0+CS2+CS3	 0x030C030C  0x00000000  0x0000740F  0x00000404 
*	CS0+CS1+CS2+CS3  0x030C030C  0x00000000  0x0000740F  0x00000404 
*		
* INPUT:
*		pBankInfo - bank info parameters.
*
* OUTPUT:
*       None
*
* RETURN:
*       None
*******************************************************************************/
static void sdramDDrOdtConfig(MV_DRAM_BANK_INFO *pBankInfo)
{
	MV_U32 populateBanks = 0;
	MV_U32 sdramCtrl6, sdramCtrl7;
	int bankNum;
	
	/* Represent the populate banks in binary form */
	populateBanks = sdramPopulateBanks(pBankInfo);
	
	switch(populateBanks)
	{
		case(BANK_PRESENT_CS0):
		case(BANK_PRESENT_CS0_CS1):
			sdramCtrl6   = DDR_ODT_CTRL_LOW_CS0_DV;
			sdramCtrl7  = DDR_ODT_CTRL_HIGH_CS0_DV;
			break;
		default:
			mvOsPrintf("sdramDDrOdtConfig: Invalid DRAM bank presence\n");
			return;
	} 
	MV_REG_WRITE(DRAM_BUF_REG7, sdramCtrl6);
	MV_REG_WRITE(DRAM_BUF_REG8, sdramCtrl7);
	return;
}
#endif /* defined(MV_INC_BOARD_DDIM) */

/*******************************************************************************
* mvDramIfWinSet - Set DRAM interface address decode window
*
* DESCRIPTION: 
*       This function sets DRAM interface address decode window.
*
* INPUT:
*	    target      - System target. Use only SDRAM targets.
*       pAddrDecWin - SDRAM address window structure.
*
* OUTPUT:
*       None
*
* RETURN:
*       MV_BAD_PARAM if parameters are invalid or window is invalid, MV_OK
*       otherwise.
*******************************************************************************/
MV_STATUS mvDramIfWinSet(MV_TARGET target, MV_DRAM_DEC_WIN *pAddrDecWin)
{
	MV_U32 baseReg=0;
	MV_U32 baseToReg=0 , sizeToReg=0;

    /* Check parameters */
	if (!MV_TARGET_IS_DRAM(target))
	{
		mvOsPrintf("mvDramIfWinSet: target %d is not SDRAM\n", target);
		return MV_BAD_PARAM;
	}

    /* Check if the requested window overlaps with current enabled windows	*/
    if (MV_TRUE == sdramIfWinOverlap(target, &pAddrDecWin->addrWin))
	{
        mvOsPrintf("mvDramIfWinSet: ERR. Target %d overlaps\n", target);
		return MV_BAD_PARAM;
	}
#if 0
	/* check if address is aligned to the size */
	if(MV_IS_NOT_ALIGN(pAddrDecWin->addrWin.baseLow, pAddrDecWin->addrWin.size))
	{
		mvOsPrintf("mvDramIfWinSet:Error setting DRAM interface window %d."\
				   "\nAddress 0x%08x is unaligned to size 0x%x.\n",
                   target, 
				   pAddrDecWin->addrWin.baseLow,
				   pAddrDecWin->addrWin.size);
		return MV_ERROR;
	}
#endif

	/* read base register*/
	baseReg = MV_REG_READ(SDRAM_MEM_MAP_REG(target));

	/* BaseLow[31:23] => base register [31:23]		*/
	baseToReg = pAddrDecWin->addrWin.baseLow & SDRAM_MMAP_BASE_MASK;

	/* Write to address decode Base Address Register                  */
	baseReg &= ~SDRAM_MMAP_BASE_MASK;
	baseReg |= baseToReg;

	/* Translate the given window size to register format			*/
	switch(pAddrDecWin->addrWin.size)
	{
	case _8M:
		sizeToReg = 0x7;
		break;
	case _16M:
		sizeToReg = 0x8;
		break;
	case _32M:
		sizeToReg = 0x9;
		break;
	case _64M:
		sizeToReg = 0xA;
		break;
	case _128M:
		sizeToReg = 0xB;
		break;
	case _256M:
		sizeToReg = 0xC;
		break;
	case _512M:
		sizeToReg = 0xD;
		break;
	case _1G:
		sizeToReg = 0xE;
		break;
	case _2G:
		sizeToReg = 0xF;
		break;
	}

	/* Size parameter validity check.                                   */
	if (-1 == sizeToReg)
	{
		mvOsPrintf("mvDramIfWinSet: ERR. Win %d size invalid.\n",target);
		return MV_BAD_PARAM;
	}

	/* set size */
	baseReg &= ~DRAM_MMAP_SIZE_MASK;
	/* Size is located at upper 16 bits */
	baseReg |= (sizeToReg << SDRAM_MMAP_SIZE_OFFS);

	/* enable/Disable */
	if (MV_TRUE == pAddrDecWin->enable)
	{
		baseReg |= SDRAM_MMAP_WIN_EN;
	}
	else
	{
		baseReg &= ~SDRAM_MMAP_WIN_EN;
	}

	/* 3) Write to address decode Base Address Register                   */
	MV_REG_WRITE(SDRAM_MEM_MAP_REG(target), baseReg);

	return MV_OK;
}
/*******************************************************************************
* mvDramIfWinGet - Get DRAM interface address decode window
*
* DESCRIPTION: 
*       This function gets DRAM interface address decode window.
*
* INPUT:
*	    target - System target. Use only SDRAM targets.
*
* OUTPUT:
*       pAddrDecWin - SDRAM address window structure.
*
* RETURN:
*       MV_BAD_PARAM if parameters are invalid or window is invalid, MV_OK
*       otherwise.
*******************************************************************************/
MV_STATUS mvDramIfWinGet(MV_TARGET target, MV_DRAM_DEC_WIN *pAddrDecWin)
{
	MV_U32 baseReg;
	MV_U32 sizeRegVal;

	/* Check parameters */
	if (!MV_TARGET_IS_DRAM(target))
	{
		mvOsPrintf("mvDramIfWinGet: target %d is Illigal\n", target);
		return MV_ERROR;
	}

	/* Read base and size registers */
	baseReg = MV_REG_READ(SDRAM_MEM_MAP_REG(target));

	/* set size */
	sizeRegVal = baseReg & DRAM_MMAP_SIZE_MASK;
	/* Size is located at upper 16 bits */
	sizeRegVal= (sizeRegVal >> SDRAM_MMAP_SIZE_OFFS);

	/* Translate the given window size to register format			*/
	switch(sizeRegVal)
	{
	case 0x7:
		pAddrDecWin->addrWin.size = _8M;
		break;
	case 0x8:
		pAddrDecWin->addrWin.size = _16M;
		break;
	case 0x9:
		pAddrDecWin->addrWin.size = _32M;
		break;
	case 0xA:
		pAddrDecWin->addrWin.size = _64M;
		break;
	case 0xB:
		pAddrDecWin->addrWin.size = _128M;
		break;
	case 0xC:
		pAddrDecWin->addrWin.size = _256M;
		break;
	case 0xD:
		pAddrDecWin->addrWin.size = _512M;
		break;
	case 0xE:
		pAddrDecWin->addrWin.size = _1G;
		break;
	case 0xF:
		pAddrDecWin->addrWin.size = _2G;
		break;
	}

    	/* Check if ctrlRegToSize returned OK */
	if (-1 == pAddrDecWin->addrWin.size)
	{
		mvOsPrintf("mvDramIfWinGet: size of target %d is Illigal\n", target);
		return MV_ERROR;
	}

	/* Extract base address						*/
	/* Base register [31:23] ==> baseLow[31:23] 		*/
	pAddrDecWin->addrWin.baseLow = baseReg & SDRAM_MMAP_BASE_MASK;

	pAddrDecWin->addrWin.baseHigh =  0;


	if (baseReg & SDRAM_MMAP_WIN_EN)
	{
		pAddrDecWin->enable = MV_TRUE;
	}
	else
	{
		pAddrDecWin->enable = MV_FALSE;			
	}

	return MV_OK;
}
/*******************************************************************************
* mvDramIfWinEnable - Enable/Disable SDRAM address decode window
*
* DESCRIPTION: 
*		This function enable/Disable SDRAM address decode window.
*
* INPUT:
*	    target - System target. Use only SDRAM targets.
*
* OUTPUT:
*		None.
*
* RETURN:
*		MV_ERROR in case function parameter are invalid, MV_OK otherewise.
*
*******************************************************************************/
MV_STATUS mvDramIfWinEnable(MV_TARGET target,MV_BOOL enable)
{
	MV_DRAM_DEC_WIN 	addrDecWin;

	/* Check parameters */
	if (!MV_TARGET_IS_DRAM(target))
	{
		mvOsPrintf("mvDramIfWinEnable: target %d is Illigal\n", target);
		return MV_ERROR;
	}

	if (enable == MV_TRUE) 
	{   /* First check for overlap with other enabled windows				*/
		if (MV_OK != mvDramIfWinGet(target, &addrDecWin))
		{
			mvOsPrintf("mvDramIfWinEnable:ERR. Getting target %d failed.\n", 
                                                                        target);
			return MV_ERROR;
		}
		/* Check for overlapping */
		if (MV_FALSE == sdramIfWinOverlap(target, &(addrDecWin.addrWin)))
		{
			/* No Overlap. Enable address decode winNum window              */
			MV_REG_BIT_SET(SDRAM_MEM_MAP_REG(target), SDRAM_MMAP_WIN_EN);
		}
		else
		{   /* Overlap detected	*/
			mvOsPrintf("mvDramIfWinEnable: ERR. Target %d overlap detect\n",
                                                                        target);
			return MV_ERROR;
		}
	}
	else
	{   /* Disable address decode winNum window                             */
		MV_REG_BIT_RESET(SDRAM_MEM_MAP_REG(target), SDRAM_MMAP_WIN_EN);
	}

	return MV_OK;
}

/*******************************************************************************
* sdramIfWinOverlap - Check if an address window overlap an SDRAM address window
*
* DESCRIPTION:
*		This function scan each SDRAM address decode window to test if it 
*		overlapps the given address windoow 
*
* INPUT:
*       target      - SDRAM target where the function skips checking.
*       pAddrDecWin - The tested address window for overlapping with 
*					  SDRAM windows.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if the given address window overlaps any enabled address
*       decode map, MV_FALSE otherwise.
*
*******************************************************************************/
static MV_BOOL sdramIfWinOverlap(MV_TARGET target, MV_ADDR_WIN *pAddrWin)
{
	MV_TARGET	targetNum;
	MV_DRAM_DEC_WIN 	addrDecWin;
	
	for(targetNum = SDRAM_CS0; targetNum < MV_DRAM_MAX_CS ; targetNum++)
	{
		/* don't check our winNum or illegal targets */
		if (targetNum == target)
		{
			continue;
		}
		
		/* Get window parameters 	*/
		if (MV_OK != mvDramIfWinGet(targetNum, &addrDecWin))
		{
			mvOsPrintf("sdramIfWinOverlap: ERR. TargetWinGet failed\n");
			return MV_ERROR;
		}
	
		/* Do not check disabled windows	*/
		if (MV_FALSE == addrDecWin.enable)
		{
			continue;
		}
	
		if(MV_TRUE == mvWinOverlapTest(pAddrWin, &addrDecWin.addrWin))
		{                    
			mvOsPrintf(
			"sdramIfWinOverlap: Required target %d overlap winNum %d\n", 
			target, targetNum);
			return MV_TRUE;           
		}
	}
	
	return MV_FALSE;
}

/*******************************************************************************
* mvDramIfBankSizeGet - Get DRAM interface bank size.
*
* DESCRIPTION:
*       This function returns the size of a given DRAM bank.
*
* INPUT:
*       bankNum - Bank number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       DRAM bank size. If bank is disabled the function return '0'. In case 
*		or paramter is invalid, the function returns -1.
*
*******************************************************************************/
MV_32 mvDramIfBankSizeGet(MV_U32 bankNum)
{
    MV_DRAM_DEC_WIN 	addrDecWin;
	
	/* Check parameters */
	if (!MV_TARGET_IS_DRAM(bankNum))
	{
		mvOsPrintf("mvDramIfBankBaseGet: bankNum %d is invalid\n", bankNum);
		return -1;
	}
	/* Get window parameters 	*/
	if (MV_OK != mvDramIfWinGet(bankNum, &addrDecWin))
	{
		mvOsPrintf("sdramIfWinOverlap: ERR. TargetWinGet failed\n");
		return -1;
	}
	
	if (MV_TRUE == addrDecWin.enable)
	{
		return addrDecWin.addrWin.size;
	}
	else
	{
		return 0;
	}
}


/*******************************************************************************
* mvDramIfSizeGet - Get DRAM interface total size.
*
* DESCRIPTION:
*       This function get the DRAM total size.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       DRAM total size. In case or paramter is invalid, the function 
*		returns -1.
*
*******************************************************************************/
MV_32 mvDramIfSizeGet(MV_VOID)
{
	MV_U32 totalSize = 0, bankSize = 0, bankNum;
	
	for(bankNum = 0; bankNum < MV_DRAM_MAX_CS; bankNum++)
	{
		bankSize = mvDramIfBankSizeGet(bankNum);

		if (-1 == bankSize)
		{
			mvOsPrintf("Dram: mvDramIfSizeGet error with bank %d \n",bankNum);
			return -1;
		}
		else
		{
			totalSize += bankSize;
		}
	}
	
	DB(mvOsPrintf("Dram: Total DRAM size is 0x%x \n",totalSize));
	
	return totalSize;
}

/*******************************************************************************
* mvDramIfBankBaseGet - Get DRAM interface bank base.
*
* DESCRIPTION:
*       This function returns the 32 bit base address of a given DRAM bank.
*
* INPUT:
*       bankNum - Bank number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       DRAM bank size. If bank is disabled or paramter is invalid, the 
*		function returns -1.
*
*******************************************************************************/
MV_32 mvDramIfBankBaseGet(MV_U32 bankNum)
{
    MV_DRAM_DEC_WIN 	addrDecWin;
	
	/* Check parameters */
	if (!MV_TARGET_IS_DRAM(bankNum))
	{
		mvOsPrintf("mvDramIfBankBaseGet: bankNum %d is invalid\n", bankNum);
		return -1;
	}
	/* Get window parameters 	*/
	if (MV_OK != mvDramIfWinGet(bankNum, &addrDecWin))
	{
		mvOsPrintf("sdramIfWinOverlap: ERR. TargetWinGet failed\n");
		return -1;
	}
	
	if (MV_TRUE == addrDecWin.enable)
	{
		return addrDecWin.addrWin.baseLow;
	}
	else
	{
		return -1;
	}
}


MV_VOID mvIntrfaceWidthPrint(MV_VOID)
{
    MV_U32 tmp = (MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DWIDTH_MASK);
    switch(tmp)
    {
	case SDRAM_CTRL4_DWIDTH_16BIT: printf(" 16bit width");
	    break;
        case SDRAM_CTRL4_DWIDTH_32BIT: printf(" 32bit width");
	    break;
        default: printf(" unknown width ");
            break;
    }
}

MV_VOID mvIntrfaceParamPrint(MV_VOID)
{
    MV_U32 rasTmp, tmp;

    printf("DRAM");

	if ((MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DTYPE_MASK) == SDRAM_CTRL4_DTYPE_DDR3)
	{
    		switch(MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_CL_MASK)
    		{
			case SDRAM_DDR3_CL_5: printf(" CAS Latency = 5");
	    		break;
        		case SDRAM_DDR3_CL_6: printf(" CAS Latency = 6");
	    		break;
        		case SDRAM_DDR3_CL_7: printf(" CAS Latency = 7");
            		break;
        		case SDRAM_DDR3_CL_8: printf(" CAS Latency = 8");
            		break;
        		case SDRAM_DDR3_CL_9: printf(" CAS Latency = 9");
	    		break;
        		case SDRAM_DDR3_CL_10: printf(" CAS Latency = 10");
            		break;
        		case SDRAM_DDR3_CL_11: printf(" CAS Latency = 11");
            		break;

        		default: printf(" unknown CAL ");
            		break;
    		}
	}
	else if ((MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_DTYPE_MASK) == SDRAM_CTRL4_DTYPE_DDR2)
	{
    		switch(MV_REG_READ(SDRAM_CTRL_4_REG) & SDRAM_CTRL4_CL_MASK)
    		{
			case SDRAM_DDR2_CL_3: printf(" CAS Latency = 3");
	    		break;
        		case SDRAM_DDR2_CL_4: printf(" CAS Latency = 4");
	    		break;
        		case SDRAM_DDR2_CL_5: printf(" CAS Latency = 5");
            		break;
        		case SDRAM_DDR2_CL_6: printf(" CAS Latency = 6");
            		break;
        		default: printf(" unknown CAL ");
            		break;
    		}
	}

	tmp = MV_REG_READ(SDRAM_TIMING_2_REG);

    	rasTmp = MV_REG_READ(SDRAM_TIMING_5_REG);
    
	printf(" tRP = %d tRAS = %d tRCD = %d\n",
           	((tmp & SDRAM_TIMING2_TRP_MASK) >> SDRAM_TIMING2_TRP_OFFS), 
		((rasTmp & SDRAM_TIMING5_TRAS_MASK) >> SDRAM_TIMING5_TRAS_OFFS), 
		((tmp & SDRAM_TIMING2_TRCD_MASK) >> SDRAM_TIMING2_TRCD_OFFS));
}


