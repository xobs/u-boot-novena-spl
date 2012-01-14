/*
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
/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

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

*******************************************************************************/

#include <common.h>
#include "mvCommon.h"
#include "mvBoardEnvLib.h"
#include "mvCpuIf.h"
#include "mvCtrlEnvLib.h"
#include "mv_mon_init.h"
#include "mvDebug.h"
#include "device/mvDevice.h"
#include "twsi/mvTwsi.h"
#include "pci-if/pex/mvPex.h"
#include "gpp/mvGppRegs.h"
#include "mvSysEthPhyApi.h"

#if defined(MV_INCLUDE_XOR)
#include "mvSysXorApi.h"
#include "xor/mvXor.h"
#endif
#if defined(MV_INCLUDE_PDMA)
#include "mvSysPdmaApi.h"
#include "pdma/mvPdma.h"
#endif
#if defined(MV_INCLUDE_USB)
#include "mvSysUsbApi.h"
#include "usb/mvUsb.h"
#endif

#ifdef MV_INCLUDE_RTC
#include "rtc/integ_rtc/mvRtc.h"
#include "rtc.h"
#elif CONFIG_RTC_DS1338_DS1339
#include "rtc/ext_rtc/mvDS133x.h"
#endif

#if defined(MV_INCLUDE_CLK_PWR_CNTRL) || defined(MV_INCLUDE_PMU)
#include "pmu/mvPmuRegs.h"
#include "pmu/mvPmu.h"
#include "mv_dvs.h"
u32 dvs_values_param = 0;
#endif


#include "cpu/mvCpu.h"
#include "nand.h"
#include "spi_flash.h"
#ifdef CONFIG_PCI
	#include <pci.h>
#endif
#include "pci-if/pci/mvPciRegs.h"

#include <asm/arch-arm1136/vfpinstr.h>
#include <asm/arch-arm1136/vfp.h>

#include <net.h>
#include <netdev.h>
#include <command.h>

extern void mvMD5(unsigned char const *buf, unsigned len, unsigned char* digest);

/* #define MV_DEBUG */
#ifdef MV_DEBUG
#define DB(x) x
#else
#define DB(x)
#endif

/* CPU address decode table. */
MV_CPU_DEC_WIN mvCpuAddrWinMap[] = MV_CPU_IF_ADDR_WIN_MAP_TBL;
#if 0
#if defined(CONFIG_CMD_RCVR)
extern void recoveryDetection(void);
#endif
#endif

void mv_cpu_init(void);
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
int mv_set_power_scheme(void);
#endif

#ifdef	CONFIG_FLASH_CFI_DRIVER
MV_VOID mvUpdateNorFlashBaseAddrBank(MV_VOID);
int mv_board_num_flash_banks;
extern flash_info_t	flash_info[]; /* info for FLASH chips */
extern unsigned long flash_add_base_addr (uint flash_index, ulong flash_base_addr);
#endif	/* CONFIG_FLASH_CFI_DRIVER */

#if defined(MV_INCLUDE_GIG_ETH)
extern MV_VOID mvEgigaPhySwitchInit(void);
#endif 

/* Define for SDK 2.0 */
int __aeabi_unwind_cpp_pr0(int a,int b,int c) {return 0;}
int __aeabi_unwind_cpp_pr1(int a,int b,int c) {return 0;}

extern nand_info_t nand_info[];       /* info for NAND chips */

extern struct spi_flash *flash;
//MV_VOID mvMppModuleTypePrint(MV_VOID);

#ifdef MV_NAND_BOOT
extern MV_U32 nandEnvBase;
#endif

MV_VOID mvMppModuleTypePrint(MV_VOID);

#if 0
/* Define for SDK 2.0 */
int raise(void) {return 0;}
#endif //0
void print_mvBanner(void)
{
#ifdef CONFIG_SILENT_CONSOLE
	DECLARE_GLOBAL_DATA_PTR;
	gd->flags |= GD_FLG_SILENT;
#endif
	printf("\n");
	printf(" __   __                      _ _\n");
	printf("|  \\/  | __ _ _ ____   _____| | |\n");
	printf("| |\\/| |/ _` | '__\\ \\ / / _ \\ | |\n");
	printf("| |  | | (_| | |   \\ V /  __/ | |\n");
	printf("|_|  |_|\\__,_|_|    \\_/ \\___|_|_|\n");
	printf("         _   _     ____              _\n");
	printf("        | | | |   | __ )  ___   ___ | |_ \n");
	printf("        | | | |___|  _ \\ / _ \\ / _ \\| __| \n");
	printf("        | |_| |___| |_) | (_) | (_) | |_ \n");
	printf("         \\___/    |____/ \\___/ \\___/ \\__| \n");
//#if !defined(MV_NAND_BOOT)
#if defined(MV_INCLUDE_MONT_EXT)
	if(!enaMonExt())
		printf(" ** LOADER **\n"); 
	else
		printf(" ** MONITOR **\n");

#else

	printf(" ** LOADER **\n"); 
#endif /* MV_INCLUDE_MONT_EXT */
	return;
}

void maskAllInt(void)
{
        /* mask all external interrupt sources */
        MV_REG_WRITE(CPU_MAIN_IRQ_MASK_REG, 0);
        MV_REG_WRITE(CPU_MAIN_FIQ_MASK_REG, 0);
        MV_REG_WRITE(CPU_ENPOINT_MASK_REG, 0);
        MV_REG_WRITE(CPU_MAIN_IRQ_MASK_HIGH_REG, 0);
        MV_REG_WRITE(CPU_MAIN_FIQ_MASK_HIGH_REG, 0);
        MV_REG_WRITE(CPU_ENPOINT_MASK_HIGH_REG, 0);
}

/* init for the Master*/
void misc_init_r_dec_win(void)
{
#if defined(MV_INCLUDE_USB)
	{
		char *env;

		env = getenv("usb0Mode");
		if((!env) || (strcmp(env,"device") == 0) || (strcmp(env,"Device") == 0) )
		{
			printf("USB 0: Device Mode\n");	
			mvSysUsbInit(0, MV_FALSE);
		}
		else
		{
			printf("USB 0: Host Mode\n");	
			mvSysUsbInit(0, MV_TRUE);
		}

		env = getenv("usb1Mode");
		if((!env) || (strcmp(env,"device") == 0) || (strcmp(env,"Device") == 0) )
		{
			printf("USB 1: Device Mode\n");	
			mvSysUsbInit(1, MV_FALSE);
		}
		else
		{
			printf("USB 1: Host Mode\n");	
			mvSysUsbInit(1, MV_TRUE);
		}

	}
#endif/* #if defined(MV_INCLUDE_USB) */

#if defined(MV_INCLUDE_XOR)
	mvSysXorInit();
#endif

#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
	mv_set_power_scheme();
#endif

    return;
}

/*
 * Miscellaneous platform dependent initialisations
 */

