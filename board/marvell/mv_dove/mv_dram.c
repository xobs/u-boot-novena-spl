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

#include <config.h>
#include <common.h>
#include "mvCommon.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "ddr/mvDramIf.h"
#include "mvBoardEnvLib.h"
#include "mvCpuIfRegs.h"

#ifdef DEBUG
#define DB(x) x
#else
#define DB(x)
#endif

extern void i2c_init(int speed, int slaveaddr);
extern void _start(void);
extern unsigned int  mvCpuPclkGet(void);
extern unsigned int  mvCpuL2ClkGet(void);
extern void reset_cpu(ulong addr);
extern int dramBoot;

#ifdef MV_INC_DRAM_MFG_TEST
static MV_VOID mvDramMfgTrst(void);
static MV_STATUS mv_mem_test(MV_U32* pMem, MV_U32 pattern, MV_U32 count);
static MV_STATUS mv_mem_cmp(MV_U32* pMem, MV_U32 pattern, MV_U32 count);
#endif

/***************************************************************************/
/* DRAM initialization table by frequency                                  */

#define MV_DDR2_Y0_INIT_SIZE	29
#define MV_DDR3_Y0_INIT_SIZE	29

#define MV_DDR2_Y1_INIT_SIZE	30
#define MV_DDR3_Y1_INIT_SIZE	30

#define MV_DDR3_AVNG_INIT_SIZE	30

#define MV_DDR3_WL_SIZE		16

MV_DRAM_INIT mv_dram_init;
MV_DRAM_REG_INIT write_leveling_reg_init[MV_DDR3_WL_SIZE];


#if defined(RD_88F6781Y0_AVNG) || defined(RD_88F6781X0_AVNG)|| defined(RD_88AP510A0_AVNG)

#if defined(MV_DRAM_ELPIDA_1G)
MV_DRAM_REG_INIT ddr3_y1_avng_elpida_400_init[MV_DDR3_AVNG_INIT_SIZE] = 
{
	{0xD0800020, 0x00042330},
	{0xD0800030, 0x00042330},
	{0xD0800050, 0x911400C3},
	{0xD0800060, 0x646602C4},
	{0xD0800190, 0xC2003053},
	{0xD08001C0, 0x34F4A187},
	{0xD0800650, 0x000F0121},
	{0xD0800660, 0x04040200},
	{0xD0800080, 0x00000000},
	{0xD0800090, 0x00080000},
	{0xD08000F0, 0xC0000000},
	{0xD08001A0, 0x20C08009},
	{0xD0800280, 0x010E0202},
	{0xD0800760, 0x00000201},
	{0xD0800770, 0x0100000A},
	{0xD0800140, 0x20004044},
	{0xD08001D0, 0x17784339},
	{0xD08001E0, 0x07700330},
	{0xD08001F0, 0x00003F33},
	{0xD0800200, 0x0011311C},
	{0xD0800210, 0x00300000},
	{0xD0800240, 0x80000000},
	{0xD0800510, 0x010E0101},
	{0xD0800230, 0x2028006A},
	{0xD0800E10, 0x00280062},
	{0xD0800E20, 0x00280062},
	{0xD0800E30, 0x00280062},
	{0xD0800100, 0x000C0001},
	{0xD0800110, 0x100C0001},
	{0xD0800120, 0x00000001}
};

MV_DRAM_REG_INIT ddr3_y1_avng_elpida_533_init[MV_DDR3_AVNG_INIT_SIZE] = 
{
	{0xD0800020, 0x00042330},
	{0xD0800030, 0x00042330},
	{0xD0800050, 0x911B00C3},
	{0xD0800060, 0x848803B4},
	{0xD0800190, 0xC200406C},
	{0xD08001C0, 0x3694DA09},
	{0xD0800650, 0x00140181},
	{0xD0800660, 0x04040200},
	{0xD0800080, 0x00000000},
	{0xD0800090, 0x00080000},
	{0xD08000F0, 0xC0000000},
	{0xD08001A0, 0x20C10409},
	{0xD0800280, 0x010E0202},
	{0xD0800760, 0x00000201},
	{0xD0800770, 0x0100000A},
	{0xD0800140, 0x20004044},
	{0xD08001D0, 0x17784339},
	{0xD08001E0, 0x07700330},
	{0xD08001F0, 0x00003F33},
	{0xD0800200, 0x0011311C},
	{0xD0800210, 0x00300000},
	{0xD0800240, 0x80000000},
	{0xD0800510, 0x010E0101},
	{0xD0800230, 0x2028006A},
	{0xD0800E10, 0x00280062},
	{0xD0800E20, 0x00280062},
	{0xD0800E30, 0x00280062},
	{0xD0800100, 0x000C0001},
	{0xD0800110, 0x100C0001},
	{0xD0800120, 0x00000001}
};
#endif

#if defined(MV_DRAM_SAMSUNG_1G)
MV_DRAM_REG_INIT ddr3_y1_avng_samsung1g_400_init[MV_DDR3_AVNG_INIT_SIZE] = 
{
	{0xD0800020, 0x00042330},
	{0xD0800030, 0x00042330},
	{0xD0800050, 0x911500C3},
	{0xD0800060, 0x646602C4},
	{0xD0800190, 0xC2003053},
	{0xD08001C0, 0x34F4A187},
	{0xD0800650, 0x000F4144},
	{0xD0800660, 0x84040200},
	{0xD0800080, 0x00000000},
	{0xD0800090, 0x00080000},
	{0xD08000F0, 0xC0000000},
	{0xD08001A0, 0x20C08009},
	{0xD0800280, 0x010E0202},
	{0xD0800760, 0x00000201},
	{0xD0800770, 0x0100000A},
	{0xD0800140, 0x20004044},
	{0xD08001D0, 0x177C2779},
	{0xD08001E0, 0x07700330},
	{0xD08001F0, 0x00003F33},
	{0xD0800200, 0x0011311C},
	{0xD0800210, 0x00300000},
	{0xD0800240, 0x80000000},
	{0xD0800510, 0x010E0101},
	{0xD0800230, 0x2028006A},
	{0xD0800E10, 0x00280062},
	{0xD0800E20, 0x00280062},
	{0xD0800E30, 0x00280062},
	{0xD0800100, 0x000C0001},
	{0xD0800110, 0x100C0001},
	{0xD0800120, 0x00000001}
};

MV_DRAM_REG_INIT ddr3_y1_avng_samsung1g_533_init[MV_DDR3_AVNG_INIT_SIZE] = 
{
	{0xD0800020, 0x00042330},
	{0xD0800030, 0x00042330},
	{0xD0800050, 0x911B00C3},
	{0xD0800060, 0x767803B4},
	{0xD0800190, 0xC200406C},
	{0xD08001C0, 0x3694DA09},
	{0xD0800650, 0x001441B4},
	{0xD0800660, 0x84040200},
	{0xD0800080, 0x00000000},
	{0xD0800090, 0x00080000},
	{0xD08000F0, 0xC0000000},
	{0xD08001A0, 0x20C0C409},
	{0xD0800280, 0x010E0202},
	{0xD0800760, 0x00000201},
	{0xD0800770, 0x0100000A},
	{0xD0800140, 0x20004044},
	{0xD08001D0, 0x177C2779},
	{0xD08001E0, 0x07700330},
	{0xD08001F0, 0x00003F33},
	{0xD0800200, 0x0011311C},
	{0xD0800210, 0x00300000},
	{0xD0800240, 0x80000000},
	{0xD0800510, 0x010E0101},
	{0xD0800230, 0x2028006A},
	{0xD0800E10, 0x00280062},
	{0xD0800E20, 0x00280062},
	{0xD0800E30, 0x00280062},
	{0xD0800100, 0x000C0001},
	{0xD0800110, 0x100C0001},
	{0xD0800120, 0x00000001}
};
#endif

