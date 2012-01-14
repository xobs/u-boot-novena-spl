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
#include "mvSysDdrConfig.h"
#include "mvDramIfConfig.h"
#include "mvDramIfRegs.h"
#include "../mvDramIf.h"

#warning "Fix this include...."
#include "pmu/mvPmuRegs.h"

MV_DDR_MC_PARAMS ddr2_400mhz[] ={{0xd00d0048, 0x00000101},	/* Select NB PLL and Ratio */
				{0xd0800010, 0xf1800000},	/* Set DDR register space */
                                {0xd00d025c, 0x000f1890},	/* Set NB register space */
				{0xd0020080, 0xf1000000},	/* Set SB register space */
			 	{0xf1800140, 0x20004433}, 
             			{0xf18001d0, 0x0FFC2771},
             			{0xf18001e0, 0x033C2770},
             			{0xf18001f0, 0xC0000077},
             			{0xf1800240, 0x8000FF02},
             			{0xf1800210, 0x00300000},
             			{0xf1800230, 0x00347C86},
           			{0xf1800280, 0x01010e01}, 
            			{0xf1800510, 0x010e0101},
             			{0xf1800E10, 0x00347C86},
             			{0xf1800E20, 0x00347C86},
             			{0xf1800E30, 0x00347C86},
             			{0xf1800020, 0x00042430},
             			{0xf1800030, 0x00042430},
             			{0xf1800050, 0x2D180618},
             			{0xf1800060, 0x84660342},
             			{0xf1800190, 0x20C938E2},
             			{0xf18001C0, 0x350628A1},
             			{0xf1800650, 0x00130142},
              			{0xf18001a0, 0x20818005},
             			{0xf1800100, 0x000D0001},
             			{0xf1800110, 0x000D0000},
             			{0xf1800760, 0x00000001},
             			{0xf1800770, 0x02000002},
             			{0xf1800120, 0x00000001}};

#ifdef CONFIG_DOVE_REV_Z0
MV_DDR_MC_PARAMS ddr2_200mhz[] ={{0xd00d0048, 0x00000102},	/* Select NB PLL and Ratio */
				{0xd0800010, 0xf1800000},	/* Set DDR register space */
                                {0xd00d025c, 0x000f1890},	/* Set NB register space */
				{0xd0020080, 0xf1000000},	/* Set SB register space */
                                {0xf1800140, 0x20004433},
                                {0xf18001f0, 0xC0000077},
                                {0xf1800240, 0x8000FF02},
                                {0xf1800210, 0x00300000},
                                {0xf1800230, 0x21F00102}, 
                                {0xf1800E10, 0x01F00102},
                                {0xf1800E20, 0x01F00102},
                                {0xf1800E30, 0x01F00102},
                                {0xf1800020, 0x00022430},
                                {0xf1800030, 0x00022430},
                                {0xf1800050, 0x4D160618},
                                {0xf1800060, 0x64660342},
                                {0xf1800190, 0x20C938E2},
                                {0xf18001C0, 0x350628A1},
                                {0xf1800650, 0x00130141},
                                {0xf18001a0, 0x00814005},
                                {0xf1800100, 0x000D0001},
                                {0xf1800110, 0x000D0000},
                                {0xf1800760, 0x00000003},
                                {0xf1800770, 0x0200000A},
                                {0xf1800120, 0x00000001}};
#else
MV_DDR_MC_PARAMS ddr2_200mhz[] ={{0xd0800010, 0xf1800000},	/* Set DDR register space */
                                {0xd00d025c, 0x000f1890},	/* Set NB register space */
				{0xd0020080, 0xf1000000},	/* Set SB register space */
//                                {0xf10d0260, 0x0000F180},
                                {0xf1800140, 0x20004433},
                                {0xf1800020, 0x00042430},
                                {0xf1800030, 0x00042430},
                                {0xf1800050, 0x488B00C3},
                                {0xf1800060, 0x323301A2},
                                {0xf1800190, 0x30C81c42},
                                {0xf18001C0, 0x32820050},
                                {0xf1800650, 0x00090071},
                                {0xf18001A0, 0x20810005},
                                {0xf1800080, 0x20000000},
                                {0xf1800760, 0x00001221},
                                {0xf1800770, 0x0200000A},
                                {0xf18001D0, 0x177c2779},
                                {0xf18001E0, 0x07700779},
                                {0xf18001F0, 0x3f200077},
                                {0xf1800210, 0x00300002},
                                {0xf1800230, 0x20007D08},
                                {0xf1800E10, 0x20007D08},
                                {0xf1800E20, 0x20007D08},
                                {0xf1800E30, 0x20007D08},
                                {0xf1800100, 0x000D0001},
                                {0xf1800110, 0x000D0000},
				{0xf1800120, 0x00000001}};
#endif

