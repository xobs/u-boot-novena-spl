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

#include <config.h>
#include <common.h>
#include <command.h>
#include <pci.h>
#include <net.h>

#include "mvCommon.h"
#include "mvCtrlEnvLib.h"
#include "mvCpuIfRegs.h"
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
#include "pmu/mvPmu.h"
#endif

#include "cntmr/mvCntmrRegs.h"

#if defined(CONFIG_CMD_NAND)
#include <nand.h>

/* references to names in cmd_nand.c */
//#define NANDRW_READ		0x01
//#define NANDRW_WRITE	0x00
//#define NANDRW_JFFS2	0x02
//extern struct nand_chip nand_dev_desc[];
extern nand_info_t nand_info[];       /* info for NAND chips */
#endif

#ifndef CONFIG_SYS_NO_FLASH
extern flash_info_t flash_info[];       /* info for FLASH chips */
#endif

/* int nand_rw (struct nand_chip* nand, int cmd,
	    size_t start, size_t len,
	    size_t * retlen, u_char * buf);
 int nand_erase(struct nand_chip* nand, size_t ofs,
				size_t len, int clean);
*/
//extern int nand_erase_opts(nand_info_t *meminfo, const nand_erase_options_t *opts);
//extern int nand_write_opts(nand_info_t *meminfo, const nand_write_options_t *opts);


#ifdef CONFIG_CMD_SF
#include <spi_flash.h>

extern struct spi_flash *flash;
#endif

#if defined(CONFIG_CMD_NAND) || defined(CONFIG_CMD_SF)
#if 0
#if !defined(MV_NAND_BOOT) || !defined(MV_SPI_BOOT)
static unsigned int flash_in_which_sec(flash_info_t *fl,unsigned int offset)
{
	unsigned int sec_num;
	if(NULL == fl)
		return 0xFFFFFFFF;

	for( sec_num = 0; sec_num < fl->sector_count ; sec_num++){
		/* If last sector*/
		if (sec_num == fl->sector_count -1)
		{
			if((offset >= fl->start[sec_num]) && 
			   (offset <= (fl->size + fl->start[0] - 1)) )
			{
				return sec_num;
			}

		}
		else
		{
			if((offset >= fl->start[sec_num]) && 
			   (offset < fl->start[sec_num + 1]) )
			{
				return sec_num;
			}

		}
	}
	/* return illegal sector Number */
	return 0xFFFFFFFF;

}

#endif /* !defined(CONFIG_NAND_BOOT) */
#endif

/*******************************************************************************
burn a u-boot.bin on the Boot Flash
********************************************************************************/

#if 0
#include "bootstrap_def.h"
#if defined(CONFIG_CMD_NET)
/* 
 * 8 bit checksum 
 */
MV_U8 checksum8(MV_U32 start, MV_U32 len, MV_U8 csum)
{
	register MV_U8 sum = csum;
	volatile MV_U8* startp = (volatile MV_U8*)start;

	do {
		sum += *startp;
		startp++;
	} while(--len);

	return (sum);
} /* end of checksum8 */

#if 0
/*
 * Check the extended header and execute the image
 */
static MV_U32 verify_extheader(ExtBHR_t *extBHR)
{
	MV_U8	chksum;


	/* Caclulate abd check the checksum to valid */
	chksum = checksum8((MV_U32)extBHR , EXT_HEADER_SIZE -1, 0);
	if (chksum != (*(MV_U8*)((MV_U32)extBHR + EXT_HEADER_SIZE - 1)))
	{
		printf("Error! invalid extende header checksum\n");
		return MV_FAIL;
	}
	
    return MV_OK;
}
#endif
/*
 * Check the CSUM8 on the main header
 */
static MV_U32 verify_main_header(BHR_t *pBHR, MV_U8 headerID)
{
	MV_U8	chksum,chksumtemp;

	/* Verify Checksum */
	chksumtemp = pBHR->checkSum;
	pBHR->checkSum = 0;
	chksum = checksum8((MV_U32)pBHR, MAIN_HDR_GET_LEN(pBHR), 0);
	pBHR->checkSum = chksumtemp;
	if (chksum != chksumtemp)
	{
		printf("\t[Fail] invalid image header checksum\n");
		return MV_FAIL;
	}

	/* Verify Header */
	if (pBHR->blockID != headerID)
	{
		printf("\t[Fail] invalid image header ID\n");
		return MV_FAIL;
	}
	
	/* Verify Alignment */
	if (pBHR->blockSize & 0x3)
	{
		printf("\t[Fail] invalid image header alignment\n");
		return MV_FAIL;
	}

	if ((cpu_to_le32(pBHR->destinationAddr) & 0x3) && (cpu_to_le32(pBHR->destinationAddr) != 0xffffffff))
	{
		printf("\t[Fail] invalid image header destination\n");
		return MV_FAIL;
	}

	if ((cpu_to_le32(pBHR->sourceAddr) & 0x3) && (pBHR->blockID != IBR_HDR_SATA_ID))
	{
		printf("\t[Fail] invalid image header source\n");
		return MV_FAIL;
	}

    return MV_OK;
}
#endif //0
#endif //0

#if defined(MV_NAND_BOOT)
/* Boot from NAND flash */
/* Write u-boot image into the nand flash */
int nand_burn_uboot_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int filesize;
	MV_U32 ret = 0;
	extern char console_buffer[];
	nand_info_t *nand = &nand_info[0];
	//u_char *load_addr;
	load_addr = CONFIG_SYS_LOAD_ADDR;
	uint64_t size = CONFIG_ENV_OFFSET;

	if(argc == 2) {
		copy_filename (BootFile, argv[1], sizeof(BootFile));
	}
	else { 
		copy_filename (BootFile, "u-boot.bin", sizeof(BootFile));
		printf("Using default filename \"u-boot.bin\" \n");
	}
	filesize = NetLoop(TFTP);
	printf("Checking file size:");
	if (filesize == -1)
	{
		printf("\t[Fail]\n");
		return 0;
	}
	printf("\t[Done]\n");
#if 0
#ifdef MV_BOOTROM
	printf("Checking header cksum:");
	BHR_t* tmpBHR = (BHR_t*) load_addr;

	/* Verify Main header checksum */
	if (verify_main_header(tmpBHR, IBR_HDR_NAND_ID))
		return 0;

	printf("\t[Done]\n");
#endif
#endif //0
	printf("Override Env parameters to default? [y/N]");
	readline(" ");
	if( strcmp(console_buffer,"Y") == 0 || 
	    strcmp(console_buffer,"yes") == 0 ||
	    strcmp(console_buffer,"y") == 0 ) {

		printf("Erasing 0x%x - 0x%x:",CONFIG_ENV_OFFSET, CONFIG_ENV_RANGE);
		nand_erase(nand, CONFIG_ENV_OFFSET, CONFIG_ENV_RANGE);
		printf("\t[Done]\n");
	}

	printf("Erasing 0x%x - 0x%x: ", 0, 0 + CONFIG_ENV_OFFSET);
	nand_erase(nand, 0, CONFIG_ENV_OFFSET);
	printf("\t[Done]\n");

	printf("Writing image to NAND:");
	ret = nand_write(nand, 0, &size, load_addr);
	if (ret)
		printf("\t[Fail]\n");
	else
		printf("\t[Done]\n");	

	return 1;
}

U_BOOT_CMD(
        bubt,      2,     1,      nand_burn_uboot_cmd,
        "bubt	- Burn an image on the Boot Nand Flash.\n",
        " file-name \n"
        "\tBurn a binary image on the Boot Nand Flash, default file-name is u-boot.bin .\n"
);

/* Write nboot loader image into the nand flash */
int nand_burn_nboot_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	MV_U32 ret = 0;
	extern char console_buffer[];
	nand_info_t *nand = &nand_info[0];
	nand_erase_options_t er_opts;
	unsigned int filesize;
	nand_write_options_t wr_opts;

	char *load_addr = CONFIG_SYS_LOAD_ADDR; 
	if(argc == 2) {
		copy_filename (BootFile, argv[1], sizeof(BootFile));
	}
	else { 
		copy_filename (BootFile, "nboot.bin", sizeof(BootFile));
		printf("using default file \"nboot.bin\" \n");
	}
 
	if ((filesize = NetLoop(TFTP)) < 0)
		return 0;
 
	printf("Erase %d - %d ... ",MV_NBOOT_BASE, MV_NBOOT_LEN);
	nand_erase(nand, MV_NBOOT_BASE, MV_NBOOT_LEN);

	printf("\nCopy to Nand Flash... ");
	ret = nand_write(nand, MV_NBOOT_BASE, MV_NBOOT_LEN, load_addr);
	if (ret)
		printf("Error - NAND burn faild!\n");
	else
		printf("\ndone\n");	

	return 1;
}

U_BOOT_CMD(
        nbubt,      2,     1,      nand_burn_nboot_cmd,
        "nbubt	- Burn a boot loader image on the Boot Nand Flash.\n",
        " file-name \n"
        "\tBurn a binary boot loader image on the Boot Nand Flash, default file-name is nboot.bin .\n"
);
#endif /* defined(CONFIG_NAND_BOOT) */

#if defined(MV_SPI_BOOT)
/* Boot from SPI flash */
/* Write u-boot image into the SPI flash */
int spi_burn_uboot_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int filesize;
	MV_U32 ret = 0;
	extern char console_buffer[];
	size_t len = CONFIG_SYS_MONITOR_LEN;
	load_addr = CONFIG_SYS_LOAD_ADDR; 

	if(argc == 2) {
		copy_filename (BootFile, argv[1], sizeof(BootFile));
	}
	else { 
		copy_filename (BootFile, "u-boot.bin", sizeof(BootFile));
		printf("Using default filename \"u-boot.bin\" \n");
	}
	filesize = NetLoop(TFTP);
	printf("Checking file size:");
	if (filesize == -1)
	{
		printf("\t\t[ERR!]\n");
		return 0;
	}
	printf("\t\t[Done]\n");
#if 0
#ifdef MV_BOOTROM
	printf("Checking header cksum:");
	BHR_t* tmpBHR = (BHR_t*) load_addr;

	/* Verify Main header checksum */
	if (verify_main_header(tmpBHR, IBR_HDR_SPI_ID))
		return 0;

	printf("\t[Done]\n");
#endif
#endif //0

	printf("Override Env parameters to default? [y/N]");
	readline(" ");
#ifdef CONFIG_SPI_FLASH_PROTECTION
	printf("Unprotecting flash:");
	spi_flash_protect(flash, 0);
	printf("\t\t[Done]\n");
#endif
	if( strcmp(console_buffer,"Y") == 0 ||
	    strcmp(console_buffer,"yes") == 0 ||
	    strcmp(console_buffer,"y") == 0 ) {

		printf("Erasing 0x%x - 0x%x:",CONFIG_ENV_OFFSET, CONFIG_ENV_OFFSET + CONFIG_ENV_SIZE);
		spi_flash_erase(flash, CONFIG_ENV_OFFSET, CONFIG_ENV_SIZE);
		printf("\t[Done]\n");
	}

	printf("Erasing 0x%x - 0x%x: ",0, 0 + CONFIG_ENV_OFFSET);
	spi_flash_erase(flash, 0, CONFIG_ENV_OFFSET);
	printf("\t\t[Done]\n");

	printf("Writing image to flash:");
	ret = spi_flash_write(flash, 0, filesize, load_addr);

	if (ret)
		printf("\t\t[Err!]\n");
	else
		printf("\t\t[Done]\n");

#ifdef CONFIG_SPI_FLASH_PROTECTION
	printf("Protecting flash:");
	spi_flash_protect(flash, 1);
	printf("\t\t[Done]\n");
#endif
	return 1;
}

U_BOOT_CMD(
        bubt,      2,     1,      spi_burn_uboot_cmd,
        "bubt	- Burn an image on the Boot SPI Flash.\n",
        " file-name \n"
        "\tBurn a binary image on the Boot SPI Flash, default file-name is u-boot.bin .\n"
);
#endif

#if 0
#ifdef MV_BOOTROM
/* Upgrade BootROM image */
int spi_burn_bootrom_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int filesize;
	MV_U32 from, to;
	ushort sector_count;
	ulong sector_size;
	extern char console_buffer[];

	if(argc == 2) {
		copy_filename (BootFile, argv[1], sizeof(BootFile));
	}
	else { 
		printf("Must provide image file name as a parameter.\n");
		return 0; 
	}
 
	if ((filesize = NetLoop(TFTP)) < 0)
		return 0;

	sector_count = flash_info[BOOT_FLASH_INDEX].sector_count;
	sector_size = flash_info[BOOT_FLASH_INDEX].size / sector_count;
	from = flash_info[BOOT_FLASH_INDEX].start[sector_count - 1];
	to = from + sector_size;

	printf("Un-Protecting 0x%X to 0x%X\n", from, to);
	flash_protect (FLAG_PROTECT_CLEAR, from, to, &flash_info[BOOT_FLASH_INDEX]);

	printf("Erasing 0x%X to 0x%X\n", from, to);
	flash_erase(&flash_info[BOOT_FLASH_INDEX], (sector_count-1), (sector_count-1));

	printf("Copy to Flash... ");

	flash_write ( (char *)(CFG_LOAD_ADDR + CFG_MONITOR_IMAGE_OFFSET), from, sector_size);

	printf("done\nProtecting 0x%X to 0x%X\n", from, to);
	flash_protect (FLAG_PROTECT_SET, from, to, &flash_info[BOOT_FLASH_INDEX]);

	return 1;
}