extern	MV_STATUS mvEthPhyRegRead(MV_U32 phyAddr, MV_U32 regOffs, MV_U16 *data);
extern	MV_STATUS mvEthPhyRegWrite(MV_U32 phyAddr, MV_U32 regOffs, MV_U16 data);

/* golabal mac address for yukon EC */
unsigned char yuk_enetaddr[6];
//alior extern int interrupt_init (void);
extern int timer_init(void );
extern void i2c_init(int speed, int slaveaddr);

#if defined(MV_INCLUDE_PMU)
static int dovetemp_init_sensor(void)
{
	u32 reg;
	u32 i;

	/* Configure the Diode Control Register #0 */
        reg = MV_REG_READ(PMU_TEMP_DIOD_CTRL0_REG);
	/* Use average of 2 */
	reg &= ~PMU_TDC0_AVG_NUM_MASK;
	reg |= (0x1 << PMU_TDC0_AVG_NUM_OFFS);
	/* Reference calibration value */
	reg &= ~PMU_TDC0_REF_CAL_CNT_MASK;
	reg |= (0x0F1 << PMU_TDC0_REF_CAL_CNT_OFFS);
	/* Set the high level reference for calibration */
	reg &= ~PMU_TDC0_SEL_VCAL_MASK;
	reg |= (0x2 << PMU_TDC0_SEL_VCAL_OFFS);
	MV_REG_WRITE(PMU_TEMP_DIOD_CTRL0_REG, reg);

	/* Reset the sensor */
	reg = MV_REG_READ(PMU_TEMP_DIOD_CTRL0_REG);
	MV_REG_WRITE(PMU_TEMP_DIOD_CTRL0_REG, (reg | PMU_TDC0_SW_RST_MASK));
	MV_REG_WRITE(PMU_TEMP_DIOD_CTRL0_REG, reg);

	/* Enable the sensor */
	reg = MV_REG_READ(PMU_THERMAL_MNGR_REG);
	reg &= ~PMU_TM_DISABLE_MASK;
	MV_REG_WRITE(PMU_THERMAL_MNGR_REG, reg);

	/* Poll the sensor for the first reading */
	for (i=0; i< 1000000; i++) {
		reg = MV_REG_READ(PMU_THERMAL_MNGR_REG);
		if (reg & PMU_TM_CURR_TEMP_MASK)
			break;;
	}

	if (i== 1000000)
		return -1;

	return 0;
}
#endif


int board_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;
#if defined(MV_INCLUDE_TWSI)
	MV_TWSI_ADDR slave;
#endif
	unsigned int i;

	maskAllInt();

	/* must initialize the int in order for udelay to work */
	//alior interrupt_init();
	timer_init();

#if defined(MV_INCLUDE_TWSI)
	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, CONFIG_SYS_I2C_SPEED, CONFIG_SYS_TCLK, &slave, 0);
#endif
 
	/* Init the Board environment module (device bank params init) */
	mvBoardEnvInit();

	/* Init the Controlloer environment module (MPP init) */
	mvCtrlEnvInit();

	mvBoardDebugLed(3);

	/* Init the Controller CPU interface */
	mvCpuIfInit(mvCpuAddrWinMap);

	/* arch number of Integrator Board */
	gd->bd->bi_arch_number = 527;

	/* adress of boot parameters */
	gd->bd->bi_boot_params = 0x00000100;

	/* relocate the exception vectors */
	/* U-Boot is running from DRAM at this stage */
	for(i = 0; i < 0x100; i+=4)
	{
		*(unsigned int *)(0x0 + i) = *(unsigned int*)(TEXT_BASE + i);
	}
	
	/* Update NOR flash base address bank for CFI driver */
#ifdef	CONFIG_FLASH_CFI_DRIVER
	mvUpdateNorFlashBaseAddrBank();
#endif	/* CONFIG_FLASH_CFI_DRIVER */

#if defined(MV_INCLUDE_GIG_ETH)
	/* Init the PHY or Switch of the board */
	mvSysEthPhyInit();
	mvEgigaPhySwitchInit();
#endif /* defined(MV_INCLUDE_GIG_ETH) */

#if defined(MV_INCLUDE_PDMA)
	mvSysPdmaInit(); /* Must be initialized before nand_init() */
#endif

#if defined(MV_INCLUDE_PMU)
	/* Basic initialization of the PMU Temp Sensor */
	dovetemp_init_sensor();
	mvSysPmuInit();

	switch (mvCpuPclkGet()/1000000) {
		case 1000:
			dvs_values_param = (DOVE_1000_PSET_HI << DOVE_PSET_HI_OFFSET) | (DOVE_1000_VSET_HI << DOVE_VSET_HI_OFFSET) | (DOVE_1000_PSET_LO << DOVE_PSET_LO_OFFSET) | (DOVE_1000_VSET_LO << DOVE_VSET_LO_OFFSET);
			break;
		case 800:
		default:
			dvs_values_param = (DOVE_800_PSET_HI << DOVE_PSET_HI_OFFSET) | (DOVE_800_VSET_HI << DOVE_VSET_HI_OFFSET) | (DOVE_800_PSET_LO << DOVE_PSET_LO_OFFSET) | (DOVE_800_VSET_LO << DOVE_VSET_LO_OFFSET);
			break;
	}
#endif
	mvBoardDebugLed(4);

	return 0;
}

