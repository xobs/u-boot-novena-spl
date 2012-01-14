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
#include "mvOs.h"
#include "mvSysCamSensor.h"
#include "mvCamSensor.h"

/* defines */
#define OV_MANUFACTURER_IDH	0x7F
#define OV_MANUFACTURER_IDL	0xA2
#define OV_7680_PROD_IDH	0x76
#define OV_7680_PROD_IDL	0x80


#define OV_REG_PIDH	0x0A
#define OV_REG_PIDL	0x0B
#define OV_REG_0C	0x0C
#define OV_REG_12	0x12
#define OV_REG_MIDH	0x1C
#define OV_REG_MIDL	0x1D
#define OV_REG_83	0x83
#define OV_REG_62	0x62
#define OV_REG_SDE	0xD5
#define  OV_SDE_BRIGHTNESS_CONTRAST (1 << 2)
#define  OV_SDE_SATURATION (1 << 1)
#define OV_REG_BRIGHTNESS 0xD6
#define OV_REG_YOFFSET	0xD8
#define OV_REG_USAT	0xDB
#define OV_REG_VSAT	0xDC
#define OV_REG_SGNSET	0xDF

#define OV_7680_RATE	30

typedef struct
{
     MV_PIXEL_FORMAT * pixFormat;
     MV_VIDEO_RESOLUTION *resolution;
}OV7680_INFO;

/* supported formats*/

MV_PIXEL_FORMAT supportedFormats[] = 
{
     MV_PIXEL_FORMAT_YUV422, /*item 0 must be the default*/
     MV_PIXEL_FORMAT_RGB444,
     MV_PIXEL_FORMAT_RGB555,
     MV_PIXEL_FORMAT_RGB565,
     MV_PIXEL_FORMAT_RAW_BAYER,
};

MV_VIDEO_RESOLUTION supportedResolutions[] =
{
     MV_VIDEO_RESOLUTION_VGA, /*item 0 must be the default*/
     MV_VIDEO_RESOLUTION_QVGA,
     MV_VIDEO_RESOLUTION_CIF,
     MV_VIDEO_RESOLUTION_QCIF,
};

