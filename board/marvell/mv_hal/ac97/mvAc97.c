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
#include "mvSysAc97Config.h"
#include "mvAc97Regs.h"
#include "mvAc97.h"

#define AC97_READ_WRITE_MAX_RETRY	100

/*******************************************************************************
* mvAc97Init 
*
* DESCRIPTION:
*       Initialize the AC'97 unit, and bring the AC'97 unit out of reset.
*
* INPUT:
*       pdmaModeEn - Enable / Disable PDMA mode.
*		     MV_TRUE: Set to PDMA mode.
*		     MV_FALSE: Set to PIO mode.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK	- On successfull init,
*	MV_FAIL	- If initialization fails.
*******************************************************************************/
MV_STATUS mvAc97Init(MV_BOOL pdmaModeEn)
{

	/* Enable DMA or PIO modes.		*/
	if(pdmaModeEn == MV_TRUE)
		MV_REG_BIT_RESET(MV_AC97_GLOBAL_CTRL_REG,AC97_GLB_CTRL_DMA_EN_MASK);
	else
		MV_REG_BIT_SET(MV_AC97_GLOBAL_CTRL_REG,AC97_GLB_CTRL_DMA_EN_MASK);


	return MV_OK;
}


/*******************************************************************************
* mvAc97Reset 
*
* DESCRIPTION:
*       Cold reset the AC'97 unit.
*
* INPUT:
*	None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK	- On successfull init,
*	MV_FAIL	- If initialization fails.
*******************************************************************************/
MV_STATUS mvAc97Reset(MV_VOID)
{
	//MV_U32 timeout = 1000;

	/* Hold CLKBPB for 100us */
	MV_REG_BIT_RESET(MV_AC97_GLOBAL_CTRL_REG,AC97_GLB_CTRL_COLD_RESET_MASK);
        MV_REG_BIT_SET(MV_AC97_GLOBAL_CTRL_REG,AC97_GLB_CTRL_INT_CLK_EN_MASK);
        mvOsUDelay(100);
        MV_REG_BIT_RESET(MV_AC97_GLOBAL_CTRL_REG,AC97_GLB_CTRL_INT_CLK_EN_MASK);

	MV_REG_BIT_SET(MV_AC97_GLOBAL_CTRL_REG,AC97_GLB_CTRL_COLD_RESET_MASK);

#if 0 /* Not sure if this is needed.	*/

	MV_REG_BIT_SET(MV_AC97_GLOBAL_CTRL_REG,
		       AC97_GLB_CTRL_COLD_RESET_MASK | AC97_GLB_CTRL_WARM_RESET_MASK);

	while(timeout > 0) {
		val = MV_REG_READ(MV_AC97_GLOBAL_STATUS_REG);
		if(val & (AC97_GLB_PCODEC_READY_MASK | AC97_GLB_SCODEC_READY_MASK))
			break;
		timeout--;
		mvOsDelay(10);
	}

	if(timeout == 0)
		return MV_TIMEOUT;

#endif /* 0 */
	return MV_OK;
}


/*******************************************************************************
* mvAc97FifoIntEnable
*
* DESCRIPTION:
*       Enable / Disable the various Fifo status indication interrupts.
*
* INPUT:
*       fifoType - the Fifo type to configure the interrupts for.
*	intType	 - The interrupt type to enable / disable.
*	enable	 - MV_TRUE to enable the interrupt indication, MV_FALSE to 
*		   disable the interrupt indication.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK	- On successfull init.
*	MV_BAD_PARAM - If interrupt type not supported for the given Fifo.
*	MV_FAIL	- Otherwise.
*******************************************************************************/
MV_STATUS mvAc97FifoIntEnable(MV_AC97_FIFO_TYPE fifoType, MV_AC97_FIFO_INT_TYPE intType, 
			      MV_BOOL enable)
{
	MV_U32	data;
	MV_U32 	regAddr;
	MV_U32 	mask;

	switch (fifoType) {
	case(AC97_PCM_OUT):
		regAddr = MV_AC97_PCM_OUT_CTRL_REG;
		break;
	case(AC97_PCM_IN):
		regAddr = MV_AC97_PCM_IN_CTRL_REG;
		break;
	case(AC97_MIC_IN):
		regAddr = MV_AC97_MIC_IN_STATUS_REG;
		break;
	case(AC97_PCM_SURROUND_OUT):
		regAddr = MV_AC97_PCM_SURR_OUT_CTRL_REG;
		break;
	case(AC97_PCM_CENTER_LFE_OUT):
		regAddr = MV_AC97_PCM_CNTR_LFE_CTRL_REG;
		break;
	case(AC97_MODEM_OUT):
		regAddr = MV_AC97_MODEM_OUT_CTRL_REG;
		break;
	case(AC97_MODEM_IN):
		regAddr = MV_AC97_MODEM_IN_CTRL_REG;
		break;
	default:
		return MV_ERROR;
	}

	data = MV_REG_READ(regAddr);

	if(intType == AC97_FIFO_ERROR_INT)
		mask = AC97_FIFO_ERR_INT_MASK;
	else
		mask = AC97_FIFO_SERV_REQ_INT_MASK;

	if(enable == MV_TRUE)
                data |= mask;	/* Set the interrupt bit.	*/
	else
		data &= ~mask; 	/* Clear the interrupt bit.	*/

	MV_REG_WRITE(regAddr,data);

	return MV_OK;
}