void misc_init_r_env(void){
	char *env;
	char tmp_buf[10];
	unsigned int malloc_len;
	DECLARE_GLOBAL_DATA_PTR;

	unsigned int flashSize =0 , secSize =0, ubootSize =0;
	char buff[256];

	env = getenv("console");
	if(!env)
#if defined(MV_NAND_GANG_MODE) && defined(MV_NAND_4K8LC_DEVICE)
			setenv("console","console=ttyS0,115200 useNandHal=ganged,8bitecc");
#elif defined(MV_NAND_GANG_MODE)
			setenv("console","console=ttyS0,115200 useNandHal=ganged");
#elif defined(MV_NAND_4K8LC_DEVICE)
			setenv("console","console=ttyS0,115200 useNandHal=8bitecc");
#else
			setenv("console","console=ttyS0,115200 useNandHal=single");
#endif

	env = getenv("mtdids");
	if(!env) {
#if !defined(RD_88F6781_AVNG) && !defined(RD_88F6781Y0_AVNG) && !defined(RD_88F6781X0_AVNG) && !defined(RD_88AP510A0_AVNG)
		setenv("mtdids","nand0=dove-nand");
#endif
	}
	env = getenv("mtdparts");
	if(!env) {
#if defined(RD_88F6781_AVNG) || defined (RD_88F6781Y0_AVNG) || defined (RD_88F6781X0_AVNG) || defined (RD_88AP510A0_AVNG)
		setenv("mtdparts","mtdparts=spi0.0:768k(u-boot)ro,128k(uboot_env),-(kernel)");
#else
	#if defined(MV_SPI_BOOT)
			setenv("mtdparts","mtdparts=dove-nand:4m(boot),-(rootfs)");
	#elif defined(MV_NAND_BOOT)
			setenv("mtdparts", "mtdparts=dove-nand:1m(u-boot),4m(boot),-(rootfs)");
	#else
			#error "No boot device configured!"
	#endif
#endif /*defined(RD_88F6781_AVNG) || defined (RD_88F6781Y0_AVNG) || defined (RD_88F6781X0_AVNG) || defined (RD_88AP510A0_AVNG*/
	}



#if defined (CONFIG_MARVELL_TAG)
	env = getenv("passDramInitTag");
	if(!env || ((strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0))) {
		setenv("passDramInitTag","yes");
	}
	else
		setenv("passDramInitTag","no");
#endif
		
        /* Linux open port support */
	env = getenv("mainlineLinux");
	if(!env || ((strcmp(env,"yes") == 0) ||  (strcmp(env,"Yes") == 0)))
		setenv("mainlineLinux","yes");
	else
		setenv("mainlineLinux","no");

	env = getenv("mainlineLinux");
	if(env && ((strcmp(env,"yes") == 0) ||  (strcmp(env,"Yes") == 0)))
	{
	    /* arch number for open port Linux */
	    env = getenv("arcNumber");
	    if(!env )
	    {
		/* arch number according to board ID */
		int board_id = mvBoardIdGet();	
		switch(board_id){
		    case(DB_88F6781_BP_ID):
			sprintf(tmp_buf,"%d", DB_88F6781_BP_MLL_ID);
			board_id = DB_88F6781_BP_MLL_ID; 
			break;
		    case(RD_88F6781_ID):
			sprintf(tmp_buf,"%d", RD_88F6781_MLL_ID);
			board_id = RD_88F6781_MLL_ID; 
			break;
		    case(RD_88F6781_AVNG_ID):
			sprintf(tmp_buf,"%d", RD_88F6781_AVNG_MLL_ID);
			board_id = RD_88F6781_AVNG_MLL_ID; 
			break;
            case(DB_88F6781Y0_BP_ID):
			sprintf(tmp_buf,"%d", DB_88F6781Y0_BP_MLL_ID);
			board_id = DB_88F6781Y0_BP_MLL_ID; 
			break;
            case(DB_88AP510BP_B_BP_ID):
			sprintf(tmp_buf,"%d", DB_88AP510BP_B_BP_MLL_ID);
			board_id = DB_88AP510BP_B_BP_MLL_ID; 
			break;
            case(DB_88AP510_PCAC_ID):
			sprintf(tmp_buf,"%d", DB_88AP510_PCAC_MLL_ID);
			board_id = DB_88AP510_PCAC_MLL_ID; 
			break;
            case(RD_88F6781X0_PLUG_ID):
            sprintf(tmp_buf,"%d", RD_88F6781X0_PLUG_MLL_ID);
            board_id = RD_88F6781X0_PLUG_MLL_ID; 
            break;
		    case(RD_88F6781Y0_AVNG_ID):
			sprintf(tmp_buf,"%d", RD_88F6781Y0_AVNG_MLL_ID);
			board_id = RD_88F6781Y0_AVNG_MLL_ID;
			break;
			case(RD_88AP510A0_AVNG_ID):
			sprintf(tmp_buf,"%d", RD_88AP510A0_AVNG_MLL_ID);
			board_id = RD_88AP510A0_AVNG_MLL_ID;
			break;
		    default:
			sprintf(tmp_buf,"%d", board_id);
			board_id = board_id; 
			break;
		}
		gd->bd->bi_arch_number = board_id;
		setenv("arcNumber", tmp_buf);
	    }
	    else
	    {
		gd->bd->bi_arch_number = simple_strtoul(env, NULL, 10);
	    }
	}

        /* update the CASset env parameter */
        env = getenv("CASset");
        if(!env )
        {
#ifdef MV_MIN_CAL
                setenv("CASset","min");
#else
                setenv("CASset","max");
#endif
        }
        /* Monitor extension */
#ifdef MV_INCLUDE_MONT_EXT
		env = getenv("enaMonExt");
		if(/* !env || */ ( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) ) ) {
				setenv("enaMonExt","yes");
				setenv("enaPT","yes");
		}
		else
#endif
			setenv("enaMonExt","no");

#if defined (MV_INC_BOARD_NOR_FLASH)
        env = getenv("enaFlashBuf");
        if( ( (strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) ) )
                setenv("enaFlashBuf","no");
        else
                setenv("enaFlashBuf","yes");
#endif

#if 0
	/* Note: Write Allocate is a page attribute, not controlled by a global bit */
	/* Write allocation */
	env = getenv("enaWrAllo");
	if( !env || ( ((strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) )))
		setenv("enaWrAllo","no");
	else
		setenv("enaWrAllo","yes");
#endif

	/* Pex mode */
	env = getenv("pexMode");
	if( env && ( ((strcmp(env,"EP") == 0) || (strcmp(env,"ep") == 0) )))
		setenv("pexMode","EP");
	else
		setenv("pexMode","RC");

    	env = getenv("disL2Cache");
    	if(!env || ( (strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) ) )
        	setenv("disL2Cache","no"); 
    	else
        	setenv("disL2Cache","yes");

#if 0
	/* Note: Always disable L2 Prefetch, don't give parameter to user */
    	env = getenv("disL2Prefetch");
    	if(!env || ( (strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) ) )
        	setenv("disL2Prefetch","no"); 
    	else
        	setenv("disL2Prefetch","yes");
#endif

    	env = getenv("disL2Ecc");
    	if(!env || ( (strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) ) )
        	setenv("disL2Ecc","no"); 
    	else
        	setenv("disL2Ecc","yes");

#if 0
	/* TODO: what should be the default for L2 - WT/WB ? */
    	env = getenv("setL2CacheWT");
    	if(!env || ( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) ) )
        	setenv("setL2CacheWT","yes"); 
    	else
        	setenv("setL2CacheWT","no");
