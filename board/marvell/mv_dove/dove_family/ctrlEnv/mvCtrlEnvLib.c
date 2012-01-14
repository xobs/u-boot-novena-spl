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
#include "mvCtrlEnvLib.h"
#include "ctrlEnv/sys/mvCpuIf.h"
#include "gpp/mvGpp.h"

#if defined(MV_INCLUDE_PEX)
#include "pci-if/mvPciIf.h"
#include "pci-if/pex/mvPex.h"
#include "pci-if/pex/mvPexRegs.h"
#include "mvSysPexApi.h"
#endif

#if defined(MV_INCLUDE_GIG_ETH)
#include "mvSysEthApi.h"
#endif

#if defined(MV_INCLUDE_XOR)
#include "mvSysXorApi.h"
#endif

#if defined(MV_INCLUDE_SATA)
#include "mvSysSataApi.h"
#endif

#if defined(MV_INCLUDE_USB)
#include "mvSysUsbApi.h"
#endif

#if defined(MV_INCLUDE_AUDIO)
#include "mvSysAudioApi.h"
#endif

#if defined(MV_INCLUDE_PDMA)
#include "mvSysPdmaApi.h"
#endif

#if defined(MV_INCLUDE_TWSI)
#include "twsi/soc/mvTwsiSocSpec.h"
#endif

/* defines  */
#ifdef MV_DEBUG
	#define DB(x)	x
#else
	#define DB(x)
#endif	


/*******************************************************************************
* mvCtrlEnvInit - Initialize Marvell controller environment.
*
* DESCRIPTION:
*       This function get environment information and initialize controller
*       internal/external environment. For example
*       1) MPP settings according to board MPP macros.
*		NOTE: It is the user responsibility to shut down all DMA channels
*		in device and disable controller sub units interrupts during 
*		boot process.
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
MV_STATUS mvCtrlEnvInit(MV_VOID)
{
    	MV_U32 mppGroup;
	MV_U32 reg = 0;

	/* Read MPP group from board level and assign to MPP register */
	for (mppGroup = 0; mppGroup <= MV_6781_MPP_MAX_GROUP; mppGroup++)
	{
		MV_REG_WRITE(mvCtrlMppRegGet(mppGroup), mvBoardMppGet(mppGroup));
	}
	/* Initialize MPPs from High GPIO */
#if defined(RD_88F6781_AVNG)
	mvGppFunctionSelectSet(0x18);
#elif defined(DB_88F6781Y0) || defined(DB_88F6781X0) || defined(DB_88AP510BP_B) || defined(DB_88AP510_PCAC)
	mvGppFunctionSelectSet(0x10);	/* Note: See also Table 6 in Spec if we want to set SSP */
	reg = MV_REG_READ(MPP_GENERAL_CONFIG_REG);
	reg &= ~(BIT0 | BIT1);		/* NF_IO[15:8] dedicated for NAND */
	MV_REG_WRITE(MPP_GENERAL_CONFIG_REG, reg);
#elif defined(RD_88F6781Y0_AVNG) || defined(RD_88F6781X0_AVNG) || defined(RD_88F6781X0_PLUG)
	mvGppFunctionSelectSet(0x0);	/* SPI, UART1, AU1, CAM, SD1 and SD0 as functional pads */
	reg = MV_REG_READ(MPP_GENERAL_CONFIG_REG);
	reg &= ~(BIT0 | BIT1);		/* NF_IO[15:8] dedicated for NAND */
	MV_REG_WRITE(MPP_GENERAL_CONFIG_REG, reg);
#else
	mvGppFunctionSelectSet(0);
#endif
#if defined(RD_88AP510A0_AVNG)
	mvGppFunctionSelectSet(0x0c); /* SPI, UART1,SD1,SD0 as function pads *//*AU1, CAM as GPIO pads */	
	reg = MV_REG_READ(MPP_GENERAL_CONFIG_REG);
	reg &= ~(BIT0 | BIT1);		/* NF_IO[15:8] dedicated for NAND */
	MV_REG_WRITE(MPP_GENERAL_CONFIG_REG, reg);					

#endif
	/* Identify MPPs group */
	mvBoardMppGroupIdUpdate();

#ifndef CONFIG_DOVE_REV_Z0
	/* Set VDDO_CAM LSB to '1' for 1.8V */
	MV_REG_BIT_SET(CPU_LCD_TWSI_CONFIG_2_REG, BIT0); 
