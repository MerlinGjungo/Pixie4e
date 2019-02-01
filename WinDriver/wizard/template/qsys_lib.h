/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

#ifndef _$$$nu$$$_LIB_H_
#define _$$$nu$$$_LIB_H_

/*****************************************************************************
*  File: $$$nl$$$_lib.h
*
*  Header file of a sample library for accessing Altera PCI Express cards
*  with Qsys design, using the WinDriver WDC_API.
*  The sample was tested with Altera's Stratix IV GX development kit.
*  For more information on the Qsys design, refer to Altera's
*  "PCI Express in Qsys Example Designs" wiki page:
*  http://alterawiki.com/wiki/PCI_Express_in_Qsys_Example_Designs
*  You can download the WinDriver Development kit from here:
*  https://www.jungo.com/st/do/download_new.php?product=WinDriver&tag=GrandMenu
*
*  Note: This code sample is provided AS-IS and as a guiding sample only.
******************************************************************************/

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
#define $$$nu$$$_DEFAULT_VENDOR_ID 0x1172 /* Vendor ID */
#define $$$nu$$$_DEFAULT_DEVICE_ID 0x0    /* All Altera devices */
  /* TODO: Change the device ID value to match your specific device. */

/* $$$nl$$$ revision */
#define $$$nu$$$_REVISION               9

/* Number of DMA descriptors */
#define $$$nu$$$_DMA_NUM_DESCRIPTORS    2

/* DMA item size, in bytes */
#define $$$nu$$$_DMA_ITEM_BYTES     sizeof(UINT32)              /* 4 bytes */
/* DMA packet size, in bytes */
#define $$$nu$$$_DMA_PKT_BYTES      32                          /* 32 bytes */
/* DMA packet size, in number of UINT32 items */
#define $$$nu$$$_DMA_PKT_NUM_ITEMS  $$$nu$$$_DMA_PKT_BYTES / $$$nu$$$_DMA_ITEM_BYTES
                                                            /* 8 UINT32 items */

/* $$$nl$$$ DMA transfer completion detection descriptor size: */
/* Descriptor size in number of packets */
#define $$$nu$$$_DMA_DONE_NUM_PKTS  1                           /* 1 packet */
/* Descriptor size in number of UINT32 items */
#define $$$nu$$$_DMA_DONE_NUM_ITEMS $$$nu$$$_DMA_DONE_NUM_PKTS * $$$nu$$$_DMA_PKT_NUM_ITEMS
                                                            /* 8 UINT32 items */
/* Descriptor size in bytes */
#define $$$nu$$$_DMA_DONE_BYTES     $$$nu$$$_DMA_DONE_NUM_PKTS * $$$nu$$$_DMA_PKT_BYTES
                                                            /* 32 bytes */

/* $$$nl$$$ DMA transfer completion detection data size (the portion of the DMA
 * transfer completion detection descriptor used to verify DMA completion): */
/* Completion-done data size in number of UINT32 items */
# define $$$nu$$$_DMA_DONE_DETECT_NUM_ITEMS 1                   /* 1 UINT32 item */
/* Completion-done data size in bytes */
#define $$$nu$$$_DMA_DONE_DETECT_BYTES \
    $$$nu$$$_DMA_DONE_DETECT_NUM_ITEMS * $$$nu$$$_DMA_ITEM_BYTES    /* 4 bytes */

/* $$$nl$$$ BAR access for DMA */
#define $$$nu$$$_DMA_BAR_ACCESS 0x07000000

/* Direct Memory Access (DMA) information struct */
typedef struct {
    WD_DMA *pDma;           /* Pointer to a DMA information structure */
    WDC_DEVICE_HANDLE hDev; /* WDC device handle */
    PVOID pBuf;             /* Pointer to a user-mode DMA data buffer */
    DWORD dwBufSize;        /* Size of the pBuf DMA buffer, in bytes */
    DWORD dwTargetAddr;     /* Target device DMA address */
    /* Array of mSGDMA descriptor sizes, in bytes */
    DWORD dwDescrptSizes[$$$nu$$$_DMA_NUM_DESCRIPTORS];
    /* Array of mSGDMA descriptor host-to-device (read) DMA base addresses */
    DWORD dwReadAddresses[$$$nu$$$_DMA_NUM_DESCRIPTORS];
    /* Array of mSGDMA descriptor device-to-host (write) DMA base addresses */
    DWORD dwWriteAddresses[$$$nu$$$_DMA_NUM_DESCRIPTORS];
} $$$nu$$$_DMA_STRUCT, *$$$nu$$$_PDMA_STRUCT;

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
    DWORD dwCounter; /* Number of interrupts received */
    DWORD dwLost;    /* Number of interrupts not yet handled */
    WD_INTERRUPT_WAIT_RESULT waitResult; /* See WD_INTERRUPT_WAIT_RESULT values
                                            in windrvr.h */
    DWORD dwEnabledIntType; /* Interrupt type that was actually enabled
                               (MSI/MSI-X/Level Sensitive/Edge-Triggered) */
    DWORD dwLastMessage; /* Message data of the last received MSI/MSI-X
                            (Windows Vista and higher); N/A to line-based
                            interrupts) */
} $$$nu$$$_INT_RESULT;
/* TODO: You can add fields to $$$nu$$$_INT_RESULT to store any additional
         information that you wish to pass to your diagnostics interrupt
         handler routine (DiagIntHandler() in $$$nl$$$_diag.c). */

