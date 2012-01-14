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

#include "mvCommon.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "cpu/mvCpu.h"
#include "cntmr/mvCntmr.h"
#include "gpp/mvGpp.h"
#include "gpp/mvGppRegs.h"
#include "twsi/mvTwsi.h"
#include "pci-if/mvPciIf.h"
#include "pci-if/pex/mvPex.h"
#include "device/mvDevice.h"
#include "ctrlEnv/sys/mvCpuIf.h"
#include "mvSysHwConfig.h"

/* defines  */
/* #define MV_DEBUG */
#ifdef MV_DEBUG
	#define DB(x)	x
#else
	#define DB(x)
#endif

extern MV_CPU_ARM_CLK _cpuARMDDRCLK[];

#define CODE_IN_ROM		MV_FALSE
#define CODE_IN_RAM		MV_TRUE

extern	MV_BOARD_INFO*	boardInfoTbl[];
#define BOARD_INFO(boardId)	boardInfoTbl[boardId - BOARD_ID_BASE]

/* Locals */
static MV_DEV_CS_INFO*  boardGetDevEntry(MV_32 devNum, MV_BOARD_DEV_CLASS devClass);

MV_U32 tClkRate   = -1;


/*******************************************************************************
* mvBoardEnvInit - Init board
*
* DESCRIPTION:
*		In this function the board environment take care of device bank
*		initialization.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_VOID mvBoardEnvInit(MV_VOID)
{
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE)&&(boardId < MV_MAX_BOARD_ID)))
	{
		mvOsPrintf("mvBoardEnvInit:Board unknown.\n");
		return;

	}

	/* Set GPP Out value */
	MV_REG_WRITE(GPP_DATA_OUT_REG(0), BOARD_INFO(boardId)->gppOutValLow);
	MV_REG_WRITE(GPP_DATA_OUT_REG(1), BOARD_INFO(boardId)->gppOutValHigh);
#ifndef CONFIG_DOVE_REV_Z0
	MV_REG_WRITE(GPP_DATA_OUT_REG(2), BOARD_INFO(boardId)->gpp2OutVal);
#endif

	/* set GPP polarity */
	mvGppPolaritySet(0, 0xFFFFFFFF, BOARD_INFO(boardId)->gppPolarityValLow);
	mvGppPolaritySet(1, 0xFFFFFFFF, BOARD_INFO(boardId)->gppPolarityValHigh);
#ifndef CONFIG_DOVE_REV_Z0
	mvGppPolaritySet(2, 0xFFFFFFFF, BOARD_INFO(boardId)->gpp2PolarityVal);
#endif

	/* Set GPP Out Enable*/
	mvGppTypeSet(0, 0xFFFFFFFF, BOARD_INFO(boardId)->gppOutEnValLow);
	mvGppTypeSet(1, 0xFFFFFFFF, BOARD_INFO(boardId)->gppOutEnValHigh);
#ifndef CONFIG_DOVE_REV_Z0
	mvGppTypeSet(2, 0xFFFFFFFF, BOARD_INFO(boardId)->gpp2OutEnVal);
#endif
}

/*******************************************************************************
* mvBoardModelGet - Get Board model
*
* DESCRIPTION:
*       This function returns 16bit describing board model.
*       Board model is constructed of one byte major and minor numbers in the
*       following manner:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       String describing board model.
*
*******************************************************************************/
MV_U16 mvBoardModelGet(MV_VOID)
{
	return (mvBoardIdGet() >> 16);
}

/*******************************************************************************
* mbBoardRevlGet - Get Board revision
*
* DESCRIPTION:
*       This function returns a 32bit describing the board revision.
*       Board revision is constructed of 4bytes. 2bytes describes major number
*       and the other 2bytes describes minor munber.
*       For example for board revision 3.4 the function will return
*       0x00030004.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       String describing board model.
*
*******************************************************************************/
MV_U16 mvBoardRevGet(MV_VOID)
{
	return (mvBoardIdGet() & 0xFFFF);
}

/*******************************************************************************
* mvBoardNameGet - Get Board name
*
* DESCRIPTION:
*       This function returns a string describing the board model and revision.
*       String is extracted from board I2C EEPROM.
*
* INPUT:
*       None.
*
* OUTPUT:
*       pNameBuff - Buffer to contain board name string. Minimum size 32 chars.
*
* RETURN:
*
*       MV_ERROR if informantion can not be read.
*******************************************************************************/
MV_STATUS mvBoardNameGet(char *pNameBuff)
{
	MV_U32 boardId= mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE)&&(boardId < MV_MAX_BOARD_ID)))
	{
		mvOsSPrintf (pNameBuff, "Board unknown.\n");
		return MV_ERROR;

	}

	mvOsSPrintf (pNameBuff, "%s",BOARD_INFO(boardId)->boardName);

	return MV_OK;
}

/*******************************************************************************
* mvBoardIsPortInSgmii -
*
* DESCRIPTION:
*       This routine returns MV_TRUE for port number works in SGMII or MV_FALSE
*	For all other options.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE - port in SGMII.
*       MV_FALSE - other.
*
*******************************************************************************/
MV_BOOL mvBoardIsPortInSgmii(MV_U32 ethPortNum)
{
    MV_BOOL ethPortSgmiiSupport[BOARD_ETH_PORT_NUM] = MV_ETH_PORT_SGMII;

    if(ethPortNum >= BOARD_ETH_PORT_NUM)
    {
	    mvOsPrintf ("Invalid portNo=%d\n", ethPortNum);
		return MV_FALSE;
    }
    return ethPortSgmiiSupport[ethPortNum];
}

/*******************************************************************************
* mvBoardPhyAddrGet - Get the phy address
*
* DESCRIPTION:
*       This routine returns the Phy address of a given ethernet port.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit describing Phy address, -1 if the port number is wrong.
*
*******************************************************************************/
MV_32 mvBoardPhyAddrGet(MV_U32 ethPortNum)
{
	MV_U32 boardId= mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE)&&(boardId < MV_MAX_BOARD_ID)))
	{
		mvOsPrintf("mvBoardPhyAddrGet: Board unknown.\n");
		return MV_ERROR;
	}

	return BOARD_INFO(boardId)->pBoardMacInfo[ethPortNum].boardEthSmiAddr;
}

