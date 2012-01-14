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
#include "mvBoardEnvLib.h"
#include "mvBoardEnvSpec.h"
#include "twsi/mvTwsi.h"

#define DB_88F6781_BOARD_PCI_IF_NUM		0x0
#define DB_88F6781_BOARD_TWSI_DEF_NUM		0x5
#define DB_88F6781_BOARD_MAC_INFO_NUM		0x1
#define DB_88F6781_BOARD_GPP_INFO_NUM		0xA
#define DB_88F6781_BOARD_MPP_CONFIG_NUM		0x1
#define DB_88F6781_BOARD_MPP_GROUP_TYPE_NUM	0x1
#define DB_88F6781_BOARD_DEVICE_CONFIG_NUM	0x1
#define DB_88F6781_BOARD_DEBUG_LED_NUM		0x4

MV_U8	db88f6781InfoBoardDebugLedIf[] =
	{14,15,18,19};

MV_BOARD_TWSI_INFO	db88f6781InfoBoardTwsiDev[] =
	/* {{MV_BOARD_DEV_CLASS	devClass, MV_U8	twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{
	{BOARD_DEV_TWSI_EXP, 0x20, ADDR7_BIT},
	{BOARD_DEV_TWSI_EXP, 0x21, ADDR7_BIT},
	{BOARD_DEV_TWSI_EXP, 0x27, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4C, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4D, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4E, ADDR7_BIT},
	{BOARD_TWSI_AUDIO_DEC, 0x4A, ADDR7_BIT}, 
	{BOARD_TWSI_BATT_CHARGER, 0x09, ADDR7_BIT}, 
	};

MV_BOARD_MAC_INFO db88f6781InfoBoardMacInfo[] = 
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{
	{BOARD_MAC_SPEED_AUTO, 0x8}
	}; 

MV_BOARD_MPP_TYPE_INFO db88f6781InfoBoardMppTypeInfo[] = 
	/* {{MV_BOARD_MPP_TYPE_CLASS	boardMppGroup1,
 		MV_BOARD_MPP_TYPE_CLASS	boardMppGroup2}} */
	{{MV_BOARD_AUTO, MV_BOARD_AUTO}
	}; 

MV_BOARD_GPP_INFO db88f6781InfoBoardGppInfo[] = 
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{
	/* TODO check which is L...*/
	{BOARD_GPP_TS_BUTTON_C, 14},
	{BOARD_GPP_TS_BUTTON_U, 15},
	{BOARD_GPP_TS_BUTTON_R, 18},
	{BOARD_GPP_TS_BUTTON_L, 19},
	{BOARD_GPP_TS_BUTTON_D, 52},

	{BOARD_GPP_TS1_BUTTON_C, 53},
	{BOARD_GPP_TS1_BUTTON_U, 54},
	{BOARD_GPP_TS1_BUTTON_R, 55},
	{BOARD_GPP_TS1_BUTTON_L, 56},
	{BOARD_GPP_TS1_BUTTON_D, 57}
	};

MV_BOARD_MPP_INFO	db88f6781InfoBoardMppConfigValue[] = 
	{{{
	DB_88F6781_MPP0_7,		
	DB_88F6781_MPP8_15,		
	DB_88F6781_MPP16_23	
	}}};

MV_DEV_CS_INFO db88f6781InfoBoardDevCsInfo[] = 
		/*{deviceCS, params, devType, devWidth}*/			   
		{
		 {0, N_A, BOARD_DEV_SPI_FLASH, 8}
		};


MV_BOARD_INFO db88f6781Info = {
	"DB-88F6781-BP",				/* boardName[MAX_BOARD_NAME_LEN] */
	DB_88F6781_BOARD_MPP_GROUP_TYPE_NUM,		/* numBoardMppGroupType */
	db88f6781InfoBoardMppTypeInfo,
	DB_88F6781_BOARD_MPP_CONFIG_NUM,		/* numBoardMppConfig */
	db88f6781InfoBoardMppConfigValue,
	0,						/* intsGppMaskLow */
	0,						/* intsGppMaskHigh */
	DB_88F6781_BOARD_DEVICE_CONFIG_NUM,		/* numBoardDevIf */
	db88f6781InfoBoardDevCsInfo,
	DB_88F6781_BOARD_TWSI_DEF_NUM,			/* numBoardTwsiDev */
	db88f6781InfoBoardTwsiDev,					
	DB_88F6781_BOARD_MAC_INFO_NUM,			/* numBoardMacInfo */
	db88f6781InfoBoardMacInfo,
	DB_88F6781_BOARD_GPP_INFO_NUM,			/* numBoardGppInfo */
	db88f6781InfoBoardGppInfo,
	DB_88F6781_BOARD_DEBUG_LED_NUM,			/* activeLedsNumber */              
	db88f6781InfoBoardDebugLedIf,
	1,						/* ledsPolarity */		
	DB_88F6781_OE_LOW,				/* gppOutEnLow */
	DB_88F6781_OE_HIGH,				/* gppOutEnHigh */
	0xFFFFFFFF, 					/* gpp2OutEn */
	DB_88F6781_OE_VAL_LOW,				/* gppOutValLow */
	DB_88F6781_OE_VAL_HIGH,				/* gppOutValHigh */
	0, 						/* gpp2OutVal */
	0,						/* gppPolarityValLow */
	0, 						/* gppPolarityValHigh */
	0, 						/* gpp2PolarityVal */
	NULL						/* pSwitchInfo */
};