#if defined(MV_DRAM_SAMSUNG_2G)
MV_DRAM_REG_INIT ddr3_y1_avng_samsung2g_400_init[MV_DDR3_AVNG_INIT_SIZE] = 
{
	{0xD0800020, 0x00042430},
	{0xD0800030, 0x00042430},
	{0xD0800050, 0x911500C3},
	{0xD0800060, 0x64660404},
	{0xD0800190, 0xC2004453},
	{0xD08001C0, 0x34F4A187},
	{0xD0800650, 0x001441B4},
	{0xD0800660, 0x84040200},
	{0xD0800080, 0x00000000},
	{0xD0800090, 0x00080000},
	{0xD08000F0, 0xC0000000},
	{0xD08001A0, 0x20C08009},
	{0xD0800280, 0x010E0202},
	{0xD0800760, 0x00000201},
	{0xD0800770, 0x0100000A},
	{0xD0800140, 0x20004044},
	{0xD08001D0, 0x177C2779},
	{0xD08001E0, 0x07700330},
	{0xD08001F0, 0x00003F33},
	{0xD0800200, 0x0011311C},
	{0xD0800210, 0x00300000},
	{0xD0800240, 0x80000000},
	{0xD0800510, 0x010E0101},
	{0xD0800230, 0x2028006A},
	{0xD0800E10, 0x00280062},
	{0xD0800E20, 0x00280062},
	{0xD0800E30, 0x00280062},
	{0xD0800100, 0x000D0001},
	{0xD0800110, 0x200D0001},
	{0xD0800120, 0x00000001}
};

MV_DRAM_REG_INIT ddr3_y1_avng_samsung2g_533_init[MV_DDR3_AVNG_INIT_SIZE] = 
{
	{0xD0800020, 0x00042430},
	{0xD0800030, 0x00042430},
	{0xD0800050, 0x911B00C3},
	{0xD0800060, 0x76780564},
	{0xD0800190, 0xC2005B6C},
	{0xD08001C0, 0x3694DA09},
	{0xD0800650, 0x001441B4},
	{0xD0800660, 0x84040200},
	{0xD0800080, 0x00000000},
	{0xD0800090, 0x00080000},
	{0xD08000F0, 0xC0000000},
	{0xD08001A0, 0x20C0C409},
	{0xD0800280, 0x010E0202},
	{0xD0800760, 0x00000201},
	{0xD0800770, 0x0100000A},
	{0xD0800140, 0x20004044},
	{0xD08001D0, 0x177C2779},
	{0xD08001E0, 0x07700330},
	{0xD08001F0, 0x00003F33},
	{0xD0800200, 0x0011311C},
	{0xD0800210, 0x00300000},
	{0xD0800240, 0x80000000},
	{0xD0800510, 0x010E0101},
	{0xD0800230, 0x2028006A},
	{0xD0800E10, 0x00280062},
	{0xD0800E20, 0x00280062},
	{0xD0800E30, 0x00280062},
	{0xD0800100, 0x000D0001},
	{0xD0800110, 0x200D0001},
	{0xD0800120, 0x00000001}
};
#endif

#if defined(MV_DRAM_SAMSUNG_2G_A0)
MV_DRAM_REG_INIT ddr3_a0_avng_samsung2g_400_init[MV_DDR3_AVNG_INIT_SIZE] = 
{

	{0xD0800020, 0x00022430},
	{0xD0800030, 0x00022430},
	{0xD0800050, 0x911500C3},
	{0xD0800060, 0x646602C4},
	{0xD0800190, 0xC2003053},
	{0xD08001C0, 0x34F4A187},
	{0xD0800650, 0x000F40F4},
	{0xD0800660, 0x84040200},
	{0xD0800080, 0x00000000},
	{0xD0800090, 0x00080000},
	{0xD08000F0, 0xC0000000},
	{0xD08001A0, 0x20C0C009},
	{0xD0800280, 0x010E0202},
	{0xD0800760, 0x00000201},
	{0xD0800770, 0x0100000A},
	{0xD0800140, 0x20004044},
	{0xD08001D0, 0x177C2779},
	{0xD08001E0, 0x07700330},
	{0xD08001F0, 0x00003F33},
	{0xD0800200, 0x0011311C},
	{0xD0800210, 0x00300000},
	{0xD0800240, 0x80000000},
	{0xD0800510, 0x010E0101},
	{0xD0800230, 0x2028006A},
	{0xD0800E10, 0x00280062},
	{0xD0800E20, 0x00280062},
	{0xD0800E30, 0x00280062},
	{0xD0800100, 0x000D0001},
	{0xD0800110, 0x200D0001},
	{0xD0800120, 0x00000001}
};
MV_DRAM_REG_INIT ddr3_a0_avng_samsung2g_500_init[MV_DDR3_AVNG_INIT_SIZE] =
{
	{0xD0800020, 0x00022430},
	{0xD0800030, 0x00022430},
	{0xD0800050, 0x911B00C3},
	{0xD0800060, 0x747703B4},
	{0xD0800190, 0xC200406C},
	{0xD08001C0, 0x3694DA09},
	{0xD0800650, 0x00144144},
	{0xD0800660, 0x84040200},
	{0xD0800080, 0x00000000},
	{0xD0800090, 0x00080000},
	{0xD08000F0, 0xC0000000},
	{0xD08001A0, 0x20C0C409},
	{0xD0800280, 0x010E0202},
	{0xD0800760, 0x00000201},
	{0xD0800770, 0x0100000A},
	{0xD0800140, 0x20004044},
	{0xD08001D0, 0x177C2779},
	{0xD08001E0, 0x07700330},
	{0xD08001F0, 0x00003F33},
	{0xD0800200, 0x0011311C},
	{0xD0800210, 0x00300000},
	{0xD0800240, 0x80000000},
	{0xD0800510, 0x010E0101},
	{0xD0800230, 0x2028006A},
	{0xD0800E10, 0x00280062},
	{0xD0800E20, 0x00280062},
	{0xD0800E30, 0x00280062},
	{0xD0800100, 0x000D0001},
	{0xD0800110, 0x200D0001},
	{0xD0800120, 0x00000001}
};
MV_DRAM_REG_INIT ddr3_a0_avng_samsung2g_533_init[MV_DDR3_AVNG_INIT_SIZE] = 
{
	{0xD0800020, 0x00022430},
	{0xD0800030, 0x00022430},
	{0xD0800050, 0x911B00C3},
	{0xD0800060, 0x747703B4},
	{0xD0800190, 0xC200406C},
	{0xD08001C0, 0x3694DA09},
	{0xD0800650, 0x00144144},
	{0xD0800660, 0x84040200},
	{0xD0800080, 0x00000000},
	{0xD0800090, 0x00080000},
	{0xD08000F0, 0xC0000000},
	{0xD08001A0, 0x20C0C409},
	{0xD0800280, 0x010E0202},
	{0xD0800760, 0x00000201},
	{0xD0800770, 0x0100000A},
	{0xD0800140, 0x20004044},
	{0xD08001D0, 0x177C2779},
	{0xD08001E0, 0x07700330},
	{0xD08001F0, 0x00003F33},
	{0xD0800200, 0x0011311C},
	{0xD0800210, 0x00300000},
	{0xD0800240, 0x80000000},
	{0xD0800510, 0x010E0101},
	{0xD0800230, 0x2028006A},
	{0xD0800E10, 0x00280062},
	{0xD0800E20, 0x00280062},
	{0xD0800E30, 0x00280062},
	{0xD0800100, 0x000D0001},
#ifdef MV_DRAM_2CS_DIMM
	{0xD0800110, 0x200D0001},
#else
	{0xD0800110, 0x000D0000},
#endif
	{0xD0800120, 0x00000001}
};
#endif

#if defined(MV_DRAM_SODIMM)
MV_DRAM_REG_INIT ddr3_y1_avng_sodimm_ddr2_400_init[MV_DDR3_AVNG_INIT_SIZE] = 
{
	{0xD0800020, 0x00022430},
	{0xD0800030, 0x00022430},
	{0xD0800050, 0x4CD700C8},
	{0xD0800060, 0x53560332},
	{0xD0800190, 0x50C83742},
	{0xD08001C0, 0x34F400A0},
	{0xD0800650, 0x001200E1},
	{0xD0800660, 0x04040200},
	{0xD0800080, 0x00000000},
	{0xD0800090, 0x00080000},
	{0xD08000F0, 0xC0000000},
	{0xD08001A0, 0x20814005},
	{0xD0800280, 0x010E0202},
	{0xD0800760, 0x00000201},
	{0xD0800770, 0x0100000A},
	{0xD0800140, 0x20004444},
	{0xD08001D0, 0x177C2CC9},
	{0xD08001E0, 0x0FF00770},
	{0xD08001F0, 0x3F000077},
	{0xD0800200, 0x0011311C},
	{0xD0800210, 0x00300000},
	{0xD0800240, 0x80000000},
	{0xD0800510, 0x010E0101},
	{0xD0800230, 0x2025006A},
	{0xD0800E10, 0x00250062},
	{0xD0800E20, 0x00250062},
	{0xD0800E30, 0x00250062},
	{0xD0800100, 0x000D0001},
#ifdef MV_DRAM_2CS_DIMM
	{0xD0800110, 0x200D0001},
#else
	{0xD0800110, 0x000D0000},
#endif
	{0xD0800120, 0x00000001}
};