/*******************************************************************************
* mvBoardMacSpeedGet - Get the Mac speed
*
* DESCRIPTION:
*       This routine returns the Mac speed if pre define of a given ethernet port.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BOARD_MAC_SPEED, -1 if the port number is wrong.
*
*******************************************************************************/
MV_BOARD_MAC_SPEED      mvBoardMacSpeedGet(MV_U32 ethPortNum)
{
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE)&&(boardId < MV_MAX_BOARD_ID)))
	{
		mvOsPrintf("mvBoardMacSpeedGet: Board unknown.\n");
		return MV_ERROR;
	}

	return BOARD_INFO(boardId)->pBoardMacInfo[ethPortNum].boardMacSpeed;
}

/*******************************************************************************
* mvBoardLinkStatusIrqGet - Get the IRQ number for the link status indication
*
* DESCRIPTION:
*       This routine returns the IRQ number for the link status indication.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       the number of the IRQ for the link status indication, -1 if the port 
*	number is wrong or if not relevant.
*
*******************************************************************************/
MV_32	mvBoardLinkStatusIrqGet(MV_U32 ethPortNum)
{
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE)&&(boardId < MV_MAX_BOARD_ID)))
	{
		mvOsPrintf("mvBoardLinkStatusIrqGet: Board unknown.\n");
		return MV_ERROR;
	}

	return BOARD_INFO(boardId)->pSwitchInfo[ethPortNum].linkStatusIrq;
}

/*******************************************************************************
* mvBoardSwitchPortGet - Get the mapping between the board connector and the 
* Ethernet Switch port
*
* DESCRIPTION:
*       This routine returns the matching Switch port.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*	boardPortNum - logical number of the connector on the board
*
* OUTPUT:
*       None.
*
* RETURN:
*       the matching Switch port, -1 if the port number is wrong or if not relevant.
*
*******************************************************************************/
MV_32	mvBoardSwitchPortGet(MV_U32 ethPortNum, MV_U8 boardPortNum)
{
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE)&&(boardId < MV_MAX_BOARD_ID)))
	{
		mvOsPrintf("mvBoardSwitchPortGet: Board unknown.\n");
		return MV_ERROR;
	}
	if (boardPortNum >= BOARD_ETH_SWITCH_PORT_NUM)
	{
		mvOsPrintf("mvBoardSwitchPortGet: Illegal board port number.\n");
		return MV_ERROR;
	}

	return BOARD_INFO(boardId)->pSwitchInfo[ethPortNum].qdPort[boardPortNum];
}

/*******************************************************************************
* mvBoardSwitchCpuPortGet - Get the the Ethernet Switch CPU port
*
* DESCRIPTION:
*       This routine returns the Switch CPU port.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       the Switch CPU port, -1 if the port number is wrong or if not relevant.
*
*******************************************************************************/
MV_32	mvBoardSwitchCpuPortGet(MV_U32 ethPortNum)
{
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE)&&(boardId < MV_MAX_BOARD_ID)))
	{
		mvOsPrintf("mvBoardSwitchCpuPortGet: Board unknown.\n");
		return MV_ERROR;
	}

	return BOARD_INFO(boardId)->pSwitchInfo[ethPortNum].qdCpuPort;
}

/*******************************************************************************
* mvBoardSmiScanModeGet - Get Switch SMI scan mode
*
* DESCRIPTION:
*       This routine returns Switch SMI scan mode.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       1 for SMI_MANUAL_MODE, -1 if the port number is wrong or if not relevant.
*
*******************************************************************************/
MV_32	mvBoardSmiScanModeGet(MV_U32 ethPortNum)
{
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE)&&(boardId < MV_MAX_BOARD_ID)))
	{
		mvOsPrintf("mvBoardSmiScanModeGet: Board unknown.\n");
		return MV_ERROR;
	}

	return BOARD_INFO(boardId)->pSwitchInfo[ethPortNum].smiScanMode;
}
/*******************************************************************************
* mvBoardSpecInitGet -
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN: Return MV_TRUE and parameters in case board need spesific phy init, 
* 	  otherwise return MV_FALSE. 
*
*
*******************************************************************************/

MV_BOOL mvBoardSpecInitGet(MV_U32* regOff, MV_U32* data)
{
	return MV_FALSE;
}

/*******************************************************************************
* mvBoardTclkGet - Get the board Tclk (Controller clock)
*
* DESCRIPTION:
*       This routine extract the controller core clock.
*       This function uses the controller counters to make identification.
*		Note: In order to avoid interference, make sure task context switch
*		and interrupts will not occure during this function operation
*
* INPUT:
*       countNum - Counter number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit clock cycles in Hertz.
*
*******************************************************************************/
MV_U32 mvBoardTclkGet(MV_VOID)
{
#if defined(TCLK_AUTO_DETECT)
	MV_U32 tmpTClkRate = MV_BOARD_TCLK_166MHZ;

        tmpTClkRate = MV_REG_READ(MPP_SAMPLE_AT_RESET_REG0);
        tmpTClkRate &= MSAR_TCLCK_MASK;

        switch (tmpTClkRate)
        {
        case MSAR_TCLCK_166:
                tmpTClkRate = MV_BOARD_TCLK_166MHZ;
                break;
	case MSAR_TCLCK_125:
                tmpTClkRate = MV_BOARD_TCLK_125MHZ;
                break;
        }
	return tmpTClkRate;
#else
	return MV_BOARD_DEFAULT_TCLK;
#endif

}
/*******************************************************************************
* mvBoardSysClkGet - Get the board SysClk (CPU bus clock)
*
* DESCRIPTION:
*       This routine extract the CPU bus clock.
*
* INPUT:
*       countNum - Counter number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit clock cycles in Hertz.
*
*******************************************************************************/
MV_U32  mvBoardSysClkGet(MV_VOID)
{
#ifdef SYSCLK_AUTO_DETECT
	MV_U32 sysClkRate, tmp, pClkRate, indexDdrRtio;
	MV_U32 cpuCLK[] = MV_CPU_CLCK_TBL;
	MV_U32 ddrRtio[][2] = MV_DDR_CLCK_RTIO_TBL;

	tmp = MV_REG_READ(MPP_SAMPLE_AT_RESET_REG0);
	pClkRate = tmp & MSAR_CPUCLCK_MASK;
	pClkRate = pClkRate >> MSAR_CPUCLCK_OFFS;
	pClkRate = cpuCLK[pClkRate];

	indexDdrRtio = tmp & MSAR_DDRCLCK_RTIO_MASK;
	indexDdrRtio = indexDdrRtio >> MSAR_DDRCLCK_RTIO_OFFS;

	sysClkRate = ((pClkRate * ddrRtio[indexDdrRtio][1]) / ddrRtio[indexDdrRtio][0]);

	return sysClkRate;
#else
	return MV_BOARD_DEFAULT_SYSCLK;
#endif
}


