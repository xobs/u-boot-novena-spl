/*
 * Copyright (C) 2010-2011 Freescale Semiconductor, Inc.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/mx6-ddr.h>
#include <asm/arch/mx6-pins.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/sys_proto.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/imx-common/iomux-v3.h>
#include <asm/imx-common/mxc_i2c.h>
#include <asm/arch/clock.h>
#include <i2c.h>

#ifndef CONFIG_SPL_I2C_SUPPORT
#error "No SPL I2C support"
#endif

DECLARE_GLOBAL_DATA_PTR;

#define SPD_NUMBYTES         0
#define SPD_REVISION         1
#define SPD_DRAM_DEVICE_TYPE 2
#define SPD_MODULE_TYPE      3
#define SPD_SDRAM_DENSITY_AND_BANKS 4
#define SPD_SDRAM_ADDRESSING 5
#define SPD_NOMINAL_VOLTAGE  6
#define SPD_ORGANIZATION     7
#define SPD_BUS_WIDTH        8

#define SPD_MTB_DIVIDEND     10
#define SPD_MTB_DIVISOR      11
#define SPD_TCKMIN           12

#define SPD_CL_LSB           14
#define SPD_CL_MSB           15
#define SPD_TAAMIN           16
#define SPD_TWRMIN           17
#define SPD_TRCDMIN          18
#define SPD_TRRDMIN          19
#define SPD_TRPMIN           20
#define SPD_TRAS_TRC_MSB     21
#define SPD_TRAS_LSB         22
#define SPD_TRC_LSB          23
#define SPD_TRFC_LSB         24
#define SPD_TRFC_MSB         25
#define SPD_WTRMIN           26
#define SPD_RTPMIN           27
#define SPD_TFAW_MSB         28
#define SPD_TFAW_LSB         29
#define SPD_OPTIONAL         30
#define SPD_THERMAL          31

#define SPD_VENDOR_ID_LSB    117
#define SPD_VENDOR_ID_MSB    118

#define SPD_NAME             128

#define MTB_PER_CYC          0xF  /* 15 * 0.125ns per 533MHz clock cycle */

#define I2C_PAD_CTRL    (PAD_CTL_PUS_100K_UP | PAD_CTL_SPEED_MED |      \
                         PAD_CTL_DSE_40ohm | PAD_CTL_HYS |              \
                         PAD_CTL_ODE | PAD_CTL_SRE_FAST)
#define PC MUX_PAD_CTRL(I2C_PAD_CTRL)

#define MPDGCTRL0_PHY0 0x021b083c
#define MPDGCTRL1_PHY0 0x021b0840
#define MPDGCTRL0_PHY1 0x021b483c
#define MPDGCTRL1_PHY1 0x021b4840
#define MPRDDLCTL_PHY0 0x021b0848
#define MPRDDLCTL_PHY1 0x021b4848
#define MPWRDLCTL_PHY0 0x021b0850
#define MPWRDLCTL_PHY1 0x021b4850

#define MDPDC_OFFSET 0x0004
#define MDCFG0_OFFSET 0x000C
#define MDCFG1_OFFSET 0x0010
#define MDCFG2_OFFSET 0x0014
#define MAPSR_OFFSET 0x0404
#define MDREF_OFFSET 0x0020
#define MDASP_OFFSET 0x0040
#define MPZQHWCTRL_OFFSET 0x0800
#define MDSCR_OFFSET 0x001C
#define MPWLGCR_OFFSET 0x0808
#define MPWLDECTRL0_OFFSET 0x080c
#define MPWLDECTRL1_OFFSET 0x0810
#define MDCTL_OFFSET 0x0000
#define MDMISC_OFFSET 0x0018
#define MPPDCMPR1_OFFSET 0x088C
#define MPSWDAR_OFFSET 0x0894
#define MPRDDLCTL_OFFSET 0x0848
#define MPMUR_OFFSET 0x08B8
#define MPDGCTRL0_OFFSET 0x083C
#define MPDGHWST0_OFFSET 0x087C
#define MPDGHWST1_OFFSET 0x0880
#define MPDGHWST2_OFFSET 0x0884
#define MPDGHWST3_OFFSET 0x0888
#define MPDGCTRL1_OFFSET 0x0840
#define MPRDDLHWCTL_OFFSET 0x0860
#define MPWRDLCTL_OFFSET 0x0850
#define MPWRDLHWCTL_OFFSET 0x0864

#define GET_BIT(n,x) ( ( (x) >> (n) ) & 1 )

#ifdef DEBUG
#undef debug
#define debug my_printf
extern void PUTC_LL(int c);
static void my_puts(char *c)
{
	while (*c) {
		if (*c == '\n')
			PUTC_LL('\r');
		PUTC_LL(*c++);
	}
}

static int my_printf(const char *fmt, ...)
{
        va_list args;
        uint i;
        char printbuffer[CONFIG_SYS_PBSIZE];

        va_start(args, fmt);

        /* For this to work, printbuffer must be larger than
         * anything we ever want to print.
         */
        i = vscnprintf(printbuffer, sizeof(printbuffer), fmt, args);
        va_end(args);

        /* Print the string */
        my_puts(printbuffer);
        return i;
}

#define printf my_printf
#endif

#define DISP_LINE_LEN 16

struct ddr_spd {
	uint	density;
	uint	banks;
	uint	rows;
	uint	cols;
	uint	rank;
	uint	devwidth;
	uint	capacity;  /* in megabits */
	uint	clockrate; /* in MHz */
	uint	caslatency;
	uint	tAAmin;
	uint	tWRmin;
	uint	tRCDmin;
	uint	tRRDmin;
	uint	tRPmin;
	uint	tRAS;
	uint	tRC;
	uint	tRFCmin;
	uint	tWTRmin;
	uint	tRTPmin;
	uint	tFAW;
	uint	vendorID;
	u_char	name[19];
};

void reg32setbit(unsigned int addr, unsigned int bit) {
	*((volatile unsigned int *)addr) |= (1 << bit);
}
void reg32clrbit(unsigned int addr, unsigned int bit) {
	*((volatile unsigned int *)addr) &= ~(1 << bit);
}


static void dram_fatal(uint8_t arg)
{
	printf("Fatal DDR error 0x%02x\r\n", arg);
	while(1);
}

static unsigned int mtb_to_cycles(unsigned int mtbs) {
	return (mtbs / MTB_PER_CYC) + (((mtbs % MTB_PER_CYC) > 0) ? 1 : 0);
}



///////////////////////////////////////////////////////
///////////////////////////////////////////////////////



#ifdef CONFIG_PERFORM_RAM_TEST
static int write_random_data(int bank)
{
	ulong	addr, writeval, count, feedback, sum1;
	int	size;
	ulong	base_address = 0x10000000;

	if (bank == 0)
		addr = base_address;
	else
		addr = base_address + 0x90000000;
	
	/* Get the value to write.  */
	writeval = 0x0;

	count = 0x20000;
	size = 2;
	sum1 = 0;

	debug("16-bit writes @ 0x%08lx - 0x%08lx / %u: ",
		addr, addr + (count * sizeof(addr)*size), sizeof(addr) * size);

	while (count-- > 0) {
		feedback = (GET_BIT(14, writeval) == GET_BIT(13, writeval));
		writeval = (writeval << 1) + feedback;
		writeval &= 0x7FFF;
		*((ushort  *)addr) = (ushort )writeval;
		sum1 += (ushort )writeval;
		addr += size;
	}
	debug("checksum: %08lx\n", sum1);
	return 0;
}

int read_random_data(int bank)
{
	ulong	addr, writeval, count, feedback, sum1, sum2;
	int	size;
	ulong	base_address = 0x10000000;

	/* Address is specified since argc > 1 */
	if (bank == 0)
		addr = base_address;
	else
		addr = base_address + 0x90000000;

	/* Get the value to write.  */
	writeval = 0x0;
	size = 2;

	sum1 = 0;
	sum2 = 0;
	count = 0x20000;
	feedback = 0;

	debug("16-bit reads @ 0x%08lx - 0x%08lx / %u: ",
		addr, addr + (count * sizeof(addr)*size), sizeof(addr) * size);

	while (count-- > 0) {
		feedback = (GET_BIT(14, writeval) == GET_BIT(13, writeval));
		writeval = (writeval << 1) + feedback;
		writeval &= 0x7FFF;
		sum1 += (ushort )writeval;
		sum2 += *((ushort  *)addr);
		addr += size;
	}
	debug("computed: %08lx, readback: %08lx\n", sum1, sum2);
	return 0;
}
#endif /* CONFNIG_PERFORM_RAM_TEST */

