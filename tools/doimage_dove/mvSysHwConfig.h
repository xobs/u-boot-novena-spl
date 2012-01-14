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

#ifndef _INC_MVSYSHWCONFIG_H
#define _INC_MVSYSHWCONFIG_H


/* includes */
#define _1K         			0x00000400
#define _2K         			0x00000800
#define _4K         			0x00001000
#define _8K         			0x00002000
#define _12K				0x00003000
#define _16K        			0x00004000
#define _32K        			0x00008000
#define _64K        			0x00010000
#define _128K       			0x00020000
#define _256K       			0x00040000
#define _512K       			0x00080000
#define _1M         			0x00100000	
#define _2M         			0x00200000
#define _4M         			0x00400000
#define _8M         			0x00800000
#define _16M        			0x01000000
#define _32M        			0x02000000
#define _64M        			0x04000000
#define _128M       			0x08000000
#define _256M       			0x10000000
#define _512M       			0x20000000
#define _1G         			0x40000000
#define _2G        			0x80000000


#define _100MHZ				100000000
#define _125MHZ				125000000
#define _133MHZ				133333333
#define _144MHZ				144000000
#define _150MHZ				150000000
#define _166MHZ				166666667
#define _178MHZ				178000000
#define _183MHZ				183333333
#define _187MHZ				187000000
#define _192MHZ				192000000
#define _194MHZ				194000000
#define _200MHZ				200000000
#define _233MHZ				233333333
#define _250MHZ				250000000
#define _266MHZ				266666667
#define _300MHZ				300000000
#define _333MHZ				333333333
#define _366MHZ				366666667
#define _400MHZ				400000000
#define _433MHZ				433333333


#define BOOTROM_SIZE			_16K
#define BOOTROM_BASE    		0xffff0000

#define INTER_REGS_BASE			0xD0000000

#define CRYPTO_SRAM_BASE		0xC8010000
#define CRYPTO_SRAM_SIZE		_2K

#define SPI_FLASH_BASE			0xE8000000
#define SPI_FLASH_SIZE			_128M

#define NAND_FLASH_BASE			0xD8000000
#define NAND_FLASH_SIZE			_128M

#endif /* _INC_MVSYSHWCONFIG_H */