U_BOOT_CMD(
        bbrom,      2,     1,      spi_burn_bootrom_cmd,
        "bbrom	- Upgrade the BootROM image on the SPI Flash.\n",
        " file-name \n"
        "\tBurn a binary BootrOM image on the SPI Flash. Must provide image file name as a parameter.\n"
);
#endif
#endif /* (CONFIG_CMD_NET) */


/*******************************************************************************
Reset environment variables.
********************************************************************************/
int resetenv_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
#if defined(CONFIG_ENV_IS_IN_NAND)
	int ret = 0;
	nand_erase_options_t nand_erase_options;
	size_t offset = 0;

	
#if defined(CONFIG_SKIP_BAD_BLOCK)
	int i = 0;
	size_t blocksize;
	blocksize = nand_info[0].erasesize;
	while(i * blocksize < nand_info[0].size) {
		if (!nand_block_isbad(&nand_info[0], offset))
			offset += blocksize;
		if (offset >= CONFIG_ENV_OFFSET)
			break;
		i++;
	}
#else
	offset = CONFIG_ENV_OFFSET;
#endif
	nand_erase_options.length = CONFIG_ENV_RANGE;
	nand_erase_options.quiet = 0;
	nand_erase_options.jffs2 = 0;
	nand_erase_options.scrub = 0;
	nand_erase_options.offset = offset;

	puts ("Erasing Nand:\n");
	if (nand_erase_opts(&nand_info[0], &nand_erase_options))
		return 1;
	puts ("[Done]\n");
#elif defined(CONFIG_ENV_IS_IN_SPI_FLASH)
	u32 sector = 1;

	if (CONFIG_ENV_SIZE > CONFIG_ENV_SECT_SIZE) {
		sector = CONFIG_ENV_SIZE / CONFIG_ENV_SECT_SIZE;
		if (CONFIG_ENV_SIZE % CONFIG_ENV_SECT_SIZE)
			sector++;
	}


#ifdef CONFIG_SPI_FLASH_PROTECTION
	printf("Unprotecting flash:");
	spi_flash_protect(flash, 0);
	printf("\t\t[Done]\n");
#endif

	printf("Erasing 0x%x - 0x%x:",CONFIG_ENV_OFFSET, CONFIG_ENV_OFFSET + sector * CONFIG_ENV_SECT_SIZE);	
	if (spi_flash_erase(flash, CONFIG_ENV_OFFSET, sector * CONFIG_ENV_SECT_SIZE))
		return 1;
	puts("\t[Done]\n");

#ifdef CONFIG_SPI_FLASH_PROTECTION
	printf("Protecting flash:");
	spi_flash_protect(flash, 1);
	printf("\t\t[Done]\n");
#endif

#endif
	printf("Warning: Default Environment Variables will take effect Only after RESET\n");
	return 0;
}

U_BOOT_CMD(
        resetenv,      1,     1,      resetenv_cmd,
        "resetenv	- earse environment sector to reset all variables to default.\n",
        " \n"
        "\t Erase the environemnt variable sector.\n"
);

#endif

#if defined(CONFIG_CMD_BSP)

/******************************************************************************
* Category     - General
* Functionality- The commands allows the user to view the contents of the MV
*                internal registers and modify them.
* Need modifications (Yes/No) - no
*****************************************************************************/
int ir_cmd( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[] )
{
	MV_U32 regNum = 0x0, regVal, regValTmp, res;
	MV_8 regValBin[40];
	MV_8 cmd[40];
	int i,j = 0, flagm = 0;
	extern MV_8 console_buffer[];

	if( argc == 2 ) {
		regNum = simple_strtoul( argv[1], NULL, 16 );
	}
	else { 
		printf( "Usage:\n%s\n", cmdtp->usage );
		return 0;
	}                                                                                                        

	regVal = MV_REG_READ( regNum + INTER_REGS_BASE);
	regValTmp = regVal;
	printf( "Internal register 0x%x value : 0x%x\n ",regNum, regVal );
	printf( "\n    31      24        16         8         0" );
	printf( "\n     |       |         |         |         |\nOLD: " );

	for( i = 31 ; i >= 0 ; i-- ) {
		if( regValTmp > 0 ) {
			res = regValTmp % 2;
			regValTmp = (regValTmp - res) / 2;
			if( res == 0 )
				regValBin[i] = '0';
			else
				regValBin[i] = '1';
		}
		else
			regValBin[i] = '0';
	}

	for( i = 0 ; i < 32 ; i++ ) {
		printf( "%c", regValBin[i] );
		if( (((i+1) % 4) == 0) && (i > 1) && (i < 31) )
			printf( "-" );
	}

	readline( "\nNEW: " );
	strcpy(cmd, console_buffer);
	if( (cmd[0] == '0') && (cmd[1] == 'x') ) {
		regVal = simple_strtoul( cmd, NULL, 16 );
		flagm=1;
	}
	else {
		for( i = 0 ; i < 40 ; i++ ) {
			if(cmd[i] == '\0')
				break;
			if( i == 4 || i == 9 || i == 14 || i == 19 || i == 24 || i == 29 || i == 34 )
				continue;
			if( cmd[i] == '1' ) {
				regVal = regVal | (0x80000000 >> j);
				flagm = 1;
			}
			else if( cmd[i] == '0' ) {
				regVal = regVal & (~(0x80000000 >> j));
				flagm = 1;
			}
			j++;
		}
	}

	if( flagm == 1 ) {
		MV_REG_WRITE( regNum + INTER_REGS_BASE, regVal );
		printf( "\nNew value = 0x%x\n\n", MV_REG_READ(regNum +
					INTER_REGS_BASE) );
	}
	return 1;
}

U_BOOT_CMD(
	ir,      2,     1,      ir_cmd,
	"ir	- reading and changing MV internal register values.\n",
	" address\n"
	"\tDisplays the contents of the internal register in 2 forms, hex and binary.\n"
	"\tIt's possible to change the value by writing a hex value beginning with \n"
	"\t0x or by writing 0 or 1 in the required place. \n"
    	"\tPressing enter without any value keeps the value unchanged.\n"
);

/******************************************************************************
* Category     - General
* Functionality- Display the auto detect values of the TCLK and SYSCLK.
* Need modifications (Yes/No) - no
*****************************************************************************/
int clk_cmd( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    	printf( "TCLK %dMhz, SYSCLK %dMhz (UART baudrate %d)\n",
		mvTclkGet()/1000000, mvSysClkGet()/1000000, CONFIG_BAUDRATE);
	return 1;
}

U_BOOT_CMD(
	dclk,      1,     1,      clk_cmd,
	"dclk	- Display the MV device CLKs.\n",
	" \n"
	"\tDisplay the auto detect values of the TCLK and SYSCLK.\n"
);

/******************************************************************************
* Functional only when using Lauterbach to load image into DRAM
* Category     - DEBUG
* Functionality- Display the array of registers the u-boot write to.
*
*****************************************************************************/
#if defined(REG_DEBUG)
int reg_arry[4096][2];
int reg_arry_index = 0;
int print_registers( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int i;
	printf("Register display\n");

	for (i=0; i < reg_arry_index; i++)
		printf("Reg no %d addr 0x%x = 0x%08x\n", i, reg_arry[i][0], reg_arry[i][1]);

	return 1;
}

U_BOOT_CMD(
	printreg,      1,     1,      print_registers,
	"printreg	- Display the register array the u-boot write to.\n",
	" \n"
	"\tDisplay the register array the u-boot write to.\n"
);
#endif

MV_STATUS g_netStatus=-1; //for manufacture test
#if defined(MV_INCLUDE_GIG_ETH)
/******************************************************************************
* Category     - Etherent
* Functionality- Display PHY ports status (using SMI access).
* Need modifications (Yes/No) - No
*****************************************************************************/
int sg_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	MV_U32 port;
	for( port = 0 ; port < mvCtrlEthMaxPortGet(); port++ ) {

		printf( "PHY %d :\n", port );
		printf( "---------\n" );

		g_netStatus=mvEthPhyPrintStatus( mvBoardPhyAddrGet(port) );

		printf("\n");
	}
	return 1;
}

U_BOOT_CMD(
	sg,      1,     1,      sg_cmd,
	"sg	- scanning the PHYs status\n",
	" \n"
	"\tScan all the Gig port PHYs and display their Duplex, Link, Speed and AN status.\n"
);
#endif /* #if defined(MV_INCLUDE_GIG_ETH) */

#if defined(MV_INCLUDE_PDMA)

/******************************************************************************
* Category     - PDMA
* Functionality- Perform a PDMA transaction
* Need modifications (Yes/No) - No
*****************************************************************************/
int mvPdma_cmd( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[] )
{
	MV_8 cmd[20];
	extern MV_8 console_buffer[];
	MV_U32 src, dst, byteCount;
	MV_PDMA_CHANNEL chan;

	/* get PDMA channel */

	/* source address */
	while(1) {
		readline( "Source Address: " );
		strcpy( cmd, console_buffer );
		src = simple_strtoul( cmd, NULL, 16 );
		if( src == 0xffffffff ) printf( "Bad address !!!\n" );
		else break;
	}

	/* desctination address */
	while(1) {
		readline( "Destination Address: " );
		strcpy(cmd, console_buffer);
		dst = simple_strtoul( cmd, NULL, 16 );
		if( dst == 0xffffffff ) printf("Bad address !!!\n");
		else break;
	}

	/* byte count */
	while(1) {
		readline( "Byte Count (up to 8KByte (0 - 0x1FFF)): " );
		strcpy( cmd, console_buffer );
		byteCount = simple_strtoul( cmd, NULL, 16 );
		if( (byteCount >= 0x2000) || (byteCount == 0) ) printf("Bad value !!!\n");
		else break;
	}

	if (mvPdmaChanAlloc(MV_PDMA_MEMORY, 0, &chan) != MV_OK) {
		printf("Error allocating PDMA channel\n");
		return 0;
	}
	/* wait for previous transfer completion */
	while(mvPdmaChannelStateGet(&chan) == MV_PDMA_CHANNEL_RUNNING);
	/* issue the transfer */
	if (mvPdmaChanTransfer(&chan, MV_PDMA_MEM_TO_MEM, src, dst, byteCount, 0) != MV_OK) {
		printf("Error with PDMA transfer\n"); 
	}
	/* wait for completion */
	while(mvPdmaChannelStateGet(&chan) == MV_PDMA_CHANNEL_RUNNING);
	if (mvPdmaChanFree(&chan) != MV_OK) {
		printf("Error freeing PDMA channel\n"); 
		return 0;
	}	

	printf( "Done...\n" );
	return 1;
}

U_BOOT_CMD(
	pdma,      1,     1,      mvPdma_cmd,
	"pdma	- Perform PDMA\n",
	" \n"
	"\tPerform PDMA memory to memory transaction with the parameters given by the user.\n"
);

#endif /* #if defined(MV_INCLUDE_PDMA) */

#if defined(MV_INCLUDE_XOR)

/******************************************************************************
* Category     - DMA
* Functionality- Perform a DMA transaction using the XOR engine
* Need modifications (Yes/No) - No
*****************************************************************************/
#define XOR_TIMEOUT 0x8000000

struct xor_channel_t
{
   	MV_CRC_DMA_DESC *pDescriptor;
	MV_ULONG	descPhyAddr;
};

#define XOR_CAUSE_DONE_MASK(chan) ((BIT0|BIT1) << (chan * 16) )
void xor_waiton_eng(int chan)
{
    int timeout = 0;
    
    while(!(MV_REG_READ(XOR_CAUSE_REG(XOR_UNIT(chan))) & XOR_CAUSE_DONE_MASK(XOR_CHAN(chan)))) 
    {
	if(timeout > XOR_TIMEOUT)
	    goto timeout; 
	timeout++;
    }

    timeout = 0;
    while(mvXorStateGet(chan) != MV_IDLE)
    {
	if(timeout > XOR_TIMEOUT)
	    goto timeout; 
	timeout++;
    }
    /* Clear int */
    MV_REG_WRITE(XOR_CAUSE_REG(XOR_UNIT(chan)), ~(XOR_CAUSE_DONE_MASK(XOR_CHAN(chan))));

timeout:
    if(timeout > XOR_TIMEOUT)
    {
	printf("ERR: XOR eng got timedout!!\n");
    }
    return;
}

