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
#ifndef __INCMVAc97H
#define __INCMVAc97H

#ifdef __cplusplus
extern "C" {
#endif

#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "mvSysAc97Config.h"
/********************************/
/* Enums and structures 	*/
/********************************/

typedef enum {
	AC97_PCM_OUT,
	AC97_PCM_IN,
	AC97_MIC_IN,
	AC97_PCM_SURROUND_OUT,
	AC97_PCM_CENTER_LFE_OUT,
	AC97_MODEM_OUT,
	AC97_MODEM_IN
}MV_AC97_FIFO_TYPE;


typedef enum {
	AC97_FIFO_ERROR_INT,
	AC97_FIFO_SERVICE_REQ_INT
}MV_AC97_FIFO_INT_TYPE;


typedef enum {
	AC97_PRIMARY_CODEC,
	AC97_SECONDARY_CODEC_01,
	AC97_SECONDARY_CODEC_10,
	AC97_SECONDARY_CODEC_11
}MV_AC97_CODEC_ID;


/* AC'97 Global interrupt types.	*/
#define AC97_GPIO_INT			0x1
#define AC97_PRI_CODEC_RESUME_INT	0x10
#define AC97_SEC01_CODEC_RESUME_INT	0x20
#define AC97_SEC10_CODEC_RESUME_INT	0x40
#define AC97_SEC11_CODEC_RESUME_INT	0x80
#define AC97_PRI_CODEC_READY_INT	0x100
#define AC97_SEC01_CODEC_READY_INT	0x200
#define AC97_SEC10_CODEC_READY_INT	0x400
#define AC97_SEC11_CODEC_READY_INT	0x800
#define AC97_STATUS_DONE_INT		0x40000
#define AC97_COMMAND_DONE_INT		0x80000


/********************************/
/* Functions API 		*/
/********************************/

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
MV_STATUS mvAc97Init(MV_BOOL pdmaModeEn);


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
MV_STATUS mvAc97Reset(MV_VOID);


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
			      MV_BOOL enable);


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
MV_STATUS mvAc97GlobalIntEnable(MV_U32 intMask, MV_BOOL enable);


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
			     MV_U16 length);


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
			      MV_U16 length);


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
MV_STATUS mvAc97PdmaFifoAccessRegAddr(MV_AC97_FIFO_TYPE fifoType, MV_U32 *addr);


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
MV_STATUS mvAc97CodecRegRead(MV_AC97_CODEC_ID codecId, MV_U16 regAddr,MV_U16 *data);


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
MV_STATUS mvAc97CodecRegWrite(MV_AC97_CODEC_ID codecId, MV_U16 regAddr,MV_U16 data);


#ifdef __cplusplus
}
#endif


#endif /* __INCMVAc97H */
