include mvRules.mk

# Objects list

COMMON_OBJS = $(COMMON_DIR)/mvCommon.o

OSSERVICES_OBJS = $(OSSERVICES_DIR)/mvOs.o $(OSSERVICES_DIR)/mvOsSata.o

BOARD_OBJS = $(BOARD_ENV_DIR)/mvBoardEnvLib.o $(BOARD_ENV_DIR)/mvBoardEnvSpec.o

##$(HAL_SYS_DIR)/mvSysSFlash.o
##$(HAL_SYS_DIR)/mvSysAudio.o $(HAL_SYS_DIR)/mvSysCamController.o		

HAL_SYS_OBJS =	$(HAL_SYS_DIR)/mvSysCesa.o $(HAL_SYS_DIR)/mvSysDdr.o			\
				$(HAL_SYS_DIR)/mvSysEth.o $(HAL_SYS_DIR)/mvSysEthPhy.o			\
				$(HAL_SYS_DIR)/mvSysPciIf.o $(HAL_SYS_DIR)/mvSysPdma.o			\
				$(HAL_SYS_DIR)/mvSysSata.o $(HAL_SYS_DIR)/mvSysAudio.o			\
				$(HAL_SYS_DIR)/mvSysSpi.o $(HAL_SYS_DIR)/mvSysUsb.o				\
				$(HAL_SYS_DIR)/mvSysXor.o

##	$(HAL_AC97_DIR)/mvAc97.o						
##		$(HAL_AUDIO_DIR)/mvAudio.o $(HAL_AUDIO_DIR)/dac/mvCLAudioCodec.o	
##		$(HAL_AUDIO_DIR)/mvAudioAddrDec.o					
##		$(HAL_CAMERA_DIR)/mvCamController.o $(HAL_CAMERA_DIR)/mvCamAddrDec.o	
##		$(HAL_CAMERA_DIR)/OV7680CamSensor.o					

HAL_OBJS =	$(HAL_AC97_DIR)/mvAc97.o $(HAL_AUDIO_DIR)/mvAudio.o				\
			$(HAL_AUDIO_DIR)/dac/mvCLAudioCodec.o							\
			$(HAL_AUDIO_DIR)/mvAudioAddrDec.o								\
			$(HAL_CESA_DIR)/mvCesa.o $(HAL_CESA_DIR)/mvLru.o				\
			$(HAL_CESA_DIR)/mvMD5.o $(HAL_CESA_DIR)/mvSHA1.o				\
			$(HAL_CESA_DIR)/mvCesaDebug.o $(HAL_CESA_DIR)/AES/mvAesAlg.o	\
			$(HAL_CESA_DIR)/AES/mvAesApi.o $(HAL_CESA_DIR)/mvCesaAddrDec.o	\
			$(HAL_CNTMR_DIR)/mvCntmr.o										\
			$(HAL_DRAM_DIR)/spd/mvSpd.o $(HAL_DRAM_DIR)/ddrmc/mvDramTmng.o	\
			$(HAL_DRAM_DIR)/ddrmc/mvDramIf.o								\
			$(HAL_ETH_GBE_DIR)/mvEth.o $(HAL_ETH_GBE_DIR)/mvEthDebug.o		\
			$(HAL_ETH_GBE_DIR)/mvEthAddrDec.o								\
			$(HAL_ETHPHY_DIR)/mvEthPhy.o									\
			$(HAL_GPP_DIR)/mvGpp.o											\
			$(HAL_NFC_DIR)/mvNfc.o											\
			$(HAL_PCIIF_DIR)/mvPciIf.o $(HAL_PEX_DIR)/mvPex.o				\
			$(HAL_PCIIF_DIR)/pci_util/mvPciUtils.o							\
			$(HAL_PEX_DIR)/mvPexAddrDec.o									\
			$(HAL_PDMA_DIR)/mvPdma.o $(HAL_PDMA_DIR)/mvPdmaAddrDec.o		\
			$(HAL_RTC_DIR)/mvRtc.o											\
			$(HAL_SPI_DIR)/mvSpi.o $(HAL_SPI_DIR)/mvSpiCmnd.o				\
			$(HAL_TWSI_DIR)/mvTwsi.o $(HAL_TWSI_DIR)/soc/mvTwsiSoc.o		\
			$(HAL_TWSI_DIR)/camera/mvTwsiCamera.o							\
			$(HAL_UART_DIR)/mvUart.o										\
			$(HAL_USB_DIR)/mvUsb.o $(HAL_USB_DIR)/mvUsbAddrDec.o			\
			$(HAL_XOR_DIR)/mvXor.o $(HAL_XOR_DIR)/mvXorAddrDec.o			\
			$(HAL_SATA_CORE_DIR)/mvSata.o 									\
			$(HAL_SATA_CORE_DIR)/mvStorageDev.o								\
			$(HAL_SATA_CORE_DIR)/mvSataAddrDec.o							\
			$(HAL_SATA_CORE_DIR)/mvLog.o