#endif

	return MV_OK;
}

/*******************************************************************************
* mvCtrlMppRegGet - return reg address of mpp group
*
* DESCRIPTION:
*
* INPUT:
*       mppGroup - MPP group.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_U32 - Register address.
*
*******************************************************************************/
MV_U32 mvCtrlMppRegGet(MV_U32 mppGroup)
{
        MV_U32 ret;

        switch(mppGroup){
                case (0):       ret = MPP_CONTROL_REG0;
                                break;
                case (1):       ret = MPP_CONTROL_REG1;
                                break;
                case (2):       ret = MPP_CONTROL_REG2;
                                break;
                case (3):       ret = MPP_CONTROL_REG3;
                                break;
                default:        ret = MPP_CONTROL_REG0;
                                break;
        }
        return ret;
}
#if defined(MV_INCLUDE_PEX) 
/*******************************************************************************
* mvCtrlPexMaxIfGet - Get Marvell controller number of PEX interfaces.
*
* DESCRIPTION:
*       This function returns Marvell controller number of PEX interfaces.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Marvell controller number of PEX interfaces. If controller 
*		ID is undefined the function returns '0'.
*
*******************************************************************************/
MV_U32 mvCtrlPexMaxIfGet(MV_VOID)
{
	return MV_PEX_MAX_IF;
}
#endif

#if defined(MV_INCLUDE_GIG_ETH)
/*******************************************************************************
* mvCtrlEthMaxPortGet - Get Marvell controller number of etherent ports.
*
* DESCRIPTION:
*       This function returns Marvell controller number of etherent port.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Marvell controller number of etherent port.
*
*******************************************************************************/
MV_U32 mvCtrlEthMaxPortGet(MV_VOID)
{
	return MV_ETH_MAX_PORTS;
}
#endif

#if defined(MV_INCLUDE_XOR)
/*******************************************************************************
* mvCtrlXorMaxChanGet - Get Marvell controller number of XOR channels.
*
* DESCRIPTION:
*       This function returns Marvell controller number of XOR channels.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Marvell controller number of XOR channels.
*
*******************************************************************************/
MV_U32 mvCtrlXorMaxChanGet(MV_VOID)
{
	return MV_XOR_MAX_CHAN; 
}
#endif

#if defined(MV_INCLUDE_USB)
/*******************************************************************************
* mvCtrlUsbHostMaxGet - Get number of Marvell Usb  controllers
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       returns number of Marvell USB  controllers.
*
*******************************************************************************/
MV_U32 mvCtrlUsbMaxGet(void)
{
	return MV_USB_MAX_PORTS;
}
#endif

#if defined(MV_INCLUDE_NAND)
/*******************************************************************************
* mvCtrlNandSupport - Return if this controller has integrated NAND flash support
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if NAND is supported and MV_FALSE otherwise
*
*******************************************************************************/
MV_U32	  mvCtrlNandSupport(MV_VOID)
{
	return MV_6781_NAND;
}
#endif

#if defined(MV_INCLUDE_SDIO)
/*******************************************************************************
* mvCtrlSdioSupport - Return if this controller has integrated SDIO flash support
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if SDIO is supported and MV_FALSE otherwise
*
*******************************************************************************/
MV_U32	  mvCtrlSdioSupport(MV_VOID)
{
	return MV_6781_SDIO;
}
#endif

#if defined(MV_INCLUDE_AUDIO)
/*******************************************************************************
* mvCtrlAudioSupport - Return if this controller has integrated AUDIO flash support
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if AUDIO is supported and MV_FALSE otherwise
*
*******************************************************************************/
MV_U32	  mvCtrlAudioSupport(MV_VOID)
{
	return MV_6781_AUDIO;
}
#endif

#if defined(MV_INCLUDE_AUDIO)
/*******************************************************************************
* mvCtrlTdmSupport - Return if this controller has integrated TDM flash support
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if TDM is supported and MV_FALSE otherwise
*
*******************************************************************************/
MV_U32	  mvCtrlTdmSupport(MV_VOID)
{
	return MV_6781_TDM;
}
#endif