int mvDma_cmd( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[] )
{
	MV_8 cmd[20];
	extern MV_8 console_buffer[];
	MV_U32 chan, src, dst, byteCount;
        struct xor_channel_t channel;
        MV_U8	*pVirt = (MV_U8*)mvOsIoUncachedAlignedMalloc(NULL, 32, sizeof(MV_XOR_DESC),
					    &(channel.descPhyAddr), NULL);

	chan = 0;

	/* source address */
	while(1) {
		readline( "Physical Source Address (must be cache-line aligned): " );
		strcpy( cmd, console_buffer );
		src = simple_strtoul( cmd, NULL, 16 );
		if ((src == 0xffffffff) || (src & 0x1F)) printf( "Bad address !!!\n" );
		else break;
	}

	/* desctination address */
	while(1) {
		readline( "Physical Destination Address (must be cache-line aligned): " );
		strcpy(cmd, console_buffer);
		dst = simple_strtoul( cmd, NULL, 16 );
		if ((dst == 0xffffffff) || (dst & 0x1F)) printf("Bad address !!!\n");
		else break;
	}

	/* byte count */
	while(1) {
		readline( "Byte Count (up to (16M-1), must be a multiple of the cache-line): " );
		strcpy( cmd, console_buffer );
		byteCount = simple_strtoul( cmd, NULL, 16 );
		if( (byteCount > 0xffffff) || (byteCount == 0) ) printf("Bad value !!!\n");
		else break;
	}

	/* wait for previous transfer completion */
	while (mvXorStateGet(chan) != MV_IDLE);

	/* build the channel descriptor */
	channel.pDescriptor = (MV_CRC_DMA_DESC *)pVirt;
        channel.pDescriptor->srcAdd0 = src;
        channel.pDescriptor->srcAdd1 = 0;
	channel.pDescriptor->destAdd = dst;
        channel.pDescriptor->byteCnt = byteCount;
        channel.pDescriptor->nextDescPtr = 0;
        channel.pDescriptor->status = BIT31;
        channel.pDescriptor->descCommand = 0x0;

	/* issue the transfer */
	if (mvXorTransfer(chan, MV_DMA, channel.descPhyAddr) != MV_OK)
		printf("Error in DMA(XOR) Operation\n");

	/* wait for completion */
	xor_waiton_eng(chan);

	mvOsIoUncachedAlignedFree(NULL, sizeof(MV_XOR_DESC), channel.descPhyAddr, pVirt, 0);

	printf( "Done...\n" );
	return 1;
}

U_BOOT_CMD(
	dma,      1,     1,      mvDma_cmd,
	"dma	- Perform DMA using the XOR engine\n",
	" \n"
	"\tPerform DMA transaction with the parameters given by the user.\n"
);

#endif /* #if defined(MV_INCLUDE_XOR) */

/******************************************************************************
* Category     - Memory
* Functionality- Displays the MV's Memory map
* Need modifications (Yes/No) - Yes
*****************************************************************************/
int displayMemoryMap_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	mvCtrlAddrDecShow();
	return 1;
}

U_BOOT_CMD(
	map,      1,     1,      displayMemoryMap_cmd,
	"map	- Diasplay address decode windows\n",
	" \n"
	"\tDisplay controller address decode windows: CPU, PCI, Gig, DMA, XOR and COMM\n"
);



#include "ddr/mvSpd.h"
#if defined(MV_INC_BOARD_DDIM)

/******************************************************************************
* Category     - Memory
* Functionality- Displays the SPD information for a givven dimm
* Need modifications (Yes/No) - 
*****************************************************************************/
              
int dimminfo_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
        int num = 0;
 
        if (argc > 1) {
                num = simple_strtoul (argv[1], NULL, 10);
        }
 
        printf("*********************** DIMM%d *****************************\n",num);
 
        dimmSpdPrint(num);
 
        printf("************************************************************\n");
         
        return 1;
}
 
U_BOOT_CMD(
        ddimm,      2,     1,      dimminfo_cmd,
        "ddimm  - Display SPD Dimm Info\n",
        " [0/1]\n"
        "\tDisplay Dimm 0/1 SPD information.\n"
);

/******************************************************************************
* Category     - Memory
* Functionality- Copy the SPD information of dimm 0 to dimm 1
* Need modifications (Yes/No) - 
*****************************************************************************/
              
int spdcpy_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
 
        printf("Copy DIMM 0 SPD data into DIMM 1 SPD...");
 
        if (MV_OK != dimmSpdCpy())
        	printf("\nDIMM SPD copy fail!\n");
 	else
        	printf("Done\n");
         
        return 1;
}
 
U_BOOT_CMD(
        spdcpy,      2,     1,      spdcpy_cmd,
        "spdcpy  - Copy Dimm 0 SPD to Dimm 1 SPD \n",
        ""
        ""
);
#endif /* #if defined(MV_INC_BOARD_DDIM) */

/******************************************************************************
* Functionality- Go to an address and execute the code there and return,
*    defualt address is 0x40004
*****************************************************************************/
extern void cpu_dcache_flush_all(void);
extern void cpu_icache_flush_invalidate_all(void);

void mv_go(unsigned long addr,int argc, char *argv[])
{
	int rc;
	addr = MV_CACHEABLE(addr);
	char* envCacheMode = getenv("cacheMode");
 
	/*
	 * pass address parameter as argv[0] (aka command name),
	 * and all remaining args
	 */

    if(envCacheMode && (strcmp(envCacheMode,"write-through") == 0))
	{	
		int i=0;

		/* Flush Invalidate I-Cache */
		cpu_icache_flush_invalidate_all();

		/* Drain write buffer */
		asm ("mcr p15, 0, %0, c7, c10, 4": :"r" (i));
		

	}
	else /*"write-back"*/
	{
		int i=0;

		/* Flush Invalidate I-Cache */
		cpu_icache_flush_invalidate_all();

		/* Drain write buffer */
		asm ("mcr p15, 0, %0, c7, c10, 4": :"r" (i));
		

		/* Flush invalidate D-cache */
		cpu_dcache_flush_all();


    }


	rc = ((ulong (*)(int, char *[]))addr) (--argc, &argv[1]);
 
        return;
}

int g_cmd (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
        ulong   addr;

	if(!enaMonExt()){
		printf("This command can be used only if enaMonExt is set!\n");
		return 0;
	}

	addr = 0x40000;

        if (argc > 1) {
		addr = simple_strtoul(argv[1], NULL, 16);
        }
	mv_go(addr,argc,&argv[0]);
	return 1;
}                                                                                                                     

U_BOOT_CMD(
	g,      CONFIG_SYS_MAXARGS,     1,      g_cmd,
        "g	- start application at cached address 'addr'(default addr 0x40000)\n",
        " addr [arg ...] \n"
	"\tStart application at address 'addr' cachable!!!(default addr 0x40004/0x240004)\n"
	"\tpassing 'arg' as arguments\n"
	"\t(This command can be used only if enaMonExt is set!)\n"
);

/******************************************************************************
* Functionality- Searches for a value
*****************************************************************************/
int fi_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    MV_U32 s_address,e_address,value,i,tempData;
    MV_BOOL  error = MV_FALSE;

    if(!enaMonExt()){
	printf("This command can be used only if enaMonExt is set!\n");
	return 0;}

    if(argc == 4){
	value = simple_strtoul(argv[1], NULL, 16);
	s_address = simple_strtoul(argv[2], NULL, 16);
	e_address = simple_strtoul(argv[3], NULL, 16);
    }else{ printf ("Usage:\n%s\n", cmdtp->usage);
    	return 0;
    }     

    if(s_address == 0xffffffff || e_address == 0xffffffff) error = MV_TRUE;
    if(s_address%4 != 0 || e_address%4 != 0) error = MV_TRUE;
    if(s_address > e_address) error = MV_TRUE;
    if(error)
    {
	printf ("Usage:\n%s\n", cmdtp->usage);
        return 0;
    }
    for(i = s_address; i < e_address ; i+=4)
    {
        tempData = (*((volatile unsigned int *)i));
        if(tempData == value)
        {
            printf("Value: %x found at ",value);
            printf("address: %x\n",i);
            return 1;
        }
    }
    printf("Value not found!!\n");
    return 1;
}

U_BOOT_CMD(
	fi,      4,     1,      fi_cmd,
	"fi	- Find value in the memory.\n",
	" value start_address end_address\n"
	"\tSearch for a value 'value' in the memory from address 'start_address to\n"
	"\taddress 'end_address'.\n"
	"\t(This command can be used only if enaMonExt is set!)\n"
);

/******************************************************************************
* Functionality- Compare the memory with Value.
*****************************************************************************/
int cmpm_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    MV_U32 s_address,e_address,value,i,tempData;
    MV_BOOL  error = MV_FALSE;

    if(!enaMonExt()){
	printf("This command can be used only if enaMonExt is set!\n");
	return 0;}

    if(argc == 4){
	value = simple_strtoul(argv[1], NULL, 16);
	s_address = simple_strtoul(argv[2], NULL, 16);
	e_address = simple_strtoul(argv[3], NULL, 16);
    }else{ printf ("Usage:\n%s\n", cmdtp->usage);
    	return 0;
    }     

    if(s_address == 0xffffffff || e_address == 0xffffffff) error = MV_TRUE;
    if(s_address%4 != 0 || e_address%4 != 0) error = MV_TRUE;
    if(s_address > e_address) error = MV_TRUE;
    if(error)
    {
	printf ("Usage:\n%s\n", cmdtp->usage);
        return 0;
    }
    for(i = s_address; i < e_address ; i+=4)
    {
        tempData = (*((volatile unsigned int *)i));
        if(tempData != value)
        {
            printf("Value: %x found at address: %x\n",tempData,i);
        }
    }
    return 1;
}

U_BOOT_CMD(
	cmpm,      4,     1,      cmpm_cmd,
	"cmpm	- Compare Memory\n",
	" value start_address end_address.\n"
	"\tCompare the memory from address 'start_address to address 'end_address'.\n"
	"\twith value 'value'\n"
	"\t(This command can be used only if enaMonExt is set!)\n"
);



#if 0
/******************************************************************************
* Category     - Etherent
* Functionality- Display PHY ports status (using SMI access).
* Need modifications (Yes/No) - No
*****************************************************************************/
int eth_show_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ethRegs(argv[1]);
	ethPortRegs(argv[1]);
	ethPortStatus(argv[1]);
	ethPortQueues(argv[1],0,0,1);
	return 1;
}

U_BOOT_CMD(
	ethShow,      2,    2,      eth_show_cmd,
	"ethShow	- scanning the PHYs status\n",
	" \n"
	"\tScan all the Gig port PHYs and display their Duplex, Link, Speed and AN status.\n"
);
#endif

#if defined(MV_INCLUDE_PEX)

#include "pci-if/pci/mvPci.h"

int pcie_phy_read_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    	MV_U16 phyReg;

    	mvPexPhyRegRead(simple_strtoul( argv[1], NULL, 16 ),
	                simple_strtoul( argv[2], NULL, 16), &phyReg);

	printf ("0x%x\n", phyReg);

	return 1;
}

U_BOOT_CMD(
	pciePhyRead,      3,     3,      pcie_phy_read_cmd,
	"phyRead	- Read PCI-E Phy register\n",
	" PCI-E_interface Phy_offset. \n"
	"\tRead the PCI-E Phy register. \n"
);


int pcie_phy_write_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	mvPexPhyRegWrite(simple_strtoul( argv[1], NULL, 16 ),
					 simple_strtoul( argv[2], NULL, 16 ),
					 simple_strtoul( argv[3], NULL, 16 ));

	return 1;
}

U_BOOT_CMD(
	pciePhyWrite,      4,     4,      pcie_phy_write_cmd,
	"pciePhyWrite	- Write PCI-E Phy register\n",
	" PCI-E_interface Phy_offset value.\n"
	"\tWrite to the PCI-E Phy register.\n"
);

#endif /* if defined(MV_INCLUDE_GIG_ETH) */
#if defined(MV_INCLUDE_GIG_ETH)

#include "eth-phy/mvEthPhy.h"

int phy_read_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    	MV_U16 phyReg;

    	mvEthPhyRegRead(simple_strtoul( argv[1], NULL, 16 ),
	                simple_strtoul( argv[2], NULL, 16), &phyReg);

	printf ("0x%x\n", phyReg);

	return 1;
}

U_BOOT_CMD(
	phyRead,      3,     3,      phy_read_cmd,
	"phyRead	- Read Phy register\n",
	" Phy_address Phy_offset. \n"
	"\tRead the Phy register. \n"
);


int phy_write_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	mvEthPhyRegWrite(simple_strtoul( argv[1], NULL, 16 ),
					 simple_strtoul( argv[2], NULL, 16 ),
					 simple_strtoul( argv[3], NULL, 16 ));

	return 1;
}