static int tune_wcal(int ddr_mr1, int *wldels)
{
	int temp1, temp2;
	int errorcount = 0;
	int wldel0 = 0;
	int wldel1 = 0;
	int wldel2 = 0;
	int wldel3 = 0;
	int wldel4 = 0;
	int wldel5 = 0;
	int wldel6 = 0;
	int wldel7 = 0;
	int ldectrl[4];

	if (wldels) {
		wldel0 = wldels[0] & 3;
		wldel1 = wldels[1] & 3;
		wldel2 = wldels[2] & 3;
		wldel3 = wldels[3] & 3;
		wldel4 = wldels[4] & 3;
		wldel5 = wldels[5] & 3;
		wldel6 = wldels[6] & 3;
		wldel7 = wldels[7] & 3;

		writel((readl(MMDC_P0_BASE_ADDR + MPWLDECTRL0_OFFSET)
			& 0xF9FFF9FF)
			| (wldel0 << 9) | (wldel1 << 25),
			MMDC_P0_BASE_ADDR + MPWLDECTRL0_OFFSET);

		writel((readl(MMDC_P0_BASE_ADDR + MPWLDECTRL1_OFFSET)
			& 0xF9FFF9FF)
			| (wldel2 << 9) | (wldel3 << 25),
			(MMDC_P0_BASE_ADDR + MPWLDECTRL1_OFFSET));

		writel((readl(MMDC_P0_BASE_ADDR + MPWLDECTRL0_OFFSET)
			& 0xF9FFF9FF)
			| (wldel4 << 9) | (wldel5 << 25),
			MMDC_P1_BASE_ADDR + MPWLDECTRL0_OFFSET);

		writel((readl(MMDC_P0_BASE_ADDR + MPWLDECTRL1_OFFSET)
			& 0xF9FFF9FF)
			| (wldel6 << 9) | (wldel7 << 25),
			MMDC_P1_BASE_ADDR + MPWLDECTRL1_OFFSET);

		debug("MMDC_MPWLDECTRL0 before write level cal: 0x%08X\n",
			readl(MMDC_P0_BASE_ADDR + MPWLDECTRL0_OFFSET));
		debug("MMDC_MPWLDECTRL1 before write level cal: 0x%08X\n",
			readl(MMDC_P0_BASE_ADDR + MPWLDECTRL1_OFFSET));
		debug("MMDC_MPWLDECTRL0 before write level cal: 0x%08X\n",
			readl(MMDC_P1_BASE_ADDR + MPWLDECTRL0_OFFSET));
		debug("MMDC_MPWLDECTRL1 before write level cal: 0x%08X\n",
			readl(MMDC_P1_BASE_ADDR + MPWLDECTRL1_OFFSET));
	}

	/*
	 * Stash old values in case calibration fails,
	 * we need to restore them
	 */
	ldectrl[0] = readl(MMDC_P0_BASE_ADDR + MPWLDECTRL0_OFFSET);
	ldectrl[1] = readl(MMDC_P0_BASE_ADDR + MPWLDECTRL1_OFFSET);
	ldectrl[2] = readl(MMDC_P1_BASE_ADDR + MPWLDECTRL0_OFFSET);
	ldectrl[3] = readl(MMDC_P1_BASE_ADDR + MPWLDECTRL1_OFFSET);

	/* disable DDR logic power down timer */
	writel(readl((MMDC_P0_BASE_ADDR + MDPDC_OFFSET)) & 0xffff00ff,
		MMDC_P0_BASE_ADDR + MDPDC_OFFSET);
	/* disable Adopt power down timer */
	writel(readl(MMDC_P0_BASE_ADDR + MAPSR_OFFSET) | 0x1,
		MMDC_P0_BASE_ADDR + MAPSR_OFFSET);

	debug("Start write leveling calibration\n");
	/*
	 * 2. disable auto refresh and ZQ calibration
	 * before proceeding with Write Leveling calibration
	 */
	temp1 = readl(MMDC_P0_BASE_ADDR + MDREF_OFFSET);
	writel(0x0000C000, MMDC_P0_BASE_ADDR + MDREF_OFFSET);
	temp2 = readl(MMDC_P0_BASE_ADDR + MPZQHWCTRL_OFFSET);
	writel(temp2 & ~(0x3), MMDC_P0_BASE_ADDR + MPZQHWCTRL_OFFSET);

	/* 3. increase walat and ralat to maximum */
	reg32setbit((MMDC_P0_BASE_ADDR + MDMISC_OFFSET), 6); //set RALAT to max
	reg32setbit((MMDC_P0_BASE_ADDR + MDMISC_OFFSET), 7);
	reg32setbit((MMDC_P0_BASE_ADDR + MDMISC_OFFSET), 8);
	reg32setbit((MMDC_P0_BASE_ADDR + MDMISC_OFFSET), 16); //set WALAT to max
	reg32setbit((MMDC_P0_BASE_ADDR + MDMISC_OFFSET), 17);

	reg32setbit((MMDC_P1_BASE_ADDR + MDMISC_OFFSET), 6); //set RALAT to max
	reg32setbit((MMDC_P1_BASE_ADDR + MDMISC_OFFSET), 7);
	reg32setbit((MMDC_P1_BASE_ADDR + MDMISC_OFFSET), 8);
	reg32setbit((MMDC_P1_BASE_ADDR + MDMISC_OFFSET), 16); //set WALAT to max
	reg32setbit((MMDC_P1_BASE_ADDR + MDMISC_OFFSET), 17);

	/*
	 * 4 & 5. Configure the external DDR device to enter write-leveling
	 * mode through Load Mode Register command.
	 * Register setting:
	 * Bits[31:16] MR1 value (0x0080 write leveling enable)
	 * Bit[9] set WL_EN to enable MMDC DQS output
	 * Bits[6:4] set CMD bits for Load Mode Register programming
	 * Bits[2:0] set CMD_BA to 0x1 for DDR MR1 programming
	 */
	writel(0x00808231, MMDC_P0_BASE_ADDR + MDSCR_OFFSET);

	/* 6. Activate automatic calibration by setting MPWLGCR[HW_WL_EN] */
	writel(0x00000001, MMDC_P0_BASE_ADDR + MPWLGCR_OFFSET);

	/*
	 * 7. Upon completion of this process the MMDC de-asserts
	 * the MPWLGCR[HW_WL_EN]
	 */
	while (readl(MMDC_P0_BASE_ADDR + MPWLGCR_OFFSET) & 0x00000001)
		debug(".");

	/*
	 * 8. check for any errors: check both PHYs for x64 configuration,
	 * if x32, check only PHY0
	 */
	if ((readl(MMDC_P0_BASE_ADDR + MPWLGCR_OFFSET) & 0x00000F00) ||
	    (readl(MMDC_P1_BASE_ADDR + MPWLGCR_OFFSET) & 0x00000F00))
		errorcount++;
	debug("Write leveling calibration completed, errcount: %d\n",
			errorcount);

	/* check to see if cal failed */
	if ((readl(MMDC_P0_BASE_ADDR + MPWLDECTRL0_OFFSET)) == 0x001F001F
	&& (readl(MMDC_P0_BASE_ADDR + MPWLDECTRL1_OFFSET)) == 0x001F001F
	&& (readl(MMDC_P1_BASE_ADDR + MPWLDECTRL0_OFFSET)) == 0x001F001F
	&& (readl(MMDC_P1_BASE_ADDR + MPWLDECTRL1_OFFSET)) == 0x001F001F) {
		debug("Cal seems to have soft-failed due to memory "
			"not supporting write leveling on all channels. "
			"Restoring original write leveling values.\n");
		writel(ldectrl[0], MMDC_P0_BASE_ADDR + MPWLDECTRL0_OFFSET);
		writel(ldectrl[1], MMDC_P0_BASE_ADDR + MPWLDECTRL1_OFFSET);
		writel(ldectrl[2], MMDC_P1_BASE_ADDR + MPWLDECTRL0_OFFSET);
		writel(ldectrl[3], MMDC_P1_BASE_ADDR + MPWLDECTRL1_OFFSET);
		errorcount++;
	}

	/*
	 * User should issue MRS command to exit write leveling mode
	 * through Load Mode Register command
	 * Register setting:
	 * Bits[31:16] MR1 value "ddr_mr1" value from initialization
	 * Bit[9] clear WL_EN to disable MMDC DQS output
	 * Bits[6:4] set CMD bits for Load Mode Register programming
	 * Bits[2:0] set CMD_BA to 0x1 for DDR MR1 programming
	 */
	writel( (ddr_mr1 << 16) + 0x8031, MMDC_P0_BASE_ADDR + MDSCR_OFFSET);
	/* re-enable to auto refresh and zq cal */
	writel(temp1, MMDC_P0_BASE_ADDR + MDREF_OFFSET);
	writel(temp2, MMDC_P0_BASE_ADDR + MPZQHWCTRL_OFFSET);
#ifdef DEBUG
	debug("MMDC_MPWLDECTRL0 after write level cal: 0x%08X\n",
		readl(MMDC_P0_BASE_ADDR + MPWLDECTRL0_OFFSET));
	debug("MMDC_MPWLDECTRL1 after write level cal: 0x%08X\n",
		readl(MMDC_P0_BASE_ADDR + MPWLDECTRL1_OFFSET));
	debug("MMDC_MPWLDECTRL0 after write level cal: 0x%08X\n",
		readl(MMDC_P1_BASE_ADDR + MPWLDECTRL0_OFFSET));
	debug("MMDC_MPWLDECTRL1 after write level cal: 0x%08X\n",
		readl(MMDC_P1_BASE_ADDR + MPWLDECTRL1_OFFSET));
#else
	/* We must force a readback of these values, to get them to stick */
	readl(MMDC_P0_BASE_ADDR + MPWLDECTRL0_OFFSET);
	readl(MMDC_P0_BASE_ADDR + MPWLDECTRL1_OFFSET);
	readl(MMDC_P1_BASE_ADDR + MPWLDECTRL0_OFFSET);
	readl(MMDC_P1_BASE_ADDR + MPWLDECTRL1_OFFSET);
#endif
	/* enable DDR logic power down timer: */
	writel(readl((MMDC_P0_BASE_ADDR + MDPDC_OFFSET)) | 0x00005500,
		MMDC_P0_BASE_ADDR + MDPDC_OFFSET);
	/* enable Adopt power down timer: */
	writel(readl((MMDC_P0_BASE_ADDR + MAPSR_OFFSET)) & 0xfffffff7,
		MMDC_P0_BASE_ADDR + MAPSR_OFFSET);
	writel(0, MMDC_P0_BASE_ADDR + MDSCR_OFFSET); //clear CON_REQ
	return errorcount;
}