/*******************************************************************************
* mvAc97GlobalIntEnable
*
* DESCRIPTION:
*       Enable / Disable the various global status indication interrupts.
*
* INPUT:
*	intMask	 - A bitwise OR of the interrupts to be enabled / disabled.
*	enable	 - MV_TRUE to enable the interrupt indication, MV_FALSE to 
*		   disable the interrupt indication.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK	- On successfull init.
*	MV_FAIL	- Otherwise.
*******************************************************************************/
MV_STATUS mvAc97GlobalIntEnable(MV_U32 intMask, MV_BOOL enable)
{
	MV_U32	data;

	data = MV_REG_READ(MV_AC97_GLOBAL_CTRL_REG);

	if(enable == MV_TRUE)
		data |= intMask;	/* Set the interrupt bits.	*/
	else
		data &= ~intMask;	/* Clear the interrupt bits.	*/

	MV_REG_WRITE(MV_AC97_GLOBAL_CTRL_REG,data);

	return MV_OK;
}


/*******************************************************************************
* mvAc97FifoDataRead32
*
* DESCRIPTION:
*       Read the given amount of 32-bit samples from one of the Rx FIFOs.
*	This function is relevant only when the unit is configured in PIO mode.
*	It's the user's responsibility to make sure that there are enough 
*	samples in the Rx Fifo to be read before calling this function.
*
* INPUT:
*	fifoType- The Fifo type to read from.
*       data	- A buffer to fill the read data in.
*	length	- Amount of 32-bit samples to read.
*
* OUTPUT:
*       data	- The data read from the Fifo.
*
* RETURN:
*       MV_OK	- On successfull init,
*	MV_BAD_PARAM - Cannot perform a read operation from the given Fifo.
*	MV_NOT_SUPPORTED - Requested access width not supported on the given Fifo.
*	MV_FAIL	- If initialization fails.
*******************************************************************************/
static MV_STATUS mvAc97FifoDataRead32(MV_AC97_FIFO_TYPE fifoType, MV_U32 *data, MV_U16 length)
{
	MV_U32	regAddr;
	MV_U32	i;

	if((data == NULL) || (length == 0))
	   return MV_BAD_PARAM;

	switch (fifoType) {
	case (AC97_PCM_IN):
		regAddr = MV_AC97_PCM_DATA_REG;
		break;
	case (AC97_MODEM_OUT):
	case (AC97_PCM_CENTER_LFE_OUT):
	case (AC97_PCM_SURROUND_OUT):
	case (AC97_PCM_OUT):
		return MV_BAD_PARAM;
	case (AC97_MIC_IN):
	case (AC97_MODEM_IN):
		return MV_NOT_SUPPORTED;
	}

	for(i = 0; i < length; i++) {
		data[i] = MV_REG_READ(regAddr);
	}
	return MV_OK;
}