U_BOOT_CMD(
	phyWrite,      4,     4,      phy_write_cmd,
	"phyWrite	- Write Phy register\n",
	" Phy_address Phy_offset value.\n"
	"\tWrite to the Phy register.\n"
);

#endif /* #if defined(MV_INCLUDE_GIG_ETH) */

#endif /* MV_TINY */

#if 0
#if defined(CONFIG_CMD_RCVR)
extern void recoveryHandle(void);
int do_rcvr (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	recoveryHandle();
	return 1;
}

U_BOOT_CMD(
	rcvr,	3,	1,	do_rcvr,
	"rcvr\t- Satrt recovery process (Distress Beacon with TFTP server)\n",
	"\n"
);
#endif	/* CFG_CMD_RCVR */
#endif //0
#if 0
#include "nfc/mvNfc.h"
#include "mvSysPdmaApi.h"

int mvNfcEWaitEvent(MV_NFC_CTRL * ctrl, MV_NFC_CMD_TYPE cmd, MV_U32 event, MV_U32 err)
{
	int i;
	MV_U32 stat;

	for (i=0; i<10; i++)
	{
		stat = mvNfcStatusGet(ctrl, cmd, NULL);
		//printf("Status = 0x%x, NDCR = %08x\n", stat, MV_REG_READ(0xc0000));	
		if (stat & err)
		{
			printf("mvNfcEWaitEvent Error (%08x)\n", stat);
			return -1;
		}
		if (stat & event)
			return 0;
		mvOsDelay(10); /*wait 10 ms */
	}

	printf("mvNfcEWaitEvent: Timeout\n");
	return -2;
}

void mvNfcDataPrint(MV_U32 buff)
{
	MV_U8 * datptr;
	MV_U32 i,j;

	datptr = (MV_U8*)buff;
	for (i=0; i<128; i+=1)
	{
		printf("%08x: ", (i*16));
		for (j=0; j<16; j++)
		{	
			printf("%02x ", *datptr);
			datptr++;
		}
		printf("\n");
	}
}

int mvNfcStatusRead(MV_NFC_CTRL * ctrl, MV_U32 * buff)
{
	if (mvNfcSelectChip(ctrl, MV_NFC_CS_0) != MV_OK)
	{
		printf("CS Assert Failed!!\n");
		return -1;
	}

	if (mvNfcCommandIssue(ctrl, MV_NFC_CMD_READ_STATUS, 0, 0) != MV_OK)
	{
		printf("Read status command issue failed!\n");
		return -1;
	}

	mvNfcEWaitEvent(ctrl, MV_NFC_CMD_READ_STATUS, MV_NFC_STATUS_RDD_REQ, 
			(MV_NFC_STATUS_COR_ERROR | MV_NFC_STATUS_UNC_ERROR | MV_NFC_STATUS_BBD));

	if (mvNfcReadWrite(ctrl, MV_NFC_CMD_READ_STATUS, buff, (MV_U32)buff) != MV_OK)
	{
		printf("Read Status Failed!\n");
		return -1;
	}
	
	if (mvNfcSelectChip(ctrl, MV_NFC_CS_NONE) != MV_OK)
	{
		printf("CS DEAssert Failed!!\n");
		return -1;
	}
	
	return 0;
}


MV_U32 mvNfcDbgFlag = 0;
MV_U32 mvNfcPrintFlag = 0;
extern MV_NFC_CTRL * gDbgCtrl;
MV_NFC_CTRL nfcDbgCtrl;