MV_DRAM_REG_INIT ddr3_y1_avng_sodimm_ddr3_500_init[MV_DDR3_AVNG_INIT_SIZE] = 
{
	{0xD0800020, 0x00042430},
	{0xD0800030, 0x00042430},
	{0xD0800050, 0x911D00C3},
	{0xD0800060, 0x848803B4},
	{0xD0800190, 0x20C84AC2},
	{0xD08001C0, 0x3694DA09},
	{0xD0800650, 0x001501B1},
	{0xD0800660, 0x04040200},
	{0xD0800080, 0x00000000},
	{0xD0800090, 0x00080000},
	{0xD08000F0, 0xC0000000},
	{0xD08001A0, 0x20C0C409},
	{0xD0800280, 0x010E0202},
	{0xD0800760, 0x00000201},
	{0xD0800770, 0x0100000A},
	{0xD0800140, 0x20004044},
	{0xD08001D0, 0x17784339},
	{0xD08001E0, 0x07700330},
	{0xD08001F0, 0x00003F33},
	{0xD0800200, 0x0011311C},
	{0xD0800210, 0x00300000},
	{0xD0800240, 0x80000000},
	{0xD0800510, 0x010E0101},
	{0xD0800230, 0x2028006A},
	{0xD0800E10, 0x00280062},
	{0xD0800E20, 0x00280062},
	{0xD0800E30, 0x00280062},
	{0xD0800100, 0x000D0001},
#ifdef MV_DRAM_2CS_DIMM
	{0xD0800110, 0x200D0001},
#else
	{0xD0800110, 0x000D0000},
#endif
	{0xD0800120, 0x00000001}
};
#endif

#else /* not (RD_88F6781Y0_AVNG, RD_88F6781X0_AVNG) */

#if defined(MV_DRAM_Y1_SUPPORT) || defined(MV_DRAM_X0_SUPPORT)

MV_DRAM_REG_INIT ddr2_x0_200_init[MV_DDR2_Y1_INIT_SIZE] =
{
	{0xD0800020, 0x00022430},
	{0xD0800030, 0x00022430},
	{0xD0800050, 0x488C00C3},
	{0xD0800060, 0x323301A2},
	{0xD0800190, 0x30C81C42},
	{0xD08001C0, 0x32820050},
	{0xD0800650, 0x00090071},
	{0xD0800660, 0x04040200},
	{0xD0800080, 0x00000000},
	{0xD0800090, 0x00080000},
	{0xD08000F0, 0x80000000},
	{0xD08001A0, 0x20814005},
	{0xD0800280, 0x010E0402},
	{0xD0800760, 0x00001221},
	{0xD0800770, 0x0100000A},
	{0xD0800140, 0x20004444},
	{0xD08001D0, 0x177C2CC9},
	{0xD08001E0, 0x0FF00770},
	{0xD08001F0, 0x3F000077},
	{0xD0800200, 0x0011311C},
	{0xD0800210, 0x00300000},
	{0xD0800240, 0x80000000},
	{0xD0800510, 0x010E0101},
	{0xD0800230, 0x2025004A},
	{0xD0800E10, 0x00250042},
	{0xD0800E20, 0x00250042},
	{0xD0800E30, 0x00250042},
	{0xD0800100, 0x000D0001},
#ifdef MV_DRAM_2CS_DIMM
	{0xF1800110, 0x200D0001},
#else
	{0xF1800110, 0x000D0000},
#endif
	{0xD0800120, 0x00000001}
};


MV_DRAM_REG_INIT ddr2_x0_333_init[MV_DDR2_Y1_INIT_SIZE] =
{
	{0xD0800020, 0x00022430},
	{0xD0800030, 0x00022430},
	{0xD0800050, 0x4CD400C3},
	{0xD0800060, 0x535502A2},
	{0xD0800190, 0x40C82E42},
	{0xD08001C0, 0x34240085},
	{0xD0800650, 0x000F00C4},
	{0xD0800660, 0x04040200},
	{0xD0800080, 0x00000000},
	{0xD0800090, 0x00080000},
	{0xD08000F0, 0xC0000000},
	{0xD08001A0, 0x20814005},
	{0xD0800280, 0x010E0202},
	{0xD0800760, 0x00000201},
	{0xD0800770, 0x0100000A},
	{0xD0800140, 0x20004444},
	{0xD08001D0, 0x177C2CC9},
	{0xD08001E0, 0x0FF00770},
	{0xD08001F0, 0x3F000077},
	{0xD0800200, 0x0011311C},
	{0xD0800210, 0x00300000},
	{0xD0800240, 0x80000000},
	{0xD0800510, 0x010E0101},
	{0xD0800230, 0x2025004A},
	{0xD0800E10, 0x00250042},
	{0xD0800E20, 0x00250042},
	{0xD0800E30, 0x00250042},
	{0xD0800100, 0x000D0001},
#ifdef MV_DRAM_2CS_DIMM
	{0xF1800110, 0x200D0001},
#else
	{0xF1800110, 0x000D0000},
#endif
	{0xD0800120, 0x00000001}
};


MV_DRAM_REG_INIT ddr2_x0_400_init[MV_DDR2_Y1_INIT_SIZE] =
{
	{0xD0800020, 0x00022430},
	{0xD0800030, 0x00022430},
	{0xD0800050, 0x4CD800C3},
	{0xD0800060, 0x53560332},
	{0xD0800190, 0x50C83742},
	{0xD08001C0, 0x34F400A0},
	{0xD0800650, 0x001200E4},
	{0xD0800660, 0x04040200},
	{0xD0800080, 0x00000000},
	{0xD0800090, 0x00080000},
	{0xD08000F0, 0xC0000000},
	{0xD08001A0, 0x20814005},
	{0xD0800280, 0x010E0202},
	{0xD0800760, 0x00000201},
	{0xD0800770, 0x0100000A},
	{0xD0800140, 0x20004444},
	{0xD08001D0, 0x177C2CC9},
	{0xD08001E0, 0x0FF00770},
	{0xD08001F0, 0x3F000077},
	{0xD0800200, 0x0011311C},
	{0xD0800210, 0x00300000},
	{0xD0800240, 0x80000000},
	{0xD0800510, 0x010E0101},
	{0xD0800230, 0x2025004A},
	{0xD0800E10, 0x00250042},
	{0xD0800E20, 0x00250042},
	{0xD0800E30, 0x00250042},
	{0xD0800100, 0x000D0001},
#ifdef MV_DRAM_2CS_DIMM
	{0xF1800110, 0x200D0001},
#else
	{0xF1800110, 0x000D0000},
#endif
	{0xD0800120, 0x00000001}
};


MV_DRAM_REG_INIT ddr2_y1_200_init[MV_DDR2_Y1_INIT_SIZE] =
{
	/* DDR parameters based on U-Boot 4.3.1 */
	{0xD0800020, 0x00022430},
	{0xD0800030, 0x00022430},
	{0xD0800050, 0x4CD700C8},
	{0xD0800060, 0x53560332},
	{0xD0800190, 0x50C83742},
	{0xD08001C0, 0x34F400A0},
	{0xD0800650, 0x001200E1},
	{0xD0800660, 0x04040200},
	{0xD0800080, 0x00000000},
	{0xD0800090, 0x00080000},
	{0xD08000F0, 0xC0000000},
	{0xD08001A0, 0x20814005},
	{0xD0800280, 0x010E0402},
	{0xD0800760, 0x00000201},
	{0xD0800770, 0x0100000A},
	{0xD0800140, 0x20004444},
	{0xD08001D0, 0x177C2CC9},
	{0xD08001E0, 0x0FF00770},
	{0xD08001F0, 0x3F000077},
	{0xD0800200, 0x0011311C},
	{0xD0800210, 0x00300000},
	{0xD0800240, 0x80000000},
	{0xD0800510, 0x010E0101},
	{0xD0800230, 0x2025004A},
	{0xD0800E10, 0x00250042},
	{0xD0800E20, 0x00250042},
	{0xD0800E30, 0x00250042},
	{0xD0800100, 0x000D0001},
#ifdef MV_DRAM_2CS_DIMM
	{0xF1800110, 0x200D0001},
#else
	{0xF1800110, 0x000D0000},
#endif
	{0xD0800120, 0x00000001}
};