static int modify_dg_result(int reg_st0, int reg_st1, int reg_ctrl)
{
	/*
	 * DQS gating absolute offset should be modified from reflecting
	 * (HW_DG_LOWx + HW_DG_UPx)/2 to reflecting (HW_DG_UPx - 0x80)
	 */
	int dg_tmp_val0,dg_tmp_val1, dg_tmp_val2;
	int dg_dl_abs_offset0, dg_dl_abs_offset1;
	int dg_hc_del0, dg_hc_del1;
	dg_tmp_val0 = ((readl(reg_st0) & 0x07ff0000) >>16) - 0xc0;
	dg_tmp_val1 = ((readl(reg_st1) & 0x07ff0000) >>16) - 0xc0;
	dg_dl_abs_offset0 = dg_tmp_val0 & 0x7f;
	dg_hc_del0 = (dg_tmp_val0 & 0x780) << 1;
	dg_dl_abs_offset1 = dg_tmp_val1 & 0x7f;
	dg_hc_del1 = (dg_tmp_val1 & 0x780) << 1;
	dg_tmp_val2 = dg_dl_abs_offset0 + dg_hc_del0 + ((dg_dl_abs_offset1 +
						   dg_hc_del1) << 16);
	writel(readl(reg_ctrl) & 0xf0000000, reg_ctrl);
	writel(readl(reg_ctrl) & 0xf0000000, reg_ctrl);
	writel(readl(reg_ctrl) | dg_tmp_val2, reg_ctrl);
	return 0;
}

