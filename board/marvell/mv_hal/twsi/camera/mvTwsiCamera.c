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
#include "../mvTwsi.h"
#include "mvTwsiCameraRegs.h"
#include "mvTwsiCamera.h"

static MV_U32 camTwsiTclk;

/*******************************************************************************
* mvTwsiCameraClkDivGet - return clock divider
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*	Data value
*
* RETURN:
*******************************************************************************/
static MV_U32 mvTwsiCameraClkDivGet(void)
{
	if (camTwsiTclk == MV_BOARD_TCLK_166MHZ)
		return 414;
	else
		mvOsPrintf("mvTwsiCameraClkDivGet: Not supported core clock %d\n",camTwsiTclk);

	return 0;
}


/*******************************************************************************
* mvTwsiCameraReadByte - Read one byte register through the I2C agent
*
* DESCRIPTION:
*
* INPUT:
*	id: I2C slave id address
*	addr: register offset
*
* OUTPUT:
*	Data value
*
* RETURN:
*    MV_ERROR: if the controller was not initialized properly
*    MV_FAIL: if the TWSI operation failed
*    MV_OK otherwise
*******************************************************************************/
MV_STATUS mvTwsiCameraReadByte(MV_U8 id, MV_U8 addr,
			MV_U8 *value)
{
     MV_U32 reg;
     MV_U32 irqStatus;
     int i;

     reg = MV_CAM_TWSI0_EN;
     reg |= id << 2; /*slave id*/
     reg |= MV_CAM_TWSI0_RDSTP;
     reg |= mvTwsiCameraClkDivGet() << 10;

     /* clear IRQ status */
     MV_TWSI_CAM_REG_WRITE(MV_CAM_IRQ_STATUS, MV_CAM_IRQ_TWSI_READ | 
		  MV_CAM_IRQ_TWSI_ERROR);

     MV_TWSI_CAM_REG_WRITE(MV_CAM_TWSI0, reg);
     
     MV_TWSI_CAM_REG_READ(MV_CAM_TWSI1); /*force previous write*/
     reg = MV_CAM_TWSI1_READ;
     reg |= addr << 16;
     MV_TWSI_CAM_REG_WRITE(MV_CAM_TWSI1, reg);

     i = 0;
     do {
	  mvOsUDelay(10);
	  irqStatus = MV_TWSI_CAM_REG_READ(MV_CAM_IRQ_STATUS);
	  if(i++ > 100000)
	  {
	       mvOsPrintf("mvTwsiCameraReadByte: TWSI Read loop Timeout\n");     
	       return MV_FAIL;
	  }
	  if(irqStatus & MV_CAM_IRQ_TWSI_ERROR)
	  {
	       mvOsPrintf("mvTwsiCameraReadByte: TWSI Read Error\n");     
	       return MV_FAIL;
	  }
     } while(!(irqStatus & MV_CAM_IRQ_TWSI_READ));

     reg = MV_TWSI_CAM_REG_READ(MV_CAM_TWSI1);
     
     if(reg & MV_CAM_TWSI1_ERROR) 
     {
	  mvOsPrintf("mvTwsiCameraReadByte: TWSI Read Error\n");     
	  return MV_FAIL;
     }
     if(!(reg & MV_CAM_TWSI1_VALID)) 
     {
	  mvOsPrintf("mvTwsiCameraReadByte: TWSI Read Timeout\n");     
	  return MV_FAIL;
     }
     *value = reg;
     return MV_OK;
}