MV_DRAM_REG_INIT ddr2_y1_333_init[MV_DDR2_Y1_INIT_SIZE] =
{
	/* DDR parameters based on U-Boot 4.3.1, same parameters as for 400MHz */
	{0xD0800020, 0x00022430},
	{0xD0800030, 0x00022430},
	{0xD0800050, 0x4CD700C8},
	{0xD0800060, 0x53560332},
	{0xD0800190, 0x50C83742},
	{0xD08001C0, 0x34F400A0},
	{0xD0800650, 0x001200E1},
	{0xD0800660, 0x04040200},
	{0xD0800080, 0x00000000},
	{0xD0800090, 0x00080000},
	{0xD08000F0, 0xC0000000},
	{0xD08001A0, 0x20814005},
	{0xD0800280, 0x010E0202},
	{0xD0800760, 0x00000201},
	{0xD0800770, 0x0100000A},
	{0xD0800140, 0x20004444},
	{0xD08001D0, 0x177C2CC9},
	{0xD08001E0, 0x0FF00770},
	{0xD08001F0, 0x3F000077},
	{0xD0800200, 0x0011311C},
	{0xD0800210, 0x00300000},
	{0xD0800240, 0x80000000},
	{0xD0800510, 0x010E0101},
	{0xD0800230, 0x2025004A},
	{0xD0800E10, 0x00250042},
	{0xD0800E20, 0x00250042},
	{0xD0800E30, 0x00250042},
	{0xD0800100, 0x000D0001},
#ifdef MV_DRAM_2CS_DIMM
	{0xF1800110, 0x200D0001},
#else
	{0xF1800110, 0x000D0000},
#endif
	{0xD0800120, 0x00000001}
};


MV_DRAM_REG_INIT ddr2_y1_400_init[MV_DDR2_Y1_INIT_SIZE] = 
{
	/* DDR parameters based on U-Boot 4.3.1 */
	{0xD0800020, 0x00022430},
	{0xD0800030, 0x00022430},
	{0xD0800050, 0x4CD700C8},
	{0xD0800060, 0x53560332},
	{0xD0800190, 0x50C83742},
	{0xD08001C0, 0x34F400A0},
	{0xD0800650, 0x001200E1},
	{0xD0800660, 0x04040200},
	{0xD0800080, 0x00000000},
	{0xD0800090, 0x00080000},
	{0xD08000F0, 0xC0000000},
	{0xD08001A0, 0x20814005},
	{0xD0800280, 0x010E0202},
	{0xD0800760, 0x00000201},
	{0xD0800770, 0x0100000A},
	{0xD0800140, 0x20004444},
	{0xD08001D0, 0x177C2CC9},
	{0xD08001E0, 0x0FF00770},
	{0xD08001F0, 0x3F000077},
	{0xD0800200, 0x0011311C},
	{0xD0800210, 0x00300000},
	{0xD0800240, 0x80000000},
	{0xD0800510, 0x010E0101},
	{0xD0800230, 0x2025004A},
	{0xD0800E10, 0x00250042},
	{0xD0800E20, 0x00250042},
	{0xD0800E30, 0x00250042},
	{0xD0800100, 0x000D0001},
#ifdef MV_DRAM_2CS_DIMM
	{0xF1800110, 0x200D0001},
#else
	{0xF1800110, 0x000D0000},
#endif
	{0xD0800120, 0x00000001}
};


MV_DRAM_REG_INIT ddr3_y1_333_init[MV_DDR3_Y1_INIT_SIZE] = 
{
	{0xD0800020, 0x00022430},
	{0xD0800030, 0x00022430},
	{0xD0800050, 0x911100C3},
	{0xD0800060, 0x54540254},
	{0xD0800190, 0xC2002853},
	{0xD08001C0, 0x34248946},
	{0xD0800650, 0x000D40D4},
	{0xD0800660, 0x84040200},
	{0xD0800080, 0x00000000},
	{0xD0800090, 0x00080000},
	{0xD08000F0, 0xC0000000},
	{0xD08001A0, 0x20C0C009},
	{0xD0800280, 0x010E0202},
	{0xD0800760, 0x00000201},
	{0xD0800770, 0x0100000A},
	{0xD0800140, 0x20004044},
	{0xD08001D0, 0x177C2779},
	{0xD08001E0, 0x07700330},
	{0xD08001F0, 0x00003F33},
	{0xD0800200, 0x0011311C},
	{0xD0800210, 0x00300000},
	{0xD0800240, 0x80000000},
	{0xD0800510, 0x010E0101},
	{0xD0800230, 0x2028006A},
	{0xD0800E10, 0x00280062},
	{0xD0800E20, 0x00280062},
	{0xD0800E30, 0x00280062},
	{0xD0800100, 0x000D0001},
#ifdef MV_DRAM_2CS_DIMM
	{0xF1800110, 0x200D0001},
#else
	{0xF1800110, 0x000D0000},
#endif
	{0xD0800120, 0x00000001}
};


MV_DRAM_REG_INIT ddr3_y1_400_init[MV_DDR3_Y1_INIT_SIZE] = 
{
	{0xD0800020, 0x00022430},
	{0xD0800030, 0x00022430},
	{0xD0800050, 0x911500C3},
	{0xD0800060, 0x646602C4},
	{0xD0800190, 0xC2003053},
	{0xD08001C0, 0x34F4A187},
	{0xD0800650, 0x000F40F4},
	{0xD0800660, 0x84040200},
	{0xD0800080, 0x00000000},
	{0xD0800090, 0x00080000},
	{0xD08000F0, 0xC0000000},
	{0xD08001A0, 0x20C0C009},
	{0xD0800280, 0x010E0202},
	{0xD0800760, 0x00000201},
	{0xD0800770, 0x0100000A},
	{0xD0800140, 0x20004044},
	{0xD08001D0, 0x177C2779},
	{0xD08001E0, 0x07700330},
	{0xD08001F0, 0x00003F33},
	{0xD0800200, 0x0011311C},
	{0xD0800210, 0x00300000},
	{0xD0800240, 0x80000000},
	{0xD0800510, 0x010E0101},
	{0xD0800230, 0x2028006A},
	{0xD0800E10, 0x00280062},
	{0xD0800E20, 0x00280062},
	{0xD0800E30, 0x00280062},
	{0xD0800100, 0x000D0001},
#ifdef MV_DRAM_2CS_DIMM
	{0xF1800110, 0x200D0001},
#else
	{0xF1800110, 0x000D0000},
#endif
	{0xD0800120, 0x00000001}
};


MV_DRAM_REG_INIT ddr3_y1_533_init[MV_DDR3_Y1_INIT_SIZE] = 
{
	{0xD0800020, 0x00022430},
	{0xD0800030, 0x00022430},
	{0xD0800050, 0x911B00C3},
	{0xD0800060, 0x747703B4},
	{0xD0800190, 0xC200406C},
	{0xD08001C0, 0x3694DA09},
	{0xD0800650, 0x00144144},
	{0xD0800660, 0x84040200},
	{0xD0800080, 0x00000000},
	{0xD0800090, 0x00080000},
	{0xD08000F0, 0xC0000000},
	{0xD08001A0, 0x20C0C409},
	{0xD0800280, 0x010E0202},
	{0xD0800760, 0x00000201},
	{0xD0800770, 0x0100000A},
	{0xD0800140, 0x20004044},
	{0xD08001D0, 0x177C2779},
	{0xD08001E0, 0x07700330},
	{0xD08001F0, 0x00003F33},
	{0xD0800200, 0x0011311C},
	{0xD0800210, 0x00300000},
	{0xD0800240, 0x80000000},
	{0xD0800510, 0x010E0101},
	{0xD0800230, 0x2028006A},
	{0xD0800E10, 0x00280062},
	{0xD0800E20, 0x00280062},
	{0xD0800E30, 0x00280062},
	{0xD0800100, 0x000D0001},
#ifdef MV_DRAM_2CS_DIMM
	{0xD0800110, 0x200D0001},
#else
	{0xD0800110, 0x000D0000},
#endif
	{0xD0800120, 0x00000001}
};


