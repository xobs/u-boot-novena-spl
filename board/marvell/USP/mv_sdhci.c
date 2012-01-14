/*
 * Copyright 2010 Marvell Semiconductor, Inc
 * Saeed Bishara
 *
 * Base on fsl_esdhc:
 * Copyright 2007,2010 Freescale Semiconductor, Inc
 * Andy Fleming
 *
 * Based vaguely on the pxa mmc code:
 * (C) Copyright 2003
 * Kyle Harris, Nexus Technologies, Inc. kharris@nexus-tech.net
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>
#include <common.h>
#include <command.h>
#include <mmc.h>
#include <part.h>
#include <malloc.h>
#include <mmc.h>
#include <mv_sdhci.h>
#include <fdt_support.h>
#include <asm/io.h>

/* copied from linux sdhci.h */
/*
 * Controller registers
 */

#define SDHCI_DMA_ADDRESS	0x00

#define SDHCI_BLOCK_SIZE	0x04
#define  SDHCI_MAKE_BLKSZ(dma, blksz) (((dma & 0x7) << 12) | (blksz & 0xFFF))

#define SDHCI_BLOCK_COUNT	0x06

#define SDHCI_ARGUMENT		0x08

#define SDHCI_TRANSFER_MODE	0x0C
#define  SDHCI_TRNS_DMA		0x01
#define  SDHCI_TRNS_BLK_CNT_EN	0x02
#define  SDHCI_TRNS_ACMD12	0x04
#define  SDHCI_TRNS_READ	0x10
#define  SDHCI_TRNS_MULTI	0x20

#define SDHCI_COMMAND		0x0E
#define  SDHCI_CMD_RESP_MASK	0x03
#define  SDHCI_CMD_CRC		0x08
#define  SDHCI_CMD_INDEX	0x10
#define  SDHCI_CMD_DATA		0x20

#define  SDHCI_CMD_RESP_NONE	0x00
#define  SDHCI_CMD_RESP_LONG	0x01
#define  SDHCI_CMD_RESP_SHORT	0x02
#define  SDHCI_CMD_RESP_SHORT_BUSY 0x03

#define SDHCI_MAKE_CMD(c, f) (((c & 0xff) << 8) | (f & 0xff))

#define SDHCI_RESPONSE		0x10

#define SDHCI_BUFFER		0x20

#define SDHCI_PRESENT_STATE	0x24
#define  SDHCI_CMD_INHIBIT	0x00000001
#define  SDHCI_DATA_INHIBIT	0x00000002
#define  SDHCI_DOING_WRITE	0x00000100
#define  SDHCI_DOING_READ	0x00000200
#define  SDHCI_SPACE_AVAILABLE	0x00000400
#define  SDHCI_DATA_AVAILABLE	0x00000800
#define  SDHCI_CARD_PRESENT	0x00010000
#define  SDHCI_WRITE_PROTECT	0x00080000

#define SDHCI_HOST_CONTROL 	0x28
#define  SDHCI_CTRL_LED		0x01
#define  SDHCI_CTRL_4BITBUS	0x02
#define  SDHCI_CTRL_HISPD	0x04
#define  SDHCI_CTRL_DMA_MASK	0x18
#define   SDHCI_CTRL_SDMA	0x00
#define   SDHCI_CTRL_ADMA1	0x08
#define   SDHCI_CTRL_ADMA32	0x10
#define   SDHCI_CTRL_ADMA64	0x18

#define SDHCI_POWER_CONTROL	0x29
#define  SDHCI_POWER_ON		0x01
#define  SDHCI_POWER_180	0x0A
#define  SDHCI_POWER_300	0x0C
#define  SDHCI_POWER_330	0x0E

#define SDHCI_BLOCK_GAP_CONTROL	0x2A

#define SDHCI_WAKE_UP_CONTROL	0x2B

#define SDHCI_CLOCK_CONTROL	0x2C
#define  SDHCI_DIVIDER_SHIFT	8
#define  SDHCI_CLOCK_CARD_EN	0x0004
#define  SDHCI_CLOCK_INT_STABLE	0x0002
#define  SDHCI_CLOCK_INT_EN	0x0001

#define SDHCI_TIMEOUT_CONTROL	0x2E