##		$(HAL_SFLASH_DIR)/mvSFlash.o						\
##		$(HAL_FLASH_DIR)/mvFlash.o $(HAL_FLASH_DIR)/mvAmdFlash.o		\
##		$(HAL_FLASH_DIR)/mvIntelFlash.o $(HAL_FLASH_DIR)/mvFlashCom.o
##		$(HAL_MFLASH_DIR)/mvMFlash.o
##                $(HAL_MFLASH_DIR)/mvSMFlash.o $(HAL_MFLASH_DIR)/mvPMFlash.o

SOC_OBJS = 	$(SOC_ENV_DIR)/mvCtrlEnvAddrDec.o $(SOC_ENV_DIR)/mvCtrlEnvLib.o	\
			$(SOC_SYS_DIR)/mvAhbToMbus.o $(SOC_SYS_DIR)/mvCpuIf.o			\
			$(SOC_CPU_DIR)/mvCpu.o $(SOC_DEVICE_DIR)/mvDevice.o				\
			$(SOC_DIR)/mv_main.o $(SOC_DIR)/mv_cmd.o $(SOC_DIR)/mv_dram.o	\
			$(SOC_DIR)/mv_service.o $(SOC_DIR)/mv_dvs.o

##ifneq ($(MV_TINY_IMAGE), y) 
##SOC_OBJS     += $(HAL_PCIIF_DIR)/pci_util/mvPciUtils.o
##endif $(USP_DIR)/cmd_rcvr
## $(USP_DIR)/mv_nfc_nand.o

USP_OBJS =	$(USP_DIR)/mv_serial.o $(USP_DIR)/orion_nfc_hal.o			\
			$(USP_DIR)/mv_pageTable.o $(USP_DIR)/mv_rtc.o				\
			$(USP_DIR)/mv_mon_init.o									\
			$(USP_DIR)/mv_pci.o $(USP_DIR)/mv_flash.o					\
			$(USP_DIR)/mv_ide.o $(USP_DIR)/mv_egiga.o					\
			$(USP_DIR)/mv_fs.o $(USP_DIR)/mv_loadnet.o					\
			$(USP_DIR)/mv_protectionUnit.o								\
			$(USP_DIR)/mv_i2c.o $(USP_DIR)/mv_ext2_boot.o				\
			$(USP_DIR)/ethSwitch/mv_switch.o $(USP_DIR)/mv_tsc2005.o	\
			$(USP_DIR)/mv_sdhci.o $(USP_DIR)/mv_spi.o					\
			$(USP_DIR)/mv_usb.o

##ifeq ($(NAND_BOOT), y)
##USP_OBJS        += $(USP_DIR)/nBootloader.o
##endif
 
SOBJS = ../mv_hal/twsi/soc/mvTwsiSocEeprom.o				\
		../mv_hal/ddr/ddrmc/mvDramIfBasicInit.o				\
		../mv_hal/ddr/ddrmc/mvDramIfConfig.o				\
		../mv_dove/dove_family/ctrlEnv/sys/mvCpuIfInit.o	\
		../mv_dove/platform.o

SOBJS	+=         ../USP/jump.o

ifeq ($(NAND_BOOT), y)
SOBJS        += ../mv_dove/nBootstrap.o
endif

OBJS = $(COMMON_OBJS) $(OSSERVICES_OBJS) $(HAL_OBJS) $(HAL_SYS_OBJS) $(SOC_OBJS) $(BOARD_OBJS) $(USP_OBJS)

LIB  = lib$(BOARD).a

all:    $(LIB)

$(LIB): .depend $(SOBJS) $(OBJS)
	$(AR) crv $@ $(SOBJS) $(OBJS)

#########################################################################

.depend:        Makefile $(SOBJS:.o=.S) $(OBJS:.o=.c) $(USB_OBJS:.o=.c)
	$(CC) -M $(CFLAGS) $(SOBJS:.o=.S) $(OBJS:.o=.c) $(USB_OBJS:.o=.c) > $@

sinclude .depend

#########################################################################