#else /********************* Y0 Settings ***********************/


MV_DRAM_REG_INIT ddr2_y0_200_init[MV_DDR2_Y0_INIT_SIZE] = 
{
	{0xF1800020, 0x00022430},
	{0xF1800030, 0x00022430},
	{0xF1800050, 0x488B00C8},
	{0xF1800060, 0x323301A2},
	{0xF1800190, 0x30C81C42},
	{0xF18001C0, 0x32820050},
	{0xF1800650, 0x00090071},
	{0xF1800080, 0x00000000},
	{0xF1800090, 0x00080000},
	{0xF18000F0, 0xC0000000},
	{0xF18001A0, 0x20814005},
	{0xF1800280, 0x010E0202},
	{0xF1800760, 0x00000201},
	{0xF1800770, 0x0100000A},
	{0xF1800140, 0x20004455},
	{0xF18001D0, 0x17784779},
	{0xF18001E0, 0x0FF00770},
	{0xF18001F0, 0x3F000077},
	{0xF1800200, 0x0011311C},
	{0xF1800210, 0x00300000},
	{0xF1800240, 0x80000000},
	{0xF1800510, 0x010E0101},
	{0xF1800230, 0x2025004A},
	{0xF1800E10, 0x00250042},
	{0xF1800E20, 0x00250042},
	{0xF1800E30, 0x00250042},
	{0xF1800100, 0x000D0001},
#ifdef MV_DRAM_2CS_DIMM
	{0xF1800110, 0x200D0001},
#else
	{0xF1800110, 0x000D0000},
#endif
	{0xF1800120, 0x00000001}
};

MV_DRAM_REG_INIT ddr2_y0_400_init[MV_DDR2_Y0_INIT_SIZE] = 
{
	{0xF1800020, 0x00022430},
	{0xF1800030, 0x00022430},
	{0xF1800050, 0x4CD700C8},
	{0xF1800060, 0x53560332},
	{0xF1800190, 0x50C83742},
	{0xF18001C0, 0x34F400A0},
	{0xF1800650, 0x001200E1},
	{0xF1800080, 0x00000000},
	{0xF1800090, 0x00080000},
	{0xF18000F0, 0xC0000000},
	{0xF18001A0, 0x20814005},
	{0xF1800280, 0x010E0202},
	{0xF1800760, 0x00000201},
	{0xF1800770, 0x0100000A},
	{0xF1800140, 0x20004455},
	{0xF18001D0, 0x17784779},
	{0xF18001E0, 0x0FF00770},
	{0xF18001F0, 0x3F000077},
	{0xF1800200, 0x0011311C},
	{0xF1800210, 0x00300000},
	{0xF1800240, 0x80000000},
	{0xF1800510, 0x010E0101},
	{0xF1800230, 0x2025004A},
	{0xF1800E10, 0x00250042},
	{0xF1800E20, 0x00250042},
	{0xF1800E30, 0x00250042},
	{0xF1800100, 0x000D0001},
#ifdef MV_DRAM_2CS_DIMM
	{0xF1800110, 0x200D0001},
#else
	{0xF1800110, 0x000D0000},
#endif
	{0xF1800120, 0x00000001}
};

MV_DRAM_REG_INIT ddr2_y0_500_init[MV_DDR2_Y0_INIT_SIZE] = 
{
	{0xF1800020, 0x00022430},
	{0xF1800030, 0x00022430},
	{0xF1800050, 0x911D00C8},
	{0xF1800060, 0x848803B4},
	{0xF1800190, 0x20C84AC2},
	{0xF18001C0, 0x3698D917},
	{0xF1800650, 0x001501B1},
	{0xF1800080, 0x00000000},
	{0xF1800090, 0x00080000},
	{0xF18000F0, 0xC0000000},
	{0xF18001A0, 0x20814005},
	{0xF1800280, 0x010E0202},
	{0xF1800760, 0x00000201},
	{0xF1800770, 0x0100000A},
	{0xF1800140, 0x20004455},
	{0xF18001D0, 0x17784779},
	{0xF18001E0, 0x0FF00770},
	{0xF18001F0, 0x3F000077},
	{0xF1800200, 0x0011311C},
	{0xF1800210, 0x00300000},
	{0xF1800240, 0x80000000},
	{0xF1800510, 0x010E0101},
	{0xF1800230, 0x2025004A},
	{0xF1800E10, 0x00250042},
	{0xF1800E20, 0x00250042},
	{0xF1800E30, 0x00250042},
	{0xF1800100, 0x000D0001},
#ifdef MV_DRAM_2CS_DIMM
	{0xF1800110, 0x200D0001},
#else
	{0xF1800110, 0x000D0000},
#endif
	{0xF1800120, 0x00000001}
};


MV_DRAM_REG_INIT ddr3_y0_500_init[MV_DDR3_Y0_INIT_SIZE] = 
{
	{0xF1800020, 0x00022430},
	{0xF1800030, 0x00022430},
	{0xF1800050, 0x911D00C8},
	{0xF1800060, 0x848803B4},
	{0xF1800190, 0x20C84AC2},
	{0xF18001C0, 0x3698D917},
	{0xF1800650, 0x001501B1},
	{0xF1800080, 0x00000000},
	{0xF1800090, 0x00080040},
	{0xF18000F0, 0xC0000000},
	{0xF18001A0, 0x2081C009},
	{0xF1800280, 0x010E0202},
	{0xF1800760, 0x00000201},
	{0xF1800770, 0x01000005},
	{0xF1800140, 0x20004044},
	{0xF18001D0, 0x17784339},
	{0xF18001E0, 0x07700330},
	{0xF18001F0, 0x00000033},
	{0xF1800200, 0x00113104},
	{0xF1800210, 0x00300000},
	{0xF1800240, 0x80000000},
	{0xF1800510, 0x010E0101},
	{0xF1800230, 0x2028006A},
	{0xF1800E10, 0x00280062},
	{0xF1800E20, 0x00280062},
	{0xF1800E30, 0x00280062},
	{0xF1800100, 0x000D0001},
#ifdef MV_DRAM_2CS_DIMM
	{0xF1800110, 0x200D0001},
#else
	{0xF1800110, 0x000D0000},
#endif
	{0xF1800120, 0x00000001}
};
#endif /* Y0 Settings */

#endif /* RD_88F6781Y0_AVNG, RD_88F6781X0_AVNG */

/* frequency mask */
#define MV_DDR_100	BIT0
#define MV_DDR_133	BIT1
#define MV_DDR_167	BIT2
#define MV_DDR_200	BIT3
#define MV_DDR_233	BIT4
#define MV_DDR_250	BIT5
#define MV_DDR_267	BIT6
#define MV_DDR_333	BIT7
#define MV_DDR_400	BIT8
#define MV_DDR_500	BIT9
#define MV_DDR_533	BIT10
#define MV_DDR_ALL	0xFFFFFFFF

