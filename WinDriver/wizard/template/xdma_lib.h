/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

#ifndef _$$$nu$$$_LIB_H_
#define _$$$nu$$$_LIB_H_

/***************************************************************************
*  File: $$$nl$$$_lib.h
*
*  Header file of a sample library for accessing Xilinx PCI Express cards
*  with XDMA design, using the WinDriver WDC API.
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

typedef void *$$$nu$$$_DMA_HANDLE;

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
    UINT32 u32DmaStatus;    /* Status of the completed DMA transfer */
    UINT32 u32IntStatus;    /* Interrupt status */
    $$$nu$$$_DMA_HANDLE hDma;   /* Completed DMA handle */
    PVOID pData;            /* Custom context */
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

#define $$$nu$$$_MIN_CONFIG_BAR_SIZE 0x8FE4

#define $$$nu$$$_BLOCK_ID_HEAD      0x1FC00000
#define $$$nu$$$_BLOCK_ID_HEAD_MASK 0xFFF00000
#define $$$nu$$$_IRQ_BLOCK_ID       0x1FC20000
#define $$$nu$$$_CONFIG_BLOCK_ID    0x1FC30000
#define $$$nu$$$_H2C_ID             0x1FC00000
#define $$$nu$$$_C2H_ID             0x1FC10000
#define $$$nu$$$_ID_MASK            0xFFFF0000
#define $$$nu$$$_CHANNEL_MASK       0x00000F00

#define $$$nu$$$_CHANNELS_NUM       4       /* Up to 4 channels 0..3 */
#define $$$nu$$$_CHANNEL_SPACING    0x100
#define $$$nu$$$_CHANNEL_OFFSET(channel, reg) \
    ((reg) + ((channel) * $$$nu$$$_CHANNEL_SPACING))
#define $$$nu$$$_ENGINE_ID(reg) ((reg) & $$$nu$$$_ID_MASK) /* ID of the DMA engine */
#define $$$nu$$$_ENGINE_CHANNEL_NUM(reg) (((reg) & $$$nu$$$_CHANNEL_MASK) >> 8)

/* H2C/C2H control register bits */
#define $$$nu$$$_CTRL_RUN_STOP                      (1 << 0)
#define $$$nu$$$_CTRL_IE_DESC_STOPPED               (1 << 1)
#define $$$nu$$$_CTRL_IE_DESC_COMPLETED             (1 << 2)
#define $$$nu$$$_CTRL_IE_DESC_ALIGN_MISMATCH        (1 << 3)
#define $$$nu$$$_CTRL_IE_MAGIC_STOPPED              (1 << 4)
#define $$$nu$$$_CTRL_IE_IDLE_STOPPED               (1 << 6)
#define $$$nu$$$_CTRL_IE_READ_ERROR                 (0x1F << 9)
#define $$$nu$$$_CTRL_IE_DESC_ERROR                 (0x1F << 19)
#define $$$nu$$$_CTRL_NON_INCR_ADDR                 (1 << 25)
#define $$$nu$$$_CTRL_POLL_MODE_WB                  (1 << 26)

/* SGDMA descriptor control field bits */
#define $$$nu$$$_DESC_STOPPED       (1 << 0)
#define $$$nu$$$_DESC_COMPLETED     (1 << 1)
#define $$$nu$$$_DESC_EOP           (1 << 4)

/* DMA status register bits */
#define $$$nu$$$_STAT_BUSY                  (1 << 0)
#define $$$nu$$$_STAT_DESC_STOPPED          (1 << 1)
#define $$$nu$$$_STAT_DESC_COMPLETED        (1 << 2)
#define $$$nu$$$_STAT_ALIGN_MISMATCH        (1 << 3)
#define $$$nu$$$_STAT_MAGIC_STOPPED         (1 << 4)
#define $$$nu$$$_STAT_FETCH_STOPPED         (1 << 5)
#define $$$nu$$$_STAT_IDLE_STOPPED          (1 << 6)
#define $$$nu$$$_STAT_READ_ERROR            (0x1F << 9)
#define $$$nu$$$_STAT_DESC_ERROR            (0x1F << 19)

#define $$$nu$$$_WB_ERR_MASK                (1 << 31)

