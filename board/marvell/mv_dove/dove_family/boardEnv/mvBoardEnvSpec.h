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


#ifndef __INCmvBoardEnvSpech
#define __INCmvBoardEnvSpech

#include "mvSysHwConfig.h"


/* For future use */
#define BD_ID_DATA_START_OFFS				0x0
#define BD_DETECT_SEQ_OFFS					0x0
#define BD_SYS_NUM_OFFS						0x4
#define BD_NAME_OFFS						0x8

/* I2C bus addresses */
#define MV_BOARD_CTRL_I2C_ADDR				0x0     /* Controller slave addr */
#define MV_BOARD_CTRL_I2C_ADDR_TYPE 		ADDR7_BIT
#define MV_BOARD_DIMM0_I2C_ADDR				0x56
#define MV_BOARD_DIMM0_I2C_ADDR_TYPE 		ADDR7_BIT
#define MV_BOARD_DIMM1_I2C_ADDR				0x54
#define MV_BOARD_DIMM1_I2C_ADDR_TYPE 		ADDR7_BIT
#define MV_BOARD_EEPROM_I2C_ADDR	    	0x51
#define MV_BOARD_EEPROM_I2C_ADDR_TYPE 		ADDR7_BIT
#define MV_BOARD_MAIN_EEPROM_I2C_ADDR	   	0x50
#define MV_BOARD_MAIN_EEPROM_I2C_ADDR_TYPE 	ADDR7_BIT
#define MV_BOARD_MUX_I2C_ADDR_ENTRY			0x2
#define MV_BOARD_DIMM_I2C_CHANNEL			0x0

#define BOOT_FLASH_INDEX					0
#define MAIN_FLASH_INDEX					1




#define MV_BOARD_SYSCLK_100MHZ				100000000   
#define MV_BOARD_SYSCLK_125MHZ				125000000  
#define MV_BOARD_SYSCLK_133MHZ				133333333   
#define MV_BOARD_SYSCLK_150MHZ				150000000  
#define MV_BOARD_SYSCLK_166MHZ				166666667   
#define MV_BOARD_SYSCLK_200MHZ				200000000   
#define MV_BOARD_SYSCLK_233MHZ				233333333   
#define MV_BOARD_SYSCLK_250MHZ				250000000   
#define MV_BOARD_SYSCLK_266MHZ				266666667


/* Board specific */
/* =============================== */

/* boards ID numbers */

#define BOARD_ID_BASE           			0x0

/* New board ID numbers */
#define DB_88F6781_BP_ID					(BOARD_ID_BASE+0x0)
#define DB_88F6781_BP_MLL_ID				1788
#define RD_88F6781_ID						(BOARD_ID_BASE+0x1)
#define RD_88F6781_MLL_ID					1789
#define RD_88F6781_AVNG_ID					(BOARD_ID_BASE+0x2)
#define RD_88F6781_AVNG_MLL_ID				1790
#define DB_88F6781Y0_BP_ID					(BOARD_ID_BASE+0x3)
/* reusing the board ID pased to Linux - same as DB-Z0 board */
#define DB_88F6781Y0_BP_MLL_ID				1788
#define RD_88F6781Y0_AVNG_ID				(BOARD_ID_BASE+0x4)
/* reusing the board ID pased to Linux - same as RD_88F6781_ID board */
#define RD_88F6781Y0_AVNG_MLL_ID			1789
#define RD_88F6781X0_PLUG_ID				(BOARD_ID_BASE+0x5)
#define RD_88F6781X0_PLUG_MLL_ID			2552
#define DB_88AP510BP_B_BP_ID				(BOARD_ID_BASE+0x6)
/* reusing the board ID pased to Linux - same as DB-Z0 board */
#define DB_88AP510BP_B_BP_MLL_ID			1788
#define DB_88AP510_PCAC_ID					(BOARD_ID_BASE+0x7)
/* reusing the board ID pased to Linux - same as DB-Z0 board */
#define DB_88AP510_PCAC_MLL_ID				1799
#define BOARD_ID_88F6781_MAX				(BOARD_ID_BASE+0x9)
/* reusing the board ID pased to Linux - same as RD_88F6781_ID board */
#define RD_88AP510A0_AVNG_ID				(BOARD_ID_BASE+0x8)
#define RD_88AP510A0_AVNG_MLL_ID			3013

#define MV_MAX_BOARD_ID 					BOARD_ID_88F6781_MAX