/*******************************************************************************
* mvTwsiCameraWriteByte - Write one byte register through the I2C agent
*
* DESCRIPTION:
*
* INPUT:
*	id: I2C target id
*	addr: register offset
*	data value
*
* OUTPUT:
*
* RETURN:
*    MV_ERROR: if the controller was not initialized properly
*    MV_FAIL: if the TWSI operation failed
*    MV_OK otherwise
*******************************************************************************/
MV_STATUS mvTwsiCameraWriteByte(MV_U8 id, MV_U8 addr, MV_U8 value)
{
     MV_U32 reg;
     MV_U32 irqStatus;
     int i;


     reg = MV_CAM_TWSI0_EN;
     reg |= id << 2; /*slave id*/
     reg |= MV_CAM_TWSI0_RDSTP;
     reg |= mvTwsiCameraClkDivGet() << 10;

     /* clear IRQ status */
     MV_TWSI_CAM_REG_WRITE(MV_CAM_IRQ_STATUS, MV_CAM_IRQ_TWSI_WRITE | 
		  MV_CAM_IRQ_TWSI_ERROR);
     
     MV_TWSI_CAM_REG_WRITE(MV_CAM_TWSI0, reg);
     
     MV_TWSI_CAM_REG_READ(MV_CAM_TWSI1); /*force previous write*/

     reg = value;
     reg |= addr << 16;
     MV_TWSI_CAM_REG_WRITE(MV_CAM_TWSI1, reg);

     i = 0;
     do {
	  mvOsUDelay(10);
	  irqStatus = MV_TWSI_CAM_REG_READ(MV_CAM_IRQ_STATUS);
	  if(i++ > 100000)
	  {
	       mvOsPrintf("mvTwsiCameraWriteByte: TWSI Write loop Timeout\n");     
	       return MV_FAIL;
	  }
	  if(irqStatus & MV_CAM_IRQ_TWSI_ERROR)
	  {
	       mvOsPrintf("mvTwsiCameraWriteByte: TWSI Write Error\n");     
	       return MV_FAIL;
	  }
     } while(!(irqStatus & MV_CAM_IRQ_TWSI_WRITE));

     reg = MV_TWSI_CAM_REG_READ(MV_CAM_TWSI1);
     
     if(reg & MV_CAM_TWSI1_ERROR) 
     {
	  mvOsPrintf("mvTwsiCameraWriteByte: TWSI Write Error\n");     
	  return MV_FAIL;
     }
     if(!(reg & MV_CAM_TWSI1_WRITE)) 
     {
	  mvOsPrintf("mvTwsiCameraWriteByte: TWSI Write Timeout\n");     
	  return MV_FAIL;
     }

     return MV_OK;
}



/*******************************************************************************
* mvTwsiCameraInit - Initialize TWSI camera unit 
*
* DESCRIPTION:
*       This routine initializes the camera twsi unit given by chanNum
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
MV_U32 mvTwsiCameraInit(MV_U8 chan,MV_KHZ freq, MV_U32 tclk, MV_TWSI_ADDR *twsiAddr, MV_BOOL generalCallEnable)
{
	camTwsiTclk = tclk;
	return freq;
}

/*******************************************************************************
* mvTwsiCameraStartBitSet - Set start bit on the bus for the twsi unit given by chanNum
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
MV_STATUS mvTwsiCameraStartBitSet(MV_U8 chanNum)
{
	return MV_OK;
}

/*******************************************************************************
* mvTwsiCameraStopBitSet - Set stop bit on the bus for the twsi unit given by chanNum
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
MV_STATUS mvTwsiCameraStopBitSet(MV_U8 chanNum)
{
	return MV_OK;
}

/*******************************************************************************
* mvTwsiCameraAddrSet - Set address on TWSI bus for the twsi unit given by chanNum
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
MV_STATUS mvTwsiCameraAddrSet(MV_U8 chanNum, MV_TWSI_ADDR *twsiAddr, MV_TWSI_CMD command)
{
	return MV_OK;
}

/*******************************************************************************
* mvTwsiCameraRead - Read data block from a TWSI Slave on the twsi unit specified by chanNum
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
MV_STATUS mvTwsiCameraRead(MV_U8 chanNum, MV_TWSI_SLAVE *twsiSlave, MV_U8 *pBlock, MV_U32 blockSize)
{
	MV_STATUS ret;
	MV_U32 index;

	for (index=0; index<blockSize; index++) {
		ret=mvTwsiCameraReadByte(twsiSlave->slaveAddr.address,twsiSlave->offset+index,&pBlock[index]);
		if (ret != MV_OK)
			return ret;
	}		
	return MV_OK;
}

/*******************************************************************************
* mvTwsiCameraWrite - Write data block to a TWSI Slave on the twsi unit given by chanNum
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
MV_STATUS mvTwsiCameraWrite(MV_U8 chanNum, MV_TWSI_SLAVE *twsiSlave, MV_U8 *pBlock, MV_U32 blockSize)
{
	MV_STATUS ret;
	MV_U32 index;

	for (index=0; index<blockSize; index++) {
		ret=mvTwsiCameraWriteByte(twsiSlave->slaveAddr.address,twsiSlave->offset+index,pBlock[index]);
		if (ret != MV_OK)
			return ret;
	}		
	return MV_OK;
}

