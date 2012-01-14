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
#include "mvTwsi.h"
#include "camera/mvTwsiCamera.h"
#include "soc/mvTwsiSoc.h"


static struct mvTwsiUnitInfo {
	MV_U8 minChannel;
	MV_U8 maxChannel;
	MV_U32 (*mvInit)(MV_U8 chan,MV_KHZ freq, MV_U32 tclk, MV_TWSI_ADDR *twsiAddr, MV_BOOL generalCallEnable);
	MV_STATUS (*mvRead)(MV_U8 chanNum, MV_TWSI_SLAVE *twsiSlave, MV_U8 *pBlock, MV_U32 blockSize);
	MV_STATUS (*mvWrite)(MV_U8 chanNum, MV_TWSI_SLAVE *twsiSlave, MV_U8 *pBlock, MV_U32 blockSize);
	MV_STATUS (*mvStartBitSet)(MV_U8 chanNum);
	MV_STATUS (*mvStopBitSet)(MV_U8 chanNum);
	MV_STATUS (*mvAddrSet)(MV_U8 chanNum, MV_TWSI_ADDR *twsiAddr, MV_TWSI_CMD command);
} mvTwsiUnitTable[] = { 
#ifdef MV_TWSI_UNIT_INCLUDE_SOC
	{	/* soc */	
		MV_TWSI_UNIT_SOC_MIN_CHANNEL,
		MV_TWSI_UNIT_SOC_MAX_CHANNEL,
		mvTwsiSocInit,
		mvTwsiSocRead,
		mvTwsiSocWrite,
		mvTwsiSocStartBitSet,
		mvTwsiSocStopBitSet,
		mvTwsiSocAddrSet
	},
#endif
#ifdef MV_TWSI_UNIT_INCLUDE_CAM
	{	/* camera */
		MV_TWSI_UNIT_CAM_MIN_CHANNEL,
		MV_TWSI_UNIT_CAM_MAX_CHANNEL,
		mvTwsiCameraInit,
		mvTwsiCameraRead,
		mvTwsiCameraWrite,
		mvTwsiCameraStartBitSet,
		mvTwsiCameraStopBitSet,
		mvTwsiCameraAddrSet		
	},
#endif
};

/*******************************************************************************
* mvTwsiGetUnitInfo - return unit info responsible for the given channel number
*
* DESCRIPTION:
*
* INPUT:
*       chanNum - channel number
*
* OUTPUT:
*       None.
*
* RETURN:
*       unit info - unit info for this channel number.
*
*******************************************************************************/
static struct mvTwsiUnitInfo* mvTwsiGetUnitInfo(MV_U8 chanNum)
{
	MV_U32 unit;

	for (unit=0; unit< sizeof(mvTwsiUnitTable); unit++) {
		if (chanNum >= mvTwsiUnitTable[unit].minChannel &&
			chanNum <= mvTwsiUnitTable[unit].maxChannel)
			return &mvTwsiUnitTable[unit];
	}
	printf("mvTwsiGetUnitInfo() failed.\n");
	return NULL;
}



/*******************************************************************************
* mvTwsiInit - Initialize TWSI interface
*
* DESCRIPTION:
*       This routine initializes the twsi unit given by chanNum
*
* INPUT:
*	chanNum - TWSI channel
*       frequency - TWSI frequancy in KHz. (up to 100KHZ)
*
* OUTPUT:
*       None.
*
* RETURN:
*       Actual frequancy.
*
*******************************************************************************/
MV_U32 mvTwsiInit(MV_U8 chanNum, MV_KHZ freq, MV_U32 tclk, MV_TWSI_ADDR *twsiAddr, MV_BOOL generalCallEnable)
{
	struct mvTwsiUnitInfo* uInfo;

	uInfo = mvTwsiGetUnitInfo(chanNum);
	if (!uInfo)
		return freq;

	if (!uInfo->mvInit)
		return freq;
	
	return 	uInfo->mvInit(chanNum-uInfo->minChannel, freq, tclk, twsiAddr, generalCallEnable);
}