MV_DDR_MC_PARAMS ddr2_400mhz_reconfig[] ={
                               {0x00140, 0x20004433},
                               {0x001d0, 0x0FFC2771},
                               {0x001e0, 0x033C2770},
                               {0x001f0, 0xC0000077},
                               {0x00240, 0x8000FF02},
                               {0x00200, 0x0011310C},
                               {0x00210, 0x00300000},
                               {0x00230, 0x00347C86},
                               {0x00280, 0x01010E01},
                               {0x00510, 0x010E0101},
                               {0x00E10, 0x00347C86},
                               {0x00E20, 0x00347C86},
                               {0x00E30, 0x00347C86},
                               {0x00020, 0x00042430},
                               {0x00030, 0x00042430},
                               {0x00050, 0x2D180618},
                               {0x00060, 0x84660342},
                               {0x00190, 0x20C938E2},
                               {0x001C0, 0x350628A1},
                               {0x00650, 0x00130142},
                               {0x001a0, 0x20818005},	/* Fast bank enabled */
                               {0x00100, 0x000D0001},
                               {0x00110, 0x000D0000},
                               {0x00760, 0x00000001},
                               {0x00770, 0x02000002},
                                {0x00120, 0x03000100},		/* Load Mode Register */
				{0x00120, 0x03000200},		/* load Extended Mode Register */
				};

MV_DDR_MC_PARAMS ddr2_200mhz_reconfig[] ={
                               {0x00140, 0x20004433},
                               {0x001d0, 0x2FFC2781},
                               {0x001e0, 0x0FF00780},
                               {0x001f0, 0xC0000078},
                               {0x00240, 0x8000FF02},
                               {0x00200, 0x00113108},
                               {0x00210, 0x00300000},
                               {0x00230, 0x00347C86},
                               {0x00280, 0x01010E01},
                               {0x00510, 0x010E0101},
                               {0x00E10, 0x00347C86},
                               {0x00E20, 0x00347C86},
                               {0x00E30, 0x00347C86},
                               {0x00020, 0x00042430},
                               {0x00030, 0x00042430},
                               {0x00050, 0x4D180618},
                               {0x00060, 0x64660342},
                               {0x00190, 0x20C938E2},
                               {0x001C0, 0x350628A1},
                               {0x00650, 0x00130142},
                               {0x001a0, 0x20818005},	/* 0x00818005 */
                               {0x00100, 0x000D0001},
                               {0x00110, 0x000D0000},
                               {0x00760, 0x00000003},
                               {0x00770, 0x0200000A},
				{0x00120, 0x03000100},				/* Load Mode Register */
				{0x00120, 0x03000200}, 				/* load Extended Mode Register */
				};

/*******************************************************************************
* mvDramIfParamCountGet - Get the number of Addr/Value configuration needed 
*			  to init the DDR
*
* DESCRIPTION:
*       Get the number of Addr/Value configuration needed to init the DDR
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Number of Address Value couples
*
*******************************************************************************/
MV_U32 mvDramIfParamCountGet(MV_VOID)
{
	MV_U32 cnt=0;

	/* Return the max number of Addr/Value configuration needed to init the DDR */	
	if ((sizeof(ddr2_200mhz)/sizeof(ddr2_200mhz[0])) > cnt)
		cnt = (sizeof(ddr2_200mhz)/sizeof(ddr2_200mhz[0]));
	if (((sizeof(ddr2_400mhz)/sizeof(ddr2_400mhz[0]))) > cnt)
		cnt = (sizeof(ddr2_400mhz)/sizeof(ddr2_400mhz[0]));
	if ((sizeof(ddr2_200mhz_reconfig)/sizeof(ddr2_200mhz_reconfig[0]) +1) > cnt)
		cnt = (sizeof(ddr2_200mhz_reconfig)/sizeof(ddr2_200mhz_reconfig[0]) +1);
	if ((sizeof(ddr2_400mhz_reconfig)/sizeof(ddr2_400mhz_reconfig[0]) + 1) > cnt)
		cnt = (sizeof(ddr2_400mhz_reconfig)/sizeof(ddr2_400mhz_reconfig[0]) + 1);

	return cnt;
}