#define DB_88F6781Y0_BOARD_PCI_IF_NUM		0x0
#define DB_88F6781Y0_BOARD_TWSI_DEF_NUM		0x7
#define DB_88F6781Y0_BOARD_MAC_INFO_NUM		0x1
#define DB_88F6781Y0_BOARD_GPP_INFO_NUM		0x0
#define DB_88F6781Y0_BOARD_MPP_CONFIG_NUM	0x1
#define DB_88F6781Y0_BOARD_MPP_GROUP_TYPE_NUM	0x1
#define DB_88F6781Y0_BOARD_DEVICE_CONFIG_NUM	0x1
#define DB_88F6781Y0_BOARD_DEBUG_LED_NUM	0x4

MV_U8	db88f6781Y0InfoBoardDebugLedIf[] =
	{14,15,62,63};

/* TODO, see if need to add more */
MV_BOARD_TWSI_INFO	db88f6781Y0InfoBoardTwsiDev[] =
	/* {{MV_BOARD_DEV_CLASS	devClass, MV_U8	twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{
	{BOARD_DEV_TWSI_EXP, 0x20, ADDR7_BIT},
	{BOARD_DEV_TWSI_EXP, 0x21, ADDR7_BIT},
	{BOARD_DEV_TWSI_EXP, 0x27, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4C, ADDR7_BIT}, 
	{BOARD_DEV_TWSI_SATR, 0x4D, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4E, ADDR7_BIT},
	{BOARD_TWSI_BATT_CHARGER, 0x09, ADDR7_BIT}, 
	};

MV_BOARD_MAC_INFO db88f6781Y0InfoBoardMacInfo[] = 
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{
	{BOARD_MAC_SPEED_AUTO, 0x8}
	}; 

MV_BOARD_MPP_TYPE_INFO db88f6781Y0InfoBoardMppTypeInfo[] = 
	/* {{MV_BOARD_MPP_TYPE_CLASS	boardMppGroup1,
 		MV_BOARD_MPP_TYPE_CLASS	boardMppGroup2}} */
	{{MV_BOARD_AUTO, MV_BOARD_AUTO}
	}; 

MV_BOARD_GPP_INFO db88f6781Y0InfoBoardGppInfo[] = 
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{
	/* TODO */
	};

MV_BOARD_MPP_INFO	db88f6781Y0InfoBoardMppConfigValue[] = 
	{{{
	DB_88F6781Y0_MPP0_7,		
	DB_88F6781Y0_MPP8_15,		
	DB_88F6781Y0_MPP16_23	
	}}};

MV_DEV_CS_INFO db88f6781Y0InfoBoardDevCsInfo[] = 
		/*{deviceCS, params, devType, devWidth}*/			   
		{
		 {0, N_A, BOARD_DEV_SPI_FLASH, 8} 
		 /* TODO {1, N_A, BOARD_DEV_GPS, 8} */
		};


MV_BOARD_INFO db88f6781Y0Info = {
#if defined(DB_88F6781X0)
	"DB-88AP510-BP",				/* boardName[MAX_BOARD_NAME_LEN] */
#else /* Y0/Y1 */
	"DB-88F6781Yx-BP",				/* boardName[MAX_BOARD_NAME_LEN] */
#endif
	DB_88F6781Y0_BOARD_MPP_GROUP_TYPE_NUM,		/* numBoardMppGroupType */
	db88f6781Y0InfoBoardMppTypeInfo,
	DB_88F6781Y0_BOARD_MPP_CONFIG_NUM,		/* numBoardMppConfig */
	db88f6781Y0InfoBoardMppConfigValue,
	0,						/* intsGppMaskLow */
	0,						/* intsGppMaskHigh */
	DB_88F6781Y0_BOARD_DEVICE_CONFIG_NUM,		/* numBoardDevIf */
	db88f6781Y0InfoBoardDevCsInfo,
	DB_88F6781Y0_BOARD_TWSI_DEF_NUM,		/* numBoardTwsiDev */
	db88f6781Y0InfoBoardTwsiDev,					
	DB_88F6781Y0_BOARD_MAC_INFO_NUM,		/* numBoardMacInfo */
	db88f6781Y0InfoBoardMacInfo,
	DB_88F6781Y0_BOARD_GPP_INFO_NUM,		/* numBoardGppInfo */
	db88f6781Y0InfoBoardGppInfo,
	DB_88F6781Y0_BOARD_DEBUG_LED_NUM,		/* activeLedsNumber */              
	db88f6781Y0InfoBoardDebugLedIf,
	1,						/* ledsPolarity */		
	DB_88F6781Y0_OE_LOW,				/* gppOutEnLow */
	DB_88F6781Y0_OE_HIGH,				/* gppOutEnHigh */
	0xFFFFFFFF, 					/* gpp2OutEn */
	DB_88F6781Y0_OE_VAL_LOW,			/* gppOutValLow */
	DB_88F6781Y0_OE_VAL_HIGH,			/* gppOutValHigh */
	0, 						/* gpp2OutVal */
	0,						/* gppPolarityValLow */
	0, 						/* gppPolarityValHigh */
	0, 						/* gpp2PolarityVal */
	NULL						/* pSwitchInfo */
};


#define RD_88F6781X0_PLUG_BOARD_PCI_IF_NUM		0x0
#define RD_88F6781X0_PLUG_BOARD_TWSI_DEF_NUM		0x0
#define RD_88F6781X0_PLUG_BOARD_MAC_INFO_NUM		0x1
#define RD_88F6781X0_PLUG_BOARD_GPP_INFO_NUM		0x0
#define RD_88F6781X0_PLUG_BOARD_MPP_CONFIG_NUM		0x1
#define RD_88F6781X0_PLUG_BOARD_MPP_GROUP_TYPE_NUM	0x1
#define RD_88F6781X0_PLUG_BOARD_DEVICE_CONFIG_NUM	0x1
#define RD_88F6781X0_PLUG_BOARD_DEBUG_LED_NUM		0x0

MV_U8	rd88f6781X0PlugInfoBoardDebugLedIf[] =
	{};

