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

#ifndef __INCmvDram
#define __INCmvDram

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_DIMM_NUM 			2
#define SPD_SIZE			128

/* Dimm spd offsets */
#define DIMM_MEM_TYPE 					2
#define DIMM_ROW_NUM 					3
#define DDR3_DIMM_ROW_NUM 				5
#define DDR3_DIMM_ROW_NUM_MIN 				12
#define DDR3_DIMM_ROW_NUM_OFF 				3
#define DDR3_DIMM_ROW_NUM_MASK 				(7 << DDR3_DIMM_ROW_NUM_OFF)
#define DIMM_COL_NUM 					4
#define DDR3_DIMM_DEV_DENSITY 				4
#define DDR3_DIMM_DEV_DENSITY_MASK 			0xf
#define DDR3_DIMM_COL_NUM 				5
#define DDR3_DIMM_COL_NUM_MIN 				9
#define DDR3_DIMM_COL_NUM_OFF 				0
#define DDR3_DIMM_COL_NUM_MASK 				(7 << DDR3_DIMM_COL_NUM_OFF)
#define DIMM_MODULE_BANK_NUM 				5
#define DDR3_DIMM_MODULE_ORG	 			7
#define DDR3_DIMM_MODULE_DEVICE_WIDTH_OFF 		0
#define DDR3_DIMM_MODULE_DEVICE_WIDTH_MASK		(7 << DDR3_DIMM_MODULE_DEVICE_WIDTH_OFF)
#define DDR3_DIMM_MODULE_BANK_NUM_MIN 			1
#define DDR3_DIMM_MODULE_BANK_NUM_OFF 			3
#define DDR3_DIMM_MODULE_BANK_NUM_MASK			(7 << DDR3_DIMM_MODULE_BANK_NUM_OFF)
#define DIMM_DATA_WIDTH 				6
#define DDR3_DIMM_DATA_WIDTH 				8
#define DDR3_DIMM_DATA_WIDTH_OFF 			0
#define DDR3_DIMM_DATA_WIDTH_MASK			(7 << DDR3_DIMM_DATA_WIDTH_OFF)
#define DDR3_DIMM_DATA_ECC_OFF 				3
#define DDR3_DIMM_DATA_ECC_MASK				(3 << DDR3_DIMM_DATA_ECC_OFF)

