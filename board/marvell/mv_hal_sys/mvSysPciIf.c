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
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "pci-if/mvPciIf.h"
#include "pci-if/pex/mvPex.h"
#include "pci-if/pex/mvPexRegs.h"

MV_STATUS mvPexTargetWinGet(MV_U32 pexIf, MV_U32 winNum, MV_PEX_DEC_WIN *pAddrDecWin);


/*******************************************************************************
* mvSysPciIfInit - Initialize the PCI subsystem
*
* DESCRIPTION:
*
* INPUT:
*       None
* OUTPUT:
*		None
* RETURN:
*       None
*
*******************************************************************************/
MV_STATUS mvSysPciIfInit(MV_U32 pciIf, PCI_IF_MODE pciIfmode)
{
	MV_PCIIF_HAL_DATA halData;
	MV_UNIT_WIN_INFO addrWinMap[MAX_TARGETS + 1];
	MV_STATUS status;

	status = mvCtrlAddrWinMapBuild(addrWinMap, MAX_TARGETS + 1);
	if(status == MV_OK)
		status = mvPciIfWinInit(pciIf, pciIfmode, addrWinMap);

	if(status == MV_OK) {
		halData.ctrlModel = mvCtrlModelGet();
		halData.maxPexIf = mvCtrlPexMaxIfGet();
		status = mvPciIfInit(pciIf, pciIfmode, &halData);
	}

	return status;
}

/*******************************************************************************
* mvPexAddrDecShow - Print the PEX address decode map (BARs and windows).
*
* DESCRIPTION:
*		This function print the PEX address decode map (BARs and windows).
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
MV_VOID mvPexAddrDecShow(MV_VOID)
{
	MV_PEX_BAR pexBar;
	MV_PEX_DEC_WIN win;
	MV_U32 pexIf;
	MV_U32 bar,winNum;

	for( pexIf = 0; pexIf < mvCtrlPexMaxIfGet(); pexIf++ )
	{
		if (MV_FALSE == mvCtrlPwrClckGet(PEX_UNIT_ID, pexIf)) continue;
		mvOsOutput( "\n" );
		mvOsOutput( "PEX%d:\n", pexIf );
		mvOsOutput( "-----\n" );

		mvOsOutput( "\nPex Bars \n\n");

		for( bar = 0; bar < PEX_MAX_BARS; bar++ ) 
		{
			memset( &pexBar, 0, sizeof(MV_PEX_BAR) );

			mvOsOutput( "%s ", pexBarNameGet(bar) );

			if( mvPexBarGet( pexIf, bar, &pexBar ) == MV_OK )
			{
				if( pexBar.enable )
				{
                    			mvOsOutput( "base %08x, ", pexBar.addrWin.baseLow );
                    			mvSizePrint( pexBar.addrWin.size );
                    			mvOsOutput( "\n" );
				}
				else
					mvOsOutput( "disable\n" );
			}
		}
		mvOsOutput( "\nPex Decode Windows\n\n");

		for( winNum = 0; winNum < PEX_MAX_TARGET_WIN - 2; winNum++)
		{
			memset( &win, 0,sizeof(MV_PEX_DEC_WIN) );

			mvOsOutput( "win%d - ", winNum );

			if ( mvPexTargetWinGet(pexIf,winNum,&win) == MV_OK)
			{
				if (win.winInfo.enable)
				{
					/* In MV88F6781 there is no differentiation between different		*/
					/* DRAM Chip Selects, they all use the same target ID and attributes.	*/
					/* So in order to print correctly we use i as the target.		*/
					/* When i is 1 it is SDRAM_CS1 etc.			 		*/
					if (mvCtrlTargetByWinInfoGet(&win.winInfo) != SDRAM_CS0) {
						mvOsOutput( "%s base %08x, ",
							mvCtrlTargetNameGet(mvCtrlTargetByWinInfoGet(&win.winInfo)), 
							win.winInfo.addrWin.baseLow );
					}
					else {
						mvOsOutput( "%s base %08x, ",
							mvCtrlTargetNameGet(winNum), win.winInfo.addrWin.baseLow );
					}
					mvOsOutput( "...." );
					mvSizePrint( win.winInfo.addrWin.size );

					mvOsOutput( "\n" );
				}
				else
					mvOsOutput( "disable\n" );
			}
		}
	
		memset( &win, 0,sizeof(MV_PEX_DEC_WIN) );

		mvOsOutput( "default win - " );

		if ( mvPexTargetWinGet(pexIf, MV_PEX_WIN_DEFAULT, &win) == MV_OK)
		{
			mvOsOutput( "%s ",
			mvCtrlTargetNameGet(win.target) );
			mvOsOutput( "\n" );
		}
		memset( &win, 0,sizeof(MV_PEX_DEC_WIN) );

		mvOsOutput( "Expansion ROM - " );

		if ( mvPexTargetWinGet(pexIf, MV_PEX_WIN_EXP_ROM, &win) == MV_OK)
		{
			mvOsOutput( "%s ",
			mvCtrlTargetNameGet(win.target) );
			mvOsOutput( "\n" );
		}
	}
}

