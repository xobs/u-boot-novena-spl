STRIP = strip

#
# Include the make variables (CC, etc...)
#
LD      = ld
CC      = gcc
OBJCOPY = objcopy
OBJDUMP = objdump
RM = rm -f
CD = cd
CP = cp


#CFLAGS  = -I./ -DMV_CPU_LE -DMV_TEST_PLATFORM -m64
CFLAGS  = -I./ -DMV_CPU_LE
LDFLAGS =

BIN     = doimage

all:	$(BIN)
doimage:      doimage.o
		$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
		$(STRIP) $@
		$(CP) doimage ../

doimage.o:	doimage.c
		$(CC) -g $(CFLAGS) -c $<

clean:	
	find . -type f \
                \( -name 'core' -o -name '*.bak' -o -name '*~' \
                -o -name '*.o'  -o -name '*.a'  \) -print \
                | xargs rm -f
	$(RM) $(ELF) $(SREC) $(BIN) $(DIS) bootstrap.map