typedef struct
{
     MV_U8	addr;
     MV_U8	value;
} sensorReg_t;
sensorReg_t ov7680_default_settings[] =
{
//     Initial (VGA YUV 30FPS)
//88 0f 1D
//88 fe 01
//88 f1 04
//88 f0 41

//     {0x12, 0x80},
     {0x1e, 0x91},   
     {0x14, 0xb2},// ;c2
     {0x63, 0x0b},   
     {0x64, 0x0f},   
     {0x65, 0x07},
     {0x12, 0x01},
     {0x0c, 0x06},
     {0x82, 0x9A},  
     {0x22, 0x40},
     {0x23, 0x20},
     {0x31, 0x11},
     {0x40, 0x20},// ;10
     {0x4a, 0x42},
     {0x4b, 0x42},// ;64
     {0x67, 0x50},
     {0x6b, 0x00},
     {0x6c, 0x00},
     {0x6d, 0x00},
     {0x6e, 0x00},

     {0x42, 0x4a},   
     {0x45, 0x40},   
     {0x3f, 0x46},   
     {0x48, 0x20},   
     {0x21, 0x23},   
     {0x5a, 0x10},   
     {0x5c, 0xe0},  
     {0x27, 0x33},  
     {0x4d, 0x2d},// ;1d  
     {0x42, 0x4a},
     {0x4e, 0x54},// ;IO2.8v

     {0x65, 0x87},
     {0x66, 0x02},   
     {0x67, 0x5c},

     {0x32, 0x40},
     {0x33, 0x00},
     {0x34, 0x40},
     {0x35, 0x20},
     {0x36, 0x40},
     {0x37, 0x40},
     {0x2f, 0x0c},
     {0x30, 0x10},  
     {0x38, 0x84}, 
     {0x31, 0x0e},
     
     
     {0x55, 0x86},
     {0x58, 0x83},
     {0x13, 0xef},
     {0x38, 0x84},
     {0x59, 0x0e},
     {0x5f, 0x20},
     {0x56, 0x40},
     {0x51, 0x00},
     {0x57, 0x22},
     {0x59, 0x07},
     {0x5f, 0xa3},
     {0x59, 0x08},
     {0x5f, 0x54},
     {0x52, 0x30},
     {0x56, 0x29},
     {0x53, 0x20},
     {0x54, 0x30},
     {0x59, 0x00},
     {0x5f, 0xf0},
     {0x59, 0x01},
     {0x5f, 0xF0},
     {0x59, 0x0F},
     {0x5f, 0x20},
     {0x5f, 0x00},
     {0x59, 0x10},
     {0x5f, 0x7E},
     {0x59, 0x0A},
     {0x5f, 0x80},
     {0x59, 0x0B},
     {0x5f, 0x01},
     {0x59, 0x0C},
     {0x5f, 0x07},
     {0x5f, 0x0F},
     {0x59, 0x0D},
     {0x5f, 0x20},
     {0x59, 0x09},
     {0x5f, 0x30},
     {0x59, 0x02},
     {0x5f, 0x80},
     {0x59, 0x03},
     {0x5f, 0x60},
     {0x59, 0x04},
     {0x5f, 0xF0},
     {0x59, 0x05},
     {0x5f, 0x80},
     {0x59, 0x06},
     {0x5f, 0x04},
     {0x59, 0x26},
     {0x59, 0x0b},
     {0x5f, 0x31},
     {0x55, 0xa8},
     
     {0x27, 0xb3},
     {0x40, 0x23},
     {0x4d, 0x2d},//1d
     
     {0x12, 0x00},//YUV
     {0x0c, 0x16},
     {0x82, 0x98},
     {0x3e, 0x30},
     {0x81, 0x06},
     
     
     {0xb7, 0x98},
     {0xb8, 0x98},
     {0xb9, 0x00},
     {0xba, 0x28},
     {0xbb, 0x70},
     {0xbc, 0x98},
     {0xbd, 0x5a},
     {0xbe, 0xb0},
     {0xbf, 0x9d},
     {0xc0, 0x13},
     {0xc1, 0x16},
     {0xc2, 0x7b},
     {0xc3, 0x91},
     {0xc4, 0x1e},
     {0xc5, 0x9d},
     {0xc6, 0x9a},
     {0xc7, 0x03},
     {0xc8, 0x2e},
     {0xc9, 0x91},
     {0xca, 0xbf},
     {0xcb, 0x1e},
     
     {0xa0, 0x0e},
     {0xa1, 0x1a},
     {0xa2, 0x31},
     {0xa3, 0x5a},
     {0xa4, 0x69},
     {0xa5, 0x75},
     {0xa6, 0x7e},
     {0xa7, 0x88},
     {0xa8, 0x8f},
     {0xa9, 0x96},
     {0xaa, 0xa3},
     {0xab, 0xaf},
     {0xac, 0xc4},
     {0xad, 0xd7},
     {0xae, 0xe8},
     {0xaf, 0x20},
     
     {0x89, 0x5c},
     {0x8a, 0x11},
     {0x8b, 0x92},
     {0x8c, 0x11},
     {0x8d, 0x52},
     {0x96, 0xff},
     {0x97, 0x00},
     {0x9c, 0xf0},
     {0x9d, 0xf0},
     {0x9e, 0xf0},
     
     {0xb2, 0x06},
     {0xb3, 0x03},
     {0xb4, 0x05},
     {0xb5, 0x04},
     {0xb6, 0x02},//;03 

     {0xd5, 0x02},
     {0xdb, 0x40},
     {0xdc, 0x40},
     
     {0x11, 0x00},//;30fps
     {0x24, 0x6c},
     {0x25, 0x62},
     {0x26, 0xb3},
     
     {0x5a, 0x14},
     {0x5b, 0xe3},
     {0x5c, 0x70},
     {0x5d, 0x30},
     {0x81, 0x07},
     
     {0x84, 0x02},
     
//     ;For USB
//88 0f 1f
};