/*******************************************************************************
* mvBoardPexBridgeIntPinGet - Get PEX to PCI bridge interrupt pin number
*
* DESCRIPTION:
*		Multi-ported PCI Express bridges that is implemented on the board
*		collapse interrupts across multiple conventional PCI/PCI-X buses.
*		A dual-headed PCI Express bridge would map (or "swizzle") the
*		interrupts per the following table (in accordance with the respective
*		logical PCI/PCI-X bridge's Device Number), collapse the INTA#-INTD#
*		signals from its two logical PCI/PCI-X bridges, collapse the
*		INTA#-INTD# signals from any internal sources, and convert the
*		signals to in-band PCI Express messages. 10
*		This function returns the upstream interrupt as it was converted by
*		the bridge, according to board configuration and the following table:
*					  		PCI dev num
*			Interrupt pin 	7, 	8, 	9
*		   			A  ->	A	D	C
*		   			B  -> 	B	A	D
*		   			C  -> 	C	B	A
*		  			D  ->	D	C	B
*
*
* INPUT:
*       devNum - PCI/PCIX device number.
*       intPin - PCI Int pin
*
* OUTPUT:
*       None.
*
* RETURN:
*       Int pin connected to the Interrupt controller
*
*******************************************************************************/
MV_U32 mvBoardPexBridgeIntPinGet(MV_U32 devNum, MV_U32 intPin)
{
	MV_U32 realIntPin = ((intPin + (3 - (devNum % 4))) %4 );

	if (realIntPin == 0) return 4;
		else return realIntPin;

}

/*******************************************************************************
* mvBoardDebugLedNumGet - Get number of debug Leds
*
* DESCRIPTION:
* INPUT:
*       boardId
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_U32 mvBoardDebugLedNumGet(MV_U32 boardId)
{
	return BOARD_INFO(boardId)->activeLedsNumber;
}

/*******************************************************************************
* mvBoardDebugLed - Set the board debug Leds
*
* DESCRIPTION: turn on/off status leds.
*
* INPUT:
*       hexNum - Number to be displied in hex by Leds.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_VOID mvBoardDebugLed(MV_U32 hexNum)
{
	MV_U32 valLow = 0, valHigh = 0, totalMaskLow = 0, totalMaskHigh = 0, currentBitMask = 1, i = 0;
	MV_U32 boardId = mvBoardIdGet();

	hexNum &= (1 << BOARD_INFO(boardId)->activeLedsNumber) - 1;

	for (i = 0 ; i < BOARD_INFO(boardId)->activeLedsNumber ; i++) {
		if (BOARD_INFO(boardId)->pLedGppPin[i] <= 31)
		{
			if (hexNum & currentBitMask) {
				valLow |= (1 << BOARD_INFO(boardId)->pLedGppPin[i]);
			}
			totalMaskLow |= (1 << BOARD_INFO(boardId)->pLedGppPin[i]);
			currentBitMask = (currentBitMask << 1);
		} else {
			if (hexNum & currentBitMask) {
				valHigh |= (1 << (BOARD_INFO(boardId)->pLedGppPin[i] - 32));
			}
			totalMaskHigh |= (1 << (BOARD_INFO(boardId)->pLedGppPin[i] - 32));
			currentBitMask = (currentBitMask << 1);
        }
	}

	if (BOARD_INFO(boardId)->ledsPolarity) {
		mvGppValueSet(0, totalMaskLow, valLow);
		mvGppValueSet(1, totalMaskHigh, valHigh);
	} else {
		mvGppValueSet(0, totalMaskLow, ~valLow);
		mvGppValueSet(1, totalMaskHigh, ~valHigh);
	}
}


/*******************************************************************************
* mvBoarGpioPinGet - mvBoarGpioPinGet
*
* DESCRIPTION:
*
* INPUT:
*		class - MV_BOARD_GPP_CLASS enum.
*
* OUTPUT:
*		None.
*
* RETURN:
*       GPIO pin number. The function return -1 for bad parameters.
*
*******************************************************************************/
MV_32 mvBoarGpioPinNumGet(MV_BOARD_GPP_CLASS class, MV_U32 index)
{
	MV_U32 boardId, i;
	MV_U32 indexFound = 0;

	boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE)&&(boardId < MV_MAX_BOARD_ID)))
	{
		mvOsPrintf("mvBoardRTCGpioPinGet:Board unknown.\n");
		return MV_ERROR;

	}

        for (i = 0; i < BOARD_INFO(boardId)->numBoardGppInfo; i++)
		if (BOARD_INFO(boardId)->pBoardGppInfo[i].devClass == class) {
			if (indexFound == index)
        			return (MV_U32)BOARD_INFO(boardId)->pBoardGppInfo[i].gppPinNum;
			else
				indexFound++;

		}

	return MV_ERROR;
}


/*******************************************************************************
* mvBoardRTCGpioPinGet - mvBoardRTCGpioPinGet
*
* DESCRIPTION:
*
* INPUT:
*		None.
*
* OUTPUT:
*		None.
*
* RETURN:
*       GPIO pin number. The function return -1 for bad parameters.
*
*******************************************************************************/
MV_32 mvBoardRTCGpioPinGet(MV_VOID)
{
	return mvBoarGpioPinNumGet(BOARD_GPP_RTC, 0);
}