static int tune_delays(uint32_t *dram_sdqs_pad_ctls)
{
	int temp1;
	int data_bus_size;
	int temp_ref;
	int cs0_enable = 0;
	int cs1_enable = 0;
	int cs0_enable_initial = 0;
	int cs1_enable_initial = 0;

	int PDDWord = 0x00FFFF00; /* best so far, place into MPPDCMPR1 */
	int errorcount = 0;
	unsigned int initdelay = 0x40404040;

	/* check to see which chip selects are enabled */
	cs0_enable_initial = (readl(MMDC_P0_BASE_ADDR + MDCTL_OFFSET)
			& 0x80000000) >> 31;
	cs1_enable_initial = (readl(MMDC_P0_BASE_ADDR + MDCTL_OFFSET)
			& 0x40000000) >> 30;

	/* disable DDR logic power down timer: */
	writel(readl(MMDC_P0_BASE_ADDR + MDPDC_OFFSET) & 0xffff00ff,
		MMDC_P0_BASE_ADDR + MDPDC_OFFSET);

	/* disable Adopt power down timer: */
	writel(readl(MMDC_P0_BASE_ADDR + MAPSR_OFFSET) | 0x1,
		MMDC_P0_BASE_ADDR + MAPSR_OFFSET);

	/* set DQS pull ups */
	writel(readl(dram_sdqs_pad_ctls[0]) | 0x7000, dram_sdqs_pad_ctls[0]);
	writel(readl(dram_sdqs_pad_ctls[1]) | 0x7000, dram_sdqs_pad_ctls[1]);
	writel(readl(dram_sdqs_pad_ctls[2]) | 0x7000, dram_sdqs_pad_ctls[2]);
	writel(readl(dram_sdqs_pad_ctls[3]) | 0x7000, dram_sdqs_pad_ctls[3]);
	writel(readl(dram_sdqs_pad_ctls[4]) | 0x7000, dram_sdqs_pad_ctls[4]);
	writel(readl(dram_sdqs_pad_ctls[5]) | 0x7000, dram_sdqs_pad_ctls[5]);
	writel(readl(dram_sdqs_pad_ctls[6]) | 0x7000, dram_sdqs_pad_ctls[6]);
	writel(readl(dram_sdqs_pad_ctls[7]) | 0x7000, dram_sdqs_pad_ctls[7]);

	/* set RALAT to max */
	temp1 = readl(MMDC_P0_BASE_ADDR + MDMISC_OFFSET);
		reg32setbit((MMDC_P0_BASE_ADDR + MDMISC_OFFSET), 6);
	reg32setbit((MMDC_P0_BASE_ADDR + MDMISC_OFFSET), 7);
	reg32setbit((MMDC_P0_BASE_ADDR + MDMISC_OFFSET), 8);

	/* set WALAT to max */
	reg32setbit((MMDC_P0_BASE_ADDR + MDMISC_OFFSET), 16);
	reg32setbit((MMDC_P0_BASE_ADDR + MDMISC_OFFSET), 17);

	/* disable auto refresh before proceeding with calibration */
	temp_ref = readl(MMDC_P0_BASE_ADDR + MDREF_OFFSET);
	writel(0x0000C000, MMDC_P0_BASE_ADDR + MDREF_OFFSET);

	/*
	 * Per the ref manual, issue one refresh cycle MDSCR[CMD]= 0x2,
	 * this also sets the CON_REQ bit.
	 */
	if (cs0_enable_initial == 1)
		writel(0x00008020, MMDC_P0_BASE_ADDR + MDSCR_OFFSET);
	if (cs1_enable_initial == 1)
		writel(0x00008028, MMDC_P0_BASE_ADDR + MDSCR_OFFSET);

	/* poll to make sure the con_ack bit was asserted */
	while (!(readl((MMDC_P0_BASE_ADDR + MDSCR_OFFSET)) & 0x00004000))
		debug(".");

	/*
	 * Check MDMISC register CALIB_PER_CS to see which CS calibration
	 * is targeted to (under normal cases, it should be cleared
	 * as this is the default value, indicating calibration is directed
	 * to CS0).
	 * Disable the other chip select not being target for calibration
	 * to avoid any potential issues.  This will get re-enabled at end
	 * of calibration.
	 */
	if ((readl(MMDC_P0_BASE_ADDR + MDMISC_OFFSET) & 0x00100000) == 0)
		/* clear SDE_1 */
		reg32clrbit((MMDC_P0_BASE_ADDR + MDCTL_OFFSET), 30);
	else
		/* clear SDE_0 */
		reg32clrbit((MMDC_P0_BASE_ADDR + MDCTL_OFFSET), 31);

	/*
	 * Check to see which chip selects are now enabled for
	 * the remainder of the calibration.
	 */
	cs0_enable = (readl(MMDC_P0_BASE_ADDR + MDCTL_OFFSET)
			& 0x80000000) >> 31;
	cs1_enable = (readl(MMDC_P0_BASE_ADDR + MDCTL_OFFSET)
			& 0x40000000) >> 30;

	/* check to see what is the data bus size: */
	data_bus_size = (readl(MMDC_P0_BASE_ADDR + MDCTL_OFFSET)
			& 0x30000) >> 16;
	debug("db size: %d\n", data_bus_size);

	/*
	 * Issue the Precharge-All command to the DDR device for both
	 * chip selects.  Note, CON_REQ bit should also remain set.
	 * If only using one chip select, then precharge only the desired
	 * chip select.
	 */
	if (cs0_enable == 1)
		/* CS0 */
		writel(0x04008050, MMDC_P0_BASE_ADDR + MDSCR_OFFSET);
	if (cs1_enable == 1)
		/* CS1 */
		writel(0x04008058, MMDC_P0_BASE_ADDR + MDSCR_OFFSET);

	/* Write the pre-defined value into MPPDCMPR1 */
	writel(PDDWord, MMDC_P0_BASE_ADDR + MPPDCMPR1_OFFSET);

	/*
	 * Issue a write access to the external DDR device by setting
	 * the bit SW_DUMMY_WR (bit 0) in the MPSWDAR0 and then poll
	 * this bit until it clears to indicate completion of the write access.
	 */
	reg32setbit((MMDC_P0_BASE_ADDR + MPSWDAR_OFFSET), 0);
	while (readl((MMDC_P0_BASE_ADDR + MPSWDAR_OFFSET)) & 0x00000001)
		debug(".");

	/* Set the RD_DL_ABS_OFFSET# bits to their default values
	 * (will be calibrated later in the read delay-line calibration).
	 * Both PHYs for x64 configuration, if x32, do only PHY0.
	 */
	writel(0x40404040, MMDC_P0_BASE_ADDR + MPRDDLCTL_OFFSET);
	if (data_bus_size == 0x2)
		writel(0x40404040, MMDC_P1_BASE_ADDR + MPRDDLCTL_OFFSET);

	/* Force a measurment, for previous delay setup to take effect. */
	writel(0x800, MMDC_P0_BASE_ADDR + MPMUR_OFFSET);
	if (data_bus_size == 0x2)
		writel(0x800, MMDC_P1_BASE_ADDR + MPMUR_OFFSET);

	/*
	 * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	 * Read DQS Gating calibration
	 * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	 */
	debug("Starting DQS gating calibration...\n");

	/*
	 * Reset the read data FIFOs (two resets); only need to issue reset
	 * to PHY0 since in x64 mode, the reset will also go to PHY1.
	 */

	 /* Read data FIFOs reset1.  */
	writel(readl((MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET)) | 0x80000000,
		MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET);
	while (readl((MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET)) & 0x80000000)
		debug(".");

	/* Read data FIFOs reset2 */
	writel(readl((MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET)) | 0x80000000,
		MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET);
	while (readl((MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET)) & 0x80000000)
		debug(".");

	/*
	 * Start the automatic read DQS gating calibration process by
	 * asserting MPDGCTRL0[HW_DG_EN] and MPDGCTRL0[DG_CMP_CYC]
	 * and then poll MPDGCTRL0[HW_DG_EN]] until this bit clears
	 * to indicate completion.
	 * Also, ensure that MPDGCTRL0[HW_DG_ERR] is clear to indicate
	 * no errors were seen during calibration.
	 */

	/*
	 * Set bit 30: chooses option to wait 32 cycles instead of
	 * 16 before comparing read data.
	 */
	reg32setbit((MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET), 30);

	/* Set bit 28 to start automatic read DQS gating calibration */
	reg32setbit((MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET), 28);

	/* Poll for completion.  MPDGCTRL0[HW_DG_EN] should be 0 */
	while (readl((MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET))
			& 0x10000000)
		debug( "." );

	/*
	 * Check to see if any errors were encountered during calibration
	 * (check MPDGCTRL0[HW_DG_ERR]).
	 * Check both PHYs for x64 configuration, if x32, check only PHY0.
	 */
	if (data_bus_size == 0x2) {
		if ((readl(MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET)
					& 0x00001000) ||
			(readl(MMDC_P1_BASE_ADDR + MPDGCTRL0_OFFSET)
					& 0x00001000)) {
			errorcount++;
		}
	} else {
		if (readl(MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET) & 0x00001000)
			errorcount++;
	}
	debug("errorcount: %d\n", errorcount);

	/*
	 * DQS gating absolute offset should be modified from
	 * reflecting (HW_DG_LOWx + HW_DG_UPx)/2 to
	 * reflecting (HW_DG_UPx - 0x80)
	 */
	modify_dg_result(MMDC_P0_BASE_ADDR + MPDGHWST0_OFFSET,
		MMDC_P0_BASE_ADDR + MPDGHWST1_OFFSET,
		MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET);
	modify_dg_result(MMDC_P0_BASE_ADDR + MPDGHWST2_OFFSET,
		MMDC_P0_BASE_ADDR + MPDGHWST3_OFFSET,
		MMDC_P0_BASE_ADDR + MPDGCTRL1_OFFSET);
	if (data_bus_size == 0x2) {
		modify_dg_result((MMDC_P1_BASE_ADDR + MPDGHWST0_OFFSET),
			(MMDC_P1_BASE_ADDR + MPDGHWST1_OFFSET),
			(MMDC_P1_BASE_ADDR + MPDGCTRL0_OFFSET));
		modify_dg_result((MMDC_P1_BASE_ADDR + MPDGHWST2_OFFSET),
			(MMDC_P1_BASE_ADDR + MPDGHWST3_OFFSET),
			(MMDC_P1_BASE_ADDR + MPDGCTRL1_OFFSET));
	}
	debug("DQS gating calibration completed, continuing...        \n");



	/*
	 * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	 * Read delay Calibration
	 * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	 */
	debug("Starting read calibration...\n");

	/*
	 * Reset the read data FIFOs (two resets); only need to issue reset
	 * to PHY0 since in x64 mode, the reset will also go to PHY1.
	 */

	/* Read data FIFOs reset1 */
	writel(readl(MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET) | 0x80000000,
		MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET);
	while (readl((MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET)) & 0x80000000)
		debug(".");
	
	/* Read data FIFOs reset2 */
	writel(readl(MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET) | 0x80000000,
		MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET);
	while (readl((MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET)) & 0x80000000)
		debug(".");
	
	/*
	 * 4. Issue the Precharge-All command to the DDR device for both
	 * chip selects.  If only using one chip select, then precharge
	 * only the desired chip select.
	 */
	if (cs0_enable == 1)
		/* CS0 */
		writel(0x04008050, MMDC_P0_BASE_ADDR + MDSCR_OFFSET);

	while (!(readl(MMDC_P0_BASE_ADDR + MDSCR_OFFSET) & 0x4000))
		debug( "x" );

	if (cs1_enable == 1)
		/* CS1 */
		writel(0x04008058, MMDC_P0_BASE_ADDR + MDSCR_OFFSET);

	while (!(readl(MMDC_P0_BASE_ADDR + MDSCR_OFFSET) & 0x4000))
		debug("x");

	/* *********** 5. 6. 7. set the pre-defined word ************ */
	writel(PDDWord, MMDC_P0_BASE_ADDR + MPPDCMPR1_OFFSET);

	/*
	 * Issue a write access to the external DDR device by setting
	 * the bit SW_DUMMY_WR (bit 0) in the MPSWDAR0 and then poll
	 * this bit until it clears to indicate completion of the write access.
	 */
	reg32setbit((MMDC_P0_BASE_ADDR + MPSWDAR_OFFSET), 0);
	while (readl((MMDC_P0_BASE_ADDR + MPSWDAR_OFFSET)) & 0x00000001)
		debug( "." );

	/* 8. set initial delays to center up dq in clock */
	writel(initdelay, MMDC_P0_BASE_ADDR + MPRDDLCTL_OFFSET);
	if (data_bus_size == 0x2)
		writel(initdelay, MMDC_P1_BASE_ADDR + MPRDDLCTL_OFFSET);
	debug("intdel0: %08x / intdel1: %08x\n", 
		readl(MMDC_P0_BASE_ADDR + MPRDDLCTL_OFFSET),
		readl(MMDC_P1_BASE_ADDR + MPRDDLCTL_OFFSET));

	/*
	 * 9. Read delay-line calibration
	 * Start the automatic read calibration process by asserting
	 * MPRDDLHWCTL[HW_RD_DL_EN].
	 */
	writel(0x00000030, MMDC_P0_BASE_ADDR + MPRDDLHWCTL_OFFSET);

	/*
	 * 10. poll for completion
	 * MMDC indicates that the write data calibration had finished by
	 * setting MPRDDLHWCTL[HW_RD_DL_EN] = 0.   Also, ensure that
	 * no error bits were set.
	 */
	while (readl((MMDC_P0_BASE_ADDR + MPRDDLHWCTL_OFFSET)) & 0x00000010)
		debug( "." );

	/* check both PHYs for x64 configuration, if x32, check only PHY0 */
	if (data_bus_size == 0x2) {
		if ((readl(MMDC_P0_BASE_ADDR + MPRDDLHWCTL_OFFSET)
					& 0x0000000f) ||
			(readl(MMDC_P1_BASE_ADDR + MPRDDLHWCTL_OFFSET)
					& 0x0000000f)) {
			errorcount++;
		}
	} else {
		if (readl(MMDC_P0_BASE_ADDR + MPRDDLHWCTL_OFFSET) & 0x0000000f)
			errorcount++;
	}
	debug("errorcount: %d\n", errorcount);
	debug("Read calibration completed, continuing...\n");


	/*
	 * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	 * Write delay Calibration
	 * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	 */
	debug("Starting write calibration...\n");

	/*
	 * 3. Reset the read data FIFOs (two resets); only need to issue
	 * reset to PHY0 since in x64 mode, the reset will also go to PHY1.
	 */

	/* read data FIFOs reset1 */
	writel(readl((MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET)) | 0x80000000,
		MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET);
	while (readl((MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET)) & 0x80000000)
		debug(".");

	/* read data FIFOs reset2 */
	writel(readl((MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET)) | 0x80000000,
		MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET);
	while (readl((MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET)) & 0x80000000)
		debug(".");

	/*
	 * 4. Issue the Precharge-All command to the DDR device for both
	 * chip selects. If only using one chip select, then precharge
	 * only the desired chip select.
	 */
	if (cs0_enable == 1)
		/* CS0 */
		writel(0x04008050, MMDC_P0_BASE_ADDR + MDSCR_OFFSET);

	if (cs1_enable == 1)
		/* CS1 */
		writel(0x04008058, MMDC_P0_BASE_ADDR + MDSCR_OFFSET);

	/* *********** 5. 6. 7. set the pre-defined word ************ */
	writel(PDDWord, MMDC_P0_BASE_ADDR + MPPDCMPR1_OFFSET);

	/*
	 * Issue a write access to the external DDR device by setting
	 * the bit SW_DUMMY_WR (bit 0) in the MPSWDAR0 and then poll this bit
	 * until it clears to indicate completion of the write access.
	 */
	reg32setbit((MMDC_P0_BASE_ADDR + MPSWDAR_OFFSET), 0);
	while (readl((MMDC_P0_BASE_ADDR + MPSWDAR_OFFSET)) & 0x00000001)
		debug(".");

	/*
	 * 8. Set the WR_DL_ABS_OFFSET# bits to their default values.
	 * Both PHYs for x64 configuration, if x32, do only PHY0.
	 */
	writel(initdelay, MMDC_P0_BASE_ADDR + MPWRDLCTL_OFFSET);
	if (data_bus_size == 0x2)
		writel(initdelay, MMDC_P1_BASE_ADDR + MPWRDLCTL_OFFSET);
	debug("intdel0: %08x / intdel1: %08x\n", 
		readl(MMDC_P0_BASE_ADDR + MPWRDLCTL_OFFSET),
		readl(MMDC_P1_BASE_ADDR + MPWRDLCTL_OFFSET));

	/*
	 * XXX This isn't in the manual. Force a measurment,
	 * for previous delay setup to effect.
	 */
	writel(0x800, MMDC_P0_BASE_ADDR + MPMUR_OFFSET);
	if (data_bus_size == 0x2)
		writel(0x800, MMDC_P1_BASE_ADDR + MPMUR_OFFSET);

	/*
	 * 9. 10. Start the automatic write calibration process
	 * by asserting MPWRDLHWCTL0[HW_WR_DL_EN].
	 */
	writel(0x00000030, MMDC_P0_BASE_ADDR + MPWRDLHWCTL_OFFSET);

	/*
	 * Poll for completion.
	 * MMDC indicates that the write data calibration had finished
	 * by setting MPWRDLHWCTL[HW_WR_DL_EN] = 0.
	 * Also, ensure that no error bits were set.
	 */
	while (readl((MMDC_P0_BASE_ADDR + MPWRDLHWCTL_OFFSET))
			& 0x00000010)
		debug(".");

	/* Check both PHYs for x64 configuration, if x32, check only PHY0 */
	if (data_bus_size == 0x2) {
		if ((readl(MMDC_P0_BASE_ADDR + MPWRDLHWCTL_OFFSET)
					& 0x0000000f) ||
			(readl(MMDC_P1_BASE_ADDR + MPWRDLHWCTL_OFFSET)
					& 0x0000000f)) {
			errorcount++;
		}
	} else {
		if (readl(MMDC_P0_BASE_ADDR + MPWRDLHWCTL_OFFSET) & 0x0000000f)
			errorcount++;
	}
	debug("errorcount: %d\n", errorcount);
	debug("Write calibration completed, continuing...        \n");

	/*
	 * Reset the read data FIFOs (two resets); only need to issue
	 * reset to PHY0 since in x64 mode, the reset will also go to PHY1.
	 */

	/* read data FIFOs reset1 */
	writel(readl((MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET)) | 0x80000000,
		MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET);
	while (readl((MMDC_P0_BASE_ADDR + MPDGCTRL0_OFFSET)) & 0x80000000)
		debug(".");

	/* read data FIFOs reset2 */
	writel(readl((MMDC_P1_BASE_ADDR + MPDGCTRL0_OFFSET)) | 0x80000000,
		MMDC_P1_BASE_ADDR + MPDGCTRL0_OFFSET);
	while (readl((MMDC_P1_BASE_ADDR + MPDGCTRL0_OFFSET)) & 0x80000000)
		debug(".");

	/* Enable DDR logic power down timer: */
	writel(readl((MMDC_P0_BASE_ADDR + MDPDC_OFFSET)) | 0x00005500,
		MMDC_P0_BASE_ADDR + MDPDC_OFFSET);

	/* Enable Adopt power down timer: */
	writel(readl((MMDC_P0_BASE_ADDR + MAPSR_OFFSET)) & 0xfffffff7,
		MMDC_P0_BASE_ADDR + MAPSR_OFFSET);

	/* Restore MDMISC value (RALAT, WALAT) */
	writel(temp1, MMDC_P1_BASE_ADDR + MDMISC_OFFSET);

	/* Clear DQS pull ups */
	writel(readl(dram_sdqs_pad_ctls[0]) & 0xffff0fff,
		dram_sdqs_pad_ctls[0]);
	writel(readl(dram_sdqs_pad_ctls[1]) & 0xffff0fff,
		dram_sdqs_pad_ctls[1]);
	writel(readl(dram_sdqs_pad_ctls[2]) & 0xffff0fff,
		dram_sdqs_pad_ctls[2]);
	writel(readl(dram_sdqs_pad_ctls[3]) & 0xffff0fff,
		dram_sdqs_pad_ctls[3]);
	writel(readl(dram_sdqs_pad_ctls[4]) & 0xffff0fff,
		dram_sdqs_pad_ctls[4]);
	writel(readl(dram_sdqs_pad_ctls[5]) & 0xffff0fff,
		dram_sdqs_pad_ctls[5]);
	writel(readl(dram_sdqs_pad_ctls[6]) & 0xffff0fff,
		dram_sdqs_pad_ctls[6]);
	writel(readl(dram_sdqs_pad_ctls[7]) & 0xffff0fff,
		dram_sdqs_pad_ctls[7]);

	/* Re-enable SDE (chip selects) if they were set initially */
	if (cs1_enable_initial == 1)
		/* Set SDE_1 */
		reg32setbit((MMDC_P0_BASE_ADDR + MDCTL_OFFSET), 30);

	if (cs0_enable_initial == 1)
		/* Set SDE_0 */
		reg32setbit((MMDC_P0_BASE_ADDR + MDCTL_OFFSET), 31);

	/* Re-enable to auto refresh */
	writel(temp_ref, MMDC_P0_BASE_ADDR + MDREF_OFFSET);

	/* Clear the MDSCR (including the con_req bit) */
	writel(0x0, MMDC_P0_BASE_ADDR + MDSCR_OFFSET); // CS0

	/* Poll to make sure the con_ack bit is clear */
	while ((readl(MMDC_P0_BASE_ADDR + MDSCR_OFFSET) & 0x00004000))
		debug(".");

	/*
	 * Print out the registers that were updated as a result
	 * of the calibration process.
	 */
	debug("MMDC registers updated from calibration \n");
	debug("\nRead DQS Gating calibration\n");
	debug("MPDGCTRL0 PHY0 (0x021b083c) = 0x%08X\n", readl(MPDGCTRL0_PHY0));
	debug("MPDGCTRL1 PHY0 (0x021b0840) = 0x%08X\n", readl(MPDGCTRL1_PHY0));
	debug("MPDGCTRL0 PHY1 (0x021b483c) = 0x%08X\n", readl(MPDGCTRL0_PHY1));
	debug("MPDGCTRL1 PHY1 (0x021b4840) = 0x%08X\n", readl(MPDGCTRL1_PHY1));
	debug("\nRead calibration\n");
	debug("MPRDDLCTL PHY0 (0x021b0848) = 0x%08X\n", readl(MPRDDLCTL_PHY0));
	debug("MPRDDLCTL PHY1 (0x021b4848) = 0x%08X\n", readl(MPRDDLCTL_PHY1));
	debug("\nWrite calibration\n");
	debug("MPWRDLCTL PHY0 (0x021b0850) = 0x%08X\n", readl(MPWRDLCTL_PHY0));
	debug("MPWRDLCTL PHY1 (0x021b4850) = 0x%08X\n", readl(MPWRDLCTL_PHY1));
	debug("\n");
	/*
	 * Registers below are for debugging purposes.  These print out
	 * the upper and lower boundaries captured during
	 * read DQS gating calibration.
	 */
	debug("Status registers, upper and lower bounds, "
			"for read DQS gating.\n");
	debug("MPDGHWST0 PHY0 (0x021b087c) = 0x%08X\n", readl(0x021b087c));
	debug("MPDGHWST1 PHY0 (0x021b0880) = 0x%08X\n", readl(0x021b0880));
	debug("MPDGHWST2 PHY0 (0x021b0884) = 0x%08X\n", readl(0x021b0884));
	debug("MPDGHWST3 PHY0 (0x021b0888) = 0x%08X\n", readl(0x021b0888));
	debug("MPDGHWST0 PHY1 (0x021b487c) = 0x%08X\n", readl(0x021b487c));
	debug("MPDGHWST1 PHY1 (0x021b4880) = 0x%08X\n", readl(0x021b4880));
	debug("MPDGHWST2 PHY1 (0x021b4884) = 0x%08X\n", readl(0x021b4884));
	debug("MPDGHWST3 PHY1 (0x021b4888) = 0x%08X\n", readl(0x021b4888));

	debug("errorcount: %d\n", errorcount);

	return 0;
}