static MV_STATUS sensorRegsWrite(MV_CAM_SENSOR *pCamSensor, sensorReg_t *sensorRegs,
				 int len)
{
     int i;
     MV_STATUS status;

     for(i = 0; i < len; i++)
     {
	  status = mvSysCamSensorRegWrite(pCamSensor->I2CClient, sensorRegs[i].addr,
			  sensorRegs[i].value);
	  
	  if(status != MV_OK)
	  {
	       mvOsPrintf("sensorRegsWrite: I2C write failed. status %d\n",
			  status);
	       return status;
	  }
     }
     return MV_OK;
}

static MV_STATUS isOV7680(MV_CAM_SENSOR *pCamSensor)
{
     MV_U8	reg;
     MV_STATUS	status;

     status = mvSysCamSensorRegRead(pCamSensor->I2CClient, OV_REG_MIDH, &reg);

     if(status != MV_OK)
	  return status;
     
     if(reg != OV_MANUFACTURER_IDH)
	  return MV_NOT_FOUND;

     status = mvSysCamSensorRegRead(pCamSensor->I2CClient, OV_REG_MIDL, &reg);

     if(status != MV_OK)
	  return status;
     
     if(reg != OV_MANUFACTURER_IDL)
	  return MV_NOT_FOUND;

     status = mvSysCamSensorRegRead(pCamSensor->I2CClient, OV_REG_PIDH, &reg);

     if(status != MV_OK)
	  return status;
     
     if(reg != OV_7680_PROD_IDH)
	  return MV_NOT_FOUND;

     status = mvSysCamSensorRegRead(pCamSensor->I2CClient, OV_REG_PIDL, &reg);

     if(status != MV_OK)
	  return status;
     
     if(reg != OV_7680_PROD_IDL)
	  return MV_NOT_FOUND;

     return MV_OK;
}

static MV_STATUS  ovRGB444Set(MV_CAM_SENSOR * pCamSensor)
{
     MV_STATUS status =  mvSysCamSensorRegWrite(pCamSensor->I2CClient,
		     OV_REG_12, 0xE);
     return status;
}

static MV_STATUS  ovRGB555Set(MV_CAM_SENSOR * pCamSensor)
{
     MV_STATUS status =  mvSysCamSensorRegWrite(pCamSensor->I2CClient,
		     OV_REG_12, 0xA);
     return status;
}

static MV_STATUS  ovRGB565Set(MV_CAM_SENSOR * pCamSensor)
{
     MV_STATUS status = mvSysCamSensorRegWrite(pCamSensor->I2CClient,
		     OV_REG_12, 0x6);
     return status;
}

static MV_STATUS ovYUV422Set(MV_CAM_SENSOR * pCamSensor)
{
     MV_STATUS status =  mvSysCamSensorRegWrite(pCamSensor->I2CClient,
		     OV_REG_12, 0);
     return status;
}

static MV_STATUS ovRawBayerSet(MV_CAM_SENSOR * pCamSensor)
{
     MV_STATUS status = mvSysCamSensorRegWrite(pCamSensor->I2CClient,
		     OV_REG_12, 0x1);
     return status;
}

static MV_STATUS configPixelFormat(MV_CAM_SENSOR *pCamSensor, int formatIdx)
{
     OV7680_INFO *pInfo = (OV7680_INFO *)pCamSensor->pInfo;

     /* if the new format the same as the current format*/
     if(pInfo->pixFormat->id == supportedFormats[formatIdx].id)
	  return MV_OK;
     
     pInfo->pixFormat = supportedFormats + formatIdx;

     switch(pInfo->pixFormat->id)
     {
     case MV_PIX_FORMAT_ID_RGB444:
	  return ovRGB444Set(pCamSensor);
     case MV_PIX_FORMAT_ID_RGB555:
	  return ovRGB555Set(pCamSensor);
     case MV_PIX_FORMAT_ID_RGB565:
	  return ovRGB565Set(pCamSensor);
     case MV_PIX_FORMAT_ID_YUV422:
	  return ovYUV422Set(pCamSensor);
     case MV_PIX_FORMAT_ID_RAW_BAYER:
	  return ovRawBayerSet(pCamSensor);
     default:
	  mvOsOutput("configPixelFormat: fatal error: unkown format id (0x%x)\n",
		     pInfo->pixFormat->id);
	  return MV_NOT_SUPPORTED;
     }
     return MV_OK;
}