/*******************************************************************************
* mvBoardReset - mvBoardReset
*
* DESCRIPTION:
*			Reset the board
* INPUT:
*		None.
*
* OUTPUT:
*		None.
*
* RETURN:
*       None
*
*******************************************************************************/
MV_VOID	mvBoardReset(MV_VOID)
{
	MV_32 resetPin;

	/* Get gpp reset pin if define */
	resetPin = mvBoardResetGpioPinGet();
	if (resetPin != MV_ERROR)
	{
        	MV_REG_BIT_RESET( GPP_DATA_OUT_REG(0) ,(1 << resetPin));
		MV_REG_BIT_RESET( GPP_DATA_OUT_EN_REG(0) ,(1 << resetPin));

	}
	else
	{
	    /* No gpp reset pin was found, try to reset ussing
	    system reset out */
	    MV_REG_BIT_SET( CPU_RSTOUTN_MASK_REG , BIT2);
	    MV_REG_BIT_SET( CPU_SYS_SOFT_RST_REG , BIT0);
	}
	
	while(1);
}

/*******************************************************************************
* mvBoardResetGpioPinGet - mvBoardResetGpioPinGet
*
* DESCRIPTION:
*
* INPUT:
*		None.
*
* OUTPUT:
*		None.
*
* RETURN:
*       GPIO pin number. The function return -1 for bad parameters.
*
*******************************************************************************/
MV_32 mvBoardResetGpioPinGet(MV_VOID)
{
	return mvBoarGpioPinNumGet(BOARD_GPP_RESET, 0);
}
/*******************************************************************************
* mvBoardSDIOGpioPinGet - mvBoardSDIOGpioPinGet
*
* DESCRIPTION:
*	used for hotswap detection
* INPUT:
*		None.
*
* OUTPUT:
*		None.
*
* RETURN:
*       GPIO pin number. The function return -1 for bad parameters.
*
*******************************************************************************/
MV_32  mvBoardSDIOGpioPinGet(MV_VOID)
{
	return mvBoarGpioPinNumGet(BOARD_GPP_SDIO_DETECT, 0);
}

/*******************************************************************************
* mvBoardUSBVbusGpioPinGet - return Vbus input GPP
*
* DESCRIPTION:
*
* INPUT:
*		int  devNo.
*
* OUTPUT:
*		None.
*
* RETURN:
*       GPIO pin number. The function return -1 for bad parameters.
*
*******************************************************************************/
MV_32 mvBoardUSBVbusGpioPinGet(int devId)
{
	return mvBoarGpioPinNumGet(BOARD_GPP_USB_VBUS, devId);
}

/*******************************************************************************
* mvBoardUSBVbusEnGpioPinGet - return Vbus Enable output GPP
*
* DESCRIPTION:
*
* INPUT:
*		int  devNo.
*
* OUTPUT:
*		None.
*
* RETURN:
*       GPIO pin number. The function return -1 for bad parameters.
*
*******************************************************************************/
MV_32 mvBoardUSBVbusEnGpioPinGet(int devId)
{
	return mvBoarGpioPinNumGet(BOARD_GPP_USB_VBUS_EN, devId);
}


/*******************************************************************************
* mvBoardGpioIntMaskGet - Get GPIO mask for interrupt pins
*
* DESCRIPTION:
*		This function returns a 32-bit mask of GPP pins that connected to
*		interrupt generating sources on board.
*		For example if UART channel A is hardwired to GPP pin 8 and
*		UART channel B is hardwired to GPP pin 4 the fuinction will return
*		the value 0x000000110
*
* INPUT:
*		None.
*
* OUTPUT:
*		None.
*
* RETURN:
*		See description. The function return -1 if board is not identified.
*
*******************************************************************************/
MV_32 mvBoardGpioIntMaskLowGet(MV_VOID)
{
	MV_U32 boardId;

	boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE)&&(boardId < MV_MAX_BOARD_ID)))
	{
		mvOsPrintf("mvBoardGpioIntMaskGet:Board unknown.\n");
		return MV_ERROR;
	}

	return BOARD_INFO(boardId)->intsGppMaskLow;
}

MV_32 mvBoardGpioIntMaskHighGet(MV_VOID)
{
	MV_U32 boardId;

	boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE)&&(boardId < MV_MAX_BOARD_ID)))
	{
		mvOsPrintf("mvBoardGpioIntMaskGet:Board unknown.\n");
		return MV_ERROR;
	}

	return BOARD_INFO(boardId)->intsGppMaskHigh;
}


/*******************************************************************************
* mvBoardMppGet - Get board dependent MPP register value
*
* DESCRIPTION:
*		MPP settings are derived from board design.
*		MPP group consist of 8 MPPs. An MPP group represent MPP
*		control register.
*       This function retrieves board dependend MPP register value.
*
* INPUT:
*       mppGroupNum - MPP group number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit value describing MPP control register value.
*
*******************************************************************************/
MV_32 mvBoardMppGet(MV_U32 mppGroupNum)
{
	MV_U32 boardId;

	boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE)&&(boardId < MV_MAX_BOARD_ID)))
	{
		mvOsPrintf("mvBoardMppGet:Board unknown.\n");
		return MV_ERROR;
	}

	return BOARD_INFO(boardId)->pBoardMppConfigValue[0].mppGroup[mppGroupNum];
}


/*******************************************************************************
* mvBoardMppGroupId - If MPP group type is AUTO then identify it using twsi
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*
*******************************************************************************/
MV_VOID mvBoardMppGroupIdUpdate(MV_VOID)
{

	MV_BOARD_MPP_GROUP_CLASS devClass;
	MV_BOARD_MODULE_ID_CLASS devClassId;
	MV_BOARD_MPP_TYPE_CLASS mppGroupType;
	MV_U32 devId;
	MV_U32 maxMppGrp = 1;
	
	devId = mvCtrlModelGet();

	switch(devId){
		case MV_6781_DEV_ID:
			maxMppGrp = MV_6781_MPP_MAX_GROUP;
			break;
	}

	for (devClass = 0; devClass < maxMppGrp; devClass++)
	{
		/* If MPP group can be defined by the module connected to it */
		if (mvBoardMppGroupTypeGet(devClass) == MV_BOARD_AUTO)
		{
			/* Get MPP module ID */
			devClassId = mvBoarModuleTypeGet(devClass);
			if (MV_ERROR != devClassId)
			{
				switch(devClassId)
				{
				case MV_BOARD_MODULE_TDM_ID:
					mppGroupType = MV_BOARD_TDM;
					break;
				case MV_BOARD_MODULE_AUDIO_ID:
					mppGroupType = MV_BOARD_AUDIO;
					break;
				case MV_BOARD_MODULE_RGMII_ID:
					mppGroupType = MV_BOARD_RGMII;
					break;
				case MV_BOARD_MODULE_GMII_ID:
					mppGroupType = MV_BOARD_GMII;
					break;
				case MV_BOARD_MODULE_TS_ID:
					mppGroupType = MV_BOARD_TS;
					break;
				default:
					mppGroupType = MV_BOARD_OTHER;
					break;
				}
			}
			else
				/* The module bay is empty */
				mppGroupType = MV_BOARD_OTHER;
		
			/* Update MPP group type */
			mvBoardMppGroupTypeSet(devClass, mppGroupType);
		}
	}
}

