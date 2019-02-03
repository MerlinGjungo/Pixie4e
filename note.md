<!-- note.md --- 
;; 
;; Description: 
;; Author: Hongyi Wu(吴鸿毅)
;; Email: wuhongyi@qq.com 
;; Created: 日 2月  3 12:57:06 2019 (+0800)
;; Last-Updated: 日 2月  3 13:36:25 2019 (+0800)
;;           By: Hongyi Wu(吴鸿毅)
;;     Update #: 5
;; URL: http://wuhongyi.cn -->

# note

## boot

*DEBUG* Pixie_Boot_System: starting ...
*INFO* (Pixie_Scan_Crate_Slots): Host computer speed (ns per cycle) = 2.000
*INFO* (Pixie_Scan_Crate_Slots): Maximum Number of Modules in a Crate = 7
*INFO* (Pixie_Scan_Crate_Slots_GN):PCI bus is EXPRESS type
*INFO* (Pixie_Scan_Crate_Slots_GN): will set device 0 to serial number 231
*DEBUG* (Pixie_Scan_Crate_Slots_GN): found card item BUS
*DEBUG* (Pixie_Scan_Crate_Slots_GN): found card item MEMORY
*DEBUG* (Pixie_Scan_Crate_Slots_GN): found card item MEMORY
*DEBUG* (Pixie_Scan_Crate_Slots_GN): found card item MEMORY
*DEBUG* (Pixie_Scan_Crate_Slots_GN): found card item INTERRUPT
*INFO* (Pixie_Scan_Crate_Slots_GN): Gennum device 0 on Bus=9
*INFO* (Pixie_Scan_Crate_Slots_GN): Device 0, Board version= 0xA551, S/N = 231
*INFO* (Pixie_Scan_Crate_Slots_GN): UPDATED Device 0, Board version= 0xA551, S/N = 231
*INFO* (Pixie_Scan_Crate_Slots_GN) Finished scanning all modules
*INFO* (Load_U16): finished loading file Firmware/P4e_16_125_vdo.bin
*INFO* (Load_U16): finished loading file Firmware/P4e_14_500_vdo.bin
*INFO* (Load_U16): finished loading file Firmware/p500e_zdt.bin
*INFO* (Load_U16): finished loading file DSP/p500e.ldr
*INFO* (Load_U16): finished loading file Configuration/default_125.set
*DEBUG* read DSP parameter values done from Configuration/default_125.set
*DEBUG* init DSP variable names done
*ERROR* (Find_Xact_Match): RUNTASK was not found
*ERROR* (Find_Xact_Match): CONTROLTASK was not found
*ERROR* (Find_Xact_Match): RESUME was not found
*ERROR* (Find_Xact_Match): RUNTIMEA was not found
*ERROR* (Find_Xact_Match): NUMEVENTSA was not found
*ERROR* (Find_Xact_Match): SYNCHDONE was not found
*ERROR* (Find_Xact_Match): FASTPEAKSA0 was not found
*ERROR* (Find_Xact_Match): COUNTTIMEA0 was not found
*ERROR* (Find_Xact_Match): FASTPEAKSA1 was not found
*ERROR* (Find_Xact_Match): COUNTTIMEA1 was not found
*ERROR* (Find_Xact_Match): FASTPEAKSA2 was not found
*ERROR* (Find_Xact_Match): COUNTTIMEA2 was not found
*ERROR* (Find_Xact_Match): FASTPEAKSA3 was not found
*ERROR* (Find_Xact_Match): COUNTTIMEA3 was not found
*ERROR* (Find_Xact_Match): FILTERRANGE was not found
*ERROR* (Find_Xact_Match): CCONTROL was not found
*INFO* (Pixie_Boot): Revision 0xA551
*INFO* (PIXIE4E_ProgramFPGA(): BEGIN
*INFO* (PIXIE500E_ProgramFPGA): Configuration done.
*INFO* (PIXIE4E_ProgramFPGA(): END
Module 0 in slot 231 started up successfully!
... *INFO* (Main) Boot Pixie-4 succeeded

----

*DEBUG* Pixie_Boot_System: starting ...
*INFO* (Pixie_Scan_Crate_Slots): Host computer speed (ns per cycle) = 2.000
*INFO* (Pixie_Scan_Crate_Slots): Maximum Number of Modules in a Crate = 7
*INFO* (Pixie_Scan_Crate_Slots_GN):PCI bus is EXPRESS type
*INFO* (Pixie_Scan_Crate_Slots_GN): will set device 0 to serial number 231
*DEBUG* (Pixie_Scan_Crate_Slots_GN): found card item BUS
*DEBUG* (Pixie_Scan_Crate_Slots_GN): found card item MEMORY
*DEBUG* (Pixie_Scan_Crate_Slots_GN): found card item MEMORY
*DEBUG* (Pixie_Scan_Crate_Slots_GN): found card item MEMORY
*DEBUG* (Pixie_Scan_Crate_Slots_GN): found card item INTERRUPT
Failed opening a WDC device handle. Error 0x2000000e - Resource overlap

*ERROR* (Pixie_Scan_Crate_Slots_GN): DeviceOpen, failed opening a handle to the device. Power cycle of module required
*ERROR* (Pixie_Boot_System): Scanning crate slots unsuccessful, error -5.
Boot Pixie-4 failed, retval=-1





## list

*INFO* (Pixie_Scan_Crate_Slots_GN):PCI bus is EXPRESS type
*INFO* (Pixie_Scan_Crate_Slots_GN): Device 0, Board version= 0xA551, S/N = 231
*ERROR* (Find_Xact_Match): RUNTASK was not found
*ERROR* (Find_Xact_Match): CONTROLTASK was not found
*ERROR* (Find_Xact_Match): RESUME was not found
*ERROR* (Find_Xact_Match): RUNTIMEA was not found
*ERROR* (Find_Xact_Match): NUMEVENTSA was not found
*ERROR* (Find_Xact_Match): SYNCHDONE was not found
*ERROR* (Find_Xact_Match): FASTPEAKSA0 was not found
*ERROR* (Find_Xact_Match): COUNTTIMEA0 was not found
*ERROR* (Find_Xact_Match): FASTPEAKSA1 was not found
*ERROR* (Find_Xact_Match): COUNTTIMEA1 was not found
*ERROR* (Find_Xact_Match): FASTPEAKSA2 was not found
*ERROR* (Find_Xact_Match): COUNTTIMEA2 was not found
*ERROR* (Find_Xact_Match): FASTPEAKSA3 was not found
*ERROR* (Find_Xact_Match): COUNTTIMEA3 was not found
*ERROR* (Find_Xact_Match): FILTERRANGE was not found
*ERROR* (Find_Xact_Match): CCONTROL was not found
*INFO* (Pixie_Boot): Revision 0xA551
*INFO* (PIXIE4E_ProgramFPGA(): BEGIN
*WARNING* (PIXIE4E_ProgramFPGA(): FPGA already programmed, skipping. Power cycle chassis to reprogram
Module 0 in slot 231 started up successfully!
*ERROR* (Find_Xact_Match): SYNCHWAIT was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.

Adjusting offsets ...
*ERROR* (Find_Xact_Match): CHANCSRA0 was not found
*ERROR* (Find_Xact_Match): CHANCSRA0 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): BASELINEPERCENT0 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): HOSTODATA was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): TRACKDAC0 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): TRACKDAC1 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): TRACKDAC2 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): TRACKDAC3 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
Offsets adjusted.
*ERROR* (Find_Xact_Match): CCONTROL was not found
*ERROR* (Find_Xact_Match): CCONTROL was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): FIFOLENGTH was not found
*ERROR* (Find_Xact_Match): TRACELENGTH0 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): FIFOLENGTH was not found
*ERROR* (Find_Xact_Match): USERDELAY0 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): FIFOLENGTH was not found
*ERROR* (Find_Xact_Match): TRACELENGTH1 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): FIFOLENGTH was not found
*ERROR* (Find_Xact_Match): USERDELAY1 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): FIFOLENGTH was not found
*ERROR* (Find_Xact_Match): TRACELENGTH2 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): FIFOLENGTH was not found
*ERROR* (Find_Xact_Match): USERDELAY2 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): FIFOLENGTH was not found
*ERROR* (Find_Xact_Match): TRACELENGTH3 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): FIFOLENGTH was not found
*ERROR* (Find_Xact_Match): USERDELAY3 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): MODNUM was not found
*ERROR* (Find_Xact_Match): MODCSRA was not found
*ERROR* (Find_Xact_Match): MODCSRB was not found
*ERROR* (Find_Xact_Match): MODCSRC was not found
*ERROR* (Find_Xact_Match): CCONTROL was not found
*ERROR* (Find_Xact_Match): RUNTASK was not found
*ERROR* (Find_Xact_Match): MAXEVENTS was not found
*ERROR* (Find_Xact_Match): COINCPATTERN was not found
*ERROR* (Find_Xact_Match): FILTERRANGE was not found
*ERROR* (Find_Xact_Match): CHANCSRA0 was not found
*ERROR* (Find_Xact_Match): CHANCSRA1 was not found
*ERROR* (Find_Xact_Match): CHANCSRA2 was not found
*ERROR* (Find_Xact_Match): CHANCSRA3 was not found
*ERROR* (Find_Xact_Match): PEAKSEP0 was not found
*ERROR* (Find_Xact_Match): COINCDELAY0 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): RESETDELAY0 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): PEAKSEP1 was not found
*ERROR* (Find_Xact_Match): COINCDELAY1 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): RESETDELAY1 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): PEAKSEP2 was not found
*ERROR* (Find_Xact_Match): COINCDELAY2 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): RESETDELAY2 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): PEAKSEP3 was not found
*ERROR* (Find_Xact_Match): COINCDELAY3 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): RESETDELAY3 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): COINCWAIT was not found
*ERROR* (Find_Xact_Match): SYNCHWAIT was not found
*ERROR* (Find_Xact_Match): INSYNCH was not found
*ERROR* (Find_Xact_Match): FILTERRANGE was not found
*ERROR* (Find_Xact_Match): MODULEPATTERN was not found
*ERROR* (Find_Xact_Match): NNSHAREPATTERN was not found
*ERROR* (Find_Xact_Match): DBLBUFCSR was not found
*ERROR* (Find_Xact_Match): XETDELAY was not found
*ERROR* (Find_Xact_Match): PDMMASKA was not found
*ERROR* (Find_Xact_Match): PDMMASKB was not found
*ERROR* (Find_Xact_Match): PDMMASKC was not found
*ERROR* (Find_Xact_Match): BUFHEADLEN was not found
*ERROR* (Find_Xact_Match): EVENTHEADLEN was not found
*ERROR* (Find_Xact_Match): CHANHEADLEN was not found
*ERROR* (Find_Xact_Match): TOTALTIMEA was not found
*ERROR* (Find_Xact_Match): CCTA was not found
*ERROR* (Find_Xact_Match): COINC_COUNT_TIME was not found
*ERROR* (Find_Xact_Match): CSFDTA was not found
*ERROR* (Find_Xact_Match): COINC_SFDT was not found
*ERROR* (Find_Xact_Match): NCOINCTRIGA was not found
*ERROR* (Find_Xact_Match): NUM_COINC_TRIG was not found
*ERROR* (Find_Xact_Match): NCOINCTRIGA was not found
*ERROR* (Find_Xact_Match): CCTA was not found
*ERROR* (Find_Xact_Match): COINC_INPUT_RATE was not found
*ERROR* (Find_Xact_Match): DSPRELEASE was not found
*ERROR* (Find_Xact_Match): DSPBUILD was not found
*ERROR* (Find_Xact_Match): FIPPIID was not found
*ERROR* (Find_Xact_Match): HARDWAREID was not found
*ERROR* (Find_Xact_Match): USERIN was not found
*ERROR* (Find_Xact_Match): EXTRA_IN was not found
*ERROR* (Find_Xact_Match): EXTRAIN was not found
*ERROR* (Find_Xact_Match): USEROUT was not found
*ERROR* (Find_Xact_Match): MODCSRB was not found
*ERROR* (Find_Xact_Match): CCTA was not found
*ERROR* (Find_Xact_Match): EXTRA_OUT was not found
module: par 0 MODULE_NUMBER = 0x0000
module: par 1 MODULE_CSRA = 0x0000
module: par 2 MODULE_CSRB = 0x0000
module: par 3 C_CONTROL = 0x0000
module: par 4 MAX_EVENTS = 0x0000
module: par 5 COINCIDENCE_PATTERN = 0x0000
module: par 6 ACTUAL_COINCIDENCE_WAIT = 0xFFF8
module: par 7 MIN_COINCIDENCE_WAIT = 0x0008
module: par 8 SYNCH_WAIT = 0xFFFF
module: par 9 IN_SYNCH = 0xFFFF
module: par 10 RUN_TYPE = 0x0000
module: par 11 FILTER_RANGE = 0xFFFF
module: par 12 MODULEPATTERN = 0xFFFF
module: par 13 NNSHAREPATTERN = 0xFFFF
module: par 14 DBLBUFCSR = 0xFFFF
module: par 15 MODULE_CSRC = 0x0000
module: par 16 BUFFER_HEAD_LENGTH = 0xFFFF
module: par 17 EVENT_HEAD_LENGTH = 0xFFFF
module: par 18 CHANNEL_HEAD_LENGTH = 0xFFFF
module: par 19  = 0x0000
module: par 20 NUMBER_EVENTS = 0x0000
module: par 21 RUN_TIME = 0x43EE
module: par 22 EVENT_RATE = 0x0478
module: par 23 TOTAL_TIME = 0x43EE
module: par 24 BOARD_VERSION = 0xA551
module: par 25 SERIAL_NUMBER = 0x00E7
module: par 26 DSP_RELEASE = 0xFFFF
module: par 27 DSP_BUILD = 0xFFFF
module: par 28 FIPPI_ID = 0xFFFF
module: par 29 SYSTEM_ID = 0xFFFF
module: par 30 XET_DELAY = 0xFFF8
module: par 31 PDM_MASKA = 0xFFFF
module: par 32 PDM_MASKB = 0xFFFF
module: par 33 PDM_MASKC = 0xFFFF
module: par 34 USER_IN = 0xFFFF
module: par 35  = 0x0000
module: par 36  = 0x0000
module: par 37  = 0x0000
module: par 38  = 0x0000
module: par 39  = 0x0000
module: par 40  = 0x0000
module: par 41  = 0x0000
module: par 42  = 0x0000
module: par 43  = 0x0000
module: par 44  = 0x0000
module: par 45  = 0x0000
module: par 46  = 0x0000
module: par 47  = 0x0000
module: par 48  = 0x0000
module: par 49  = 0x0000
module: par 50 USER_OUT = 0x0000
module: par 51  = 0x0000
module: par 52  = 0x0000
module: par 53  = 0x0000
module: par 54  = 0x0000
module: par 55  = 0x0000
module: par 56  = 0x0000
module: par 57  = 0x0000
module: par 58  = 0x0000
module: par 59  = 0x0000
module: par 60  = 0x0000
module: par 61  = 0x0000
module: par 62  = 0x0000
module: par 63  = 0x0000
module: par 64  = 0x0000
module: par 65  = 0x0000
module: par 66 ADC_BITS = 0x0010
module: par 67 ADC_RATE = 0x007D
module: par 68  = 0x0000
module: par 69  = 0x0000
module: par 70  = 0x0000
module: par 71  = 0x0000
module: par 72  = 0x0000
module: par 73  = 0x0000
module: par 74  = 0x0000
module: par 75  = 0x0000
module: par 76  = 0x0000
module: par 77  = 0x0000
module: par 78  = 0x0000
module: par 79  = 0x0000
module: par 80  = 0x0000
module: par 81  = 0x0000
module: par 82  = 0x0000
module: par 83  = 0x0000
module: par 84  = 0x0000
module: par 85  = 0x0000
module: par 86  = 0x0000
module: par 87  = 0x0000
module: par 88  = 0x0000
module: par 89  = 0x0000
module: par 90  = 0x0000
module: par 91  = 0x0000
module: par 92  = 0x0000
module: par 93  = 0x0000
module: par 94  = 0x0000
module: par 95  = 0x0000
module: par 96  = 0x0000
module: par 97  = 0x0000
module: par 98  = 0x0000
module: par 99  = 0x0000
module: par 100  = 0x0000
module: par 101  = 0x0000
module: par 102  = 0x0000
module: par 103  = 0x0000
module: par 104  = 0x0000
module: par 105  = 0x0000
module: par 106  = 0x0000
module: par 107  = 0x0000
module: par 108  = 0x0000
module: par 109  = 0x0000
module: par 110  = 0x0000
module: par 111  = 0x0000
module: par 112  = 0x0000
module: par 113  = 0x0000
module: par 114  = 0x0000
module: par 115  = 0x0000
module: par 116  = 0x0000
module: par 117  = 0x0000
module: par 118  = 0x0000
module: par 119  = 0x0000
module: par 120  = 0x0000
module: par 121  = 0x0000
module: par 122  = 0x0000
module: par 123  = 0x0000
module: par 124  = 0x0000
module: par 125  = 0x0000
module: par 126  = 0x0000
module: par 127  = 0x0000
*ERROR* (Find_Xact_Match): FASTLENGTH0 was not found
*ERROR* (Find_Xact_Match): FASTGAP0 was not found
*ERROR* (Find_Xact_Match): SLOWLENGTH0 was not found
*ERROR* (Find_Xact_Match): SLOWGAP0 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH0 was not found
*ERROR* (Find_Xact_Match): FASTTHRESH0 was not found
*ERROR* (Find_Xact_Match): SGA0 was not found
*ERROR* (Find_Xact_Match): DIGGAIN0 was not found
*ERROR* (Find_Xact_Match): TRACKDAC0 was not found
*ERROR* (Find_Xact_Match): PREAMPTAUA0 was not found
*ERROR* (Find_Xact_Match): TRACELENGTH0 was not found
*ERROR* (Find_Xact_Match): USERDELAY0 was not found
*ERROR* (Find_Xact_Match): PSAOFFSET0 was not found
*ERROR* (Find_Xact_Match): PSALENGTH0 was not found
*ERROR* (Find_Xact_Match): XWAIT0 was not found
*ERROR* (Find_Xact_Match): BASELINEPERCENT0 was not found
*ERROR* (Find_Xact_Match): CHANCSRA0 was not found
*ERROR* (Find_Xact_Match): CHANCSRB0 was not found
*ERROR* (Find_Xact_Match): CHANCSRC0 was not found
*ERROR* (Find_Xact_Match): CFDTHR0 was not found
*ERROR* (Find_Xact_Match): FCFD_THRESHOLD was not found
*ERROR* (Find_Xact_Match): FCFDTH0 was not found
*ERROR* (Find_Xact_Match): LOG2EBIN0 was not found
*ERROR* (Find_Xact_Match): INTEGRATOR0 was not found
*ERROR* (Find_Xact_Match): GATEWINDOW0 was not found
*ERROR* (Find_Xact_Match): GATEDELAY0 was not found
*ERROR* (Find_Xact_Match): QDC0LENGTH0 was not found
*ERROR* (Find_Xact_Match): QDC1LENGTH0 was not found
*ERROR* (Find_Xact_Match): QDC0DELAY0 was not found
*ERROR* (Find_Xact_Match): QDC1DELAY0 was not found
*ERROR* (Find_Xact_Match): BLCUT0 was not found
*ERROR* (Find_Xact_Match): COINCDELAY0 was not found
*ERROR* (Find_Xact_Match): LOG2BWEIGHT0 was not found
*ERROR* (Find_Xact_Match): FTDTA0 was not found
*ERROR* (Find_Xact_Match): NOUTA0 was not found
*ERROR* (Find_Xact_Match): NPPIA0 was not found
*ERROR* (Find_Xact_Match): NOUTA0 was not found
*ERROR* (Find_Xact_Match): NPPIA0 was not found
*ERROR* (Find_Xact_Match): FTDTA0 was not found
*ERROR* (Find_Xact_Match): GCOUNTA0 was not found
*ERROR* (Find_Xact_Match): GCOUNTA0 was not found
*ERROR* (Find_Xact_Match): SFDTA0 was not found
*ERROR* (Find_Xact_Match): GDTA0 was not found
*ERROR* (Find_Xact_Match): ICR0 was not found
*ERROR* (Find_Xact_Match): OORF0 was not found
*ERROR* (Find_Xact_Match): CH_EXTRA_IN was not found
*ERROR* (Find_Xact_Match): CHEXTRAIN0 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH1 was not found
*ERROR* (Find_Xact_Match): FASTGAP1 was not found
*ERROR* (Find_Xact_Match): SLOWLENGTH1 was not found
*ERROR* (Find_Xact_Match): SLOWGAP1 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH1 was not found
*ERROR* (Find_Xact_Match): FASTTHRESH1 was not found
*ERROR* (Find_Xact_Match): SGA1 was not found
*ERROR* (Find_Xact_Match): DIGGAIN1 was not found
*ERROR* (Find_Xact_Match): TRACKDAC1 was not found
*ERROR* (Find_Xact_Match): PREAMPTAUA1 was not found
*ERROR* (Find_Xact_Match): TRACELENGTH1 was not found
*ERROR* (Find_Xact_Match): USERDELAY1 was not found
*ERROR* (Find_Xact_Match): PSAOFFSET1 was not found
*ERROR* (Find_Xact_Match): PSALENGTH1 was not found
*ERROR* (Find_Xact_Match): XWAIT1 was not found
*ERROR* (Find_Xact_Match): BASELINEPERCENT1 was not found
*ERROR* (Find_Xact_Match): CHANCSRA1 was not found
*ERROR* (Find_Xact_Match): CHANCSRB1 was not found
*ERROR* (Find_Xact_Match): CHANCSRC1 was not found
*ERROR* (Find_Xact_Match): CFDTHR1 was not found
*ERROR* (Find_Xact_Match): FCFD_THRESHOLD was not found
*ERROR* (Find_Xact_Match): FCFDTH1 was not found
*ERROR* (Find_Xact_Match): LOG2EBIN1 was not found
*ERROR* (Find_Xact_Match): INTEGRATOR1 was not found
*ERROR* (Find_Xact_Match): GATEWINDOW1 was not found
*ERROR* (Find_Xact_Match): GATEDELAY1 was not found
*ERROR* (Find_Xact_Match): QDC0LENGTH1 was not found
*ERROR* (Find_Xact_Match): QDC1LENGTH1 was not found
*ERROR* (Find_Xact_Match): QDC0DELAY1 was not found
*ERROR* (Find_Xact_Match): QDC1DELAY1 was not found
*ERROR* (Find_Xact_Match): BLCUT1 was not found
*ERROR* (Find_Xact_Match): COINCDELAY1 was not found
*ERROR* (Find_Xact_Match): LOG2BWEIGHT1 was not found
*ERROR* (Find_Xact_Match): FTDTA1 was not found
*ERROR* (Find_Xact_Match): NOUTA1 was not found
*ERROR* (Find_Xact_Match): NPPIA1 was not found
*ERROR* (Find_Xact_Match): NOUTA1 was not found
*ERROR* (Find_Xact_Match): NPPIA1 was not found
*ERROR* (Find_Xact_Match): FTDTA1 was not found
*ERROR* (Find_Xact_Match): GCOUNTA1 was not found
*ERROR* (Find_Xact_Match): GCOUNTA1 was not found
*ERROR* (Find_Xact_Match): SFDTA1 was not found
*ERROR* (Find_Xact_Match): GDTA1 was not found
*ERROR* (Find_Xact_Match): ICR1 was not found
*ERROR* (Find_Xact_Match): OORF1 was not found
*ERROR* (Find_Xact_Match): CH_EXTRA_IN was not found
*ERROR* (Find_Xact_Match): CHEXTRAIN1 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH2 was not found
*ERROR* (Find_Xact_Match): FASTGAP2 was not found
*ERROR* (Find_Xact_Match): SLOWLENGTH2 was not found
*ERROR* (Find_Xact_Match): SLOWGAP2 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH2 was not found
*ERROR* (Find_Xact_Match): FASTTHRESH2 was not found
*ERROR* (Find_Xact_Match): SGA2 was not found
*ERROR* (Find_Xact_Match): DIGGAIN2 was not found
*ERROR* (Find_Xact_Match): TRACKDAC2 was not found
*ERROR* (Find_Xact_Match): PREAMPTAUA2 was not found
*ERROR* (Find_Xact_Match): TRACELENGTH2 was not found
*ERROR* (Find_Xact_Match): USERDELAY2 was not found
*ERROR* (Find_Xact_Match): PSAOFFSET2 was not found
*ERROR* (Find_Xact_Match): PSALENGTH2 was not found
*ERROR* (Find_Xact_Match): XWAIT2 was not found
*ERROR* (Find_Xact_Match): BASELINEPERCENT2 was not found
*ERROR* (Find_Xact_Match): CHANCSRA2 was not found
*ERROR* (Find_Xact_Match): CHANCSRB2 was not found
*ERROR* (Find_Xact_Match): CHANCSRC2 was not found
*ERROR* (Find_Xact_Match): CFDTHR2 was not found
*ERROR* (Find_Xact_Match): FCFD_THRESHOLD was not found
*ERROR* (Find_Xact_Match): FCFDTH2 was not found
*ERROR* (Find_Xact_Match): LOG2EBIN2 was not found
*ERROR* (Find_Xact_Match): INTEGRATOR2 was not found
*ERROR* (Find_Xact_Match): GATEWINDOW2 was not found
*ERROR* (Find_Xact_Match): GATEDELAY2 was not found
*ERROR* (Find_Xact_Match): QDC0LENGTH2 was not found
*ERROR* (Find_Xact_Match): QDC1LENGTH2 was not found
*ERROR* (Find_Xact_Match): QDC0DELAY2 was not found
*ERROR* (Find_Xact_Match): QDC1DELAY2 was not found
*ERROR* (Find_Xact_Match): BLCUT2 was not found
*ERROR* (Find_Xact_Match): COINCDELAY2 was not found
*ERROR* (Find_Xact_Match): LOG2BWEIGHT2 was not found
*ERROR* (Find_Xact_Match): FTDTA2 was not found
*ERROR* (Find_Xact_Match): NOUTA2 was not found
*ERROR* (Find_Xact_Match): NPPIA2 was not found
*ERROR* (Find_Xact_Match): NOUTA2 was not found
*ERROR* (Find_Xact_Match): NPPIA2 was not found
*ERROR* (Find_Xact_Match): FTDTA2 was not found
*ERROR* (Find_Xact_Match): GCOUNTA2 was not found
*ERROR* (Find_Xact_Match): GCOUNTA2 was not found
*ERROR* (Find_Xact_Match): SFDTA2 was not found
*ERROR* (Find_Xact_Match): GDTA2 was not found
*ERROR* (Find_Xact_Match): ICR2 was not found
*ERROR* (Find_Xact_Match): OORF2 was not found
*ERROR* (Find_Xact_Match): CH_EXTRA_IN was not found
*ERROR* (Find_Xact_Match): CHEXTRAIN2 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH3 was not found
*ERROR* (Find_Xact_Match): FASTGAP3 was not found
*ERROR* (Find_Xact_Match): SLOWLENGTH3 was not found
*ERROR* (Find_Xact_Match): SLOWGAP3 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH3 was not found
*ERROR* (Find_Xact_Match): FASTTHRESH3 was not found
*ERROR* (Find_Xact_Match): SGA3 was not found
*ERROR* (Find_Xact_Match): DIGGAIN3 was not found
*ERROR* (Find_Xact_Match): TRACKDAC3 was not found
*ERROR* (Find_Xact_Match): PREAMPTAUA3 was not found
*ERROR* (Find_Xact_Match): TRACELENGTH3 was not found
*ERROR* (Find_Xact_Match): USERDELAY3 was not found
*ERROR* (Find_Xact_Match): PSAOFFSET3 was not found
*ERROR* (Find_Xact_Match): PSALENGTH3 was not found
*ERROR* (Find_Xact_Match): XWAIT3 was not found
*ERROR* (Find_Xact_Match): BASELINEPERCENT3 was not found
*ERROR* (Find_Xact_Match): CHANCSRA3 was not found
*ERROR* (Find_Xact_Match): CHANCSRB3 was not found
*ERROR* (Find_Xact_Match): CHANCSRC3 was not found
*ERROR* (Find_Xact_Match): CFDTHR3 was not found
*ERROR* (Find_Xact_Match): FCFD_THRESHOLD was not found
*ERROR* (Find_Xact_Match): FCFDTH3 was not found
*ERROR* (Find_Xact_Match): LOG2EBIN3 was not found
*ERROR* (Find_Xact_Match): INTEGRATOR3 was not found
*ERROR* (Find_Xact_Match): GATEWINDOW3 was not found
*ERROR* (Find_Xact_Match): GATEDELAY3 was not found
*ERROR* (Find_Xact_Match): QDC0LENGTH3 was not found
*ERROR* (Find_Xact_Match): QDC1LENGTH3 was not found
*ERROR* (Find_Xact_Match): QDC0DELAY3 was not found
*ERROR* (Find_Xact_Match): QDC1DELAY3 was not found
*ERROR* (Find_Xact_Match): BLCUT3 was not found
*ERROR* (Find_Xact_Match): COINCDELAY3 was not found
*ERROR* (Find_Xact_Match): LOG2BWEIGHT3 was not found
*ERROR* (Find_Xact_Match): FTDTA3 was not found
*ERROR* (Find_Xact_Match): NOUTA3 was not found
*ERROR* (Find_Xact_Match): NPPIA3 was not found
*ERROR* (Find_Xact_Match): NOUTA3 was not found
*ERROR* (Find_Xact_Match): NPPIA3 was not found
*ERROR* (Find_Xact_Match): FTDTA3 was not found
*ERROR* (Find_Xact_Match): GCOUNTA3 was not found
*ERROR* (Find_Xact_Match): GCOUNTA3 was not found
*ERROR* (Find_Xact_Match): SFDTA3 was not found
*ERROR* (Find_Xact_Match): GDTA3 was not found
*ERROR* (Find_Xact_Match): ICR3 was not found
*ERROR* (Find_Xact_Match): OORF3 was not found
*ERROR* (Find_Xact_Match): CH_EXTRA_IN was not found
*ERROR* (Find_Xact_Match): CHEXTRAIN3 was not found
chan 0: par 0 CHANNEL_CSRA = 0.000000
chan 0: par 1 CHANNEL_CSRB = 0.000000
chan 0: par 2 ENERGY_RISETIME = 0.000000
chan 0: par 3 ENERGY_FLATTOP = 0.000000
chan 0: par 4 TRIGGER_RISETIME = 0.000000
chan 0: par 5 TRIGGER_FLATTOP = 0.000000
chan 0: par 6 TRIGGER_THRESHOLD = 0.000000
chan 0: par 7 VGAIN = 1.600000
chan 0: par 8 VOFFSET = 2.500000
chan 0: par 9 TRACE_LENGTH = 0.000000
chan 0: par 10 TRACE_DELAY = 0.000000
chan 0: par 11 PSA_START = 0.000000
chan 0: par 12 PSA_END = 0.000000
chan 0: par 13  = 0.000000
chan 0: par 14 BINFACTOR = 65536.000000
chan 0: par 15 TAU = 0.000000
chan 0: par 16 BLCUT = 0.000000
chan 0: par 17 XDT = 0.000000
chan 0: par 18 BASELINE_PERCENT = 0.000000
chan 0: par 19 CFD_THRESHOLD = 0.000000
chan 0: par 20 INTEGRATOR = 0.000000
chan 0: par 21 CHANNEL_CSRC = 0.000000
chan 0: par 22 GATE_WINDOW = 0.000000
chan 0: par 23 GATE_DELAY = 0.000000
chan 0: par 24 COINC_DELAY = 0.000000
chan 0: par 25 BLAVG = 0.000000
chan 0: par 26 COUNT_TIME = 0.000000
chan 0: par 27 INPUT_COUNT_RATE = 0.000000
chan 0: par 28 FAST_PEAKS = 0.000000
chan 0: par 29 OUTPUT_COUNT_RATE = 0.000000
chan 0: par 30 NOUT = 0.000000
chan 0: par 31 GATE_RATE = 0.000000
chan 0: par 32 GATE_COUNTS = 0.000000
chan 0: par 33 FTDT = 0.000000
chan 0: par 34 SFDT = 0.000000
chan 0: par 35 GDT = 0.000000
chan 0: par 36 CURRENT_ICR = 0.000000
chan 0: par 37 CURRENT_OORF = 0.000000
chan 0: par 38 PSM_GAIN_AVG = 3.141593
chan 0: par 39 PSM_GAIN_AVG_LEN = 10.000000
chan 0: par 40 PSM_TEMP_AVG = 3.141593
chan 0: par 41 PSM_TEMP_AVG_LEN = 10.000000
chan 0: par 42 PSM_GAIN_CORR = 10.000000
chan 0: par 43 QDC0_LENGTH = 0.000000
chan 0: par 44 QDC1_LENGTH = 0.000000
chan 0: par 45 QDC0_DELAY = 0.000000
chan 0: par 46 QDC1_DELAY = 0.000000
chan 0: par 47 NPPI = 0.000000
chan 0: par 48 PASS_PILEUP_RATE = 0.000000
chan 0: par 49  = 0.000000
chan 0: par 50  = 0.000000
chan 0: par 51  = 0.000000
chan 0: par 52  = 0.000000
chan 0: par 53  = 0.000000
chan 0: par 54  = 0.000000
chan 0: par 55  = 0.000000
chan 0: par 56  = 0.000000
chan 0: par 57  = 0.000000
chan 0: par 58  = 0.000000
chan 0: par 59  = 0.000000
chan 0: par 60  = 0.000000
chan 0: par 61  = 0.000000
chan 0: par 62  = 0.000000
chan 0: par 63  = 0.000000
*ERROR* (Find_Xact_Match): FASTLENGTH0 was not found
*ERROR* (Find_Xact_Match): FASTGAP0 was not found
*ERROR* (Find_Xact_Match): SLOWLENGTH0 was not found
*ERROR* (Find_Xact_Match): SLOWGAP0 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH0 was not found
*ERROR* (Find_Xact_Match): FASTTHRESH0 was not found
*ERROR* (Find_Xact_Match): SGA0 was not found
*ERROR* (Find_Xact_Match): DIGGAIN0 was not found
*ERROR* (Find_Xact_Match): TRACKDAC0 was not found
*ERROR* (Find_Xact_Match): PREAMPTAUA0 was not found
*ERROR* (Find_Xact_Match): TRACELENGTH0 was not found
*ERROR* (Find_Xact_Match): USERDELAY0 was not found
*ERROR* (Find_Xact_Match): PSAOFFSET0 was not found
*ERROR* (Find_Xact_Match): PSALENGTH0 was not found
*ERROR* (Find_Xact_Match): XWAIT0 was not found
*ERROR* (Find_Xact_Match): BASELINEPERCENT0 was not found
*ERROR* (Find_Xact_Match): CHANCSRA0 was not found
*ERROR* (Find_Xact_Match): CHANCSRB0 was not found
*ERROR* (Find_Xact_Match): CHANCSRC0 was not found
*ERROR* (Find_Xact_Match): CFDTHR0 was not found
*ERROR* (Find_Xact_Match): FCFD_THRESHOLD was not found
*ERROR* (Find_Xact_Match): FCFDTH0 was not found
*ERROR* (Find_Xact_Match): LOG2EBIN0 was not found
*ERROR* (Find_Xact_Match): INTEGRATOR0 was not found
*ERROR* (Find_Xact_Match): GATEWINDOW0 was not found
*ERROR* (Find_Xact_Match): GATEDELAY0 was not found
*ERROR* (Find_Xact_Match): QDC0LENGTH0 was not found
*ERROR* (Find_Xact_Match): QDC1LENGTH0 was not found
*ERROR* (Find_Xact_Match): QDC0DELAY0 was not found
*ERROR* (Find_Xact_Match): QDC1DELAY0 was not found
*ERROR* (Find_Xact_Match): BLCUT0 was not found
*ERROR* (Find_Xact_Match): COINCDELAY0 was not found
*ERROR* (Find_Xact_Match): LOG2BWEIGHT0 was not found
*ERROR* (Find_Xact_Match): FTDTA0 was not found
*ERROR* (Find_Xact_Match): NOUTA0 was not found
*ERROR* (Find_Xact_Match): NPPIA0 was not found
*ERROR* (Find_Xact_Match): NOUTA0 was not found
*ERROR* (Find_Xact_Match): NPPIA0 was not found
*ERROR* (Find_Xact_Match): FTDTA0 was not found
*ERROR* (Find_Xact_Match): GCOUNTA0 was not found
*ERROR* (Find_Xact_Match): GCOUNTA0 was not found
*ERROR* (Find_Xact_Match): SFDTA0 was not found
*ERROR* (Find_Xact_Match): GDTA0 was not found
*ERROR* (Find_Xact_Match): ICR0 was not found
*ERROR* (Find_Xact_Match): OORF0 was not found
*ERROR* (Find_Xact_Match): CH_EXTRA_IN was not found
*ERROR* (Find_Xact_Match): CHEXTRAIN0 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH1 was not found
*ERROR* (Find_Xact_Match): FASTGAP1 was not found
*ERROR* (Find_Xact_Match): SLOWLENGTH1 was not found
*ERROR* (Find_Xact_Match): SLOWGAP1 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH1 was not found
*ERROR* (Find_Xact_Match): FASTTHRESH1 was not found
*ERROR* (Find_Xact_Match): SGA1 was not found
*ERROR* (Find_Xact_Match): DIGGAIN1 was not found
*ERROR* (Find_Xact_Match): TRACKDAC1 was not found
*ERROR* (Find_Xact_Match): PREAMPTAUA1 was not found
*ERROR* (Find_Xact_Match): TRACELENGTH1 was not found
*ERROR* (Find_Xact_Match): USERDELAY1 was not found
*ERROR* (Find_Xact_Match): PSAOFFSET1 was not found
*ERROR* (Find_Xact_Match): PSALENGTH1 was not found
*ERROR* (Find_Xact_Match): XWAIT1 was not found
*ERROR* (Find_Xact_Match): BASELINEPERCENT1 was not found
*ERROR* (Find_Xact_Match): CHANCSRA1 was not found
*ERROR* (Find_Xact_Match): CHANCSRB1 was not found
*ERROR* (Find_Xact_Match): CHANCSRC1 was not found
*ERROR* (Find_Xact_Match): CFDTHR1 was not found
*ERROR* (Find_Xact_Match): FCFD_THRESHOLD was not found
*ERROR* (Find_Xact_Match): FCFDTH1 was not found
*ERROR* (Find_Xact_Match): LOG2EBIN1 was not found
*ERROR* (Find_Xact_Match): INTEGRATOR1 was not found
*ERROR* (Find_Xact_Match): GATEWINDOW1 was not found
*ERROR* (Find_Xact_Match): GATEDELAY1 was not found
*ERROR* (Find_Xact_Match): QDC0LENGTH1 was not found
*ERROR* (Find_Xact_Match): QDC1LENGTH1 was not found
*ERROR* (Find_Xact_Match): QDC0DELAY1 was not found
*ERROR* (Find_Xact_Match): QDC1DELAY1 was not found
*ERROR* (Find_Xact_Match): BLCUT1 was not found
*ERROR* (Find_Xact_Match): COINCDELAY1 was not found
*ERROR* (Find_Xact_Match): LOG2BWEIGHT1 was not found
*ERROR* (Find_Xact_Match): FTDTA1 was not found
*ERROR* (Find_Xact_Match): NOUTA1 was not found
*ERROR* (Find_Xact_Match): NPPIA1 was not found
*ERROR* (Find_Xact_Match): NOUTA1 was not found
*ERROR* (Find_Xact_Match): NPPIA1 was not found
*ERROR* (Find_Xact_Match): FTDTA1 was not found
*ERROR* (Find_Xact_Match): GCOUNTA1 was not found
*ERROR* (Find_Xact_Match): GCOUNTA1 was not found
*ERROR* (Find_Xact_Match): SFDTA1 was not found
*ERROR* (Find_Xact_Match): GDTA1 was not found
*ERROR* (Find_Xact_Match): ICR1 was not found
*ERROR* (Find_Xact_Match): OORF1 was not found
*ERROR* (Find_Xact_Match): CH_EXTRA_IN was not found
*ERROR* (Find_Xact_Match): CHEXTRAIN1 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH2 was not found
*ERROR* (Find_Xact_Match): FASTGAP2 was not found
*ERROR* (Find_Xact_Match): SLOWLENGTH2 was not found
*ERROR* (Find_Xact_Match): SLOWGAP2 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH2 was not found
*ERROR* (Find_Xact_Match): FASTTHRESH2 was not found
*ERROR* (Find_Xact_Match): SGA2 was not found
*ERROR* (Find_Xact_Match): DIGGAIN2 was not found
*ERROR* (Find_Xact_Match): TRACKDAC2 was not found
*ERROR* (Find_Xact_Match): PREAMPTAUA2 was not found
*ERROR* (Find_Xact_Match): TRACELENGTH2 was not found
*ERROR* (Find_Xact_Match): USERDELAY2 was not found
*ERROR* (Find_Xact_Match): PSAOFFSET2 was not found
*ERROR* (Find_Xact_Match): PSALENGTH2 was not found
*ERROR* (Find_Xact_Match): XWAIT2 was not found
*ERROR* (Find_Xact_Match): BASELINEPERCENT2 was not found
*ERROR* (Find_Xact_Match): CHANCSRA2 was not found
*ERROR* (Find_Xact_Match): CHANCSRB2 was not found
*ERROR* (Find_Xact_Match): CHANCSRC2 was not found
*ERROR* (Find_Xact_Match): CFDTHR2 was not found
*ERROR* (Find_Xact_Match): FCFD_THRESHOLD was not found
*ERROR* (Find_Xact_Match): FCFDTH2 was not found
*ERROR* (Find_Xact_Match): LOG2EBIN2 was not found
*ERROR* (Find_Xact_Match): INTEGRATOR2 was not found
*ERROR* (Find_Xact_Match): GATEWINDOW2 was not found
*ERROR* (Find_Xact_Match): GATEDELAY2 was not found
*ERROR* (Find_Xact_Match): QDC0LENGTH2 was not found
*ERROR* (Find_Xact_Match): QDC1LENGTH2 was not found
*ERROR* (Find_Xact_Match): QDC0DELAY2 was not found
*ERROR* (Find_Xact_Match): QDC1DELAY2 was not found
*ERROR* (Find_Xact_Match): BLCUT2 was not found
*ERROR* (Find_Xact_Match): COINCDELAY2 was not found
*ERROR* (Find_Xact_Match): LOG2BWEIGHT2 was not found
*ERROR* (Find_Xact_Match): FTDTA2 was not found
*ERROR* (Find_Xact_Match): NOUTA2 was not found
*ERROR* (Find_Xact_Match): NPPIA2 was not found
*ERROR* (Find_Xact_Match): NOUTA2 was not found
*ERROR* (Find_Xact_Match): NPPIA2 was not found
*ERROR* (Find_Xact_Match): FTDTA2 was not found
*ERROR* (Find_Xact_Match): GCOUNTA2 was not found
*ERROR* (Find_Xact_Match): GCOUNTA2 was not found
*ERROR* (Find_Xact_Match): SFDTA2 was not found
*ERROR* (Find_Xact_Match): GDTA2 was not found
*ERROR* (Find_Xact_Match): ICR2 was not found
*ERROR* (Find_Xact_Match): OORF2 was not found
*ERROR* (Find_Xact_Match): CH_EXTRA_IN was not found
*ERROR* (Find_Xact_Match): CHEXTRAIN2 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH3 was not found
*ERROR* (Find_Xact_Match): FASTGAP3 was not found
*ERROR* (Find_Xact_Match): SLOWLENGTH3 was not found
*ERROR* (Find_Xact_Match): SLOWGAP3 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH3 was not found
*ERROR* (Find_Xact_Match): FASTTHRESH3 was not found
*ERROR* (Find_Xact_Match): SGA3 was not found
*ERROR* (Find_Xact_Match): DIGGAIN3 was not found
*ERROR* (Find_Xact_Match): TRACKDAC3 was not found
*ERROR* (Find_Xact_Match): PREAMPTAUA3 was not found
*ERROR* (Find_Xact_Match): TRACELENGTH3 was not found
*ERROR* (Find_Xact_Match): USERDELAY3 was not found
*ERROR* (Find_Xact_Match): PSAOFFSET3 was not found
*ERROR* (Find_Xact_Match): PSALENGTH3 was not found
*ERROR* (Find_Xact_Match): XWAIT3 was not found
*ERROR* (Find_Xact_Match): BASELINEPERCENT3 was not found
*ERROR* (Find_Xact_Match): CHANCSRA3 was not found
*ERROR* (Find_Xact_Match): CHANCSRB3 was not found
*ERROR* (Find_Xact_Match): CHANCSRC3 was not found
*ERROR* (Find_Xact_Match): CFDTHR3 was not found
*ERROR* (Find_Xact_Match): FCFD_THRESHOLD was not found
*ERROR* (Find_Xact_Match): FCFDTH3 was not found
*ERROR* (Find_Xact_Match): LOG2EBIN3 was not found
*ERROR* (Find_Xact_Match): INTEGRATOR3 was not found
*ERROR* (Find_Xact_Match): GATEWINDOW3 was not found
*ERROR* (Find_Xact_Match): GATEDELAY3 was not found
*ERROR* (Find_Xact_Match): QDC0LENGTH3 was not found
*ERROR* (Find_Xact_Match): QDC1LENGTH3 was not found
*ERROR* (Find_Xact_Match): QDC0DELAY3 was not found
*ERROR* (Find_Xact_Match): QDC1DELAY3 was not found
*ERROR* (Find_Xact_Match): BLCUT3 was not found
*ERROR* (Find_Xact_Match): COINCDELAY3 was not found
*ERROR* (Find_Xact_Match): LOG2BWEIGHT3 was not found
*ERROR* (Find_Xact_Match): FTDTA3 was not found
*ERROR* (Find_Xact_Match): NOUTA3 was not found
*ERROR* (Find_Xact_Match): NPPIA3 was not found
*ERROR* (Find_Xact_Match): NOUTA3 was not found
*ERROR* (Find_Xact_Match): NPPIA3 was not found
*ERROR* (Find_Xact_Match): FTDTA3 was not found
*ERROR* (Find_Xact_Match): GCOUNTA3 was not found
*ERROR* (Find_Xact_Match): GCOUNTA3 was not found
*ERROR* (Find_Xact_Match): SFDTA3 was not found
*ERROR* (Find_Xact_Match): GDTA3 was not found
*ERROR* (Find_Xact_Match): ICR3 was not found
*ERROR* (Find_Xact_Match): OORF3 was not found
*ERROR* (Find_Xact_Match): CH_EXTRA_IN was not found
*ERROR* (Find_Xact_Match): CHEXTRAIN3 was not found
chan 1: par 0 CHANNEL_CSRA = 0.000000
chan 1: par 1 CHANNEL_CSRB = 0.000000
chan 1: par 2 ENERGY_RISETIME = 0.000000
chan 1: par 3 ENERGY_FLATTOP = 0.000000
chan 1: par 4 TRIGGER_RISETIME = 0.000000
chan 1: par 5 TRIGGER_FLATTOP = 0.000000
chan 1: par 6 TRIGGER_THRESHOLD = 0.000000
chan 1: par 7 VGAIN = 1.600000
chan 1: par 8 VOFFSET = 2.500000
chan 1: par 9 TRACE_LENGTH = 0.000000
chan 1: par 10 TRACE_DELAY = 0.000000
chan 1: par 11 PSA_START = 0.000000
chan 1: par 12 PSA_END = 0.000000
chan 1: par 13  = 0.000000
chan 1: par 14 BINFACTOR = 65536.000000
chan 1: par 15 TAU = 0.000000
chan 1: par 16 BLCUT = 0.000000
chan 1: par 17 XDT = 0.000000
chan 1: par 18 BASELINE_PERCENT = 0.000000
chan 1: par 19 CFD_THRESHOLD = 0.000000
chan 1: par 20 INTEGRATOR = 0.000000
chan 1: par 21 CHANNEL_CSRC = 0.000000
chan 1: par 22 GATE_WINDOW = 0.000000
chan 1: par 23 GATE_DELAY = 0.000000
chan 1: par 24 COINC_DELAY = 0.000000
chan 1: par 25 BLAVG = 0.000000
chan 1: par 26 COUNT_TIME = 0.000000
chan 1: par 27 INPUT_COUNT_RATE = 0.000000
chan 1: par 28 FAST_PEAKS = 0.000000
chan 1: par 29 OUTPUT_COUNT_RATE = 0.000000
chan 1: par 30 NOUT = 0.000000
chan 1: par 31 GATE_RATE = 0.000000
chan 1: par 32 GATE_COUNTS = 0.000000
chan 1: par 33 FTDT = 0.000000
chan 1: par 34 SFDT = 0.000000
chan 1: par 35 GDT = 0.000000
chan 1: par 36 CURRENT_ICR = 0.000000
chan 1: par 37 CURRENT_OORF = 0.000000
chan 1: par 38 PSM_GAIN_AVG = 3.141593
chan 1: par 39 PSM_GAIN_AVG_LEN = 10.000000
chan 1: par 40 PSM_TEMP_AVG = 3.141593
chan 1: par 41 PSM_TEMP_AVG_LEN = 10.000000
chan 1: par 42 PSM_GAIN_CORR = 10.000000
chan 1: par 43 QDC0_LENGTH = 0.000000
chan 1: par 44 QDC1_LENGTH = 0.000000
chan 1: par 45 QDC0_DELAY = 0.000000
chan 1: par 46 QDC1_DELAY = 0.000000
chan 1: par 47 NPPI = 0.000000
chan 1: par 48 PASS_PILEUP_RATE = 0.000000
chan 1: par 49  = 0.000000
chan 1: par 50  = 0.000000
chan 1: par 51  = 0.000000
chan 1: par 52  = 0.000000
chan 1: par 53  = 0.000000
chan 1: par 54  = 0.000000
chan 1: par 55  = 0.000000
chan 1: par 56  = 0.000000
chan 1: par 57  = 0.000000
chan 1: par 58  = 0.000000
chan 1: par 59  = 0.000000
chan 1: par 60  = 0.000000
chan 1: par 61  = 0.000000
chan 1: par 62  = 0.000000
chan 1: par 63  = 0.000000
*ERROR* (Find_Xact_Match): FASTLENGTH0 was not found
*ERROR* (Find_Xact_Match): FASTGAP0 was not found
*ERROR* (Find_Xact_Match): SLOWLENGTH0 was not found
*ERROR* (Find_Xact_Match): SLOWGAP0 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH0 was not found
*ERROR* (Find_Xact_Match): FASTTHRESH0 was not found
*ERROR* (Find_Xact_Match): SGA0 was not found
*ERROR* (Find_Xact_Match): DIGGAIN0 was not found
*ERROR* (Find_Xact_Match): TRACKDAC0 was not found
*ERROR* (Find_Xact_Match): PREAMPTAUA0 was not found
*ERROR* (Find_Xact_Match): TRACELENGTH0 was not found
*ERROR* (Find_Xact_Match): USERDELAY0 was not found
*ERROR* (Find_Xact_Match): PSAOFFSET0 was not found
*ERROR* (Find_Xact_Match): PSALENGTH0 was not found
*ERROR* (Find_Xact_Match): XWAIT0 was not found
*ERROR* (Find_Xact_Match): BASELINEPERCENT0 was not found
*ERROR* (Find_Xact_Match): CHANCSRA0 was not found
*ERROR* (Find_Xact_Match): CHANCSRB0 was not found
*ERROR* (Find_Xact_Match): CHANCSRC0 was not found
*ERROR* (Find_Xact_Match): CFDTHR0 was not found
*ERROR* (Find_Xact_Match): FCFD_THRESHOLD was not found
*ERROR* (Find_Xact_Match): FCFDTH0 was not found
*ERROR* (Find_Xact_Match): LOG2EBIN0 was not found
*ERROR* (Find_Xact_Match): INTEGRATOR0 was not found
*ERROR* (Find_Xact_Match): GATEWINDOW0 was not found
*ERROR* (Find_Xact_Match): GATEDELAY0 was not found
*ERROR* (Find_Xact_Match): QDC0LENGTH0 was not found
*ERROR* (Find_Xact_Match): QDC1LENGTH0 was not found
*ERROR* (Find_Xact_Match): QDC0DELAY0 was not found
*ERROR* (Find_Xact_Match): QDC1DELAY0 was not found
*ERROR* (Find_Xact_Match): BLCUT0 was not found
*ERROR* (Find_Xact_Match): COINCDELAY0 was not found
*ERROR* (Find_Xact_Match): LOG2BWEIGHT0 was not found
*ERROR* (Find_Xact_Match): FTDTA0 was not found
*ERROR* (Find_Xact_Match): NOUTA0 was not found
*ERROR* (Find_Xact_Match): NPPIA0 was not found
*ERROR* (Find_Xact_Match): NOUTA0 was not found
*ERROR* (Find_Xact_Match): NPPIA0 was not found
*ERROR* (Find_Xact_Match): FTDTA0 was not found
*ERROR* (Find_Xact_Match): GCOUNTA0 was not found
*ERROR* (Find_Xact_Match): GCOUNTA0 was not found
*ERROR* (Find_Xact_Match): SFDTA0 was not found
*ERROR* (Find_Xact_Match): GDTA0 was not found
*ERROR* (Find_Xact_Match): ICR0 was not found
*ERROR* (Find_Xact_Match): OORF0 was not found
*ERROR* (Find_Xact_Match): CH_EXTRA_IN was not found
*ERROR* (Find_Xact_Match): CHEXTRAIN0 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH1 was not found
*ERROR* (Find_Xact_Match): FASTGAP1 was not found
*ERROR* (Find_Xact_Match): SLOWLENGTH1 was not found
*ERROR* (Find_Xact_Match): SLOWGAP1 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH1 was not found
*ERROR* (Find_Xact_Match): FASTTHRESH1 was not found
*ERROR* (Find_Xact_Match): SGA1 was not found
*ERROR* (Find_Xact_Match): DIGGAIN1 was not found
*ERROR* (Find_Xact_Match): TRACKDAC1 was not found
*ERROR* (Find_Xact_Match): PREAMPTAUA1 was not found
*ERROR* (Find_Xact_Match): TRACELENGTH1 was not found
*ERROR* (Find_Xact_Match): USERDELAY1 was not found
*ERROR* (Find_Xact_Match): PSAOFFSET1 was not found
*ERROR* (Find_Xact_Match): PSALENGTH1 was not found
*ERROR* (Find_Xact_Match): XWAIT1 was not found
*ERROR* (Find_Xact_Match): BASELINEPERCENT1 was not found
*ERROR* (Find_Xact_Match): CHANCSRA1 was not found
*ERROR* (Find_Xact_Match): CHANCSRB1 was not found
*ERROR* (Find_Xact_Match): CHANCSRC1 was not found
*ERROR* (Find_Xact_Match): CFDTHR1 was not found
*ERROR* (Find_Xact_Match): FCFD_THRESHOLD was not found
*ERROR* (Find_Xact_Match): FCFDTH1 was not found
*ERROR* (Find_Xact_Match): LOG2EBIN1 was not found
*ERROR* (Find_Xact_Match): INTEGRATOR1 was not found
*ERROR* (Find_Xact_Match): GATEWINDOW1 was not found
*ERROR* (Find_Xact_Match): GATEDELAY1 was not found
*ERROR* (Find_Xact_Match): QDC0LENGTH1 was not found
*ERROR* (Find_Xact_Match): QDC1LENGTH1 was not found
*ERROR* (Find_Xact_Match): QDC0DELAY1 was not found
*ERROR* (Find_Xact_Match): QDC1DELAY1 was not found
*ERROR* (Find_Xact_Match): BLCUT1 was not found
*ERROR* (Find_Xact_Match): COINCDELAY1 was not found
*ERROR* (Find_Xact_Match): LOG2BWEIGHT1 was not found
*ERROR* (Find_Xact_Match): FTDTA1 was not found
*ERROR* (Find_Xact_Match): NOUTA1 was not found
*ERROR* (Find_Xact_Match): NPPIA1 was not found
*ERROR* (Find_Xact_Match): NOUTA1 was not found
*ERROR* (Find_Xact_Match): NPPIA1 was not found
*ERROR* (Find_Xact_Match): FTDTA1 was not found
*ERROR* (Find_Xact_Match): GCOUNTA1 was not found
*ERROR* (Find_Xact_Match): GCOUNTA1 was not found
*ERROR* (Find_Xact_Match): SFDTA1 was not found
*ERROR* (Find_Xact_Match): GDTA1 was not found
*ERROR* (Find_Xact_Match): ICR1 was not found
*ERROR* (Find_Xact_Match): OORF1 was not found
*ERROR* (Find_Xact_Match): CH_EXTRA_IN was not found
*ERROR* (Find_Xact_Match): CHEXTRAIN1 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH2 was not found
*ERROR* (Find_Xact_Match): FASTGAP2 was not found
*ERROR* (Find_Xact_Match): SLOWLENGTH2 was not found
*ERROR* (Find_Xact_Match): SLOWGAP2 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH2 was not found
*ERROR* (Find_Xact_Match): FASTTHRESH2 was not found
*ERROR* (Find_Xact_Match): SGA2 was not found
*ERROR* (Find_Xact_Match): DIGGAIN2 was not found
*ERROR* (Find_Xact_Match): TRACKDAC2 was not found
*ERROR* (Find_Xact_Match): PREAMPTAUA2 was not found
*ERROR* (Find_Xact_Match): TRACELENGTH2 was not found
*ERROR* (Find_Xact_Match): USERDELAY2 was not found
*ERROR* (Find_Xact_Match): PSAOFFSET2 was not found
*ERROR* (Find_Xact_Match): PSALENGTH2 was not found
*ERROR* (Find_Xact_Match): XWAIT2 was not found
*ERROR* (Find_Xact_Match): BASELINEPERCENT2 was not found
*ERROR* (Find_Xact_Match): CHANCSRA2 was not found
*ERROR* (Find_Xact_Match): CHANCSRB2 was not found
*ERROR* (Find_Xact_Match): CHANCSRC2 was not found
*ERROR* (Find_Xact_Match): CFDTHR2 was not found
*ERROR* (Find_Xact_Match): FCFD_THRESHOLD was not found
*ERROR* (Find_Xact_Match): FCFDTH2 was not found
*ERROR* (Find_Xact_Match): LOG2EBIN2 was not found
*ERROR* (Find_Xact_Match): INTEGRATOR2 was not found
*ERROR* (Find_Xact_Match): GATEWINDOW2 was not found
*ERROR* (Find_Xact_Match): GATEDELAY2 was not found
*ERROR* (Find_Xact_Match): QDC0LENGTH2 was not found
*ERROR* (Find_Xact_Match): QDC1LENGTH2 was not found
*ERROR* (Find_Xact_Match): QDC0DELAY2 was not found
*ERROR* (Find_Xact_Match): QDC1DELAY2 was not found
*ERROR* (Find_Xact_Match): BLCUT2 was not found
*ERROR* (Find_Xact_Match): COINCDELAY2 was not found
*ERROR* (Find_Xact_Match): LOG2BWEIGHT2 was not found
*ERROR* (Find_Xact_Match): FTDTA2 was not found
*ERROR* (Find_Xact_Match): NOUTA2 was not found
*ERROR* (Find_Xact_Match): NPPIA2 was not found
*ERROR* (Find_Xact_Match): NOUTA2 was not found
*ERROR* (Find_Xact_Match): NPPIA2 was not found
*ERROR* (Find_Xact_Match): FTDTA2 was not found
*ERROR* (Find_Xact_Match): GCOUNTA2 was not found
*ERROR* (Find_Xact_Match): GCOUNTA2 was not found
*ERROR* (Find_Xact_Match): SFDTA2 was not found
*ERROR* (Find_Xact_Match): GDTA2 was not found
*ERROR* (Find_Xact_Match): ICR2 was not found
*ERROR* (Find_Xact_Match): OORF2 was not found
*ERROR* (Find_Xact_Match): CH_EXTRA_IN was not found
*ERROR* (Find_Xact_Match): CHEXTRAIN2 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH3 was not found
*ERROR* (Find_Xact_Match): FASTGAP3 was not found
*ERROR* (Find_Xact_Match): SLOWLENGTH3 was not found
*ERROR* (Find_Xact_Match): SLOWGAP3 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH3 was not found
*ERROR* (Find_Xact_Match): FASTTHRESH3 was not found
*ERROR* (Find_Xact_Match): SGA3 was not found
*ERROR* (Find_Xact_Match): DIGGAIN3 was not found
*ERROR* (Find_Xact_Match): TRACKDAC3 was not found
*ERROR* (Find_Xact_Match): PREAMPTAUA3 was not found
*ERROR* (Find_Xact_Match): TRACELENGTH3 was not found
*ERROR* (Find_Xact_Match): USERDELAY3 was not found
*ERROR* (Find_Xact_Match): PSAOFFSET3 was not found
*ERROR* (Find_Xact_Match): PSALENGTH3 was not found
*ERROR* (Find_Xact_Match): XWAIT3 was not found
*ERROR* (Find_Xact_Match): BASELINEPERCENT3 was not found
*ERROR* (Find_Xact_Match): CHANCSRA3 was not found
*ERROR* (Find_Xact_Match): CHANCSRB3 was not found
*ERROR* (Find_Xact_Match): CHANCSRC3 was not found
*ERROR* (Find_Xact_Match): CFDTHR3 was not found
*ERROR* (Find_Xact_Match): FCFD_THRESHOLD was not found
*ERROR* (Find_Xact_Match): FCFDTH3 was not found
*ERROR* (Find_Xact_Match): LOG2EBIN3 was not found
*ERROR* (Find_Xact_Match): INTEGRATOR3 was not found
*ERROR* (Find_Xact_Match): GATEWINDOW3 was not found
*ERROR* (Find_Xact_Match): GATEDELAY3 was not found
*ERROR* (Find_Xact_Match): QDC0LENGTH3 was not found
*ERROR* (Find_Xact_Match): QDC1LENGTH3 was not found
*ERROR* (Find_Xact_Match): QDC0DELAY3 was not found
*ERROR* (Find_Xact_Match): QDC1DELAY3 was not found
*ERROR* (Find_Xact_Match): BLCUT3 was not found
*ERROR* (Find_Xact_Match): COINCDELAY3 was not found
*ERROR* (Find_Xact_Match): LOG2BWEIGHT3 was not found
*ERROR* (Find_Xact_Match): FTDTA3 was not found
*ERROR* (Find_Xact_Match): NOUTA3 was not found
*ERROR* (Find_Xact_Match): NPPIA3 was not found
*ERROR* (Find_Xact_Match): NOUTA3 was not found
*ERROR* (Find_Xact_Match): NPPIA3 was not found
*ERROR* (Find_Xact_Match): FTDTA3 was not found
*ERROR* (Find_Xact_Match): GCOUNTA3 was not found
*ERROR* (Find_Xact_Match): GCOUNTA3 was not found
*ERROR* (Find_Xact_Match): SFDTA3 was not found
*ERROR* (Find_Xact_Match): GDTA3 was not found
*ERROR* (Find_Xact_Match): ICR3 was not found
*ERROR* (Find_Xact_Match): OORF3 was not found
*ERROR* (Find_Xact_Match): CH_EXTRA_IN was not found
*ERROR* (Find_Xact_Match): CHEXTRAIN3 was not found
chan 2: par 0 CHANNEL_CSRA = 0.000000
chan 2: par 1 CHANNEL_CSRB = 0.000000
chan 2: par 2 ENERGY_RISETIME = 0.000000
chan 2: par 3 ENERGY_FLATTOP = 0.000000
chan 2: par 4 TRIGGER_RISETIME = 0.000000
chan 2: par 5 TRIGGER_FLATTOP = 0.000000
chan 2: par 6 TRIGGER_THRESHOLD = 0.000000
chan 2: par 7 VGAIN = 1.600000
chan 2: par 8 VOFFSET = 2.500000
chan 2: par 9 TRACE_LENGTH = 0.000000
chan 2: par 10 TRACE_DELAY = 0.000000
chan 2: par 11 PSA_START = 0.000000
chan 2: par 12 PSA_END = 0.000000
chan 2: par 13  = 0.000000
chan 2: par 14 BINFACTOR = 65536.000000
chan 2: par 15 TAU = 0.000000
chan 2: par 16 BLCUT = 0.000000
chan 2: par 17 XDT = 0.000000
chan 2: par 18 BASELINE_PERCENT = 0.000000
chan 2: par 19 CFD_THRESHOLD = 0.000000
chan 2: par 20 INTEGRATOR = 0.000000
chan 2: par 21 CHANNEL_CSRC = 0.000000
chan 2: par 22 GATE_WINDOW = 0.000000
chan 2: par 23 GATE_DELAY = 0.000000
chan 2: par 24 COINC_DELAY = 0.000000
chan 2: par 25 BLAVG = 0.000000
chan 2: par 26 COUNT_TIME = 0.000000
chan 2: par 27 INPUT_COUNT_RATE = 0.000000
chan 2: par 28 FAST_PEAKS = 0.000000
chan 2: par 29 OUTPUT_COUNT_RATE = 0.000000
chan 2: par 30 NOUT = 0.000000
chan 2: par 31 GATE_RATE = 0.000000
chan 2: par 32 GATE_COUNTS = 0.000000
chan 2: par 33 FTDT = 0.000000
chan 2: par 34 SFDT = 0.000000
chan 2: par 35 GDT = 0.000000
chan 2: par 36 CURRENT_ICR = 0.000000
chan 2: par 37 CURRENT_OORF = 0.000000
chan 2: par 38 PSM_GAIN_AVG = 3.141593
chan 2: par 39 PSM_GAIN_AVG_LEN = 10.000000
chan 2: par 40 PSM_TEMP_AVG = 3.141593
chan 2: par 41 PSM_TEMP_AVG_LEN = 10.000000
chan 2: par 42 PSM_GAIN_CORR = 10.000000
chan 2: par 43 QDC0_LENGTH = 0.000000
chan 2: par 44 QDC1_LENGTH = 0.000000
chan 2: par 45 QDC0_DELAY = 0.000000
chan 2: par 46 QDC1_DELAY = 0.000000
chan 2: par 47 NPPI = 0.000000
chan 2: par 48 PASS_PILEUP_RATE = 0.000000
chan 2: par 49  = 0.000000
chan 2: par 50  = 0.000000
chan 2: par 51  = 0.000000
chan 2: par 52  = 0.000000
chan 2: par 53  = 0.000000
chan 2: par 54  = 0.000000
chan 2: par 55  = 0.000000
chan 2: par 56  = 0.000000
chan 2: par 57  = 0.000000
chan 2: par 58  = 0.000000
chan 2: par 59  = 0.000000
chan 2: par 60  = 0.000000
chan 2: par 61  = 0.000000
chan 2: par 62  = 0.000000
chan 2: par 63  = 0.000000
*ERROR* (Find_Xact_Match): FASTLENGTH0 was not found
*ERROR* (Find_Xact_Match): FASTGAP0 was not found
*ERROR* (Find_Xact_Match): SLOWLENGTH0 was not found
*ERROR* (Find_Xact_Match): SLOWGAP0 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH0 was not found
*ERROR* (Find_Xact_Match): FASTTHRESH0 was not found
*ERROR* (Find_Xact_Match): SGA0 was not found
*ERROR* (Find_Xact_Match): DIGGAIN0 was not found
*ERROR* (Find_Xact_Match): TRACKDAC0 was not found
*ERROR* (Find_Xact_Match): PREAMPTAUA0 was not found
*ERROR* (Find_Xact_Match): TRACELENGTH0 was not found
*ERROR* (Find_Xact_Match): USERDELAY0 was not found
*ERROR* (Find_Xact_Match): PSAOFFSET0 was not found
*ERROR* (Find_Xact_Match): PSALENGTH0 was not found
*ERROR* (Find_Xact_Match): XWAIT0 was not found
*ERROR* (Find_Xact_Match): BASELINEPERCENT0 was not found
*ERROR* (Find_Xact_Match): CHANCSRA0 was not found
*ERROR* (Find_Xact_Match): CHANCSRB0 was not found
*ERROR* (Find_Xact_Match): CHANCSRC0 was not found
*ERROR* (Find_Xact_Match): CFDTHR0 was not found
*ERROR* (Find_Xact_Match): FCFD_THRESHOLD was not found
*ERROR* (Find_Xact_Match): FCFDTH0 was not found
*ERROR* (Find_Xact_Match): LOG2EBIN0 was not found
*ERROR* (Find_Xact_Match): INTEGRATOR0 was not found
*ERROR* (Find_Xact_Match): GATEWINDOW0 was not found
*ERROR* (Find_Xact_Match): GATEDELAY0 was not found
*ERROR* (Find_Xact_Match): QDC0LENGTH0 was not found
*ERROR* (Find_Xact_Match): QDC1LENGTH0 was not found
*ERROR* (Find_Xact_Match): QDC0DELAY0 was not found
*ERROR* (Find_Xact_Match): QDC1DELAY0 was not found
*ERROR* (Find_Xact_Match): BLCUT0 was not found
*ERROR* (Find_Xact_Match): COINCDELAY0 was not found
*ERROR* (Find_Xact_Match): LOG2BWEIGHT0 was not found
*ERROR* (Find_Xact_Match): FTDTA0 was not found
*ERROR* (Find_Xact_Match): NOUTA0 was not found
*ERROR* (Find_Xact_Match): NPPIA0 was not found
*ERROR* (Find_Xact_Match): NOUTA0 was not found
*ERROR* (Find_Xact_Match): NPPIA0 was not found
*ERROR* (Find_Xact_Match): FTDTA0 was not found
*ERROR* (Find_Xact_Match): GCOUNTA0 was not found
*ERROR* (Find_Xact_Match): GCOUNTA0 was not found
*ERROR* (Find_Xact_Match): SFDTA0 was not found
*ERROR* (Find_Xact_Match): GDTA0 was not found
*ERROR* (Find_Xact_Match): ICR0 was not found
*ERROR* (Find_Xact_Match): OORF0 was not found
*ERROR* (Find_Xact_Match): CH_EXTRA_IN was not found
*ERROR* (Find_Xact_Match): CHEXTRAIN0 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH1 was not found
*ERROR* (Find_Xact_Match): FASTGAP1 was not found
*ERROR* (Find_Xact_Match): SLOWLENGTH1 was not found
*ERROR* (Find_Xact_Match): SLOWGAP1 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH1 was not found
*ERROR* (Find_Xact_Match): FASTTHRESH1 was not found
*ERROR* (Find_Xact_Match): SGA1 was not found
*ERROR* (Find_Xact_Match): DIGGAIN1 was not found
*ERROR* (Find_Xact_Match): TRACKDAC1 was not found
*ERROR* (Find_Xact_Match): PREAMPTAUA1 was not found
*ERROR* (Find_Xact_Match): TRACELENGTH1 was not found
*ERROR* (Find_Xact_Match): USERDELAY1 was not found
*ERROR* (Find_Xact_Match): PSAOFFSET1 was not found
*ERROR* (Find_Xact_Match): PSALENGTH1 was not found
*ERROR* (Find_Xact_Match): XWAIT1 was not found
*ERROR* (Find_Xact_Match): BASELINEPERCENT1 was not found
*ERROR* (Find_Xact_Match): CHANCSRA1 was not found
*ERROR* (Find_Xact_Match): CHANCSRB1 was not found
*ERROR* (Find_Xact_Match): CHANCSRC1 was not found
*ERROR* (Find_Xact_Match): CFDTHR1 was not found
*ERROR* (Find_Xact_Match): FCFD_THRESHOLD was not found
*ERROR* (Find_Xact_Match): FCFDTH1 was not found
*ERROR* (Find_Xact_Match): LOG2EBIN1 was not found
*ERROR* (Find_Xact_Match): INTEGRATOR1 was not found
*ERROR* (Find_Xact_Match): GATEWINDOW1 was not found
*ERROR* (Find_Xact_Match): GATEDELAY1 was not found
*ERROR* (Find_Xact_Match): QDC0LENGTH1 was not found
*ERROR* (Find_Xact_Match): QDC1LENGTH1 was not found
*ERROR* (Find_Xact_Match): QDC0DELAY1 was not found
*ERROR* (Find_Xact_Match): QDC1DELAY1 was not found
*ERROR* (Find_Xact_Match): BLCUT1 was not found
*ERROR* (Find_Xact_Match): COINCDELAY1 was not found
*ERROR* (Find_Xact_Match): LOG2BWEIGHT1 was not found
*ERROR* (Find_Xact_Match): FTDTA1 was not found
*ERROR* (Find_Xact_Match): NOUTA1 was not found
*ERROR* (Find_Xact_Match): NPPIA1 was not found
*ERROR* (Find_Xact_Match): NOUTA1 was not found
*ERROR* (Find_Xact_Match): NPPIA1 was not found
*ERROR* (Find_Xact_Match): FTDTA1 was not found
*ERROR* (Find_Xact_Match): GCOUNTA1 was not found
*ERROR* (Find_Xact_Match): GCOUNTA1 was not found
*ERROR* (Find_Xact_Match): SFDTA1 was not found
*ERROR* (Find_Xact_Match): GDTA1 was not found
*ERROR* (Find_Xact_Match): ICR1 was not found
*ERROR* (Find_Xact_Match): OORF1 was not found
*ERROR* (Find_Xact_Match): CH_EXTRA_IN was not found
*ERROR* (Find_Xact_Match): CHEXTRAIN1 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH2 was not found
*ERROR* (Find_Xact_Match): FASTGAP2 was not found
*ERROR* (Find_Xact_Match): SLOWLENGTH2 was not found
*ERROR* (Find_Xact_Match): SLOWGAP2 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH2 was not found
*ERROR* (Find_Xact_Match): FASTTHRESH2 was not found
*ERROR* (Find_Xact_Match): SGA2 was not found
*ERROR* (Find_Xact_Match): DIGGAIN2 was not found
*ERROR* (Find_Xact_Match): TRACKDAC2 was not found
*ERROR* (Find_Xact_Match): PREAMPTAUA2 was not found
*ERROR* (Find_Xact_Match): TRACELENGTH2 was not found
*ERROR* (Find_Xact_Match): USERDELAY2 was not found
*ERROR* (Find_Xact_Match): PSAOFFSET2 was not found
*ERROR* (Find_Xact_Match): PSALENGTH2 was not found
*ERROR* (Find_Xact_Match): XWAIT2 was not found
*ERROR* (Find_Xact_Match): BASELINEPERCENT2 was not found
*ERROR* (Find_Xact_Match): CHANCSRA2 was not found
*ERROR* (Find_Xact_Match): CHANCSRB2 was not found
*ERROR* (Find_Xact_Match): CHANCSRC2 was not found
*ERROR* (Find_Xact_Match): CFDTHR2 was not found
*ERROR* (Find_Xact_Match): FCFD_THRESHOLD was not found
*ERROR* (Find_Xact_Match): FCFDTH2 was not found
*ERROR* (Find_Xact_Match): LOG2EBIN2 was not found
*ERROR* (Find_Xact_Match): INTEGRATOR2 was not found
*ERROR* (Find_Xact_Match): GATEWINDOW2 was not found
*ERROR* (Find_Xact_Match): GATEDELAY2 was not found
*ERROR* (Find_Xact_Match): QDC0LENGTH2 was not found
*ERROR* (Find_Xact_Match): QDC1LENGTH2 was not found
*ERROR* (Find_Xact_Match): QDC0DELAY2 was not found
*ERROR* (Find_Xact_Match): QDC1DELAY2 was not found
*ERROR* (Find_Xact_Match): BLCUT2 was not found
*ERROR* (Find_Xact_Match): COINCDELAY2 was not found
*ERROR* (Find_Xact_Match): LOG2BWEIGHT2 was not found
*ERROR* (Find_Xact_Match): FTDTA2 was not found
*ERROR* (Find_Xact_Match): NOUTA2 was not found
*ERROR* (Find_Xact_Match): NPPIA2 was not found
*ERROR* (Find_Xact_Match): NOUTA2 was not found
*ERROR* (Find_Xact_Match): NPPIA2 was not found
*ERROR* (Find_Xact_Match): FTDTA2 was not found
*ERROR* (Find_Xact_Match): GCOUNTA2 was not found
*ERROR* (Find_Xact_Match): GCOUNTA2 was not found
*ERROR* (Find_Xact_Match): SFDTA2 was not found
*ERROR* (Find_Xact_Match): GDTA2 was not found
*ERROR* (Find_Xact_Match): ICR2 was not found
*ERROR* (Find_Xact_Match): OORF2 was not found
*ERROR* (Find_Xact_Match): CH_EXTRA_IN was not found
*ERROR* (Find_Xact_Match): CHEXTRAIN2 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH3 was not found
*ERROR* (Find_Xact_Match): FASTGAP3 was not found
*ERROR* (Find_Xact_Match): SLOWLENGTH3 was not found
*ERROR* (Find_Xact_Match): SLOWGAP3 was not found
*ERROR* (Find_Xact_Match): FASTLENGTH3 was not found
*ERROR* (Find_Xact_Match): FASTTHRESH3 was not found
*ERROR* (Find_Xact_Match): SGA3 was not found
*ERROR* (Find_Xact_Match): DIGGAIN3 was not found
*ERROR* (Find_Xact_Match): TRACKDAC3 was not found
*ERROR* (Find_Xact_Match): PREAMPTAUA3 was not found
*ERROR* (Find_Xact_Match): TRACELENGTH3 was not found
*ERROR* (Find_Xact_Match): USERDELAY3 was not found
*ERROR* (Find_Xact_Match): PSAOFFSET3 was not found
*ERROR* (Find_Xact_Match): PSALENGTH3 was not found
*ERROR* (Find_Xact_Match): XWAIT3 was not found
*ERROR* (Find_Xact_Match): BASELINEPERCENT3 was not found
*ERROR* (Find_Xact_Match): CHANCSRA3 was not found
*ERROR* (Find_Xact_Match): CHANCSRB3 was not found
*ERROR* (Find_Xact_Match): CHANCSRC3 was not found
*ERROR* (Find_Xact_Match): CFDTHR3 was not found
*ERROR* (Find_Xact_Match): FCFD_THRESHOLD was not found
*ERROR* (Find_Xact_Match): FCFDTH3 was not found
*ERROR* (Find_Xact_Match): LOG2EBIN3 was not found
*ERROR* (Find_Xact_Match): INTEGRATOR3 was not found
*ERROR* (Find_Xact_Match): GATEWINDOW3 was not found
*ERROR* (Find_Xact_Match): GATEDELAY3 was not found
*ERROR* (Find_Xact_Match): QDC0LENGTH3 was not found
*ERROR* (Find_Xact_Match): QDC1LENGTH3 was not found
*ERROR* (Find_Xact_Match): QDC0DELAY3 was not found
*ERROR* (Find_Xact_Match): QDC1DELAY3 was not found
*ERROR* (Find_Xact_Match): BLCUT3 was not found
*ERROR* (Find_Xact_Match): COINCDELAY3 was not found
*ERROR* (Find_Xact_Match): LOG2BWEIGHT3 was not found
*ERROR* (Find_Xact_Match): FTDTA3 was not found
*ERROR* (Find_Xact_Match): NOUTA3 was not found
*ERROR* (Find_Xact_Match): NPPIA3 was not found
*ERROR* (Find_Xact_Match): NOUTA3 was not found
*ERROR* (Find_Xact_Match): NPPIA3 was not found
*ERROR* (Find_Xact_Match): FTDTA3 was not found
*ERROR* (Find_Xact_Match): GCOUNTA3 was not found
*ERROR* (Find_Xact_Match): GCOUNTA3 was not found
*ERROR* (Find_Xact_Match): SFDTA3 was not found
*ERROR* (Find_Xact_Match): GDTA3 was not found
*ERROR* (Find_Xact_Match): ICR3 was not found
*ERROR* (Find_Xact_Match): OORF3 was not found
*ERROR* (Find_Xact_Match): CH_EXTRA_IN was not found
*ERROR* (Find_Xact_Match): CHEXTRAIN3 was not found
chan 3: par 0 CHANNEL_CSRA = 0.000000
chan 3: par 1 CHANNEL_CSRB = 0.000000
chan 3: par 2 ENERGY_RISETIME = 0.000000
chan 3: par 3 ENERGY_FLATTOP = 0.000000
chan 3: par 4 TRIGGER_RISETIME = 0.000000
chan 3: par 5 TRIGGER_FLATTOP = 0.000000
chan 3: par 6 TRIGGER_THRESHOLD = 0.000000
chan 3: par 7 VGAIN = 1.600000
chan 3: par 8 VOFFSET = 2.500000
chan 3: par 9 TRACE_LENGTH = 0.000000
chan 3: par 10 TRACE_DELAY = 0.000000
chan 3: par 11 PSA_START = 0.000000
chan 3: par 12 PSA_END = 0.000000
chan 3: par 13  = 0.000000
chan 3: par 14 BINFACTOR = 65536.000000
chan 3: par 15 TAU = 0.000000
chan 3: par 16 BLCUT = 0.000000
chan 3: par 17 XDT = 0.000000
chan 3: par 18 BASELINE_PERCENT = 0.000000
chan 3: par 19 CFD_THRESHOLD = 0.000000
chan 3: par 20 INTEGRATOR = 0.000000
chan 3: par 21 CHANNEL_CSRC = 0.000000
chan 3: par 22 GATE_WINDOW = 0.000000
chan 3: par 23 GATE_DELAY = 0.000000
chan 3: par 24 COINC_DELAY = 0.000000
chan 3: par 25 BLAVG = 0.000000
chan 3: par 26 COUNT_TIME = 0.000000
chan 3: par 27 INPUT_COUNT_RATE = 0.000000
chan 3: par 28 FAST_PEAKS = 0.000000
chan 3: par 29 OUTPUT_COUNT_RATE = 0.000000
chan 3: par 30 NOUT = 0.000000
chan 3: par 31 GATE_RATE = 0.000000
chan 3: par 32 GATE_COUNTS = 0.000000
chan 3: par 33 FTDT = 0.000000
chan 3: par 34 SFDT = 0.000000
chan 3: par 35 GDT = 0.000000
chan 3: par 36 CURRENT_ICR = 0.000000
chan 3: par 37 CURRENT_OORF = 0.000000
chan 3: par 38 PSM_GAIN_AVG = 3.141593
chan 3: par 39 PSM_GAIN_AVG_LEN = 10.000000
chan 3: par 40 PSM_TEMP_AVG = 3.141593
chan 3: par 41 PSM_TEMP_AVG_LEN = 10.000000
chan 3: par 42 PSM_GAIN_CORR = 10.000000
chan 3: par 43 QDC0_LENGTH = 0.000000
chan 3: par 44 QDC1_LENGTH = 0.000000
chan 3: par 45 QDC0_DELAY = 0.000000
chan 3: par 46 QDC1_DELAY = 0.000000
chan 3: par 47 NPPI = 0.000000
chan 3: par 48 PASS_PILEUP_RATE = 0.000000
chan 3: par 49  = 0.000000
chan 3: par 50  = 0.000000
chan 3: par 51  = 0.000000
chan 3: par 52  = 0.000000
chan 3: par 53  = 0.000000
chan 3: par 54  = 0.000000
chan 3: par 55  = 0.000000
chan 3: par 56  = 0.000000
chan 3: par 57  = 0.000000
chan 3: par 58  = 0.000000
chan 3: par 59  = 0.000000
chan 3: par 60  = 0.000000
chan 3: par 61  = 0.000000
chan 3: par 62  = 0.000000
chan 3: par 63  = 0.000000
TRACE_LENGTH: 0.000000 0.000000 0.000000 0.000000
Make 5-sec general list mode run ...
*ERROR* (Find_Xact_Match): COINCPATTERN was not found
*ERROR* (Find_Xact_Match): COINCWAIT was not found
*ERROR* (Find_Xact_Match): TRACELENGTH0 was not found
*ERROR* (Find_Xact_Match): TRACELENGTH1 was not found
*ERROR* (Find_Xact_Match): TRACELENGTH2 was not found
*ERROR* (Find_Xact_Match): TRACELENGTH3 was not found
*ERROR* (Find_Xact_Match): CHANCSRC0 was not found
*ERROR* (Find_Xact_Match): CHANCSRC1 was not found
*ERROR* (Find_Xact_Match): CHANCSRC2 was not found
*ERROR* (Find_Xact_Match): CHANCSRC3 was not found
*DEBUG* (PIXIE500E_InterruptSetup_INT3): ready to assert INT_STAT=0x00000000
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Pixie_Acquire_Data): Failed to read Run Status, aborting
Info: list mode run 0x400 ended

