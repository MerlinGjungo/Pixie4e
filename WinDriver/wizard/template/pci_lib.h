#ifndef _$$$nu$$$_LIB_H_
#define _$$$nu$$$_LIB_H_

/************************************************************************
*  File: $$$nl$$$_lib.h
*
*  Library for accessing $$$nu$$$ devices, possibly using a Kernel PlugIn driver.
*  The code accesses hardware using WinDriver's WDC library.
@@@CODE_GEN@@@
*
*  Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com
*************************************************************************/

#include "wdc_lib.h"
@@@kp@@@
#include "wdc_defs.h"
@@@kp@@@
#ifndef ISA
#include "$$$nl$$$_regs.h"
#endif /* ifndef ISA */
#include "bits.h"

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************
  General definitions
 *************************************************************/
/* Kernel PlugIn driver name (should be no more than 8 characters) */
#define KP_$$$nu$$$_DRIVER_NAME "KP_$$$nu$$$"

@@@kp@@@
/* Kernel PlugIn messages - used in WDC_CallKerPlug() calls (user mode) /
 * KP_$$$nu$$$_Call() (kernel mode) */
enum {
    KP_$$$nu$$$_MSG_VERSION = 1, /* Query the version of the Kernel PlugIn */
};

/* Kernel PlugIn messages status */
enum {
    KP_$$$nu$$$_STATUS_OK          = 0x1,
    KP_$$$nu$$$_STATUS_MSG_NO_IMPL = 0x1000,
    KP_$$$nu$$$_STATUS_FAIL        = 0x1001,
};
@@@kp@@@
#ifndef ISA

/* Default vendor and device IDs (0 == all) */
/* TODO: Replace the ID values with your device's vendor and device IDs */
#define $$$nu$$$_DEFAULT_VENDOR_ID 0x0 /* Vendor ID */
#define $$$nu$$$_DEFAULT_DEVICE_ID 0x0 /* Device ID */

#else /* ifdef ISA */

/* ISA address spaces information */
#define $$$nu$$$_ADDR_SPACES_NUM 0 /* Number of address spaces */

/* Base address spaces numbers */
@@@BaseAddrSpaceNum@@@

/* Physical base addresses */
@@@PhysicalBaseAddresses@@@

/* Size (in bytes) of address spaces */
@@@SizeOfAddrSpaces@@@

#ifdef HAS_INTS
/* Interrupts information */
#define $$$nu$$$_INTS_NUM 1 /* Number of interrupts */

/* Interrupt Request (IRQ) */
/* @@@Irqs@@@ */

/* Interrupt registration options */
@@@InterruptOptions@@@
#endif /* ifdef HAS_INTS */

// TODO: Add address space info
#endif /* ifdef ISA */

#ifdef ISA
/* Total number of items - address spaces, interrupts and bus items */
#define $$$nu$$$_ITEMS_NUM $$$nu$$$_ADDR_SPACES_NUM + 1
#endif /* ifdef ISA */


@@@kp@@@
/* Kernel PlugIn version information struct */
typedef struct {
    DWORD dwVer;
    CHAR cVer[100];
} KP_$$$nu$$$_VERSION;

/* Device address description struct */
typedef struct {
    DWORD dwNumAddrSpaces;    /* Total number of device address spaces */
    WDC_ADDR_DESC *pAddrDesc; /* Array of device address spaces information */
} $$$nu$$$_DEV_ADDR_DESC;
@@@kp@@@

/* Address space information struct */
#define MAX_TYPE 8
typedef struct {
    DWORD dwAddrSpace;
    CHAR sType[MAX_TYPE];
    CHAR sName[MAX_NAME];
    CHAR sDesc[MAX_DESC];
} $$$nu$$$_ADDR_SPACE_INFO;

#ifdef HAS_INTS
/* Interrupt result information struct */
typedef struct {
    DWORD dwCounter; /* Number of interrupts received */
    DWORD dwLost;    /* Number of interrupts not yet handled */
    WD_INTERRUPT_WAIT_RESULT waitResult; /* See WD_INTERRUPT_WAIT_RESULT values
                                            in windrvr.h */
    DWORD dwEnabledIntType; /* Interrupt type that was actually enabled
                               (MSI/MSI-X / Level Sensitive / Edge-Triggered) */
    DWORD dwLastMessage; /* Message data of the last received MSI/MSI-X
                            (Windows Vista and higher); N/A to line-based
                            interrupts. */
} $$$nu$$$_INT_RESULT;
/* TODO: You can add fields to $$$nu$$$_INT_RESULT to store any additional
         information that you wish to pass to your diagnostics interrupt
         handler routine (DiagIntHandler() in $$$nl$$$_diag.c). */