/*******************************************************************************
* mvTwsiStartBitSet - Set start bit on the bus for the twsi unit given by chanNum
*
* DESCRIPTION:
*
* INPUT:
*       chanNum - TWSI channel.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK is start bit was set successfuly on the bus.
*       MV_FAIL if interrupt flag was set before setting start bit.
*
*******************************************************************************/
MV_STATUS mvTwsiStartBitSet(MV_U8 chanNum)
{
	struct mvTwsiUnitInfo* uInfo;

	uInfo = mvTwsiGetUnitInfo(chanNum);
	if (!uInfo)
		return MV_FAIL;

	if (!uInfo->mvStartBitSet)
		return MV_OK;

	return 	uInfo->mvStartBitSet(chanNum-uInfo->minChannel);
}



/*******************************************************************************
* mvTwsiStopBitSet - Set stop bit on the bus for the twsi unit given by chanNum
*
* DESCRIPTION:
*
* INPUT:
*       chanNum - TWSI channel.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK is start bit was set successfuly on the bus.
*       MV_FAIL if interrupt flag was set before setting start bit.
*
*******************************************************************************/
MV_STATUS mvTwsiStopBitSet(MV_U8 chanNum)
{
	struct mvTwsiUnitInfo* uInfo;

	uInfo = mvTwsiGetUnitInfo(chanNum);
	if (!uInfo)
		return MV_FAIL;

	if (!uInfo->mvStopBitSet)
		return MV_OK;

	return 	uInfo->mvStopBitSet(chanNum-uInfo->minChannel);
}


/*******************************************************************************
* mvTwsiAddrSet - Set address on TWSI bus for the twsi unit given by chanNum
*
* DESCRIPTION:
*       This function Set address (7 or 10 Bit address) on the Twsi Bus.
*
* INPUT:
*	chanNum - TWSI channel
*       pTwsiAddr - twsi address.
*	command	 - read / write .
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK - if setting the address completed succesfully.
*	MV_FAIL otherwmise.
*
*******************************************************************************/
MV_STATUS mvTwsiAddrSet(MV_U8 chanNum, MV_TWSI_ADDR *twsiAddr, MV_TWSI_CMD command)
{
	struct mvTwsiUnitInfo* uInfo;

	uInfo = mvTwsiGetUnitInfo(chanNum);
	if (!uInfo)
		return MV_FAIL;

	if (!uInfo->mvAddrSet)
		return MV_OK;

	return 	uInfo->mvAddrSet(chanNum-uInfo->minChannel,twsiAddr,command);
}

/*******************************************************************************
* mvTwsiRead - Read data block from a TWSI Slave on the twsi unit specified by chanNum
*
* DESCRIPTION:
*
* INPUT:
*	chanNum - TWSI channel
*      	pTwsiSlave - Twsi Slave structure. 
*       blockSize - number of bytes to read.	
*
* OUTPUT:
*      	pBlock - Data block.
*
* RETURN:
*       MV_OK - if EEPROM read transaction completed succesfully,
* 	MV_BAD_PARAM - if pBlock is NULL,
*	MV_FAIL otherwmise.
*
*******************************************************************************/
MV_STATUS mvTwsiRead (MV_U8 chanNum, MV_TWSI_SLAVE *twsiSlave, MV_U8 *pBlock, MV_U32 blockSize)
{
	struct mvTwsiUnitInfo* uInfo;

	uInfo = mvTwsiGetUnitInfo(chanNum);
	if (!uInfo)
		return MV_FAIL;

	if (!uInfo->mvRead)
		return MV_OK;

	return 	uInfo->mvRead(chanNum-uInfo->minChannel,twsiSlave,pBlock,blockSize);
}


/*******************************************************************************
* mvTwsiWrite - Write data block to a TWSI Slave on the twsi unit given by chanNum
*
* DESCRIPTION:
*
* INPUT:
*	chanNum - TWSI channel
*       twsiSlave - slave address. 
*       blockSize - number of bytes to write.	
*      	pBlock - Data block.
*
* OUTPUT:
*	None
*
* RETURN:
*       MV_OK - if write transaction completed succesfully.
*	MV_FAIL otherwmise.
*
*******************************************************************************/
MV_STATUS mvTwsiWrite(MV_U8 chanNum, MV_TWSI_SLAVE *twsiSlave, MV_U8 *pBlock, MV_U32 blockSize)
{
	struct mvTwsiUnitInfo* uInfo;

	uInfo = mvTwsiGetUnitInfo(chanNum);
	if (!uInfo)
		return MV_FAIL;

	if (!uInfo->mvWrite)
		return MV_OK;

	return 	uInfo->mvWrite(chanNum-uInfo->minChannel,twsiSlave,pBlock,blockSize);
}