#define DIMM_VOLT_IF 					8
#define DIMM_MIN_CC_AT_MAX_CAS 				9
#define DDR3_DIMM_MTB_DIV 				11
#define DDR3_DIMM_MIN_CC 				12
#define DIMM_ERR_CHECK_TYPE 				11
#define DIMM_REFRESH_INTERVAL 				12
#define DIMM_SDRAM_WIDTH 				13
#define DDR3_DIMM_SDRAM_WIDTH 				13
#define DDR3_DIMM_SDRAM_WIDTH_OFF			0
#define DDR3_DIMM_SDRAM_WIDTH_MASK			(7 << DDR3_DIMM_SDRAM_WIDTH_OFF)
#define DIMM_ERR_CHECK_DATA_WIDTH 			14
#define DIMM_MIN_CLK_DEL 				15
#define DIMM_BURST_LEN_SUP 				16
#define DIMM_DEV_BANK_NUM 				17
#define DDR3_DIMM_DEV_BANK_NUM 				4
#define DDR3_DIMM_DEV_BANK_OFF 				4
#define DDR3_DIMM_DEV_BANK_MASK				(7 << DDR3_DIMM_DEV_BANK_OFF)
#define DIMM_SUP_CAL 					18
#define DDR3_DIMM_SUP_CAL_LSB 				14
#define DDR3_DIMM_SUP_CAL_MSB 				15
#define DIMM_DDR2_TYPE_INFORMATION          		20      /* DDR2 only */
#define DIMM_BUF_ADDR_CONT_IN 				21
#define DIMM_MIN_CC_AT_MAX_CAS_MINUS1			23
#define DIMM_MIN_CC_AT_MAX_CAS_MINUS2			25
#define DIMM_MIN_ROW_PRECHARGE_TIME			27
#define DDR3_DIMM_MIN_ROW_PRECHARGE_TIME		20
#define DIMM_MIN_ROW_ACTIVE_TO_ROW_ACTIVE		28
#define DDR3_DIMM_MIN_ROW_ACTIVE_TO_ROW_ACTIVE		19
#define DIMM_MIN_RAS_TO_CAS_DELAY			29
#define DDR3_DIMM_MIN_RAS_TO_CAS_DELAY			18
#define DIMM_MIN_RAS_PULSE_WIDTH			30
#define DDR3_DIMM_MIN_RAS_PULSE_WIDTH_LSB		22
#define DDR3_DIMM_MIN_RAS_PULSE_WIDTH_MSB		21
#define DDR3_DIMM_MIN_RAS_PULSE_WIDTH_MSB_MASK		0xf
#define DIMM_BANK_DENSITY				31
#define DIMM_MIN_WRITE_RECOVERY_TIME        		36
#define DDR3_DIMM_MIN_WRITE_RECOVERY_TIME        	17
#define DIMM_MIN_WRITE_TO_READ_CMD_DELAY    		37
#define DDR3_DIMM_MIN_WRITE_TO_READ_CMD_DELAY    	26
#define DIMM_MIN_READ_TO_PRECH_CMD_DELAY    		38
#define DDR3_DIMM_MIN_READ_TO_PRECH_CMD_DELAY    	27
#define DIMM_MIN_ACTIVATE_TO_ACTIVATE_CMD    		41
#define DDR3_DIMM_MIN_ACTIVATE_TO_ACTIVATE_CMD_MSB    	21
#define DDR3_DIMM_MIN_ACTIVATE_TO_ACTIVATE_CMD_LSB    	23
#define DIMM_MIN_REFRESH_TO_ACTIVATE_CMD    		42
#define DDR3_DIMM_MIN_REFRESH_TO_ACTIVATE_CMD_MSB    	25
#define DDR3_DIMM_MIN_REFRESH_TO_ACTIVATE_CMD_LSB    	24
#define DDR3_DIMM_MIN_FOUR_ACTIVATE_WIN_DELAY_MSB	28
#define DDR3_DIMM_MIN_FOUR_ACTIVATE_WIN_DELAY_MSB_MASK		0xf
#define DDR3_DIMM_MIN_FOUR_ACTIVATE_WIN_DELAY_LSB    	29
#define DIMM_SPD_VERSION    				62
#define DDR3_MIN_CAS_LAT_TIME  				16

/* Dimm Memory Type values */
#define DIMM_MEM_TYPE_SDRAM					0x4
#define DIMM_MEM_TYPE_DDR1 					0x7
#define DIMM_MEM_TYPE_DDR2 					0x8
#define DIMM_MEM_TYPE_DDR3 					0xB
        
#define DIMM_MODULE_MANU_OFFS 		64
#define DIMM_MODULE_MANU_SIZE 		8
#define DIMM_MODULE_VEN_OFFS 		73 
#define DIMM_MODULE_VEN_SIZE 		25
#define DIMM_MODULE_ID_OFFS 		99
#define DIMM_MODULE_ID_SIZE 		18

/* enumeration for voltage levels. */
typedef enum _mvDimmVoltageIf
{
    TTL_5V_TOLERANT, 
    LVTTL, 
    HSTL_1_5V, 
    SSTL_3_3V, 
    SSTL_2_5V, 
    VOLTAGE_UNKNOWN, 
} MV_DIMM_VOLTAGE_IF;


/* enumaration for SDRAM CAS Latencies. */
typedef enum _mvDimmSdramCas
{
    SD_CL_1 =1,  
    SD_CL_2,  
    SD_CL_3, 
    SD_CL_4, 
    SD_CL_5, 
    SD_CL_6, 
    SD_CL_7, 
    SD_FAULT
}MV_DIMM_SDRAM_CAS;

/* enumeration for memory types */
typedef enum _mvMemoryType
{
    MEM_TYPE_SDRAM,
    MEM_TYPE_DDR1,
    MEM_TYPE_DDR2,
    MEM_TYPE_DDR3
}MV_MEMORY_TYPE;

