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

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>

#define _HOST_COMPILER
#include "mvSysHwConfig.h"
#include "bootstrap_os.h"
#include "bootstrap_hdr.h"

#ifndef O_BINARY                /* should be define'd on __WIN32__ */
#define O_BINARY        0
#endif

/*
 * Type definiions 
 */
typedef enum 
{
	IMG_SATA,
	IMG_UART,
	IMG_FLASH,
	IMG_NAND,
	IMG_PEX,
}IMG_TYPE;

/*
 * Definitions
 */
#define T_OPTION_MASK		0x1	/* image type */
#define D_OPTION_MASK		0x2	/* image destination */
#define E_OPTION_MASK		0x4	/* image execution address */
#define S_OPTION_MASK		0x8	/* image source */
#define R_OPTION_MASK		0x10    /* DRAM file */
#define I_OPTION_MASK		0x20	/* SATA PIO mode*/
#define L_OPTION_MASK		0x40	/* delay time in mseconds*/
#define F_OPTION_MASK		0x80	/* write full sector of NAND flash */
#define B_OPTION_MASK		0x100	/* append a binary header file */
#define C_OPTION_MASK		0x200	/* Cell Type for NAND flash */
#define MAX_EXT_HDR_NUMBER	32
#define HDR_SIZE		0x200
#define MAX_BIN_HDR_SIZE	(0x800 - 0x30 - 0x4) /* 2K - header - 32bit checksum */
#define MAX_BIN_HDR_NUMBER	32
#define BIN_SIZE		0x800
#define MAX_LINE_LEN		0x200

/*
 * Calculate 8-bit checksum
 */
u8 checksum8(void* start, u32 len,u8 csum)
{
    register u8 sum = csum;
	volatile u8* startp = (volatile u8*)start;

    do{
	  	sum += *startp;
		startp++;
    }while(--len);

    return (sum);
}

/*
 * Calculate 32-bit checksum
 */
u32 checksum32(void* start, u32 len, u32 csum)
{
    register u32 sum = csum;
	volatile u32* startp = (volatile u32*)start;


    do{
	  	sum += *(u32*)startp;
		startp++;
		len -= 4;
    }while(len);

    return (sum);
}

/*
 * Main routine
 */
