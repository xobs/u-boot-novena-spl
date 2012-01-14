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
#ifndef __INCMVCamControllerRegsH
#define __INCMVCamControllerRegsH

#ifdef __cplusplus
extern "C" {
#endif

#include "mvSysCamConfig.h"

#define MV_CAM_Y0_BASE		0x4000
#define MV_CAM_Y1_BASE		0x4004
#define MV_CAM_Y2_BASE		0x4008
#define MV_CAM_U0_BASE		0x400C
#define MV_CAM_U1_BASE		0x4010
#define MV_CAM_U2_BASE		0x4014
#define MV_CAM_V0_BASE		0x4018
#define MV_CAM_V1_BASE		0x401C
#define MV_CAM_V2_BASE		0x4020

#define MV_CAM_IRQ_RAW_STATUS	0x4028	/* IRQ RAW Status */
#define  MV_CAM_IRQ_EOF(i)		(1 << (i))
#define  MV_CAM_IRQ_SOF(i)		(8 << (i))
#define  MV_CAM_IRQ_FIFO_FULL	(1 << 6)
#define  MV_CAM_IRQ_TWSI_WRITE 	(1 << 16)
#define  MV_CAM_IRQ_TWSI_READ 	(1 << 17)
#define  MV_CAM_IRQ_TWSI_ERROR 	(1 << 18)

#define MV_CAM_IRQ_MASK		0x402c
#define MV_CAM_IRQ_STATUS	0x4030

#define MV_CAM_IMAGE_PITCH	0x4024

#define MV_CAM_IMAGE_SIZE	0x4034
#define  MV_CAM_IMAGE_SIZE_V_MASK	  0x1fff0000
#define  MV_CAM_IMAGE_SIZE_V_OFFSET	  16
#define	 MV_CAM_IMAGE_SIZE_H_MASK	  0x00003fff
#define MV_CAM_IMAGE_OFFSET	0x4038

#define MV_CAM_CONTROL0		0x403c
#define  MV_CAM_C0_ENABLE	0x1
#define  MV_CAM_C0_RGB		0xA0
#define  MV_CAM_C0_YUV		0x0
#define  MV_CAM_C0_RAW		0x140
#define  MV_CAM_C0_RGB444	0x800
#define  MV_CAM_C0_RGB555	0x200
#define  MV_CAM_C0_RGB565	0x0
#define  MV_CAM_C0_RGB444_ORD	0x4
#define  MV_CAM_C0_RGB555_ORD	0xC
#define  MV_CAM_C0_RGB565_ORD	0xC
#define  MV_CAM_C0_YUV_FMT_PACKED  0x0
#define  MV_CAM_C0_YUV422_ENDIANESS 0x0


#define MV_CAM_CONTROL1		0x4040
#define  MV_CAM_C1_PWRDWN	(1 << 28)  /* Power down */
#define  MV_CAM_C1_DMABRSTSEL_MASK	(3 << 25)

#define MV_CAM_CLKCTRL		0x4088	/* Clock control */

#define MV_CAM_GPR		0x40B4	/* General Purpose */
#define  MV_CAM_GPR_CTL0	1
#define  MV_CAM_GPR_CTL1	(1 << 1)
#define  MV_CAM_GPR_CTL0PADEN	(1 << 4)
#define  MV_CAM_GPR_CTL1PADEN	(1 << 5)

#define MV_CAM_TWSI0		0x40B8
#define  MV_CAM_TWSI0_EN	1
#define  MV_CAM_TWSI0_MODE	(1 << 1)
#define  MV_CAM_TWSI0_CLKDIV    (x << 10)
#define  MV_CAM_TWSI0_RDSTP	(1 << 23)

#define MV_CAM_TWSI1		0x40BC
#define MV_CAM_TWSI1_READ	(1 << 24)
#define MV_CAM_TWSI1_WRITE	(1 << 25)
#define MV_CAM_TWSI1_VALID	(1 << 26)
#define MV_CAM_TWSI1_ERROR	(1 << 27)

/* Address decoding register.	*/
#define MV_CAM_SDIO_MAX_ADDR_DECODE_WIN 4

#define MV_CAM_SDIO_WIN_CTRL_REG(win)        (MV_CAM_SDIO_REGS_OFFSET + 0x8030 + ((win)<<4))
#define MV_CAM_SDIO_WIN_BASE_REG(win)        (MV_CAM_SDIO_REGS_OFFSET + 0x8034 + ((win)<<4))

/* BITs in Windows 0-3 Control and Base Registers */
#define MV_CAM_SDIO_WIN_ENABLE_BIT               0
#define MV_CAM_SDIO_WIN_ENABLE_MASK              (1<<MV_CAM_SDIO_WIN_ENABLE_BIT)

#define MV_CAM_SDIO_WIN_TARGET_OFFSET            4
#define MV_CAM_SDIO_WIN_TARGET_MASK              (0xF<<MV_CAM_SDIO_WIN_TARGET_OFFSET)

#define MV_CAM_SDIO_WIN_ATTR_OFFSET              8
#define MV_CAM_SDIO_WIN_ATTR_MASK                (0xFF<<MV_CAM_SDIO_WIN_ATTR_OFFSET)

#define MV_CAM_SDIO_WIN_SIZE_OFFSET              16
#define MV_CAM_SDIO_WIN_SIZE_MASK                (0xFFFF<<MV_CAM_SDIO_WIN_SIZE_OFFSET)

#define MV_CAM_SDIO_WIN_BASE_OFFSET              16
#define MV_CAM_SDIO_WIN_BASE_MASK                (0xFFFF<<MV_CAM_SDIO_WIN_BASE_OFFSET)
#define MV_CAM_SDIO_WIN_SIZE_ALIGN              _64K

#ifdef __cplusplus
}
#endif


#endif /* __INCMVCamControllerRegsH */