/* $$$nl$$$ diagnostics interrupt handler function type */
typedef void (*$$$nu$$$_INT_HANDLER)(WDC_DEVICE_HANDLE hDev,
    $$$nu$$$_INT_RESULT *pIntResult);

/* $$$nl$$$ diagnostics plug-and-play and power management events handler function
 * type */
typedef void (*$$$nu$$$_EVENT_HANDLER)(WDC_DEVICE_HANDLE hDev,
    DWORD dwAction);

/*************************************************************
  Function prototypes
 *************************************************************/
/* -----------------------------------------------
    $$$nl$$$ and WDC libraries initialize/uninitialize
   ----------------------------------------------- */
/* Initialize the Altera $$$nl$$$ and WDC libraries */
DWORD $$$nu$$$_LibInit(void);
/* Uninitialize the Altera $$$nl$$$ and WDC libraries */
DWORD $$$nu$$$_LibUninit(void);

/* -----------------------------------------------
    Device open/close
   ----------------------------------------------- */
/* Open a device handle */
WDC_DEVICE_HANDLE $$$nu$$$_DeviceOpen(WD_PCI_CARD_INFO *pDeviceInfo,
    char *kp_name);
/* Close a device handle */
BOOL $$$nu$$$_DeviceClose(WDC_DEVICE_HANDLE hDev);

/* -----------------------------------------------
    Interrupts
   ----------------------------------------------- */
/* Enable interrupts */
DWORD $$$nu$$$_IntEnable(WDC_DEVICE_HANDLE hDev,
    $$$nu$$$_INT_HANDLER funcIntHandler);
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
    $$$nl$$$ Revision Verification
   ----------------------------------------------- */
/* Verify $$$nl$$$ revision */
BOOL $$$nu$$$_Is$$$nl$$$Revision(WDC_DEVICE_HANDLE hDev);

/* -----------------------------------------------
    Direct Memory Access (DMA)
   ----------------------------------------------- */
/* Open a DMA handle: Allocate a contiguous DMA buffer and initialize the given
 * $$$nl$$$ DMA information structure */
DWORD $$$nu$$$_DmaOpen(WDC_DEVICE_HANDLE hDev, $$$nu$$$_PDMA_STRUCT pDma,
    DWORD dwOptions, DWORD dwNumTransItems);
/* Close a DMA handle: Unlock and free a DMA buffer */
BOOL $$$nu$$$_DmaClose($$$nu$$$_PDMA_STRUCT pDma);
/* Prepare a device for a DMA transfer */
/* NOTE: Call this function after $$$nu$$$_DmaOpen() and before $$$nu$$$_DmaStart().
 */
BOOL $$$nu$$$_DmaDevicePrepare($$$nu$$$_PDMA_STRUCT pDma, BOOL fIsToDevice);

/* Synchronize all CPU caches with the DMA buffer */
DWORD $$$nu$$$_DmaSyncCpu($$$nu$$$_PDMA_STRUCT pDma);
/* Synchronize the I/O caches with the DMA buffer */
DWORD $$$nu$$$_DmaSyncIo($$$nu$$$_PDMA_STRUCT pDma);

/* Start a DMA transfer */
BOOL $$$nu$$$_DmaStart($$$nu$$$_PDMA_STRUCT pDma, BOOL fIsToDevice);
/* Detect the completion of a DMA transfer */
BOOL $$$nu$$$_DmaIsDone($$$nu$$$_PDMA_STRUCT pDma, UINT32 u32ExpectedData);
/* Poll for DMA transfer completion */
BOOL $$$nu$$$_DmaPollCompletion($$$nu$$$_PDMA_STRUCT pDma, UINT32 u32ExpectedData);
/* Verify DMA transfer completion in the device */
BOOL $$$nu$$$_DmaTransferVerify(WDC_DEVICE_HANDLE hDev);

/* -----------------------------------------------
    Read register
   ----------------------------------------------- */
/**
 * Function name:  $$$nu$$$_ReadReg8 / $$$nu$$$_ReadReg16 / $$$nu$$$_ReadReg32
 * Description:    Read from a 8/16/32-bit register
 * Parameters:
 *     @hDev:        (IN) Handle to the card, received from $$$nu$$$_DeviceOpen()
 *     @dwAddrSpace: (IN) Address space containing the register
 *     @dwOffset:    (IN) Offset of the register within the address space
 * Return values:  Read data
 * Scope: Global
 **/
BYTE $$$nu$$$_ReadReg8(WDC_DEVICE_HANDLE hDev, DWORD dwAddrSpace, DWORD dwOffset);
WORD $$$nu$$$_ReadReg16(WDC_DEVICE_HANDLE hDev, DWORD dwAddrSpace, DWORD dwOffset);
UINT32 $$$nu$$$_ReadReg32(WDC_DEVICE_HANDLE hDev, DWORD dwAddrSpace,
    DWORD dwOffset);

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