static void dram_init_setup(void)
{
	/* setup DRAM initialization to pass to Linux */

	memset(&(mv_dram_init), 0, sizeof(MV_DRAM_INIT));

	/* Identify "default settings" situation - board booted with unsupported sample at reset configuration */
	if ( (*(MV_U32 *)(CRYPT_ENG_BASE + 0x4)) == 0xDEADBEEF)
	{
		printf("Warning: using default DRAM parameters! Check your Sample at Reset configuration\n");
		return;
	}

	/* Identify "5:2 divider mode" situation - board booted with 5:2 ratio - don't pass parameters to Linux */
	if ( (*(MV_U32 *)(CRYPT_ENG_BASE + 0x4)) == 0x5252DEAD)
	{
		printf("Warning: Not passing DRAM parameters to Linux PM\n");
		return;
	}

#if defined(RD_88F6781Y0_AVNG) || defined(RD_88F6781X0_AVNG) || defined(RD_88AP510A0_AVNG)

#if defined(MV_DRAM_SODIMM)
	mv_dram_init.dram_init_ctrl[0].freq_mask = (MV_DDR_ALL);
	mv_dram_init.dram_init_ctrl[0].start_index = 0;
	mv_dram_init.dram_init_ctrl[0].size = MV_DDR3_AVNG_INIT_SIZE;
	mv_dram_init.dram_init_ctrl[0].flags = 0;
	if (mvBoardDdrTypeGet() == 0) { /* DDR2 */
		memcpy(&(mv_dram_init.reg_init[0]), ddr3_y1_avng_sodimm_ddr2_400_init,
			(MV_DDR3_AVNG_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));
	} else { /* DDR3 */
		memcpy(&(mv_dram_init.reg_init[0]), ddr3_y1_avng_sodimm_ddr3_500_init,
			(MV_DDR3_AVNG_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));
	}
#else
	/* Entry 1 for DDR3 400 MHz */
	mv_dram_init.dram_init_ctrl[0].freq_mask = (    MV_DDR_100 | MV_DDR_133 | MV_DDR_167 | MV_DDR_200 | 
							MV_DDR_233 | MV_DDR_250 | MV_DDR_267 | MV_DDR_333 | MV_DDR_400);
	mv_dram_init.dram_init_ctrl[0].start_index = 0;
	mv_dram_init.dram_init_ctrl[0].size = MV_DDR3_AVNG_INIT_SIZE;
	mv_dram_init.dram_init_ctrl[0].flags = 0;
#if defined(MV_DRAM_ELPIDA_1G)
	memcpy(&(mv_dram_init.reg_init[0]), ddr3_y1_avng_elpida_400_init,
		(MV_DDR3_AVNG_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));
#elif defined(MV_DRAM_SAMSUNG_1G)
	memcpy(&(mv_dram_init.reg_init[0]), ddr3_y1_avng_samsung1g_400_init,
		(MV_DDR3_AVNG_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));
#elif defined(MV_DRAM_SAMSUNG_2G)
	memcpy(&(mv_dram_init.reg_init[0]), ddr3_y1_avng_samsung2g_400_init,
		(MV_DDR3_AVNG_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));
#elif defined(MV_DRAM_SAMSUNG_2G_A0)
	memcpy(&(mv_dram_init.reg_init[0]), ddr3_a0_avng_samsung2g_400_init,
		(MV_DDR3_AVNG_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));
#else
	#error "Undefined type of DDR3\n"
#endif
	/* Entry 2 for DDR3 533/500 MHz */
	mv_dram_init.dram_init_ctrl[1].freq_mask = (MV_DDR_500 | MV_DDR_533);
	mv_dram_init.dram_init_ctrl[1].start_index = 
			(mv_dram_init.dram_init_ctrl[0].start_index + mv_dram_init.dram_init_ctrl[0].size);
	mv_dram_init.dram_init_ctrl[1].size = MV_DDR3_AVNG_INIT_SIZE;
	mv_dram_init.dram_init_ctrl[1].flags = 0;
#if defined(MV_DRAM_ELPIDA_1G)
	memcpy(&(mv_dram_init.reg_init[1*MV_DDR3_AVNG_INIT_SIZE]),
		ddr3_y1_avng_elpida_533_init, (MV_DDR3_AVNG_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));
#elif defined(MV_DRAM_SAMSUNG_1G)
	memcpy(&(mv_dram_init.reg_init[1*MV_DDR3_AVNG_INIT_SIZE]),
		ddr3_y1_avng_samsung1g_533_init, (MV_DDR3_AVNG_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));
#elif defined(MV_DRAM_SAMSUNG_2G)
	memcpy(&(mv_dram_init.reg_init[1*MV_DDR3_AVNG_INIT_SIZE]),
		ddr3_y1_avng_samsung2g_533_init, (MV_DDR3_AVNG_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));
#elif defined(MV_DRAM_SAMSUNG_2G_A0)
	memcpy(&(mv_dram_init.reg_init[1*MV_DDR3_AVNG_INIT_SIZE]),
		ddr3_a0_avng_samsung2g_500_init, (MV_DDR3_AVNG_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));
#else
	#error "Undefined type of DDR3\n"
#endif
#endif /* not MV_DRAM_SODIMM */

#else /* not (RD_88F6781Y0_AVNG, RD_88F6781X0_AVNG) ,RD_88AP510A0_AVNG)*/

#if defined(MV_DRAM_X0_SUPPORT)

	if (mvBoardDdrTypeGet() == 0) { /* DDR2 */

		mv_dram_init.dram_init_ctrl[0].freq_mask = (MV_DDR_100 | MV_DDR_133 | MV_DDR_167 | MV_DDR_200);
		mv_dram_init.dram_init_ctrl[0].start_index = 0;
		mv_dram_init.dram_init_ctrl[0].size = MV_DDR2_Y1_INIT_SIZE;
		mv_dram_init.dram_init_ctrl[0].flags = 0;

		memcpy(&(mv_dram_init.reg_init[0]), ddr2_x0_200_init, (MV_DDR2_Y1_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));

		mv_dram_init.dram_init_ctrl[1].freq_mask = (MV_DDR_233 | MV_DDR_250 | MV_DDR_267 | MV_DDR_333);
		mv_dram_init.dram_init_ctrl[1].start_index = 
			(mv_dram_init.dram_init_ctrl[0].start_index + mv_dram_init.dram_init_ctrl[0].size);
		mv_dram_init.dram_init_ctrl[1].size = MV_DDR2_Y1_INIT_SIZE ;
		mv_dram_init.dram_init_ctrl[1].flags = 0;

		memcpy(&(mv_dram_init.reg_init[1*MV_DDR2_Y1_INIT_SIZE]), 
			ddr2_x0_333_init, (MV_DDR2_Y1_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));

		/* Entry 3 for DDR2 400 MHz */
		mv_dram_init.dram_init_ctrl[2].freq_mask = (MV_DDR_400);
		mv_dram_init.dram_init_ctrl[2].start_index = 
			(mv_dram_init.dram_init_ctrl[1].start_index + mv_dram_init.dram_init_ctrl[1].size);
		mv_dram_init.dram_init_ctrl[2].size = MV_DDR2_Y1_INIT_SIZE;
		mv_dram_init.dram_init_ctrl[2].flags = 0;

		memcpy(&(mv_dram_init.reg_init[2*MV_DDR2_Y1_INIT_SIZE]), 
			ddr2_x0_400_init, (MV_DDR2_Y1_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));
	}
	else {
		mv_dram_init.dram_init_ctrl[0].freq_mask = 	(MV_DDR_100 | MV_DDR_133 | MV_DDR_167 | MV_DDR_200 | 
								MV_DDR_233 | MV_DDR_250 | MV_DDR_267 | MV_DDR_333);
		mv_dram_init.dram_init_ctrl[0].start_index = 0;
		mv_dram_init.dram_init_ctrl[0].size = MV_DDR3_Y1_INIT_SIZE;
		mv_dram_init.dram_init_ctrl[0].flags = 0;

		memcpy(&(mv_dram_init.reg_init[0]), ddr3_y1_333_init, (MV_DDR3_Y1_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));

		mv_dram_init.dram_init_ctrl[1].freq_mask = (MV_DDR_400);
		mv_dram_init.dram_init_ctrl[1].start_index = 
				(mv_dram_init.dram_init_ctrl[0].start_index + mv_dram_init.dram_init_ctrl[0].size);
		mv_dram_init.dram_init_ctrl[1].size = MV_DDR3_Y1_INIT_SIZE;
		mv_dram_init.dram_init_ctrl[1].flags = 0;

		memcpy(&(mv_dram_init.reg_init[1*MV_DDR3_Y1_INIT_SIZE]), 
			ddr3_y1_400_init, (MV_DDR3_Y1_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));

		mv_dram_init.dram_init_ctrl[2].freq_mask = (MV_DDR_500 | MV_DDR_533);
		mv_dram_init.dram_init_ctrl[2].start_index = 
				(mv_dram_init.dram_init_ctrl[1].start_index + mv_dram_init.dram_init_ctrl[1].size);
		mv_dram_init.dram_init_ctrl[2].size = MV_DDR3_Y1_INIT_SIZE;
		mv_dram_init.dram_init_ctrl[2].flags = 0;

		memcpy(&(mv_dram_init.reg_init[2*MV_DDR3_Y1_INIT_SIZE]), 
				ddr3_y1_533_init, (MV_DDR3_Y1_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));
	}

#elif defined(MV_DRAM_Y1_SUPPORT)

	if (mvBoardDdrTypeGet() == 0) { /* DDR2 */

		mv_dram_init.dram_init_ctrl[0].freq_mask = (MV_DDR_100 | MV_DDR_133 | MV_DDR_167 | MV_DDR_200);
		mv_dram_init.dram_init_ctrl[0].start_index = 0;
		mv_dram_init.dram_init_ctrl[0].size = MV_DDR2_Y1_INIT_SIZE;
		mv_dram_init.dram_init_ctrl[0].flags = 0;

		memcpy(&(mv_dram_init.reg_init[0]), ddr2_y1_200_init, (MV_DDR2_Y1_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));

		mv_dram_init.dram_init_ctrl[1].freq_mask = (MV_DDR_233 | MV_DDR_250 | MV_DDR_267 | MV_DDR_333);
		mv_dram_init.dram_init_ctrl[1].start_index = 
			(mv_dram_init.dram_init_ctrl[0].start_index + mv_dram_init.dram_init_ctrl[0].size);
		mv_dram_init.dram_init_ctrl[1].size = MV_DDR2_Y1_INIT_SIZE ;
		mv_dram_init.dram_init_ctrl[1].flags = 0;

		memcpy(&(mv_dram_init.reg_init[1*MV_DDR2_Y1_INIT_SIZE]), 
			ddr2_y1_333_init, (MV_DDR2_Y1_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));

		/* Entry 3 for DDR2 400 MHz */
		mv_dram_init.dram_init_ctrl[2].freq_mask = (MV_DDR_400);
		mv_dram_init.dram_init_ctrl[2].start_index = 
			(mv_dram_init.dram_init_ctrl[1].start_index + mv_dram_init.dram_init_ctrl[1].size);
		mv_dram_init.dram_init_ctrl[2].size = MV_DDR2_Y1_INIT_SIZE;
		mv_dram_init.dram_init_ctrl[2].flags = 0;

		memcpy(&(mv_dram_init.reg_init[2*MV_DDR2_Y1_INIT_SIZE]), 
			ddr2_y1_400_init, (MV_DDR2_Y1_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));
	}
	else {
		mv_dram_init.dram_init_ctrl[0].freq_mask = 	(MV_DDR_100 | MV_DDR_133 | MV_DDR_167 | MV_DDR_200 | 
								MV_DDR_233 | MV_DDR_250 | MV_DDR_267 | MV_DDR_333);
		mv_dram_init.dram_init_ctrl[0].start_index = 0;
		mv_dram_init.dram_init_ctrl[0].size = MV_DDR3_Y1_INIT_SIZE;
		mv_dram_init.dram_init_ctrl[0].flags = 0;

		memcpy(&(mv_dram_init.reg_init[0]), ddr3_y1_333_init, (MV_DDR3_Y1_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));

		mv_dram_init.dram_init_ctrl[1].freq_mask = (MV_DDR_400);
		mv_dram_init.dram_init_ctrl[1].start_index = 
				(mv_dram_init.dram_init_ctrl[0].start_index + mv_dram_init.dram_init_ctrl[0].size);
		mv_dram_init.dram_init_ctrl[1].size = MV_DDR3_Y1_INIT_SIZE;
		mv_dram_init.dram_init_ctrl[1].flags = 0;

		memcpy(&(mv_dram_init.reg_init[1*MV_DDR3_Y1_INIT_SIZE]), 
			ddr3_y1_400_init, (MV_DDR3_Y1_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));

		mv_dram_init.dram_init_ctrl[2].freq_mask = (MV_DDR_500 | MV_DDR_533);
		mv_dram_init.dram_init_ctrl[2].start_index = 
				(mv_dram_init.dram_init_ctrl[1].start_index + mv_dram_init.dram_init_ctrl[1].size);
		mv_dram_init.dram_init_ctrl[2].size = MV_DDR3_Y1_INIT_SIZE;
		mv_dram_init.dram_init_ctrl[2].flags = 0;

		memcpy(&(mv_dram_init.reg_init[2*MV_DDR3_Y1_INIT_SIZE]), 
				ddr3_y1_533_init, (MV_DDR3_Y1_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));
	}

#elif defined (MV_DRAM_Y0_SUPPORT) 

	if (mvBoardDdrTypeGet() == 0) { /* DDR2 */

		mv_dram_init.dram_init_ctrl[0].freq_mask = (MV_DDR_100 | MV_DDR_133 | MV_DDR_167 | MV_DDR_200);
		mv_dram_init.dram_init_ctrl[0].start_index = 0;
		mv_dram_init.dram_init_ctrl[0].size = MV_DDR2_Y0_INIT_SIZE;
		mv_dram_init.dram_init_ctrl[0].flags = 0;

		memcpy(&(mv_dram_init.reg_init[0]), ddr2_y0_200_init, (MV_DDR2_Y0_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));

		mv_dram_init.dram_init_ctrl[1].freq_mask = (MV_DDR_233 | MV_DDR_250 | MV_DDR_267 | MV_DDR_333 | MV_DDR_400);
		mv_dram_init.dram_init_ctrl[1].start_index = 
			(mv_dram_init.dram_init_ctrl[0].start_index + mv_dram_init.dram_init_ctrl[0].size);
		mv_dram_init.dram_init_ctrl[1].size = MV_DDR2_Y0_INIT_SIZE ;
		mv_dram_init.dram_init_ctrl[1].flags = 0;

		memcpy(&(mv_dram_init.reg_init[1*MV_DDR2_Y0_INIT_SIZE]), 
			ddr2_y0_400_init, (MV_DDR2_Y0_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));

		/* Entry 3 for DDR2 533/500 MHz */
		mv_dram_init.dram_init_ctrl[2].freq_mask = (MV_DDR_500 | MV_DDR_533);
		mv_dram_init.dram_init_ctrl[2].start_index = 
			(mv_dram_init.dram_init_ctrl[1].start_index + mv_dram_init.dram_init_ctrl[1].size);
		mv_dram_init.dram_init_ctrl[2].size = MV_DDR2_Y0_INIT_SIZE;
		mv_dram_init.dram_init_ctrl[2].flags = 0;

		memcpy(&(mv_dram_init.reg_init[2*MV_DDR2_Y0_INIT_SIZE]), 
			ddr2_y0_500_init, (MV_DDR2_Y0_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));
	}
	else {
		/* DDR3: copy all available freqs from DDR3 table, then add Write Leveling */
		mv_dram_init.dram_init_ctrl[0].freq_mask = MV_DDR_ALL;
		mv_dram_init.dram_init_ctrl[0].start_index = 0;

		mv_dram_init.dram_init_ctrl[0].size = MV_DDR3_Y0_INIT_SIZE;

		mv_dram_init.dram_init_ctrl[0].flags = 0;

		memcpy(&(mv_dram_init.reg_init[0]), ddr3_y0_500_init, (MV_DDR3_Y0_INIT_SIZE*sizeof(MV_DRAM_REG_INIT)));
	}
#else
	#error "Undefined type of DRAM support (should be Y0/Y1/X0)\n"
#endif

#endif /* RD_88F6781Y0_AVNG, RD_88F6781X0_AVNG */
}