/* $$$nu$$$ diagnostics interrupt handler function type */
typedef void (*$$$nu$$$_INT_HANDLER)(WDC_DEVICE_HANDLE hDev,
    $$$nu$$$_INT_RESULT *pIntResult);
#endif /* ifdef HAS_INTS */

#ifndef ISA
/* $$$nu$$$ diagnostics plug-and-play and power management events handler function
 * type */
typedef void (*$$$nu$$$_EVENT_HANDLER)(WDC_DEVICE_HANDLE hDev, DWORD dwAction);
#endif /* ifndef ISA */

@@@regs_lib_enums@@@

/*************************************************************
  Function prototypes
 *************************************************************/
/* -----------------------------------------------
    $$$nu$$$ and WDC libraries initialize/uninitialize
   ----------------------------------------------- */
/* Initialize the $$$nu$$$ and WDC libraries */
DWORD $$$nu$$$_LibInit(void);
/* Uninitialize the $$$nu$$$ and WDC libraries */
DWORD $$$nu$$$_LibUninit(void);

#ifndef __KERNEL__
/* -----------------------------------------------
    Device open/close
   ----------------------------------------------- */
/* Open a device handle */
#ifndef ISA
WDC_DEVICE_HANDLE $$$nu$$$_DeviceOpen(const WD_$$$nu$$$_CARD_INFO *pDeviceInfo);
#else /* ifdef ISA */
WDC_DEVICE_HANDLE $$$nu$$$_DeviceOpen(void);
#endif /* ifdef ISA */
/* Close a device handle */
BOOL $$$nu$$$_DeviceClose(WDC_DEVICE_HANDLE hDev);

#ifdef HAS_INTS
/* -----------------------------------------------
    Interrupts
   ----------------------------------------------- */
/* Enable interrupts */
DWORD $$$nu$$$_IntEnable(WDC_DEVICE_HANDLE hDev, $$$nu$$$_INT_HANDLER funcIntHandler);
/* Disable interrupts */
DWORD $$$nu$$$_IntDisable(WDC_DEVICE_HANDLE hDev);
/* Check whether interrupts are enabled for the given device */
BOOL $$$nu$$$_IntIsEnabled(WDC_DEVICE_HANDLE hDev);
#endif /* ifdef HAS_INTS */

#ifndef ISA
/* -----------------------------------------------
    Plug-and-play and power management events
   ----------------------------------------------- */
/* Register a plug-and-play or power management event */
DWORD $$$nu$$$_EventRegister(WDC_DEVICE_HANDLE hDev,
    $$$nu$$$_EVENT_HANDLER funcEventHandler);
/* Unregister a plug-and-play or power management event */
DWORD $$$nu$$$_EventUnregister(WDC_DEVICE_HANDLE hDev);
/* Check whether a given plug-and-play or power management event is registered
 */
BOOL $$$nu$$$_EventIsRegistered(WDC_DEVICE_HANDLE hDev);
#endif /* ifndef ISA */
#endif /* ifndef __KERNEL */

#ifndef ISA
/* -----------------------------------------------
    Address spaces information
   ----------------------------------------------- */
/* Get number of address spaces */
DWORD $$$nu$$$_GetNumAddrSpaces(WDC_DEVICE_HANDLE hDev);
#endif /* ifndef ISA */
/* Get address space information */
BOOL $$$nu$$$_GetAddrSpaceInfo(WDC_DEVICE_HANDLE hDev,
    $$$nu$$$_ADDR_SPACE_INFO *pAddrSpaceInfo);

@@@regs_lib_rw_header@@@
/* -----------------------------------------------
    Debugging and error handling
   ----------------------------------------------- */
/* Get last error */
const char *$$$nu$$$_GetLastErr(void);

#ifdef __cplusplus
}
#endif

#endif /* _$$$nu$$$_LIB_H_ */