/*******************************************************************************
* mvBoardMppGroupTypeGet 
*
* DESCRIPTION:
*
* INPUT:
*       mppGroupClass - MPP group number 0  for MPP[35:20] or 1 for MPP[49:36].
*
* OUTPUT:
*       None.
*
* RETURN:
*
*******************************************************************************/
MV_BOARD_MPP_TYPE_CLASS mvBoardMppGroupTypeGet(MV_BOARD_MPP_GROUP_CLASS mppGroupClass)
{
	MV_U32 boardId;

	boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE)&&(boardId < MV_MAX_BOARD_ID)))
	{
		mvOsPrintf("mvBoardMppGet:Board unknown.\n");
		return MV_ERROR;
	}
	
	if (mppGroupClass == MV_BOARD_MPP_GROUP_1)
		return BOARD_INFO(boardId)->pBoardMppTypeValue[0].boardMppGroup1;
	else
		return BOARD_INFO(boardId)->pBoardMppTypeValue[0].boardMppGroup2;
}

/*******************************************************************************
* mvBoardMppGroupTypeSet 
*
* DESCRIPTION:
*
* INPUT:
*       mppGroupClass - MPP group number 0  for MPP[35:20] or 1 for MPP[49:36].
*       mppGroupType - MPP group type for MPP[35:20] or for MPP[49:36].
*
* OUTPUT:
*       None.
*
* RETURN:
*
*******************************************************************************/
MV_VOID mvBoardMppGroupTypeSet(MV_BOARD_MPP_GROUP_CLASS mppGroupClass,
						MV_BOARD_MPP_TYPE_CLASS mppGroupType)
{
	MV_U32 boardId;

	boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE)&&(boardId < MV_MAX_BOARD_ID)))
	{
		mvOsPrintf("mvBoardMppGet:Board unknown.\n");
	}

	if (mppGroupClass == MV_BOARD_MPP_GROUP_1)
		BOARD_INFO(boardId)->pBoardMppTypeValue[0].boardMppGroup1 = mppGroupType;
	else
		BOARD_INFO(boardId)->pBoardMppTypeValue[0].boardMppGroup2 = mppGroupType;
}


/* Board devices API managments */

/*******************************************************************************
* mvBoardGetDeviceNumber - Get number of device of some type on the board
*
* DESCRIPTION:
*
* INPUT:
*		devType - The device type ( Flash,RTC , etc .. )
*
* OUTPUT:
*       None.
*
* RETURN:
*       If the device is found on the board the then the functions returns the
*		number of those devices else the function returns 0
*
*
*******************************************************************************/
MV_32 mvBoardGetDevicesNumber(MV_BOARD_DEV_CLASS devClass)
{
	MV_U32	foundIndex=0,devNum;
	MV_U32	boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE)&&(boardId < MV_MAX_BOARD_ID)))
	{
		mvOsPrintf("mvBoardGetDeviceNumber:Board unknown.\n");
		return 0xFFFFFFFF;
	}

	for (devNum = START_DEV_CS; devNum < BOARD_INFO(boardId)->numBoardDeviceIf; devNum++)
	{
		if (BOARD_INFO(boardId)->pDevCsInfo[devNum].devClass == devClass)
		{
			foundIndex++;
		}
	}

    	return foundIndex;

}

/*******************************************************************************
* mvBoardGetDeviceBaseAddr - Get base address of a device existing on the board
*
* DESCRIPTION:
*
* INPUT:
*       devIndex - The device sequential number on the board
*		devType - The device type ( Flash,RTC , etc .. )
*
* OUTPUT:
*       None.
*
* RETURN:
*       If the device is found on the board the then the functions returns the
*		Base address else the function returns 0xffffffff
*
*
*******************************************************************************/
MV_32 mvBoardGetDeviceBaseAddr(MV_32 devNum, MV_BOARD_DEV_CLASS devClass)
{
	MV_DEV_CS_INFO* devEntry;
	devEntry = boardGetDevEntry(devNum,devClass);
	if (devEntry != NULL)
	{
		return mvCpuIfTargetWinBaseLowGet(DEV_TO_TARGET(devEntry->deviceCS));

	}

	return 0xFFFFFFFF;
}

/*******************************************************************************
* mvBoardGetDeviceBusWidth - Get Bus width of a device existing on the board
*
* DESCRIPTION:
*
* INPUT:
*       devIndex - The device sequential number on the board
*		devType - The device type ( Flash,RTC , etc .. )
*
* OUTPUT:
*       None.
*
* RETURN:
*       If the device is found on the board the then the functions returns the
*		Bus width else the function returns 0xffffffff
*
*
*******************************************************************************/
MV_32 mvBoardGetDeviceBusWidth(MV_32 devNum, MV_BOARD_DEV_CLASS devClass)
{
	MV_DEV_CS_INFO* devEntry;

	devEntry = boardGetDevEntry(devNum,devClass);
	if (devEntry != NULL)
	{
		return 8; 
	}

	return 0xFFFFFFFF;

}

/*******************************************************************************
* mvBoardGetDeviceWidth - Get dev width of a device existing on the board
*
* DESCRIPTION:
*
* INPUT:
*       devIndex - The device sequential number on the board
*		devType - The device type ( Flash,RTC , etc .. )
*
* OUTPUT:
*       None.
*
* RETURN:
*       If the device is found on the board the then the functions returns the
*		dev width else the function returns 0xffffffff
*
*
*******************************************************************************/
MV_32 mvBoardGetDeviceWidth(MV_32 devNum, MV_BOARD_DEV_CLASS devClass)
{
	MV_DEV_CS_INFO* devEntry;
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE)&&(boardId < MV_MAX_BOARD_ID)))
	{
		mvOsPrintf("Board unknown.\n");
		return 0xFFFFFFFF;
	}

	devEntry = boardGetDevEntry(devNum,devClass);
	if (devEntry != NULL)
		return devEntry->devWidth;

	return MV_ERROR;
}

