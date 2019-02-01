# Template file for configure script

LINUX_USB_SUPPORT=yes
LINUX_SRIOV_SUPPORT=no

MAKE_WD = make -f makefile.wd.kbuild

ifeq ($(LINUX_USB_SUPPORT), yes)
    MAKE_USB = make -f makefile.usb.kbuild
    MAKE_USB_CLEAN = $(MAKE_USB) clean
endif

ifeq ($(LINUX_SRIOV_SUPPORT), yes)
    MAKE_SRIOV = make -f makefile.sriov.kbuild
    MAKE_SRIOV_CLEAN = $(MAKE_SRIOV) clean
endif

all:
	$(MAKE_USB)
	$(MAKE_SRIOV)
	$(MAKE_WD)

install: all
	$(MAKE_WD) install

clean:
	$(MAKE_USB_CLEAN)
	$(MAKE_SRIOV_CLEAN)
	$(MAKE_WD) clean