#endif

        env = getenv("sata_dma_mode");
        if( env && ((strcmp(env,"No") == 0) || (strcmp(env,"no") == 0) ) )
                setenv("sata_dma_mode","no");
        else
                setenv("sata_dma_mode","yes");


        /* Malloc length */
        env = getenv("MALLOC_len");
        malloc_len =  simple_strtoul(env, NULL, 10) << 20;
        if(malloc_len == 0){
		sprintf(tmp_buf,"%d",CONFIG_SYS_MALLOC_LEN>>20);
                setenv("MALLOC_len",tmp_buf);
	}
         
        /* primary network interface */
        env = getenv("ethprime");
	if(!env)
		setenv("ethprime",ENV_ETH_PRIME);
 
	/* netbsd boot arguments */
        env = getenv("netbsd_en");
	if( !env || ( ((strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) )))
		setenv("netbsd_en","no");
	else
	{
	    setenv("netbsd_en","yes");
	    env = getenv("netbsd_gw");
	    if(!env)
                setenv("netbsd_gw","192.168.0.254");

	    env = getenv("netbsd_mask");
	    if(!env)
                setenv("netbsd_mask","255.255.255.0");

	    env = getenv("netbsd_fs");
	    if(!env)
                setenv("netbsd_fs","nfs");

	    env = getenv("netbsd_server");
	    if(!env)
                setenv("netbsd_server","192.168.0.1");

	    env = getenv("netbsd_ip");
	    if(!env)
	    {
		env = getenv("ipaddr");
               	setenv("netbsd_ip",env);
	    }

	    env = getenv("netbsd_rootdev");
	    if(!env)
                setenv("netbsd_rootdev","mgi0");

	    env = getenv("netbsd_add");
	    if(!env)
                setenv("netbsd_add","0x800000");

	    env = getenv("netbsd_get");
	    if(!env)
                setenv("netbsd_get","tftpboot ${netbsd_add} ${image_name}");

	    env = getenv("netbsd_set_args");
	    if(!env)
	    	setenv("netbsd_set_args","setenv bootargs nfsroot=${netbsd_server}:${rootpath} fs=${netbsd_fs} \
ip=${netbsd_ip} serverip=${netbsd_server} mask=${netbsd_mask} gw=${netbsd_gw} rootdev=${netbsd_rootdev} \
ethaddr=${ethaddr} ${netbsd_netconfig}");

	    env = getenv("netbsd_boot");
	    if(!env)
	    	setenv("netbsd_boot","bootm ${netbsd_add} ${bootargs}");

	    env = getenv("netbsd_bootcmd");
	    if(!env)
	    	setenv("netbsd_bootcmd","run netbsd_get ; run netbsd_set_args ; run netbsd_boot");
	}

	/* vxWorks boot arguments */
        env = getenv("vxworks_en");
	if( !env || ( ((strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) )))
		setenv("vxworks_en","no");
	else
	{
	    char* buff = (char *)0x1100;
	    setenv("vxworks_en","yes");
		
	    sprintf(buff,"mgi(0,0) host:vxWorks.st");
	    env = getenv("serverip");
	    strcat(buff, " h=");
	    strcat(buff,env);
	    env = getenv("ipaddr");
	    strcat(buff, " e=");
	    strcat(buff,env);
	    strcat(buff, ":ffff0000 u=anonymous pw=target ");

	    setenv("vxWorks_bootargs",buff);
	}

        /* linux boot arguments */
        env = getenv("bootargs_root");
        if(!env)
                setenv("bootargs_root","root=/dev/nfs rw");
 
	/* For open Linux we set boot args differently */
	env = getenv("mainlineLinux");
	if(env && ((strcmp(env,"yes") == 0) ||  (strcmp(env,"Yes") == 0)))
	{
        	env = getenv("bootargs_end");
        	if(!env)
                setenv("bootargs_end",":::dove:eth0:none");
	}
	else
	{
        	env = getenv("bootargs_end");
        	if(!env)
                setenv("bootargs_end",MV_BOOTARGS_END);
	}
	
        env = getenv("image_name");
        if(!env)
                setenv("image_name","uImage");
 

#if (CONFIG_BOOTDELAY >= 0)

        env = getenv("bootcmd");
        if(!env)

#if defined(CONFIG_SYS_HUSH_PARSER)
            setenv("bootcmd","echo Scanning for boot devices;\
usb start;\
ide reset;\
for i in usb ide;\
do for j in 0 1; do for l in / /boot/;\
do for m in fat ext2;\
do setenv interface $i;\
setenv device $j;\
setenv prefix $l;\
setenv fs $m;\
setenv partition 1;\
echo Scanning ${fs} ${interface} ${device} on prefix ${prefix} ...;\
if ${fs}load ${interface} ${device} 1000 ${prefix}boot.scr; then;\
if imi 1000; then echo boot.scr found! Executing ...;\
autoscr 1000;\
fi;\
fi;\
done;\
done;\
done;\
done;\
echo No boot device found. Falling back to TFTP boot;\
run tftpbootcmd;");

#else
            setenv("bootcmd","echo Hush not available, failling back to TFTP boot;\
run tftpbootcmd;");
#endif

        env = getenv("tftpbootcmd");
        if(!env)
            setenv("tftpbootcmd","tftpboot 0x2000000 ${image_name};\
 setenv bootargs ${console} ${mtdparts} ${bootargs_root} nfsroot=${serverip}:${rootpath} \
 ip=${ipaddr}:${serverip}${bootargs_end} usb0Mode=${usb0Mode} usb1Mode=${usb1Mode} \
 video=dovefb:lcd0:${lcd0_params},lcd1:${lcd1_params} clcd.lcd0_enable=${lcd0_enable} clcd.lcd1_enable=${lcd1_enable}; \
 bootm 0x2000000; ");

#endif /* (CONFIG_BOOTDELAY >= 0) */

        env = getenv("standalone");
        if(!env)
            setenv("standalone","fsload 0x2000000 ${image_name};setenv bootargs ${console} ${mtdparts} root=/dev/mtdblock0 rw \
ip=${ipaddr}:${serverip}${bootargs_end} usb0Mode=${usb0Mode} usb1Mode=${usb1Mode} \
video=dovefb:lcd0:${lcd0_params},lcd1:${lcd1_params} clcd.lcd0_enable=${lcd0_enable} clcd.lcd1_enable=${lcd1_enable}; \
bootm 0x2000000;");

#if defined(RD_88F6781_AVNG) || defined(RD_88F6781Y0_AVNG) || defined (RD_88F6781X0_AVNG) || defined (RD_88AP510A0_AVNG)
        env = getenv("bootsd");
        if(!env)
            setenv("bootsd","sflash read a0000 0x2000000 0x300000;setenv bootargs ${console} ${mtdparts} rootdelay=5 \
root=/dev/mmcblk0 rw usb0Mode=${usb0Mode} usb1Mode=${usb1Mode} \
video=dovefb:lcd0:${lcd0_params},lcd1:${lcd1_params} clcd.lcd0_enable=${lcd0_enable} clcd.lcd1_enable=${lcd1_enable}; \
bootm 0x2000000;");

        env = getenv("bootusb");
        if(!env)
            setenv("bootusb","sflash read a0000 0x2000000 0x300000;setenv bootargs ${console} ${mtdparts} rootdelay=5 \
root=/dev/sda rw usb0Mode=${usb0Mode} usb1Mode=${usb1Mode} \
video=dovefb:lcd0:${lcd0_params},lcd1:${lcd1_params} clcd.lcd0_enable=${lcd0_enable} clcd.lcd1_enable=${lcd1_enable}; \
bootm 0x2000000;");

        env = getenv("bootnand");
        if(!env)
            setenv("bootnand","sflash read a0000 0x2000000 0x300000;setenv bootargs ${console} ${mtdparts} ubi.mtd=0 \
root=ubi0:rootfs rootfstype=ubifs rw usb0Mode=${usb0Mode} usb1Mode=${usb1Mode} \
video=dovefb:lcd0:${lcd0_params},lcd1:${lcd1_params} clcd.lcd0_enable=${lcd0_enable} clcd.lcd1_enable=${lcd1_enable}; \
bootm 0x2000000;");
#endif
                 
       /* Set boodelay to 3 sec, if Monitor extension are disabled */
        if(!enaMonExt()){
                setenv("bootdelay","3");
		/* setenv("disaMvPnp","no"); */ /* allow disaMvPnp even not in monitor extension mode */
	}

	/* Disable PNP config of Marvel memory controller devices. */
        env = getenv("disaMvPnp");
        if(!env)
                setenv("disaMvPnp","no");