/*******************************************************************************
* mvBoardGetDeviceWinSize - Get the window size of a device existing on the board
*
* DESCRIPTION:
*
* INPUT:
*       devIndex - The device sequential number on the board
*		devType - The device type ( Flash,RTC , etc .. )
*
* OUTPUT:
*       None.
*
* RETURN:
*       If the device is found on the board the then the functions returns the
*		window size else the function returns 0xffffffff
*
*
*******************************************************************************/
MV_32 mvBoardGetDeviceWinSize(MV_32 devNum, MV_BOARD_DEV_CLASS devClass)
{
	MV_DEV_CS_INFO* devEntry;
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE)&&(boardId < MV_MAX_BOARD_ID)))
	{
		mvOsPrintf("Board unknown.\n");
		return 0xFFFFFFFF;
	}

	devEntry = boardGetDevEntry(devNum,devClass);
	if (devEntry != NULL)
	{
		return mvCpuIfTargetWinSizeGet(DEV_TO_TARGET(devEntry->deviceCS));
	}

	return 0xFFFFFFFF;
}


/*******************************************************************************
* boardGetDevEntry - returns the entry pointer of a device on the board
*
* DESCRIPTION:
*
* INPUT:
*       devIndex - The device sequential number on the board
*		devType - The device type ( Flash,RTC , etc .. )
*
* OUTPUT:
*       None.
*
* RETURN:
*       If the device is found on the board the then the functions returns the
*		dev number else the function returns 0x0
*
*
*******************************************************************************/
static MV_DEV_CS_INFO*  boardGetDevEntry(MV_32 devNum, MV_BOARD_DEV_CLASS devClass)
{
	MV_U32	foundIndex = 0, devIndex;
	MV_U32	boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE)&&(boardId < MV_MAX_BOARD_ID)))
	{
		mvOsPrintf("boardGetDevEntry: Board unknown.\n");
		return NULL;

	}

	for (devIndex = START_DEV_CS; devIndex < BOARD_INFO(boardId)->numBoardDeviceIf; devIndex++)
	{
		/* TBR */
		/*if (BOARD_INFO(boardId)->pDevCsInfo[devIndex].deviceCS == MV_BOOTDEVICE_INDEX)
		     continue;*/

		if (BOARD_INFO(boardId)->pDevCsInfo[devIndex].devClass == devClass)
		{
            		if (foundIndex == devNum)
			{
				return &(BOARD_INFO(boardId)->pDevCsInfo[devIndex]);
			}
			foundIndex++;
		}
	}

	/* device not found */
	return NULL;
}

/* Get device CS number */

MV_U32 boardGetDevCSNum(MV_32 devNum, MV_BOARD_DEV_CLASS devClass)
{
	MV_DEV_CS_INFO* devEntry;
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE)&&(boardId < MV_MAX_BOARD_ID)))
	{
		mvOsPrintf("Board unknown.\n");
		return 0xFFFFFFFF;

	}

	devEntry = boardGetDevEntry(devNum,devClass);
	if (devEntry != NULL)
		return devEntry->deviceCS;

	return 0xFFFFFFFF;

}



/*******************************************************************************
* mvBoardChanNumGet -
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*
*
*******************************************************************************/
MV_U8		mvBoardTwsiChanNumGet(MV_U8 unit, MV_BOARD_TWSI_CLASS twsiClass)
{
#warning "not yet implemented"
	return -1;
}


/*******************************************************************************
* mvBoardTwsiAddrTypeGet -
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*
*
*******************************************************************************/
MV_U8		mvBoardTwsiAddrTypeGet(MV_U8 unit, MV_BOARD_TWSI_CLASS twsiClass)
{
	int i;
	MV_U32 boardId = mvBoardIdGet();

	for (i = 0; i < BOARD_INFO(boardId)->numBoardTwsiDev; i++)
		if (BOARD_INFO(boardId)->pBoardTwsiDev[i].devClass == twsiClass)
			return BOARD_INFO(boardId)->pBoardTwsiDev[i].twsiDevAddrType;
	return (MV_ERROR);
}


/*******************************************************************************
* mvBoardTwsiAddrGet -
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*
*
*******************************************************************************/
MV_U8		mvBoardTwsiAddrGet(MV_U8 unit, MV_BOARD_TWSI_CLASS twsiClass)
{
	int i;
	MV_U32 boardId = mvBoardIdGet();

	for (i = 0; i < BOARD_INFO(boardId)->numBoardTwsiDev; i++)
		if (BOARD_INFO(boardId)->pBoardTwsiDev[i].devClass == twsiClass)
			return BOARD_INFO(boardId)->pBoardTwsiDev[i].twsiDevAddr;
	return (0xFF);
}



/*******************************************************************************
* mvBoardRtcTwsiAddrTypeGet -
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*
*
*******************************************************************************/
MV_U8 mvBoardRtcTwsiAddrTypeGet(void)
{
	return mvBoardTwsiAddrTypeGet(0, BOARD_TWSI_RTC);
}

/*******************************************************************************
* mvBoardRtcTwsiAddrGet -
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*
*
*******************************************************************************/
MV_U8 mvBoardRtcTwsiAddrGet(void)
{
	return mvBoardTwsiAddrGet(0, BOARD_TWSI_RTC);
}


/*******************************************************************************
* mvBoardA2DTwsiChanNumGet -
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*
*
*******************************************************************************/
MV_U8		mvBoardA2DTwsiChanNumGet(MV_U8 unit)
{
	return mvBoardTwsiChanNumGet(unit, BOARD_TWSI_AUDIO_DEC);
}

/*******************************************************************************
* mvBoardA2DTwsiAddrTypeGet -
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*
*
*******************************************************************************/
MV_U8 mvBoardA2DTwsiAddrTypeGet(MV_U8 unit)
{
	return mvBoardTwsiAddrTypeGet(unit, BOARD_TWSI_AUDIO_DEC);
}

/*******************************************************************************
* mvBoardA2DTwsiAddrGet -
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*
*
*******************************************************************************/
MV_U8 mvBoardA2DTwsiAddrGet(MV_U8 unit)
{
	return mvBoardTwsiAddrGet(unit, BOARD_TWSI_AUDIO_DEC);
}



