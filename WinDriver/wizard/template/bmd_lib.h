/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

#ifndef _$$$nu$$$_LIB_H_
#define _$$$nu$$$_LIB_H_

/***************************************************************************
*  File: $$$nl$$$_lib.h
*
*  Header file of a sample library for accessing Xilinx PCI Express cards
*  with BMD design, using the WinDriver WDC API.
*  The sample was tested with Xilinx's Virtex and Spartan development kits.
****************************************************************************/

#include "wdc_defs.h"
#include "wdc_lib.h"
#include "pci_regs.h"
#include "bits.h"

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************
  General definitions
 *************************************************************/
@@@kp@@@
/* Kernel PlugIn driver name (should be no more than 8 characters) */
#define KP_$$$nu$$$_DRIVER_NAME "KP_$$$nu$$$"
/* Kernel PlugIn messages - used in WDC_CallKerPlug() calls (user mode) /
 * KP_$$$nu$$$_Call() (kernel mode) */
enum {
    KP_$$$nu$$$_MSG_VERSION = 1, /* Query the version of the Kernel PlugIn */
};

/* Kernel PlugIn messages status */
enum {
    KP_$$$nu$$$_STATUS_OK = 0x1,
    KP_$$$nu$$$_STATUS_MSG_NO_IMPL = 0x1000,
};
@@@kp@@@

/* Default vendor and device IDs (0 == all) */
#define $$$nu$$$_DEFAULT_VENDOR_ID 0x10EE    /* Vendor ID */
#define $$$nu$$$_DEFAULT_DEVICE_ID 0x0       /* All Xilinx devices */
  /* TODO: Change the device ID value to match your specific device. */

/* Direct Memory Access (DMA) information struct */
typedef struct {
    WD_DMA *pDma;
    WDC_DEVICE_HANDLE hDev;
} $$$nu$$$_DMA_STRUCT, *$$$nu$$$_DMA_HANDLE;

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

/* Interrupt result information struct */
typedef struct
{
    DWORD dwCounter;        /* Number of interrupts received */
    DWORD dwLost;           /* Number of interrupts not yet handled */
    WD_INTERRUPT_WAIT_RESULT waitResult; /* See WD_INTERRUPT_WAIT_RESULT values
                                            in windrvr.h */
    BOOL fIsMessageBased;
    DWORD dwLastMessage;    /* Message data of the last received MSI/MSI-X
                             * (Windows Vista and higher); N/A to line-based
                             * interrupts) */
    PVOID pBuf;             /* Pointer to a user-mode DMA data buffer */
    UINT32 u32Pattern;      /* 32-bit data pattern (used for DMA data) */
    DWORD dwBufNumItems;    /* Size of the pBuf buffer, in units of UINT32 */
    BOOL fIsRead; /* DMA direction: host-to-device=read; device-to-host=write */
} $$$nu$$$_INT_RESULT;
/* TODO: You can add fields to $$$nu$$$_INT_RESULT to store any additional
         information that you wish to pass to your diagnostics interrupt
         handler routine (DiagIntHandler() in $$$nl$$$_diag.c). */

/* $$$nu$$$ diagnostics interrupt handler function type */
typedef void (*$$$nu$$$_INT_HANDLER)(WDC_DEVICE_HANDLE hDev,
    $$$nu$$$_INT_RESULT *pIntResult);

/* $$$nu$$$ diagnostics plug-and-play and power management events handler
 * function type */
typedef void (*$$$nu$$$_EVENT_HANDLER)(WDC_DEVICE_HANDLE hDev, DWORD dwAction);