/*******************************************************************************
* mvCtrlModelGet - Get Marvell controller device model (ID)
*
* DESCRIPTION:
*       This function returns 16bit describing the device model (ID) as defined
*       in PCI Device and Vendor ID configuration register offset 0x0.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       16bit desscribing Marvell controller ID 
*
*******************************************************************************/
MV_U16 mvCtrlModelGet(MV_VOID)
{
	MV_U32 devId = 0;
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
	/* Check pex power state */
	MV_U32 pexPower;
	pexPower = mvCtrlPwrClckGet(PEX_UNIT_ID,0);
	if (pexPower == MV_FALSE)
		mvCtrlPwrClckSet(PEX_UNIT_ID, 0, MV_TRUE);
#endif
	devId = MV_REG_READ(PEX_CFG_DIRECT_ACCESS(0, PEX_DEVICE_AND_VENDOR_ID));
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
	/* Return to power off state */
	if (pexPower == MV_FALSE)
		mvCtrlPwrClckSet(PEX_UNIT_ID, 0, MV_FALSE);
#endif
	return ((devId & PXDAVI_DEV_ID_MASK) >> PXDAVI_DEV_ID_OFFS);
}
/*******************************************************************************
* mvCtrlRevGet - Get Marvell controller device revision number
*
* DESCRIPTION:
*       This function returns 8bit describing the device revision as defined
*       in PCI Express Class Code and Revision ID Register.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       8bit desscribing Marvell controller revision number 
*
*******************************************************************************/
MV_U8 mvCtrlRevGet(MV_VOID)
{
	MV_U8 revNum;
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
	/* Check pex power state */
	MV_U32 pexPower;
	pexPower = mvCtrlPwrClckGet(PEX_UNIT_ID,0);
	if (pexPower == MV_FALSE)
		mvCtrlPwrClckSet(PEX_UNIT_ID, 0, MV_TRUE);
#endif
	revNum = (MV_U8)MV_REG_READ(PEX_CFG_DIRECT_ACCESS(0, PCI_CLASS_CODE_AND_REVISION_ID));
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
	/* Return to power off state */
	if (pexPower == MV_FALSE)
		mvCtrlPwrClckSet(PEX_UNIT_ID, 0, MV_FALSE);
#endif
	return ((revNum & PCCRIR_REVID_MASK) >> PCCRIR_REVID_OFFS);
}

/*******************************************************************************
* mvCtrlNameGet - Get Marvell controller name
*
* DESCRIPTION:
*       This function returns a string describing the device model and revision.
*
* INPUT:
*       None.
*
* OUTPUT:
*       pNameBuff - Buffer to contain device name string. Minimum size 30 chars.
*
* RETURN:
*       
*       MV_ERROR if informantion can not be read.
*******************************************************************************/
MV_STATUS mvCtrlNameGet(char *pNameBuff)
{
	mvOsSPrintf (pNameBuff, "%s%x Rev %d", SOC_NAME_PREFIX, 
				mvCtrlModelGet(), mvCtrlRevGet()); 
	
	return MV_OK;
}

/*******************************************************************************
* mvCtrlModelRevGet - Get Controller Model (Device ID) and Revision
*
* DESCRIPTION:
*       This function returns 32bit value describing both Device ID and Revision
*       as defined in PCI Express Device and Vendor ID Register and device revision
*	    as defined in PCI Express Class Code and Revision ID Register.
     
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit describing both controller device ID and revision number
*
*******************************************************************************/
MV_U32	mvCtrlModelRevGet(MV_VOID)
{
	return ((mvCtrlModelGet() << 16) | mvCtrlRevGet());
}

/*******************************************************************************
* mvCtrlModelRevNameGet - Get Marvell controller name
*
* DESCRIPTION:
*       This function returns a string describing the device model and revision.
*
* INPUT:
*       None.
*
* OUTPUT:
*       pNameBuff - Buffer to contain device name string. Minimum size 30 chars.
*
* RETURN:
*       
*       MV_ERROR if informantion can not be read.
*******************************************************************************/