/* TODO, see if need to add more */
MV_BOARD_TWSI_INFO	rd88f6781X0PlugInfoBoardTwsiDev[] =
	/* {{MV_BOARD_DEV_CLASS	devClass, MV_U8	twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{
	};

MV_BOARD_MAC_INFO rd88f6781X0PlugInfoBoardMacInfo[] = 
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{
	{BOARD_MAC_SPEED_AUTO, 0x1}
	}; 

MV_BOARD_MPP_TYPE_INFO rd88f6781X0PlugInfoBoardMppTypeInfo[] = 
	/* {{MV_BOARD_MPP_TYPE_CLASS	boardMppGroup1,
 		MV_BOARD_MPP_TYPE_CLASS	boardMppGroup2}} */
	{{MV_BOARD_AUTO, MV_BOARD_AUTO}
	}; 

MV_BOARD_GPP_INFO rd88f6781X0PlugInfoBoardGppInfo[] = 
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{
	/* TODO */
	};

MV_BOARD_MPP_INFO	rd88f6781X0PlugInfoBoardMppConfigValue[] = 
	{{{
	RD_88F6781X0_PLUG_MPP0_7,		
	RD_88F6781X0_PLUG_MPP8_15,		
	RD_88F6781X0_PLUG_MPP16_23	
	}}};

MV_DEV_CS_INFO rd88f6781X0PlugInfoBoardDevCsInfo[] = 
		/*{deviceCS, params, devType, devWidth}*/			   
		{
		 {0, N_A, BOARD_DEV_SPI_FLASH, 8} 
		 /* TODO {1, N_A, BOARD_DEV_GPS, 8} */
		};


MV_BOARD_INFO rd88f6781X0PlugInfo = {
	"VideoPlug",									/* boardName[MAX_BOARD_NAME_LEN] */
	RD_88F6781X0_PLUG_BOARD_MPP_GROUP_TYPE_NUM,		/* numBoardMppGroupType */
	rd88f6781X0PlugInfoBoardMppTypeInfo,
	RD_88F6781X0_PLUG_BOARD_MPP_CONFIG_NUM,		/* numBoardMppConfig */
	rd88f6781X0PlugInfoBoardMppConfigValue,
	0,						/* intsGppMaskLow */
	0,						/* intsGppMaskHigh */
	RD_88F6781X0_PLUG_BOARD_DEVICE_CONFIG_NUM,		/* numBoardDevIf */
	rd88f6781X0PlugInfoBoardDevCsInfo,
	RD_88F6781X0_PLUG_BOARD_TWSI_DEF_NUM,		/* numBoardTwsiDev */
	rd88f6781X0PlugInfoBoardTwsiDev,					
	RD_88F6781X0_PLUG_BOARD_MAC_INFO_NUM,		/* numBoardMacInfo */
	rd88f6781X0PlugInfoBoardMacInfo,
	RD_88F6781X0_PLUG_BOARD_GPP_INFO_NUM,		/* numBoardGppInfo */
	rd88f6781X0PlugInfoBoardGppInfo,
	RD_88F6781X0_PLUG_BOARD_DEBUG_LED_NUM,		/* activeLedsNumber */              
	rd88f6781X0PlugInfoBoardDebugLedIf,
	1,						/* ledsPolarity */		
	RD_88F6781X0_PLUG_OE_LOW,				/* gppOutEnLow */
	RD_88F6781X0_PLUG_OE_HIGH,				/* gppOutEnHigh */
	0xFFFFFFFF, 					/* gpp2OutEn */
	RD_88F6781X0_PLUG_OE_VAL_LOW,			/* gppOutValLow */
	RD_88F6781X0_PLUG_OE_VAL_HIGH,			/* gppOutValHigh */
	0, 						/* gpp2OutVal */
	0,						/* gppPolarityValLow */
	0, 						/* gppPolarityValHigh */
	0, 						/* gpp2PolarityVal */
	NULL						/* pSwitchInfo */
};

#define RD_88F6781_BOARD_PCI_IF_NUM		0x0
#define RD_88F6781_BOARD_TWSI_DEF_NUM		0x2
#define RD_88F6781_BOARD_MAC_INFO_NUM		0x1
#define RD_88F6781_BOARD_GPP_INFO_NUM		0xA
#define RD_88F6781_BOARD_MPP_GROUP_TYPE_NUM	0x1
#define RD_88F6781_BOARD_MPP_CONFIG_NUM		0x1
#define RD_88F6781_BOARD_DEVICE_CONFIG_NUM	0x1
#define RD_88F6781_BOARD_DEBUG_LED_NUM		0x1

MV_U8	rd88f6781InfoBoardDebugLedIf[] =
	{8};

MV_BOARD_MAC_INFO rd88f6781InfoBoardMacInfo[] = 
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{
		{BOARD_MAC_SPEED_AUTO, 0x8}
	}; 

MV_BOARD_GPP_INFO rd88f6781InfoBoardGppInfo[] = 
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{
	/* TODO check which is L...*/
	{BOARD_GPP_TS_BUTTON_C, 12},
	{BOARD_GPP_TS_BUTTON_U, 13},
	{BOARD_GPP_TS_BUTTON_R, 14},
	{BOARD_GPP_TS_BUTTON_L, 15},
	{BOARD_GPP_TS_BUTTON_D, 18},

	{BOARD_GPP_TS1_BUTTON_C, 19},
	{BOARD_GPP_TS1_BUTTON_U, 52},
	{BOARD_GPP_TS1_BUTTON_R, 53},
	{BOARD_GPP_TS1_BUTTON_L, 54},
	{BOARD_GPP_TS1_BUTTON_D, 55}
	};

