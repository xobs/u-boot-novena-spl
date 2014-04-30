/*
 * Author: Tungyi Lin <tungyilin1127@gmail.com>
 *
 * Derived from EDM_CF_IMX6 code by TechNexion,Inc
 * Ported to SolidRun microSOM by Rabeeh Khoury <rabeeh@solid-run.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
#include <common.h>
#include <asm/io.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/mx6-pins.h>
#include <asm/imx-common/iomux-v3.h>
#ifdef CONFIG_SPL
#include <spl.h>
#endif

#define CONFIG_SPL_STACK	0x0091FFB8

#define __REG16(x)	(*((volatile u16 *)(x)))

#define UART_PAD_CTRL  (PAD_CTL_PUS_100K_UP | PAD_CTL_SPEED_MED |	\
			PAD_CTL_DSE_40ohm   | PAD_CTL_SRE_FAST  | PAD_CTL_HYS)


DECLARE_GLOBAL_DATA_PTR;

#define URTX0           0x40            /* Transmitter Register */

#define UCR1            0x80            /* Control Register 1 */
#define UCR1_UARTEN     (1 << 0)        /* UART enabled */

#define USR2            0x98            /* Status Register 2 */
#define USR2_TXDC       (1 << 3)        /* Transmitter complete */

extern int novena_dram_init(void);

void PUTC_LL(int c)
{
        void __iomem *base = (void *)CONFIG_MXC_UART_BASE;

        if (!base)
                return;

        if (!(readl(base + UCR1) & UCR1_UARTEN))
                return;

        while (!(readl(base + USR2) & USR2_TXDC));

        writel(c, base + URTX0);
}

#if defined(CONFIG_SPL_BUILD)

static enum boot_device boot_dev;
enum boot_device get_boot_device(void);

static inline void setup_boot_device(void)
{
	uint soc_sbmr = readl(SRC_BASE_ADDR + 0x4);
	uint bt_mem_ctl = (soc_sbmr & 0x000000FF) >> 4 ;
	uint bt_mem_type = (soc_sbmr & 0x00000008) >> 3;
	uint bt_mem_mmc = (soc_sbmr & 0x00001000) >> 12;

	switch (bt_mem_ctl) {
	case 0x0:
		if (bt_mem_type)
			boot_dev = MX6_ONE_NAND_BOOT;
		else
			boot_dev = MX6_WEIM_NOR_BOOT;
		break;
	case 0x2:
			boot_dev = MX6_SATA_BOOT;
		break;
	case 0x3:
		if (bt_mem_type)
			boot_dev = MX6_I2C_BOOT;
		else
			boot_dev = MX6_SPI_NOR_BOOT;
		break;
	case 0x4:
	case 0x5:
		if (bt_mem_mmc)
			boot_dev = MX6_SD0_BOOT;
		else
			boot_dev = MX6_SD1_BOOT;
		break;
	case 0x6:
	case 0x7:
		boot_dev = MX6_MMC_BOOT;
		break;
	case 0x8 ... 0xf:
		boot_dev = MX6_NAND_BOOT;
		break;
	default:
		boot_dev = MX6_UNKNOWN_BOOT;
		break;
	}
}

enum boot_device get_boot_device(void) {
	return MX6_SD0_BOOT;
	//return boot_dev;
}

static void spl_mx6qdl_uart_setup_iomux(void)
{
	u32 cpurev, imxtype;
	iomux_v3_cfg_t const uart2_pads_6q[] = {
		MX6Q_PAD_EIM_D26__UART2_TX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
		MX6Q_PAD_EIM_D27__UART2_RX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
	};

	iomux_v3_cfg_t const uart2_pads_6dl[] = {
		MX6DL_PAD_EIM_D26__UART2_TX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
		MX6DL_PAD_EIM_D27__UART2_RX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
	};

	cpurev = get_cpu_rev();
	imxtype = (cpurev & 0xFF000) >> 12;

	get_imx_type(imxtype);

	switch (imxtype) {
	case MXC_CPU_MX6Q:
		imx_iomux_v3_setup_multiple_pads(uart2_pads_6q,
			ARRAY_SIZE(uart2_pads_6q));
		break;
	case MXC_CPU_MX6DL:
	default:
		imx_iomux_v3_setup_multiple_pads(uart2_pads_6dl,
			ARRAY_SIZE(uart2_pads_6dl));
		break;
	}
}