#if defined(MV_INCLUDE_GIG_ETH)
	/* Generate random ip and mac address */
	/* Read DRAM FTDLL register to create random data for enc */
	unsigned int xi, xj, xk, xl;
	char ethaddr_0[30];
	char ethaddr_1[30];

	/* Read RTC to create pseudo-random data for enc */
	struct rtc_time tm;

	rtc_get(&tm);
	xi = ((tm.tm_yday + tm.tm_sec)% 254);
	/* No valid ip with one of the fileds has the value 0 */
	if (xi == 0)
		xi+=2;

	xj = ((tm.tm_yday + tm.tm_min)%254);
	/* No valid ip with one of the fileds has the value 0 */
	if (xj == 0)
		xj+=2;

	/* Check if the ip address is the same as the server ip */
	if ((xj == 1) && (xi == 11))
		xi+=2;

	xk = (tm.tm_min * tm.tm_sec)%254;
	xl = (tm.tm_hour * tm.tm_sec)%254;

	sprintf(ethaddr_0,"00:50:43:%02x:%02x:%02x",xk,xi,xj);
	sprintf(ethaddr_1,"00:50:43:%02x:%02x:%02x",xl,xi,xj);

	/* MAC addresses */
        env = getenv("ethaddr");
        if(!env)
                setenv("ethaddr",ethaddr_0);

#endif /*  (MV_INCLUDE_GIG_ETH) */

#if defined(MV_INCLUDE_USB)
	/* USB Host */
	env = getenv("usb0Mode");
	if(!env)
		setenv("usb0Mode",ENV_USB0_MODE);

	env = getenv("usb1Mode");
	if(!env)
		setenv("usb1Mode",ENV_USB1_MODE);

#endif  /* (MV_INCLUDE_USB) */

#if defined(YUK_ETHADDR)
	env = getenv("yuk_ethaddr");
	if(!env)
		setenv("yuk_ethaddr",YUK_ETHADDR);

	{
		int i;
		char *tmp = getenv ("yuk_ethaddr");
		char *end;

		for (i=0; i<6; i++) {
			yuk_enetaddr[i] = tmp ? simple_strtoul(tmp, &end, 16) : 0;
			if (tmp)
				tmp = (*end) ? end+1 : end;
		}
	}
#endif /* defined(YUK_ETHADDR) */

#if 0
#if defined(CONFIG_CMD_RCVR)
	env = getenv("netretry");
	if (!env)
		setenv("netretry","no");

	env = getenv("rcvrip");
	if (!env)
		setenv("rcvrip",RCVR_IP_ADDR);

	env = getenv("loadaddr");
	if (!env)
		setenv("loadaddr",RCVR_LOAD_ADDR);

	env = getenv("autoload");
	if (!env)
		setenv("autoload","no");

	/* Check the recovery trigger */
	recoveryDetection();
#endif
#endif //0
	/* LCD support */
	env = getenv("lcd0_enable");
	if(!env)
#if defined(RD_88F6781_AVNG) || defined(RD_88F6781Y0_AVNG)
		setenv("lcd0_enable","1");
#else
		setenv("lcd0_enable","0");
#endif

	env = getenv("lcd1_enable");
	if(!env)
		setenv("lcd1_enable","1");

	env = getenv("lcd0_params");
	if(!env)
#if defined(RD_88F6781_AVNG) || defined(RD_88F6781Y0_AVNG)
		setenv("lcd0_params","1024x600-16@60");
#else
		setenv("lcd0_params","1280x768-24@60");
#endif
	env = getenv("lcd1_params");
	if(!env)
		setenv("lcd1_params","640x480-16@60");

        return;
}

#ifdef BOARD_LATE_INIT
int board_late_init (void)
{
	/* Check if to use the LED's for debug or to use single led for init and Linux heartbeat */
	mvBoardDebugLed(0);
	return 0;
}
#endif

