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
#ifndef __INCmvCamSensorh
#define __INCmvCamSensorh
/*
 * Generic API for Camera Sensor devices.
 */ 
    
/* includes */
#ifdef __cplusplus
    extern "C" { 
#endif

/* those function provided by the upper layer, used to read/write one byte registers */ 
typedef MV_STATUS (* mvSensorI2CRead_t) (MV_VOID *I2CClient, MV_U8 addr, MV_U8 *val);
typedef MV_STATUS (* mvSensorI2CWrite_t) (MV_VOID *I2CClient, MV_U8 addr, MV_U8 val);

/*
 * MV_CAM_SENSOR: Camera Sensor data structure handler
 *
 */
typedef struct {
     /* These field must be initialized by the upper layer before calling 
	mvCamSensorInit */
     MV_VOID *I2CClient; /* handler for i2c client, sensor driver pass it
			    to the mvSensorI2CRead/mvSensorI2CWrite functions */
//     mvSensorI2CRead_t mvSensorI2CRead; 
//     mvSensorI2CWrite_t mvSensorI2CWrite;
     MV_U32	inputClk; /* input clk rate in MHz*/

     /* internal fields, upper layer should not access these fields*/
     MV_VOID	*pInfo; /* private info*/
} MV_CAM_SENSOR;

typedef enum
{
     MV_IMAGE_PARAM_RATE,
     MV_IMAGE_PARAM_BRIGHTNESS,
     MV_IMAGE_PARAM_CONTRAST,
     MV_IMAGE_PARAM_SATURATION,
     MV_IMAGE_PARAM_HUE,
     MV_IMAGE_PARAM_VFLIP,
     MV_IMAGE_PARAM_HFLIP,
} MV_IMAGE_PARAM;

/*******************************************************************************
* mvCamSensorInit - Initialize the Camera Sensor
*
* DESCRIPTION:
*	this function does the following:
* 1. Initializes the internal fields of the data stucture.
* 2. initialize the sensor and loads default values.
* 3. checks if the senser id match the support device
*
* INPUT:
*       Pointer to Camera Sensor Data structure, this sturcture must be allocated
*       by callee.
* OUTPUT:
*		None
* RETURN:
*    MV_BAD_PTR: if data structure not allocated or have bad external field   
*    MV_FAIL: failed to allocate memory 
*    MV_NOT_FOUND: if sensor id doesn't match
*    MV_ERROR: I2C failure transaction failure or if the senser reports failure
*    MV_OK otherwise	
*******************************************************************************/
MV_STATUS mvCamSensorInit(MV_CAM_SENSOR *pCamSensor);


/*******************************************************************************
* mvCamSensorReset - request from Camera Sensor to reset itself
*
* DESCRIPTION:
*
* INPUT:
*       Pointer to Camera Sensor Data structure.
*
* OUTPUT:
*	None
* RETURN:
*    MV_BAD_PTR: if Sensor data structure is invalid 
*    MV_ERROR: I2C failure
*    MV_ERROR: if senser reports failure
*    MV_OK otherwise	
*******************************************************************************/
MV_STATUS mvCamSensorReset(MV_CAM_SENSOR *pCamSensor);

/*******************************************************************************
* mvCamSensorImageFormatSet - Set Image format (pixel format and resolution)
*
* DESCRIPTION:
*	Configure the Sensor with pixel format and resolution
* INPUT:
*       Pointer to Camera Sensor Data structure.
*	Pointer to requested MV_IMAGE_FORMAT
*
* OUTPUT:
*
* RETURN:
*    MV_BAD_PTR: if Sensor data structure is invalid 
*    MV_ERROR: I2C failure
*    MV_NOT_SUPPORTED: requested format not supported
*    MV_OK otherwise
*******************************************************************************/
MV_STATUS mvCamSensorImageFormatSet(MV_CAM_SENSOR *pCamSensor, MV_IMAGE_FORMAT *pFormat);

/*******************************************************************************
* mvCamSensorImageFormatGet - Get Image format (pixel format and resolution)
*
* DESCRIPTION:
*	get the current image format pixel format and resolution
* INPUT:
*       Pointer to Camera Sensor Data structure.
*
* OUTPUT:
*	Pointer to MV_IMAGE_FORMAT
*
* RETURN:
*    MV_BAD_PTR: if Sensor data structure is invalid 
*    MV_ERROR: I2C failure
*    MV_OK otherwise
*******************************************************************************/
MV_STATUS mvCamSensorImageFormatGet(MV_CAM_SENSOR *pCamSensor, MV_IMAGE_FORMAT *pFormat);

/*******************************************************************************
* mvCamSensorPixelSizeGet - Get the Pixel size in bytes
*
* DESCRIPTION:
*
* INPUT:
*       Pointer to Camera Sensor Data structure.
*
* OUTPUT:
*	pixelSize - Pointer to store the pixel size
*
* RETURN:
*    MV_BAD_PTR: if data structure not allocated or invalid output pointers
*    MV_OK otherwise
*******************************************************************************/
MV_STATUS mvCamSensorPixelSizeGet(MV_CAM_SENSOR *pCamSensor, MV_U32 *pixelSize);

/*******************************************************************************
* mvCamSensorTestPatternConfig - test pattern configuration
*
* DESCRIPTION:
*	Enable/Disable given test pattern
* INPUT:
*       Pointer to Camera Sensor Data structure.
*	indext of the test pattern to config.
*	0 - Overlay Color Bar
*	1 - DSP Color Bar 1
*	2 - Bit Shift
*	enable/disable flag.
*
* OUTPUT:
*	None
*
* RETURN:
*    MV_BAD_PTR: if data structure not allocated
*    MV_BAD_PARAM: if pattern not supported
*    MV_ERROR: I2C failure
*    MV_OK otherwise
*******************************************************************************/
MV_STATUS mvCamSensorTestPatternConfig(MV_CAM_SENSOR *pCamSensor, MV_U32 pattern,
				       MV_BOOL enable);

/*******************************************************************************
* mvCamSensorImageParamRangeGet - Generic function for getting image parameter range
*c
* DESCRIPTION:
*
* INPUT:
*       Pointer to Camera Sensor Data structure.
*       image param type
*
* OUTPUT:
*	param range min value
*	param range max value
*	param range step
*
* RETURN:
*    MV_ERROR: I2C failure
*    MV_NOT_SUPPORTED: requested parameter not supported
*    MV_OK otherwise
*******************************************************************************/
MV_STATUS mvCamSensorImageParamRangeGet(MV_CAM_SENSOR *pCamSensor, MV_IMAGE_PARAM param,
					MV_U32 *min, MV_U32 *max, MV_U32 *step);

/*******************************************************************************
* mvCamSensorImageParamSet - Generic function for setting image parameter
*
* DESCRIPTION:
*
* INPUT:
*       Pointer to Camera Sensor Data structure.
*       image param type
*	param value:
*	MV_IMAGE_PARAM_RATE: value is the frames per second rate
*       MV_IMAGE_PARAM_BRIGHTNESS: value is the image brightness
*       MV_IMAGE_PARAM_CONTRAST
*       MV_IMAGE_PARAM_SATURATION
*       MV_IMAGE_PARAM_HUE
*       MV_IMAGE_PARAM_VFLIP: 1 enable vflop. 0 disable
*       MV_IMAGE_PARAM_HFLIP: 1 enable hflop. 0 disable
*
* OUTPUT:
*
* RETURN:
*    MV_BAD_PTR: if data structure not allocated
*    MV_NOT_SUPPORTED: requested parameter not supported
*    MV_BAD_PARAM: invalid parameter
*    MV_BAD_VALUE: invalid parameter value
*    MV_ERROR: I2C failure
*    MV_OK otherwise
*******************************************************************************/
MV_STATUS mvCamSensorImageParamSet(MV_CAM_SENSOR *pCamSensor, MV_IMAGE_PARAM param,
				   MV_U32 value);

/*******************************************************************************
* mvCamSensorImageParamGet - Generic function for getting image parameter
*
* DESCRIPTION:
*
* INPUT:
*       Pointer to Camera Sensor Data structure.
*       image param type
*
* OUTPUT:
*	param value
* RETURN:
*    MV_ERROR: I2C failure
*    MV_NOT_SUPPORTED: requested parameter not supported
*    MV_OK otherwise
*******************************************************************************/
MV_STATUS mvCamSensorImageParamGet(MV_CAM_SENSOR *pCamSensor, MV_IMAGE_PARAM param,
				   MV_U32 *value);

#ifdef __cplusplus
    }
#endif

#endif /* __INCmvCamSensorh*/