MV_BOARD_TWSI_INFO	rd88f6781InfoBoardTwsiDev[] =
	/* {{MV_BOARD_DEV_CLASS	devClass, MV_U8	twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{
	{BOARD_TWSI_AUDIO_DEC, 0x4A, ADDR7_BIT}, 
	{BOARD_TWSI_BATT_CHARGER, 0x09, ADDR7_BIT}, 
	};

MV_BOARD_MPP_TYPE_INFO rd88f6781InfoBoardMppTypeInfo[] = 
	/* {{MV_BOARD_MPP_TYPE_CLASS	boardMppGroup1,
 		MV_BOARD_MPP_TYPE_CLASS	boardMppGroup2}} */
	{
	{MV_BOARD_OTHER, MV_BOARD_OTHER} /* TODO */
	}; 

MV_BOARD_MPP_INFO	rd88f6781InfoBoardMppConfigValue[] = 
	{{{
	RD_88F6781_MPP0_7,		
	RD_88F6781_MPP8_15,		
	RD_88F6781_MPP16_23	
	}}};

MV_DEV_CS_INFO rd88f6781InfoBoardDevCsInfo[] = 
		/*{deviceCS, params, devType, devWidth}*/
		{
		 /*{0, N_A, BOARD_DEV_NAND_FLASH, 8},
		 {1, N_A, BOARD_DEV_NAND_FLASH, 8},
		 {2, N_A, BOARD_DEV_NAND_FLASH, 8},
		 {3, N_A, BOARD_DEV_NAND_FLASH, 8},*/
		 {0, N_A, BOARD_DEV_SPI_FLASH, 8}
		};


MV_BOARD_INFO rd88f6781Info = {
	"RD-88F6781",					/* boardName[MAX_BOARD_NAME_LEN] */
	RD_88F6781_BOARD_MPP_GROUP_TYPE_NUM,		/* numBoardMppGroupType */
	rd88f6781InfoBoardMppTypeInfo,
	RD_88F6781_BOARD_MPP_CONFIG_NUM,		/* numBoardMppConfig */
	rd88f6781InfoBoardMppConfigValue,
	0,						/* intsGppMaskLow */
	0,						/* intsGppMaskHigh */
	RD_88F6781_BOARD_DEVICE_CONFIG_NUM,		/* numBoardDevIf */
	rd88f6781InfoBoardDevCsInfo,
	RD_88F6781_BOARD_TWSI_DEF_NUM,			/* numBoardTwsiDev */
	rd88f6781InfoBoardTwsiDev,					
	RD_88F6781_BOARD_MAC_INFO_NUM,			/* numBoardMacInfo */
	rd88f6781InfoBoardMacInfo,
	RD_88F6781_BOARD_GPP_INFO_NUM,			/* numBoardGppInfo */
	rd88f6781InfoBoardGppInfo,
	RD_88F6781_BOARD_DEBUG_LED_NUM,			/* activeLedsNumber */              
	rd88f6781InfoBoardDebugLedIf,
	1,						/* ledsPolarity */		
	RD_88F6781_OE_LOW,				/* gppOutEnLow */
	RD_88F6781_OE_HIGH,				/* gppOutEnHigh */
	0xFFFFFFFF, 					/* gpp2OutEn */
	RD_88F6781_OE_VAL_LOW,				/* gppOutValLow */
	RD_88F6781_OE_VAL_HIGH,				/* gppOutValHigh */
	0, 						/* gpp2OutVal */
	0,						/* gppPolarityValLow */
	0, 						/* gppPolarityValHigh */
	0, 						/* gpp2PolarityVal */
	NULL						/* pSwitchInfo */
};

#define RD_88F6781_AVNG_BOARD_PCI_IF_NUM		0x0
#define RD_88F6781_AVNG_BOARD_TWSI_DEF_NUM		0x2
#define RD_88F6781_AVNG_BOARD_MAC_INFO_NUM		0x1
#define RD_88F6781_AVNG_BOARD_GPP_INFO_NUM		0x0
#define RD_88F6781_AVNG_BOARD_MPP_GROUP_TYPE_NUM	0x1
#define RD_88F6781_AVNG_BOARD_MPP_CONFIG_NUM		0x1
#define RD_88F6781_AVNG_BOARD_DEVICE_CONFIG_NUM		0x1
#define RD_88F6781_AVNG_BOARD_DEBUG_LED_NUM		0x0

MV_U8	rd88f6781AvngInfoBoardDebugLedIf[] =
	{};

MV_BOARD_MAC_INFO rd88f6781AvngInfoBoardMacInfo[] = 
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{
		{BOARD_MAC_SPEED_AUTO, 0x8}
	}; 

MV_BOARD_GPP_INFO rd88f6781AvngInfoBoardGppInfo[] = 
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{
	};

MV_BOARD_TWSI_INFO	rd88f6781AvngInfoBoardTwsiDev[] =
	/* {{MV_BOARD_DEV_CLASS	devClass, MV_U8	twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{
	{BOARD_TWSI_AUDIO_DEC, 0x4A, ADDR7_BIT}, 
	{BOARD_TWSI_BATT_CHARGER, 0x09, ADDR7_BIT}, 
	};

MV_BOARD_MPP_TYPE_INFO rd88f6781AvngInfoBoardMppTypeInfo[] = 
	/* {{MV_BOARD_MPP_TYPE_CLASS	boardMppGroup1,
 		MV_BOARD_MPP_TYPE_CLASS	boardMppGroup2}} */
	{
	{MV_BOARD_OTHER, MV_BOARD_OTHER} /* TODO */
	}; 

MV_BOARD_MPP_INFO	rd88f6781AvngInfoBoardMppConfigValue[] = 
	{{{
	RD_88F6781_AVNG_MPP0_7,		
	RD_88F6781_AVNG_MPP8_15,		
	RD_88F6781_AVNG_MPP16_23	
	}}};

MV_DEV_CS_INFO rd88f6781AvngInfoBoardDevCsInfo[] = 
	/*{deviceCS, params, devType, devWidth}*/
	{
		{0, N_A, BOARD_DEV_SPI_FLASH, 8}
	};