#define SDHCI_SOFTWARE_RESET	0x2F
#define  SDHCI_RESET_ALL	0x01
#define  SDHCI_RESET_CMD	0x02
#define  SDHCI_RESET_DATA	0x04

#define SDHCI_INT_STATUS	0x30
#define SDHCI_INT_ENABLE	0x34
#define SDHCI_SIGNAL_ENABLE	0x38
#define  SDHCI_INT_RESPONSE	0x00000001
#define  SDHCI_INT_DATA_END	0x00000002
#define  SDHCI_INT_DMA_END	0x00000008
#define  SDHCI_INT_SPACE_AVAIL	0x00000010
#define  SDHCI_INT_DATA_AVAIL	0x00000020
#define  SDHCI_INT_CARD_INSERT	0x00000040
#define  SDHCI_INT_CARD_REMOVE	0x00000080
#define  SDHCI_INT_CARD_INT	0x00000100
#define  SDHCI_INT_ERROR	0x00008000
#define  SDHCI_INT_TIMEOUT	0x00010000
#define  SDHCI_INT_CRC		0x00020000
#define  SDHCI_INT_END_BIT	0x00040000
#define  SDHCI_INT_INDEX	0x00080000
#define  SDHCI_INT_DATA_TIMEOUT	0x00100000
#define  SDHCI_INT_DATA_CRC	0x00200000
#define  SDHCI_INT_DATA_END_BIT	0x00400000
#define  SDHCI_INT_BUS_POWER	0x00800000
#define  SDHCI_INT_ACMD12ERR	0x01000000
#define  SDHCI_INT_ADMA_ERROR	0x02000000

#define  SDHCI_INT_NORMAL_MASK	0x00007FFF
#define  SDHCI_INT_ERROR_MASK	0xFFFF8000

#define  SDHCI_INT_CMD_MASK	(SDHCI_INT_RESPONSE | SDHCI_INT_TIMEOUT | \
		SDHCI_INT_CRC | SDHCI_INT_END_BIT | SDHCI_INT_INDEX)
#define  SDHCI_INT_DATA_MASK	(SDHCI_INT_DATA_END | SDHCI_INT_DMA_END | \
		SDHCI_INT_DATA_AVAIL | SDHCI_INT_SPACE_AVAIL | \
		SDHCI_INT_DATA_TIMEOUT | SDHCI_INT_DATA_CRC | \
		SDHCI_INT_DATA_END_BIT | SDHCI_ADMA_ERROR)
#define SDHCI_INT_ALL_MASK	((unsigned int)-1)

#define SDHCI_ACMD12_ERR	0x3C

/* 3E-3F reserved */

#define SDHCI_CAPABILITIES	0x40
#define  SDHCI_TIMEOUT_CLK_MASK	0x0000003F
#define  SDHCI_TIMEOUT_CLK_SHIFT 0
#define  SDHCI_TIMEOUT_CLK_UNIT	0x00000080
#define  SDHCI_CLOCK_BASE_MASK	0x00003F00
#define  SDHCI_CLOCK_BASE_SHIFT	8
#define  SDHCI_MAX_BLOCK_MASK	0x00030000
#define  SDHCI_MAX_BLOCK_SHIFT  16
#define  SDHCI_CAN_DO_ADMA2	0x00080000
#define  SDHCI_CAN_DO_ADMA1	0x00100000
#define  SDHCI_CAN_DO_HISPD	0x00200000
#define  SDHCI_CAN_DO_SDMA	0x00400000
#define  SDHCI_CAN_VDD_330	0x01000000
#define  SDHCI_CAN_VDD_300	0x02000000
#define  SDHCI_CAN_VDD_180	0x04000000
#define  SDHCI_CAN_64BIT	0x10000000

/* 44-47 reserved for more caps */

#define SDHCI_MAX_CURRENT	0x48

/* 4C-4F reserved for more max current */

#define SDHCI_SET_ACMD12_ERROR	0x50
#define SDHCI_SET_INT_ERROR	0x52

#define SDHCI_ADMA_ERROR	0x54

/* 55-57 reserved */

#define SDHCI_ADMA_ADDRESS	0x58

/* 60-FB reserved */

#define SDHCI_SLOT_INT_STATUS	0xFC