/* init ddr3, do calibrations */
static uint32_t imx6qdl_ddr_init(struct i2c_pads_info *i2c_pad_info,
				uint32_t *dram_sdqs_pad_ctls)
{
	uint8_t spd[256];
	uint8_t	chip;
	uint	addr, alen, length;
	int	j, nbytes, linebytes;
	struct ddr_spd ddrSPD;
	unsigned short cl_support = 0;
	unsigned int cfgval = 0;
	int errorcount = 0;
	int  i;
	uint32_t ram_size;

	debug("\nSPD dump:\n");

	chip   = 0x50;	/* Address of the SPD I2C device */
	addr   = 0x0;	/* Start of SPD address */
	alen   = 0x1;	/* Address is one-byte long */
	length = 0x100; /* length to display */

	setup_i2c(0, CONFIG_SYS_I2C_SPEED, 0, i2c_pad_info);
	i2c_init(CONFIG_SYS_I2C_SPEED, 0);

	nbytes = length;
	i = 0;
	do {
		unsigned char	linebuf[DISP_LINE_LEN];
		unsigned char	*cp;
		int ret;

		linebytes = (nbytes > DISP_LINE_LEN) ? DISP_LINE_LEN : nbytes;

		ret = i2c_read(chip, addr, alen, linebuf, linebytes);
		if (ret != 0) {
			printf("Error reading SPD on DDR3.\n");
			dram_fatal(-ret);
		}
#ifdef DEBUG
		else {
			debug("%04x:", addr);
			cp = linebuf;
			for (j=0; j<linebytes; j++) {
				spd[i++] = *cp;
				debug(" %02x", *cp++);
				addr++;
			}
			debug("    ");
			cp = linebuf;
			for (j=0; j<linebytes; j++) {
				if ((*cp < 0x20) || (*cp > 0x7e))
					debug(".");
				else
					debug("%c", *cp);
				cp++;
			}
			debug("\n");
		}
#else
		cp = linebuf;
		for (j=0; j<linebytes; j++) {
			spd[i++] = *cp++;
			addr++;
		}
#endif
		nbytes -= linebytes;
	} while (nbytes > 0);

	debug("\nRaw DDR3 characteristics based on SPD:\n");
	if ((spd[SPD_DRAM_DEVICE_TYPE] != 0xB) || (spd[SPD_MODULE_TYPE] != 3)) {
		printf("Unrecognized DIMM type installed: %d / %d\n",
				spd[SPD_DRAM_DEVICE_TYPE],
				spd[SPD_MODULE_TYPE]);
		dram_fatal(0x01);
	}

	if ((spd[SPD_SDRAM_DENSITY_AND_BANKS] & 0x30) != 0)
		debug("  Warning: memory has an unsupported bank size\n");
	else
		debug("  8 banks\n");
	ddrSPD.banks = 8;

	ddrSPD.density = 256 * (1 << (spd[SPD_SDRAM_DENSITY_AND_BANKS] & 0xF));
	debug("  Individual chip density is %d Mib\n", ddrSPD.density);

	ddrSPD.rows = ((spd[SPD_SDRAM_ADDRESSING] & 0x38) >> 3) + 12;
	ddrSPD.cols = (spd[SPD_SDRAM_ADDRESSING] & 0x7) + 9;
	debug("  Rows: %d, Cols: %d\n", ddrSPD.rows, ddrSPD.cols);

	if (spd[SPD_NOMINAL_VOLTAGE] & 0x1) {
		printf("Module not operable at 1.5V, fatal error.\n");
		dram_fatal(0x02);
	} else
		debug("  Supports 1.5V operation.\n");

	ddrSPD.rank = ((spd[SPD_ORGANIZATION] >> 3) & 0x7) + 1;
	debug("  Module has %d rank(s)\n", ddrSPD.rank);

	ddrSPD.devwidth = (1 << (spd[SPD_ORGANIZATION] & 0x7)) * 4;
	debug("  Chips have a width of %d bits\n", ddrSPD.devwidth);

	if (spd[SPD_BUS_WIDTH] != 0x3) {
		printf("Unsupported device width, fatal.\n");
		dram_fatal(0x03);
	} else
		debug("  Module width is 64 bits, no ECC\n");

	ddrSPD.capacity = (64 / ddrSPD.devwidth) * ddrSPD.rank * ddrSPD.density;
	debug("  Module capacity is %d GiB\n", ddrSPD.capacity / 8192);

	if ((spd[SPD_MTB_DIVIDEND] != 1) || (spd[SPD_MTB_DIVISOR] != 8)) {
		printf( "Module has non-standard MTB for timing calculation. "
			"Doesn't mean the module is bad, just means this "
			"bootloader can't derive timing information based "
			"on the units coded in the SPD. This is, "
			"unfortunately, a fatal error. File a bug to "
			"get it fixed.\n");
		dram_fatal(0x04);
	}

	switch (spd[SPD_TCKMIN]) {
	case 0x14:
		ddrSPD.clockrate = 400;
		break;
	case 0x0F:
		ddrSPD.clockrate = 533;
		break;
	case 0x0C:
		ddrSPD.clockrate = 667;
		break;
	case 0x0A:
		ddrSPD.clockrate = 800;
		break;
	case 0x09:
		ddrSPD.clockrate = 933;
		break;
	case 0x08:
		ddrSPD.clockrate = 1067;
		break;
	default:
		if (spd[SPD_TCKMIN] <= 0xF) {
			debug("**undecodable but sufficiently fast "
					"clock rate detected\n");
			ddrSPD.clockrate = 533;
		} else {
			debug("**undecodable but too slow "
					"clock rate detected\n");
			ddrSPD.clockrate = 400;
		}
		break;
	}
	debug("  DDR3-%d speed rating detected\n", ddrSPD.clockrate * 2);
	if (ddrSPD.clockrate < 533) {
		printf("memory is too slow.\n");
		dram_fatal(0x05);
	}

	/*
	 * cl_support is a bit vector with bit 0 set <-> CL=4,
	 * bit 1 set <-> CL=5, etc.  These are just supported rates,
	 * not the actual rate computed.
	 */
	cl_support = (spd[SPD_CL_MSB] << 8) | spd[SPD_CL_LSB];

	ddrSPD.caslatency = mtb_to_cycles((unsigned int) spd[SPD_TAAMIN]);

	while ( !((1 << (ddrSPD.caslatency - 4)) & cl_support) ) {
		if (ddrSPD.caslatency > 18) {
			printf("no module-supported CAS latencies found\n");
			dram_fatal(0x06);
		}
		ddrSPD.caslatency++;
	}

	if (ddrSPD.caslatency > 11) {
		printf("cas latency larger than supported by i.MX6\n");
		dram_fatal(0x07);
	}
	if (ddrSPD.caslatency < 3) {
		printf("cas latency shorter than supported by i.MX6\n");
		dram_fatal(0x08);
	}
	debug("Derived optimal timing parameters, in 533MHz cycles:\n");
	debug("  CAS latency: %d\n", ddrSPD.caslatency);

	ddrSPD.tWRmin = mtb_to_cycles((unsigned int) spd[SPD_TWRMIN]);
	if (ddrSPD.tWRmin > 8) {
		debug( "  optimal tWRmin greater than supported by i.MX6, "
				"value saturated.\n");
		ddrSPD.tWRmin = 8;
	}
	debug( "  tWRmin: %d\n", ddrSPD.tWRmin );

	ddrSPD.tRCDmin = mtb_to_cycles((unsigned int) spd[SPD_TRCDMIN]);
	if (ddrSPD.tRCDmin > 8) {
		debug("  optimal tRCDmin greater than supported by i.MX6, "
				"value saturated.\n");
		ddrSPD.tRCDmin = 8;
	}
	debug("  tRCDmin: %d\n", ddrSPD.tRCDmin);

	ddrSPD.tRRDmin = mtb_to_cycles((unsigned int) spd[SPD_TRRDMIN]);
	if (ddrSPD.tRRDmin > 0x8) {
		debug("  optimal tRRDmin greater than supported by i.MX6, "
				"value saturated.\n" );
		ddrSPD.tRRDmin = 0x8;
	}
	debug("  tRRDmin: %d\n", ddrSPD.tRRDmin);

	ddrSPD.tRPmin = mtb_to_cycles((unsigned int) spd[SPD_TRPMIN]);
	debug("  tRPmin: %d\n", ddrSPD.tRPmin);
	if (ddrSPD.tRPmin > 8) {
		debug("  optimal tRPmin greater than supported by i.MX6, "
				"value saturated.\n");
		ddrSPD.tRPmin = 8;
	}

	ddrSPD.tRAS = mtb_to_cycles((unsigned int) spd[SPD_TRAS_LSB] | 
			(((unsigned int) spd[SPD_TRAS_TRC_MSB] & 0xF) << 8));
	if (ddrSPD.tRAS > 0x20) {
		debug("  optimal tRAS greater than supported by i.MX6, "
				"value saturated.\n");
		ddrSPD.tRAS = 0x20;
	}
	debug("  tRAS: %d\n", ddrSPD.tRAS);

	ddrSPD.tRC = mtb_to_cycles((unsigned int) spd[SPD_TRC_LSB] |
			(((unsigned int) spd[SPD_TRAS_TRC_MSB] & 0xF0) << 4));
	if (ddrSPD.tRC > 0x20) {
		debug("  optimal tRC greater than supported by i.MX6, "
				"value saturated.\n");
		ddrSPD.tRC = 0x20;
	}
	debug("  tRC: %d\n", ddrSPD.tRC);

	ddrSPD.tRFCmin = mtb_to_cycles((unsigned int) spd[SPD_TRFC_LSB] | 
				((unsigned int) spd[SPD_TRFC_MSB]) << 8 );
	if (ddrSPD.tRFCmin > 0x100) {
		ddrSPD.tRFCmin = 0x100;
		debug("  Info: derived tRFCmin exceeded max allowed value "
				"by i.MX6\n");
	}
	debug("  tRFCmin: %d\n", ddrSPD.tRFCmin);

	ddrSPD.tWTRmin = mtb_to_cycles((unsigned int) spd[SPD_WTRMIN]);
	if (ddrSPD.tWTRmin > 0x8) {
		debug("  optimal tWTRmin greater than supported by i.MX6, "
				"value saturated.\n");
		ddrSPD.tWTRmin = 0x8;
	}
	debug("  tWTRmin: %d\n", ddrSPD.tWTRmin);

	ddrSPD.tRTPmin = mtb_to_cycles((unsigned int) spd[SPD_RTPMIN]);
	if (ddrSPD.tRTPmin > 0x8) {
		debug("  optimal tRTPmin greater than supported by i.MX6, "
				"value saturated.\n");
		ddrSPD.tRTPmin = 0x8;
	}
	debug("  tRTPmin: %d\n", ddrSPD.tRTPmin);

	ddrSPD.tFAW = mtb_to_cycles((unsigned int) spd[SPD_TFAW_LSB] |
				((unsigned int) spd[SPD_TFAW_MSB]) << 8);
	if (ddrSPD.tFAW > 0x20) {
		ddrSPD.tFAW = 0x20;
		debug("  Info: derived tFAW exceeded max allowed value "
				"by i.MX6\n");
	}
	debug("  tFAW: %d\n", ddrSPD.tFAW);

	if (spd[SPD_THERMAL] & 0x80)
		debug("Info: thermal sensor exists on this module\n");
	else
		debug("Info: no thermal sensor on-module\n");

	ddrSPD.vendorID = spd[SPD_VENDOR_ID_LSB] 
			| (spd[SPD_VENDOR_ID_MSB] << 8);
	debug("Vendor ID: 0x%04x\n", ddrSPD.vendorID);

	for (i = 0; i < 18; i++)
		ddrSPD.name[i] = spd[SPD_NAME + i];
	ddrSPD.name[i] = '\0';
	debug("Module name: %s\n", ddrSPD.name);

	debug("\nReprogramming DDR timings...\n" );

	cfgval = readl(MMDC_P0_BASE_ADDR + MDCTL_OFFSET);
	debug("Original CTL: %08x\n", cfgval);
	cfgval = 0x80000000;
	if(ddrSPD.rank == 2) 
		cfgval |= 0x40000000;
	cfgval |= (ddrSPD.rows - 11) << 24;
	cfgval |= (ddrSPD.cols - 9) << 20;
	cfgval |= 1 << 19; /* burst length = 8 */
	cfgval |= 2 << 16; /* data size is 64 bits */
	debug("Optimal CTL: %08x\n", cfgval);
	writel(cfgval, MMDC_P0_BASE_ADDR + MDCTL_OFFSET);

	cfgval = readl(MMDC_P0_BASE_ADDR + MDASP_OFFSET);
	debug("Original ASP: %08x\n", cfgval);
	cfgval = (ddrSPD.capacity / (256 * ddrSPD.rank)) - 1;
	debug("Optimal ASP: %08x\n", cfgval);
	writel(cfgval, MMDC_P0_BASE_ADDR + MDASP_OFFSET);

	cfgval = readl(MMDC_P0_BASE_ADDR + MDCFG0_OFFSET);
	debug("Original CFG0: %08x\n", cfgval);
	cfgval &= 0x00FFFE00;
	cfgval |= ((ddrSPD.tRFCmin - 1) << 24);
	cfgval |= ((ddrSPD.tFAW - 1) & 0x1F << 4);
	cfgval |= ddrSPD.caslatency - 3;
	debug("Optimal CFG0: %08x\n", cfgval);
	writel(cfgval, MMDC_P0_BASE_ADDR + MDCFG0_OFFSET);

	cfgval = readl(MMDC_P0_BASE_ADDR + MDCFG1_OFFSET);
	debug("Original CFG1: %08x\n", cfgval);
	cfgval &= 0x000081FF;
	cfgval |= ((ddrSPD.tRCDmin - 1) << 29);
	cfgval |= ((ddrSPD.tRPmin - 1) << 26);
	cfgval |= ((ddrSPD.tRC - 1) << 21);
	cfgval |= ((ddrSPD.tRAS -1) << 16);
	cfgval |= ((ddrSPD.tWRmin -1) << 9);
	if ((cfgval & 0x7) + 2 < ddrSPD.caslatency) {
		debug( "Original CFG1 tCWL shorter "
				"than supported cas latency, fixing...\n");
		cfgval &= 0xFFFFFFF8;
		if( ddrSPD.caslatency > 7 )
			cfgval |= 0x6;
		else
			cfgval |= (ddrSPD.caslatency - 2);
	}
	debug("Optimal CFG1: %08x\n", cfgval);
	writel(cfgval, MMDC_P0_BASE_ADDR + MDCFG1_OFFSET);

	cfgval = readl(MMDC_P0_BASE_ADDR + MDCFG2_OFFSET);
	debug("Original CFG2: %08x\n", cfgval);
	cfgval &= 0xFFFF0000;
	cfgval |= ((ddrSPD.tRTPmin - 1) << 6);
	cfgval |= ((ddrSPD.tWTRmin - 1) << 3);
	cfgval |= ((ddrSPD.tRRDmin - 1) << 0);
	debug("Optimal CFG2: %08x\n", cfgval);
	writel(cfgval, MMDC_P0_BASE_ADDR + MDCFG2_OFFSET);

	/*
	 * Write and read back some dummy data to demonstrate 
	 * that ddr3 is not broken
	 */
#ifdef CONFIG_PERFORM_RAM_TEST
	debug("\nReference read/write test prior to tuning\n");
	write_random_data(0);
	read_random_data(0);
#endif

	/* do write (fly-by) calibration */
	debug("\nFly-by calibration\n");
	errorcount = tune_wcal(0x04, NULL);

	/* let it settle in...seems it's necessary */
	if (errorcount != 0) {
		debug("Fly-by calibration seems to have failed. "
			"Guessing values for wcal based on rank...\n");
		if (ddrSPD.rank == 1) {
			/* Parameters for boards built at King Credie */
			writel(0x00390042,
				MMDC_P0_BASE_ADDR + MPWLDECTRL0_OFFSET);
			writel(0x00650057,
				MMDC_P0_BASE_ADDR + MPWLDECTRL1_OFFSET);
			writel(0x00630106,
				MMDC_P1_BASE_ADDR + MPWLDECTRL0_OFFSET);
			writel(0x01060116,
				MMDC_P1_BASE_ADDR + MPWLDECTRL1_OFFSET);
		} else {
			/* Parameters for boards built at King Credie */
			writel(0x00290039,
				MMDC_P0_BASE_ADDR + MPWLDECTRL0_OFFSET);
			writel(0x00160057,
				MMDC_P0_BASE_ADDR + MPWLDECTRL1_OFFSET);
			writel(0x00640158,
				MMDC_P1_BASE_ADDR + MPWLDECTRL0_OFFSET);
			writel(0x0111012C,
				MMDC_P1_BASE_ADDR + MPWLDECTRL1_OFFSET);
		}
	}

	/* Tune DQS delays. For some reason, has to be run twice. */
	debug("\nDQS delay calibration\n");
	tune_delays(dram_sdqs_pad_ctls);
	errorcount = tune_delays(dram_sdqs_pad_ctls);
	if (errorcount != 0) {
		debug("DQS delay calibration has failed. Guessing values "
				"for delay cal based on rank...\n");
		if (ddrSPD.rank == 1) {
			/* Parameters for boards built at King Credie */
			writel(0x456B057A, MPDGCTRL0_PHY0);
			writel(0x057F0607, MPDGCTRL1_PHY0);
			writel(0x46470645, MPDGCTRL0_PHY1);
			writel(0x0651061D, MPDGCTRL1_PHY1);
			writel(0x48444249, MPRDDLCTL_PHY0);
			writel(0x4D4D424E, MPRDDLCTL_PHY1);
			writel(0x322D4132, MPWRDLCTL_PHY0);
			writel(0x3D2E3F37, MPWRDLCTL_PHY1);
		} else {
			/* Parameters for boards built at King Credie */
			writel(0x4604061F, MPDGCTRL0_PHY0);
			writel(0x0555062B, MPDGCTRL1_PHY0);
			writel(0x4672073F, MPDGCTRL0_PHY1);
			writel(0x07010665, MPDGCTRL1_PHY1);
			writel(0x4B3F4145, MPRDDLCTL_PHY0);
			writel(0x48423F47, MPRDDLCTL_PHY1);
			writel(0x39354132, MPWRDLCTL_PHY0);
			writel(0x3C323840, MPWRDLCTL_PHY1);
		}
	}

	/* 
	 * Confirm that the memory is working by read/write demo.
	 * Confirmation currently read out on terminal.
	 */
#ifdef CONFIG_PERFORM_RAM_TEST
	debug("\nReference read/write test post-tuning\n");
	write_random_data(0);
	read_random_data(0);
#endif

	ram_size = ((ddrSPD.capacity / 8) - 256) * 1024 * 1024;
	debug("ddrSPD.capacity: %08x\n", ddrSPD.capacity);
	debug("Ramsize according to SPD: %08x\n", ram_size);

	/* Give the RAM some time to settle, otherwise it isn't ready */
	udelay(1000);

	return ram_size;
}