MV_BOARD_INFO rd88f6781AvngInfo = {
	"RD-88F6781-Avengers",				/* boardName[MAX_BOARD_NAME_LEN] */
	RD_88F6781_AVNG_BOARD_MPP_GROUP_TYPE_NUM,	/* numBoardMppGroupType */
	rd88f6781AvngInfoBoardMppTypeInfo,
	RD_88F6781_AVNG_BOARD_MPP_CONFIG_NUM,		/* numBoardMppConfig */
	rd88f6781AvngInfoBoardMppConfigValue,
	0,						/* intsGppMaskLow */
	0,						/* intsGppMaskHigh */
	RD_88F6781_AVNG_BOARD_DEVICE_CONFIG_NUM,	/* numBoardDevIf */
	rd88f6781AvngInfoBoardDevCsInfo,
	RD_88F6781_AVNG_BOARD_TWSI_DEF_NUM,		/* numBoardTwsiDev */
	rd88f6781AvngInfoBoardTwsiDev,
	RD_88F6781_AVNG_BOARD_MAC_INFO_NUM,		/* numBoardMacInfo */
	rd88f6781AvngInfoBoardMacInfo,
	RD_88F6781_AVNG_BOARD_GPP_INFO_NUM,		/* numBoardGppInfo */
	rd88f6781AvngInfoBoardGppInfo,
	RD_88F6781_AVNG_BOARD_DEBUG_LED_NUM,		/* activeLedsNumber */              
	rd88f6781AvngInfoBoardDebugLedIf,
	1,						/* ledsPolarity */		
	RD_88F6781_AVNG_OE_LOW,				/* gppOutEnLow */
	RD_88F6781_AVNG_OE_HIGH,			/* gppOutEnHigh */
	0xFFFFFFFF, 					/* gpp2OutEn */
	RD_88F6781_AVNG_OE_VAL_LOW,			/* gppOutValLow */
	RD_88F6781_AVNG_OE_VAL_HIGH,			/* gppOutValHigh */
	0, 						/* gpp2OutVal */
	0,						/* gppPolarityValLow */
	0, 						/* gppPolarityValHigh */
	0, 						/* gpp2PolarityVal */
	NULL						/* pSwitchInfo */
};


#define RD_88F6781Y0_AVNG_BOARD_PCI_IF_NUM		0x0
#define RD_88F6781Y0_AVNG_BOARD_TWSI_DEF_NUM		0x2
#define RD_88F6781Y0_AVNG_BOARD_MAC_INFO_NUM		0x1
#define RD_88F6781Y0_AVNG_BOARD_GPP_INFO_NUM		0x0
#define RD_88F6781Y0_AVNG_BOARD_MPP_GROUP_TYPE_NUM	0x1
#define RD_88F6781Y0_AVNG_BOARD_MPP_CONFIG_NUM		0x1
#define RD_88F6781Y0_AVNG_BOARD_DEVICE_CONFIG_NUM	0x1
#define RD_88F6781Y0_AVNG_BOARD_DEBUG_LED_NUM		0x0

MV_U8	rd88f6781Y0AvngInfoBoardDebugLedIf[] =
	{};

MV_BOARD_MAC_INFO rd88f6781Y0AvngInfoBoardMacInfo[] = 
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{
		{BOARD_MAC_SPEED_AUTO, 0x1}
	}; 

MV_BOARD_GPP_INFO rd88f6781Y0AvngInfoBoardGppInfo[] = 
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{
	};

MV_BOARD_TWSI_INFO	rd88f6781Y0AvngInfoBoardTwsiDev[] =
	/* {{MV_BOARD_DEV_CLASS	devClass, MV_U8	twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{
	{BOARD_TWSI_AUDIO_DEC, 0x1F, ADDR7_BIT}, 
	{BOARD_TWSI_BATT_CHARGER, 0x34, ADDR7_BIT}, 
	};

MV_BOARD_MPP_TYPE_INFO rd88f6781Y0AvngInfoBoardMppTypeInfo[] = 
	/* {{MV_BOARD_MPP_TYPE_CLASS	boardMppGroup1,
 		MV_BOARD_MPP_TYPE_CLASS	boardMppGroup2}} */
	{
	{MV_BOARD_OTHER, MV_BOARD_OTHER} /* TODO */
	}; 

MV_BOARD_MPP_INFO	rd88f6781Y0AvngInfoBoardMppConfigValue[] = 
	{{{
	RD_88F6781Y0_AVNG_MPP0_7,		
	RD_88F6781Y0_AVNG_MPP8_15,		
	RD_88F6781Y0_AVNG_MPP16_23	
	}}};

MV_DEV_CS_INFO rd88f6781Y0AvngInfoBoardDevCsInfo[] = 
	/*{deviceCS, params, devType, devWidth}*/
	{
		{0, N_A, BOARD_DEV_SPI_FLASH, 8}
	};