#define SDHCI_HOST_VERSION	0xFE
#define  SDHCI_VENDOR_VER_MASK	0xFF00
#define  SDHCI_VENDOR_VER_SHIFT	8
#define  SDHCI_SPEC_VER_MASK	0x00FF
#define  SDHCI_SPEC_VER_SHIFT	0
#define   SDHCI_SPEC_100	0
#define   SDHCI_SPEC_200	1


/* Select the correct accessors depending on endianess */
#define sdhci_read8		__arch_getb
#define sdhci_write8(a,v)		__arch_putb(v,a)

#define sdhci_read16		__arch_getw
#define sdhci_write16(a,v)		__arch_putw(v,a)

#define sdhci_read32		__arch_getl
#define sdhci_write32(a,v)		__arch_putl(v,a)

inline void sdhci_clrbits8(u32 a,u8 mask)
{
	u8 tmp = sdhci_read8(a);
	sdhci_write8(a, tmp & ~mask);
}

inline void sdhci_setbits8(u32 a,u8 mask)
{
	u8 tmp = sdhci_read8(a);
	sdhci_write8(a, tmp | mask);
}


static void sdhci_dumpregs(u32 regs_base)
{
	printf(": ============== REGISTER DUMP ==============\n");

	printf(" Sys addr: 0x%08x | Version:  0x%08x\n",
		sdhci_read32(regs_base + SDHCI_DMA_ADDRESS),
		0/*sdhci_read16(host + SDHCI_HOST_VERSION)*/);
	printf(" Command: 0x%08x\n",
		sdhci_read16(regs_base + SDHCI_COMMAND));
	printf( ": Blk size: 0x%08x | Blk cnt:  0x%08x\n",
		sdhci_read16(regs_base + SDHCI_BLOCK_SIZE),
		sdhci_read16(regs_base + SDHCI_BLOCK_COUNT));
	printf( ": Argument: 0x%08x | Trn mode: 0x%08x\n",
		sdhci_read32(regs_base + SDHCI_ARGUMENT),
		sdhci_read16(regs_base + SDHCI_TRANSFER_MODE));
	printf( ": Present:  0x%08x | Host ctl: 0x%08x\n",
		sdhci_read32(regs_base + SDHCI_PRESENT_STATE),
		sdhci_read8(regs_base + SDHCI_HOST_CONTROL));
	printf( ": Power:    0x%08x | Blk gap:  0x%08x\n",
		sdhci_read8(regs_base + SDHCI_POWER_CONTROL),
		sdhci_read8(regs_base + SDHCI_BLOCK_GAP_CONTROL));
	printf( ": Wake-up:  0x%08x | Clock:    0x%08x\n",
		sdhci_read8(regs_base + SDHCI_WAKE_UP_CONTROL),
		sdhci_read16(regs_base + SDHCI_CLOCK_CONTROL));
	printf( ": Timeout:  0x%08x | Int stat: 0x%08x\n",
		sdhci_read8(regs_base + SDHCI_TIMEOUT_CONTROL),
		sdhci_read32(regs_base + SDHCI_INT_STATUS));
	printf( ": Int enab: 0x%08x | Sig enab: 0x%08x\n",
		sdhci_read32(regs_base + SDHCI_INT_ENABLE),
		sdhci_read32(regs_base + SDHCI_SIGNAL_ENABLE));
	printf( ": AC12 err: 0x%08x | Slot int: 0x%08x\n",
		sdhci_read16(regs_base + SDHCI_ACMD12_ERR),
		0/*sdhci_read16(regs_base + SDHCI_SLOT_INT_STATUS)*/);
	printf( ": Caps:     0x%08x | Max curr: 0x%08x\n",
		sdhci_read32(regs_base + SDHCI_CAPABILITIES),
		sdhci_read32(regs_base + SDHCI_MAX_CURRENT));
	printf( ": ===========================================\n");
}

/* Return the transfer mode for a given command and data packet */
u16 sdhci_get_xfer(struct mmc_cmd *cmd, struct mmc_data *data)
{
	u16 xfertyp = 0;

	if (data) {
		xfertyp = SDHCI_TRNS_BLK_CNT_EN;
		xfertyp |= SDHCI_TRNS_DMA;
		if (data->blocks > 1)
			xfertyp |= SDHCI_TRNS_MULTI;

		if (data->flags & MMC_DATA_READ)
			xfertyp |= SDHCI_TRNS_READ;
	}
	xfertyp |= SDHCI_TRNS_DMA;
	return xfertyp;
}

