/*
 * Copyright (C) 2012 Freescale Semiconductor, Inc.
 * Copyright (C) 2013 SolidRun ltd.
 * Copyright (C) 2013 Jon Nettleton <jon.nettleton@gmail.com>.
 *
 * Author: Fabio Estevam <fabio.estevam@freescale.com>
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
 */

#include <asm/arch/clock.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/mx6-pins.h>
#include <asm/arch/mxc_hdmi.h>
#include <asm/arch/sys_proto.h>
#include <asm/errno.h>
#include <asm/gpio.h>
#include <asm/imx-common/iomux-v3.h>
#include <asm/imx-common/boot_mode.h>
#include <asm/io.h>
#include <mmc.h>
#include <fsl_esdhc.h>
#include <miiphy.h>
#include <netdev.h>
#include <ipu_pixfmt.h>

DECLARE_GLOBAL_DATA_PTR;

#define MX6QDL_SET_PAD(p, q) \
	if (is_cpu_type(MXC_CPU_MX6Q)) \
		imx_iomux_v3_setup_pad(MX6Q_##p | q);\
	else \
		imx_iomux_v3_setup_pad(MX6DL_##p | q)

#define USDHC_PAD_CTRL (PAD_CTL_PUS_47K_UP |			\
	PAD_CTL_SPEED_LOW | PAD_CTL_DSE_80ohm |			\
	PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define USDHC_PAD_CLK_CTRL (PAD_CTL_SPEED_LOW |			\
	PAD_CTL_DSE_80ohm | PAD_CTL_SRE_FAST |			\
	PAD_CTL_HYS)

#define USDHC_PAD_GPIO_CTRL (PAD_CTL_PUS_22K_UP |		\
	PAD_CTL_SPEED_LOW | PAD_CTL_DSE_40ohm |			\
	PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

static struct fsl_esdhc_cfg usdhc_cfg[CONFIG_SYS_FSL_USDHC_NUM] = {
	{USDHC2_BASE_ADDR},
	{USDHC3_BASE_ADDR},
};

static struct mmc usdhc_mmc[CONFIG_SYS_FSL_USDHC_NUM];

int board_mmc_getcd(struct mmc *mmc)
{
	struct fsl_esdhc_cfg *cfg = (struct fsl_esdhc_cfg *)mmc->priv;
	int ret;

	if (cfg->esdhc_base == USDHC3_BASE_ADDR) {
		gpio_direction_input(IMX_GPIO_NR(7, 0));
		ret = 1; /* there is no CD for a microSD card */
	}
	else {
		gpio_direction_input(IMX_GPIO_NR(1, 4));
		ret = 1; /* there is no CD for a microSD card */
	}

	return ret;
}

int board_mmc_init(bd_t *bis)
{
	s32 status = 0;
	u32 index = 0;

	for (index = 0; index < CONFIG_SYS_FSL_USDHC_NUM; ++index) {
		switch (index) {
		case 0:
			MX6QDL_SET_PAD(PAD_SD3_CLK__SD3_CLK   , MUX_PAD_CTRL(USDHC_PAD_CLK_CTRL));
			MX6QDL_SET_PAD(PAD_SD3_CMD__SD3_CMD   , MUX_PAD_CTRL(USDHC_PAD_CTRL));
			MX6QDL_SET_PAD(PAD_SD3_DAT0__SD3_DATA0, MUX_PAD_CTRL(USDHC_PAD_CTRL));
			MX6QDL_SET_PAD(PAD_SD3_DAT1__SD3_DATA1, MUX_PAD_CTRL(USDHC_PAD_CTRL));
			MX6QDL_SET_PAD(PAD_SD3_DAT2__SD3_DATA2, MUX_PAD_CTRL(USDHC_PAD_CTRL));
			MX6QDL_SET_PAD(PAD_SD3_DAT3__SD3_DATA3, MUX_PAD_CTRL(USDHC_PAD_CTRL));
			usdhc_cfg[index].sdhc_clk = mxc_get_clock(MXC_ESDHC3_CLK);
			break;
		case 1:
			MX6QDL_SET_PAD(PAD_SD2_CLK__SD2_CLK   , MUX_PAD_CTRL(USDHC_PAD_CLK_CTRL));
			MX6QDL_SET_PAD(PAD_SD2_CMD__SD2_CMD   , MUX_PAD_CTRL(USDHC_PAD_CTRL));
			MX6QDL_SET_PAD(PAD_SD2_DAT0__SD2_DATA0, MUX_PAD_CTRL(USDHC_PAD_CTRL));
			MX6QDL_SET_PAD(PAD_SD2_DAT1__SD2_DATA1, MUX_PAD_CTRL(USDHC_PAD_CTRL));
			MX6QDL_SET_PAD(PAD_SD2_DAT2__SD2_DATA2, MUX_PAD_CTRL(USDHC_PAD_CTRL));
			MX6QDL_SET_PAD(PAD_SD2_DAT3__SD2_DATA3, MUX_PAD_CTRL(USDHC_PAD_CTRL));
			/* MicroSD Card Detect */
			MX6QDL_SET_PAD(PAD_GPIO_4__GPIO1_IO04 , MUX_PAD_CTRL(NO_PAD_CTRL));
			usdhc_cfg[index].sdhc_clk = mxc_get_clock(MXC_ESDHC2_CLK);

			break;
		default:
			printf("Warning: you configured more USDHC controllers"
				"(%d) then supported by the board (%d)\n",
				index + 1, CONFIG_SYS_FSL_USDHC_NUM);
			return status;
		}

		status |= fsl_esdhc_initialize_withmmc(bis, &usdhc_cfg[index],
			&usdhc_mmc[index]);
	}

	return status;
}

char *config_sys_prompt = "Novena U-Boot > ";

int board_early_init_f(void)
{
	return 0;
}

int board_init(void)
{
	return 0;
}

int checkboard(void)
{
	return 0;
}

#ifdef CONFIG_CMD_BMODE
static const struct boot_mode board_boot_modes[] = {
	/* 4 bit bus width */
	{"mmc0", MAKE_CFGVAL(0x40, 0x28, 0x00, 0x00)},
	{NULL,	 0},
};
#endif

int board_late_init(void)
{
        int cpurev = get_cpu_rev();
        setenv("cpu",get_imx_type((cpurev & 0xFF000) >> 12));

#ifdef CONFIG_CMD_BMODE
        add_board_boot_modes(board_boot_modes);
#endif

	return 0;
}