MV_BOARD_INFO rd88f6781Y0AvngInfo = {
#if defined(RD_88F6781X0_AVNG)
	"RD-88AP510-Avengers",				/* boardName[MAX_BOARD_NAME_LEN] */
#else
	"RD-88F6781Y0-Avengers",			/* boardName[MAX_BOARD_NAME_LEN] */
#endif
	RD_88F6781Y0_AVNG_BOARD_MPP_GROUP_TYPE_NUM,	/* numBoardMppGroupType */
	rd88f6781Y0AvngInfoBoardMppTypeInfo,
	RD_88F6781Y0_AVNG_BOARD_MPP_CONFIG_NUM,		/* numBoardMppConfig */
	rd88f6781Y0AvngInfoBoardMppConfigValue,
	0,						/* intsGppMaskLow */
	0,						/* intsGppMaskHigh */
	RD_88F6781Y0_AVNG_BOARD_DEVICE_CONFIG_NUM,	/* numBoardDevIf */
	rd88f6781Y0AvngInfoBoardDevCsInfo,
	RD_88F6781Y0_AVNG_BOARD_TWSI_DEF_NUM,		/* numBoardTwsiDev */
	rd88f6781Y0AvngInfoBoardTwsiDev,
	RD_88F6781Y0_AVNG_BOARD_MAC_INFO_NUM,		/* numBoardMacInfo */
	rd88f6781Y0AvngInfoBoardMacInfo,
	RD_88F6781Y0_AVNG_BOARD_GPP_INFO_NUM,		/* numBoardGppInfo */
	rd88f6781Y0AvngInfoBoardGppInfo,
	RD_88F6781Y0_AVNG_BOARD_DEBUG_LED_NUM,		/* activeLedsNumber */              
	rd88f6781Y0AvngInfoBoardDebugLedIf,
	1,						/* ledsPolarity */		
	RD_88F6781Y0_AVNG_OE_LOW,			/* gppOutEnLow */
	RD_88F6781Y0_AVNG_OE_HIGH,			/* gppOutEnHigh */
	0xFFFFFFFF,					/* gpp2OutEn */
	RD_88F6781Y0_AVNG_OE_VAL_LOW,			/* gppOutValLow */
	RD_88F6781Y0_AVNG_OE_VAL_HIGH,			/* gppOutValHigh */
	0,						/* gpp2OutVal */
	0,						/* gppPolarityValLow */
	0, 						/* gppPolarityValHigh */
	0,						/* gpp2PolarityVal */
	NULL						/* pSwitchInfo */
};
//----------------------------------------------------------------------------------------
//A0 RD board,AVD1 v3.0
#define RD_88AP510A0_AVNG_BOARD_PCI_IF_NUM				0x0
#define RD_88AP510A0_AVNG_BOARD_TWSI_DEF_NUM			0x2
#define RD_88AP510A0_AVNG_BOARD_MAC_INFO_NUM			0x1
#define RD_88AP510A0_AVNG_BOARD_GPP_INFO_NUM			0x0
#define RD_88AP510A0_AVNG_BOARD_MPP_GROUP_TYPE_NUM		0x1
#define RD_88AP510A0_AVNG_BOARD_MPP_CONFIG_NUM			0x1
#define RD_88AP510A0_AVNG_BOARD_DEVICE_CONFIG_NUM		0x1
#define RD_88AP510A0_AVNG_BOARD_DEBUG_LED_NUM			0x0

MV_U8	rd88ap510A0AvngInfoBoardDebugLedIf[] =
	{};

MV_BOARD_MAC_INFO rd88ap510A0AvngInfoBoardMacInfo[] = 
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{
		{BOARD_MAC_SPEED_AUTO, 0x1}
	}; 

MV_BOARD_GPP_INFO rd88ap510A0AvngInfoBoardGppInfo[] = 
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{
	};

MV_BOARD_TWSI_INFO	rd88ap510A0AvngInfoBoardTwsiDev[] =
	/* {{MV_BOARD_DEV_CLASS	devClass, MV_U8	twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{
	{BOARD_TWSI_AUDIO_DEC, 0x1F, ADDR7_BIT}, 
	{BOARD_TWSI_BATT_CHARGER, 0x34, ADDR7_BIT}, 
	};

MV_BOARD_MPP_TYPE_INFO rd88ap510A0AvngInfoBoardMppTypeInfo[] = 
	/* {{MV_BOARD_MPP_TYPE_CLASS	boardMppGroup1,
 		MV_BOARD_MPP_TYPE_CLASS	boardMppGroup2}} */
	{
	{MV_BOARD_OTHER, MV_BOARD_OTHER} /* TODO */
	}; 

MV_BOARD_MPP_INFO	rd88ap510A0AvngInfoBoardMppConfigValue[] = 
	{{{
	RD_88AP510A0_AVNG_MPP0_7,		
	RD_88AP510A0_AVNG_MPP8_15,		
	RD_88AP510A0_AVNG_MPP16_23	
	}}};

MV_DEV_CS_INFO rd88ap510A0AvngInfoBoardDevCsInfo[] = 
	/*{deviceCS, params, devType, devWidth}*/
	{
		{0, N_A, BOARD_DEV_SPI_FLASH, 8}
	};



MV_BOARD_INFO rd88ap510AvngInfo = {
	"RD-88AP510-Avengers-V3.x",				/* boardName[MAX_BOARD_NAME_LEN] */
	RD_88AP510A0_AVNG_BOARD_MPP_GROUP_TYPE_NUM,	/* numBoardMppGroupType */
	rd88ap510A0AvngInfoBoardMppTypeInfo,
	RD_88AP510A0_AVNG_BOARD_MPP_CONFIG_NUM,		/* numBoardMppConfig */
	rd88ap510A0AvngInfoBoardMppConfigValue,
	0,											/* intsGppMaskLow */
	0,											/* intsGppMaskHigh */
	RD_88AP510A0_AVNG_BOARD_DEVICE_CONFIG_NUM,	/* numBoardDevIf */
	rd88ap510A0AvngInfoBoardDevCsInfo,
	RD_88AP510A0_AVNG_BOARD_TWSI_DEF_NUM,		/* numBoardTwsiDev */
	rd88ap510A0AvngInfoBoardTwsiDev,
	RD_88AP510A0_AVNG_BOARD_MAC_INFO_NUM,		/* numBoardMacInfo */
	rd88ap510A0AvngInfoBoardMacInfo,
	RD_88AP510A0_AVNG_BOARD_GPP_INFO_NUM,		/* numBoardGppInfo */
	rd88ap510A0AvngInfoBoardGppInfo,
	RD_88AP510A0_AVNG_BOARD_DEBUG_LED_NUM,		/* activeLedsNumber */              
	rd88ap510A0AvngInfoBoardDebugLedIf,
	1,											/* ledsPolarity */		
	RD_88AP510A0_AVNG_OE_LOW,					/* gppOutEnLow */
	RD_88AP510A0_AVNG_OE_HIGH,					/* gppOutEnHigh */
	0xFFFFFFFF,									/* gpp2OutEn */
	RD_88AP510A0_AVNG_OE_VAL_LOW,				/* gppOutValLow */
	RD_88AP510A0_AVNG_OE_VAL_HIGH,				/* gppOutValHigh */
	0,											/* gpp2OutVal */
	0,											/* gppPolarityValLow */
	0, 											/* gppPolarityValHigh */
	0,											/* gpp2PolarityVal */
	NULL										/* pSwitchInfo */
};