static MV_STATUS configResolution(MV_CAM_SENSOR * pCamSensor, int resIdx)
{
     OV7680_INFO *pInfo = (OV7680_INFO *)pCamSensor->pInfo;

     /* if the new resolution the same as the current*/
     if((pInfo->resolution->height == supportedResolutions[resIdx].height) &&
	(pInfo->resolution->width == supportedResolutions[resIdx].width))
	  return MV_OK;
     
     mvOsOutput("configResolution: fatal error: unkown resolution (%s)\n",
		supportedResolutions[resIdx].name);

     return MV_NOT_SUPPORTED;
}

static MV_STATUS resetSensor(MV_CAM_SENSOR *pCamSensor)
{
     MV_STATUS status = mvSysCamSensorRegWrite(pCamSensor->I2CClient,
		     OV_REG_12, 1 << 7);
     mvOsDelay(2);
     return status;
}
static MV_U32 getBitsPerPixel(MV_CAM_SENSOR *pCamSensor)
{
     OV7680_INFO *pInfo = (OV7680_INFO *)pCamSensor->pInfo;

     switch(pInfo->pixFormat->id)
     {
     case MV_PIX_FORMAT_ID_RAW_BAYER:
	  return 1;
     default:
	  return 2;
     }
     /*never reached*/
     return 0;
}

