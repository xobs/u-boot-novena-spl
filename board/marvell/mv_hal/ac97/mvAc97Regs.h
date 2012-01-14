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
#ifndef __INCMVAc97RegsH
#define __INCMVAc97RegsH

#ifdef __cplusplus
extern "C" {
#endif

#include "mvSysAc97Config.h"

#define MV_AC97_PCM_OUT_CTRL_REG		(MV_AC97_REGS_BASE + 0x0000)
#define MV_AC97_PCM_IN_CTRL_REG			(MV_AC97_REGS_BASE + 0x0004)
#define MV_AC97_MIC_IN_CTRL_REG			(MV_AC97_REGS_BASE + 0x0008)
#define MV_AC97_GLOBAL_CTRL_REG			(MV_AC97_REGS_BASE + 0x000C)
#define MV_AC97_PCM_OUT_STATUS_REG		(MV_AC97_REGS_BASE + 0x0010)
#define MV_AC97_PCM_IN_STATUS_REG		(MV_AC97_REGS_BASE + 0x0014)
#define MV_AC97_MIC_IN_STATUS_REG		(MV_AC97_REGS_BASE + 0x0018)
#define MV_AC97_GLOBAL_STATUS_REG		(MV_AC97_REGS_BASE + 0x001C)
#define MV_AC97_CODEC_ACCESS_REG		(MV_AC97_REGS_BASE + 0x0020)
#define MV_AC97_PCM_SURR_OUT_CTRL_REG		(MV_AC97_REGS_BASE + 0x0024)
#define MV_AC97_PCM_SURR_OUT_STATUS_REG		(MV_AC97_REGS_BASE + 0x0028)
#define MV_AC97_PCM_SURR_DATA_REG		(MV_AC97_REGS_BASE + 0x002C)
#define MV_AC97_PCM_CNTR_LFE_CTRL_REG		(MV_AC97_REGS_BASE + 0x0030)
#define MV_AC97_PCM_CNTR_LFE_STATUS_REG		(MV_AC97_REGS_BASE + 0x0034)
#define MV_AC97_PCM_CNTR_LFE_DATA_REG		(MV_AC97_REGS_BASE + 0x0038)
#define MV_AC97_PCM_DATA_REG			(MV_AC97_REGS_BASE + 0x0040)
#define MV_AC97_MIC_IN_DATA_REG			(MV_AC97_REGS_BASE + 0x0060)
#define MV_AC97_MODEM_OUT_CTRL_REG		(MV_AC97_REGS_BASE + 0x0100)
#define MV_AC97_MODEM_IN_CTRL_REG		(MV_AC97_REGS_BASE + 0x0108)
#define MV_AC97_MODEM_OUT_STATUS_REG		(MV_AC97_REGS_BASE + 0x0110)
#define MV_AC97_MODEM_IN_STATUS_REG		(MV_AC97_REGS_BASE + 0x0118)
#define MV_AC97_MODEM_DATA_REG			(MV_AC97_REGS_BASE + 0x0140)

#define MV_AC97_PRI_AUDIO_CODEC_REG		(MV_AC97_REGS_BASE + 0x0200)
#define MV_AC97_SEC01_AUDIO_CODEC_REG		(MV_AC97_REGS_BASE + 0x0300)
#define MV_AC97_SEC10_AUDIO_CODEC_REG		(MV_AC97_REGS_BASE + 0x0800)
#define MV_AC97_SEC11_AUDIO_CODEC_REG		(MV_AC97_REGS_BASE + 0x0900)
#define MV_AC97_PRI_MODEM_CODEC_REG		(MV_AC97_REGS_BASE + 0x0400)
#define MV_AC97_SEC01_MODEM_CODEC_REG		(MV_AC97_REGS_BASE + 0x0500)
#define MV_AC97_SEC10_MODEM_CODEC_REG		(MV_AC97_REGS_BASE + 0x0600)
#define MV_AC97_SEC11_MODEM_CODEC_REG		(MV_AC97_REGS_BASE + 0x0700)


/* Fifo interrupt bits.			*/
#define AC97_FIFO_SERV_REQ_INT_OFFS	1
#define AC97_FIFO_SERV_REQ_INT_MASK	(1 << AC97_FIFO_SERV_REQ_INT_OFFS)
#define AC97_FIFO_ERR_INT_OFFS		3
#define AC97_FIFO_ERR_INT_MASK		(1 << AC97_FIFO_ERR_INT_OFFS)

/* Global Status register bits.		*/
#define AC97_GLB_PCODEC_READY_OFFS	8
#define AC97_GLB_PCODEC_READY_MASK	(1 << AC97_GLB_PCODEC_READY_OFFS)
#define AC97_GLB_SCODEC_READY_OFFS	9
#define AC97_GLB_SCODEC_READY_MASK	(1 << AC97_GLB_SCODEC_READY_OFFS)
#define AC97_GLB_STATUS_DONE_OFFS	18
#define AC97_GLB_STATUS_DONE_MASK	(1 << AC97_GLB_STATUS_DONE_OFFS)
#define AC97_GLB_CMND_DONE_OFFS		19
#define AC97_GLB_CMND_DONE_MASK		(1 << AC97_GLB_CMND_DONE_OFFS)

/* Global control register bits.	*/
#define AC97_GLB_CTRL_COLD_RESET_OFFS	1
#define AC97_GLB_CTRL_COLD_RESET_MASK	(1 << AC97_GLB_CTRL_COLD_RESET_OFFS)
#define AC97_GLB_CTRL_WARM_RESET_OFFS	2
#define AC97_GLB_CTRL_WARM_RESET_MASK	(1 << AC97_GLB_CTRL_WARM_RESET_OFFS)
#define AC97_GLB_CTRL_DMA_EN_OFFS	24
#define AC97_GLB_CTRL_DMA_EN_MASK	(1 << AC97_GLB_CTRL_DMA_EN_OFFS)
#define AC97_GLB_CTRL_INT_CLK_EN_OFFS	31
#define AC97_GLB_CTRL_INT_CLK_EN_MASK	(1 << AC97_GLB_CTRL_INT_CLK_EN_OFFS)

/* AC97 Codec related registers.	*/
#define AC97_CODEC_GPIO_STATUS_REG	0x54


#ifdef __cplusplus
}
#endif


#endif /* __INCMVAc97RegsH */

