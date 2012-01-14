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

#include <config.h>
#include <common.h>
#include <command.h>
#include <pci.h>
#include <net.h>
#include <usb.h>

#include "mvCommon.h"
#include "mvCtrlEnvLib.h"
#if defined(MV_INC_BOARD_NOR_FLASH)
#include "norflash/mvFlash.h"
#endif

#if defined(MV_INCLUDE_GIG_ETH)
#include "eth-phy/mvEthPhy.h"
#endif

#if defined(MV_INCLUDE_PEX)
#include "pci-if/pex/mvPex.h"
#endif

#if defined(MV_INCLUDE_PDMA)
#include "pdma/mvPdma.h"
#include "mvSysPdmaApi.h"
#endif

#if defined(MV_INCLUDE_XOR)
#include "xor/mvXorRegs.h"
#include "xor/mvXor.h"
#endif

#if defined(MV_INCLUDE_PMU)
#include "pmu/mvPmuRegs.h"
#endif

#include "cntmr/mvCntmrRegs.h"

#if defined(CONFIG_CMD_NAND)
#include <nand.h>
#endif

#ifdef CONFIG_CMD_SF
#include <spi_flash.h>
extern struct spi_flash *flash;
#endif
#define USB_DEVICE_VENDER_ID		"Kingston"
#define IDE_DEVICE_VENDER_ID		"WDC WD160"
#define SF_DEVICE_VENDER_ID			"MX25L3205D"
#define NAND_DEVICE_NAME			"nand0"

#define MAX_MAN_TEST_COUNT			10
#define MAN_FUNC_ID_SG				0
#define MAN_FUNC_ID_NAND			1
#define MAN_FUNC_ID_SF				2 
#define MAN_FUNC_ID_IDE				3
#define MAN_FUNC_ID_I2C				4
#define MAN_FUNC_ID_USB				5
#define MAN_FUNC_ID_PCI				6
#define MAN_FUNC_ID_MMC				7
#define MAN_FUNC_ID_DRAM			8
#define MAN_FUNC_ID_RTC				9

//----------------------------------------------------------------------------
typedef int (*pfunc_man_test)(struct cmd_tbl_s *, int, int, char *[]);

//----------------------------------------------------------------------------
int mv_man_register_func(u8 func_id,pfunc_man_test pfunc);
int mv_man_init(void);
//----------------------------------------------------------------------------
extern struct usb_device usb_dev[USB_MAX_DEVICE];
extern block_dev_desc_t ide_dev_desc[CONFIG_SYS_IDE_MAXDEVICE];
#ifdef CONFIG_CMD_SF
extern struct spi_flash *flash;
#endif
#if defined(CONFIG_CMD_NAND)
extern nand_info_t nand_info[CONFIG_SYS_MAX_NAND_DEVICE];
#endif
extern unsigned char g_pciDeviceStatus;//for manufacture test
extern unsigned char g_mmcStatus; //for manufacture test
extern MV_STATUS g_netStatus; //for manufacture test
#if defined(CONFIG_CMD_NAND)
extern int g_nand_bad_block_count;
extern uint64_t g_nand_good_block_addr;//added by braver for manufacture test for nand to record good blocks.
#endif
extern int sg_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
#if defined(CONFIG_CMD_NAND)
extern int do_nand(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]);
#endif
#ifdef CONFIG_CMD_SF
extern int do_spi_flash(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
#endif
extern int do_ide (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int do_i2c(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]);
extern int do_usb(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int sp_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int do_mmcops(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int do_mem_mtest (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int do_mmcinfo (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int do_date (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);

