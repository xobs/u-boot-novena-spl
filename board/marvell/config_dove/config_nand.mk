
BOOT_TEXT_BASE = 0x00020000

NAND_LDSCRIPT = ./board/$(BOARDDIR)/u-boot-nand.lds

NAND_LDFLAGS += -Bdynamic -T $(NAND_LDSCRIPT) -Ttext $(BOOT_TEXT_BASE) $(PLATFORM_LDFLAGS)
NAND_OBJS        = ./board/marvell/mv_dove/nBootstrap.o
NAND_OBJS +=   ./board/marvell/USP/nBootloader.o