//----------------------------------------------------------------------------------------


/*

A0 DB board

*/


#define DB_88AP510BP_B_BOARD_PCI_IF_NUM		0x0
#define DB_88AP510BP_B_BOARD_TWSI_DEF_NUM		0x7
#define DB_88AP510BP_B_BOARD_MAC_INFO_NUM		0x1
#define DB_88AP510BP_B_BOARD_GPP_INFO_NUM		0x0
#define DB_88AP510BP_B_BOARD_MPP_CONFIG_NUM	0x1
#define DB_88AP510BP_B_BOARD_MPP_GROUP_TYPE_NUM	0x1
#define DB_88AP510BP_B_BOARD_DEVICE_CONFIG_NUM	0x1
#define DB_88AP510BP_B_BOARD_DEBUG_LED_NUM	0x4

MV_U8	db88f6781bpbInfoBoardDebugLedIf[] =
	{14,15,62,63};

/* TODO, see if need to add more */
MV_BOARD_TWSI_INFO	db88f6781bpbInfoBoardTwsiDev[] =
	/* {{MV_BOARD_DEV_CLASS	devClass, MV_U8	twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{
	{BOARD_DEV_TWSI_EXP, 0x20, ADDR7_BIT},
	{BOARD_DEV_TWSI_EXP, 0x21, ADDR7_BIT},
	{BOARD_DEV_TWSI_EXP, 0x27, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4C, ADDR7_BIT}, 
	{BOARD_DEV_TWSI_SATR, 0x4D, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4E, ADDR7_BIT},
	{BOARD_TWSI_BATT_CHARGER, 0x09, ADDR7_BIT}, 
	};

MV_BOARD_MAC_INFO db88f6781bpbInfoBoardMacInfo[] = 
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{
	{BOARD_MAC_SPEED_AUTO, 0x1}
	}; 

MV_BOARD_MPP_TYPE_INFO db88f6781bpbInfoBoardMppTypeInfo[] = 
	/* {{MV_BOARD_MPP_TYPE_CLASS	boardMppGroup1,
 		MV_BOARD_MPP_TYPE_CLASS	boardMppGroup2}} */
	{{MV_BOARD_AUTO, MV_BOARD_AUTO}
	}; 

MV_BOARD_GPP_INFO db88f6781bpbInfoBoardGppInfo[] = 
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{
	/* TODO */
	};

MV_BOARD_MPP_INFO	db88f6781bpbInfoBoardMppConfigValue[] = 
	{{{
	DB_88AP510BP_B_MPP0_7,		
	DB_88AP510BP_B_MPP8_15,		
	DB_88AP510BP_B_MPP16_23	
	}}};

MV_DEV_CS_INFO db88f6781bpbInfoBoardDevCsInfo[] = 
		/*{deviceCS, params, devType, devWidth}*/			   
		{
		 {0, N_A, BOARD_DEV_SPI_FLASH, 8} 
		 /* TODO {1, N_A, BOARD_DEV_GPS, 8} */
		};


MV_BOARD_INFO db88f6781bpbInfo = {
	"DB-88AP510-BP_B",				/* boardName[MAX_BOARD_NAME_LEN] */
	DB_88AP510BP_B_BOARD_MPP_GROUP_TYPE_NUM,		/* numBoardMppGroupType */
	db88f6781bpbInfoBoardMppTypeInfo,
	DB_88AP510BP_B_BOARD_MPP_CONFIG_NUM,		/* numBoardMppConfig */
	db88f6781bpbInfoBoardMppConfigValue,
	0,						/* intsGppMaskLow */
	0,						/* intsGppMaskHigh */
	DB_88AP510BP_B_BOARD_DEVICE_CONFIG_NUM,		/* numBoardDevIf */
	db88f6781bpbInfoBoardDevCsInfo,
	DB_88AP510BP_B_BOARD_TWSI_DEF_NUM,		/* numBoardTwsiDev */
	db88f6781bpbInfoBoardTwsiDev,					
	DB_88AP510BP_B_BOARD_MAC_INFO_NUM,		/* numBoardMacInfo */
	db88f6781bpbInfoBoardMacInfo,
	DB_88AP510BP_B_BOARD_GPP_INFO_NUM,		/* numBoardGppInfo */
	db88f6781bpbInfoBoardGppInfo,
	DB_88AP510BP_B_BOARD_DEBUG_LED_NUM,		/* activeLedsNumber */
	db88f6781bpbInfoBoardDebugLedIf,
	1,						/* ledsPolarity */
	DB_88AP510BP_B_OE_LOW,				/* gppOutEnLow */
	DB_88AP510BP_B_OE_HIGH,				/* gppOutEnHigh */
	0xFFFFFFFF, 					/* gpp2OutEn */
	DB_88AP510BP_B_OE_VAL_LOW,			/* gppOutValLow */
	DB_88AP510BP_B_OE_VAL_HIGH,			/* gppOutValHigh */
	0, 						/* gpp2OutVal */
	0,						/* gppPolarityValLow */
	0, 						/* gppPolarityValHigh */
	0, 						/* gpp2PolarityVal */
	NULL						/* pSwitchInfo */
};