/* $$$nu$$$ registers offsets */
enum {
    /* H2C Channel Registers. Up to 4 channels with 0x100 bytes spacing */
    $$$nu$$$_H2C_CHANNEL_IDENTIFIER_OFFSET                  = 0x0000,
    $$$nu$$$_H2C_CHANNEL_CONTROL_OFFSET                     = 0x0004,
    $$$nu$$$_H2C_CHANNEL_CONTROL_W1S_OFFSET                 = 0x0008,
    $$$nu$$$_H2C_CHANNEL_CONTROL_W1C_OFFSET                 = 0x000C,
    $$$nu$$$_H2C_CHANNEL_STATUS_OFFSET                      = 0x0040,
    $$$nu$$$_H2C_CHANNEL_STATUS_RC_OFFSET                   = 0x0044,
    $$$nu$$$_H2C_CHANNEL_COMPLETED_DESC_COUNT_OFFSET        = 0x0048,
    $$$nu$$$_H2C_CHANNEL_ALIGNMENTS_OFFSET                  = 0x004C,
    $$$nu$$$_H2C_CHANNEL_POLL_WRITE_BACK_ADDR_OFFSET        = 0x0088, /* 64 bit */
    $$$nu$$$_H2C_CHANNEL_POLL_LOW_WRITE_BACK_ADDR_OFFSET    = 0x0088, /* Low 32 bit */
    $$$nu$$$_H2C_CHANNEL_POLL_HIGH_WRITE_BACK_ADDR_OFFSET   = 0x008C, /* High 32 bit */
    $$$nu$$$_H2C_CHANNEL_INT_ENABLE_MASK_OFFSET             = 0x0090,
    $$$nu$$$_H2C_CHANNEL_INT_ENABLE_MASK_W1S_OFFSET         = 0x0094,
    $$$nu$$$_H2C_CHANNEL_INT_ENABLE_MASK_W1C_OFFSET         = 0x0098,
    $$$nu$$$_H2C_CHANNEL_PERFORMANCE_MONITOR_CONTROL_OFFSET = 0x00C0,
    $$$nu$$$_H2C_CHANNEL_PERFORMANCE_CYCLE_COUNT_OFFSET     = 0x00C4,
    $$$nu$$$_H2C_CHANNEL_PERFORMANCE_DATA_COUNT_OFFSET      = 0x00CC,

    /* C2H Channel Registers. Up to 4 channels with 0x100 bytes spacing */
    $$$nu$$$_C2H_CHANNEL_IDENTIFIER_OFFSET                  = 0x1000,
    $$$nu$$$_C2H_CHANNEL_CONTROL_OFFSET                     = 0x1004,
    $$$nu$$$_C2H_CHANNEL_CONTROL_W1S_OFFSET                 = 0x1008,
    $$$nu$$$_C2H_CHANNEL_CONTROL_W1C_OFFSET                 = 0x100C,
    $$$nu$$$_C2H_CHANNEL_STATUS_OFFSET                      = 0x1040,
    $$$nu$$$_C2H_CHANNEL_STATUS_RC_OFFSET                   = 0x1044,
    $$$nu$$$_C2H_CHANNEL_COMPLETED_DESC_COUNT_OFFSET        = 0x1048,
    $$$nu$$$_C2H_CHANNEL_ALIGNMENTS_OFFSET                  = 0x104C,
    $$$nu$$$_C2H_CHANNEL_POLL_WRITE_BACK_ADDR_OFFSET        = 0x1088, /* 64 bit */
    $$$nu$$$_C2H_CHANNEL_POLL_LOW_WRITE_BACK_ADDR_OFFSET    = 0x1088, /* Low 32 bit */
    $$$nu$$$_C2H_CHANNEL_POLL_HIGH_WRITE_BACK_ADDR_OFFSET   = 0x108C, /* High 32 bit */
    $$$nu$$$_C2H_CHANNEL_INT_ENABLE_MASK_OFFSET             = 0x1090,
    $$$nu$$$_C2H_CHANNEL_INT_ENABLE_MASK_W1S_OFFSET         = 0x1094,
    $$$nu$$$_C2H_CHANNEL_INT_ENABLE_MASK_W1C_OFFSET         = 0x1098,
    $$$nu$$$_C2H_CHANNEL_PERFORMANCE_MONITOR_CONTROL_OFFSET = 0x10C0,
    $$$nu$$$_C2H_CHANNEL_PERFORMANCE_CYCLE_COUNT_OFFSET     = 0x10C4,
    $$$nu$$$_C2H_CHANNEL_PERFORMANCE_DATA_COUNT_OFFSET      = 0x10CC,