int main(int argc,char** argv)
{
	IMG_TYPE img;
	char *image_type = NULL;
	int f_in = -1;
	int f_out = -1;
	int f_header = -1;	
	char *fname_in = NULL;
	char *fname_out = NULL;
	char *fname_hdr_out = NULL;
	char *fname_dram = NULL;
	char *fname_bin[MAX_BIN_HDR_NUMBER]= {
				NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
				NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
				NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
				NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
	unsigned int image_dest = 0;
	unsigned int image_exec = 0;
	unsigned int header_size = 0;
	unsigned int image_source = 0;
	unsigned int fname_bin_cnt = 0;
        unsigned char *ptr;
	BHR_t hdr;
	ExtBHR_t extHdr;
    	struct stat fs_stat;
	char *buf_in = NULL;
	int size_written = 0; 
	char *tmpHeader = NULL;
	unsigned int opts=0;
	unsigned int chsum32 = 0;
	unsigned int ms_delay=0;
	FILE *f_dram;
	FILE *f_bin;
	unsigned long lastDword = 0;
	int i=0;
	unsigned int bytesToAlign=0;
	int file_size = 0;
	int hdrnum = 0;
	unsigned char nand_technology = 0;
	int new_file_size = 0;
	int num_sectors = 0;
	int sector_padding_size = 0;
	int sector_size = 0;
	int sector_padding = 0;
	char dram_line_str[MAX_LINE_LEN];

	if (argc ==1)
	{
		printf("Dove BootROM Header Generation Tool version 2.33.1\n");
		printf("This version is compatible with Dove A0 and previous devices\n\n");
		printf("usage: \n");
		printf("doimage <must_options> [other_options] image_in image_out [header_out]\n");
		printf("\n<must_options> - can be one or more of the following:\n\n");
		printf("-T image_type:   sata\\uart\\flash\\bootrom\\nand\\hex\\pex\\resume\n");
		printf("                 if image_type is sata, the image_out will\n");
		printf("                 include header only.\n");
		printf("-D image_dest:   image destination in dram (in hex)\n");
		printf("-E image_exec:   execution address in dram (in hex)\n");
		printf("                 if image_type is 'flash' and image_dest is 0xffffffff\n");
		printf("                 then execution address on the flash\n");
		printf("-S image_source: if image_type is sata then the starting sector of\n");
		printf("                 the source image on the disk - mandatory for sata\n");
		printf("                 if image_type is flash\\nand then the starting offset of\n");
		printf("                 the source image at the flash - optional for flash\\nand\n");
		printf("-F sector_size:  NAND Flash sector size (in hex)\n");
		printf("-C technology:   NAND technology, SLC is default (in hex: 0x0:SLC, other:MLC)\n");
		printf("\n<other_options> - optional and can be one or more of the following:\n\n");
		printf("-R dram_file:    ascii file name that contains the list of dram regs\n");
		printf("-B bin_file:     binary file to be executed before loading the image\n");
		printf("                 This option can be repeated up to 32 times\n");
		printf("-L delay in mili seconds to replace DDR Init Done polling\n");
		printf("-I PIO mode only (valid for SATA only)\n");
		printf("\ncommand possibilities: \n\n");
		printf("doimage -T sata -S sector -D image_dest -E image_exec\n");
		printf("         [other_options] image_in image_out header_out\n\n");
		printf("doimage -T flash -D image_dest -E image_exec [-S address]\n");
		printf("         [other_options] image_in image_out\n\n");
		printf("doimage -T pex -D image_dest -E image_exec \n");
		printf("         [other_options] image_in image_out\n\n");
		printf("doimage -T nand -D image_dest -E image_exec [-S address]\n");
		printf("         [other_options] image_in image_out\n\n");
		printf("doimage -T uart -D image_dest -E image_exec\n");
		printf("         [other_options] image_in image_out\n\n");
		printf("doimage -T pex -D image_dest -E image_exec \n");
		printf("         [other_options] image_in image_out\n\n");
		printf("\n\n\n");
		exit(1);

	}

	while (--argc > 0 && **++argv == '-') 
	{
		while (*++*argv) 
		{
			switch (**argv) 
			{
			case 'T': /* image type */
				if (--argc <= 0) {fprintf(stderr, "Missing identifier after -T option!\n");exit(1);}
                                image_type = *++argv;
				if (opts & T_OPTION_MASK) {fprintf(stderr, "The -T option specified more than once!\n");exit(1);}
				opts |= T_OPTION_MASK;
                                break;
			case 'D': /* image destination  */
				if (--argc <= 0) {fprintf(stderr, "Missing identifier after -D option!\n");exit(1);}
                                image_dest = strtoul (*++argv, (char **)&ptr, 16);
				if (*ptr) {fprintf(stderr, "Invalid argument after -D option!\n");exit(1);}
				if (opts & D_OPTION_MASK) {fprintf(stderr, "The -D option specified more than once!\n");exit(1);}
				opts |= D_OPTION_MASK;
				break;
			case 'E': /* image execution  */
				if (--argc <= 0) {fprintf(stderr, "Missing identifier after -E option!\n");exit(1);}
                                image_exec = strtoul (*++argv, (char **)&ptr, 16);
				if (*ptr) {fprintf(stderr, "Invalid argument after -E option!\n");exit(1);}
				if (opts & E_OPTION_MASK) {fprintf(stderr, "The -E option specified more than once!\n");exit(1);}
				opts |= E_OPTION_MASK;
				break;
			case 'I': /* PIO */
				if (argc <= 0) {fprintf(stderr, "Missing identifier after -I option!\n");exit(1);}
				if (opts & I_OPTION_MASK) {fprintf(stderr, "The -I option specified more than once!\n");exit(1);}
				opts |= I_OPTION_MASK;
				break;
            		case 'S': /* starting sector */
				if (--argc <= 0) {fprintf(stderr, "Missing identifier after -S option!\n");exit(1);}
				image_source = strtoul (*++argv, (char **)&ptr, 16);
				if (*ptr) {fprintf(stderr, "Invalid argument after -S option!\n");exit(1);}
				if (opts & S_OPTION_MASK) {fprintf(stderr, "The -S option specified more than once!\n");exit(1);}
				opts |= S_OPTION_MASK;
				break;
			case 'L': /* Delay time */
				if (--argc <= 0) {fprintf(stderr, "Missing identifier after -L option!\n");exit(1);}
				ms_delay = strtoul (*++argv, (char **)&ptr, 10);
				if (*ptr) {fprintf(stderr, "Invalid argument after -L option!\n");exit(1);}
				if (opts & L_OPTION_MASK) {fprintf(stderr, "The -L option specified more than once!\n");exit(1);}
				opts |= L_OPTION_MASK;
				break;
            		case 'R': /* dram file*/
				if (--argc <= 0) {fprintf(stderr, "Missing identifier after -R option!\n");exit(1);}
				fname_dram = *++argv;
				if (opts & R_OPTION_MASK) {fprintf(stderr, "The -R option specified more than once!\n");exit(1);}
				opts |= R_OPTION_MASK;
				break;
			case 'F': /* write full sector size in NAND flash */
				if (--argc <= 0) {fprintf(stderr, "Missing identifier after -F option!\n");exit(1);}
				sector_size = strtoul (*++argv, (char **)&ptr, 16);
				sector_padding=1;
				if (*ptr) {fprintf(stderr, "Invalid argument after -F option!\n");exit(1);}
				if (opts & F_OPTION_MASK) {fprintf(stderr, "The -F option specified more than once!\n");exit(1);}
				opts |= F_OPTION_MASK;
				break;
			case 'C': /* Cell type used for NAND flash */
				if (--argc <= 0) {fprintf(stderr, "Missing identifier after -C option!\n");exit(1);}
				nand_technology = strtoul (*++argv, (char **)&ptr, 16);
				if (*ptr) {fprintf(stderr, "Invalid argument after -C option!\n");exit(1);}
				if (opts & C_OPTION_MASK) {fprintf(stderr, "The -C option specified more than once!\n");exit(1);}
				opts |= C_OPTION_MASK;
				break;				
			case 'B': /* binary header file */
				if (--argc <= 0) {fprintf(stderr, "Missing identifier after -B option!\n");exit(1);}
				if (fname_bin_cnt >= (MAX_BIN_HDR_NUMBER-1))
				{
					fprintf(stderr, "Error: Input exceeded MAX binary header count (%d)!\n\n\n", (MAX_BIN_HDR_NUMBER-1));
					exit(1);
				}
				fname_bin[fname_bin_cnt] = *++argv;
				fname_bin_cnt++;
				opts |= B_OPTION_MASK;
				break;
			}
		}
	}

	/* Make sure that reserved fields are 0x0 */
	memset((void*)&hdr,0,sizeof(BHR_t));

	/* Decode the boot mode */
	if (opts & T_OPTION_MASK)
	{
		if (strcmp(image_type, "sata") == 0) 
		{
			img=IMG_SATA;
			hdr.blockID = IBR_HDR_SATA_ID;
	
			/* Check that the -S option was specified */
			if ((opts & S_OPTION_MASK) == 0)
			{
				fprintf(stderr, "Missing the source address (-S option) for SATA boot!\n");
				exit(1);
			}
		}
		else if (strcmp(image_type, "nand") == 0)
		{
			img=IMG_NAND;
			hdr.blockID = IBR_HDR_NAND_ID;

			/* Check that the -F option was specified */
			if ((opts & F_OPTION_MASK) == 0)
			{
				fprintf(stderr, "Missing the sector size (-F option) for NAND boot!\n");
				exit(1);
			}
		}
		else if (strcmp(image_type, "uart") == 0)
		{
			img=IMG_UART;
			hdr.blockID = IBR_HDR_UART_ID;
		}
		else if (strcmp(image_type, "flash") == 0)
		{
			img=IMG_FLASH;
			hdr.blockID = IBR_HDR_SPI_ID;
		}
		else if (strcmp(image_type, "pex") == 0)
		{
			img=IMG_PEX;
			hdr.blockID = IBR_HDR_PEX_ID;
		}
		else 
		{
			fprintf(stderr, "Unknown image type (check the -T option)!\n");
			exit(1);
		}
	}
	else
	{
		fprintf(stderr, "Image type unspecified (-T option missing)!\n");
		exit(1);
	}

	/* Check for the destination address */
	if ((opts & D_OPTION_MASK) == 0)
	{
		fprintf(stderr, "Destination address unspecified (-D option missing)!\n");
		exit(1);
	}

	/* Check for the execution address */
	if ((opts & E_OPTION_MASK) == 0)
	{
		fprintf(stderr, "Execution address unspecified (-E option missing)!\n");
		exit(1);
	}

	hdr.destinationAddr = image_dest;
	hdr.executionAddr = (pFunction)image_exec;
	hdr.nandBlockSize = (sector_size / (0x10000));
	hdr.nandTechnology = nand_technology;
	
	/* Add DMA/PIO mode for Sata if specified */
	if ((img == IMG_SATA) && (opts & I_OPTION_MASK))
		hdr.sataPioMode = 1;

	/* Read file names based on image type */
	if (img == IMG_SATA)
	{
		if (argc == 3)
		{
			fname_in = *argv++;
			fname_out = *argv++;
			fname_hdr_out = *argv++;
		}
		else
		{
			fprintf(stderr, "Missing input/output file names in Sata boot mode!\n");
			exit(1);
		}
	}
	else
	{	if (argc == 2)
		{
			fname_in = *argv++;
			fname_out = *argv++;
		}
		else
		{
			fprintf(stderr, "Missing input/output file names!\n");
			exit(1);
		}
	}

	/* open input image */
	if ((f_in = open(fname_in,O_RDONLY|O_BINARY)) == -1)
	{
		fprintf(stderr, "Failed to open input image file (%s)!\n", fname_in);
		exit(0);
	}

	/* get the size of the input image */
	if (fstat(f_in, &fs_stat) != 0)
	{
		close(f_in);
		fprintf(stderr, "Failed to get the input file (%s) information!\n", fname_in);
		exit(1);
	}

	/* map the input image */
	buf_in = mmap(0, fs_stat.st_size, PROT_READ, MAP_SHARED, f_in, 0);
	if (!buf_in)
	{
		fprintf(stderr, "Failed to map input file (%s) to memory\n", fname_in);
		goto end;
	}
	
	/* open the output image */
	if ((f_out = open(fname_out,O_RDWR|O_TRUNC|O_CREAT|O_BINARY,0666)) == -1)
	{
		fprintf(stderr, "Failed to open image output file (%s)!\n", fname_out);
    	}

	/* Sata boot needs 2 output files, image and header */
	if (img == IMG_SATA)
	{
		if ((f_header = open(fname_hdr_out, O_RDWR|O_TRUNC|O_CREAT|O_BINARY, 0666)) == -1)
		{
			fprintf(stderr, "Error openning header output file (%s)!\n", fname_hdr_out);
		}
	}

	/*********************/
	/* Build Header Part */
	/*********************/

	/* Loadable image size */
	hdr.blockSize = fs_stat.st_size + 4; /* binary size + 4B for 32bit checksum */

	/* Align size to 4 byte*/
	if (hdr.blockSize & 0x3)
	{
		printf("hdr.blockSize = 0x%x fs_stat.st_size = 0x%x\n", hdr.blockSize, fs_stat.st_size);
		bytesToAlign = (4 - (hdr.blockSize & 0x3));
		hdr.blockSize += bytesToAlign;
	}

	/* Create a temporary buffer to hold the headers */	
	tmpHeader = malloc((MAX_EXT_HDR_NUMBER * HDR_SIZE) + /* a maximum of 32 headers of 512B each */
			   (MAX_BIN_HDR_NUMBER * BIN_SIZE)); /* a maximum of 32 bin headers od 2KB each */
	memset(tmpHeader, 0 , ((MAX_EXT_HDR_NUMBER * HDR_SIZE) + (MAX_BIN_HDR_NUMBER * BIN_SIZE)));

        /* Read IN Register Header extension if exists */
	if (opts & R_OPTION_MASK)
	{
		int i;
		int fileend=0;
		unsigned int sar_addr, sar_mask, sar_val, ddr3_wl_cs, mreset_mpp, cke_mpp, wa_delay;
		char	sar_str[256];
		unsigned int dram_reg[(512>>2)];

         	if ((f_dram = fopen(fname_dram, "r")) == NULL)
		{
			fprintf(stderr, "Failed to open DRAM registers file (%s)!\n", fname_dram);
			exit(1);
		}

		/* loop on the file and scan ddr parameters */
		for (hdrnum=0; hdrnum < MAX_EXT_HDR_NUMBER; hdrnum++)
		{
			if (fgets(dram_line_str, MAX_LINE_LEN, f_dram) == NULL)
				break;

			if (dram_line_str[0] == '#') {
				hdrnum --;
				continue;
			}
	
			if (sscanf(dram_line_str,"%s %x %x %x %x %x %x %x\n",sar_str, &sar_addr, &sar_mask, &sar_val,\
								 &ddr3_wl_cs, &mreset_mpp, &cke_mpp, &wa_delay) == EOF) {
					printf("Error reading from string\n");
					exit(1);

			}

			if (strncmp(sar_str, "SAR", 3) != 0)
			{
				fprintf(stderr,"Register File does not start with SAR header\n");
				fclose(f_dram);
				goto end;
			}

			header_size += sizeof(BHR_t);	/* reserve space for main header in each 512 Bytes */
			
			for (i=0; i<111; i+=2)
			{
				if (fgets(dram_line_str, MAX_LINE_LEN, f_dram) == NULL) {
					fileend = 1;
					break;
				}

				if (dram_line_str[0] == '#') {
					i -=2;
					continue;
				}

               			if (sscanf(dram_line_str, "%x %x\n",&dram_reg[i], &dram_reg[i+1]) == EOF)
				{
					printf("Error reading from string\n");
					exit(1);
				}
				else if((dram_reg[i] == 0x0) && (dram_reg[i+1] == 0x0))
					break;
			}
			if (i==111)
			{
				fprintf(stderr,"Registers per header exceed max number of 55 couples\n");
				fclose(f_dram);
				exit(1);
			}
					
			memset((void*)&extHdr, 0, sizeof(ExtBHR_t));
			extHdr.dramRegsOffs = (sizeof(BHR_t) + sizeof(ExtBHR_t));
			extHdr.ddr_init_delay = ms_delay;
			extHdr.sar_addr = sar_addr;
			extHdr.sar_mask = sar_mask;
			extHdr.sar_val = sar_val;
			extHdr.ddr_wl_type = ddr3_wl_cs;
			extHdr.mreset_mpp = mreset_mpp;
			extHdr.cke_mpp = cke_mpp;
			extHdr.wa_delay = wa_delay;
				
			memcpy(tmpHeader + header_size, &extHdr, sizeof(ExtBHR_t));
			header_size +=sizeof(ExtBHR_t);

			memcpy(tmpHeader + header_size, dram_reg, (i*4) );
			header_size += (HEADER_SIZE - (sizeof(BHR_t) + sizeof(ExtBHR_t)));
			*(MV_U8*)(tmpHeader + header_size  - 1) = \
				checksum8((void*)(tmpHeader + (HEADER_SIZE * hdrnum) + sizeof(BHR_t)), (HEADER_SIZE - sizeof(BHR_t)), 0);
			if (fileend)
				break;
		}
		fclose(f_dram);
	}

	/* If no extended header was needed switch to min values per image type */
	if (header_size == 0)
	{
		if (img == IMG_SATA)
			header_size = 512; 	/* sector size */
		else if (img == IMG_UART)
			header_size = 128;	/* Xmodem packet size */
		else if ((img == IMG_FLASH) || (img == IMG_NAND) || (img == IMG_PEX))
			header_size = sizeof(BHR_t);
	}

	/* Read IN binary extension if exists */
	if (opts & B_OPTION_MASK)
	{
		for (i=0; i<fname_bin_cnt; i++)
		{
			if (fname_bin[i]) 
			{
				int cnt;
				binHDR_t *bin_hdr;

       	        		if ((f_bin = fopen(fname_bin[i], "r")) == NULL)
				{
					fprintf(stderr,"Failed to open binary header file (%s)!\n", fname_bin[i]);
					fclose(f_bin);
					goto end;
				}

				cnt = fread((tmpHeader + header_size), 1, BIN_SIZE, f_bin);
				if (cnt > (BIN_SIZE-4))
				{
					printf("ERROR: Binary header file '%s' exceeds max size %d\n", fname_bin, (BIN_SIZE-4));	
					printf("       Truncating....\n");
					cnt = (BIN_SIZE-4); /* leave 32bit for checksum */
				}
					
				if (cnt == 0)
					printf("Warning: It seems that the file '%s' is empty (0 bytes read)!\n", fname_bin);	
					

				fclose(f_bin);

				/* allign to 32 bit */
				if (cnt & 0x3)
					cnt = (cnt & 0xFFFFFFFC) + 0x4;				

				bin_hdr = (binHDR_t*)(tmpHeader + header_size);

				/* update the binary size in the header */
				bin_hdr->size = (cnt - 0x30);
	
				/*printf("Binary Header #%d: %08x bytes (offs=%08x, dst=%08x, exec=%08x, size=%08x)\n",
							 i, cnt, bin_hdr->src, bin_hdr->dest, bin_hdr->exec, bin_hdr->size);*/

				/* calculate the header checksum 8 */
				*(MV_U8*)(tmpHeader + header_size  + BINARY_HDR_SIZE - 1) = \
					checksum8((void*)(tmpHeader + header_size) , (BINARY_HDR_SIZE - 1), 0);
	
				/* calcualte the binary code checksum 32 */
				*(MV_32*)(tmpHeader + header_size + cnt) = \
					checksum32((void*)(tmpHeader + header_size + bin_hdr->src), bin_hdr->size, 0);
	
				header_size += BIN_SIZE;
			}
			else
			{
				fprintf(stderr,"Error finding binary header file #%d\n", (i+1));
				goto end;
			}
		}
	}

	/* Check for source address location */	
	if (img == IMG_SATA)
	{
		hdr.sourceAddr = image_source;	/* Sata is a must */
	}
	else if (image_source)
	{
		if (image_source < header_size)
		{
			fprintf(stderr, "Error: Source Address is less than header size!\n");
			goto end;
		}
		
		/* Override source address */
		hdr.sourceAddr = image_source;
	}
	else
	{
		hdr.sourceAddr = header_size;
	}

	/* main header checksum delayed to this stage to decide source address headers number */			
	printf("Register headers number = %d\n", hdrnum);
	printf("Binary headers number = %d\n", fname_bin_cnt);
	hdr.ext = hdrnum;
	hdr.bin = fname_bin_cnt;
	hdr.checkSum = checksum8((void*)&hdr, sizeof(BHR_t) ,0);
	memcpy(tmpHeader, &hdr, sizeof(BHR_t));


	if (img == IMG_SATA)
	{
		/* copy header to output image */
		size_written = write(f_header, tmpHeader, header_size);
		if (size_written != header_size)
		{
			fprintf(stderr,"Error writing %s file \n",fname_hdr_out);
			goto end;
		}

		fprintf(stdout, "%s was created \n", fname_hdr_out);
	}
	else
	{
		/* copy header to output image */
		size_written = write(f_out, tmpHeader, header_size);
		if (size_written != header_size)
		{
			fprintf(stderr,"Error writing %s file \n",fname_out);
			goto end;
		}
	}

	free(tmpHeader);

	/********************/
	/* Build Image Part */
	/********************/

	/* Calculate checksum */
	chsum32 = checksum32((void*)buf_in, (u32)((u32)fs_stat.st_size - bytesToAlign), chsum32);
	if (bytesToAlign)
	{
		memcpy(&lastDword, (buf_in + (fs_stat.st_size - bytesToAlign)) , bytesToAlign);
	}
	chsum32 = checksum32((void*)&lastDword, 4,chsum32);

	/* copy input image to output image */
	size_written = write(f_out, buf_in, fs_stat.st_size);
	if (size_written != fs_stat.st_size)
	{
		fprintf(stderr,"Error writing binary image to %s file \n",f_out);
		goto end;
	}
	new_file_size = size_written;

	if (bytesToAlign)
	{
		size_written = write(f_out, &lastDword, bytesToAlign);
		if (size_written != bytesToAlign)
		{
			fprintf(stderr,"Error writing binary allignment to %s file \n",f_out);
			goto end;
		}
		new_file_size += size_written;
	}

	/* write checksum */
	size_written = write(f_out, &chsum32, sizeof(chsum32));
	if (size_written != sizeof(chsum32))
	{
		fprintf(stderr,"Error writing checksum allignment to %s file \n",f_out);
		goto end;
	}
	new_file_size += size_written;

	/* pad with 0xFF till the end of the NAND sector */
	if ((sector_padding)&&(sector_size))
	{
		file_size = (new_file_size + header_size);
		num_sectors = file_size / sector_size;
		if (file_size % sector_size)
			num_sectors++;

		sector_padding_size = ((num_sectors*sector_size) - file_size);

		if (sector_padding_size > 0)
		{
			char *padding = NULL;
			padding = malloc(sector_padding_size);
			if (padding)
			{
				memset((void*)padding, 0xFF, sector_padding_size);
				size_written = write(f_out, padding, sector_padding_size);
				if (size_written != sector_padding_size)
				{
					fprintf(stderr,"Error writing NAND sector padding to %s file \n",f_out);
					goto end;
				}
				free(padding);
			}
			else
			{
				fprintf(stderr, "error allocating padding\n");
				goto end;
			}
		}
	}
	
	fprintf(stdout, "%s was created \n", fname_out);

end:
	/* close handles */
	close(f_out);
	if (img == IMG_SATA)
		close(f_header);
    
	munmap((void*)buf_in, fs_stat.st_size);
	close(f_in);

	return 0;
}

