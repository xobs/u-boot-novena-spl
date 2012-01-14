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
#define MV_INCLUDE_MAN_TEST
#ifdef MV_INCLUDE_MAN_TEST

#include "mv_manufacture_test.h"
u32	g_testReport=0;
/*
For marvell manufacture test only
*/

char *ptestItem[MAX_MAN_TEST_COUNT]=
{
	"Ethernet Test   =>",
	"NAND Flash Test =>",
	"NOR  Flash Test =>",
	"SATA Test       =>",
	"I2C  Test       =>",
	"USB  Test       =>",
	"PCIe Test       =>",
	"MMC  Test       =>",
	"DRAM Test       =>",
	"RTC  Test       =>"

};

pfunc_man_test pfunc_man_test_table[MAX_MAN_TEST_COUNT];
//-----------------------------------------------------------------------------
int mv_man_register_func(u8 func_id,pfunc_man_test pfunc)
{
	if(func_id >= MAX_MAN_TEST_COUNT) return 0;//something wrong

	pfunc_man_test_table[func_id]=pfunc;
	return 1;
}
//-----------------------------------------------------------------------------
int mv_man_test_sg(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	printf("*****Testing Giga net now*****\n");
	sg_cmd(cmdtp,flag,argc,argv);
	printf("******************************\n");
	if(g_netStatus==MV_OK)
	{
		printf("Ethernet Works!!\n");
		g_testReport|= (0x01<<MAN_FUNC_ID_SG);
	}
	else
		printf("Ethernet Failed!!\n");

	
	return 1;
}
//-----------------------------------------------------------------------------
#if defined(CONFIG_CMD_NAND)
int mv_man_test_nand(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	printf("*****Testing NAND now*********\n");
	argv[1]="info"; //for testing nand flash ID info
	do_nand(cmdtp,flag,2,argv);
	printf("******************************\n");

	if(strncmp(nand_info[0].name,NAND_DEVICE_NAME,5) == 0)
	{	
		printf("NAND Flash Works!!\n");
		g_testReport|= (0x01<<MAN_FUNC_ID_NAND);
		
	}
	else
	{
		printf("NAND Flash Failed!!\n");
		return 1;

	}

	return 1;
}
#endif
//-----------------------------------------------------------------------------
#ifdef CONFIG_CMD_SF
int mv_man_test_sf(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	printf("*****Testing NOR Flash now****\n");
	argv[1]="probe";
	argv[2]="0";

	do_spi_flash(cmdtp,flag,3,argv);
	printf("******************************\n");
	if(strncmp(flash->name,SF_DEVICE_VENDER_ID,10) == 0)
	{
		printf("NOR Flash Works!!\n");
		g_testReport|= (0x01<<MAN_FUNC_ID_SF);

	}
	else
		printf("NOR Flash Failed!!\n");

	return 1;
}
#endif
//-----------------------------------------------------------------------------
int mv_man_test_sata(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	printf("*****Testing SATA now*********\n");
	argv[1]="reset";

	do_ide(cmdtp,flag,2,argv);
	printf("******************************\n");
	if(strncmp(ide_dev_desc[0].vendor,IDE_DEVICE_VENDER_ID,9) == 0)
	{
		printf("SATA Works!!\n");
		g_testReport|= (0x01<<MAN_FUNC_ID_IDE);
		
	}
	else
		printf("SATA Failed,make sure you have pluged in sata disk for testing!!\n");
		
	return 1;
}
//-----------------------------------------------------------------------------
int mv_man_test_i2c(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	MV_STATUS status;
	u8 probe0=0;//detect if bus 0 ok ?
	printf("*****Testing I2C now**********\n");
	argv[1]="probe";
	do_i2c(cmdtp,flag,2,argv);

	argv[1]="md";
	argv[2]="0x1f";//device address
	argv[3]="0x00";//reg address
	argv[4]="0x02";//number of bytes to read
	status=do_i2c(cmdtp,flag,5,argv);
	printf("******************************\n");
	if(status==MV_OK)
	{
		printf("I2C BUS0 Works!!\n");
		probe0=1;

	}
	else
		printf("I2C BUS0 Failed,make sure Codec ALC6530 works !!\n");	

	argv[1]="dev";
	argv[2]="1";//switch to bus 1
	do_i2c(cmdtp,flag,3,argv);

	argv[1]="probe";
	do_i2c(cmdtp,flag,2,argv);
	
	argv[1]="md";
	argv[2]="0x39";//device address
	argv[3]="0x43";//reg address
	argv[4]="0x02";//number of bytes to read
	status=do_i2c(cmdtp,flag,5,argv); 
	if(status==MV_OK)
	{
		printf("I2C BUS1 Works!!\n");

	}
	else
		printf("I2C BUS1 Failed,make sure Codec HDMI(TX) works !!\n");	

	if(status==MV_OK && probe0==1)
		g_testReport|= (0x01<<MAN_FUNC_ID_I2C);

	argv[1]="dev";
	argv[2]="0";//switch back to bus 0
	do_i2c(cmdtp,flag,3,argv);
		
	return 1;
}
//-----------------------------------------------------------------------------
int mv_man_test_usb(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	printf("*****Testing USB now**********\n");
	argv[1]="reset";

	do_usb(cmdtp,flag,2,argv);
	printf("******************************\n");
	if(strncmp(usb_dev[2].mf,USB_DEVICE_VENDER_ID,8) == 0)
	{
		printf("USB Works!!\n");
		g_testReport|= (0x01<<MAN_FUNC_ID_USB);

	}
	else
		printf("USB Failed,make sure you have pluged in usb disk for testing!!\n");

	return 1;
}
//-----------------------------------------------------------------------------
int mv_man_test_pcie(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	printf("*****Testing PCIe now*********\n");
	argv[1]="0";
	sp_cmd(cmdtp,flag,2,argv);
	argv[1]="1";
	sp_cmd(cmdtp,flag,2,argv);
	printf("******************************\n");
	if(g_pciDeviceStatus==1)
	{
		printf("PCIe BUS Works!!\n");
		g_testReport|= (0x01<<MAN_FUNC_ID_PCI);

	}
	else
		printf("PCIe Failed,make sure you have pluged in PCIe device for testing!!\n");		

	return 1;
}
//-----------------------------------------------------------------------------
int mv_man_test_mmc(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	printf("*****Testing MMC now**********\n");
	argv[1]="1";
	do_mmcinfo (cmdtp,flag,2,argv);
	printf("******************************\n");
	if(g_mmcStatus==1)
	{
		printf("MMC BUS Works!!\n");
		g_testReport|= (0x01<<MAN_FUNC_ID_MMC);

	}
	else
		printf("MMC BUS Failed,make sure you have pluged in SD card for testing!!\n");		
	
	return 1;
}
//-----------------------------------------------------------------------------
int mv_man_test_dram(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	MV_STATUS status;
	printf("*****Testing DRAM now**********\n");
	argv[1]="0x1FFFFFF0";
	argv[2]="0x2000000F";
	argv[3]="0xaaaa5555";
	argv[4]="2";
	printf("From Addr=%s to Addr=%s\n",argv[1],argv[2]);	
	status=do_mem_mtest(cmdtp,flag,5,argv);
	printf("******************************\n");
	if(status==MV_OK)
	{
		printf("DRAM Works!!\n");
		g_testReport|= (0x01<<MAN_FUNC_ID_DRAM);

	}
	else
		printf("DRAM Failed!!\n");			

	return 1;
}
//-----------------------------------------------------------------------------
int mv_man_test_rtc(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	MV_STATUS status;
	printf("*****Testing RTC now**********\n");
	argv[1]="091610302010"; //2010/09/16 10:30
	status=do_date(cmdtp,flag,2,argv);
	printf("******************************\n");
	if(status==MV_OK)
	{
		printf("RTC Works!!\n");
		g_testReport|= (0x01<<MAN_FUNC_ID_RTC);

	}
	else
		printf("RTC Failed!!\n");			

	return 1;
}
//-----------------------------------------------------------------------------
int mv_man_init(void)
{
	mv_man_register_func(MAN_FUNC_ID_SG,mv_man_test_sg);
#if defined(CONFIG_CMD_NAND)
	mv_man_register_func(MAN_FUNC_ID_NAND,mv_man_test_nand);
#endif
#ifdef CONFIG_CMD_SF
	mv_man_register_func(MAN_FUNC_ID_SF,mv_man_test_sf);
#endif
	mv_man_register_func(MAN_FUNC_ID_IDE,mv_man_test_sata);
	mv_man_register_func(MAN_FUNC_ID_I2C,mv_man_test_i2c);
	mv_man_register_func(MAN_FUNC_ID_USB,mv_man_test_usb);
	mv_man_register_func(MAN_FUNC_ID_PCI,mv_man_test_pcie);
	mv_man_register_func(MAN_FUNC_ID_MMC,mv_man_test_mmc);
	mv_man_register_func(MAN_FUNC_ID_DRAM,mv_man_test_dram);
	mv_man_register_func(MAN_FUNC_ID_RTC,mv_man_test_rtc);

	return 1;
}
//-----------------------------------------------------------------------------
void printTestReport(void)
{
	u32 i;
	u8  j;

	printf("*******TEST REPOET************\n" );
	if(g_testReport != 0x3ff)//not all OK ,means error occurs 
	{
		printf("Failed Items as below\n");
		printf("------------------------------\n" );
		for(i=0,j=1;i<MAX_MAN_TEST_COUNT;i++)
		{	
			if((g_testReport & (0x01<<i))==0)
			{
				printf("%2d.%sFailed\n",j,ptestItem[i]);
				j++;
			}
		}
		printf("------------------------------\n" );

	}
	if(g_testReport !=0)
	{
		printf("OK Items as below\n");
		printf("------------------------------\n" );
		for(i=0,j=1;i<MAX_MAN_TEST_COUNT;i++)
		{	
			if((g_testReport & (0x01<<i))!=0)
			{
				printf("%2d.%sOK\n",j,ptestItem[i]);
				j++;
			}
		}
	}

	printf("******************************\n" );


}
//-----------------------------------------------------------------------------
int man_test(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	u8	i=0;
	char *pdata[10];

	g_testReport=0;
	printf("------------------------------\n" );
	printf("AVD1 A0 V3.0 Manufacture Test*\n" );
	printf("------------------------------\n" );
	for(i=0;i<MAX_MAN_TEST_COUNT;i++)
		pfunc_man_test_table[i](cmdtp,flag,argc,pdata);

	printTestReport();

	return 1;
}

U_BOOT_CMD(
	man,1,1,man_test,
	"manufacture test\n",
	" \n"
	"\tmanufacture test!!\n"
);
//-----------------------------------------------------------------------------

#endif