static int sdhci_setup_data(struct mmc *mmc, struct mmc_data *data)
{
	int timeout;
	struct mv_sdhci_cfg *cfg = (struct mv_sdhci_cfg *)mmc->priv;
	u32 regs_base = cfg->sdhci_base;

	if (data->flags & MMC_DATA_READ) {
		sdhci_write32(regs_base + SDHCI_DMA_ADDRESS, (u32)data->dest);
	} else {
		sdhci_write32(regs_base + SDHCI_DMA_ADDRESS, (u32)data->src);
	}

        sdhci_write16(regs_base + SDHCI_BLOCK_SIZE, SDHCI_MAKE_BLKSZ(7, data->blocksize));
        sdhci_write16(regs_base + SDHCI_BLOCK_COUNT, data->blocks);
	
	timeout = 14;
        sdhci_write8(regs_base + SDHCI_TIMEOUT_CONTROL, timeout);

	return 0;
}


/*
 * Sends a command out on the bus.  Takes the mmc pointer,
 * a command pointer, and an optional data pointer.
 */
static int
sdhci_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
	u16	xfertyp;
	uint	irqstat;
	struct mv_sdhci_cfg *cfg = (struct mv_sdhci_cfg *)mmc->priv;
	u32 regs_base = cfg->sdhci_base;
	u32	mask = SDHCI_CMD_INHIBIT | SDHCI_DATA_INHIBIT;
	int	timeout = 10;
	u32	flags = 0;

	sdhci_write32(regs_base + SDHCI_INT_STATUS, 0xffffffff);

	sync();

	if (!sdhci_read32(regs_base + SDHCI_PRESENT_STATE) & SDHCI_CARD_PRESENT)
	{
		printf("SDHCI error: card is not preset\n");
		return -1;
	}

	/* Wait for the bus to be idle */
        while (sdhci_read32(regs_base + SDHCI_PRESENT_STATE) & mask) {
                if (timeout == 0) {
                        printf("SDHCI: Controller never released "
			       "inhibit bit(s).\n");
                        return -1;
                }
                timeout--;
		udelay(1000);
        }

	/* Set up for a data transfer if we have one */
	if (data) {
		int err;
		err = sdhci_setup_data(mmc, data);
		if(err)
			return err;
	}

	/* Figure out the transfer arguments */
	xfertyp = sdhci_get_xfer(cmd, data);
	sdhci_write16(regs_base + SDHCI_TRANSFER_MODE, xfertyp);

	/* Set the argument mode */
	sdhci_write32(regs_base + SDHCI_ARGUMENT, cmd->cmdarg);


        if (!(cmd->resp_type & MMC_RSP_PRESENT))
                flags = SDHCI_CMD_RESP_NONE;
        else if (cmd->resp_type & MMC_RSP_136)
                flags = SDHCI_CMD_RESP_LONG;
        else if (cmd->resp_type & MMC_RSP_BUSY)
                flags = SDHCI_CMD_RESP_SHORT_BUSY;
        else
                flags = SDHCI_CMD_RESP_SHORT;

        if (cmd->resp_type & MMC_RSP_CRC)
                flags |= SDHCI_CMD_CRC;
        if (cmd->resp_type & MMC_RSP_OPCODE)
                flags |= SDHCI_CMD_INDEX;
        if (data)
                flags |= SDHCI_CMD_DATA;

//	printf("%s: cmdidx %x flags %x cmdarg %x xfer mode %x data %p\n", __func__,
//       cmd->cmdidx, flags, cmd->cmdarg, xfertyp, data);

        sdhci_write16(regs_base + SDHCI_COMMAND, SDHCI_MAKE_CMD(cmd->cmdidx, flags));