static void spl_uart_init(void)
{
	int i;
	void __iomem *ccmbase = (void *)CCM_BASE_ADDR;
	void __iomem *uartbase = (void *)CONFIG_MXC_UART_BASE;

	spl_mx6qdl_uart_setup_iomux();

	writel(0xffffffff, ccmbase + 0x68);
	writel(0xffffffff, ccmbase + 0x6c);
	writel(0xffffffff, ccmbase + 0x70);
	writel(0xffffffff, ccmbase + 0x74);
	writel(0xffffffff, ccmbase + 0x78);
	writel(0xffffffff, ccmbase + 0x7c);
	writel(0xffffffff, ccmbase + 0x80);

	writel(0x00000000, uartbase + 0x80);
	writel(0x00004027, uartbase + 0x84);
	writel(0x00000704, uartbase + 0x88);
	writel(0x00000a81, uartbase + 0x90);
	writel(0x0000002b, uartbase + 0x9c);
	writel(0x00013880, uartbase + 0xb0);
	writel(0x0000047f, uartbase + 0xa4);
	writel(0x0000c34f, uartbase + 0xa8);
	writel(0x00000001, uartbase + 0x80);

	PUTC_LL('\r');
	PUTC_LL('\n');
}

/* Yank the FPGA "reset" GPIO, which frees the I2C bus */
static void reset_fpga(void)
{
	u32 cpurev, imxtype;
	iomux_v3_cfg_t const fpga_reset_pads_6q[] = {
		MX6Q_PAD_DISP0_DAT13__GPIO5_IO07 | MUX_PAD_CTRL(NO_PAD_CTRL),
	};

	iomux_v3_cfg_t const fpga_reset_pads_6dl[] = {
		MX6DL_PAD_DISP0_DAT13__GPIO5_IO07 | MUX_PAD_CTRL(NO_PAD_CTRL),
	};

	cpurev = get_cpu_rev();
	imxtype = (cpurev & 0xFF000) >> 12;

	get_imx_type(imxtype);

	switch (imxtype) {
	case MXC_CPU_MX6Q:
		imx_iomux_v3_setup_multiple_pads(fpga_reset_pads_6q,
			ARRAY_SIZE(fpga_reset_pads_6q));
		break;
	case MXC_CPU_MX6DL:
	default:
		imx_iomux_v3_setup_multiple_pads(fpga_reset_pads_6dl,
			ARRAY_SIZE(fpga_reset_pads_6dl));
		break;
	}

	/* Pull FPGA reset line to 0, disabling power */
	gpio_direction_output(IMX_GPIO_NR(5, 7), 0);
}

void board_init_f(ulong dummy)
{
	/* Set the stack pointer. */
	asm volatile("mov sp, %0\n" : : "r"(CONFIG_SPL_STACK));

	arch_cpu_init();

	spl_uart_init();

	/* Clear the BSS. */
	//memset(__bss_start, 0, __bss_end - __bss_start);

	/* Set global data pointer. */
	gd = &gdata;

	board_early_init_f();
	preloader_console_init();

	board_init_r(NULL, 0);
}

int spl_start_uboot(void)
{
	/* Yes, we should start U-Boot */
        return 1;
}

void __noreturn jump_to_image_no_args(struct spl_image_info *spl_image)
{
        typedef void __noreturn (*image_entry_noargs_t)(int r0, int r1, int r2);

        image_entry_noargs_t image_entry =
                        (image_entry_noargs_t) spl_image->entry_point;

        printf("image entry point: 0x%X\n", spl_image->entry_point);
        image_entry(gd->ram_size, 0, 0);
}

void spl_board_init(void)
{
	novena_dram_init();

	setup_boot_device();
}

u32 spl_boot_device(void)
{
	return BOOT_DEVICE_MMC1;
/*
	puts("Boot Device: ");
	switch (get_boot_device()) {
	case MX6_SD0_BOOT:
		printf("SD0\n");
		return BOOT_DEVICE_MMC1;
	case MX6_SD1_BOOT:
		printf("SD1\n");
		return BOOT_DEVICE_MMC2;
	case MX6_MMC_BOOT:
		printf("MMC\n");
		return BOOT_DEVICE_MMC2;
	case MX6_NAND_BOOT:
		printf("NAND\n");
		return BOOT_DEVICE_NAND;
	case MX6_SATA_BOOT:
		printf("SATA\n");
		return BOOT_DEVICE_SATA;
	case MX6_UNKNOWN_BOOT:
	default:
		printf("UNKNOWN\n");
		return BOOT_DEVICE_NONE;
	}
*/
}

u32 spl_boot_mode(void)
{
	return MMCSD_MODE_FAT;
/*
	switch (spl_boot_device()) {
	case BOOT_DEVICE_MMC1:
	case BOOT_DEVICE_MMC2:
	case BOOT_DEVICE_MMC2_2:
		return MMCSD_MODE_ANY;
		break;
	case BOOT_DEVICE_SATA:
		return MMCSD_MODE_UNDEFINED;
		break;
	default:
		puts("spl: ERROR:  unsupported device\n");
		hang();
	}
*/
}

void reset_cpu(ulong addr)
{
	__REG16(WDOG1_BASE_ADDR) = 4;
}
#endif