/* DB-88F6781-BP Z0 */
#define DB_88F6781_MPP0_7                   0x00000000
#define DB_88F6781_MPP8_15                  0x00442000
#define DB_88F6781_MPP16_23                 0x22220033
#define DB_88F6781_MPP24_31                 0x00000000
#define DB_88F6781_MPP32_39                 0x00000000
#define DB_88F6781_MPP40_47                 0x00000000
#define DB_88F6781_MPP48_55                 0x00000000
#define DB_88F6781_OE_LOW                   (~((BIT0)|(BIT8)|(BIT14)|(BIT15)|(BIT18)|(BIT19)))
#define DB_88F6781_OE_HIGH                  0xFFFFFFFF
#define DB_88F6781_OE_VAL_LOW               0x1
#define DB_88F6781_OE_VAL_HIGH              0x0

/* DB-88F6781-BP Y0 */
#define DB_88F6781Y0_MPP0_7					0x00000033
#define DB_88F6781Y0_MPP8_15				0x00442000
#define DB_88F6781Y0_MPP16_23				0x66664040
#define DB_88F6781Y0_MPP24_31				0x11111111
#define DB_88F6781Y0_MPP32_39				0x11111111
#define DB_88F6781Y0_MPP40_47				0x11111111
#define DB_88F6781Y0_MPP48_55				0x11111111
#define DB_88F6781Y0_MPP56_63				0x00111111
#define DB_88F6781Y0_OE_LOW					(~((BIT2)|(BIT5)|(BIT7)|(BIT8)|(BIT10)|(BIT14)|(BIT15)|(BIT16)|(BIT18)|(BIT19)))
#define DB_88F6781Y0_OE_HIGH				(~((BIT30)|(BIT31)))
#define DB_88F6781Y0_OE_VAL_LOW				0x000509A4
#define DB_88F6781Y0_OE_VAL_HIGH			0x0

/* RD-88F6781-PLUG X0 */
#define RD_88F6781X0_PLUG_MPP0_7			0x00000000
#define RD_88F6781X0_PLUG_MPP8_15			0x00000000
#define RD_88F6781X0_PLUG_MPP16_23			0x00000000
/*
#define RD_88F6781X0_PLUG_OE_LOW			((BIT0 | BIT3 | BIT4 | BIT7 | BIT12))
#define RD_88F6781X0_PLUG_OE_HIGH			(0)
*/
#define RD_88F6781X0_PLUG_OE_LOW			(~(BIT1 | BIT2 | BIT5 | BIT6 | BIT8 | BIT10 | BIT13 | BIT14))
#define RD_88F6781X0_PLUG_OE_HIGH			(~(0))
#define RD_88F6781X0_PLUG_OE_VAL_LOW		0xffffffff
#define RD_88F6781X0_PLUG_OE_VAL_HIGH		0xffffffff

/* RD-88F6781 */
#define RD_88F6781_MPP0_7                   0x00000000
#define RD_88F6781_MPP8_15                  0x00002000
#define RD_88F6781_MPP16_23                 0x22220033
#define RD_88F6781_MPP24_31                 0x00000000
#define RD_88F6781_MPP32_39                 0x00000000
#define RD_88F6781_MPP40_47                 0x00000000
#define RD_88F6781_MPP48_55                 0x00000000
#define RD_88F6781_OE_LOW                   (~(BIT8))
#define RD_88F6781_OE_HIGH                  0xFFFFFFFF
#define RD_88F6781_OE_VAL_LOW               0x0
#define RD_88F6781_OE_VAL_HIGH              0x0

/* RD-88F6781_AVNG */
#define RD_88F6781_AVNG_MPP0_7              0x00000000
#define RD_88F6781_AVNG_MPP8_15             0x00000000
#define RD_88F6781_AVNG_MPP16_23            0x22220033
#define RD_88F6781_AVNG_MPP24_31            0x00000000
#define RD_88F6781_AVNG_MPP32_39            0x00000000
#define RD_88F6781_AVNG_MPP40_47            0x00000000
#define RD_88F6781_AVNG_MPP48_55            0x00000000
#define RD_88F6781_AVNG_OE_LOW              (~((BIT2)|(BIT5)|(BIT6)|(BIT18)))
#define RD_88F6781_AVNG_OE_HIGH             (~((BIT20)|(BIT23)|(BIT25)|(BIT30)|(BIT31)))
#define RD_88F6781_AVNG_OE_VAL_LOW          0x00040004
#define RD_88F6781_AVNG_OE_VAL_HIGH         0xC2900000

