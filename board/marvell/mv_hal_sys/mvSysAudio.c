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

#include "mvTypes.h"
#include "mvCommon.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "audio/mvAudio.h"
#include "twsi/mvTwsi.h"
#include "audio/mvAudioRegs.h"


/*******************************************************************************
* mvSysAudioInit - Initialize the Audio subsystem
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
MV_VOID mvSysAudioInit(MV_U8 unit)
{
	MV_AUDIO_HAL_DATA halData;
	MV_UNIT_WIN_INFO addrWinMap[MAX_TARGETS + 1];
	MV_STATUS status;

	status = mvCtrlAddrWinMapBuild(addrWinMap, MAX_TARGETS + 1);
	if(status == MV_OK)
		status = mvAudioWinInit(unit, addrWinMap);

	if(status == MV_OK) {
		halData.tclk = mvBoardTclkGet();
		mvAudioHalInit(unit,&halData);
	}

	return;
}

/*******************************************************************************
* mvAudioAddrDecShow - Print the AUDIO address decode map.
*
* DESCRIPTION:
*		This function print the AUDIO address decode map.
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
MV_VOID mvAudioUnitAddrDecShow(int unit)
{
	MV_UNIT_WIN_INFO win;
	int i;

	mvOsOutput( "\n" );
	mvOsOutput( "AUDIO:\n" );
	mvOsOutput( "----\n" );

	for( i = 0; i < MV_AUDIO_MAX_ADDR_DECODE_WIN; i++ )
	{
            memset( &win, 0, sizeof(MV_UNIT_WIN_INFO) );

	    mvOsOutput( "win%d - ", i );

	    if( mvAudioWinRead(unit, i, &win) == MV_OK )
	    {
	        if( win.enable )
	        {
                    mvOsOutput( "%s base %08x, ",
                    mvCtrlTargetNameGet(mvCtrlTargetByWinInfoGet(&win)), win.addrWin.baseLow );
                    mvOsOutput( "...." );

                    mvSizePrint( win.addrWin.size );
    
		    mvOsOutput( "\n" );
                }
		else
		mvOsOutput( "disable\n" );
	    }
	}
}

MV_VOID mvAudioAddrDecShow(MV_VOID)
{
	int unit;

	for (unit = 0; unit < MV_AUDIO_MAX_UNITS; unit++)
	{
		if (MV_FALSE == mvCtrlPwrClckGet(AUDIO_UNIT_ID, unit))
			continue;
		mvAudioUnitAddrDecShow(unit);
	} 
}



/*******************************************************************************
* mvSysAudioCodecRegRead
*
* DESCRIPTION:
*	System interface for reading an Audio codec register.
*
* INPUT:
*       codecHandle: Handle passed by OS glue by which an audio codec is
*		     identified.
*       regOffset:   Offset of codec register to be read.
*
* OUTPUT:
*       regData:     Register data
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*
*******************************************************************************/
MV_STATUS mvSysAudioCodecRegRead(MV_VOID *codecHandle, MV_U32  regOffset, MV_U32 *regData)
{
	MV_TWSI_SLAVE slave;

	slave.slaveAddr.address = mvBoardA2DTwsiAddrGet(0);
	slave.slaveAddr.type = mvBoardA2DTwsiAddrTypeGet(0);
    	slave.validOffset = MV_TRUE;
    	slave.offset = regOffset;
    	slave.moreThen256 = MV_FALSE;	

	return mvTwsiRead(mvBoardA2DTwsiChanNumGet(0), &slave, (MV_U8*)regData, 1);
}


/*******************************************************************************
* mvSysAudioCodecRegWrite
*
* DESCRIPTION:
*	System interface for writing an Audio codec register.
*
* INPUT:
*       codecHandle: Handle passed by OS glue by which an audio codec is
*		     identified.
*       regOffset:   Offset of codec register to be written. 
*       regData:     Register data to write.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*
*******************************************************************************/
MV_STATUS mvSysAudioCodecRegWrite(MV_VOID *codecHandle, MV_U32  regOffset, MV_U32 regData)
{
	MV_TWSI_SLAVE slave;

	slave.slaveAddr.address = mvBoardA2DTwsiAddrGet(0);
	slave.slaveAddr.type = mvBoardA2DTwsiAddrTypeGet(0);
    	slave.validOffset = MV_TRUE;
    	slave.offset = regOffset;
    	slave.moreThen256 = MV_FALSE;	

	return	mvTwsiWrite(mvBoardA2DTwsiChanNumGet(0), &slave, (MV_U8*)&regData, 1);
}