MV_STATUS mvCtrlModelRevNameGet(char *pNameBuff)
{
	switch (mvCtrlModelRevGet()) {
		case MV_6781_Z0_ID:
			mvOsSPrintf (pNameBuff, "%s",MV_6781_Z0_NAME);
			break;
		case MV_6781_Z1_ID:
			mvOsSPrintf (pNameBuff, "%s",MV_6781_Z1_NAME);
			break;
		case MV_6781_Y0_ID:
			mvOsSPrintf (pNameBuff, "%s",MV_6781_Y0_NAME);
			break;
		case MV_6781_Y1_ID:
			mvOsSPrintf (pNameBuff, "%s",MV_6781_Y1_NAME);
			break;
		case MV_6781_X0_ID:
			mvOsSPrintf (pNameBuff, "%s",MV_6781_X0_NAME);
			break;
		case MV_6781_A0_ID:
			mvOsSPrintf (pNameBuff, "%s",MV_6781_A0_NAME);
			break;
		case MV_6781_A1_ID:
			mvOsSPrintf (pNameBuff, "%s",MV_6781_A1_NAME);
			break;

		default:
			mvCtrlNameGet(pNameBuff);
			break;
	}
	return MV_OK;
}


/*******************************************************************************
* ctrlWinOverlapTest - Test address windows for overlaping.
*
* DESCRIPTION:
*       This function checks the given two address windows for overlaping.
*
* INPUT:
*       pAddrWin1 - Address window 1.
*       pAddrWin2 - Address window 2.
*
* OUTPUT:
*       None.
*
* RETURN:
*       
*       MV_TRUE if address window overlaps, MV_FALSE otherwise.
*******************************************************************************/
MV_STATUS ctrlWinOverlapTest(MV_ADDR_WIN *pAddrWin1, MV_ADDR_WIN *pAddrWin2)
{
    MV_U32 winBase1, winBase2;
    MV_U32 winTop1, winTop2;
    
    /* check if we have overflow than 4G*/
    if (((0xffffffff - pAddrWin1->baseLow) < pAddrWin1->size-1)||
       ((0xffffffff - pAddrWin2->baseLow) < pAddrWin2->size-1))
    {
	return MV_TRUE;
    }

    winBase1 = pAddrWin1->baseLow;
    winBase2 = pAddrWin2->baseLow;
    winTop1  = winBase1 + pAddrWin1->size-1;
    winTop2  = winBase2 + pAddrWin2->size-1;

    
    if (((winBase1 <= winTop2 ) && ( winTop2 <= winTop1)) ||
        ((winBase1 <= winBase2) && (winBase2 <= winTop1)))
    {
        return MV_TRUE;
    }
    else
    {
        return MV_FALSE;
    }
}

/*******************************************************************************
* ctrlWinWithinWinTest - Test address windows for overlaping.
*
* DESCRIPTION:
*       This function checks the given win1 boundries is within
*		win2 boundries.
*
* INPUT:
*       pAddrWin1 - Address window 1.
*       pAddrWin2 - Address window 2.
*
* OUTPUT:
*       None.
*
* RETURN:
*       
*       MV_TRUE if found win1 inside win2, MV_FALSE otherwise.
*******************************************************************************/
MV_STATUS ctrlWinWithinWinTest(MV_ADDR_WIN *pAddrWin1, MV_ADDR_WIN *pAddrWin2)
{
    MV_U32 winBase1, winBase2;
    MV_U32 winTop1, winTop2;
    
    winBase1 = pAddrWin1->baseLow;
    winBase2 = pAddrWin2->baseLow;
    winTop1  = winBase1 + pAddrWin1->size -1;
    winTop2  = winBase2 + pAddrWin2->size -1;
    
    if (((winBase1 >= winBase2 ) && ( winBase1 <= winTop2)) ||
        ((winTop1  >= winBase2) && (winTop1 <= winTop2)))
    {
        return MV_TRUE;
    }
    else
    {
        return MV_FALSE;
    }
}

static const char* cntrlName[] = TARGETS_NAME_ARRAY;

/*******************************************************************************
* mvCtrlTargetNameGet - Get Marvell controller target name
*
* DESCRIPTION:
*       This function convert the trget enumeration to string.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Target name (const MV_8 *)
*******************************************************************************/
const MV_8* mvCtrlTargetNameGet( MV_TARGET target )
{

	if (target >= MAX_TARGETS)
	{
		return "target unknown";
	}

	return cntrlName[target];
}