/* $$$nu$$$ registers address space (BAR 0) */
#define $$$nu$$$_SPACE AD_PCI_BAR0
/* $$$nu$$$ registers offsets */
enum {
    $$$nu$$$_DSCR_OFFSET = 0x0,
    $$$nu$$$_DDMACR_OFFSET = 0x4,
    $$$nu$$$_WDMATLPA_OFFSET = 0x8,
    $$$nu$$$_WDMATLPS_OFFSET = 0xc,
    $$$nu$$$_WDMATLPC_OFFSET = 0x10,
    $$$nu$$$_WDMATLPP_OFFSET = 0x14,
    $$$nu$$$_RDMATLPP_OFFSET = 0x18,
    $$$nu$$$_RDMATLPA_OFFSET = 0x1c,
    $$$nu$$$_RDMATLPS_OFFSET = 0x20,
    $$$nu$$$_RDMATLPC_OFFSET = 0x24,
    $$$nu$$$_WDMAPERF_OFFSET = 0x28,
    $$$nu$$$_RDMAPERF_OFFSET = 0x2c,
    $$$nu$$$_RDMASTAT_OFFSET = 0x30,
    $$$nu$$$_NRDCOMP_OFFSET = 0x34,
    $$$nu$$$_RCOMPDSIZE_OFFSET = 0x38,
    $$$nu$$$_DLWSTAT_OFFSET = 0x3c,
    $$$nu$$$_DLTRSSTAT_OFFSET = 0x40,
    $$$nu$$$_DMISCCONT_OFFSET = 0x44
};

/*************************************************************
  Function prototypes
 *************************************************************/
/* -----------------------------------------------
    $$$nu$$$ and WDC libraries initialize/uninitialize
   ----------------------------------------------- */
/* Initialize the Xilinx $$$nu$$$ and WDC libraries */
DWORD $$$nu$$$_LibInit(void);
/* Uninitialize the Xilinx $$$nu$$$ and WDC libraries */
DWORD $$$nu$$$_LibUninit(void);

#if !defined(__KERNEL__)
/* -----------------------------------------------
    Device open/close
   ----------------------------------------------- */
/* Open a device handle */
WDC_DEVICE_HANDLE $$$nu$$$_DeviceOpen(const WD_PCI_CARD_INFO *pDeviceInfo,
    char *kp_name);
/* Close a device handle */
BOOL $$$nu$$$_DeviceClose(WDC_DEVICE_HANDLE hDev);

/* -----------------------------------------------
    Interrupts
   ----------------------------------------------- */
/* Enable interrupts */
DWORD $$$nu$$$_IntEnable(WDC_DEVICE_HANDLE hDev, $$$nu$$$_INT_HANDLER funcIntHandler);
/* Disable interrupts */
DWORD $$$nu$$$_IntDisable(WDC_DEVICE_HANDLE hDev);
/* Check whether interrupts are enabled for the given device */
BOOL $$$nu$$$_IntIsEnabled(WDC_DEVICE_HANDLE hDev);

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

/* -----------------------------------------------
    Direct Memory Access (DMA)
   ----------------------------------------------- */
/* Open a DMA handle: Allocate and initialize a $$$nu$$$ DMA information structure,
 * including allocation of a contiguous DMA buffer */
DWORD $$$nu$$$_DmaOpen(WDC_DEVICE_HANDLE hDev, PVOID *ppBuf, DWORD dwOptions,
    DWORD dwBytes, $$$nu$$$_DMA_HANDLE *ppDmaHandle);
/* Close a DMA handle: Unlock and free a DMA buffer and the containing $$$nu$$$ DMA
 * information structure */
BOOL $$$nu$$$_DmaClose($$$nu$$$_DMA_HANDLE hDma);
/* Prepare a device for a DMA transfer */
/* NOTE: Call this function after $$$nu$$$_DmaOpen() and before $$$nu$$$_DmaStart(). */
BOOL $$$nu$$$_DmaDevicePrepare($$$nu$$$_DMA_HANDLE hDma, BOOL fIsRead, WORD wTLPNumItems,
    WORD dwNumItems, UINT32 u32Pattern, BOOL fEnable64bit, BYTE bTrafficClass);
/* Get maximum DMA packet size */
WORD $$$nu$$$_DmaGetMaxPacketSize(WDC_DEVICE_HANDLE hDev, BOOL fIsRead);