uint32_t imx6q_ddr_init(void)
{
	uint32_t dram_pads[] = {
		MX6Q_IOM_DRAM_SDQS0,
		MX6Q_IOM_DRAM_SDQS1,
		MX6Q_IOM_DRAM_SDQS2,
		MX6Q_IOM_DRAM_SDQS3,
		MX6Q_IOM_DRAM_SDQS4,
		MX6Q_IOM_DRAM_SDQS5,
		MX6Q_IOM_DRAM_SDQS6,
		MX6Q_IOM_DRAM_SDQS7,
	};

	struct i2c_pads_info i2c_pad_info0_6q = {
		.scl = {
			.i2c_mode = MX6Q_PAD_EIM_D21__I2C1_SCL | PC,
			.gpio_mode = MX6Q_PAD_EIM_D21__GPIO3_IO21 | PC,
			.gp = IMX_GPIO_NR(3, 21)
		},
		.sda = {
			.i2c_mode = MX6Q_PAD_EIM_D28__I2C1_SDA | PC,
			.gpio_mode = MX6Q_PAD_EIM_D28__GPIO3_IO28 | PC,
			.gp = IMX_GPIO_NR(3, 28)
		}
	};

	writel(0x00000030, MX6Q_IOM_DRAM_SDQS0);
	writel(0x00000030, MX6Q_IOM_DRAM_SDQS1);
	writel(0x00000030, MX6Q_IOM_DRAM_SDQS2);
	writel(0x00000030, MX6Q_IOM_DRAM_SDQS3);
	writel(0x00000030, MX6Q_IOM_DRAM_SDQS4);
	writel(0x00000030, MX6Q_IOM_DRAM_SDQS5);
	writel(0x00000030, MX6Q_IOM_DRAM_SDQS6);
	writel(0x00000030, MX6Q_IOM_DRAM_SDQS7);

	writel(0x00000030, MX6Q_IOM_GRP_B0DS);
	writel(0x00000030, MX6Q_IOM_GRP_B1DS);
	writel(0x00000030, MX6Q_IOM_GRP_B2DS);
	writel(0x00000030, MX6Q_IOM_GRP_B3DS);
	writel(0x00000030, MX6Q_IOM_GRP_B4DS);
	writel(0x00000030, MX6Q_IOM_GRP_B5DS);
	writel(0x00000030, MX6Q_IOM_GRP_B6DS);
	writel(0x00000030, MX6Q_IOM_GRP_B7DS);
	writel(0x00000030, MX6Q_IOM_GRP_ADDDS);
	/* 40 Ohm drive strength for cs0/1,sdba2,cke0/1,sdwe */
	writel(0x00000030, MX6Q_IOM_GRP_CTLDS);

	writel(0x00020030, MX6Q_IOM_DRAM_DQM0);
	writel(0x00020030, MX6Q_IOM_DRAM_DQM1);
	writel(0x00020030, MX6Q_IOM_DRAM_DQM2);
	writel(0x00020030, MX6Q_IOM_DRAM_DQM3);
	writel(0x00020030, MX6Q_IOM_DRAM_DQM4);
	writel(0x00020030, MX6Q_IOM_DRAM_DQM5);
	writel(0x00020030, MX6Q_IOM_DRAM_DQM6);
	writel(0x00020030, MX6Q_IOM_DRAM_DQM7);

	writel(0x00020030, MX6Q_IOM_DRAM_CAS);
	writel(0x00020030, MX6Q_IOM_DRAM_RAS);
	writel(0x00020030, MX6Q_IOM_DRAM_SDCLK_0);
	writel(0x00020030, MX6Q_IOM_DRAM_SDCLK_1);

	writel(0x00020030, MX6Q_IOM_DRAM_RESET);
	writel(0x00003000, MX6Q_IOM_DRAM_SDCKE0);
	writel(0x00003000, MX6Q_IOM_DRAM_SDCKE1);

	writel(0x00003030, MX6Q_IOM_DRAM_SDODT0);
	writel(0x00003030, MX6Q_IOM_DRAM_SDODT1);

	/* (differential input) */ 
	writel(0x00020000, MX6Q_IOM_DDRMODE_CTL);
	/* (differential input) */
	writel(0x00020000, MX6Q_IOM_GRP_DDRMODE);
	/* disable ddr pullups */ 
	writel(0x00000000, MX6Q_IOM_GRP_DDRPKE);
	writel(0x00000000, MX6Q_IOM_DRAM_SDBA2);
	/* 40 Ohm drive strength for cs0/1,sdba2,cke0/1,sdwe */
	writel(0x000C0000, MX6Q_IOM_GRP_DDR_TYPE);

	/*
	 * These pins must be set to GPIO inputs, to prevent fighting
	 * with the DDR3 I2C bus
	 */
	static iomux_v3_cfg_t const unused_i2c1_pins[] = {
		MX6Q_PAD_EIM_D28__GPIO3_IO28 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6Q_PAD_EIM_D21__GPIO3_IO21 | MUX_PAD_CTRL(NO_PAD_CTRL),
	};
	imx_iomux_v3_setup_multiple_pads(unused_i2c1_pins,
					ARRAY_SIZE(unused_i2c1_pins));
	gpio_direction_input(IMX_GPIO_NR(3, 28));
	gpio_direction_input(IMX_GPIO_NR(3, 21));

	return imx6qdl_ddr_init(&i2c_pad_info0_6q, dram_pads);
}