//	sdhci_dumpregs(regs_base);
	/* Wait for the command to complete */
	timeout = 1000000;
	while (!(sdhci_read32(regs_base + SDHCI_INT_STATUS) & SDHCI_INT_CMD_MASK)) {
		udelay(1);
		timeout--;
		if (timeout <= 0) {
			printf("mmc command timed out\n");
			sdhci_dumpregs(regs_base);
			return -1;
		}
	}

	irqstat = sdhci_read32(regs_base + SDHCI_INT_STATUS);

	if (irqstat & (SDHCI_INT_CRC|SDHCI_INT_END_BIT|SDHCI_INT_INDEX))
		return COMM_ERR;

	if (irqstat & SDHCI_INT_TIMEOUT)
		return TIMEOUT;

	/* Copy the response to the response buffer */
	if (cmd->resp_type & MMC_RSP_136) {
		u32 cmdrsp3, cmdrsp2, cmdrsp1, cmdrsp0;

		cmdrsp3 = sdhci_read32(regs_base + SDHCI_RESPONSE + 12);
		cmdrsp2 = sdhci_read32(regs_base + SDHCI_RESPONSE + 8);
		cmdrsp1 = sdhci_read32(regs_base + SDHCI_RESPONSE + 4);
		cmdrsp0 = sdhci_read32(regs_base + SDHCI_RESPONSE);
		cmd->response[0] = (cmdrsp3 << 8) | (cmdrsp2 >> 24);
		cmd->response[1] = (cmdrsp2 << 8) | (cmdrsp1 >> 24);
		cmd->response[2] = (cmdrsp1 << 8) | (cmdrsp0 >> 24);
		cmd->response[3] = (cmdrsp0 << 8);
	} else
		cmd->response[0] = sdhci_read32(regs_base + SDHCI_RESPONSE);

	/* Wait until all of the blocks are transferred */
	if (data) {
		do {
			irqstat = sdhci_read32(regs_base + SDHCI_INT_STATUS);

			if (irqstat & (SDHCI_INT_DATA_CRC | SDHCI_INT_DATA_END_BIT))
				return COMM_ERR;

			if (irqstat & SDHCI_INT_DATA_TIMEOUT)
				return TIMEOUT;
		} while (!(irqstat & SDHCI_INT_DATA_END) &&
				(sdhci_read32(regs_base + SDHCI_PRESENT_STATE) & SDHCI_CARD_PRESENT));
	}

	sdhci_write32(regs_base + SDHCI_INT_STATUS, 0xffffffff);

	return 0;
}

void set_clock(struct mmc *mmc, uint clock)
{
	int div, timeout;
	struct mv_sdhci_cfg *cfg = (struct mv_sdhci_cfg *)mmc->priv;
	u32 regs_base = cfg->sdhci_base;
	u16 clk;

	sdhci_write16(regs_base + SDHCI_CLOCK_CONTROL, 0);

	if (clock < mmc->f_min)
		clock = mmc->f_min;


        for (div = 1;div < 256;div *= 2) {
                if ((mmc->f_max / div) <= clock)
                        break;
        }
        div >>= 1;

        clk = div << SDHCI_DIVIDER_SHIFT;
	clk |= SDHCI_CLOCK_INT_EN;
	sdhci_write16(regs_base + SDHCI_CLOCK_CONTROL, clk);


        /* Wait max 20 ms */
        timeout = 20;
        while (!((clk = sdhci_read16(regs_base + SDHCI_CLOCK_CONTROL))
		 & SDHCI_CLOCK_INT_STABLE)) {
                if (timeout == 0) {
                        printf("SDHCI: Internal clock never stabilised.\n");

                        return;
                }
                timeout--;
                udelay(1000);
        }

        clk |= SDHCI_CLOCK_CARD_EN;
        sdhci_write16(regs_base + SDHCI_CLOCK_CONTROL, clk);
}

void set_power(struct mmc *mmc, u8 pwr)
{
	struct mv_sdhci_cfg *cfg = (struct mv_sdhci_cfg *)mmc->priv;
	u32 regs_base = cfg->sdhci_base;

	sdhci_write8(regs_base + SDHCI_POWER_CONTROL, pwr);
	pwr |= SDHCI_POWER_ON;
	sdhci_write8(regs_base + SDHCI_POWER_CONTROL, pwr);
	udelay(400);
}