/*******************************************************************************
* mvCtrlAddrDecShow - Print the Controller units address decode map.
*
* DESCRIPTION:
*		This function the Controller units address decode map.
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
MV_VOID mvCtrlAddrDecShow(MV_VOID)
{
    	mvCpuIfAddDecShow();
    	mvAhbToMbusAddDecShow();
#if defined(MV_INCLUDE_PEX)
	mvPexAddrDecShow();
#endif
#if defined(MV_INCLUDE_USB)
    	mvUsbAddrDecShow();
#endif
#if defined(MV_INCLUDE_GIG_ETH)
	mvEthAddrDecShow();
#endif
#if defined(MV_INCLUDE_XOR)
	mvXorAddrDecShow();
#endif
#if defined(MV_INCLUDE_SATA)
    	mvSataAddrDecShow();
#endif
#if defined(MV_INCLUDE_AUDIO)
    	mvAudioAddrDecShow();
#endif
#if defined(MV_INCLUDE_PDMA)
    	mvPdmaAddrDecShow();
#endif

}

/*******************************************************************************
* ctrlSizeToReg - Extract size value for register assignment.
*
* DESCRIPTION:		
*       Address decode size parameter must be programed from LSB to MSB as
*       sequence of 1's followed by sequence of 0's. The number of 1's 
*       specifies the size of the window in 64 KB granularity (e.g. a 
*       value of 0x00ff specifies 256x64k = 16 MB).
*       This function extract the size value from the size parameter according 
*	to given aligment paramter. For example for size 0x1000000 (16MB) and 
*	aligment 0x10000 (64KB) the function will return 0x00FF.
*
* INPUT:
*       size - Size.
*		alignment - Size alignment. Note that alignment must be power of 2!
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit describing size register value correspond to size parameter. 
*		If value is '-1' size parameter or aligment are invalid.
*******************************************************************************/
MV_U32	ctrlSizeToReg(MV_U32 size, MV_U32 alignment)
{
	MV_U32 retVal;

	/* Check size parameter alignment		*/
	if ((0 == size) || (MV_IS_NOT_ALIGN(size, alignment)))
	{
		DB(mvOsPrintf("ctrlSizeToReg: ERR. Size is zero or not aligned.\n"));
		return -1;
	}
	
	/* Take out the "alignment" portion out of the size parameter */
	alignment--;	/* Now the alignmet is a sequance of '1' (e.g. 0xffff) 		*/
			/* and size is 0x1000000 (16MB) for example	*/
	while(alignment & 1)	/* Check that alignmet LSB is set	*/
	{
		size = (size >> 1); /* If LSB is set, move 'size' one bit to right	*/	
		alignment = (alignment >> 1);
	}
	
	/* If after the alignment first '0' was met we still have '1' in 		*/
	/* it then aligment is invalid (not power of 2) 				*/
	if (alignment)
	{
		DB(mvOsPrintf("ctrlSizeToReg: ERR. Alignment parameter 0x%x invalid.\n", 
			(MV_U32)alignment));
		return -1;
	}

	/* Now the size is shifted right according to aligment: 0x0100			*/
	size--;         /* Now the size is a sequance of '1': 0x00ff 			*/
    
	retVal = size ;
	
	/* Check that LSB to MSB is sequence of 1's followed by sequence of 0's		*/
	while(size & 1)	/* Check that LSB is set	*/
	{
		size = (size >> 1); /* If LSB is set, move one bit to the right		*/	
	}

    	if (size) /* Sequance of 1's is over. Check that we have no other 1's		*/
	{
		DB(mvOsPrintf("ctrlSizeToReg: ERR. Size parameter 0x%x invalid.\n", 
                                                                        size));
		return -1;
	}
	
    	return retVal;
}