----

## ADC


*INFO* (Pixie_Scan_Crate_Slots_GN):PCI bus is EXPRESS type
*INFO* (Pixie_Scan_Crate_Slots_GN): Device 0, Board version= 0xA551, S/N = 231
*ERROR* (Find_Xact_Match): RUNTASK was not found
*ERROR* (Find_Xact_Match): CONTROLTASK was not found
*ERROR* (Find_Xact_Match): RESUME was not found
*ERROR* (Find_Xact_Match): RUNTIMEA was not found
*ERROR* (Find_Xact_Match): NUMEVENTSA was not found
*ERROR* (Find_Xact_Match): SYNCHDONE was not found
*ERROR* (Find_Xact_Match): FASTPEAKSA0 was not found
*ERROR* (Find_Xact_Match): COUNTTIMEA0 was not found
*ERROR* (Find_Xact_Match): FASTPEAKSA1 was not found
*ERROR* (Find_Xact_Match): COUNTTIMEA1 was not found
*ERROR* (Find_Xact_Match): FASTPEAKSA2 was not found
*ERROR* (Find_Xact_Match): COUNTTIMEA2 was not found
*ERROR* (Find_Xact_Match): FASTPEAKSA3 was not found
*ERROR* (Find_Xact_Match): COUNTTIMEA3 was not found
*ERROR* (Find_Xact_Match): FILTERRANGE was not found
*ERROR* (Find_Xact_Match): CCONTROL was not found
*INFO* (Pixie_Boot): Revision 0xA551
*INFO* (PIXIE4E_ProgramFPGA(): BEGIN
*WARNING* (PIXIE4E_ProgramFPGA(): FPGA already programmed, skipping. Power cycle chassis to reprogram
Module 0 in slot 231 started up successfully!

Adjusting offsets ...
*ERROR* (Find_Xact_Match): CHANCSRA0 was not found
*ERROR* (Find_Xact_Match): CHANCSRA0 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): BASELINEPERCENT0 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): HOSTODATA was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): TRACKDAC0 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): TRACKDAC1 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): TRACKDAC2 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Find_Xact_Match): TRACKDAC3 was not found
*ERROR* (Pixie_IODM) Write to wrong address 16895.

Acquire ADC trace ...
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (Pixie_IODM) Write to wrong address 16895.
*ERROR* (VDMADriver_Halt): Stopping DMA failed
*DEBUG* (PIXIE500E_DMA_WaitForCompletion): Timeout in wait loop, halting DMA
*ERROR* (Get_Traces) IO_BUFFER read: DMA transfer timed out.
*ERROR* (Pixie_Acquire_Data): failure to acquire ADC traces in Module 0, retval=-2
Acquire ADC trace failed, retval=-3




<!-- note.md ends here -->