/***************************************************************************/

int dram_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;
	unsigned int i, dramTotalSize=0;
	char name[30];
	MV_32 memBase;
	MV_U32 wlValue = 0;
	

#ifndef RD_88F6781
	if (mvBoardDdrTypeGet() != 0)
	{
		/* Sanity Check - Write Leveling values should not be all zeros */
		if ( (*(MV_U32 *)CRYPT_ENG_BASE) == 0) 
		{
			printf("\nWarning: DDR3 Write Leveling values are all zeros!\n");
			printf("Make sure you are using the latest BootROM and U-Boot versions.\n\n");
		}
		else
		{
			/* Print DDR3 Write Leveling results */
			printf("Write Leveling Results:\n");
			for (i = 0; i < 4; i++) {
				wlValue = ( ((*(MV_U32 *)CRYPT_ENG_BASE) >> (i*8)) & 0xFF );
				write_leveling_reg_init[(2*i)].reg_addr = 0xF1800E40;
				write_leveling_reg_init[(2*i)].reg_value = (0x00000100 + i);
				write_leveling_reg_init[(2*i)+1].reg_addr = 0xF1800E50;
				write_leveling_reg_init[(2*i)+1].reg_value = ((wlValue << 0x10) | wlValue);
				printf("       CS0, Lane: %d, value: %d\n", i, wlValue);
#if defined(MV_DRAM_2CS_DIMM)
				wlValue = ( ((*(MV_U32 *)(CRYPT_ENG_BASE + 4)) >> (i*8)) & 0xFF );
				write_leveling_reg_init[(2*i)+8].reg_addr = 0xF1800E40;
				write_leveling_reg_init[(2*i)+8].reg_value = (0x00000200 + i);
				write_leveling_reg_init[(2*i)+9].reg_addr = 0xF1800E50;
				write_leveling_reg_init[(2*i)+9].reg_value = ((wlValue << 0x10) | wlValue);
				printf("       CS1, Lane: %d, value: %d\n", i, wlValue);
#else
				write_leveling_reg_init[(2*i)+8].reg_addr = 0;
				write_leveling_reg_init[(2*i)+8].reg_value = 0;
				write_leveling_reg_init[(2*i)+9].reg_addr = 0;
				write_leveling_reg_init[(2*i)+9].reg_value = 0;
#endif /* MV_DRAM_2CS_DIMM */
			}
		}
	}

	dram_init_setup(); /* only call this after setting write leveling values */