/*******************************************************************************
* ctrlRegToSize - Extract size value from register value.
*
* DESCRIPTION:		
*       This function extract a size value from the register size parameter 
*	according to given aligment paramter. For example for register size 
*	value 0xff and aligment 0x10000 the function will return 0x01000000.
*
* INPUT:
*       regSize   - Size as in register format.	See ctrlSizeToReg.
*	alignment - Size alignment. Note that alignment must be power of 2!
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit describing size. 
*		If value is '-1' size parameter or aligment are invalid.
*******************************************************************************/
MV_U32	ctrlRegToSize(MV_U32 regSize, MV_U32 alignment)
{
   	MV_U32 temp;

	/* Check that LSB to MSB is sequence of 1's followed by sequence of 0's		*/ 
	temp = regSize;		/* Now the size is a sequance of '1': 0x00ff		*/
	
	while(temp & 1)	/* Check that LSB is set					*/
	{
		temp = (temp >> 1); /* If LSB is set, move one bit to the right		*/	
	}

    	if (temp) /* Sequance of 1's is over. Check that we have no other 1's		*/
	{
		DB(mvOsPrintf("ctrlRegToSize: ERR. Size parameter 0x%x invalid.\n", 
					regSize));
	   	return -1;
	}
	

	/* Check that aligment is a power of two					*/
	temp = alignment - 1;/* Now the alignmet is a sequance of '1' (0xffff) 		*/
					
	while(temp & 1)	/* Check that alignmet LSB is set				*/
	{
		temp = (temp >> 1); /* If LSB is set, move 'size' one bit to right	*/	
	}
	
	/* If after the 'temp' first '0' was met we still have '1' in 'temp'		*/
	/* then 'temp' is invalid (not power of 2) 					*/
	if (temp)
	{
		DB(mvOsPrintf("ctrlSizeToReg: ERR. Alignment parameter 0x%x invalid.\n", 
					alignment));
		return -1;
	}

	regSize++;      /* Now the size is 0x0100					*/

	/* Add in the "alignment" portion to the register size parameter 		*/
	alignment--;	/* Now the alignmet is a sequance of '1' (e.g. 0xffff) 		*/

	while(alignment & 1)	/* Check that alignmet LSB is set			*/
	{
		regSize   = (regSize << 1); /* LSB is set, move 'size' one bit left	*/	
		alignment = (alignment >> 1);
	}
		
    	return regSize;	
}


/*******************************************************************************
* ctrlSizeRegRoundUp - Round up given size 
*
* DESCRIPTION:		
*       This function rounds up a given size to a size that fits the 
*       restrictions of size format given an aligment parameter.
*	For example for size parameter 0xa1000 and aligment 0x1000 
* 	the function will return 0xFF000.
*
* INPUT:
*       size - Size.
*	alignment - Size alignment. Note that alignment must be power of 2!
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit describing size value correspond to size in register.  
*******************************************************************************/
MV_U32	ctrlSizeRegRoundUp(MV_U32 size, MV_U32 alignment)
{
	MV_U32 msbBit = 0;
	MV_U32 retSize;
	
	/* Check if size parameter is already comply with restriction */
	if (!(-1 == ctrlSizeToReg(size, alignment)))
	{
		return size;
	}
    
    	while(size)
	{
		size = (size >> 1);
        	msbBit++;
	}

    	retSize = (1 << msbBit);
    
    	if (retSize < alignment)
    	{
        	return alignment;
    	}
    	else
    	{
        	return retSize;
    	}
}

MV_BOOL	  mvCtrlIsBootFromSPI(MV_VOID)
{
    MV_U32 satr = 0;
    satr = MV_REG_READ(MPP_SAMPLE_AT_RESET_REG0);
    satr = satr & MSAR_BOOT_MODE_MASK;
    
    if ((satr == MSAR_BOOT_SPI_NO_BOOTROM) || 
	(satr == MSAR_BOOT_SPI_3_CYCLES)   ||
	(satr == MSAR_BOOT_SPI_4_CYCLES))
	return MV_TRUE;
    else
	return MV_FALSE;
}

MV_BOOL	  mvCtrlIsBootFromNAND(MV_VOID)
{
    MV_U32 satr = 0;
    satr = MV_REG_READ(MPP_SAMPLE_AT_RESET_REG0);
    satr = satr & MSAR_BOOT_MODE_MASK;
    
    if (((satr >= MSAR_BOOT_NAND_1) && (satr <= MSAR_BOOT_NAND_9)) || 
	((satr >= MSAR_BOOT_NAND_10) && (satr <= MSAR_BOOT_NAND_21)))
	return MV_TRUE;
    else
	return MV_FALSE;
}