/*******************************************************************************
* mvPexTargetWinGet - Get PEX to peripheral target address window
*
* DESCRIPTION:
*		Get the PEX to peripheral target address window BAR.
*
* INPUT:
*       pexIf - PEX interface number.
*       bar   - BAR to be accessed by slave.
*
* OUTPUT:
*       pAddrBarWin - PEX target window information data structure.
*
* RETURN:
*       MV_BAD_PARAM for bad parameters ,MV_ERROR on error ! otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPexTargetWinGet(MV_U32 pexIf, MV_U32 winNum, 
                            MV_PEX_DEC_WIN *pAddrDecWin)
{
	MV_TARGET_ATTRIB targetAttrib;
	MV_DEC_REGS decRegs;

	PEX_WIN_REG_INFO winRegInfo;

	/* Parameter checking   */
	if(pexIf >= mvCtrlPexMaxIfGet())
	{
		mvOsPrintf("mvPexTargetWinGet: ERR. Invalid PEX interface %d\n", pexIf);
		return MV_BAD_PARAM;
	}

	if (winNum >= PEX_MAX_TARGET_WIN)
	{
		mvOsPrintf("mvPexTargetWinGet: ERR. Invalid PEX winNum %d\n", winNum);
		return MV_BAD_PARAM;

	}

	/* get the pex Window registers offsets */
	pexWinRegInfoGet(pexIf,winNum,&winRegInfo);

	/* read base register*/
	if (winRegInfo.baseLowRegOffs)
	{
		decRegs.baseReg = MV_REG_READ(winRegInfo.baseLowRegOffs);
	}
	else
	{
		decRegs.baseReg = 0;
	}

	/* read size reg */
	if (winRegInfo.sizeRegOffs)
	{
		decRegs.sizeReg = MV_REG_READ(winRegInfo.sizeRegOffs);
	}
	else
	{
		decRegs.sizeReg =0;
	}

	if (MV_OK != mvCtrlRegToAddrDec(&decRegs,&(pAddrDecWin->winInfo.addrWin)))
	{
		mvOsPrintf("mvPexTargetWinGet: mvCtrlRegToAddrDec Failed \n");
		return MV_ERROR;

	}

	if (decRegs.sizeReg & PXWCR_WIN_EN)
	{
		pAddrDecWin->winInfo.enable = MV_TRUE;
	}
	else
	{
		pAddrDecWin->winInfo.enable = MV_FALSE;	  

	}


	#if 0
    	if (-1 == pAddrDecWin->addrWin.size)
	{
		return MV_ERROR;
	}
	#endif


	/* get target bar */
	if ((decRegs.sizeReg & PXWCR_WIN_BAR_MAP_MASK) == PXWCR_WIN_BAR_MAP_BAR1 )
	{
		pAddrDecWin->targetBar = 1;
	} 
	else if ((decRegs.sizeReg & PXWCR_WIN_BAR_MAP_MASK) == 
			 PXWCR_WIN_BAR_MAP_BAR2 )
	{
		pAddrDecWin->targetBar = 2;
	}

	/* attrib and targetId */
	pAddrDecWin->winInfo.attrib = (decRegs.sizeReg & PXWCR_ATTRIB_MASK) >> PXWCR_ATTRIB_OFFS;
	pAddrDecWin->winInfo.targetId = (decRegs.sizeReg & PXWCR_TARGET_MASK) >> PXWCR_TARGET_OFFS;

	targetAttrib.attrib = pAddrDecWin->winInfo.attrib;
	targetAttrib.targetId = pAddrDecWin->winInfo.targetId;

	pAddrDecWin->target = mvCtrlTargetGet(&targetAttrib);

	return MV_OK;
}


