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
/*
 * Cafe Camera Interface Controller HAL driver
 */
/* includes */
#include "mvCommon.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "mvSysCamConfig.h"
#include "mvCamControllerRegs.h"
#include "mvCamController.h"

static MV_CAMERA_HAL_DATA	cameraHalData;

static MV_STATUS configClockContol(MV_CAM_CONTROLLER *pCam)
{
     MV_U32 tclock = cameraHalData.tclk;
     MV_U32 reg = MV_REG_READ(pCam->base + MV_CAM_CLKCTRL);

     reg &= ~0xFFFFFFF;

     switch (tclock)
     {
     case MV_BOARD_TCLK_166MHZ:
	  reg |= 0x20000003;
	  break;
     default:
	  mvOsPrintf("configClockContol: Not supported core clock %d\n",tclock);
	  return MV_ERROR;
     }
     MV_REG_WRITE(pCam->base + MV_CAM_CLKCTRL, reg);
     return MV_OK;
}

/*---------------------------API----------------------------------------------*/
/*******************************************************************************
* mvCamCtrlInit - Initialize the Camera Controller
*
* DESCRIPTION:
*	this function does the following:
* 1. Initializes the internal fields of the data stucture.
* 2. initialize the controller and loads default values.
* 3. Power up the controller
* 4. The CMOS sensor will not be powered up by this function
*
* INPUT:
*       Pointer to Camera Controller Data structure, this sturcture must 
* 	be allocated by callee.
* OUTPUT:
*		None
* RETURN:
*    MV_BAD_PTR: if data structure not allocated or have bad external field
*    MV_ERROR: failed to initialize the controller
*    MV_OK otherwise	
*******************************************************************************/
MV_STATUS mvCamCtrlInit(MV_CAM_CONTROLLER *pCam, MV_CAMERA_HAL_DATA *halData)
{
     if(!pCam || !pCam->mvCamFrameCompleted)
     {
	  mvOsPrintf("mvCamCtrlInit: Bad Input\n");
	  return MV_BAD_PTR;
     }

     mvOsMemcpy(&cameraHalData, halData, sizeof(MV_CAMERA_HAL_DATA));

     pCam->base = MV_CAFE_REGS_OFFSET;

     /* Confiure CCIC in normal mode */
     MV_REG_BIT_RESET(pCam->base + MV_CAM_CONTROL1, MV_CAM_C1_PWRDWN);

     /* disable CCIC */
     MV_REG_BIT_RESET(pCam->base + MV_CAM_CONTROL0, MV_CAM_C0_ENABLE);

     /* unmask interrupts */
     MV_REG_WRITE(pCam->base + MV_CAM_IRQ_MASK, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT16|BIT17|BIT18);

//     MV_REG_WRITE(pCam->base + MV_CAM_IRQ_MASK, 0);

     return configClockContol(pCam);
}

/*******************************************************************************
* mvCamCtrlPowerUp - Power up the Camera Controller and the Sensor
*
* DESCRIPTION:
*	Power up the camera controller and the sensor.
*	This function doesn't wait for the devices to be power up, this should
*	be done by the upper layer.
*
* INPUT:
*       Pointer to Camera Cafe Controller Data structure
*
* OUTPUT:
*	None
* RETURN:
*    MV_BAD_PTR: if data structure not allocated or have bad external field   
*    MV_OK otherwise	
*******************************************************************************/
MV_STATUS mvCamCtrlPowerUp(MV_CAM_CONTROLLER *pCam)
{
     if(!pCam || !pCam->mvCamFrameCompleted)
     {
	  mvOsPrintf("mvCamCtrlPowerUp: Bad Input\n");
	  return MV_BAD_PTR;
     }

     MV_REG_BIT_RESET(pCam->base + MV_CAM_CONTROL1, MV_CAM_C1_PWRDWN);

     /* put external CMOS sensor in reset and power-down mode*/
     MV_REG_WRITE(pCam->base + MV_CAM_GPR,
		  MV_CAM_GPR_CTL1PADEN | MV_CAM_GPR_CTL0PADEN |
		  MV_CAM_GPR_CTL1);

     mvOsDelay(10);

     /* Release external CMOS sensor  reset and power-down*/
     MV_REG_WRITE(pCam->base + MV_CAM_GPR,
		  MV_CAM_GPR_CTL1PADEN | MV_CAM_GPR_CTL0PADEN |
		  MV_CAM_GPR_CTL1);
     return MV_OK;
}
/*******************************************************************************
* mvCamCtrlPowerDown - Power down the Camera Controller
*
* DESCRIPTION:
*	Power down the camera controller and the sensor
*
* INPUT:
*       Pointer to Camera Cafe Controller Data structure
*
* OUTPUT:
*		None
* RETURN:
*    MV_BAD_PTR: if data structure not allocated or have bad external field   
*    MV_OK otherwise	
*******************************************************************************/
MV_STATUS mvCamCtrlPowerDown(MV_CAM_CONTROLLER *pCam)
{
     if(!pCam || !pCam->mvCamFrameCompleted)
     {
	  mvOsPrintf("mvCamCtrlPowerDown: Bad Input\n");
	  return MV_BAD_PTR;
     }

     /* put external CMOS sensor in power-down mode*/
     MV_REG_WRITE(pCam->base + MV_CAM_GPR,
		  MV_CAM_GPR_CTL1PADEN | MV_CAM_GPR_CTL0PADEN |
		  MV_CAM_GPR_CTL1);
     
     MV_REG_BIT_SET(pCam->base + MV_CAM_CONTROL1, MV_CAM_C1_PWRDWN);
     return MV_OK;
}

