#
# Copyright (C) 2009 Marvell Semiconductors. All rights reserved.
# 

#CROSS_COMPILE=arm-marvell-linux-gnueabi-
TEXT_BASE = 0xC8010000


#
# Include the make variables (CC, etc...)
#
LD      = $(CROSS_COMPILE)ld
CC      = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
CP = cp

CFLAGS	+= -Os -g -DMV_CPU_LE
AFLAGS  := -Os -g -march=armv5t  
AFLAGS  += -I./
LDFLAGS := -EL -Bstatic -Ttext $(TEXT_BASE)

OBJS	+= binary_hdr.o ddr3_wl2cs.o

SREC	= binary_hdr.srec
BIN	= binary_hdr.bin 
ELF	= binary_hdr.elf
DIS	= binary_hdr.dis
FINAL	= binary_hdr.bin

       
all:	$(ELF) $(SREC) $(BIN) $(DIS) 
		$(CP) $(FINAL) ../

binary_hdr.elf: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -Map binary_hdr.map -o binary_hdr.elf

%.o: %.S %.c
	$(CC) $(AFLAGS) -c -o $@ $<

%.srec: %.elf
	$(OBJCOPY) -O srec $< $@ 2>/dev/null

%.bin:  %.elf
	$(OBJCOPY) -O binary $< $@ 2>/dev/null

%.dis:  %.elf
	$(OBJDUMP) -lhaD $<  >$@

clean:	
	find . -type f \
                \( -name 'core' -o -name '*.bak' -o -name '*~' \
                -o -name '*.o'  -o -name '*.a'  \) -print \
                | xargs rm -f
	rm -f $(ELF) $(SREC) $(BIN) $(DIS) $(FINAL) binary_hdr.map

help:
	@echo ""
	@echo "make"
	@echo ""