/*******************************************************************************
* mvAc97FifoDataWrite32
*
* DESCRIPTION:
*       Write the given amount of 32-bit samples to one of the Tx FIFOs.
*	This function is relevant only when the unit is configured in PIO mode.
*	It's the user's responsibility to make sure that there are enough 
*	place in the Tx fifo before calling this function.
*
* INPUT:
*	fifoType- The Fifo type to write to.
*       data	- A buffer holding the data to be written.
*	length	- Amount of 32-bit samples to write.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK	- On successfull init,
*	MV_BAD_PARAM - Cannot perform a write operation to the given Fifo.
*	MV_NOT_SUPPORTED - Requested access width not supported on the given Fifo.
*	MV_FAIL	- If initialization fails.
*******************************************************************************/
static MV_STATUS mvAc97FifoDataWrite32(MV_AC97_FIFO_TYPE fifoType, MV_U32 *data, MV_U16 length)
{
	MV_U32	regAddr;
	MV_U32	i;

	if((data == NULL) || (length == 0))
	   return MV_BAD_PARAM;

	switch (fifoType) {
	case (AC97_PCM_OUT):
		regAddr = MV_AC97_PCM_DATA_REG;
		break;
	case (AC97_PCM_CENTER_LFE_OUT):
		regAddr = MV_AC97_PCM_CNTR_LFE_DATA_REG;
		break;
	case (AC97_PCM_SURROUND_OUT):
		regAddr = MV_AC97_PCM_SURR_DATA_REG;
		break;
	case (AC97_PCM_IN):
	case (AC97_MIC_IN):
	case (AC97_MODEM_IN):
		return MV_BAD_PARAM;
	case (AC97_MODEM_OUT):
		return MV_NOT_SUPPORTED;
	default:
		return MV_ERROR;
	}

	for(i = 0; i < length; i++) {
		MV_REG_WRITE(regAddr,data[i]);
	}
	return MV_OK;
}


/*******************************************************************************
* mvAc97FifoDataRead16
*
* DESCRIPTION:
*       Read the given amount of 16-bit samples from one of the Rx FIFOs.
*	This function is relevant only when the unit is configured in PIO mode.
*	It's the user's responsibility to make sure that there are enough 
*	samples in the Rx Fifo to be read before calling this function.
*
* INPUT:
*	fifoType- The Fifo type to read from.
*       data	- A buffer to fill the read data in.
*	length	- Amount of 16-bit samples to read.
*
* OUTPUT:
*       data	- The data read from the Fifo.
*
* RETURN:
*       MV_OK	- On successfull init,
*	MV_BAD_PARAM - Cannot perform a read operation from the given Fifo.
*	MV_NOT_SUPPORTED - Requested access width not supported on the given Fifo.
*	MV_FAIL	- If initialization fails.
*******************************************************************************/
static MV_STATUS mvAc97FifoDataRead16(MV_AC97_FIFO_TYPE fifoType, MV_U16 *data, MV_U16 length)
{
	MV_U32	regAddr=0;
	MV_U32	i;

	if((data == NULL) || (length == 0))
	   return MV_BAD_PARAM;

	switch (fifoType) {
	case (AC97_MIC_IN):
		regAddr = MV_AC97_MIC_IN_DATA_REG;
		break;
	case (AC97_MODEM_IN):
		regAddr = MV_AC97_MODEM_DATA_REG;
		break;
	case (AC97_MODEM_OUT):
	case (AC97_PCM_CENTER_LFE_OUT):
	case (AC97_PCM_SURROUND_OUT):
	case (AC97_PCM_OUT):
		return MV_BAD_PARAM;
	case (AC97_PCM_IN):
		return MV_NOT_SUPPORTED;
	}

	for(i = 0; i < length; i++) {
		data[i] = (MV_U16)(MV_REG_READ(regAddr) & 0xFFFF);
	}
	return MV_OK;
}