/*******************************************************************************
* mvCamCtrlDMABuffersSet - Set buffers to the DMA of the Camera Controller
*
* DESCRIPTION:
* 
* INPUT:
*       Pointer to Camera Cafe Controller Data structure
*	array of DMA buffer address
*       number of buffers
*
* OUTPUT:
*		None
* RETURN:
*    MV_BAD_PTR: if data structure not allocated or have bad external field   
*    MV_BAD_PARAM: if number of buffers in not 2 or 3.
*    MV_ERROR: if DMA is active	
*    MV_OK otherwise	
*******************************************************************************/
MV_STATUS mvCamCtrlDMABufferSet(MV_CAM_CONTROLLER *pCam, MV_U32 dmaAddress[], 
				MV_U32 numberOfBuffers)
{
     if(!pCam || !pCam->mvCamFrameCompleted)
     {
	  mvOsPrintf("mvCamCtrlDMABufferSet: Bad Input\n");
	  return MV_BAD_PTR;
     }

     if((numberOfBuffers != 2) && (numberOfBuffers != 3))
     {
	  mvOsPrintf("mvCamCtrlDMABufferSet: bad DMA buffers numbers %d\n",
	       numberOfBuffers);
	  return MV_BAD_PARAM;
     }
     pCam->numberOfBuffers = numberOfBuffers;

     pCam->dmaBuffer[0] = dmaAddress[0];
     pCam->dmaBuffer[1] = dmaAddress[1];
     if(numberOfBuffers == 3)
	  pCam->dmaBuffer[2] = dmaAddress[2];

     MV_REG_WRITE(pCam->base + MV_CAM_Y0_BASE,  pCam->dmaBuffer[0]); 
     MV_REG_WRITE(pCam->base + MV_CAM_Y1_BASE,  pCam->dmaBuffer[1]); 
     if(numberOfBuffers == 3)
	  MV_REG_WRITE(pCam->base + MV_CAM_Y2_BASE,  pCam->dmaBuffer[2]); 

     return MV_OK;
}

/*******************************************************************************
* mvCamCtrlSetDMABurstSizeSet - Configure the DMA burst size of the Camera Controller
*
* DESCRIPTION:
* 
* INPUT:
*       Pointer to Camera Cafe Controller Data structure
*       burst size 
*
* OUTPUT:
*		None
* RETURN:
*    MV_BAD_PTR: if data structure not allocated or have bad external field   
*    MV_BAD_PARAM: bad burst size.
*    MV_BUSY: if DMA is active	
*    MV_OK otherwise	
*******************************************************************************/
MV_STATUS mvCamCtrlDMABurstSizeSet(MV_CAM_CONTROLLER *pCam, MV_CAM_BURST_SIZE size)
{
     MV_U32	burstSel;
     MV_U32	reg;
     if(!pCam || !pCam->mvCamFrameCompleted)
     {
	  mvOsPrintf("mvCamCtrlDMABurstSizeSet: Bad Input\n");
	  return MV_BAD_PTR;
     }
     
     switch (size)
     {
     case MV_CAM_BURST_SIZE_16:
 	  burstSel = 1;
	  break;
     case MV_CAM_BURST_SIZE_32:
 	  burstSel = 0;
	  break;
     case MV_CAM_BURST_SIZE_64:
 	  burstSel = 2;
	  break;
     default:
	  mvOsPrintf("mvCamCtrlDMABurstSizeSet: Bad size\n");
	  return MV_BAD_PARAM;
     }
     
     reg = MV_REG_READ(pCam->base + MV_CAM_CONTROL1); 
     reg &= ~MV_CAM_C1_DMABRSTSEL_MASK;
     reg |= burstSel;
     MV_REG_WRITE(pCam->base + MV_CAM_CONTROL1, reg);
     return MV_OK;
}