/*******************************************************************************
* mvDramIfParamFill - Fill in the Address/Value couples
*
* DESCRIPTION:
*       This function fills in the addr/val couples needed to init the DDR
*       controller based on the requesed frequency
*
* INPUT:
*       ddrFreq - Target frequency
*
* OUTPUT:
*	params - pointer to the first addr/value element.
*	paramcnt - Number of paramters filled in the addr/value array.
*
* RETURN:
*	STATUS
*
*******************************************************************************/
MV_STATUS mvDramIfParamFill(MV_U32 ddrFreq, MV_DDR_MC_PARAMS * params, MV_U32 * paramcnt)
{
	switch (ddrFreq)
	{
		case 400: /* DDR2 400Mhz */
			mvOsMemcpy((void*)params, (void*)ddr2_400mhz, sizeof(ddr2_400mhz));
			*paramcnt = (sizeof(ddr2_400mhz)/sizeof(ddr2_400mhz[0]));
			break;

		case 200: /* DDR2 200Mhz */
			mvOsMemcpy((void*)params, (void*)ddr2_200mhz, sizeof(ddr2_200mhz));
			*paramcnt = (sizeof(ddr2_200mhz)/sizeof(ddr2_200mhz[0]));
			break;
	
		default:
			*paramcnt = 0;
			return MV_FAIL;
	}
	
	return MV_OK;
}


/*******************************************************************************
* mvDramReconfigParamFill - Fill in the Address/Value couples
*
* DESCRIPTION:
*       This function fills in the addr/val couples needed to init the DDR
*       controller based on the requesed frequency
*
* INPUT:
*       ddrFreq - Target frequency
*	cpuFreq - cpu frequency to calculate Timing against
*
* OUTPUT:
*	params - pointer to the first addr/value element.
*	paramcnt - Number of paramters filled in the addr/value array.
*
* RETURN:
*	STATUS
*
*******************************************************************************/
MV_STATUS mvDramReconfigParamFill(MV_U32 ddrFreq, MV_U32 cpuFreq, MV_DDR_MC_PARAMS * params, MV_U32 * paramcnt)
{
	MV_U32 i, cnt;
	MV_U32 idx = 0;

#ifdef CONFIG_DOVE_REV_Z0
	/* First configuration is the DDR clk source and ratio */
	params[idx].addr = (PMU_CLK_DIVIDER_1_REG | DOVE_SB_REGS_VIRT_BASE);		/* DDR clk source and ratio */
	switch (cpuFreq/ddrFreq)
	{
		case 1:	/* CPU=400Mhz, DDR=400Mhz */
			params[idx].val = (PMU_CLK1_DIV_D2CLK_PLL_CPU);
			break;

		case 2: /* CPU=800Mhz, DDR=400Mhz  OR  CPU=400Mhz, DDR=200Mhz */
			params[idx].val = (PMU_CLK1_DIV_D2CLK_PLL_CPU | PMU_CLK1_DIV_D2PRATIO_1TO1);
			break;

		case 4: /* CPU=800Mhz, DDR=200Mhz */
			params[idx].val = (PMU_CLK1_DIV_D2CLK_PLL_CPU | PMU_CLK1_DIV_D2PRATIO_1TO2);
			break;

		default:
			return MV_FAIL;
	}
	idx++;
#endif

	switch (ddrFreq)
	{
		case 400: /* DDR2 400MHz */
			cnt = (sizeof(ddr2_400mhz_reconfig)/sizeof(ddr2_400mhz_reconfig[0]));
			*paramcnt = (idx + cnt);
			for (i=0; i < cnt; idx++, i++)
			{
				params[idx].addr = (ddr2_400mhz_reconfig[i].addr | DOVE_NB_REGS_VIRT_BASE);
				params[idx].val = ddr2_400mhz_reconfig[i].val;		
			}
			break;

		case 200: /* DDR2 200Mhz */
			cnt = (sizeof(ddr2_200mhz_reconfig)/sizeof(ddr2_200mhz_reconfig[0]));
			*paramcnt = (idx + cnt);
			for (i=0; i < cnt; idx++, i++)
			{
				params[idx].addr = (ddr2_200mhz_reconfig[i].addr | DOVE_NB_REGS_VIRT_BASE);
				params[idx].val = ddr2_200mhz_reconfig[i].val;		
			}
			break;
	
		default:
			return MV_FAIL;
	}
	
	return MV_OK;
}


/*******************************************************************************
* mvDramInitPollAmvFill - Fill in the Address/Value couples
*
* DESCRIPTION:
*       This function fills in the addr/val couples needed to init the DDR
*       controller based on the requesed frequency
*
* INPUT:
*       None.
*
* OUTPUT:
*	amv - address/mask/value for the DDR init done register.
*		amv->addr: Physical adddress of the init done register
*		amv->mask: Bit mask to poll for init done
*		amv->val: Value expected after the mask.
*
* RETURN:
*	STATUS
*
*******************************************************************************/
MV_STATUS mvDramInitPollAmvFill(MV_DDR_INIT_POLL_AMV * amv)
{
	amv->addr = (DOVE_SB_REGS_PHYS_BASE|SDRAM_STATUS_REG);/*mvOsIoVirtToPhy(NULL, (void*)(INTER_REGS_BASE|SDRAM_STATUS_REG))*/;
	amv->mask = SDRAM_STATUS_INIT_DONE_MASK;
	amv->val = SDRAM_STATUS_INIT_DONE;

	return MV_OK;
}