uint32_t imx6dl_ddr_init(void)
{
	uint32_t dram_pads[] = {
		MX6DL_IOM_DRAM_SDQS0,
		MX6DL_IOM_DRAM_SDQS1,
		MX6DL_IOM_DRAM_SDQS2,
		MX6DL_IOM_DRAM_SDQS3,
		MX6DL_IOM_DRAM_SDQS4,
		MX6DL_IOM_DRAM_SDQS5,
		MX6DL_IOM_DRAM_SDQS6,
		MX6DL_IOM_DRAM_SDQS7,
	};

	struct i2c_pads_info i2c_pad_info0_6dl = {
		.scl = {
			.i2c_mode = MX6DL_PAD_CSI0_DAT9__I2C1_SCL | PC,
			.gpio_mode = MX6DL_PAD_CSI0_DAT9__GPIO5_IO27 | PC,
			.gp = IMX_GPIO_NR(5, 27)
		},
		.sda = {
			.i2c_mode = MX6DL_PAD_CSI0_DAT8__I2C1_SDA | PC,
			.gpio_mode = MX6DL_PAD_CSI0_DAT8__GPIO5_IO26 | PC,
			.gp = IMX_GPIO_NR(5, 26)
		}
	};

	writel(0x00000030, MX6DL_IOM_DRAM_SDQS0);
	writel(0x00000030, MX6DL_IOM_DRAM_SDQS1);
	writel(0x00000030, MX6DL_IOM_DRAM_SDQS2);
	writel(0x00000030, MX6DL_IOM_DRAM_SDQS3);
	writel(0x00000030, MX6DL_IOM_DRAM_SDQS4);
	writel(0x00000030, MX6DL_IOM_DRAM_SDQS5);
	writel(0x00000030, MX6DL_IOM_DRAM_SDQS6);
	writel(0x00000030, MX6DL_IOM_DRAM_SDQS7);

	writel(0x00000030, MX6DL_IOM_GRP_B0DS);
	writel(0x00000030, MX6DL_IOM_GRP_B1DS);
	writel(0x00000030, MX6DL_IOM_GRP_B2DS);
	writel(0x00000030, MX6DL_IOM_GRP_B3DS);
	writel(0x00000030, MX6DL_IOM_GRP_B4DS);
	writel(0x00000030, MX6DL_IOM_GRP_B5DS);
	writel(0x00000030, MX6DL_IOM_GRP_B6DS);
	writel(0x00000030, MX6DL_IOM_GRP_B7DS);
	writel(0x00000030, MX6DL_IOM_GRP_ADDDS);
	/* 40 Ohm drive strength for cs0/1,sdba2,cke0/1,sdwe */
	writel(0x00000030, MX6DL_IOM_GRP_CTLDS);

	writel(0x00020030, MX6DL_IOM_DRAM_DQM0);
	writel(0x00020030, MX6DL_IOM_DRAM_DQM1);
	writel(0x00020030, MX6DL_IOM_DRAM_DQM2);
	writel(0x00020030, MX6DL_IOM_DRAM_DQM3);
	writel(0x00020030, MX6DL_IOM_DRAM_DQM4);
	writel(0x00020030, MX6DL_IOM_DRAM_DQM5);
	writel(0x00020030, MX6DL_IOM_DRAM_DQM6);
	writel(0x00020030, MX6DL_IOM_DRAM_DQM7);

	writel(0x00020030, MX6DL_IOM_DRAM_CAS);
	writel(0x00020030, MX6DL_IOM_DRAM_RAS);
	writel(0x00020038, MX6DL_IOM_DRAM_SDCLK_0);
	writel(0x00020038, MX6DL_IOM_DRAM_SDCLK_1);

	writel(0x00020030, MX6DL_IOM_DRAM_RESET);
	writel(0x00003000, MX6DL_IOM_DRAM_SDCKE0);
	writel(0x00003000, MX6DL_IOM_DRAM_SDCKE1);

	writel(0x00003030, MX6DL_IOM_DRAM_SDODT0);
	writel(0x00003030, MX6DL_IOM_DRAM_SDODT1);

	/* (differential input) */
	writel(0x00020000, MX6DL_IOM_DDRMODE_CTL);
	/* (differential input) */
	writel(0x00020000, MX6DL_IOM_GRP_DDRMODE);
	/* disable ddr pullups */
	writel(0x00000000, MX6DL_IOM_GRP_DDRPKE);
	writel(0x00000000, MX6DL_IOM_DRAM_SDBA2);
	/* 40 Ohm drive strength for cs0/1,sdba2,cke0/1,sdwe */
	writel(0x000C0000, MX6DL_IOM_GRP_DDR_TYPE);

	/*
	 * These pins must be set to GPIO inputs, to prevent fighting
	 * with the DDR3 I2C bus
	 */
	static iomux_v3_cfg_t const unused_i2c1_pins[] = {
		MX6DL_PAD_CSI0_DAT10__GPIO5_IO28 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6DL_PAD_EIM_D28__GPIO3_IO28 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6DL_PAD_EIM_D21__GPIO3_IO21 | MUX_PAD_CTRL(NO_PAD_CTRL),
	};
	imx_iomux_v3_setup_multiple_pads(unused_i2c1_pins,
					ARRAY_SIZE(unused_i2c1_pins));
	gpio_direction_input(IMX_GPIO_NR(3, 28));
	gpio_direction_input(IMX_GPIO_NR(3, 21));
	gpio_direction_output(IMX_GPIO_NR(5, 28), 1);

	return imx6qdl_ddr_init(&i2c_pad_info0_6dl, dram_pads);
}

int novena_dram_init(void)
{
	if (is_cpu_type(MXC_CPU_MX6Q))
		gd->ram_size = imx6q_ddr_init();
	else
		gd->ram_size = imx6dl_ddr_init();
	printf("Detected RAM size: %lu MB\n", gd->ram_size / 1024 / 1024);
	return 0;
}

int dram_init(void)
{
	return 0;
}