/*******************************************************************************
* mvAc97FifoDataWrite16
*
* DESCRIPTION:
*       Write the given amount of 16-bit samples to one of the Tx FIFOs.
*	This function is relevant only when the unit is configured in PIO mode.
*	It's the user's responsibility to make sure that there are enough 
*	place in the Tx fifo before calling this function.
*
* INPUT:
*	fifoType- The Fifo type to write to.
*       data	- A buffer holding the data to be written.
*	length	- Amount of 16-bit samples to write.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK	- On successfull init,
*	MV_BAD_PARAM - Cannot perform a write operation to the given Fifo.
*	MV_NOT_SUPPORTED - Requested access width not supported on the given Fifo.
*	MV_FAIL	- If initialization fails.
*******************************************************************************/
static MV_STATUS mvAc97FifoDataWrite16(MV_AC97_FIFO_TYPE fifoType, MV_U16 *data, MV_U16 length)
{
	MV_U32	regAddr;
	MV_U32	regData;
	MV_U32	i;

	if((data == NULL) || (length == 0))
	   return MV_BAD_PARAM;

	switch (fifoType) {
	case (AC97_MODEM_OUT):
		regAddr = MV_AC97_MODEM_DATA_REG;
		break;
	case (AC97_PCM_IN):
	case (AC97_MIC_IN):
	case (AC97_MODEM_IN):
		return MV_BAD_PARAM;
	case (AC97_PCM_OUT):
	case (AC97_PCM_CENTER_LFE_OUT):
	case (AC97_PCM_SURROUND_OUT):
		return MV_NOT_SUPPORTED;
	}

	for(i = 0; i < length; i++) {
		regData = (MV_U32)data[i];
		MV_REG_WRITE(regAddr,regData);
	}
	return MV_OK;
}


/*******************************************************************************
* mvAc97FifoDataRead
*
* DESCRIPTION:
*       Read the given amount of 32-bit / 16-bit samples from one of the Rx FIFOs.
*	This function is relevant only when the unit is configured in PIO mode.
*	The sample sizes of the read data depends on the Fifo to read from.
*	It's the user's responsibility to make sure that there are enough 
*	samples in the Rx Fifo to be read before calling this function.
*	If reading from a 32-bit fifo, it's required that the data pointer 
*	is aligned to 32-bit.
*	If reading from a 16-bit fifo, it's required that the data pointer is
*	aligned to 16-bit.
*
* INPUT:
*	fifoType- The Fifo type to read from.
*       data	- A buffer to fill the read data in.
*	length	- Number of samples to read.
*
* OUTPUT:
*       data	- The data read from the Fifo.
*
* RETURN:
*       MV_OK	- Success,
*	MV_BAD_PARAM - Cannot perform a read operation from the given Fifo.
*	MV_BAD_PTR - If data has an incorrect alignment.
*	MV_FAIL	- Otherwise.
*******************************************************************************/
MV_STATUS mvAc97FifoDataRead(MV_AC97_FIFO_TYPE fifoType, MV_VOID *data,
			     MV_U16 length)
{
	MV_STATUS status = MV_OK;

	if((data == NULL) || (length == 0))
	   return MV_BAD_PARAM;

	switch (fifoType) {
	case (AC97_PCM_IN):
		if(((MV_U32)data) & 0x3)
			status = MV_BAD_PTR;
		else
			status = mvAc97FifoDataRead32(fifoType,data,length);
		break;
	case (AC97_MIC_IN):
	case (AC97_MODEM_IN):
		if(((MV_U32)data) & 0x1)
			status = MV_BAD_PTR;
		else
			status = mvAc97FifoDataRead16(fifoType,data,length);
		break;
	case (AC97_PCM_CENTER_LFE_OUT):
	case (AC97_PCM_SURROUND_OUT):
	case (AC97_PCM_OUT):
	case (AC97_MODEM_OUT):
		status = MV_BAD_PARAM;
		break;
	}

	return status;
}


/*******************************************************************************
* mvAc97FifoDataWrite
*
* DESCRIPTION:
*       Write the given amount of 32-bit / 16-bit samples to one of the Tx FIFOs.
*	This function is relevant only when the unit is configured in PIO mode.
*	The sample sizes of the written data depends on the Fifo to write to.
*	It's the user's responsibility to make sure that there are enough 
*	place in the Tx fifo before calling this function.
*	If writing to a 32-bit fifo, it's required that the data pointer 
*	is aligned to 32-bit.
*	If writing to a 16-bit fifo, it's required that the data pointer is
*	aligned to 16-bit.
*
* INPUT:
*	fifoType- The Fifo type to write to.
*       data	- A buffer to write the data from.
*	length	- Number of samples to write.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK	- Success,
*	MV_BAD_PARAM - Cannot perform a write operation to the given Fifo.
*	MV_BAD_PTR - If data has an incorrect alignment.
*	MV_FAIL	- Otherwise.
*******************************************************************************/
MV_STATUS mvAc97FifoDataWrite(MV_AC97_FIFO_TYPE fifoType, MV_VOID *data,
			      MV_U16 length)
{
	MV_STATUS status = MV_OK;

	if((data == NULL) || (length == 0))
	   return MV_BAD_PARAM;

	switch (fifoType) {
	case (AC97_PCM_CENTER_LFE_OUT):
	case (AC97_PCM_SURROUND_OUT):
	case (AC97_PCM_OUT):
		if(((MV_U32)data) & 0x3)
			status = MV_BAD_PTR;
		else
			status = mvAc97FifoDataWrite32(fifoType,data,length);
		break;
	case (AC97_MODEM_OUT):
		if(((MV_U32)data) & 0x1)
			status = MV_BAD_PTR;
		else
			status = mvAc97FifoDataWrite16(fifoType,data,length);
		break;
	case (AC97_PCM_IN):
	case (AC97_MIC_IN):
	case (AC97_MODEM_IN):
		status = MV_BAD_PARAM;
		break;
	}

	return status;
}