static MV_STATUS enableSDEFeature(MV_CAM_SENSOR *pCamSensor, MV_U8 OV_SDE_BIT)
{
     MV_U8 reg;
     MV_STATUS status;
     status = mvSysCamSensorRegRead(pCamSensor->I2CClient, OV_REG_SDE, &reg);
     if(status != MV_OK)
	  return MV_ERROR;
     
     reg |= OV_SDE_BIT;
          
     return mvSysCamSensorRegWrite(pCamSensor->I2CClient, OV_REG_SDE, reg);
}
/*----------------------------------API---------------------------------------*/

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
MV_STATUS mvCamSensorInit(MV_CAM_SENSOR *pCamSensor)
{
     OV7680_INFO *pInfo;
     mvOsOutput("mvCamSensorInit\n");
     
     if(!pCamSensor)
     {
	  mvOsPrintf("mvCamSensorInit: Bad Input\n");
	  return MV_BAD_PTR;
     }

     pInfo = mvOsMalloc(sizeof(OV7680_INFO));
     pCamSensor->pInfo = pInfo;
     if(!pCamSensor->pInfo)
     {
	  mvOsPrintf("mvCamSensorInit: failed to allocate memory\n");
	  return MV_FAIL;
     }
     
     pInfo->pixFormat = supportedFormats;
     pInfo->resolution = supportedResolutions;
     
     /* reset  */
     if (resetSensor(pCamSensor) != MV_OK)
     {
	  mvOsPrintf("mvCamSensorInit: sensor reset failed\n");
	  return MV_ERROR;
     }

     /*load defaults*/
     if(sensorRegsWrite(pCamSensor, ov7680_default_settings, 
			sizeof(ov7680_default_settings)/sizeof(sensorReg_t)) != MV_OK){
	  mvOsPrintf("mvCamSensorInit: sensor default registers loading failed\n");
	  return MV_ERROR;
     }
     
     return isOV7680(pCamSensor);
}


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
MV_STATUS mvCamSensorReset(MV_CAM_SENSOR *pCamSensor)
{
     mvOsOutput("mvCamSensorReset\n");
     
     if(!pCamSensor)
     {
	  mvOsPrintf("mvCamSensorReset: Bad Input\n");
	  return MV_BAD_PTR;
     }
     return resetSensor(pCamSensor);
}

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
MV_STATUS mvCamSensorImageFormatSet(MV_CAM_SENSOR *pCamSensor, MV_IMAGE_FORMAT *pFormat)
{
     int fmt;
     int res;
     MV_STATUS status;

     mvOsPrintf("mvCamSensorImageFormatSet\n");
     
     if(!pCamSensor)
     {
	  mvOsPrintf("mvCamSensorImageFormatSet: Bad Input\n");
	  return MV_BAD_PTR;
     }
     for (fmt = 0; fmt < sizeof(supportedFormats)/sizeof(supportedFormats[0]); fmt++)
	  if(pFormat->pixelFormat.id == supportedFormats[fmt].id)
	       break;

     if(fmt >= (sizeof(supportedFormats)/sizeof(supportedFormats[0])))
     {
	  mvOsPrintf("mvCamSensorImageFormatSet: unsupported pixel format %s\n",
	       pFormat->pixelFormat.name);
	  return MV_NOT_SUPPORTED;
     }

     for (res = 0; res < sizeof(supportedResolutions)/sizeof(supportedResolutions[0]);
	  res++)
	  if((pFormat->resolution.height == supportedResolutions[res].height) &&
	     (pFormat->resolution.width == supportedResolutions[res].width))
	       break;
     
     if(res >= (sizeof(supportedResolutions)/sizeof(supportedResolutions[0])))
     {
	  mvOsPrintf("mvCamSensorImageFormatSet: unsupported resolution %s\n",
		     pFormat->resolution.name);
	  return MV_NOT_SUPPORTED;
     }
     status = configPixelFormat(pCamSensor, fmt);
     if(status != MV_OK)
     {
	  return status;
     }
     return configResolution(pCamSensor, res);
}

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
MV_STATUS mvCamSensorImageFormatGet(MV_CAM_SENSOR *pCamSensor, MV_IMAGE_FORMAT *pFormat)
{
     OV7680_INFO *pInfo;

     mvOsPrintf("mvCamSensorImageFormatGet\n");
     
     if(!pCamSensor || !pFormat)
     {
	  mvOsPrintf("mvCamSensorImageFormatGet: Bad Input\n");
	  return MV_BAD_PTR;
     }
     pInfo = (OV7680_INFO *)pCamSensor->pInfo;

     pFormat->pixelFormat.id = pInfo->pixFormat->id;
     pFormat->pixelFormat.name = pInfo->pixFormat->name;
     pFormat->resolution.width = pInfo->resolution->width;
     pFormat->resolution.height = pInfo->resolution->height;
     pFormat->resolution.name = pInfo->resolution->name;
     return MV_OK;
}

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
MV_STATUS mvCamSensorPixelSizeGet(MV_CAM_SENSOR *pCamSensor, MV_U32 *pixelSize)
{
     mvOsPrintf("mvCamSensorPixelSizeGet\n");
     
     if(!pCamSensor || !pixelSize)
     {
	  mvOsPrintf("mvCamSensorPixelSizeGet: Bad Input\n");
	  return MV_BAD_PTR;
     }

     *pixelSize = getBitsPerPixel(pCamSensor);
     return MV_OK;
}

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
				       MV_BOOL enable)
{
     MV_U8	reg;
     MV_STATUS	status = MV_ERROR;

     mvOsPrintf("mvCamSensorTestPatternConfig\n");
     
     if(!pCamSensor)
     {
	  mvOsPrintf("mvCamSensorTestPatternConfig: Bad Input\n");
	  return MV_BAD_PTR;
     }

     switch(pattern)
     {
     case 0:
	  status = mvSysCamSensorRegRead(pCamSensor->I2CClient, OV_REG_0C, &reg);
	  if(status != MV_OK)
	       return MV_ERROR;
	  
	  if(enable)
	       reg |= 1;
	  else
	       reg &= ~1;
	  
	  status = mvSysCamSensorRegWrite(pCamSensor->I2CClient, OV_REG_0C, reg);
	  break;
     case 1:
	  status = mvSysCamSensorRegRead(pCamSensor->I2CClient, OV_REG_83, &reg);
	  if(status != MV_OK)
	       return MV_ERROR;
	  
	  if(enable)
	       reg |= 1;
	  else
	       reg &= ~1;
	  
	  status = mvSysCamSensorRegWrite(pCamSensor->I2CClient, OV_REG_83, reg);
	  break;
     case 2:
	  status = mvSysCamSensorRegRead(pCamSensor->I2CClient, OV_REG_62, &reg);
 	  if(status != MV_OK)
	       return MV_ERROR;
	  
	  if(enable)
	       reg |= (1 << 7) | (1 << 5);
	  else
	       reg &= ~((1 << 7) | (1 << 5));
	  
	  status = mvSysCamSensorRegWrite(pCamSensor->I2CClient, OV_REG_62, reg);
	  break;
     default:
  	  mvOsPrintf("mvCamSensorTestPatternConfig: unsupported test pattern (%d)\n",
	       pattern);
	  return MV_BAD_PARAM;
     }
     return status;
}
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
					MV_U32 *min, MV_U32 *max, MV_U32 *step)
{
     MV_STATUS  status = MV_OK;

     if(!pCamSensor || !min || !max || !step)
     {
	  mvOsPrintf("mvCamSensorImageParamRangeGet: Bad Input\n");
	  return MV_BAD_PTR;
     }

     switch (param)
     {
     case MV_IMAGE_PARAM_RATE:
	  *min = OV_7680_RATE;
	  *max = OV_7680_RATE;
	  *step = 0;
	  break;
     case MV_IMAGE_PARAM_BRIGHTNESS:
	  *min = 0;
	  *max = 512;
	  *step = 1;
	  break;
     case MV_IMAGE_PARAM_CONTRAST:
	  *min = 0;
	  *max = 512;
	  *step = 1;
	  break;
     case MV_IMAGE_PARAM_SATURATION:
	  *min = 0;
	  *max = 256;
	  *step = 0;
	  break;
     case MV_IMAGE_PARAM_VFLIP:
     case MV_IMAGE_PARAM_HFLIP:
	  *min = 0;
	  *max = 1;
	  *step = 1;
	  break;
     case MV_IMAGE_PARAM_HUE:
	  mvOsPrintf("mvCamSensorImageParamRangeGet: not supported param\n");
	  status = MV_NOT_SUPPORTED;
	  break;
     default:
	  mvOsPrintf("mvCamSensorImageParamGet: Invalid param\n");
	  status = MV_BAD_PARAM;
     }
     return status;
}

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
				   MV_U32 value)
{
     MV_STATUS  status = MV_OK;
     MV_U8	reg;
     int	sign;

     if(!pCamSensor)
     {
	  mvOsPrintf("mvCamSensorImageParamSet: Bad Input\n");
	  return MV_BAD_PTR;
     }

     switch (param)
     {
     case MV_IMAGE_PARAM_RATE:
	  if(value != OV_7680_RATE)
	       status = MV_BAD_VALUE;
	  break;
     case MV_IMAGE_PARAM_BRIGHTNESS:
	  status = MV_ERROR;
	  if(MV_OK != enableSDEFeature(pCamSensor, OV_SDE_BRIGHTNESS_CONTRAST))
	       break;
	  if((value > 512) || (value < 0))
	  {
	       status = MV_BAD_VALUE;
	       break;
	  }
	  else if(value > 256)
	  {
	       reg = value & 0xFF;
	       sign = 0;
	  }
	  else
	  {
	       reg = 256 - value;
	       sign = 1;
	  }
	  
	  if(MV_OK != mvSysCamSensorRegWrite(pCamSensor->I2CClient, OV_REG_BRIGHTNESS, reg))
	       break;
	  
	  /*set sign*/
	  if(MV_OK != mvSysCamSensorRegRead(pCamSensor->I2CClient, OV_REG_SGNSET, &reg))
	       break;

	  reg &= ~1;
	  if(sign)
	       reg |= (1 << 3);

	  if(MV_OK != mvSysCamSensorRegWrite(pCamSensor->I2CClient,
				  OV_REG_SGNSET, reg))
	       break;
	  status = MV_OK;
	  break;
     case MV_IMAGE_PARAM_CONTRAST:
	  status = MV_ERROR;
	  if(MV_OK != enableSDEFeature(pCamSensor, OV_SDE_BRIGHTNESS_CONTRAST))
	       break;

	  if((value > 512) || (value < 0))
	  {
	       status = MV_BAD_VALUE;
	       break;
	  }
	  else if(value > 256)
	  {
	       reg = value & 0xFF;
	       sign = 0;
	  }
	  else
	  {
	       reg = 256 - value;
	       sign = 1;
	  }
	  
	  if(MV_OK != mvSysCamSensorRegWrite(pCamSensor->I2CClient,
				  OV_REG_YOFFSET, reg))
	       break;
	  
	  /*set sign*/
	  if(MV_OK != mvSysCamSensorRegRead(pCamSensor->I2CClient,
				  OV_REG_SGNSET, &reg))
	       break;

	  reg &= ~1;
	  if(sign)
	       reg |= (1 << 2);

	  if(MV_OK != mvSysCamSensorRegWrite(pCamSensor->I2CClient,
				  OV_REG_SGNSET, reg))
	       break;
	  status = MV_OK;
	  break;
     case MV_IMAGE_PARAM_SATURATION:
	  status = MV_ERROR;
	  if(MV_OK != enableSDEFeature(pCamSensor, OV_SDE_SATURATION))
	       break;
	  
	  if((value > 256) || (value < 0))
	  {
	       status = MV_BAD_VALUE;
	       break;
	  }
	  reg = value & 0xFF;
	  if(MV_OK != mvSysCamSensorRegWrite(pCamSensor->I2CClient,
				  OV_REG_VSAT, reg))
	       break;
	  
	  if(MV_OK != mvSysCamSensorRegWrite(pCamSensor->I2CClient,
				  OV_REG_USAT, reg))
	       break;
	  status = MV_OK;
	  break;

     case MV_IMAGE_PARAM_VFLIP:
	  status = MV_ERROR;
	  if((value != 0) || (value != 1))
	  {
	       status = MV_BAD_VALUE;
	       break;
	  }

	  if(MV_OK != mvSysCamSensorRegRead(pCamSensor->I2CClient,
				  OV_REG_0C, &reg))
	       break;

	  if(value)
	       reg |= (1 << 7);
	  else
	       reg &= ~(1 << 7);
	  
	  if(MV_OK != mvSysCamSensorRegWrite(pCamSensor->I2CClient,
				  OV_REG_0C, reg))
	       break;

	  status = MV_OK;
	  break;

     case MV_IMAGE_PARAM_HFLIP:
	  status = MV_ERROR;
	  if((value != 0) || (value != 1))
	  {
	       status = MV_BAD_VALUE;
	       break;
	  }

	  if(MV_OK != mvSysCamSensorRegRead(pCamSensor->I2CClient,
				  OV_REG_0C, &reg))
	       break;

	  if(value)
	       reg |= (1 << 6);
	  else
	       reg &= ~(1 << 6);
	  
	  if(MV_OK != mvSysCamSensorRegWrite(pCamSensor->I2CClient,
				  OV_REG_0C, reg))
	       break;

	  status = MV_OK;
	  break;
     case MV_IMAGE_PARAM_HUE:
	  mvOsPrintf("mvCamSensorImageParamSet: not supported param\n");
	  status = MV_NOT_SUPPORTED;
	  break;
     default:
	  mvOsPrintf("mvCamSensorImageParamSet: Invalid param\n");
	  status = MV_BAD_PARAM;
     }
     return status;
}

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
				   MV_U32 *value)
{
     MV_STATUS  status = MV_OK;
     MV_U8	reg;

     if(!pCamSensor || !value)
     {
	  mvOsPrintf("mvCamSensorImageParamGet: Bad Input\n");
	  return MV_BAD_PTR;
     }

     switch (param)
     {
     case MV_IMAGE_PARAM_RATE:
	  *value = OV_7680_RATE;
	  break;
     case MV_IMAGE_PARAM_SATURATION:
	  status = MV_ERROR;
	  if(MV_OK != mvSysCamSensorRegRead(pCamSensor->I2CClient,
				  OV_REG_VSAT, &reg))
	       break;
	  
	  *value = reg;
	  status = MV_OK;
	  break;

     case MV_IMAGE_PARAM_BRIGHTNESS:
     case MV_IMAGE_PARAM_CONTRAST:
     case MV_IMAGE_PARAM_HUE:
     case MV_IMAGE_PARAM_VFLIP:
     case MV_IMAGE_PARAM_HFLIP:
	  mvOsPrintf("mvCamSensorImageParamGet: not supported param\n");
	  status = MV_NOT_SUPPORTED;
	  break;
     default:
	  mvOsPrintf("mvCamSensorImageParamGet: Invalid param\n");
	  status = MV_BAD_PARAM;
     }
     return status;

}