/* RD-88F6781_AVNG X0/Y0 */
#if defined(RD_88F6781X0_AVNG)
#define RD_88F6781Y0_AVNG_MPP0_7            0x00000000
#define RD_88F6781Y0_AVNG_MPP8_15           0x00040000
#define RD_88F6781Y0_AVNG_MPP16_23          0x01104440
#define RD_88F6781Y0_AVNG_MPP24_31          0x00000000
#define RD_88F6781Y0_AVNG_MPP32_39          0x00000000
#define RD_88F6781Y0_AVNG_MPP40_47          0x00000000
#define RD_88F6781Y0_AVNG_MPP48_55          0x00000000
#define RD_88F6781Y0_AVNG_OE_LOW            (~((BIT9)|(BIT11)|(BIT13)|(BIT16)|(BIT17)|(BIT20)))
#define RD_88F6781Y0_AVNG_OE_HIGH           0xFFFFFFFF
#define RD_88F6781Y0_AVNG_OE_VAL_LOW        0x00132A00
#define RD_88F6781Y0_AVNG_OE_VAL_HIGH       0x0
#else
#define RD_88F6781Y0_AVNG_MPP0_7            0x00000000
#define RD_88F6781Y0_AVNG_MPP8_15           0x00040000
#define RD_88F6781Y0_AVNG_MPP16_23          0x01100400
#define RD_88F6781Y0_AVNG_MPP24_31          0x00000000
#define RD_88F6781Y0_AVNG_MPP32_39          0x00000000
#define RD_88F6781Y0_AVNG_MPP40_47          0x00000000
#define RD_88F6781Y0_AVNG_MPP48_55          0x00000000
#define RD_88F6781Y0_AVNG_OE_LOW            (~((BIT6)|(BIT9)|(BIT11)|(BIT14)|(BIT16)|(BIT17)|(BIT20)))
#define RD_88F6781Y0_AVNG_OE_HIGH           0xFFFFFFFF
#define RD_88F6781Y0_AVNG_OE_VAL_LOW        0x00120A40
#define RD_88F6781Y0_AVNG_OE_VAL_HIGH       0x0
#endif
/* RD-88AP510_AVNG A0 V3.0 */

#define RD_88AP510A0_AVNG_MPP0_7            0x00000000
#define RD_88AP510A0_AVNG_MPP8_15           0x22032000
#define RD_88AP510A0_AVNG_MPP16_23          0x66664440
#define RD_88AP510A0_AVNG_MPP24_31          0x00000000
#define RD_88AP510A0_AVNG_MPP32_39          0x00000000
#define RD_88AP510A0_AVNG_MPP40_47          0x00000000
#define RD_88AP510A0_AVNG_MPP48_55          0x00000000
#define RD_88AP510A0_AVNG_OE_LOW            (~((BIT28)|(BIT26)|(BIT16)))
#define RD_88AP510A0_AVNG_OE_HIGH           (~((BIT24)|(BIT23)|(BIT21)|(BIT20)|(BIT7)|(BIT6)|(BIT1)))
#define RD_88AP510A0_AVNG_OE_VAL_LOW        0x00010184
#define RD_88AP510A0_AVNG_OE_VAL_HIGH       0x018000C0

/* DB-88F6781-BP B0 */
#define DB_88AP510BP_B_MPP0_7				0x00000000
#define DB_88AP510BP_B_MPP8_15				0x00442000
#define DB_88AP510BP_B_MPP16_23				0x22224043
#define DB_88AP510BP_B_MPP24_31				0x11111111
#define DB_88AP510BP_B_MPP32_39				0x11111111
#define DB_88AP510BP_B_MPP40_47				0x11111111
#define DB_88AP510BP_B_MPP48_55				0x11111111
#define DB_88AP510BP_B_MPP56_63				0x00111111
#define DB_88AP510BP_B_OE_LOW				(~((BIT1)|(BIT2)|(BIT5)|(BIT7)|(BIT8)|(BIT11)|(BIT14)|(BIT15)|(BIT16)|(BIT18)|(BIT19)))
#define DB_88AP510BP_B_OE_HIGH				0x0FFFFFFF
#define DB_88AP510BP_B_OE_VAL_LOW			((BIT1)|(BIT2)|(BIT5)|(BIT7)|(BIT8)|(BIT11)|(BIT16)|(BIT18)|(BIT19))
#define DB_88AP510BP_B_OE_VAL_HIGH			0x0

/* DB-88AP510-PCAC */
#define DB_88AP510_PCAC_MPP0_7				0x00000030
#define DB_88AP510_PCAC_MPP8_15				0x00000000
#define DB_88AP510_PCAC_MPP16_23			0x00004040
#define DB_88AP510_PCAC_MPP24_31			0x11111111
#define DB_88AP510_PCAC_MPP32_39			0x11111111
#define DB_88AP510_PCAC_MPP40_47			0x11111111
#define DB_88AP510_PCAC_MPP48_55			0x11111111
#define DB_88AP510_PCAC_MPP56_63			0x00111111
#define DB_88AP510_PCAC_OE_LOW				(~(BIT2 | BIT14 | BIT15 | BIT16))
#define DB_88AP510_PCAC_OE_HIGH				0xBFFFFFFF
#define DB_88AP510_PCAC_OE_VAL_LOW			((BIT2))
#define DB_88AP510_PCAC_OE_VAL_HIGH			0x0

#endif /* __INCmvBoardEnvSpech */