#ifdef MV_BOOTROM
void mv_bootrom_status_print(void)
{
	volatile unsigned int reg;
	unsigned int value = 0;
	int print_err_flag = 1;
	ushort sector_count;

	printf("BootROM:\n");
	/* Read the BootROM version from 0xFFFF003C */
	value = *(unsigned int *)(0xFFFF003C);
	printf("       Version on chip: %X.%02X\n", ((value & 0xF00) >> 8), (value & 0xFF));

	/* Read the BootROM version from the last page of the SPI at offset 0x3C */
/*	sector_count = flash_info[BOOT_FLASH_INDEX].sector_count;
	value = *(unsigned int *)(flash_info[BOOT_FLASH_INDEX].start[sector_count - 1] + 0x3C);
	if ( ((value & 0xFFFFFF00) == 0x00000100) || ((value & 0xFFFFFF00) == 0x00000200) )
		printf("       Version on SPI: %X.%02X\n", ((value & 0xF00) >> 8), (value & 0xFF));
*/
	reg = MV_REG_READ(BOOTROM_ROUTINE_ERR_REG);

	printf("       Status: ");
	/* printf("REG = 0x%X\n", reg); */
	switch(reg & BRE_RTN_NUM_MASK) {
		case 0:
			print_err_flag = 0;
			break;
		case BRE_RTN_UART_BOOT:
			printf("Error in UART Boot: ");
			break;
		case BRE_RTN_SATA_BOOT:
			printf("Error in SATA Boot: ");
			break;
		case BRE_RTN_SPI_BOOT:
			printf("Error in SPI Boot: ");
			break;
		case BRE_RTN_NAND_BOOT:
			printf("Error in NAND Boot: ");
			break;
		case BRE_RTN_EXCEP:
			printf("Error in EXECP Boot: ");
			break;
		case BRE_RTN_EXEC:
			printf("Error in EXEC Boot: ");
			break;
		case BRE_RTN_I2C:
			printf("Error in I2C Boot: ");
			break;
		case BRE_RTN_PEX_BOOT:
			printf("Error in PEX Boot: ");
			break;
		case BRE_RTN_PM_RESUME:
			printf("Error in PM Resume Boot: ");
			break;
	}

	if (print_err_flag) {
		switch (reg & BRE_ERR_CODE_MASK) {
			case ERROR_INVALID_HEADER_ID:
				printf("Invalid header ID\n");
				break;
			case ERROR_INVALID_HDR_CHKSUM:
				printf("Invalid header checksum\n");
				break;
			case ERROR_INVALID_IMG_CHKSUM:
				printf("Invalid image checksum\n");
				break;
			case ERROR_UNSUPPORTED_EXT_HDR:
				printf("Unsupported extended header\n");
				break;
			case ERROR_INVALID_EXT_HDR_CHKSUM:
				printf("Invalid extended header checksum\n");
				break;
			case ERROR_BUSY:
				printf("Busy\n");
				break;
			case ERROR_LINK:
				printf("Link\n");
				break;
			case ERROR_DMA:
				printf("DMA\n");
				break;
			case ERROR_PIO:
				printf("PIO\n");
				break;
			case ERROR_TIMEOUT:
				printf("Timeout\n");
				break;
			case ERROR_UNSUPPORTED_BOOT:
				printf("Unsupported boot\n");
				break;
			case ERROR_ALIGN_SIZE:
				printf("Size not aligned\n");
				break;
			case ERROR_ALIGN_SRC:
				printf("Source not aligned\n");
				break;
			case ERROR_ALIGN_DEST:
				printf("Destination not aligned\n");
				break;
			case ERROR_INVALID_BOOTROM_CHKSUM:
				printf("Invalid BootROM checksum\n");
				break;
			case ERROR_NAND_READ_FAIL:
				printf("NAND read failed\n");
				break;
			case ERROR_NAND_NOT_READY:
				printf("NAND not ready\n");
				break;
			case ERROR_NAND_IMAGE_NOT_ALIGN_256:
				printf("NAND image not aligned\n");
				break;
			case ERROR_NAND_ECC_ERROR:
				printf("NAND ECC error\n");
				break;
			case ERROR_NAND_TIMEOUT:
				printf("NAND timeout\n");
				break;
			case ERROR_NAND_INVALID_OPTION:
				printf("NAND invalid option\n");
				break;
			case ERROR_NAND_EXCESS_BAD_BLKS:
				printf("NAND excess bad blocks\n");
				break;
			case ERROR_NAND_BAD_BLK:
				printf("NAND bad block\n");
				break;
			case ERROR_I2C_READ_ERROR:
				printf("I2C read error\n");
				break;
			case ERROR_I2C_WRITE_ERROR:
				printf("I2C write error\n");
				break;
			case ERROR_I2C_START_ERROR:
				printf("I2C start error\n");
				break;
			case ERROR_I2C_STOP_ERROR:
				printf("I2C stop error\n");
				break;
			case ERROR_I2C_NO_HDR_FOUND_ERROR:
				printf("I2C no header found\n");
				break;
		}
	}
	else
		printf("OK\n");

	/*
	if (reg & BRE_DRAM_INIT_MASK)
		printf("DRAM initialized\n");
	else
		printf("DRAM not initialized\n");
	*/
	if (reg & BRE_REDUNDANT_IMG_MASK)
		printf("Redundant image was used\n");

	printf("       Retries #: %d\n", ((reg & BRE_RTN_RETRY_MASK) >> BRE_RTN_RETRY_OFFS));

}
#endif /* MV_BOOTROM */

int print_cpuinfo (void)
{
	char name[50];

	mv_bootrom_status_print();

	mvBoardNameGet(name);
	printf("Board: %s\n",  name);
	mvCtrlModelRevNameGet(name);
	printf("SoC:   %s\n",  name);
	mvCpuNameGet(name);
	printf("CPU:   %s\n",  name);
	printf("       CPU @ %dMhz, L2 @ %dMhz\n",  mvCpuPclkGet()/1000000, mvCpuL2ClkGet()/1000000);

	if (mvBoardDdrTypeGet() == 0)
		printf("       DDR2 @ %dMhz, TClock @ %dMhz\n", CONFIG_SYS_BUS_CLK/1000000, mvTclkGet()/1000000);
	else
		printf("       DDR3 @ %dMhz, TClock @ %dMhz\n", CONFIG_SYS_BUS_CLK/1000000, mvTclkGet()/1000000);

	return 0;
}

int board_eth_init(bd_t *bis)
{
#ifdef  CONFIG_MARVELL
#if defined(MV_INCLUDE_GIG_ETH)
	/* move to the begining so in case we have a PCI NIC it will
	read the env mac addresses correctlly. */
	mv_eth_initialize(bis);
#endif
#endif
#if defined(CONFIG_SK98)
	skge_initialize(bis);
#endif
#if defined(CONFIG_E1000)
	e1000_initialize(bis);
#endif
	return 0;
}

int board_mmc_init(bd_t *bd)
{
	mv_sdhci_mmc_init(CONFIG_SYS_MV_SDHCI0_ADDR);
	mv_sdhci_mmc_init(CONFIG_SYS_MV_SDHCI1_ADDR);
	return 0;
}

int misc_init_r (void)
{
	char *env;
	
	mvBoardDebugLed(5);

	/* init special env variables */
	misc_init_r_env();

	mv_cpu_init();

#if defined(MV_INCLUDE_MONT_EXT)
	if(enaMonExt()){
		printf("Marvell monitor extension:\n");
		mon_extension_after_relloc();
	}
#endif /* MV_INCLUDE_MONT_EXT */

	/* print detected modules */
	mvMppModuleTypePrint();

	/* init the units decode windows */
	misc_init_r_dec_win();

#ifdef CONFIG_PCI
	//pci_init();
#endif

#if 0
#ifdef MV_BOOTROM
	mv_bootrom_status_print();
#endif /* MV_BOOTROM */
#endif
	mvBoardDebugLed(6);

	mvBoardDebugLed(7);
	
	return 0;
}

MV_U32 mvTclkGet(void)
{
        DECLARE_GLOBAL_DATA_PTR;
        /* get it only on first time */
        if(gd->tclk == 0)
                gd->tclk = mvBoardTclkGet();

        return gd->tclk;
}

MV_U32 mvSysClkGet(void)
{
        DECLARE_GLOBAL_DATA_PTR;
        /* get it only on first time */
        if(gd->bus_clk == 0)
                gd->bus_clk = mvBoardSysClkGet();

        return gd->bus_clk;
}

/* exported for EEMBC */
MV_U32 mvGetRtcSec(void)
{
        MV_RTC_TIME time;
#ifdef MV_INCLUDE_RTC
        mvRtcTimeGet(&time);
#elif CONFIG_RTC_DS1338_DS1339
        mvRtcDS133xTimeGet(&time);
#endif
	return (time.minutes * 60) + time.seconds;
}

void reset_cpu(ulong addr)
{
	mvBoardReset();
}