/* A0 PCAC board */

#define DB_88AP510_PCAC_BOARD_PCI_IF_NUM		0x0
#define DB_88AP510_PCAC_BOARD_TWSI_DEF_NUM		0x7
#define DB_88AP510_PCAC_BOARD_MAC_INFO_NUM		0x1
#define DB_88AP510_PCAC_BOARD_GPP_INFO_NUM		0x0
#define DB_88AP510_PCAC_BOARD_MPP_CONFIG_NUM		0x1
#define DB_88AP510_PCAC_BOARD_MPP_GROUP_TYPE_NUM	0x1
#define DB_88AP510_PCAC_BOARD_DEVICE_CONFIG_NUM		0x1
#define DB_88AP510_PCAC_BOARD_DEBUG_LED_NUM		0x4

MV_U8	db88f6781pcacInfoBoardDebugLedIf[] =
	{14,15,62,16};

/* TODO, see if need to add more */
MV_BOARD_TWSI_INFO	db88f6781pcacInfoBoardTwsiDev[] =
	/* {{MV_BOARD_DEV_CLASS	devClass, MV_U8	twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{
	{BOARD_DEV_TWSI_EXP, 0x20, ADDR7_BIT},
	{BOARD_DEV_TWSI_EXP, 0x21, ADDR7_BIT},
	{BOARD_DEV_TWSI_EXP, 0x27, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4C, ADDR7_BIT}, 
	{BOARD_DEV_TWSI_SATR, 0x4D, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4E, ADDR7_BIT},
	{BOARD_TWSI_BATT_CHARGER, 0x09, ADDR7_BIT}, 
	};

MV_BOARD_MAC_INFO db88f6781pcacInfoBoardMacInfo[] = 
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{
	{BOARD_MAC_SPEED_AUTO, 0x1}
	}; 

MV_BOARD_MPP_TYPE_INFO db88f6781pcacInfoBoardMppTypeInfo[] = 
	/* {{MV_BOARD_MPP_TYPE_CLASS	boardMppGroup1,
 		MV_BOARD_MPP_TYPE_CLASS	boardMppGroup2}} */
	{{MV_BOARD_AUTO, MV_BOARD_AUTO}
	}; 

MV_BOARD_GPP_INFO db88f6781pcacInfoBoardGppInfo[] = 
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{
	/* TODO */
	};

MV_BOARD_MPP_INFO	db88f6781pcacInfoBoardMppConfigValue[] = 
	{{{
	DB_88AP510_PCAC_MPP0_7,		
	DB_88AP510_PCAC_MPP8_15,		
	DB_88AP510_PCAC_MPP16_23	
	}}};

MV_DEV_CS_INFO db88f6781pcacInfoBoardDevCsInfo[] = 
		/*{deviceCS, params, devType, devWidth}*/			   
		{
		 {0, N_A, BOARD_DEV_SPI_FLASH, 8} 
		 /* TODO {1, N_A, BOARD_DEV_GPS, 8} */
		};


MV_BOARD_INFO db88f6781pcacInfo = {
	"DB-88AP510-PCAC",				/* boardName[MAX_BOARD_NAME_LEN] */
	DB_88AP510_PCAC_BOARD_MPP_GROUP_TYPE_NUM,		/* numBoardMppGroupType */
	db88f6781pcacInfoBoardMppTypeInfo,
	DB_88AP510_PCAC_BOARD_MPP_CONFIG_NUM,		/* numBoardMppConfig */
	db88f6781pcacInfoBoardMppConfigValue,
	0,						/* intsGppMaskLow */
	0,						/* intsGppMaskHigh */
	DB_88AP510_PCAC_BOARD_DEVICE_CONFIG_NUM,		/* numBoardDevIf */
	db88f6781pcacInfoBoardDevCsInfo,
	DB_88AP510_PCAC_BOARD_TWSI_DEF_NUM,		/* numBoardTwsiDev */
	db88f6781pcacInfoBoardTwsiDev,					
	DB_88AP510_PCAC_BOARD_MAC_INFO_NUM,		/* numBoardMacInfo */
	db88f6781pcacInfoBoardMacInfo,
	DB_88AP510_PCAC_BOARD_GPP_INFO_NUM,		/* numBoardGppInfo */
	db88f6781pcacInfoBoardGppInfo,
	DB_88AP510_PCAC_BOARD_DEBUG_LED_NUM,		/* activeLedsNumber */
	db88f6781pcacInfoBoardDebugLedIf,
	1,						/* ledsPolarity */
	DB_88AP510_PCAC_OE_LOW,				/* gppOutEnLow */
	DB_88AP510_PCAC_OE_HIGH,				/* gppOutEnHigh */
	0xFFFFFFFF, 					/* gpp2OutEn */
	DB_88AP510_PCAC_OE_VAL_LOW,			/* gppOutValLow */
	DB_88AP510_PCAC_OE_VAL_HIGH,			/* gppOutValHigh */
	0, 						/* gpp2OutVal */
	0,						/* gppPolarityValLow */
	0, 						/* gppPolarityValHigh */
	0, 						/* gpp2PolarityVal */
	NULL						/* pSwitchInfo */
};



MV_BOARD_INFO*	boardInfoTbl[] = 	{
					 &db88f6781Info,
					 &rd88f6781Info,
					 &rd88f6781AvngInfo,
					 &db88f6781Y0Info,
					 &rd88f6781Y0AvngInfo,
					 &rd88f6781X0PlugInfo,
					 &db88f6781bpbInfo,
					 &db88f6781pcacInfo,
					 &rd88ap510AvngInfo,
					};


