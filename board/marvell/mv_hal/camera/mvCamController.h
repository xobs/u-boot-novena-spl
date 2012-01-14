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
#ifndef __INCmvCamControllerh
#define __INCmvCamControllerh

#ifdef __cplusplus
    extern "C" { 
#endif

#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "mvSysCamConfig.h"
/*
 * API for the Camera Interface Controller HAL driver
 */

typedef struct {
	MV_U32		tclk;
}MV_CAMERA_HAL_DATA;

/* callback function to acknowledge frame completion, takes buffer index */ 
    typedef MV_VOID (* mvCamFrameComplete_t) (MV_VOID *, MV_U32 bufIdx);

typedef enum {
     MV_CAM_BURST_SIZE_32,
     MV_CAM_BURST_SIZE_16,
     MV_CAM_BURST_SIZE_64,
}MV_CAM_BURST_SIZE;

/*
 * MV_CAM_CONTROLLER: Camera Controller data structure handler
 *
 */
typedef struct {
     /* These field must be initialized by the upper layer before calling 
	mvCamCafeInit */
     mvCamFrameComplete_t mvCamFrameCompleted;
     MV_VOID	*mvCamFrameCompletionHandler;

     /* internal fields, upper layer should not access these fields*/
     /* must be cleared before calling the init function */
     MV_U32 base; /* camera base address*/

     MV_U32 numberOfBuffers;
     MV_U32 dmaBuffer[3];

} MV_CAM_CONTROLLER;

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
*       Pointer to Camera Cafe Controller Data structure, this sturcture must 
* 	be allocated by callee.
* OUTPUT:
*		None
* RETURN:
*    MV_BAD_PTR: if data structure not allocated or have bad external field
*    MV_ERROR: failed to initialize the controller
*    MV_OK otherwise	
*******************************************************************************/
MV_STATUS mvCamCtrlInit(MV_CAM_CONTROLLER *pCam, MV_CAMERA_HAL_DATA *halData);

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
MV_STATUS mvCamCtrlPowerUp(MV_CAM_CONTROLLER *pCam);

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
MV_STATUS mvCamCtrlPowerDown(MV_CAM_CONTROLLER *pCam);

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
				MV_U32 numberOfBuffers);

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
MV_STATUS mvCamCtrlDMABurstSizeSet(MV_CAM_CONTROLLER *pCam, MV_CAM_BURST_SIZE size);

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
MV_STATUS mvCamCtrlDMAStart(MV_CAM_CONTROLLER *pCam);


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
MV_STATUS mvCamCtrlDMAStop(MV_CAM_CONTROLLER *pCam);

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
				  MV_PIXEL_FORMAT *outFormat);

/*******************************************************************************
* mvCamCtrlImageSizeSet - Set size if the image frame
*
* DESCRIPTION:
*
* INPUT:
*       Pointer to Camera Controller Data structure.
*	vsize - height 
*	hsize - width
*
* OUTPUT:
*
* RETURN:
*    MV_NOT_SUPPORTED: requested format not supported
*    MV_ERROR: if the controller was not initialized properly
*    MV_BUSY: if DMA is active	
*    MV_OK otherwise
*******************************************************************************/
MV_STATUS mvCamCtrlImageSizeSet(MV_CAM_CONTROLLER *pCam, MV_U32 vsize, MV_U32 hsize);


/*******************************************************************************
* mvCamCafeSetParam - Generic function for setting controller parameter
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
/*MV_VOID mvCamCafeSetImageParam(MV_CAM_SENSOR *, MV_IMAGE_PARAM param,MV_U32 value);*/



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
MV_STATUS mvCamCtrlISR(MV_CAM_CONTROLLER *pCam);

/*******************************************************************************
* mvCamCtrlI2CRead - Read one byte register through the I2C agent
*
* DESCRIPTION:
*
* INPUT:
*       Pointer to Camera Controller Data structure.
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
MV_STATUS mvCamCtrlI2CRead(MV_CAM_CONTROLLER *pCam, MV_U8 id, MV_U8 addr,
			   MV_U8 *value);

/*******************************************************************************
* mvCamCtrlI2CWrite - Write one byte register through the I2C agent
*
* DESCRIPTION:
*
* INPUT:
*       Pointer to Camera Controller Data structure.
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
MV_STATUS mvCamCtrlI2CWrite(MV_CAM_CONTROLLER *pCam, MV_U8 id, MV_U8 addr, MV_U8 value);


MV_STATUS mvCamSDIOWinInit(MV_UNIT_WIN_INFO *addrWinMap);
MV_STATUS mvCamSDIOWinRead(MV_U32 winNum, MV_UNIT_WIN_INFO *pAddrDecWin);
MV_STATUS mvCamSDIOWinWrite(MV_U32 winNum, MV_UNIT_WIN_INFO *pAddrDecWin);

#ifdef __cplusplus
    }
#endif

#endif /* __INCmvCamControllerh */
