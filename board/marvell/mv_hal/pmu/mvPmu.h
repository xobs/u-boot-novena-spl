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

#ifndef __INCMVpmuh
#define __INCMVpmuh

#ifdef __cplusplus
extern "C" {
#endif

#include "mvCommon.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "mvSysPmuConfig.h"

/********************/
/* SRAM definitions */
/********************/
#define PMU_SRAM_SIZE		0x800
#define PMU_SRAM_BASE		0xD00DC000

/***************************/
/* Enumeration Definitions */
/***************************/

/* PMU Signal source selector options */
typedef enum
{
	PMU_SIGNAL_NC = 0x0,
	PMU_SIGNAL_0 = 0x1,
	PMU_SIGNAL_1 = 0x2,
	PMU_SIGNAL_SDI = 0x3,
	PMU_SIGNAL_CPU_PWRDWN = 0x4,
	PMU_SIGNAL_SLP_PWRDWN = 0x5,
	PMU_SIGNAL_CPU_PWRGOOD = 0x8,
	PMU_SIGNAL_BAT_FAULT = 0xA,
	PMU_SIGNAL_EXT0_WKUP = 0xB,
	PMU_SIGNAL_EXT1_WKUP = 0xC,
	PMU_SIGNAL_EXT2_WKUP = 0xD,
	PMU_SIGNAL_BLINK = 0xE,
	/*
	* Following are virtiual values. The value set in the PMU signal selector is masked with 0xF
	*/
	PMU_SIGNAL_MRESET_OVRID = 0x10,	/* Converted to 0x0 ==> NC after masking - Pull-UP */
	PMU_SIGNAL_CKE_OVRID = 0x11	/* Converted to 0x1 ==> force '0' after masking  - CKE normal */

} MV_PMU_SIG_SRC;

/* CPU Clock Speed - DFS */
typedef enum
{
	CPU_CLOCK_TURBO = 0x0,
	CPU_CLOCK_SLOW = 0x1
} MV_PMU_CPU_SPEED;

/*************************/
/* Structure Definitions */
/*************************/

/* PMU init structure */
typedef struct 
{
	MV_BOOL     	L2LowLeakEn; 	/* enable/disable L2 retention enable in WFI */
	MV_BOOL		batFltMngDis;	/* Disable the handling of battery fault assertion */
	MV_BOOL     	exitOnBatFltDis;/* Disable resume in battery fault situation in Standby */
	MV_PMU_SIG_SRC	sigSelctor[16]; /* PMU signal selector of signal 0-15 */
	MV_U32		dvsDelay;	/* Delay for waiting the voltage change to complete */
	MV_32		ddrTermGpioNum; /* GPIO number used for DDR termination 0-31 or -1 */
	MV_U32		standbyPwrDelay;/* Delay needed to wait for power up in 32Khz CC */
} MV_PMU_INFO;

/* System Frequencies */
typedef struct
{
	MV_U32		cpuFreq;
	MV_U32		axiFreq;
	MV_U32		l2Freq;
	MV_U32		ddrFreq;
	MV_U32		pllFreq;
} MV_PMU_FREQ_INFO;

/* Temp sensor configuration structure */
typedef struct
{
	MV_U32		overheatDelay;	/* Time to wait before declaring an overheat event */
	MV_U32		coolingDelay;	/* Time to wait before declaring a normal temperature */
	MV_BOOL		powerDown;	/* Power down the temperature sensor */
	MV_U8		tcTrip;		/* Temperature curves of the band-gap circuit */
	MV_U8		selVcal;	/* Selects the high level of referance for calibration */
	MV_BOOL		vbeBypass;	/* Dynamic range of sensor reduced by half */
	MV_BOOL		selRefIcc;	/* Select referance current source from ICC */
	MV_U8		aTest;		/* */
	MV_U8		refCalCount;	/* Referance Value for calibration */
	MV_U8		selCalCapInt;	/* Select timing internal capacitor */
	MV_U8		averageNumber;	/* Average No/2/4/8/16/32 */
	MV_BOOL		selDblSlope;	/* Helps in reducing offsets in the sensor */
	MV_BOOL		otfCal;		/* Enable on the fly calibration */
	MV_BOOL		sleepEnable;	/* Put the sensor in sleep mode */
	MV_U8		initCalibVal;	/* Initial value for Diode Calibration */
} MV_TEMP_SNSR_INFO;


/* PMU Wakeup Events */
#define PMU_WAKE_ON_NONE       0x00
#define PMU_WAKE_ON_RTC        0x01
#define PMU_WAKE_ON_EXT0_RISE  0x02
#define PMU_WAKE_ON_EXT0_FALL  0x04
#define PMU_WAKE_ON_EXT1_RISE  0x08
#define PMU_WAKE_ON_EXT1_FALL  0x10
#define PMU_WAKE_ON_EXT2_RISE  0x20
#define PMU_WAKE_ON_EXT3_FALL  0x40
#define PMU_WAKE_ON_BAT_FAIL   0x80

/* Unit Clock Gate Mask */
#define PMU_CLOCK_GATE_USB0    0x1
#define PMU_CLOCK_GATE_USB1    0x2
#define PMU_CLOCK_GATE_GBE     0x4
#define PMU_CLOCK_GATE_SATA    0x8
#define PMU_CLOCK_GATE_PEX0    0x10
#define PMU_CLOCK_GATE_PEX1    0x20
#define PMU_CLOCK_GATE_SDI0    0x100
#define PMU_CLOCK_GATE_SDI1    0x200
#define PMU_CLOCK_GATE_NAND    0x400
#define PMU_CLOCK_GATE_CAM     0x800
#define PMU_CLOCK_GATE_AUD0    0x1000
#define PMU_CLOCK_GATE_AUD1    0x2000
#define PMU_CLOCK_GATE_AC97    0x20000
#define PMU_CLOCK_GATE_PDMA    0x40000

/* Function Prototypes */
MV_STATUS mvPmuInit 		(MV_PMU_INFO * pPmu);
MV_STATUS mvPmuDeepIdle		(MV_BOOL lcdRefresh);
MV_STATUS mvPmuStandby		(MV_VOID);
MV_STATUS mvPmuWakeupEventSet 	(MV_U32 wkupEvents);
MV_STATUS mvPmuWakeupEventGet	(MV_U32 * wkupEvents);
MV_STATUS mvPmuCpuFreqScale 	(MV_PMU_CPU_SPEED cpuSpeed);
MV_STATUS mvPmuSysFreqScale 	(MV_U32 ddrFreq, MV_U32 l2Freq, MV_U32 cpuFreq);
MV_STATUS mvPmuVoltageScale 	(MV_U32 pSet, MV_U32 vSet, MV_U32 rAddr, MV_U32 sAddr);
MV_STATUS mvPmuClockGateSet 	(MV_U32 unitMask, MV_BOOL gateStatus);
MV_STATUS mvPmuClockGateGet 	(MV_BOOL * gateStatus);
MV_STATUS mvPmuTempThresholdSet (MV_U32 coolThr, MV_U32 overheatThr, MV_BOOL thrMngrEn);
MV_STATUS mvPmuTempThresholdGet (MV_U32 * coolThr, MV_U32 * overheatThr, MV_BOOL * thrMngrEn);
MV_STATUS mvPmuTempGet 		(MV_U32 *temp);
MV_STATUS mvPmuTempSensorCfgSet (MV_TEMP_SNSR_INFO tempInfo);
MV_STATUS mvPmuTempSensorCalib  (MV_VOID);
MV_STATUS mvPmuGpuPowerDown	(MV_BOOL pwrStat);
MV_STATUS mvPmuVpuPowerDown	(MV_BOOL pwrStat);
MV_STATUS mvPmuGetCurrentFreq	(MV_PMU_FREQ_INFO * freqs);
MV_STATUS mvPmuCpuSetOP 	(MV_PMU_CPU_SPEED cpuSpeed, MV_BOOL dvsEnable);
#ifndef CONFIG_DOVE_REV_Z0
MV_VOID   mvPmuCpuIdleThresholdsSet	(MV_U32 hiThreshold, MV_U32 lowThreshold);
MV_VOID   mvPmuCpuIdleTimeBaseValueSet	(MV_U32 timeBase);
MV_VOID   mvPmuCpuIdleIntMaskSet	(MV_BOOL hiIntEnable, MV_BOOL lowIntEnable);
MV_U32 	  mvPmuCpuIdleTimeGet		(void);
MV_VOID   mvPmuCpuIdleIntStatGet	(MV_BOOL *hiIntStat, MV_BOOL *lowIntStat);
MV_VOID   mvPmuMcIdleThresholdsSet	(MV_U32 hiThreshold, MV_U32 lowThreshold);
MV_VOID   mvPmuMcIdleTimeBaseValueSet	(MV_U32 timeBase);
MV_VOID   mvPmuMcIdleIntMaskSet		(MV_BOOL hiIntEnable, MV_BOOL lowIntEnable);
MV_U32 	  mvPmuMcIdleTimeGet		(void);
MV_VOID   mvPmuMcIdleIntStatGet		(MV_BOOL *hiIntStat, MV_BOOL *lowIntStat);
#endif

#ifdef __cplusplus
}
#endif


#endif /* __INCMVpmuh */