static void sdhci_set_ios(struct mmc *mmc)
{
	struct mv_sdhci_cfg *cfg = (struct mv_sdhci_cfg *)mmc->priv;
	u32 regs_base = cfg->sdhci_base;

	/* Set the clock speed */
	set_clock(mmc, mmc->clock);
	/* Set the power */
	set_power(mmc, SDHCI_POWER_330);
	/* Set the bus width */
	if (mmc->bus_width == 4)
		sdhci_setbits8(regs_base + SDHCI_HOST_CONTROL, SDHCI_CTRL_4BITBUS);
	else
		sdhci_clrbits8(regs_base + SDHCI_HOST_CONTROL, SDHCI_CTRL_4BITBUS);
}

static void sdhci_reset(u32 regs_base)
{
	unsigned long timeout = 100; /* wait max 100 ms */
	u32	irq_mask;
	/* reset the controller */
	sdhci_write8(regs_base + SDHCI_SOFTWARE_RESET, SDHCI_RESET_ALL);

	/* hardware clears the bit when it is done */
	while ((sdhci_read8(regs_base + SDHCI_SOFTWARE_RESET) & SDHCI_RESET_ALL) && --timeout)
		udelay(1000);
	if (!timeout)
		printf("SDHCI: Reset never completed.\n");
	
	irq_mask = SDHCI_INT_BUS_POWER | SDHCI_INT_DATA_END_BIT |
                SDHCI_INT_DATA_CRC | SDHCI_INT_DATA_TIMEOUT | SDHCI_INT_INDEX |
                SDHCI_INT_END_BIT | SDHCI_INT_CRC | SDHCI_INT_TIMEOUT |
		SDHCI_INT_DATA_END | SDHCI_INT_RESPONSE | SDHCI_INT_CARD_INSERT |
		SDHCI_INT_CARD_REMOVE;

	sdhci_write32(regs_base + SDHCI_INT_ENABLE, irq_mask);
	sdhci_write32(regs_base + SDHCI_SIGNAL_ENABLE, irq_mask);
}

static int sdhci_init(struct mmc *mmc)
{
	struct mv_sdhci_cfg *cfg = (struct mv_sdhci_cfg *)mmc->priv;
	u32 regs_base = cfg->sdhci_base;
	int timeout = 400;
	int ret = 0;

	sdhci_reset(regs_base);

	while (!(sdhci_read32(regs_base + SDHCI_PRESENT_STATE) & SDHCI_CARD_PRESENT) &&
	       --timeout)
		udelay(1000);
	
	if (timeout <= 0)
		ret = NO_CARD_ERR;

	return ret;
}

int mv_sdhci_initialize(struct mv_sdhci_cfg *cfg)
{
	u32 regs_base;
	struct mmc *mmc;
	u32 caps;

	if (!cfg)
		return -1;

	mmc = malloc(sizeof(struct mmc));

	sprintf(mmc->name, "MV_SDHCI");
	regs_base = cfg->sdhci_base;

	/* First reset the SDHCI controller */
	sdhci_reset(regs_base);

	mmc->priv = cfg;
	mmc->send_cmd = sdhci_send_cmd;
	mmc->set_ios = sdhci_set_ios;
	mmc->init = sdhci_init;

	caps = sdhci_read32(regs_base + SDHCI_CAPABILITIES);

	if (caps & SDHCI_CAN_VDD_180)
		mmc->voltages |= MMC_VDD_165_195;
	if (caps & SDHCI_CAN_VDD_330)
		mmc->voltages |= MMC_VDD_33_34 | MMC_VDD_34_35;

	mmc->host_caps = MMC_MODE_4BIT;

	if (caps & SDHCI_CAN_DO_HISPD)
		mmc->host_caps |= MMC_MODE_HS_52MHz | MMC_MODE_HS;

	mmc->f_max = 50000000;
	mmc->f_min = mmc->f_max/256;
	mmc->f_min = 400000;
	mmc_register(mmc);

	return 0;
}

int mv_sdhci_mmc_init(u32 base)
{
	struct mv_sdhci_cfg *cfg;

	cfg = malloc(sizeof(struct mv_sdhci_cfg));
	memset(cfg, 0, sizeof(struct mv_sdhci_cfg));
	cfg->sdhci_base = base;
	return mv_sdhci_initialize(cfg);
}