/* DIMM information structure */                                                    
typedef struct _mvDimmInfo
{
    MV_MEMORY_TYPE  memoryType; 	/* DDR or SDRAM */

    MV_U8       spdRawData[SPD_SIZE];  	/* Content of SPD-EEPROM copied 1:1  */

    /* DIMM dimensions */
    MV_U32  numOfRowAddr;
    MV_U32  numOfColAddr;
    MV_U32  numOfModuleBanks;
    MV_U32  dataWidth;
    MV_U32  errorCheckType;             /* ECC , PARITY..*/
    MV_U32  sdramWidth;                 /* 4,8,16 or 32 */
    MV_U32  errorCheckDataWidth;        /* 0 - no, 1 - Yes */
    MV_U32  burstLengthSupported;
    MV_U32  numOfBanksOnEachDevice;
    MV_U32  suportedCasLatencies;
    MV_U32  refreshInterval;
    MV_U32  dimmBankDensity;
    MV_U32  dimmTypeInfo;           /* DDR2 only */
    MV_U32  dimmAttributes;

    /* DIMM timing parameters */
    MV_U32  minCycleTimeAtMaxCasLatPs;	
    MV_U32  minCycleTimeAtMaxCasLatMinus1Ps;
    MV_U32  minCycleTimeAtMaxCasLatMinus2Ps;
	MV_U32  minRowPrechargeTime;
	MV_U32  minRowActiveToRowActive;
	MV_U32  minRasToCasDelay;
	MV_U32  minRasPulseWidth;
    MV_U32  minWriteRecoveryTime;   /* DDR3/2 only */
    MV_U32  minWriteToReadCmdDelay; /* DDR3/2 only */
    MV_U32  minReadToPrechCmdDelay; /* DDR3/2 only */
    MV_U32  minRefreshToActiveCmd;  /* DDR3/2 only */
    MV_U32  minActivateToActiveCmd;  /* DDR3/2 only */

    /* Parameters calculated from the extracted DIMM information */
    MV_U32  size;               /* 16,64,128,256 or 512 MByte in MB units */
    MV_U32  deviceDensity;      /* 16,64,128,256 or 512 Mbit in MB units  */
    MV_U32  numberOfDevices;
    /* Added for DDR3 */
    MV_U32  minCasLatTime;
    MV_U32  minFourActiveWinDelay;
} MV_DIMM_INFO;

typedef struct _mvDramBankInfo
{
    MV_MEMORY_TYPE  memoryType; 	/* DDR1, DDR2 or SDRAM */

    /* DIMM dimensions */
    MV_U32  numOfRowAddr;
    MV_U32  numOfColAddr;
    MV_U32  dataWidth;
    MV_U32  errorCheckType;             /* ECC , PARITY..*/
    MV_U32  sdramWidth;                 /* 4,8,16 or 32 */
    MV_U32  errorCheckDataWidth;        /* 0 - no, 1 - Yes */
    MV_U32  burstLengthSupported;
    MV_U32  numOfBanksOnEachDevice;
    MV_U32  suportedCasLatencies;
    MV_U32  refreshInterval;

    /* DIMM timing parameters */
    MV_U32  minCycleTimeAtMaxCasLatPs;	
    MV_U32  minCycleTimeAtMaxCasLatMinus1Ps;
    MV_U32  minCycleTimeAtMaxCasLatMinus2Ps;
    MV_U32  minRowPrechargeTime;
    MV_U32  minRowActiveToRowActive;
    MV_U32  minRasToCasDelay;
    MV_U32  minRasPulseWidth;
    MV_U32  minWriteRecoveryTime;   /* DDR3/2 only */
    MV_U32  minWriteToReadCmdDelay; /* DDR3/2 only */
    MV_U32  minReadToPrechCmdDelay; /* DDR3/2 only */
    MV_U32  minRefreshToActiveCmd;  /* DDR3/2 only */
    MV_U32  minActivateToActiveCmd;  /* DDR3/2 only */
                      
    /* Parameters calculated from the extracted DIMM information */
    MV_U32  size;
    MV_U32  deviceDensity;           	/* 16,64,128,256 or 512 Mbit */
    MV_U32  numberOfDevices;

    /* DIMM attributes (MV_TRUE for yes) */
    MV_BOOL registeredAddrAndControlInputs;

    /* Added for DDR3 */
    MV_U32  minCasLatTime;
    MV_U32  minFourActiveWinDelay;

}MV_DRAM_BANK_INFO;

MV_STATUS mvDramBankInfoGet(MV_U32 bankNum, MV_DRAM_BANK_INFO *pBankInfo);
MV_STATUS dimmSpdGet(MV_U32 dimmNum, MV_DIMM_INFO *pDimmInfo);
MV_VOID dimmSpdPrint(MV_U32 dimmNum, MV_U32 sysClock);
MV_STATUS dimmSpdCpy(MV_VOID);

#ifdef __cplusplus
}
#endif

#endif /* __INCmvDram */