/*******************************************************************************
* mvBoardCamTwsiChanNumGet -
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*
*
*******************************************************************************/
MV_U8		mvBoardCamTwsiChanNumGet(MV_U8 unit)
{
	return mvBoardTwsiChanNumGet(unit, BOARD_TWSI_CAM);
}



/*******************************************************************************
* mvBoardCamTwsiAddrTypeGet -
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*
*
*******************************************************************************/
MV_U8 mvBoardCamTwsiAddrTypeGet(MV_U8 unit)
{
	return mvBoardTwsiAddrTypeGet(unit, BOARD_TWSI_CAM);
}

/*******************************************************************************
* mvBoardCamTwsiAddrGet -
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*
*
*******************************************************************************/
MV_U8 mvBoardCamTwsiAddrGet(MV_U8 unit)
{
	return mvBoardTwsiAddrGet(unit, BOARD_TWSI_CAM);
}






/*******************************************************************************
* mvBoardTwsiExpAddrTypeGet -
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*
*
*******************************************************************************/
MV_U8 mvBoardTwsiExpAddrTypeGet(MV_U32 index)
{
	int i;
	MV_U32 indexFound = 0;
	MV_U32 boardId = mvBoardIdGet();

	for (i = 0; i < BOARD_INFO(boardId)->numBoardTwsiDev; i++)
		if (BOARD_INFO(boardId)->pBoardTwsiDev[i].devClass == BOARD_DEV_TWSI_EXP)
		{
			if (indexFound == index)
				return BOARD_INFO(boardId)->pBoardTwsiDev[i].twsiDevAddrType;
			else
				indexFound++;
		}

	return (MV_ERROR);
}

/*******************************************************************************
* mvBoardTwsiExpAddrGet -
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*
*
*******************************************************************************/
MV_U8 mvBoardTwsiExpAddrGet(MV_U32 index)
{
	int i;
	MV_U32 indexFound = 0;
	MV_U32 boardId = mvBoardIdGet();

	for (i = 0; i < BOARD_INFO(boardId)->numBoardTwsiDev; i++)
		if (BOARD_INFO(boardId)->pBoardTwsiDev[i].devClass == BOARD_DEV_TWSI_EXP)
		{
			if (indexFound == index)
				return BOARD_INFO(boardId)->pBoardTwsiDev[i].twsiDevAddr;
			else
				indexFound++;
		}

	return (0xFF);
}


/*******************************************************************************
* mvBoardTwsiSatRAddrTypeGet -
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*
*
*******************************************************************************/
MV_U8 mvBoardTwsiSatRAddrTypeGet(MV_U32 index)
{
	int i;
	MV_U32 indexFound = 0;
	MV_U32 boardId = mvBoardIdGet();

	for (i = 0; i < BOARD_INFO(boardId)->numBoardTwsiDev; i++)
		if (BOARD_INFO(boardId)->pBoardTwsiDev[i].devClass == BOARD_DEV_TWSI_SATR)
		{
			if (indexFound == index)
				return BOARD_INFO(boardId)->pBoardTwsiDev[i].twsiDevAddrType;
			else
				indexFound++;
		}

	return (MV_ERROR);
}

/*******************************************************************************
* mvBoardTwsiSatRAddrGet -
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*
*
*******************************************************************************/
MV_U8 mvBoardTwsiSatRAddrGet(MV_U32 index)
{
	int i;
	MV_U32 indexFound = 0;
	MV_U32 boardId = mvBoardIdGet();

	for (i = 0; i < BOARD_INFO(boardId)->numBoardTwsiDev; i++)
		if (BOARD_INFO(boardId)->pBoardTwsiDev[i].devClass == BOARD_DEV_TWSI_SATR)
		{
			if (indexFound == index)
				return BOARD_INFO(boardId)->pBoardTwsiDev[i].twsiDevAddr;
			else
				indexFound++;
		}

	return (0xFF);
}

/*******************************************************************************
* mvBoardNandWidthGet -
*
* DESCRIPTION: Get the width of the first NAND device in byte.
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN: 1, 2, 4 or MV_ERROR
*
*
*******************************************************************************/
MV_32 mvBoardNandWidthGet(void)
{
	MV_U32 devNum;
	MV_U32 devWidth;
	MV_U32 boardId = mvBoardIdGet();

	for (devNum = START_DEV_CS; devNum < BOARD_INFO(boardId)->numBoardDeviceIf; devNum++)
	{
		devWidth = mvBoardGetDeviceWidth(devNum, BOARD_DEV_NAND_FLASH);
		if (devWidth != MV_ERROR)
			return (devWidth / 8);
	}
		
	/* NAND wasn't found */
	return MV_ERROR;
}

MV_U32 gBoardId = -1;

/*******************************************************************************
* mvBoardIdGet - Get Board model
*
* DESCRIPTION:
*       This function returns board ID.
*       Board ID is 32bit word constructed of board model (16bit) and
*       board revision (16bit) in the following way: 0xMMMMRRRR.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit board ID number, '-1' if board is undefined.
*
*******************************************************************************/
MV_U32 mvBoardIdGet(MV_VOID)
{
	MV_U32 tmpBoardId = -1;

	if(gBoardId == -1)
        {
		#if defined(DB_88F6781Z0)
		tmpBoardId = DB_88F6781_BP_ID;
		#elif defined(RD_88F6781)
		tmpBoardId = RD_88F6781_ID;
		#elif defined(RD_88F6781_AVNG)
		tmpBoardId = RD_88F6781_AVNG_ID;
		#elif defined(DB_88F6781Y0) || defined(DB_88F6781X0)
		tmpBoardId =  DB_88F6781Y0_BP_ID;
		#elif defined(DB_88AP510BP_B)
		tmpBoardId =  DB_88AP510BP_B_BP_ID;
		#elif defined(DB_88AP510_PCAC)
		tmpBoardId =  DB_88AP510_PCAC_ID;
		#elif defined(RD_88F6781X0_PLUG)
		tmpBoardId = RD_88F6781X0_PLUG_ID;
		#elif defined(RD_88F6781Y0_AVNG) || defined(RD_88F6781X0_AVNG)
		tmpBoardId = RD_88F6781Y0_AVNG_ID;
		#elif defined(RD_88AP510A0_AVNG)
		tmpBoardId = RD_88AP510A0_AVNG_ID;
		#elif defined(CUBOX)
		tmpBoardId = RD_88AP510A0_CUBOX_ID;
		#endif
		gBoardId = tmpBoardId;
	}

	return gBoardId;
}