void mv_cpu_init(void)
{
	char *env;
	volatile unsigned int temp, reg;

	/* L2 extra features */
    	env = getenv("enaL2ExtraFeatures");
    	if(!env || ( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) ) )
	{
		setenv("enaL2ExtraFeatures","yes");
		/* CPU streaming & write allocate */
		__asm__ volatile ("mrc    p15, 1, %0, c15, c1, 0" : "=r" (temp));

#if 0
		/* Write Allocate is a page attribute, not controlled by a global bit. No need to control it here */
		env = getenv("enaWrAllo");
		if(env && ((strcmp(env,"yes") == 0) ||  (strcmp(env,"Yes") == 0)))  
		{
			temp |= BIT28;
		}
		else
		{
			temp &= ~BIT28;
		}
#endif

		env = getenv("enaCpuStream");
		if(!env || (strcmp(env,"no") == 0) ||  (strcmp(env,"No") == 0) )
		{
			setenv("enaCpuStream","no");
			temp &= ~BIT29;
		}
		else
		{
			setenv("enaCpuStream","yes");
			temp |= BIT29;
		}

#if 0
		/* always disable L2 Prefetch ! */
		env = getenv("disL2Prefetch");
		if(!env || (strcmp(env,"no") == 0) ||  (strcmp(env,"No") == 0) )
		{
			temp &= ~BIT24;
		}
		else
		{
			temp |= BIT24;
		}
#endif
		temp |= BIT24; /* disable L2 Prefetch */

		env = getenv("disL2Ecc");
		if(!env || (strcmp(env,"no") == 0) ||  (strcmp(env,"No") == 0) )
		{
			temp |= BIT23;
		}
		else
		{
			temp &= ~BIT23;
		}

		__asm__ volatile ("mcr    p15, 1, %0, c15, c1, 0" :: "r" (temp));
	}
	else 
	{
		setenv("enaL2ExtraFeatures","no");
	}

	/* Verify write allocate and streaming */
	printf("\n");
	__asm__ volatile ("mrc    p15, 1, %0, c15, c1, 0" : "=r" (temp));
	if (temp & BIT29)
		printf("Streaming enabled \n");
	else
		printf("Streaming disabled \n");

#if 0
	/* Write Allocate is a page attribute, not controlled by a global bit */
	if (temp & BIT28)
		printf("Write allocate enabled\n");
	else
		printf("Write allocate disabled\n");
#endif


	asm ("mrc p15, 0, %0, c1, c0, 0":"=r" (reg));

	/* enable L2C - Set bit 26 */
    	env = getenv("disL2Cache");
    	if(!env || ( (strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) ) ) 
	{
		reg |= BIT26;
		/* MV_REG_BIT_SET(CPU_CONTROL_REG, BIT24); TODO, not clear if needed */

		if (temp & BIT24)
			printf("L2 Cache Prefetch disabled\n");
		else
			printf("L2 Cache Prefetch enabled\n");
		if (temp & BIT23)
			printf("L2 Cache ECC enabled\n");
		else
			printf("L2 Cache ECC disabled\n");
	}
	else
	{
		reg &= ~BIT26;
		/* MV_REG_BIT_CLEAR(CPU_CONTROL_REG, BIT24); TODO, not clear if needed */
	}

#if 0 
	/* TODO */
	/* Set L2C WT mode - Set bit 25. Note we use temp here so don't move this code */
	/* before printing the L2 Prefetch and ECC above. */
	temp = MV_REG_READ(CPU_CONTROL_REG);
    	env = getenv("setL2CacheWT");
    	if(!env || ( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) ) )
	{
		temp |= BIT25;
		printf("L2 Cache in Write Through mode\n");
	}
	else {
		temp &= ~BIT25;
		printf("L2 Cache in Write Back mode\n");
	}
	MV_REG_WRITE(CPU_CONTROL_REG, temp);
#endif

	/* Enable i cache */
	reg |= BIT12;
	asm ("mcr p15, 0, %0, c1, c0, 0": :"r" (reg));

	/* Change reset vector to address 0x0 */
	asm ("mrc p15, 0, %0, c1, c0, 0":"=r" (reg));
	reg &= ~BIT13;
	asm ("mcr p15, 0, %0, c1, c0, 0": :"r" (reg));
}
/*******************************************************************************
* mvMppModuleTypePrint - print module detect
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*
*******************************************************************************/
MV_VOID mvMppModuleTypePrint(MV_VOID)
{

	MV_BOARD_MPP_GROUP_CLASS devClass;
	MV_BOARD_MPP_TYPE_CLASS mppGroupType;
	MV_U32 devId;
	MV_U32 maxMppGrp = 1;
	
	devId = mvCtrlModelGet();

	switch(devId){
		case MV_6781_DEV_ID:
			maxMppGrp = MV_6781_MPP_MAX_MODULE;
			break;
	}

	for (devClass = 0; devClass < maxMppGrp; devClass++)
	{
		mppGroupType = mvBoardMppGroupTypeGet(devClass);

		switch(mppGroupType)
		{
			case MV_BOARD_TDM:
				printf("Module %d is TDM\n", devClass);
				break;
			case MV_BOARD_AUDIO:
				printf("Module %d is AUDIO\n", devClass);
				break;
			case MV_BOARD_RGMII:
				printf("Module %d is RGMII\n", devClass);
				break;
			case MV_BOARD_GMII:
				printf("Module %d is GMII\n", devClass);
				break;
			case MV_BOARD_TS:
				printf("Module %d is TS\n", devClass);
				break;
			default:
				break;
		}
	}
}

/* Set unit in power off mode acording to the detection of MPP */
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)

/*******************************************************************************
* mvPmuVpuPowerDown - Power Down/Up the Video Unit
*
* DESCRIPTION:
*       Power down/up the Video Processing unit
*
* INPUT:
*	pwrStat: power down status
* OUTPUT:
*       None
* RETURN:
*    	MV_OK      : Calibration started and ended successfully
*******************************************************************************/
MV_STATUS mvPmuVpuPowerDown(MV_BOOL pwrStat)
{
	MV_U32 reg;

	if (pwrStat)
	{
		/* Isolate unit */
		reg = MV_REG_READ(PMU_ISO_CTRL_REG);
		reg &= ~PMU_ISO_VIDEO_MASK;
		MV_REG_WRITE(PMU_ISO_CTRL_REG, reg);

		/* Reset unit */
		reg = MV_REG_READ(PMU_SW_RST_CTRL_REG);
		reg &= ~PMU_SW_RST_VIDEO_MASK;
		MV_REG_WRITE(PMU_SW_RST_CTRL_REG, reg);

		/* Power down the unit */
		reg = MV_REG_READ(PMU_PWR_SUPLY_CTRL_REG);
		reg |= PMU_PWR_VPU_PWR_DWN_MASK;
		MV_REG_WRITE(PMU_PWR_SUPLY_CTRL_REG, reg);
	}
	else
	{
		/* Power up the unit */
		reg = MV_REG_READ(PMU_PWR_SUPLY_CTRL_REG);
		reg &= ~PMU_PWR_VPU_PWR_DWN_MASK;
		MV_REG_WRITE(PMU_PWR_SUPLY_CTRL_REG, reg);
	
		/* Add delay to wait for Power Up */

		/* Deassert unit Reset */
		reg = MV_REG_READ(PMU_SW_RST_CTRL_REG);
		reg |= PMU_SW_RST_VIDEO_MASK;
		MV_REG_WRITE(PMU_SW_RST_CTRL_REG, reg);

		/* Deassert unit Isolation*/
		reg = MV_REG_READ(PMU_ISO_CTRL_REG);
		reg |= PMU_ISO_VIDEO_MASK;
		MV_REG_WRITE(PMU_ISO_CTRL_REG, reg);
	}

	return MV_OK;
}