/*******************************************************************************
* mvAc97PdmaFifoAccessRegAddr 
*
* DESCRIPTION:
*       Return the register address to be used by the PDMA for reading / writing 
*	data to the given Fifo.
*
* INPUT:
*       fifoType - The Fifo for which the PDMA access address should be returned.
*
* OUTPUT:
*       addr - The address of the Fifo access register. This address is the
*	       Virtual address of the register.
*
* RETURN:
*       MV_OK	- On successfull init,
*	MV_FAIL	- Otherwise.
*******************************************************************************/
MV_STATUS mvAc97PdmaFifoAccessRegAddr(MV_AC97_FIFO_TYPE fifoType, MV_U32 *addr)
{
	MV_U32	regAddr;

	if(addr == NULL)
		return MV_BAD_PARAM;

	switch (fifoType) {
	case (AC97_PCM_IN):
	case (AC97_PCM_OUT):
		regAddr = MV_AC97_PCM_DATA_REG;
		break;
	case (AC97_MIC_IN):
		regAddr = MV_AC97_MIC_IN_DATA_REG;
		break;
	case (AC97_PCM_SURROUND_OUT):
		regAddr = MV_AC97_PCM_SURR_DATA_REG;
		break;
	case (AC97_PCM_CENTER_LFE_OUT):
		regAddr = MV_AC97_PCM_CNTR_LFE_DATA_REG;
		break;
	case (AC97_MODEM_OUT):
	case (AC97_MODEM_IN):
		regAddr = MV_AC97_MODEM_DATA_REG;
		break;
	default:
		return MV_ERROR;
	}

	*addr = regAddr;

	return MV_OK;
}


/*******************************************************************************
* mvAc97CodecAccessRegGet 
*
* DESCRIPTION:
*       A helper function to calculate the address of the codec accress register.
*
* INPUT:
*       codecId - The Codec ID to read / write the register for.
*	regAddr	- The Codec register address to read.
*
* OUTPUT:
*       accReg	- The calculated access register.
*
* RETURN:
*	None.
*******************************************************************************/
static void mvAc97CodecAccessRegGet(MV_AC97_CODEC_ID codecId, MV_U32 regAddr,
				    MV_U32 *accReg)
{
	MV_U32 temp;

	if(regAddr != AC97_CODEC_GPIO_STATUS_REG) {
		switch (codecId) {
		case (AC97_PRIMARY_CODEC):
			temp = MV_AC97_PRI_AUDIO_CODEC_REG;
			break;
		case (AC97_SECONDARY_CODEC_01):
			temp = MV_AC97_SEC01_AUDIO_CODEC_REG;
			break;
		case (AC97_SECONDARY_CODEC_10):
			temp = MV_AC97_SEC10_AUDIO_CODEC_REG;
			break;
		case (AC97_SECONDARY_CODEC_11):
			temp = MV_AC97_SEC11_AUDIO_CODEC_REG;
			break;
		default:
			return;
		}
	} else {
		switch (codecId) {
		case (AC97_PRIMARY_CODEC):
			temp = MV_AC97_PRI_MODEM_CODEC_REG;
			break;
		case (AC97_SECONDARY_CODEC_01):
			temp = MV_AC97_SEC01_MODEM_CODEC_REG;
			break;
		case (AC97_SECONDARY_CODEC_10):
			temp = MV_AC97_SEC10_MODEM_CODEC_REG;
			break;
		case (AC97_SECONDARY_CODEC_11):
			temp = MV_AC97_SEC11_MODEM_CODEC_REG;
			break;
		default:
			return;
		}
	}

	*accReg = temp + (regAddr << 1);
	return;
}