/*******************************************************************************
* mvCamCtrlDMAStart - enabel the Camera DMA
*
* DESCRIPTION:
* 
* INPUT:
*       Pointer to Camera Cafe Controller Data structure
*
* OUTPUT:
*		None
* RETURN:
*    MV_ERROR: if the controller was not initialized properly
*    MV_BUSY: if DMA is already active	
*    MV_OK otherwise	
*******************************************************************************/
MV_STATUS mvCamCtrlDMAStart(MV_CAM_CONTROLLER *pCam)
{
     if(!pCam || !pCam->mvCamFrameCompleted)
     {
	  mvOsPrintf("mvCamDMACtrl: Bad Input\n");
	  return MV_BAD_PTR;
     }

     MV_REG_BIT_SET(pCam->base + MV_CAM_CONTROL0, MV_CAM_C0_ENABLE);
     return MV_OK;
}

/*******************************************************************************
* mvCamCtrlDMAStop - disabel the Camera DMA
*
* DESCRIPTION:
* 
* INPUT:
*       Pointer to Camera Cafe Controller Data structure
*
* OUTPUT:
*		None
* RETURN:
*    MV_ERROR: if the controller was not initialized properly
*    MV_OK otherwise	
*******************************************************************************/
MV_STATUS mvCamCtrlDMAStop(MV_CAM_CONTROLLER *pCam)
{
     if(!pCam || !pCam->mvCamFrameCompleted)
     {
	  mvOsPrintf("mvCamCtrlDMAStop: Bad Input\n");
	  return MV_BAD_PTR;
     }
     
     MV_REG_BIT_RESET(pCam->base + MV_CAM_CONTROL0, MV_CAM_C0_ENABLE);
     return MV_OK;
}
/*******************************************************************************
* mvCamCtrlImageFormatSet - Set Image Input pixel format
*
* DESCRIPTION:
*	Configure the input pixel format
* INPUT:
*       Pointer to Camera Controller Data structure.
*	Pointer to requested input MV_PIX_FORMAT
*	Pointer to requested output MV_PIX_FORMAT
*
* OUTPUT:
*
* RETURN:
*    MV_NOT_SUPPORTED: requested format not supported
*    MV_ERROR: if the controller was not initialized properly
*    MV_BUSY: if DMA is active	
*    MV_OK otherwise
*******************************************************************************/
MV_STATUS mvCamCtrlImageFormatSet(MV_CAM_CONTROLLER *pCam, MV_PIXEL_FORMAT *inFormat,
				  MV_PIXEL_FORMAT *outFormat)
{
     MV_U32 reg;
     if(!pCam || !pCam->mvCamFrameCompleted || !inFormat || !outFormat)
     {
	  mvOsPrintf("mvCamCtrlImageFormatSet: Bad Input\n");
	  return MV_BAD_PTR;
     }
     
     if(inFormat->id != outFormat->id)
     {
/*	  if((inFormat->id != MV_PIX_FORMAT_ID_YUV422) && 
	  (outFormat->id != MV_PIX_FORMAT_ID_YUV420))*/
	       mvOsPrintf("mvCamCtrlImageFormatSet: in/out formats doesn't match\n");
	  return MV_NOT_SUPPORTED;
     }
	  

     reg = MV_REG_READ(pCam->base + MV_CAM_CONTROL0);
     reg &= ~0x3FFFC;

     switch(inFormat->id)
     {
     case MV_PIX_FORMAT_ID_RGB444:
	  reg |= MV_CAM_C0_RGB444_ORD | MV_CAM_C0_RGB444 | MV_CAM_C0_RGB;
	  break;
     case MV_PIX_FORMAT_ID_RGB555:
	  reg |= MV_CAM_C0_RGB555_ORD | MV_CAM_C0_RGB555 | MV_CAM_C0_RGB;
	  break;
     case MV_PIX_FORMAT_ID_RGB565:
	  reg |= MV_CAM_C0_RGB565_ORD | MV_CAM_C0_RGB565 | MV_CAM_C0_RGB;	       
	  break;
     case MV_PIX_FORMAT_ID_RAW_BAYER:
	  reg |= MV_CAM_C0_RAW;	       
	  break;
     case MV_PIX_FORMAT_ID_YUV422:
/*	  if(outFormat->id == MV_PIX_FORMAT_ID_YUV420)
	       reg |= MV_CAM_C0_YUV422_ENDIANESS | MV_CAM_C0_YUV_FMT_PACKED |
		    MV_CAM_C0_YUV;
		    else*/
	  reg |= MV_CAM_C0_YUV422_ENDIANESS | MV_CAM_C0_YUV_FMT_PACKED |
	       MV_CAM_C0_YUV;
	  break;
     default:
	  mvOsPrintf("mvCamCtrlImageFormatSet: not supported input format\n");
	  return MV_NOT_SUPPORTED;
     }
     MV_REG_WRITE(pCam->base + MV_CAM_CONTROL0, reg | BIT15);
     return MV_OK;
}