/*******************************************************************************
* mvPmuGpuPowerDown - Power Down/Up the Graphical Unit
*
* DESCRIPTION:
*       Power down/up the graphical unit
*
* INPUT:
*	pwrStat: power down status
* OUTPUT:
*       None
* RETURN:
*    	MV_OK      : Calibration started and ended successfully
*******************************************************************************/
MV_STATUS mvPmuGpuPowerDown(MV_BOOL pwrStat)
{
	MV_U32 reg;

	if (pwrStat)
	{
		/* Isolate unit */
		reg = MV_REG_READ(PMU_ISO_CTRL_REG);
		reg &= ~PMU_ISO_GPU_MASK;
		MV_REG_WRITE(PMU_ISO_CTRL_REG, reg);

		/* Reset unit */
		reg = MV_REG_READ(PMU_SW_RST_CTRL_REG);
		reg &= ~PMU_SW_RST_GPU_MASK;
		MV_REG_WRITE(PMU_SW_RST_CTRL_REG, reg);

		/* Power down the unit */
		reg = MV_REG_READ(PMU_PWR_SUPLY_CTRL_REG);
		reg |= PMU_PWR_GPU_PWR_DWN_MASK;
		MV_REG_WRITE(PMU_PWR_SUPLY_CTRL_REG, reg);
	}
	else
	{
		/* Power up the unit */
		reg = MV_REG_READ(PMU_PWR_SUPLY_CTRL_REG);
		reg &= ~PMU_PWR_GPU_PWR_DWN_MASK;
		MV_REG_WRITE(PMU_PWR_SUPLY_CTRL_REG, reg);
	
		/* Add delay to wait for Power Up */

		/* Deassert unit Reset */
		reg = MV_REG_READ(PMU_SW_RST_CTRL_REG);
		reg |= PMU_SW_RST_GPU_MASK;
		MV_REG_WRITE(PMU_SW_RST_CTRL_REG, reg);

		/* Deassert unit Isolation*/
		reg = MV_REG_READ(PMU_ISO_CTRL_REG);
		reg |= PMU_ISO_GPU_MASK;
		MV_REG_WRITE(PMU_ISO_CTRL_REG, reg);
	}
	
	return MV_OK;
}

int mv_set_power_scheme(void)
{
	char *env;
#if 0
	/* TODO */
	int mppGroupType1 = mvBoardMppGroupTypeGet(MV_BOARD_MPP_GROUP_1);
	int mppGroupType2 = mvBoardMppGroupTypeGet(MV_BOARD_MPP_GROUP_2);

	/* Close AUDIO */
	if ((mppGroupType1 != MV_BOARD_AUDIO) && (mppGroupType2 != MV_BOARD_AUDIO))
	{
		DB(printf("Warning: AUDIO is Powered Off\n"));
		mvCtrlPwrClckSet(AUDIO_UNIT_ID, 0, MV_FALSE);
	}
#endif
	/* VPU power */
        env = getenv("enaVpuPower");
        if(!env || ( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) ) )
	{
                setenv("enaVpuPower","yes");
		printf("Setting VPU power ON.\n");
		mvPmuVpuPowerDown(MV_FALSE);
	}
        else
	{
                setenv("enaVpuPower","no");
		printf("Setting VPU power OFF.\n");
		mvPmuVpuPowerDown(MV_TRUE);
	}

	/* GPU power */
        env = getenv("enaGpuPower");
        if(!env || ( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) ) )
	{
                setenv("enaGpuPower","yes");
		printf("Setting GPU power ON.\n");
		mvPmuGpuPowerDown(MV_FALSE);
	}
        else
	{
                setenv("enaGpuPower","no");
		printf("Setting GPU power OFF.\n");
		mvPmuGpuPowerDown(MV_TRUE);
	}
	/* Removing DVS startup calibration
	printf("Calibrating Voltage:\n");
	mvDvsToTarget("cpu", 1100);
	mvDvsToTarget("core", 1000);
	*/
	return MV_OK;
}

#endif /* defined(MV_INCLUDE_CLK_PWR_CNTRL) */

/*******************************************************************************
* mvUpdateNorFlashBaseAddrBank - 
*
* DESCRIPTION:
*       This function update the CFI driver base address bank with on board NOR
*       devices base address.
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
*       None
*
*******************************************************************************/
#ifdef	CONFIG_FLASH_CFI_DRIVER
MV_VOID mvUpdateNorFlashBaseAddrBank(MV_VOID)
{
	MV_U32 devBaseAddr;
	MV_U32 devNum = 0;
	int i;

	/* Update NOR flash base address bank for CFI flash init driver */
	for (i = 0 ; i < CONFIG_SYS_MAX_FLASH_BANKS_DETECT; i++)
	{
		devBaseAddr = mvBoardGetDeviceBaseAddr(i,BOARD_DEV_NOR_FLASH);
		if (devBaseAddr != 0xFFFFFFFF)
		{
			flash_add_base_addr (devNum, devBaseAddr);
			devNum++;
		}
	}
	mv_board_num_flash_banks = devNum;

	/* Update SPI flash count for CFI flash init driver */
	/* Assumption only 1 SPI flash on board */
	for (i = 0 ; i < CONFIG_SYS_MAX_FLASH_BANKS_DETECT; i++)
	{
		devBaseAddr = mvBoardGetDeviceBaseAddr(i,BOARD_DEV_SPI_FLASH);
		if (devBaseAddr != 0xFFFFFFFF)
		mv_board_num_flash_banks += 1;
	}
}
#endif	/* CONFIG_FLASH_CFI_DRIVER */

#ifdef RD_88F6781_AVNG

#define MPP_DETECT_TIME	(1 * 10)	/* unit in 100 milliseconds */
#define MPP_WAIT_TIME	(100 * 1000)	/* 100 milliseconds */
#define MPP_HOLD_KEY	9

int detect_hold_key(void)
{
	unsigned long reg;
	int i, pin = MPP_HOLD_KEY;

	reg = MV_REG_READ(GPP_DATA_IN_REG(0));
	reg &= (1 << pin);

	if (reg) {
		/* Wait for hold key detection */
		for (i = 0; i < MPP_DETECT_TIME; i++)
		{
			reg = MV_REG_READ(GPP_DATA_IN_REG(0));
			reg &= (1 << pin);

			if (! reg)
				break;

			udelay (MPP_WAIT_TIME);
		}

		if (i == MPP_DETECT_TIME)
			return 1;
	}

	return 0;
}
#endif