/*******************************************************************************
* mvAc97CodecRegRead 
*
* DESCRIPTION:
*       Read an attached AC'97 codec register.
*
* INPUT:
*       codecId - The Codec ID to read the register for.
*	regAddr	- The Codec register address to read.
*
* OUTPUT:
*       data	- The Codec register value as returned by the Codec.
*
* RETURN:
*       MV_OK	- On successfull init,
*	MV_TIMEOUT - On read timeout.
*	MV_FAIL	- Otherwise.
*******************************************************************************/
MV_STATUS mvAc97CodecRegRead(MV_AC97_CODEC_ID codecId, MV_U16 regAddr,MV_U16 *data)
{
        MV_U32 	val;
	MV_U32	status = MV_OK;
	MV_U32 	accAddr;
	MV_U32	retries = AC97_READ_WRITE_MAX_RETRY;

	if(data == NULL)
		return MV_BAD_PARAM;

	/* Get the access register.			*/
	mvAc97CodecAccessRegGet(codecId,regAddr,&accAddr);

	if(regAddr == AC97_CODEC_GPIO_STATUS_REG) {
		/* Read from the AC'97 controller cache.*/
		goto done;
	}

	/* Clear the command / status done bits.	*/
	MV_REG_BIT_SET(MV_AC97_GLOBAL_STATUS_REG,
		       (AC97_GLB_STATUS_DONE_MASK | AC97_GLB_CMND_DONE_MASK));

	/* Issue a dummy read.		*/
	val = MV_REG_READ(accAddr);

	/* Wait till the status done bit is valid.	*/
	while(retries > 0)
	{
		retries--;
		val = MV_REG_READ(MV_AC97_GLOBAL_STATUS_REG);
		if(val & AC97_GLB_STATUS_DONE_MASK)
			break;
	}

	/* Check for a timeout.		*/
	if(retries == 0)
	{
		status = MV_TIMEOUT;
		goto done;
	}

	/* Clear the command / status done bits.	*/
	MV_REG_BIT_SET(MV_AC97_GLOBAL_STATUS_REG,
		       (AC97_GLB_STATUS_DONE_MASK | AC97_GLB_CMND_DONE_MASK));

done:
	if(status == MV_OK)
	{
		/* Now read the "real" data.	*/	
		val = MV_REG_READ(accAddr);
		*data = (MV_U16)(val & 0xFFFF);
	}
	return status;
}


/*******************************************************************************
* mvAc97CodecRegWrite
*
* DESCRIPTION:
*       Write to an attached AC'97 codec register.
*
* INPUT:
*       codecId - The Codec ID to write the register for.
*	regAddr	- The Codec register address to write to.
*	data	- Data to be written.
*
* OUTPUT:
*	None.
*
* RETURN:
*       MV_OK	- On successfull init,
*	MV_FAIL	- Otherwise.
*******************************************************************************/
MV_STATUS mvAc97CodecRegWrite(MV_AC97_CODEC_ID codecId, MV_U16 regAddr,MV_U16 data)
{
	MV_U32 	val;
	MV_U32	status = MV_OK;
	MV_U32 	accAddr;
	MV_U32	retries = AC97_READ_WRITE_MAX_RETRY;

	/* Get the access register.			*/
	mvAc97CodecAccessRegGet(codecId,regAddr,&accAddr);

	/* Clear the command / status done bits.	*/
	MV_REG_BIT_SET(MV_AC97_GLOBAL_STATUS_REG,
		       (AC97_GLB_STATUS_DONE_MASK | AC97_GLB_CMND_DONE_MASK));

	/* Issue the write operation.			*/
	val = (MV_U32)data;
	MV_REG_WRITE(accAddr,val);

	/* Wait till the status done bit is valid.	*/
	while(retries > 0)
	{
		retries--;
		val = MV_REG_READ(MV_AC97_GLOBAL_STATUS_REG);
		if(val & AC97_GLB_CMND_DONE_MASK)
			break;
	}

	/* Check for a timeout.				*/
	if(retries == 0)
		status = MV_TIMEOUT;

	return status;
}