/*******************************************************************************
* mvCamCtrlImageSizeSet - Set size if the image frame
*
* DESCRIPTION:
*
* INPUT:
*       Pointer to Camera Controller Data structure.
*	vsize - height 
*	hsize - wedth
*
* OUTPUT:
*
* RETURN:
*    MV_NOT_SUPPORTED: requested format not supported
*    MV_ERROR: if the controller was not initialized properly
*    MV_BUSY: if DMA is active	
*    MV_OK otherwise
*******************************************************************************/
MV_STATUS mvCamCtrlImageSizeSet(MV_CAM_CONTROLLER *pCam, MV_U32 vsize, MV_U32 hsize)
{
     MV_U32 reg;
     if(!pCam || !pCam->mvCamFrameCompleted)
     {
	  mvOsPrintf("mvCamCtrlImageSizeSet: Bad Input\n");
	  return MV_BAD_PTR;
     }

     
     reg = ((vsize << MV_CAM_IMAGE_SIZE_V_OFFSET) & MV_CAM_IMAGE_SIZE_V_MASK) |
	  (hsize & MV_CAM_IMAGE_SIZE_H_MASK);
     
     MV_REG_WRITE(pCam->base + MV_CAM_IMAGE_SIZE, reg);
     MV_REG_WRITE(pCam->base + MV_CAM_IMAGE_OFFSET, 0);

     MV_REG_WRITE(pCam->base + MV_CAM_IMAGE_PITCH, hsize);
     return MV_OK;
}

/*******************************************************************************
* mvCamCtrlSetParam - Generic function for setting controller parameter
*
* DESCRIPTION:
*
* INPUT:
*       Pointer to Camera controller Data structure.
*       param type
*	param value:
*	MV_CICC_PARAM_ 
*
*
* OUTPUT:
*
* RETURN:
*    MV_ERROR: I2C failure
*    MV_NOT_SUPPORTED: requested parameter not supported
*    MV_BUSY: if DMA is active	
*    MV_OK otherwise
*******************************************************************************/
/*MV_VOID mvCamCtrlSetImageParam(MV_CAM_SENSOR *, MV_IMAGE_PARAM param,MV_U32 value);*/



/*******************************************************************************
* mvCamCtrlISR -Interrupt service routine
*
* DESCRIPTION:
*
* INPUT:
*       Pointer to Camera Controller Data structure.
*
* OUTPUT:
*
* RETURN:
*    MV_ERROR: if the controller was not initialized properly
*    MV_NOT_FOUND: no interrupt found
*    MV_OK otherwise
*******************************************************************************/
MV_STATUS mvCamCtrlISR(MV_CAM_CONTROLLER *pCam)
{
     MV_U32 status;
     int i;

     if(!pCam || !pCam->mvCamFrameCompleted)
     {
	  mvOsPrintf("mvCamCtrlISR: Bad Input\n");
	  return MV_BAD_PTR;
     }
     
     status = MV_REG_READ(pCam->base + MV_CAM_IRQ_STATUS);

     if((status & 0x7) == 0)
	  return MV_NOT_FOUND;

     if(status & MV_CAM_IRQ_FIFO_FULL)
     {
	  mvOsPrintf("mvCamCtrlISR: FIFO FULL Error\n");
	  return MV_ERROR;
     }

     if(status & MV_CAM_IRQ_TWSI_ERROR)
     {
	  mvOsPrintf("mvCamCtrlISR: TWSI Error\n");
	  return MV_ERROR;
     }

     MV_REG_WRITE(pCam->base + MV_CAM_IRQ_STATUS, 0xFFFFFFFF);
     for ( i = 0; i < 3; i++)
     {
	  if(status & MV_CAM_IRQ_EOF(i))
	       pCam->mvCamFrameCompleted(pCam->mvCamFrameCompletionHandler, i);
     }
     return MV_OK;
}