/* Synchronize all CPU caches with the DMA buffer */
DWORD $$$nu$$$_DmaSyncCpu($$$nu$$$_DMA_HANDLE hDma);
/* Synchronize the I/O caches with the DMA buffer */
DWORD $$$nu$$$_DmaSyncIo($$$nu$$$_DMA_HANDLE hDma);

/* Start a DMA transfer */
BOOL $$$nu$$$_DmaStart($$$nu$$$_DMA_HANDLE hDma, BOOL fIsRead);
/* Detect the completion of a DMA transfer */
BOOL $$$nu$$$_DmaIsDone(WDC_DEVICE_HANDLE hDev, BOOL fIsRead);
/* Poll for DMA transfer completion */
BOOL $$$nu$$$_DmaPollCompletion($$$nu$$$_DMA_HANDLE hDma, BOOL fIsRead);
/* Verify success of a host-to-device (read) DMA transfer */
BOOL $$$nu$$$_DmaIsReadSucceed(WDC_DEVICE_HANDLE hDev);

/* Enable DMA interrupts */
BOOL $$$nu$$$_DmaIntEnable(WDC_DEVICE_HANDLE hDev, BOOL fIsRead);
/* Disable DMA interrupts */
BOOL $$$nu$$$_DmaIntDisable(WDC_DEVICE_HANDLE hDev, BOOL fIsRead);

/* -----------------------------------------------
    Read/write registers
   ----------------------------------------------- */
/**
 * Function name:  $$$nu$$$_ReadReg8 / $$$nu$$$_ReadReg16 / $$$nu$$$_ReadReg32
 * Description:    Read from a 8/16/32-bit register
 * Parameters:
 *     @hDev:     (IN) Handle to the card, received from $$$nu$$$_DeviceOpen()
 *     @dwOffset: (IN) Offset of the register within the $$$nu$$$_SPACE address space
 * Return values:  Read data
 * Scope: Global
 **/
BYTE $$$nu$$$_ReadReg8(WDC_DEVICE_HANDLE hDev, DWORD dwOffset);
WORD $$$nu$$$_ReadReg16(WDC_DEVICE_HANDLE hDev, DWORD dwOffset);
UINT32 $$$nu$$$_ReadReg32(WDC_DEVICE_HANDLE hDev, DWORD dwOffset);

/**
 * Function name:  $$$nu$$$_WriteReg8 / $$$nu$$$_WriteReg16 / $$$nu$$$_WriteReg32
 * Description:    Write to a 8/16/32-bit register
 * Parameters:
 *     @hDev:     (IN) Handle to the card, received from $$$nu$$$_DeviceOpen()
 *     @dwOffset: (IN) Offset of the register within the $$$nu$$$_SPACE address space
 *     @bData/@wData/@u32Data:
                  (IN) Data to write to the register (8/16/32 bits)
 * Return values:  None
 * Scope: Global
 **/
void $$$nu$$$_WriteReg8(WDC_DEVICE_HANDLE hDev, DWORD dwOffset, BYTE bData);
void $$$nu$$$_WriteReg16(WDC_DEVICE_HANDLE hDev, DWORD dwOffset, WORD wData);
void $$$nu$$$_WriteReg32(WDC_DEVICE_HANDLE hDev, DWORD dwOffset, UINT32 u32Data);
#endif

/* -----------------------------------------------
    Address spaces information
   ----------------------------------------------- */
/* Get number of address spaces */
DWORD $$$nu$$$_GetNumAddrSpaces(WDC_DEVICE_HANDLE hDev);
/* Get address space information */
BOOL $$$nu$$$_GetAddrSpaceInfo(WDC_DEVICE_HANDLE hDev,
    $$$nu$$$_ADDR_SPACE_INFO *pAddrSpaceInfo);

/* -----------------------------------------------
    Debugging and error handling
   ----------------------------------------------- */
/* Get last error */
const char *$$$nu$$$_GetLastErr(void);

#ifdef __cplusplus
}
#endif

#endif /* _$$$nu$$$_LIB_H_ */

