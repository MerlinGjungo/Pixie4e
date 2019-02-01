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
CFLAGS += -Dx86_64 -DWD_DRIVER_NAME_CHANGE -Wno-unused-result -Wno-write-strings
# Need also thread support for interrupt handling
LFLAGS += -lpthread

TARGET = i8253
SRCS = ../i8253.c ../../../src/wdapi/windrvr.c ../../../src/wdapi/utils.c

LD = gcc

OD = ./
OBJS = $(addsuffix .o, $(addprefix $(OD)/, $(basename $(notdir $(SRCS)))))


all : $(TARGET)

$(TARGET) : $(OBJS)
	$(LD) -o $@ $(OBJS) $(LFLAGS) $(ADDITIONAL_LIBS)

i8253.o : ../i8253.c
	$(CC) -c $(CFLAGS) -o $@ $<

windrvr.o : ../../../src/wdapi/windrvr.c
	$(CC) -c $(CFLAGS) -o $@ $<

utils.o : ../../../src/wdapi/utils.c
	$(CC) -c $(CFLAGS) -o $@ $<


clean :
	rm -f *~ core $(OBJS) $(TARGET) $(ARCH)

