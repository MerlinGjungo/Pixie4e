# Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com

# TODO: Set WD_BASEDIR to the location of your WinDriver installation directory.
#       For example: WD_BASEDIR=/home/john/WinDriver

# Make sure that WD_BASEDIR points to the right place
ifeq ($(WD_BASEDIR),)
        WD_BASEDIR=../../../
endif

WD_BASEDIR:=$(wildcard $(WD_BASEDIR))

ifeq ($(wildcard $(WD_BASEDIR)/include/windrvr.h),)
        $(error Please edit the makefile and set the WD_BASEDIR variable \
        to point to the location of your WinDriver installation directory)
endif

# Comment/uncomment to enable/disable debugging code
# DEBUG = 1

ifeq ($(DEBUG),1)
    DEBFLAGS = -g -O -DDEBUG
else
    DEBFLAGS = -O2
endif

ifndef TARGET_CPU
	TARGET_CPU=$(shell uname -m | sed 's/i.86/i386/' | sed 's/arm.*/ARM/')
endif
ifeq ("$(TARGET_CPU)", "x86_64")
	CFLAGS += -DKERNEL_64BIT
	ifndef USER_BITS
		USER_BITS = 64
	endif
	CFLAGS += -m$(USER_BITS)
	LFLAGS += -m$(USER_BITS)
endif
ifeq ("$(TARGET_CPU)", "ARM")
	CFLAGS += -DARM
endif

CFLAGS += -DLINUX $(DEBFLAGS) -Wall -I$(WD_BASEDIR)/include -I$(WD_BASEDIR)
CFLAGS += -fpic -Dx86_64 -DWD_DRIVER_NAME_CHANGE -Wno-unused-result -Wno-write-strings
LFLAGS += -fpic -shared
# Need also thread support for interrupt handling
LFLAGS += -lpthread

TARGET = libwdapi1281.so
EXPORT_SO_FILE = $(WD_BASEDIR)/lib/libwdapi1281.so
SRCS = ../windrvr.c ../utils.c ../status_strings.c ../wdu_lib.c ../windrvr_events.c ../windrvr_int_thread.c ../wd_log.c ../wdc_err.c ../wdc_general.c ../wdc_cfg.c ../wdc_mem_io.c ../wdc_dma.c ../wdc_ints.c ../wds_ipc.c ../wdc_events.c ../wds_kerbuf.c ../wdc_sriov.c ../pci_strings.c

LD = gcc

OD = ./
OBJS = $(addsuffix .o, $(addprefix $(OD)/, $(basename $(notdir $(SRCS)))))


all : $(TARGET)

copy_lib : $(EXPORT_SO_FILE)

$(TARGET) : $(OBJS)
	$(LD) -o $@ $(OBJS) $(LFLAGS) $(ADDITIONAL_LIBS)

$(EXPORT_SO_FILE) : $(TARGET)
	cp $< $@

windrvr.o : ../windrvr.c
	$(CC) -c $(CFLAGS) -o $@ $<

utils.o : ../utils.c
	$(CC) -c $(CFLAGS) -o $@ $<

status_strings.o : ../status_strings.c
	$(CC) -c $(CFLAGS) -o $@ $<

wdu_lib.o : ../wdu_lib.c
	$(CC) -c $(CFLAGS) -o $@ $<

windrvr_events.o : ../windrvr_events.c
	$(CC) -c $(CFLAGS) -o $@ $<

windrvr_int_thread.o : ../windrvr_int_thread.c
	$(CC) -c $(CFLAGS) -o $@ $<

wd_log.o : ../wd_log.c
	$(CC) -c $(CFLAGS) -o $@ $<

wdc_err.o : ../wdc_err.c
	$(CC) -c $(CFLAGS) -o $@ $<

wdc_general.o : ../wdc_general.c
	$(CC) -c $(CFLAGS) -o $@ $<

wdc_cfg.o : ../wdc_cfg.c
	$(CC) -c $(CFLAGS) -o $@ $<

wdc_mem_io.o : ../wdc_mem_io.c
	$(CC) -c $(CFLAGS) -o $@ $<

wdc_dma.o : ../wdc_dma.c
	$(CC) -c $(CFLAGS) -o $@ $<

wdc_ints.o : ../wdc_ints.c
	$(CC) -c $(CFLAGS) -o $@ $<

wds_ipc.o : ../wds_ipc.c
	$(CC) -c $(CFLAGS) -o $@ $<

wdc_events.o : ../wdc_events.c
	$(CC) -c $(CFLAGS) -o $@ $<

wds_kerbuf.o : ../wds_kerbuf.c
	$(CC) -c $(CFLAGS) -o $@ $<

wdc_sriov.o : ../wdc_sriov.c
	$(CC) -c $(CFLAGS) -o $@ $<

pci_strings.o : ../pci_strings.c
	$(CC) -c $(CFLAGS) -o $@ $<


clean :
	rm -f *~ core $(OBJS) $(TARGET) $(ARCH)