    /* IRQ Block Registers */
    $$$nu$$$_IRQ_BLOCK_IDENTIFIER_OFFSET                    = 0x2000,
    $$$nu$$$_IRQ_BLOCK_USER_INT_ENABLE_MASK_OFFSET          = 0x2004,
    $$$nu$$$_IRQ_BLOCK_USER_INT_ENABLE_MASK_W1S_OFFSET      = 0x2008,
    $$$nu$$$_IRQ_BLOCK_USER_INT_ENABLE_MASK_W1C_OFFSET      = 0x200C,
    $$$nu$$$_IRQ_BLOCK_CHANNEL_INT_ENABLE_MASK_OFFSET       = 0x2010,
    $$$nu$$$_IRQ_BLOCK_CHANNEL_INT_ENABLE_MASK_W1S_OFFSET   = 0x2014,
    $$$nu$$$_IRQ_BLOCK_CHANNEL_INT_ENABLE_MASK_W1C_OFFSET   = 0x2018,
    $$$nu$$$_IRQ_USER_INT_REQUEST_OFFSET                    = 0x2040,
    $$$nu$$$_IRQ_BLOCK_CHANNEL_INT_REQUEST_OFFSET           = 0x2044,
    $$$nu$$$_IRQ_BLOCK_USER_INT_PENDING_OFFSET              = 0x2048,
    $$$nu$$$_IRQ_BLOCK_CHANNEL_INT_PENDING_OFFSET           = 0x204C,
    $$$nu$$$_IRQ_BLOCK_USER_VECTOR_1_OFFSET                 = 0x2080,
    $$$nu$$$_IRQ_BLOCK_USER_VECTOR_2_OFFSET                 = 0x2084,
    $$$nu$$$_IRQ_BLOCK_USER_VECTOR_3_OFFSET                 = 0x2088,
    $$$nu$$$_IRQ_BLOCK_USER_VECTOR_4_OFFSET                 = 0x208C,
    $$$nu$$$_IRQ_BLOCK_CHANNEL_VECTOR_1_OFFSET              = 0x20A0,
    $$$nu$$$_IRQ_BLOCK_CHANNEL_VECTOR_2_OFFSET              = 0x20A4,

    /* Config Block Registers */
    $$$nu$$$_CONFIG_BLOCK_IDENTIFIER_OFFSET                 = 0x3000,
    $$$nu$$$_CONFIG_BLOCK_BUSDEV_OFFSET                     = 0x3004,
    $$$nu$$$_CONFIG_BLOCK_PCIE_MAX_PAYLOAD_SIZE_OFFSET      = 0x3008,
    $$$nu$$$_CONFIG_BLOCK_PCIE_MAX_READ_REQUEST_SIZE_OFFSET = 0x300C,
    $$$nu$$$_CONFIG_BLOCK_SYSTEM_ID_OFFSET                  = 0x3010,
    $$$nu$$$_CONFIG_BLOCK_MSI_ENABLE_OFFSET                 = 0x3014,
    $$$nu$$$_CONFIG_BLOCK_PCIE_DATA_WIDTH_OFFSET            = 0x3018,
    $$$nu$$$_CONFIG_PCIE_CONTROL_OFFSET                     = 0x301C,
    $$$nu$$$_CONFIG_AXI_USER_MAX_PAYLOAD_SIZE_OFFSET        = 0x3040,
    $$$nu$$$_CONFIG_AXI_USER_MAX_READ_REQUSEST_SIZE_OFFSET  = 0x3044,
    $$$nu$$$_CONFIG_WRITE_FLUSH_TIMEOUT_OFFSET              = 0x3060,

    /* H2C SGDMA Registers */
    $$$nu$$$_H2C_SGDMA_IDENTIFIER_OFFSET                    = 0x4000,
    $$$nu$$$_H2C_SGDMA_DESC_OFFSET                          = 0x4080, /* 64 bit */
    $$$nu$$$_H2C_SGDMA_DESC_LOW_OFFSET                      = 0x4080, /* Low 32 bit */
    $$$nu$$$_H2C_SGDMA_DESC_HIGH_OFFSET                     = 0x4084, /* High 32 bit */
    $$$nu$$$_H2C_SGDMA_DESC_ADJACENT_OFFSET                 = 0x4088,