/*******************************************************************************
* mvBoarModuleTypeGet - mvBoarModuleTypeGet
*
* DESCRIPTION:
*
* INPUT:
*		group num - MV_BOARD_MPP_GROUP_CLASS enum
*
* OUTPUT:
*		None.
*
* RETURN:
*		module num - MV_BOARD_MODULE_CLASS enum
*
*******************************************************************************/
MV_BOARD_MODULE_ID_CLASS mvBoarModuleTypeGet(MV_BOARD_MPP_GROUP_CLASS devClass)
{
    	MV_TWSI_SLAVE twsiSlave;
	MV_TWSI_ADDR slave;
    	MV_U8 data;
	
	/* TWSI init */    	
	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(MV_BOARD_DIMM_I2C_CHANNEL, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);

	/* Read MPP module ID */
    	DB(mvOsPrintf("Board: Read MPP module ID\n"));
    	twsiSlave.slaveAddr.address = mvBoardTwsiExpAddrGet(devClass);
    	twsiSlave.slaveAddr.type = mvBoardTwsiExpAddrTypeGet(devClass);
    	twsiSlave.validOffset = MV_TRUE;
	/* Offset is the first command after the address which indicate the register number to be read 
	   in next operation */
    	twsiSlave.offset = 0;
    	twsiSlave.moreThen256 = MV_FALSE;

    	if( MV_OK != mvTwsiRead (MV_BOARD_DIMM_I2C_CHANNEL, &twsiSlave, &data, 1) )
    	{
    		DB(mvOsPrintf("Board: Read MPP module ID fail\n"));
        	return MV_ERROR;
    	}
    	DB(mvOsPrintf("Board: Read MPP module ID succeded\n"));
	
	return data;
}

/*******************************************************************************
* mvBoardSlicGpioPinGet -
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*
*
*******************************************************************************/
MV_32 mvBoardSlicGpioPinGet(MV_U32 slicNum)
{
	MV_U32 boardId;
	boardId = mvBoardIdGet();

	switch (boardId)
	{
	case DB_88F6781_BP_ID:
	case RD_88F6781_ID:
	case RD_88F6781_AVNG_ID:
	case DB_88F6781Y0_BP_ID:
	case DB_88AP510BP_B_BP_ID:
	case DB_88AP510_PCAC_ID:
	case RD_88F6781X0_PLUG_ID:
	case RD_88F6781Y0_AVNG_ID:
	default:
		return MV_ERROR;
		break;

	}
}


/*******************************************************************************
* mvBoardDdrTypeGet -
*
* DESCRIPTION: identify DDR DIMM type (DDR2/DDR3) based on sample at reset.
*
* INPUT: none.
*
* OUTPUT:
*       1 - DDR3, 0 - DDR2.
*
* RETURN:
*
*
*******************************************************************************/
MV_32 mvBoardDdrTypeGet(void)
{
	MV_U32 reg = MV_REG_READ(MPP_SAMPLE_AT_RESET_REG0);
	if (reg & BIT28)	/* bit 28 determines DDR type: 1 is DDR3, 0 is DDR2 */
		return 1;

	return 0;
}


/*******************************************************************************
* mvBoarTwsiSatRGet - 
*
* DESCRIPTION:
*
* INPUT:
*		device num - one of three devices
*		reg num - 0 or 1
*
* OUTPUT:
*		None.
*
* RETURN:
*		reg value
*
*******************************************************************************/
MV_U8 mvBoarTwsiSatRGet(MV_U8 devNum, MV_U8 regNum)
{
    	MV_TWSI_SLAVE twsiSlave;
	MV_TWSI_ADDR slave;
    	MV_U8 data;

	/* TWSI init */    	
	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);

    	/* Read MPP module ID */
    	DB(mvOsPrintf("Board: Read S@R device read\n"));
    	twsiSlave.slaveAddr.address = mvBoardTwsiSatRAddrGet(devNum);
    	twsiSlave.slaveAddr.type = mvBoardTwsiSatRAddrTypeGet(devNum);
    	twsiSlave.validOffset = MV_TRUE;
	/* Use offset as command */
    	twsiSlave.offset = regNum;
    	twsiSlave.moreThen256 = MV_FALSE;

    	if( MV_OK != mvTwsiRead (0, &twsiSlave, &data, 1) )
    	{
    		DB(mvOsPrintf("Board: Read S@R fail\n"));
        	return MV_ERROR;
    	}
    	DB(mvOsPrintf("Board: Read S@R succeded\n"));
	
	return data;
}

/*******************************************************************************
* mvBoarTwsiSatRSet - 
*
* DESCRIPTION:
*
* INPUT:
*		devNum - one of three devices
*		regNum - 0 or 1
*		regVal - value
*
*
* OUTPUT:
*		None.
*
* RETURN:
*		reg value
*
*******************************************************************************/
MV_STATUS mvBoarTwsiSatRSet(MV_U8 devNum, MV_U8 regNum, MV_U8 regVal)
{
    	MV_TWSI_SLAVE twsiSlave;
	MV_TWSI_ADDR slave;
	
	/* TWSI init */    	
	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);

    	/* Read MPP module ID */
    	twsiSlave.slaveAddr.address = mvBoardTwsiSatRAddrGet(devNum);
    	twsiSlave.slaveAddr.type = mvBoardTwsiSatRAddrTypeGet(devNum);
    	twsiSlave.validOffset = MV_TRUE;
    	DB(mvOsPrintf("Board: Write S@R device addr %x, type %x, data %x\n", twsiSlave.slaveAddr.address,\
								twsiSlave.slaveAddr.type, regVal));
	/* Use offset as command */
    	twsiSlave.offset = regNum;
    	twsiSlave.moreThen256 = MV_FALSE;
    	if( MV_OK != mvTwsiWrite (0, &twsiSlave, &regVal, 1) )
    	{
    		DB(mvOsPrintf("Board: Write S@R fail\n"));
        	return MV_ERROR;
    	}
    	DB(mvOsPrintf("Board: Write S@R succeded\n"));
	
	return MV_OK;
}