#endif


#if defined(MV_INC_BOARD_DDIM)
	/* Call dramInit */
	if (0 == (initdram(0)))
	{
		printf("DRAM Initialization Failed\n");
		reset_cpu(0);
		return (1);
	}
#endif

	//mvIntrfaceParamPrint();

	for(i = 0; i< MV_DRAM_MAX_CS; i++)
	{
		memBase = mvDramIfBankBaseGet(i);
		if (MV_ERROR == memBase)
			gd->bd->bi_dram[i].start = 0;
		else
			gd->bd->bi_dram[i].start = memBase;

		gd->bd->bi_dram[i].size = mvDramIfBankSizeGet(i);
		dramTotalSize += gd->bd->bi_dram[i].size;
		//if (gd->bd->bi_dram[i].size)
		//{
			//printf("DRAM CS[%d] base 0x%08lx   ",i, gd->bd->bi_dram[i].start);
			//mvSizePrint(gd->bd->bi_dram[i].size);
			//printf("\n");
		//}
	}
 
//	printf("DRAM Total ");
//	mvSizePrint(dramTotalSize);
//	mvIntrfaceWidthPrint();
//	printf("\n");

#ifdef MV_INC_DRAM_MFG_TEST
	mvDramMfgTrst();
#endif
        return 0;
}

#if defined(MV_INC_BOARD_DDIM)

/* u-boot interface function to SDRAM init - this is where all the
 * controlling logic happens */
long int initdram(int board_type)
{
    MV_VOIDFUNCPTR pRom; 
    MV_U32 forcedCl;    /* Forced CAS Latency */
    MV_U32 totalSize;
    char * env;
    MV_TWSI_ADDR slave;

    /* r0 <- current position of code   */
    /* test if we run from flash or RAM */
    if(dramBoot != 1)
    {
    slave.type = ADDR7_BIT;
    slave.address = 0;
    mvTwsiInit(0, CONFIG_SYS_I2C_SPEED, CONFIG_SYS_TCLK, &slave, 0);

    /* Calculating MIN/MAX CAS latency according to user settings */
    env = getenv("CASset");
	
    if(env && (strcmp(env,"1.5") == 0))
    {
        forcedCl = 15;
    }
    else if(env && (strcmp(env,"2") == 0))
    {
        forcedCl = 20;
    }
    else if(env && (strcmp(env,"2.5") == 0))
    {
        forcedCl = 25;
    }
    else if(env && (strcmp(env,"3") == 0))
    {
        forcedCl = 30;
    }
    else if(env && (strcmp(env,"4") == 0))
    {
        forcedCl = 40;
    }
    else if(env && (strcmp(env,"5") == 0))
    {
        forcedCl = 50;
    }
    else if(env && (strcmp(env,"6") == 0))
    {
        forcedCl = 60;
    }
    else
    {
        forcedCl = 0;
    }
    
    /* detect the dram configuartion parameters */
    if (MV_OK != mvDramIfDetect(forcedCl,1))
    {
        printf("DRAM Auto Detection Failed! System Halt!\n");
        return 0;
    }

    /* set the dram configuration */
    /* Calculate jump address of _mvDramIfConfig() */

#if defined(MV_BOOTROM)
    pRom = (MV_VOIDFUNCPTR)(((MV_VOIDFUNCPTR)_mvDramIfConfig - (MV_VOIDFUNCPTR)_start) +
		(MV_VOIDFUNCPTR)CONFIG_SYS_MONITOR_BASE + (MV_VOIDFUNCPTR)MONITOR_HEADER_LEN);
#else
    pRom = (MV_VOIDFUNCPTR)(((MV_VOIDFUNCPTR)_mvDramIfConfig - (MV_VOIDFUNCPTR)_start) +
					(MV_VOIDFUNCPTR)CONFIG_SYS_MONITOR_BASE);
#endif

    
    (*pRom) (); /* Jump to _mvDramIfConfig*/
    }
    
    totalSize = mvDramIfSizeGet();


    return(totalSize);
}

#endif /* #if defined(MV_INC_BOARD_DDIM) */

#ifdef MV_INC_DRAM_MFG_TEST
static MV_VOID mvDramMfgTrst(void)
{

	/* Memory test */
	DECLARE_GLOBAL_DATA_PTR;
	unsigned int mem_len,i,j, pattern;
	unsigned int *mem_start;
        char *env;
        
	env = getenv("enaPost");
        if(!env || ( (strcmp(env,"Yes") == 0) || (strcmp(env,"yes") == 0) ) )
	{
	    printf("Memory test pattern: ");

	    for (j = 0 ; j<2 ; j++)
	    {

		switch(j){
		case 0:
		    pattern=0x55555555;
		    printf("0x%X, ",pattern);
		    break;
		case 1:
		    pattern=0xAAAAAAAA;
		    printf("0x%X, ",pattern);
		    break;
		default:
		    pattern=0x0;
		    printf("0x%X, ",pattern);
		    break;
		}

		for(i = 0; i< MV_DRAM_MAX_CS; i++)
		{
		    mem_start = (unsigned int *)gd->bd->bi_dram[i].start;
		    mem_len = gd->bd->bi_dram[i].size;
		    if (i == 0)
		    {
			mem_start+= _4M;
			mem_len-= _4M;
		    }
		    mem_len/=4;
		    if (MV_OK != mv_mem_test(mem_start, pattern, mem_len))
		    {
			printf(" Fail!\n");
			while(1);
		    }
		}
	    }
	    printf(" Pass\n");
	}
}


static MV_STATUS mv_mem_test(MV_U32* pMem, MV_U32 pattern, MV_U32 count)
{
	int i;
	for (i=0 ; i< count ; i+=1)
		*(pMem + i) = pattern;
	
	if (MV_OK != mv_mem_cmp(pMem, pattern, count))
	{
		return MV_ERROR;
	}
	return MV_OK;
}

static MV_STATUS mv_mem_cmp(MV_U32* pMem, MV_U32 pattern, MV_U32 count)
{
	int i;
	for (i=0 ; i< count ; i+=1)
	{
		if (*(pMem + i) != pattern)
		{
			printf("Fail\n");
			printf("Test failed at 0x%x\n",(pMem + i));
			return MV_ERROR;
		}
	}

	return MV_OK;
}
#endif /* MV_INC_DRAM_MFG_TEST */