int mvNfc_cmd(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{		
	MV_U32 buff[1088];	
	char *cmd;
	cmd = argv[2];

	if (strcmp(cmd, "help") == 0) 
	{
		printf("nand dbg enable                      -> enable register read write log\n");
		printf("nand dbg disable                     -> disable register read write log\n");
		printf("nand dbg print                       -> enable read data print\n");
		printf("nand dbg noprint                     -> disable read data print\n");
		printf("nand dbg init                        -> initialize NFC\n");
		printf("nand dbg status                      -> read device status\n");
		printf("nand dbg id                          -> read device ID\n");
		printf("nand dbg read  <dest> <off> <size>   -> read \n");
		printf("nand dbg write <src> <off> <size>    -> program \n");
		printf("nand dbg erase <off> <size>          -> erase\n");
		printf("nand dbg readchain <des> <off> <cnt> -> Read in chained mode\n");
	}
	else if (strcmp(cmd, "enable") == 0) 
	{
		mvNfcDbgFlag = 1;
	}
	else if (strcmp(cmd, "disable") == 0) 
	{
		mvNfcDbgFlag = 0;
	}	
	else if (strcmp(cmd, "print") == 0) 
	{
		mvNfcPrintFlag = 1;
	}
	else if (strcmp(cmd, "noprint") == 0) 
	{
		mvNfcPrintFlag = 0;
	}
	else if (strcmp(cmd, "init") == 0) 
	{
		MV_NFC_INFO nfcInfo;

		/* Override Control structure */
		gDbgCtrl = &nfcDbgCtrl; 

		/*Fill info structure */
		nfcInfo.ioMode = MV_NFC_PDMA_ACCESS/*MV_NFC_PIO_ACCESS*/;
		nfcInfo.eccMode = MV_NFC_ECC_HAMMING/*MV_NFC_ECC_BCH*/;
#if defined(MV_NAND_GANG_MODE)
		nfcInfo.ifMode = MV_NFC_IF_2X8;
#else
		nfcInfo.ifMode = MV_NFC_IF_1X8;
#endif
		nfcInfo.autoStatusRead = MV_FALSE;
		nfcInfo.tclk = 166666667;
		nfcInfo.readyBypass = MV_FALSE;
		
		if (nfcInfo.ioMode == MV_NFC_PDMA_ACCESS)
		{
#if 0
/* Not needed here, mvPdmaInit() is performed in board_init() */
			printf("Initializing PDMA ... ");
			if (mvPdmaInit() != MV_OK)
			{
				printf("FAILED\n");
				return -1;
			}
			printf("OK\n");
#endif
		}		

		printf("Initializing NFC ... ");	
		if (mvNfcInit(&nfcInfo, gDbgCtrl) != MV_OK)
		{
			printf("FAILED\n");
			return -1;
		}
		printf("Flash detected %s\n", mvNfcFlashModelGet(gDbgCtrl));
	}
	else if (gDbgCtrl == NULL)
	{
		printf("board_nand_init() was never called!\n");
		return -1;
	}
	else if (strcmp(cmd, "status") == 0) 
	{
		if (mvNfcStatusRead(gDbgCtrl, buff) != 0)
			return -1;

		printf("Status = %08x\n", buff[0]);
	}
	else if (strcmp(cmd, "id") == 0) 
	{
		if (mvNfcSelectChip(gDbgCtrl, MV_NFC_CS_0) != MV_OK)
		{
			printf("CS Assert Failed!!\n");
			return -1;
		}

		if (mvNfcCommandIssue(gDbgCtrl, MV_NFC_CMD_READ_ID, 0, 0) != MV_OK)
		{
			printf("Command issue failed!\n");
			return -1;
		}
		
		mvNfcEWaitEvent(gDbgCtrl, MV_NFC_CMD_READ_ID, MV_NFC_STATUS_RDD_REQ, 
				(MV_NFC_STATUS_COR_ERROR | MV_NFC_STATUS_UNC_ERROR | MV_NFC_STATUS_BBD));

		buff[0] = buff[1] = 0xFFFFFFFF;
		if (mvNfcReadWrite(gDbgCtrl, MV_NFC_CMD_READ_ID, buff, (MV_U32)buff) != MV_OK)
		{
			printf("Read Status Failed!\n");
			return -1;
		}

		mvNfcEWaitEvent(gDbgCtrl, MV_NFC_CMD_READ_ID, MV_NFC_STATUS_CMDD, 
				(MV_NFC_STATUS_COR_ERROR | MV_NFC_STATUS_UNC_ERROR | MV_NFC_STATUS_BBD));

		if (mvNfcSelectChip(gDbgCtrl, MV_NFC_CS_NONE) != MV_OK)
		{
			printf("CS DEAssert Failed!!\n");
			return -1;
		}
	
		printf("ID = %08x\n", buff[0]);
	}
	else if (strcmp(cmd, "erase") == 0)
	{
		MV_U32 blkSize;
		MV_8* endptr;
		MV_U32 dest, size, offs, blk;

		if (mvNfcFlashBlockSizeGet(gDbgCtrl, &blkSize) != MV_OK)
		{	
			printf("mvNfcFlashBlockSizeGet Failed!\n");
			return -1;
		}
	
		if (argc != 5)
		{
			if (argc != 3)
				printf("Illegal parameters, assuming single page 0\n");			

			dest = 0;
			size = blkSize;
		}
		else
		{
			dest = simple_strtoul(argv[3], &endptr, 16);
			size = simple_strtoul(argv[4], &endptr, 16);
		
			if (dest & (blkSize-1))
				printf("Offset not alligned to block boundaries (0x%x)\n", blkSize);
			if (size & (blkSize-1))
				printf("Erase size not alligned to block size(0x%x)\n", blkSize);
		}

		printf("NFC Erase: Erase Offset=0x%08x, Erase Size=0x%08x (Block Size = %08x)\n", dest, size, blkSize);
		
		/* Loop and write all space required */		
		for (offs = dest; offs < (dest+size); offs+=blkSize)
		{
			printf("Erasing block offset %08x\n", offs);

			mvNfcAddress2BlockConvert(gDbgCtrl, offs, &blk);

			if (mvNfcSelectChip(gDbgCtrl, MV_NFC_CS_0) != MV_OK)
			{
				printf("CS Assert Failed!!\n");
				return -1;
			}

			if (mvNfcCommandIssue(gDbgCtrl, MV_NFC_CMD_ERASE, blk, 0) != MV_OK)
			{
				printf("Command issue failed!\n");
				return -1;
			}
	
			mvNfcEWaitEvent(gDbgCtrl, MV_NFC_CMD_ERASE, MV_NFC_STATUS_RDY/*MV_NFC_STATUS_CMDD*/, 
				(MV_NFC_STATUS_COR_ERROR | MV_NFC_STATUS_UNC_ERROR | MV_NFC_STATUS_BBD));

			if (mvNfcSelectChip(gDbgCtrl, MV_NFC_CS_NONE) != MV_OK)
			{
				printf("CS DEAssert Failed!!\n");
				return -1;
			}

			if (mvNfcStatusRead(gDbgCtrl, buff) != 0)
			{
				printf("Status Read Failed!\n");
				return -1;
			}
	
			if (buff[0] & 0x1)
				printf("SR0: Erase command Failed!\n");
		}
		//printf("\n");
	}
	else if (strcmp(cmd, "erasechain") == 0) 
	{
		MV_NFC_MULTI_CMD desc[MV_NFC_MAX_DESC_CHAIN];
		MV_U32 src, cnt, i;		
		MV_8* endptr;
		MV_U32 stat;

		if (argc != 5)
		{
			printf("Illegal parameters, assuming single page 0\n");			
			return -1;
		}

		src = simple_strtoul(argv[3], &endptr, 16);
		cnt = simple_strtoul(argv[4], &endptr, 16);
		
		if (cnt > MV_NFC_MAX_DESC_CHAIN)
		{
			printf("Count exceeds allocated descriptor count\n");
			return -1;
		}

		printf("NFS Chained Erase: Block Number=0x%08x, Block Count=%08x\n", src, cnt);

		if (mvNfcSelectChip(gDbgCtrl, MV_NFC_CS_0) != MV_OK)
		{
			printf("CS Assert Failed!!\n");
			return -1;
		}

		for (i=0; i<cnt; i++)
		{
			desc[i].cmd = MV_NFC_CMD_ERASE;		
			desc[i].pageAddr = (src + i);
			desc[i].pageCount = 0;
			desc[i].virtAddr = 0;
			desc[i].physAddr = 0;
		}

		/* Start the timer */
		MV_REG_WRITE(CNTMR_VAL_REG(1), 0xFFFFFFFF);
		MV_REG_BIT_SET(CNTMR_CTRL_REG, 0x4);	/* enable Timer 1 */

		if (mvNfcCommandMultiple(gDbgCtrl, desc, cnt) != MV_OK)
		{
			printf("Multiple Command Issue Failed!!\n");
			goto stopetimer;
		}		

		/* wait for the command to finish */
		while(1)
		{
			if(mvPdmaChannelStateGet(&gDbgCtrl->cmdChanHndl) == MV_PDMA_CHANNEL_STOPPED)
			{
				stat = mvNfcStatusGet(gDbgCtrl, MV_NFC_CMD_ERASE, NULL);
				if (stat & MV_NFC_STATUS_RDY)
				{
					/* Stop the imer and calculate time */
					MV_REG_BIT_RESET(CNTMR_CTRL_REG, 0x4);	/* disable Timer 1 as fast as possible */
					printf("Command Finished OK (Time Elapsed %dus)\n", ((0xFFFFFFFF - MV_REG_READ(CNTMR_VAL_REG(1))) / 166));
					break;
				}
			}
		}	
stopetimer:
		MV_REG_BIT_RESET(CNTMR_CTRL_REG, 0x4);	/* disable Timer 1 */

		if (mvNfcSelectChip(gDbgCtrl, MV_NFC_CS_NONE) != MV_OK)
		{
			printf("CS Assert Failed!!\n");
			return -1;
		}
	}
	else if (strcmp(cmd, "readchain") == 0) 
	{
		MV_NFC_MULTI_CMD desc[MV_NFC_MAX_DESC_CHAIN];
		MV_U32 src, dest, cnt, i;		
		MV_8* endptr;
		MV_U32 stat;

		if (argc != 6)
		{
			printf("Illegal parameters, assuming single page 0\n");			
			return -1;
		}

		dest = simple_strtoul(argv[3], &endptr, 16);
		src = simple_strtoul(argv[4], &endptr, 16);
		cnt = simple_strtoul(argv[5], &endptr, 16);
		
		if (cnt > MV_NFC_MAX_DESC_CHAIN)
		{
			printf("Count exceeds allocated descriptor count\n");
			return -1;
		}

		printf("NFS Chained Read: Page number=0x%08x, DDR destination=0x%08x, Page Count=%08x\n", src, dest, cnt);

		if (mvNfcSelectChip(gDbgCtrl, MV_NFC_CS_0) != MV_OK)
		{
			printf("CS Assert Failed!!\n");
			return -1;
		}

		for (i=0; i<cnt; i++)
		{
			desc[i].cmd = MV_NFC_CMD_READ_MONOLITHIC;		
			desc[i].pageAddr = (src + i);
			desc[i].pageCount = 1;
			desc[i].virtAddr = (MV_U32*)(dest + (i * 0x800));
			desc[i].physAddr = (dest + (i * 0x800));
		}

		/* Start the timer */
		MV_REG_WRITE(CNTMR_VAL_REG(1), 0xFFFFFFFF);
		MV_REG_BIT_SET(CNTMR_CTRL_REG, 0x4);	/* enable Timer 1 */

		if (mvNfcCommandMultiple(gDbgCtrl, desc, cnt) != MV_OK)
		{
			printf("Multiple Command Issue Failed!!\n");
			goto stoptimer;
		}		

		/* wait for the command to finish */
		while(1)
		{
			
			
			if(mvPdmaChannelStateGet(&gDbgCtrl->dataChanHndl) == MV_PDMA_CHANNEL_STOPPED)
			{
				stat = mvNfcStatusGet(gDbgCtrl, MV_NFC_CMD_READ_MONOLITHIC, NULL);
				if (stat & (MV_NFC_STATUS_UNC_ERROR |MV_NFC_STATUS_BBD))
				{				
					MV_REG_BIT_RESET(CNTMR_CTRL_REG, 0x4);	/* disable Timer 1 as fast as possible */
					printf("Error Detected (%08x)\n", stat);
					printf("Time Elapsed %dus\n",
							((0xFFFFFFFF - MV_REG_READ(CNTMR_VAL_REG(1))) / 166));
					break;
				}
			
			
				if (stat & MV_NFC_STATUS_CMDD)
				{
					/* Stop the imer and calculate time */
					MV_REG_BIT_RESET(CNTMR_CTRL_REG, 0x4);	/* disable Timer 1 as fast as possible */
					printf("Command Finished OK (Time Elapsed %dus)\n", ((0xFFFFFFFF - MV_REG_READ(CNTMR_VAL_REG(1))) / 166));
					break;
				}
			}
		}	
stoptimer:
		MV_REG_BIT_RESET(CNTMR_CTRL_REG, 0x4);	/* disable Timer 1 */

		if (mvNfcSelectChip(gDbgCtrl, MV_NFC_CS_NONE) != MV_OK)
		{
			printf("CS Assert Failed!!\n");
			return -1;
		}
	}
	else if (strcmp(cmd, "readchaing") == 0) 
	{
		MV_NFC_MULTI_CMD desc[MV_NFC_MAX_DESC_CHAIN];
		MV_U32 src, dest, cnt, i;		
		MV_8* endptr;
		MV_U32 stat;

		if (argc != 6)
		{
			printf("Illegal parameters, assuming single page 0\n");			
			return -1;
		}

		dest = simple_strtoul(argv[3], &endptr, 16);
		src = simple_strtoul(argv[4], &endptr, 16);
		cnt = simple_strtoul(argv[5], &endptr, 16);
		
		if (cnt > (MV_NFC_MAX_DESC_CHAIN/2))
		{
			printf("Count exceeds allocated descriptor count\n");
			return -1;
		}

		printf("NFS Ganaged Chained Read: Page number=0x%08x, DDR destination=0x%08x, Page Count=%08x\n", src, dest, cnt);

		if (mvNfcSelectChip(gDbgCtrl, MV_NFC_CS_0) != MV_OK)
		{
			printf("CS Assert Failed!!\n");
			return -1;
		}

		for (i=0; i<(cnt*2); i+=2)
		{
			desc[i].cmd = MV_NFC_CMD_READ_MONOLITHIC;		
			desc[i].pageAddr = (src + (i/2));
			desc[i].pageCount = 1;
			desc[i].virtAddr = (MV_U32*)(dest + (i * 0x800));
			desc[i].physAddr = (dest + (i * 0x800));

			desc[i+1].cmd = MV_NFC_CMD_READ_NAKED;		
			desc[i+1].pageAddr = (src + (i/2));
			desc[i+1].pageCount = 1;
			desc[i+1].virtAddr = (MV_U32*)(dest + ((i+1) * 0x800));
			desc[i+1].physAddr = (dest + ((i+1) * 0x800));
		}

		/* Start the timer */
		MV_REG_WRITE(CNTMR_VAL_REG(1), 0xFFFFFFFF);
		MV_REG_BIT_SET(CNTMR_CTRL_REG, 0x4);	/* enable Timer 1 */

		if (mvNfcCommandMultiple(gDbgCtrl, desc, (cnt*2)) != MV_OK)
		{
			printf("Multiple Command Issue Failed!!\n");
			goto stopgtimer;
		}		

		/* wait for the command to finish */
		while(1)
		{
			
			
			if(mvPdmaChannelStateGet(&gDbgCtrl->dataChanHndl) == MV_PDMA_CHANNEL_STOPPED)
			{
				stat = mvNfcStatusGet(gDbgCtrl, MV_NFC_CMD_READ_MONOLITHIC, NULL);
				if (stat & (MV_NFC_STATUS_UNC_ERROR |MV_NFC_STATUS_BBD))
				{				
					MV_REG_BIT_RESET(CNTMR_CTRL_REG, 0x4);	/* disable Timer 1 as fast as possible */
					printf("Error Detected (%08x)\n", stat);
					printf("Time Elapsed %dus\n", ((0xFFFFFFFF - MV_REG_READ(CNTMR_VAL_REG(1))) / 166));
					break;
				}
			
			
				if (stat & MV_NFC_STATUS_CMDD)
				{
					/* Stop the timer and calculate time */
					MV_REG_BIT_RESET(CNTMR_CTRL_REG, 0x4);	/* disable Timer 1 as fast as possible */
					printf("Command Finished OK (Time Elapsed %dus)\n", ((0xFFFFFFFF - MV_REG_READ(CNTMR_VAL_REG(1))) / 166));
					break;
				}
			}
		}	
stopgtimer:
		MV_REG_BIT_RESET(CNTMR_CTRL_REG, 0x4);	/* disable Timer 1 */

		if (mvNfcSelectChip(gDbgCtrl, MV_NFC_CS_NONE) != MV_OK)
		{
			printf("CS Assert Failed!!\n");
			return -1;
		}
	}
	else if (strcmp(cmd, "readchain1k") == 0) 
	{
		MV_NFC_MULTI_CMD desc[MV_NFC_MAX_DESC_CHAIN];
		MV_U32 src, dest, cnt, i;		
		MV_8* endptr;
		MV_U32 stat;
		MV_U32 spare[8] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
		MV_U32 err;
		MV_PDMA_CHAN_STATE pdmastat;
		MV_U32 reg;

		if (argc != 6)
		{
			printf("Illegal parameters, assuming single page 0\n");			
			return -1;
		}

		dest = simple_strtoul(argv[3], &endptr, 16);
		src = simple_strtoul(argv[4], &endptr, 16);		
		cnt = simple_strtoul(argv[5], &endptr, 16);
		
		if (cnt > (MV_NFC_MAX_DESC_CHAIN/5))
		{
			printf("Count exceeds allocated descriptor count (%d bigger than %d)\n", cnt, (MV_NFC_MAX_DESC_CHAIN/5));
			return -1;
		}

		printf("NFS Ganaged Chained Read 1KB: DDR Dest=0x%08x, Source page # =0x%08x, Page Count=%08x\n", dest, src, cnt);

		if (mvNfcSelectChip(gDbgCtrl, MV_NFC_CS_0) != MV_OK)
		{
			printf("CS Assert Failed!!\n");
			return -1;
		}

		for (i=0; i<(cnt*5); i+=5)
		{
			desc[i].cmd = MV_NFC_CMD_READ_MONOLITHIC;
			desc[i].pageAddr = (src + (i/5));
			desc[i].pageCount = 1;
			desc[i].virtAddr = (MV_U32*)(dest + ((i/5)*0x1000) + 0x0);
			desc[i].physAddr = (dest + ((i/5)*0x1000) + 0x0);
			desc[i].length = 0x400;
			desc[i].numSgBuffs = 1;

			desc[i+1].cmd = MV_NFC_CMD_READ_NAKED;		
			desc[i+1].pageAddr = (src + (i/5));
			desc[i+1].pageCount = 1;
			desc[i+1].virtAddr = (MV_U32*)(dest + ((i/5)*0x1000) + 0x400);
			desc[i+1].physAddr = (dest + ((i/5)*0x1000) + 0x400);
			desc[i+1].length = 0x400;
			desc[i+1].numSgBuffs = 1;

			desc[i+2].cmd = MV_NFC_CMD_READ_NAKED;		
			desc[i+2].pageAddr = (src + (i/5));
			desc[i+2].pageCount = 1;
			desc[i+2].virtAddr = (MV_U32*)(dest + ((i/5)*0x1000) + 0x800);
			desc[i+2].physAddr = (dest + ((i/5)*0x1000) + 0x800);
			desc[i+2].length = 0x400;
			desc[i+2].numSgBuffs = 1;

			desc[i+3].cmd = MV_NFC_CMD_READ_NAKED;		
			desc[i+3].pageAddr = (src + (i/5));
			desc[i+3].pageCount = 1;
			desc[i+3].virtAddr = (MV_U32*)(dest + ((i/5)*0x1000) + 0xC00);
			desc[i+3].physAddr = (dest + ((i/5)*0x1000) + 0xC00);
			desc[i+3].length = 0x400;
			desc[i+3].numSgBuffs = 1;

			desc[i+4].cmd = MV_NFC_CMD_READ_LAST_NAKED;		
			desc[i+4].pageAddr = (src + (i/5));
			desc[i+4].pageCount = 1;
			desc[i+4].virtAddr = spare;
			desc[i+4].physAddr = spare;
			desc[i+4].length = 0x20;
			desc[i+4].numSgBuffs = 1;
		}

		/* Start the timer */
		MV_REG_WRITE(CNTMR_VAL_REG(1), 0xFFFFFFFF);
		MV_REG_BIT_SET(CNTMR_CTRL_REG, 0x4);	/* enable Timer 1 */

		if ((err = mvNfcCommandMultiple(gDbgCtrl, desc, (cnt*5))) != MV_OK)
		{
			printf("Multiple Command Issue Failed (Error = %x!!\n", err);
			goto stoprtimer;
		}

		/* wait for the command to finish */
		while(1)
		{			
			if((pdmastat = mvPdmaChannelStateGet(&gDbgCtrl->dataChanHndl)) == MV_PDMA_CHANNEL_STOPPED)
			{
				stat = mvNfcStatusGet(gDbgCtrl, MV_NFC_CMD_READ_MONOLITHIC, NULL);
				if (stat & (MV_NFC_STATUS_UNC_ERROR | MV_NFC_STATUS_BBD))
				{				
					MV_REG_BIT_RESET(CNTMR_CTRL_REG, 0x4);	/* disable Timer 1 as fast as possible */
					printf("Error Detected (%08x)\n", stat);
					printf("Time Elapsed %dus\n", ((0xFFFFFFFF - MV_REG_READ(CNTMR_VAL_REG(1))) / 166));
					break;
				}
			
			
				if (stat & MV_NFC_STATUS_CMDD)
				{
					/* Stop the timer and calculate time */
					MV_REG_BIT_RESET(CNTMR_CTRL_REG, 0x4);	/* disable Timer 1 as fast as possible */
					printf("Command Finished OK (Time Elapsed %dus)\n", ((0xFFFFFFFF - MV_REG_READ(CNTMR_VAL_REG(1))) / 166));
					break;
				}
			}
		}	

stoprtimer:
		MV_REG_BIT_RESET(CNTMR_CTRL_REG, 0x4);	/* disable Timer 1 */

		if (mvNfcSelectChip(gDbgCtrl, MV_NFC_CS_NONE) != MV_OK)
		{
			printf("CS Assert Failed!!\n");
			return -1;
		}
	}
	else if (strcmp(cmd, "read") == 0) 
	{
		MV_U32 pgSize;
		MV_8* endptr;
		MV_U32 src, dest, size, offs, row, col;
		MV_U32 totalsize;

		if (mvNfcFlashPageSizeGet(gDbgCtrl, &pgSize, &totalsize) != MV_OK)
		{	
			printf("mvNfcFlashPageSizeGet Failed!\n");
			return -1;
		}
	
		if (argc != 6)
		{
			if (argc != 3)
				printf("Illegal parameters, assuming single page 0\n");			
			
			dest = (MV_U32) buff;
			src = 0;
			size = pgSize;
		}
		else
		{
			dest = simple_strtoul(argv[3], &endptr, 16);
			src = simple_strtoul(argv[4], &endptr, 16);
			size = simple_strtoul(argv[5], &endptr, 16);
		}

		printf("NFS Read: Source=0x%08x, DDR destination=0x%08x, Size=%08x\n (page Size = %x)", src, dest, size, pgSize);
		
		/* Loop and write all space required */		
		for (offs = src; offs < (src+size); offs+=pgSize, dest+=pgSize)
		{			
			mvNfcAddress2RowConvert(gDbgCtrl, offs, &row, &col);

			if (mvNfcSelectChip(gDbgCtrl, MV_NFC_CS_0) != MV_OK)
			{
				printf("CS Assert Failed!!\n");
				return -1;
			}	

			if (gDbgCtrl->ifMode == MV_NFC_IF_2X8) /* GANGED MODE */
			{
				row = (row/2);
				printf("READ-GNGD: row(%x), col(%x), dest(%x)\n", row, col, dest);\

				//printf("STEP 1: Issue MV_NFC_CMD_READ_MONOLITHIC\n");
				if (mvNfcCommandIssue(gDbgCtrl, MV_NFC_CMD_READ_MONOLITHIC, row, col) != MV_OK)
				{
					printf("Command issue failed!\n");
					return -1;
				}
	
				mvNfcEWaitEvent(gDbgCtrl, MV_NFC_CMD_READ_MONOLITHIC, MV_NFC_STATUS_RDD_REQ, 
					(MV_NFC_STATUS_COR_ERROR | MV_NFC_STATUS_UNC_ERROR | MV_NFC_STATUS_BBD));

				//printf("STEP 2: Date read MV_NFC_CMD_READ_MONOLITHIC\n");
				if (mvNfcReadWrite(gDbgCtrl, MV_NFC_CMD_READ_MONOLITHIC, (MV_U32*)dest, dest) != MV_OK)
				{
					printf("Read Status Failed!\n");
					return -1;
				}

				mvNfcEWaitEvent(gDbgCtrl, MV_NFC_CMD_READ_MONOLITHIC, (MV_NFC_STATUS_CMDD | MV_NFC_STATUS_PAGED), 
					(MV_NFC_STATUS_COR_ERROR | MV_NFC_STATUS_UNC_ERROR | MV_NFC_STATUS_BBD));

				if (mvNfcPrintFlag)
					mvNfcDataPrint(dest);

				dest += (pgSize/2);

			//	printf("dest2(%x)\n", dest);

				//printf("STEP 3: Issue MV_NFC_CMD_READ_MONOLITHIC\n");
				if (mvNfcCommandIssue(gDbgCtrl, MV_NFC_CMD_READ_NAKED, row, col) != MV_OK)
				{
					printf("Command issue failed!\n");
					return -1;
				}
	
				mvNfcEWaitEvent(gDbgCtrl, MV_NFC_CMD_READ_NAKED, MV_NFC_STATUS_RDD_REQ, 
					(MV_NFC_STATUS_COR_ERROR | MV_NFC_STATUS_UNC_ERROR | MV_NFC_STATUS_BBD));

				//printf("STEP 4: Date read MV_NFC_CMD_READ_MONOLITHIC\n");
				if (mvNfcReadWrite(gDbgCtrl, MV_NFC_CMD_READ_NAKED, (MV_U32*)dest, dest) != MV_OK)
				{
					printf("Read Status Failed!\n");
					return -1;
				}
		
				mvNfcEWaitEvent(gDbgCtrl, MV_NFC_CMD_READ_NAKED, (MV_NFC_STATUS_CMDD | MV_NFC_STATUS_PAGED), 
					(MV_NFC_STATUS_COR_ERROR | MV_NFC_STATUS_UNC_ERROR | MV_NFC_STATUS_BBD));
	
				if (mvNfcPrintFlag)
					mvNfcDataPrint(dest);
			
				dest -= (pgSize/2);
			}
			else /* NON-GANGED MODE */
			{
				printf("Read: ROW=%x COL=%x DST=%x\n", row, col, dest);
				if (mvNfcCommandIssue(gDbgCtrl, MV_NFC_CMD_READ_MONOLITHIC, row, col) != MV_OK)
				{
					printf("Command issue failed!\n");
					return -1;
				}

				mvNfcEWaitEvent(gDbgCtrl, MV_NFC_CMD_READ_MONOLITHIC, MV_NFC_STATUS_RDD_REQ, 
					(MV_NFC_STATUS_COR_ERROR | MV_NFC_STATUS_UNC_ERROR | MV_NFC_STATUS_BBD));

				if (mvNfcReadWrite(gDbgCtrl, MV_NFC_CMD_READ_MONOLITHIC, (MV_U32*)(dest), dest) != MV_OK)
				{
					printf("Read Status Failed!\n");
					return -1;
				}

				mvNfcEWaitEvent(gDbgCtrl, MV_NFC_CMD_READ_MONOLITHIC, (MV_NFC_STATUS_CMDD | MV_NFC_STATUS_PAGED), 
					(MV_NFC_STATUS_COR_ERROR | MV_NFC_STATUS_UNC_ERROR | MV_NFC_STATUS_BBD));

				if (mvNfcPrintFlag)
					mvNfcDataPrint(dest);
			}
	
			if (mvNfcSelectChip(gDbgCtrl, MV_NFC_CS_NONE) != MV_OK)
			{
				printf("CS DEAssert Failed!!\n");
				return -1;
			}
		} // read loop
	}
	else if (strcmp(cmd, "writechain1k") == 0) 
	{
		MV_NFC_MULTI_CMD desc[MV_NFC_MAX_DESC_CHAIN];
		MV_U32 src, dest, cnt, i;		
		MV_8* endptr;
		MV_U32 stat;
		MV_U32 spare[8] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
		MV_U32 err;

		if (argc != 6)
		{
			printf("Illegal parameters, assuming single page 0\n");			
			return -1;
		}

		src = simple_strtoul(argv[3], &endptr, 16);
		dest = simple_strtoul(argv[4], &endptr, 16);		
		cnt = simple_strtoul(argv[5], &endptr, 16);
		
		if (cnt > (MV_NFC_MAX_DESC_CHAIN/7))
		{
			printf("Count exceeds allocated descriptor count (%d bigger than %d)\n", cnt, (MV_NFC_MAX_DESC_CHAIN/7));
			return -1;
		}

		printf("NFS Ganaged Chained Write 1KB: DDR Source=0x%08x, Destination page # =0x%08x, Page Count=%08x\n", src, dest, cnt);

		if (mvNfcSelectChip(gDbgCtrl, MV_NFC_CS_0) != MV_OK)
		{
			printf("CS Assert Failed!!\n");
			return -1;
		}

		for (i=0; i<(cnt*7); i+=7)
		{
			desc[i].cmd = MV_NFC_CMD_WRITE_DISPATCH_START;
			desc[i].pageAddr = (dest + (i/7));
			desc[i].pageCount = 1;
			desc[i].virtAddr = 0;
			desc[i].physAddr = 0;
			desc[i].length = 0;
			desc[i].numSgBuffs = 1;

			desc[i+1].cmd = MV_NFC_CMD_WRITE_NAKED;		
			desc[i+1].pageAddr = (dest + (i/7));
			desc[i+1].pageCount = 1;
			desc[i+1].virtAddr = (MV_U32*)(src + ((i/7)*0x1000) + 0x4);
			desc[i+1].physAddr = (src + ((i/7)*0x1000) + 0x0);
			desc[i+1].length = 0x400;
			desc[i+1].numSgBuffs = 1;

			desc[i+2].cmd = MV_NFC_CMD_WRITE_NAKED;		
			desc[i+2].pageAddr = (dest + (i/7));
			desc[i+2].pageCount = 1;
			desc[i+2].virtAddr = (MV_U32*)(src + ((i/7)*0x1000) + 0x400);
			desc[i+2].physAddr = (src + ((i/7)*0x1000) + 0x400);
			desc[i+2].length = 0x400;
			desc[i+2].numSgBuffs = 1;

			desc[i+3].cmd = MV_NFC_CMD_WRITE_NAKED;		
			desc[i+3].pageAddr = (dest + (i/7));
			desc[i+3].pageCount = 1;
			desc[i+3].virtAddr = (MV_U32*)(src + ((i/7)*0x1000) + 0x800);
			desc[i+3].physAddr = (src + ((i/7)*0x1000) + 0x800);
			desc[i+3].length = 0x400;
			desc[i+3].numSgBuffs = 1;

			desc[i+4].cmd = MV_NFC_CMD_WRITE_NAKED;		
			desc[i+4].pageAddr = (dest + (i/7));
			desc[i+4].pageCount = 1;
			desc[i+4].virtAddr = (MV_U32*)(src + ((i/7)*0x1000) + 0xC00);
			desc[i+4].physAddr = (src + ((i/7)*0x1000) + 0xC00);
			desc[i+4].length = 0x400;
			desc[i+4].numSgBuffs = 1;

			desc[i+5].cmd = MV_NFC_CMD_WRITE_NAKED;		
			desc[i+5].pageAddr = (dest + (i/7));
			desc[i+5].pageCount = 1;
			desc[i+5].virtAddr = spare;
			desc[i+5].physAddr = spare;
			desc[i+5].length = 0x20;
			desc[i+5].numSgBuffs = 1;

			desc[i+6].cmd = MV_NFC_CMD_WRITE_DISPATCH_END;
			desc[i+6].pageAddr = (dest + (i/7));
			desc[i+6].pageCount = 1;
			desc[i+6].virtAddr = 0;
			desc[i+6].physAddr = 0;
			desc[i+6].length = 0;
			desc[i+6].numSgBuffs = 1;
		}

		/* Start the timer */
		MV_REG_WRITE(CNTMR_VAL_REG(1), 0xFFFFFFFF);
		MV_REG_BIT_SET(CNTMR_CTRL_REG, 0x4);	/* enable Timer 1 */

		if ((err = mvNfcCommandMultiple(gDbgCtrl, desc, (cnt*7))) != MV_OK)
		{
			printf("Multiple Command Issue Failed (Error = %x!!\n", err);
			goto stopwtimer;
		}		

		/* wait for the command to finish */
		while(1)
		{			
			if(mvPdmaChannelStateGet(&gDbgCtrl->dataChanHndl) == MV_PDMA_CHANNEL_STOPPED)
			{
				stat = mvNfcStatusGet(gDbgCtrl, MV_NFC_CMD_WRITE_MONOLITHIC, NULL);
				printf("mvNfcStatusGet = %08x\n", stat);
				if (stat & MV_NFC_STATUS_RDY)
				{
					/* Stop the timer and calculate time */
					MV_REG_BIT_RESET(CNTMR_CTRL_REG, 0x4);	/* disable Timer 1 as fast as possible */
					printf("Command Finished OK (Time Elapsed %dus)\n", ((0xFFFFFFFF - MV_REG_READ(CNTMR_VAL_REG(1))) / 166));
					break;
				}
			}
		}	
stopwtimer:
		MV_REG_BIT_RESET(CNTMR_CTRL_REG, 0x4);	/* disable Timer 1 */

		if (mvNfcSelectChip(gDbgCtrl, MV_NFC_CS_NONE) != MV_OK)
		{
			printf("CS Assert Failed!!\n");
			return -1;
		}
	}
	else if (strcmp(cmd, "writechain") == 0) 
	{
		MV_NFC_MULTI_CMD desc[MV_NFC_MAX_DESC_CHAIN];
		MV_U32 src, dest, cnt, i;		
		MV_8* endptr;
		MV_U32 stat;

		if (argc != 6)
		{
			printf("Illegal parameters, assuming single page 0\n");			
			return -1;
		}

		dest = simple_strtoul(argv[3], &endptr, 16);
		src = simple_strtoul(argv[4], &endptr, 16);
		cnt = simple_strtoul(argv[5], &endptr, 16);
		
		if (cnt > MV_NFC_MAX_DESC_CHAIN)
		{
			printf("Count exceeds allocated descriptor count\n");
			return -1;
		}

		printf("NFS Chained Program: Page number=0x%08x, DDR destination=0x%08x, Page Count=%08x\n", dest, src, cnt);

		if (mvNfcSelectChip(gDbgCtrl, MV_NFC_CS_0) != MV_OK)
		{
			printf("CS Assert Failed!!\n");
			return -1;
		}

		for (i=0; i<cnt; i++)
		{
			desc[i].cmd = MV_NFC_CMD_WRITE_MONOLITHIC;		
			desc[i].pageAddr = (dest + i);
			desc[i].pageCount = 1;
			desc[i].virtAddr = (MV_U32*)(src + (i * 0x800));
			desc[i].physAddr = (src + (i * 0x800));
		}

		/* Start the timer */
		MV_REG_WRITE(CNTMR_VAL_REG(1), 0xFFFFFFFF);
		MV_REG_BIT_SET(CNTMR_CTRL_REG, 0x4);	/* enable Timer 1 */

		if (mvNfcCommandMultiple(gDbgCtrl, desc, cnt) != MV_OK)
		{
			printf("Multiple Command Issue Failed!!\n");
			goto stopptimer;
		}		

		/* wait for the command to finish */
		while(1)
		{
			if(mvPdmaChannelStateGet(&gDbgCtrl->dataChanHndl) == MV_PDMA_CHANNEL_STOPPED)
			{
				stat = mvNfcStatusGet(gDbgCtrl, MV_NFC_CMD_WRITE_MONOLITHIC, NULL);
				if (stat & (MV_NFC_STATUS_UNC_ERROR |MV_NFC_STATUS_BBD))
				{									
					MV_REG_BIT_RESET(CNTMR_CTRL_REG, 0x4);	/* disable Timer 1 as fast as possible */
					printf("Error Detected (%08x)\n", stat);
					printf("Time Elapsed %dus\n", ((0xFFFFFFFF - MV_REG_READ(CNTMR_VAL_REG(1))) / 166));
					break;
				}
			
			
				if (stat & MV_NFC_STATUS_CMDD)
				{
					/* Stop the imer and calculate time */
					MV_REG_BIT_RESET(CNTMR_CTRL_REG, 0x4);	/* disable Timer 1 as fast as possible */
					printf("Command Finished OK (Time Elapsed %dus)\n", ((0xFFFFFFFF - MV_REG_READ(CNTMR_VAL_REG(1))) / 166));
					break;
				}
			}
		}	
stopptimer:
		MV_REG_BIT_RESET(CNTMR_CTRL_REG, 0x4);	/* disable Timer 1 */

		if (mvNfcSelectChip(gDbgCtrl, MV_NFC_CS_NONE) != MV_OK)
		{
			printf("CS Assert Failed!!\n");
			return -1;
		}
	}
	else if (strcmp(cmd, "write") == 0)
	{
		MV_U32 pgSize;
		MV_8* endptr;
		MV_U32 src, dest, size, offs, row, col;
		MV_STATUS ret;
		MV_U8	tmpbuff[2112];
		MV_U32 totalsize;

		memset(tmpbuff, 0xFF, 2112);

		if (mvNfcFlashPageSizeGet(gDbgCtrl, &pgSize, &totalsize) != MV_OK)
		{	
			printf("mvNfcFlashPageSizeGet Failed!\n");
			return -1;
		}
	
		src = simple_strtoul(argv[3], &endptr, 16);
		dest = simple_strtoul(argv[4], &endptr, 16);
		size = simple_strtoul(argv[5], &endptr, 16);

		printf("NFS Program: DDR Source=0x%08x, Flash Offset=0x%08x, Size=%08x (Page Size = %08x)\n", src, dest, size, pgSize);
		
		/* Loop and write all space required */		
		for (offs = dest; offs < (dest+size); offs+=pgSize, src+=pgSize)
		{
			mvNfcAddress2RowConvert(gDbgCtrl, offs, &row, &col);

			if (mvNfcSelectChip(gDbgCtrl, MV_NFC_CS_0) != MV_OK)
			{
				printf("CS Assert Failed!!\n");
				return -1;
			}

			if (gDbgCtrl->ifMode == MV_NFC_IF_2X8) /* GANGED MODE */
			{
				row = (row/2);

				printf("PRGRM-GNGD: row(%x), col(%x), src1(%x)\n", row, col, src);

				/*****/
				/* 1 */
			//	printf(">>>>>>>>>>>>>>>>>>>>>>STEP 1: MV_NFC_CMD_WRITE_DISPATCH_START\n");
				if ((ret = mvNfcCommandIssue(gDbgCtrl, MV_NFC_CMD_WRITE_DISPATCH_START, row, col)) != MV_OK)
				{
					printf("Command issue failed (errCode = %d)!\n", ret);
					return -1;
				}

				mvNfcEWaitEvent(gDbgCtrl, MV_NFC_CMD_WRITE_DISPATCH_START, MV_NFC_STATUS_CMDD, 
					(MV_NFC_STATUS_COR_ERROR | MV_NFC_STATUS_UNC_ERROR | MV_NFC_STATUS_BBD));

				
				/*****/
				/* 2 */
			//	printf(">>>>>>>>>>>>>>>>>>>>>>STEP 2: Issue MV_NFC_CMD_WRITE_NAKED\n");
				if ((ret = mvNfcCommandIssue(gDbgCtrl, MV_NFC_CMD_WRITE_NAKED, row, col)) != MV_OK)
				{
					printf("Command issue failed (errCode = %d)!\n", ret);
					return -1;
				}

				mvNfcEWaitEvent(gDbgCtrl, MV_NFC_CMD_WRITE_NAKED, MV_NFC_STATUS_WRD_REQ, 
					(MV_NFC_STATUS_COR_ERROR | MV_NFC_STATUS_UNC_ERROR | MV_NFC_STATUS_BBD));

				/*****/
				/* 3 */
				memcpy(tmpbuff, (MV_U8*)src, 2048);
			//	printf(">>>>>>>>>>>>>>>>>>>>>>STEP 3: Write Data MV_NFC_CMD_WRITE_NAKED\n");
				if ((ret = mvNfcReadWrite(gDbgCtrl, MV_NFC_CMD_WRITE_NAKED, (MV_U32*)tmpbuff, (MV_U32)tmpbuff)) != MV_OK)
				{
					printf("Read Status Failed (errCode = %d)!\n", ret);
					return -1;
				}

				mvNfcEWaitEvent(gDbgCtrl, MV_NFC_CMD_WRITE_NAKED, MV_NFC_STATUS_PAGED, 
					(MV_NFC_STATUS_COR_ERROR | MV_NFC_STATUS_UNC_ERROR | MV_NFC_STATUS_BBD));

				/* increment address */
				src += (pgSize/2);

			//	printf("src2(%x)\n", src);
	
				/*****/
				/* 4 */
			//	printf(">>>>>>>>>>>>>>>>>>>>>>STEP 4: Issue MV_NFC_CMD_WRITE_NAKED\n");
				if ((ret = mvNfcCommandIssue(gDbgCtrl, MV_NFC_CMD_WRITE_NAKED, row, col)) != MV_OK)
				{
					printf("Command issue failed (errCode = %d)!\n", ret);
					return -1;
				}

				mvNfcEWaitEvent(gDbgCtrl, MV_NFC_CMD_WRITE_NAKED, MV_NFC_STATUS_WRD_REQ, 
					(MV_NFC_STATUS_COR_ERROR | MV_NFC_STATUS_UNC_ERROR | MV_NFC_STATUS_BBD));


				/*****/
				/* 5 */
				memcpy(tmpbuff, (MV_U8*)src, 2048);
			//	printf(">>>>>>>>>>>>>>>>>>>>>>STEP 5: Write Data MV_NFC_CMD_WRITE_NAKED\n");
				if ((ret = mvNfcReadWrite(gDbgCtrl, MV_NFC_CMD_WRITE_NAKED, (MV_U32*)tmpbuff, (MV_U32)tmpbuff)) != MV_OK)
				{
					printf("Read Status Failed (errCode = %d)!\n", ret);
					return -1;
				}

				mvNfcEWaitEvent(gDbgCtrl, MV_NFC_CMD_WRITE_NAKED, MV_NFC_STATUS_PAGED, 
					(MV_NFC_STATUS_COR_ERROR | MV_NFC_STATUS_UNC_ERROR | MV_NFC_STATUS_BBD));

				/*****/
				/* 6 */
			//	printf(">>>>>>>>>>>>>>>>>>>>>>STEP 6: MV_NFC_CMD_WRITE_DISPATCH_END\n");
				if ((ret = mvNfcCommandIssue(gDbgCtrl, MV_NFC_CMD_WRITE_DISPATCH_END, row, col)) != MV_OK)
				{
					printf("Command issue failed (errCode = %d)!\n", ret);
					return -1;
				}

				mvNfcEWaitEvent(gDbgCtrl, MV_NFC_CMD_WRITE_DISPATCH_END, MV_NFC_STATUS_CMDD, 
					(MV_NFC_STATUS_COR_ERROR | MV_NFC_STATUS_UNC_ERROR | MV_NFC_STATUS_BBD));

				/* increment address */
				src -= (pgSize/2);
			}
			else /* NON-GANGED MODE */
			{
				printf("Write: ROW=%x COL=%x SRC=%x\n", row, col, src);
				if ((ret = mvNfcCommandIssue(gDbgCtrl, MV_NFC_CMD_WRITE_MONOLITHIC, row, col)) != MV_OK)
				{
					printf("Command issue failed (errCode = %d)!\n", ret);
					return -1;
				}

				mvNfcEWaitEvent(gDbgCtrl, MV_NFC_CMD_WRITE_MONOLITHIC, MV_NFC_STATUS_WRD_REQ, 
					(MV_NFC_STATUS_COR_ERROR | MV_NFC_STATUS_UNC_ERROR | MV_NFC_STATUS_BBD));

				memcpy(tmpbuff, (MV_U8*)src, 2048);
				if ((ret = mvNfcReadWrite(gDbgCtrl, MV_NFC_CMD_WRITE_MONOLITHIC, (MV_U32*)tmpbuff, (MV_U32)tmpbuff)) != MV_OK)
				{
					printf("Read Status Failed (errCode = %d)!\n", ret);
					return -1;
				}

				mvNfcEWaitEvent(gDbgCtrl, MV_NFC_CMD_WRITE_MONOLITHIC, MV_NFC_STATUS_PAGED, 
					(MV_NFC_STATUS_COR_ERROR | MV_NFC_STATUS_UNC_ERROR | MV_NFC_STATUS_BBD));
			}
			
			if (mvNfcSelectChip(gDbgCtrl, MV_NFC_CS_NONE) != MV_OK)
			{
				printf("CS DEAssert Failed!!\n");
				return -1;
			}
		} /* page loop */
	}

	return 1;
}
#endif /* MV_NFC_DBG */

#if 0
#if defined(CFG_NAND_BOOT)
extern int doNandScrub(struct nand_chip* nand, size_t ofs);

/* Erase bad blocks - for internal use only */
int do_nand_scrub(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	extern char console_buffer[];

	printf("\n**Warning**\n");
	printf("This command will erase NAND flash bad blocks ...\n");
	printf("Are you sure ? (y/n)");
	readline(" ");

	if( strcmp(console_buffer,"Y") == 0 || 
	    strcmp(console_buffer,"yes") == 0 ||
	    strcmp(console_buffer,"y") == 0 ) {

		doNandScrub(nand_dev_desc + 0, 0);
	}
	return 1;
}

U_BOOT_CMD(
	nandScrub,      4,     4,      do_nand_scrub,
	"",
	" "
	""
);

#endif /* defined(CFG_NAND_BOOT) */
#endif


int _4BitSwapArry[] = {0,8,4,0xc,2,0xa,6,0xe,1,9,5,0xd,3,0xb,7,0xf};
int _3BitSwapArry[] = {0,4,2,6,1,5,3,7};

int do_satr(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	char *cmd, *s;
	MV_U8 data0=0, data1=0, devNum0=0, devNum1=0;
	MV_U8 moreThenOneDev=0, regNum = 0;
	MV_U8 mask0=0, mask1=0, shift0=0, shift1=0;
	MV_U8 val=0, width=0;

	/* at least two arguments please */
	if (argc < 2)
		goto usage;

	cmd = argv[1];

	if (strncmp(cmd, "read", 4) != 0 && strncmp(cmd, "write", 5) != 0 && strncmp(cmd, "options", 7) != 0)
		goto usage;

	/* read write */
	if (strncmp(cmd, "read", 4) == 0 || strncmp(cmd, "write", 5) == 0) {
		int read;


		read = strncmp(cmd, "read", 4) == 0; /* 1 = read, 0 = write */

		/* In write mode we have additional value */
		if (!read)
		{
			if (argc < 3)
				goto usage;
			else
				/* Value for write */
				val = (ulong)simple_strtoul(argv[2], NULL, 16);
		}

		printf("\nS@R %s: ", read ? "read" : "write");
		s = strchr(cmd, '.');
		if ((s != NULL) && (strcmp(s, ".cpu") == 0))
		{
			moreThenOneDev = 0;
			regNum = 0;
			devNum0 = 0;
			mask0 = 0xf;
			shift0 = 0;
			mask1 = 0x0;
			shift1 = 0;
			width = 4;
		}

		if ((s != NULL) && (strcmp(s, ".cpu2L2") == 0))
		{
			moreThenOneDev = 1;
			regNum = 0;
			devNum0 = 0;
			devNum1 = 1;
			mask0 = 0x10;
			shift0 = 4;
			mask1 = 0x3;
			shift1 = 1;
			width = 3;
		}

		if ((s != NULL) && (strcmp(s, ".cpu2ddr") == 0))
		{
			moreThenOneDev = 1;
			regNum = 0;
			devNum0 = 1;
			devNum1 = 2;
			mask0 = 0x1c;
			shift0 = 2;
			mask1 = 0x1;
			shift1 = 3;
			width = 4;
		}

		if ((s != NULL) && (strcmp(s, ".SSCG") == 0)) 
		{
			moreThenOneDev = 0;
			regNum = 0;
			devNum0 = 2;
			mask0 = 0x10;
			shift0 = 4;
			mask1 = 0x0;
			shift1 = 0;
		}

		if (read) {
			/* read */
			data0 = mvBoarTwsiSatRGet(devNum0, regNum);
			if (moreThenOneDev)
				data1 = mvBoarTwsiSatRGet(devNum1, regNum);

			data0 = ((data0 & mask0) >> shift0);

			if (moreThenOneDev)
			{
				data1 = ((data1 & mask1) << shift1);
				data0 |= data1;
			}
			printf("Read S@R val %x\n", data0);

		} else {
			/* read modify write */
			data0 = mvBoarTwsiSatRGet(devNum0, regNum);
			data0 = (data0 & ~mask0);
			data0 |= ((val << shift0) & mask0);
			if (mvBoarTwsiSatRSet(devNum0, regNum, data0) != MV_OK)
			{
				printf("Write S@R first device val %x fail\n", data0);
				return 1;
			}
			printf("Write S@R first device val %x succeded\n", data0);

			if (moreThenOneDev)
			{
				data1 = mvBoarTwsiSatRGet(devNum1, regNum);
				data1 = (data1 & ~mask1);
				data1 |= ((val >> shift1) & mask1);
				if (mvBoarTwsiSatRSet(devNum1, regNum, data1) != MV_OK)
				{
					printf("Write S@R second device val %x fail\n", data1);
					return 1;
				}
				printf("Write S@R second device val %x succeded\n", data1);
			}
		}

		return 0;
	}
    else if(strncmp(cmd, "options", 7) == 0)
    {
        printf("CPU\n=======\n");
        printf("0xF - 333MHz\n");
        printf("0xE - 400MHz\n");
        printf("0xD - 533MHz\n");
        printf("0xC - 667MHz\n");
        printf("0xA - 800MHz\n");
        printf("0x6 - 933MHz\n");
        printf("0x5 - 1000MHz\n");
        printf("0xB - 1067MHz\n");

        printf("\n\nCPU : L2\n=========\n");
        printf("0x0 - 1:1\n");
        printf("0x2 - 1:2\n");
        printf("0x4 - 1:3\n");
        printf("0x6 - 1:4\n");

        printf("\n\nCPU : DDR DRAM\n=============\n");
        printf("0x0 - 1:1\n");
        printf("0x2 - 1:2\n");
        printf("0x3 - 1:2.5\n");
        printf("0x4 - 1:3\n");
        printf("0x6 - 1:4\n");
        printf("0x8 - 1:5\n");
        printf("0xA - 1:6\n");
        printf("0xC - 1:7\n");
        printf("0xE - 1:8\n");
        printf("0xF - 1:10\n");
        return 0;
    }

usage:
	printf("Usage:\n%s\n", cmdtp->usage);
	return 1;
}

U_BOOT_CMD(SatR, 5, 1, do_satr,
	"SatR - sample at reset sub-system, relevent for DB only\n",
	"SatR read.cpu 		- read cpu clock from S@R devices\n"
	"SatR read.cpu2ddr	- read cpu2ddr clock ratio from S@R devices\n"
	"SatR read.cpu2L2	- read cpu2L2 clock ratio from S@R devices\n"
	"SatR read.SSCG		- read SSCG state from S@R devices [0 ~ en]\n"
	"SatR write.cpu val	- write cpu clock val to S@R devices [0,1,..,F]\n"
	"SatR write.cpu2ddr val	- write cpu2ddr clock ratio val to S@R devices [0,1,..,F]\n"
	"SatR write.cpu2L2 val	- write cpu2L2 clock ratio val to S@R devices [0,1,..,7]\n"
	"SatR write.SSCG val	- write SSCG state val to S@R devices [0 ~ en]\n"
    "SatR options	- List all reset strapping options\n"
);

#include "twsi/mvTwsi.h"
#include "audio/mvAudioRegs.h"
#include "audio/mvAudio.h"
#include "audio/dac/mvCLAudioCodec.h"


static MV_U32 codec_init_values[][2] = {
	/* Address	Value	*/
	{	0x00,	0x0	},
	{	0x0c,	0x808	},
	{	0x1c,	0x2740	},
	{	0x6a,	0x46	},
	{	0x6c,	0xffff	},
	{	0x40,	0x3700	},
	{	0x34,	0x8000	},
	{	0x3e,	0x96c1	},
	{	0x3c,	0x67f3	},
	{	0x3a,	0xcd70	},
	{	0x04,	0x0	},
	{	0x02,	0x0	},
	{	0x5e,	0x0	},
	{	-1,	-1	}	
};

/*
** Initialize codec registers to enable audio playback.
*/
void audio_init_codec(void)
{
	MV_TWSI_ADDR	twsiAddr;
	MV_U32		i;
	MV_TWSI_SLAVE 	twsiSlave;
	MV_U8		data[2];
	int ADDRS = 0x4A;

	twsiAddr.address = ADDRS;
	twsiAddr.type = ADDR7_BIT;
	mvTwsiInit(0, CONFIG_SYS_I2C_SPEED, CONFIG_SYS_TCLK, &twsiAddr, MV_FALSE);

	for(i = 0; codec_init_values[i][0] != 0xFFFFFFFF; i++) {
		twsiSlave.slaveAddr.address = ADDRS;
		twsiSlave.slaveAddr.type = ADDR7_BIT;
		twsiSlave.validOffset = MV_TRUE;
		twsiSlave.offset = codec_init_values[i][0];
		twsiSlave.moreThen256 = MV_FALSE;
		data[0] = (MV_U8)((codec_init_values[i][1] >> 8) & 0xFF);
		data[1] = (MV_U8)(codec_init_values[i][1] & 0xFF);
		if(mvTwsiWrite(0, &twsiSlave, data, 2) != MV_OK) {
			printf("mvTwsiWrite() failed address [0x%x], value [0x%x] - FAILED\n", codec_init_values[i][0], codec_init_values[i][1]);
			continue;
		} else {
			printf("address [0x%x], value [0x%x]\n", codec_init_values[i][0], codec_init_values[i][1]);
		}
	}
	return;
}

int audio_play_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	MV_U32	dataAddr;
	MV_U32	dataSize;
	MV_STATUS status;
	MV_AUDIO_PLAYBACK_CTRL pbCtrl;
	MV_I2S_PLAYBACK_CTRL i2sCtrl;
	MV_AUDIO_FREQ_DATA dcoCtrl;
	
//	audio_init_codec();

	dataAddr = simple_strtoul( argv[1], NULL, 16 );
	dataSize = simple_strtoul( argv[2], NULL, 16 );

	/* Initialize Audio HAL */
	mvSysAudioInit(1);

	pbCtrl.burst = AUDIO_128BYTE_BURST;
	pbCtrl.loopBack = MV_FALSE;
	pbCtrl.monoMode = AUDIO_PLAY_BOTH_MONO;
	pbCtrl.bufferPhyBase = dataAddr;	/* Buffer holding PCM data */
	pbCtrl.bufferSize = dataSize;		/* Size of buffer in bytes */
	pbCtrl.intByteCount = dataSize;		/* N/A, no interrupts in Uboot */

	/* Set default sample size to 16-non compact */
	pbCtrl.sampleSize = SAMPLE_16BIT_NON_COMPACT;
	/* Check if user wants to override the default value */
	if(argc > 3) {
		if(strcmp(argv[3],"16nc") == 0)
			pbCtrl.sampleSize = SAMPLE_16BIT_NON_COMPACT;
		else if(strcmp(argv[3],"16") == 0)
			pbCtrl.sampleSize = SAMPLE_16BIT;
		else if(strcmp(argv[3],"24") == 0)
			pbCtrl.sampleSize = SAMPLE_24BIT;
		else if(strcmp(argv[3],"32") == 0)
			pbCtrl.sampleSize = SAMPLE_32BIT;
		else {
			printf("Bad sample size (16, 16nc, 24, 32).\n");
			return 0;
		}
	}
	status = mvAudioPlaybackControlSet(1, &pbCtrl);
	if(status != MV_OK) {
		printf("mvAudioPlaybackControlSet() failed (%d).\n", status);
		return 0;
	}

	/* Get the current DCO config */
	mvAudioDCOCtrlGet(1,&dcoCtrl);
	/* Set default sample rate. */
	dcoCtrl.baseFreq = AUDIO_FREQ_44_1KH;
	/* Check if user wants to overried default sample rate */ 
	if(argc > 4) {
		if(strcmp(argv[4],"44.1khz") == 0)
			dcoCtrl.baseFreq = AUDIO_FREQ_44_1KH;
		else if(strcmp(argv[4],"48khz") == 0)
			dcoCtrl.baseFreq = AUDIO_FREQ_48KH;
		else if(strcmp(argv[4],"96khz") == 0)
			dcoCtrl.baseFreq = AUDIO_FREQ_96KH;
		else {
			printf("Bad sample rate.\n");
			return 0;
		}
	}
	/* Set Sample rate */
	status = mvAudioDCOCtrlSet(1, &dcoCtrl);
	if(status != MV_OK) {
		printf("mvAudioDCOCtrlSet() failed (%d).\n", status);
		return 0;
	}

	/* Set I2S playback related parameters.	*/
	i2sCtrl.sampleSize = ((pbCtrl.sampleSize == SAMPLE_16BIT_NON_COMPACT) ? 
			SAMPLE_16BIT : pbCtrl.sampleSize);
	i2sCtrl.justification = I2S_JUSTIFIED;
	i2sCtrl.sendLastFrame = MV_FALSE;
	status = mvI2SPlaybackCtrlSet(0, &i2sCtrl);
	if(status != MV_OK) {
		printf("mvI2SPlaybackCtrlSet() failed (%d).\n", status);
		return 0;
	}

	/* Clear interrupt cause bit */
	MV_REG_WRITE(MV_AUDIO_INT_CAUSE_REG(1),(1 << 7));

	/* Unmute I2S */
	mvAudioI2SPlaybackMute(1, MV_FALSE);
	/* Enable playback */
	mvAudioI2SPlaybackEnable(1, MV_TRUE);
	/* Unpause I2S */
	mvAudioPlaybackPause(1, 0);

	/* 
	** Wait till audio buffer is consumed.
	** Otherwise the playback will loop over the buffer forever.
	*/
	while(!(MV_REG_READ(MV_AUDIO_INT_CAUSE_REG(1)) & (1 << 7))) {
		if(ctrlc())
			break;
	}
	/* Clear interrupt cause bit */
	MV_REG_WRITE(MV_AUDIO_INT_CAUSE_REG(1),(1 << 7));
	/* Stop playback */
	mvAudioI2SPlaybackMute(1, MV_TRUE);
	mvAudioPlaybackPause(1, MV_TRUE);
	mvAudioI2SPlaybackEnable(1, MV_FALSE);

	return 1;
}

U_BOOT_CMD(
	audioTest,      5,     1,      audio_play_cmd,
	"audioTest - Test audio playback\n",
	" AuidTest address size sample_size sample_rate\n"
	" \taddress\t - Address holding PCM data.\n" 
	" \tsize\t - data buffer size in bytes (< 16MB).\n"
	" \tsample_size\t - 16, 16nc, 24 or 32 (default 16nc).\n"
	" \tsample_rate\t - 44.1khz, 48khz or 96khz (default 44.1khz).\n"
	"\tPlay audio buffer. \n"
);