#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
/*******************************************************************************
* mvCtrlPwrClckSet - Set Power State for specific Unit
*
* DESCRIPTION:		
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
*******************************************************************************/
MV_VOID	mvCtrlPwrClckSet(MV_UNIT_ID unitId, MV_U32 index, MV_BOOL enable)
{
	/* Note: '0' in bit means enable clock, '1' means disable clock */
	switch (unitId)
	{
#if defined(MV_INCLUDE_USB)
	case USB_UNIT_ID:
		if (enable == MV_TRUE)
			MV_REG_BIT_SET(CLOCK_GATING_CTRL_REG, CGC_USBENCLOCK_MASK(index));
		else
			MV_REG_BIT_RESET(CLOCK_GATING_CTRL_REG, CGC_USBENCLOCK_MASK(index));
		break;
#endif
#if defined(MV_INCLUDE_GIG_ETH)
	case ETH_GIG_UNIT_ID:
		if (enable == MV_TRUE)
			MV_REG_BIT_SET(CLOCK_GATING_CTRL_REG, CGC_GEENCLOCK_MASK);
		else
			MV_REG_BIT_RESET(CLOCK_GATING_CTRL_REG, CGC_GEENCLOCK_MASK);
		break;
#endif
#if defined(MV_INCLUDE_INTEG_SATA)
	case SATA_UNIT_ID:
		if (enable == MV_TRUE)
			MV_REG_BIT_SET(CLOCK_GATING_CTRL_REG, CGC_SATAENCLOCK_MASK);
		else
			MV_REG_BIT_RESET(CLOCK_GATING_CTRL_REG, CGC_SATAENCLOCK_MASK);
		break;
#endif
#if defined(MV_INCLUDE_PEX)
	case PEX_UNIT_ID:
		if (enable == MV_TRUE)
			MV_REG_BIT_SET(CLOCK_GATING_CTRL_REG, CGC_PEXENCLOCK_MASK(index));
		else
			MV_REG_BIT_RESET(CLOCK_GATING_CTRL_REG, CGC_PEXENCLOCK_MASK(index));
		break;
#endif
#if defined(MV_INCLUDE_SDIO)
	case SDIO_UNIT_ID:
		if (enable == MV_TRUE)
			MV_REG_BIT_SET(CLOCK_GATING_CTRL_REG, CGC_SDIOENCLOCK_MASK(index));
		else
			MV_REG_BIT_RESET(CLOCK_GATING_CTRL_REG, CGC_SDIOENCLOCK_MASK(index));
		break;
#endif
#if defined(MV_INCLUDE_NAND)
	case NAND_UNIT_ID:
		if (enable == MV_TRUE)
			MV_REG_BIT_SET(CLOCK_GATING_CTRL_REG, CGC_NANDENCLOCK_MASK);
		else
			MV_REG_BIT_RESET(CLOCK_GATING_CTRL_REG, CGC_NANDENCLOCK_MASK);
		break;
#endif
#if defined(MV_INCLUDE_CAMERA)
	case CAMERA_UNIT_ID:
		if (enable == MV_TRUE)
			MV_REG_BIT_SET(CLOCK_GATING_CTRL_REG, CGC_CAMENCLOCK_MASK);
		else
			MV_REG_BIT_RESET(CLOCK_GATING_CTRL_REG, CGC_CAMENCLOCK_MASK);
		break;
#endif
#if defined(MV_INCLUDE_AUDIO)
	case AUDIO_UNIT_ID:
		if (enable == MV_TRUE)
			MV_REG_BIT_SET(CLOCK_GATING_CTRL_REG, CGC_ADENCLOCK_MASK(index));
		else
			MV_REG_BIT_RESET(CLOCK_GATING_CTRL_REG, CGC_ADENCLOCK_MASK(index));
		break;
#endif

#if defined(MV_INCLUDE_GPU)
	case GPU_UNIT_ID:
		if (enable == MV_TRUE)
			MV_REG_BIT_SET(CLOCK_GATING_CTRL_REG, CGC_GPUENCLOCK_MASK);
		else
			MV_REG_BIT_RESET(CLOCK_GATING_CTRL_REG, CGC_GPUENCLOCK_MASK);
		break;
#endif

#if defined(MV_INCLUDE_AC97)
	case AC97_UNIT_ID:
		if (enable == MV_TRUE)
			MV_REG_BIT_SET(CLOCK_GATING_CTRL_REG, CGC_AC97ENCLOCK_MASK);
		else
			MV_REG_BIT_RESET(CLOCK_GATING_CTRL_REG, CGC_AC97ENCLOCK_MASK);
		break;
#endif

#if defined(MV_INCLUDE_PDMA)
	case PDMA_UNIT_ID:
		if (enable == MV_TRUE)
			MV_REG_BIT_SET(CLOCK_GATING_CTRL_REG, CGC_PDMAENCLOCK_MASK);
		else
			MV_REG_BIT_RESET(CLOCK_GATING_CTRL_REG, CGC_PDMAENCLOCK_MASK);
		break;
#endif

#if defined(MV_INCLUDE_XOR)
	case XOR_UNIT_ID:
		if (enable == MV_TRUE)
			MV_REG_BIT_SET(CLOCK_GATING_CTRL_REG, CGC_XORENCLOCK_MASK(index));
		else
			MV_REG_BIT_RESET(CLOCK_GATING_CTRL_REG, CGC_XORENCLOCK_MASK(index));
		break;
#endif
	default:
		break;
	}
}