    /* C2H SGDMA Registers */
    $$$nu$$$_C2H_SGDMA_IDENTIFIER_OFFSET                    = 0x5000,
    $$$nu$$$_C2H_SGDMA_DESC_OFFSET                          = 0x5080, /* 64 bit */
    $$$nu$$$_C2H_SGDMA_DESC_LOW_OFFSET                      = 0x5080, /* Low 32 bit */
    $$$nu$$$_C2H_SGDMA_DESC_HIGH_OFFSET                     = 0x5084, /* High 32 bit */
    $$$nu$$$_C2H_SGDMA_DESC_ADJACENT_OFFSET                 = 0x5088,
};

/*************************************************************
  Function prototypes
 *************************************************************/
/* -----------------------------------------------
    $$$nu$$$ and WDC libraries initialize/uninitialize
   ----------------------------------------------- */
/* Initialize the Xilinx $$$nu$$$ and WDC libraries */
DWORD $$$nu$$$_LibInit(const CHAR *sLicense);
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
/* Enable user interrupts */
DWORD $$$nu$$$_UserInterruptsEnable(WDC_DEVICE_HANDLE hDev, UINT32 mask);
/* Disable user interrupts */
DWORD $$$nu$$$_UserInterruptsDisable(WDC_DEVICE_HANDLE hDev, UINT32 mask);
/* Enable channel interrupts */
DWORD $$$nu$$$_ChannelInterruptsEnable(WDC_DEVICE_HANDLE hDev, UINT32 mask);
/* Disable channel interrupts */
DWORD $$$nu$$$_ChannelInterruptsDisable(WDC_DEVICE_HANDLE hDev, UINT32 mask);

/* -----------------------------------------------
    Direct Memory Access (DMA)
   ----------------------------------------------- */
/* Open a DMA handle: Allocate and initialize a $$$nu$$$ DMA information structure,
 * including allocation of a scatter/gather DMA buffer */
DWORD $$$nu$$$_DmaOpen(WDC_DEVICE_HANDLE hDev, $$$nu$$$_DMA_HANDLE *phDma,
    DWORD dwBytes, UINT64 u64FPGAOffset, BOOL fToDevice, DWORD dwChannel,
    BOOL fPolling, BOOL fNonIncMode, PVOID pData);
/* Close DMA handle */
DWORD $$$nu$$$_DmaClose($$$nu$$$_DMA_HANDLE hDma);
/* Start DMA transfer */
DWORD $$$nu$$$_DmaTransferStart($$$nu$$$_DMA_HANDLE hDma);
/* Stop DMA transfer */
DWORD $$$nu$$$_DmaTransferStop($$$nu$$$_DMA_HANDLE hDma);
/* Poll for DMA transfer completion */
DWORD $$$nu$$$_DmaPollCompletion($$$nu$$$_DMA_HANDLE hDma);
/* Read $$$nu$$$ engine status */
DWORD $$$nu$$$_EngineStatusRead($$$nu$$$_DMA_HANDLE hDma, BOOL fClear, UINT32 *pStatus);
/* Returns DMA direction. TRUE - host to device, FALSE - device to host */
BOOL $$$nu$$$_DmaIsToDevice($$$nu$$$_DMA_HANDLE hDma);
/* Returns pointer to the allocated virtual buffer and buffer size in bytes */
PVOID $$$nu$$$_DmaBufferGet($$$nu$$$_DMA_HANDLE hDma, DWORD *pBytes);

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

#endif

/* -----------------------------------------------
    Address spaces information
   ----------------------------------------------- */
/* Get number of address spaces */
DWORD $$$nu$$$_GetNumAddrSpaces(WDC_DEVICE_HANDLE hDev);
/* Get address space information */
BOOL $$$nu$$$_GetAddrSpaceInfo(WDC_DEVICE_HANDLE hDev,
    $$$nu$$$_ADDR_SPACE_INFO *pAddrSpaceInfo);
/* Get configuration BAR number */
DWORD $$$nu$$$_ConfigBarNumGet(WDC_DEVICE_HANDLE hDev);

/* -----------------------------------------------
    Debugging and error handling
   ----------------------------------------------- */
/* Get last error */
const char *$$$nu$$$_GetLastErr(void);

#ifdef __cplusplus
}
#endif

#endif /* _$$$nu$$$_LIB_H_ */