/*******************************************************************************
* mvCtrlPwrClckGet - Get Power State of specific Unit
*
* DESCRIPTION:		
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
******************************************************************************/
MV_BOOL	mvCtrlPwrClckGet(MV_UNIT_ID unitId, MV_U32 index)
{
	MV_U32 reg = MV_REG_READ(CLOCK_GATING_CTRL_REG);
	MV_BOOL state = MV_TRUE;

	switch (unitId)
	{
#if defined(MV_INCLUDE_USB)
	case USB_UNIT_ID:
		if ((reg & CGC_USBENCLOCK_MASK(index)) == CGC_USBENCLOCK_DIS(index))
			state = MV_FALSE;
		else 
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_GIG_ETH)
	case ETH_GIG_UNIT_ID:
		if ((reg & CGC_GEENCLOCK_MASK) == CGC_GEENCLOCK_DIS)
			state = MV_FALSE;
		else 
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_SATA)
	case SATA_UNIT_ID:
		if ((reg & CGC_SATAENCLOCK_MASK) == CGC_SATAENCLOCK_DIS)
			state = MV_FALSE;
		else 
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_PEX)
	case PEX_UNIT_ID:
		if ((reg & CGC_PEXENCLOCK_MASK(index)) == CGC_PEXENCLOCK_DIS(index))
			state = MV_FALSE;
		else 
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_SDIO)
	case SDIO_UNIT_ID:
		if ((reg & CGC_SDIOENCLOCK_MASK(index))== CGC_SDIOENCLOCK_DIS(index))
			state = MV_FALSE;
		else 
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_NAND)
	case NAND_UNIT_ID:
		if ((reg & CGC_NANDENCLOCK_MASK) == CGC_NANDENCLOCK_DIS)
			state = MV_FALSE;
		else 
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_CAMERA)
	case CAMERA_UNIT_ID:
		if ((reg & CGC_CAMENCLOCK_MASK) == CGC_CAMENCLOCK_DIS)
			state = MV_FALSE;
		else 
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_AUDIO)
	case AUDIO_UNIT_ID:
		if ((reg & CGC_ADENCLOCK_MASK(index)) == CGC_ADENCLOCK_DIS(index))
			state = MV_FALSE;
		else 
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_GPU)
	case GPU_UNIT_ID:
		if ((reg & CGC_GPUENCLOCK_MASK) == CGC_GPUENCLOCK_DIS)
			state = MV_FALSE;
		else 
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_AC97)
	case AC97_UNIT_ID:
		if ((reg & CGC_AC97ENCLOCK_MASK) == CGC_AC97ENCLOCK_DIS)
			state = MV_FALSE;
		else 
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_PDAM)
	case PDMA_UNIT_ID:
		if ((reg & CGC_PDMAENCLOCK_MASK) == CGC_PDMAENCLOCK_DIS)
			state = MV_FALSE;
		else 
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_XOR)
	case XOR_UNIT_ID:
		if ((reg & CGC_XORENCLOCK_MASK(index))== CGC_XORENCLOCK_DIS(index))
			state = MV_FALSE;
		else 
			state = MV_TRUE;
		break;
#endif
	default:
		state = MV_TRUE;
		break;
	}

	return state;	
}
#else
MV_VOID   mvCtrlPwrClckSet(MV_UNIT_ID unitId, MV_U32 index, MV_BOOL enable) {return;}
MV_BOOL	  mvCtrlPwrClckGet(MV_UNIT_ID unitId, MV_u32 index) {return MV_TRUE;}
#endif /* #if defined(MV_INCLUDE_CLK_PWR_CNTRL) */


