/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

/****************************************************************************
*  File: $$$nl$$$_lib.c
*
*  Implementation of a sample library for accessing Xilinx PCI Express cards
*  with XDMA design, using the WinDriver WDC API.
*
*  Note: This code sample is provided AS-IS and as a guiding sample only.
*****************************************************************************/

#if defined(__KERNEL__)
    #include "kpstdlib.h"
#else
    #include <stdio.h>
    #include <stdarg.h>
#endif
#include "utils.h"
#include "status_strings.h"
#include "$$$nl$$$_lib.h"

/*************************************************************
  Internal definitions
 *************************************************************/
/* WinDriver license registration string */
/* TODO: When using a registered WinDriver version, replace the license string
         below with your specific WinDriver license registration string and
         replace the driver name below with your driver's name. */
#define $$$nu$$$_DEFAULT_LICENSE_STRING "$$$lic$$$"
#define $$$nu$$$_DEFAULT_DRIVER_NAME WD_DEFAULT_DRIVER_NAME_BASE

#define DMA_ADDR_LOW(addr)   ((UINT32)((addr) & 0xffffffff))
#define DMA_ADDR_HIGH(addr)  ((UINT32)((addr) >> 32))

typedef struct {
    WDC_DEVICE_HANDLE hDev; /* Device handle */
    WD_DMA *pDma;           /* S/G DMA buffer for data transfer */
    PVOID pBuf;             /* Virtual buffer that represents DMA buffer */
    DWORD dwBytes;          /* DMA buffer size in bytes */
    UINT64 u64FPGAOffset;   /* FPGA offset */
    DWORD dwChannel;        /* DMA channel number */
    BOOL fToDevice;
    BOOL fPolling;
    BOOL fStreaming;
    BOOL fNonIncMode;
    WD_DMA *pDmaDesc;       /* S/G DMA descriptors */
    PVOID pDescBuf;         /* S/G DMA descriptors virtual buffer */
    WD_DMA *pWBDma;         /* Polling WriteBack DMA */
    PVOID pWBBuf;           /* Polling WriteBack DMA virtual buffer */
    PVOID pData;            /* Private data of the calling thread */
} $$$nu$$$_DMA_STRUCT;

typedef struct {
#define $$$nu$$$_DESC_MAGIC   0xAD4B0000
#define $$$nu$$$_MAX_ADJACENT 15
    UINT32 u32Control;
    UINT32 u32Bytes;    /* Transfer length in bytes */
    UINT64 u64SrcAddr;  /* Source address */
    UINT64 u64DstAddr;  /* Destination address */
    UINT64 u64NextDesc; /* Next descriptor address */
} $$$nu$$$_DMA_DESC;

typedef struct {
    UINT32 u32CompletedDescs; /* Completed descriptors count */
    UINT32 Reserved[7];
} $$$nu$$$_DMA_POLL_WB;

/* $$$nu$$$ device information struct */
typedef struct {
    $$$nu$$$_INT_HANDLER funcDiagIntHandler;     /* Interrupt handler routine */
    $$$nu$$$_EVENT_HANDLER funcDiagEventHandler; /* Event handler routine */
    DWORD dwConfigBarNum;                    /* Configuration BAR number. Can be
                                                BAR0 or BAR1, depending on FPGA
                                                configuration */
    DWORD dwEnabledIntType;                  /* Enabled Interrupt type. Possible
                                                values: INTERRUPT_MESSAGE_X,
                                                INTERRUPT_MESSAGE,
                                                INTERRUPT_LEVEL_SENSITIVE */
    WD_TRANSFER *pTrans;                     /* Interrupt transfer commands */

#define ENGINE_IDX(dwChannel, fToDevice) \
    (fToDevice ? dwChannel : dwChannel + $$$nu$$$_CHANNELS_NUM)
    $$$nu$$$_DMA_STRUCT *pEnginesArr[$$$nu$$$_CHANNELS_NUM * 2]; /* Array of active $$$nu$$$
                                                            engines. */
} $$$nu$$$_DEV_CTX, *P$$$nu$$$_DEV_CTX;
/* TODO: You can add fields to store additional device-specific information. */

/* Last error information string */
static CHAR gs$$$nu$$$_LastErr[256];

/*************************************************************
  Static functions prototypes and inline implementation
 *************************************************************/
#if !defined(__KERNEL__)
static BOOL DeviceValidate(const PWDC_DEVICE pDev);
#endif
static void DLLCALLCONV $$$nu$$$_IntHandler(PVOID pData);
static void $$$nu$$$_EventHandler(WD_EVENT *pEvent, PVOID pData);
static void ErrLog(const CHAR *sFormat, ...);
static void TraceLog(const CHAR *sFormat, ...);

#if !defined(__KERNEL__)
/* Allocate buffer with page aligned address */
static void *__valloc(unsigned long size)
{
#if defined(WIN32)
    UPTR p = 0, buf, page_size;

    page_size = GetPageSize();
    buf = (UPTR)malloc(size + page_size);
    if (buf)
    {
        p = __ALIGN_DOWN(buf + page_size, page_size);
        *(UPTR *)(p - sizeof(UPTR)) = buf;
    }
    return (void *)p;
#else
    return valloc(size);
#endif
}

/* Free buffer with page aligned address */
static void __vfree(void *p)
{
#if defined(WIN32)
    UPTR buf = *(UPTR *)((UPTR)p - sizeof(UPTR));
    free((void *)buf);
#else
    free(p);
#endif
}
#endif

/* Validate a WDC device handle */
static inline BOOL IsValidDevice(PWDC_DEVICE pDev, const CHAR *sFunc)
{
    if (!pDev || !WDC_GetDevContext(pDev))
    {
        ErrLog("%s: NULL device %s\n", sFunc, !pDev ? "handle" : "context");
        return FALSE;
    }

    return TRUE;
}

/*************************************************************
  Functions implementation
 *************************************************************/
/* -----------------------------------------------
    $$$nu$$$ and WDC libraries initialize/uninitialize
   ----------------------------------------------- */
/* Initialize the Xilinx $$$nu$$$ and WDC libraries */
DWORD $$$nu$$$_LibInit(const CHAR *sLicense)
{
    DWORD dwStatus;

#if defined(WD_DRIVER_NAME_CHANGE)
    /* Set the driver name */
    if (!WD_DriverName($$$nu$$$_DEFAULT_DRIVER_NAME))
    {
        ErrLog("Failed to set the driver name for WDC library.\n");
        return WD_SYSTEM_INTERNAL_ERROR;
    }
#endif

    /* Set WDC library's debug options
     * (default: level=TRACE; redirect output to the Debug Monitor) */
    dwStatus = WDC_SetDebugOptions(WDC_DBG_DEFAULT, NULL);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed to initialize debug options for WDC library.\n"
            "Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        return dwStatus;
    }

    /* Open a handle to the driver and initialize the WDC library */
    dwStatus = WDC_DriverOpen(WDC_DRV_OPEN_DEFAULT,
        sLicense ? sLicense : $$$nu$$$_DEFAULT_LICENSE_STRING);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed to initialize the WDC library. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
        return dwStatus;
    }

    return WD_STATUS_SUCCESS;
}

/* Uninitialize the Xilinx $$$nu$$$ and WDC libraries */
DWORD $$$nu$$$_LibUninit(void)
{
    DWORD dwStatus;

    /* Uninitialize the WDC library and close the handle to WinDriver */
    dwStatus = WDC_DriverClose();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed to uninit the WDC library. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
    }

    return dwStatus;
}

#if !defined(__KERNEL__)

static DWORD getConfigBar(WDC_DEVICE_HANDLE hDev)
{
    UINT32 i, irqId, configId;
    DWORD dwStatus;

    for (i = 0; i < $$$nu$$$_GetNumAddrSpaces(hDev); i++)
    {
        if (!WDC_AddrSpaceIsActive(hDev, i) ||
            WDC_GET_ADDR_SPACE_SIZE(hDev, i) < (UINT64)$$$nu$$$_MIN_CONFIG_BAR_SIZE)
        {
            continue;
        }

        dwStatus = WDC_ReadAddr32(hDev, i, $$$nu$$$_IRQ_BLOCK_IDENTIFIER_OFFSET,
            &irqId);
        if (dwStatus != WD_STATUS_SUCCESS)
        {
            ErrLog("Failed reading IRQ Block Identifier register\n");
            return (DWORD)-1;
        }

        WDC_ReadAddr32(hDev, i, $$$nu$$$_CONFIG_BLOCK_IDENTIFIER_OFFSET,
            &configId);
        if (dwStatus != WD_STATUS_SUCCESS)
        {
            ErrLog("Failed reading Config Block Identifier register\n");
            return (DWORD)-1;
        }

        if ((irqId & $$$nu$$$_ID_MASK) == $$$nu$$$_IRQ_BLOCK_ID &&
            (configId & $$$nu$$$_ID_MASK) == $$$nu$$$_CONFIG_BLOCK_ID)
        {
            return i;
        }
    }

    return (DWORD)-1;
}

/* -----------------------------------------------
    Device open/close
   ----------------------------------------------- */
/* Open a device handle */
WDC_DEVICE_HANDLE $$$nu$$$_DeviceOpen(const WD_PCI_CARD_INFO *pDeviceInfo,
    char *kp_name)
{
    DWORD dwStatus;
    P$$$nu$$$_DEV_CTX pDevCtx = NULL;
    WDC_DEVICE_HANDLE hDev = NULL;
    @@@kp@@@
    $$$nu$$$_DEV_ADDR_DESC devAddrDesc;
    PWDC_DEVICE pDev;
    @@@kp@@@

    /* Validate arguments */
    if (!pDeviceInfo)
    {
        ErrLog("$$$nu$$$_DeviceOpen: Error - NULL device information struct "
            "pointer\n");
        return NULL;
    }

    /* Allocate memory for the $$$nu$$$ device context */
    pDevCtx = (P$$$nu$$$_DEV_CTX)calloc(1, sizeof($$$nu$$$_DEV_CTX));
    if (!pDevCtx)
    {
        ErrLog("Failed allocating memory for $$$nu$$$ device context\n");
        return NULL;
    }

    BZERO(*pDevCtx);

    /* Open a Kernel PlugIn WDC device handle */
    dwStatus = WDC_PciDeviceOpen(&hDev, pDeviceInfo, pDevCtx);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed opening a WDC device handle. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
        goto Error;
    }

    pDevCtx->dwConfigBarNum = getConfigBar(hDev);
    if (pDevCtx->dwConfigBarNum == -1)
    {
        ErrLog("Could not find config BAR, probably not an $$$nu$$$ device\n");
        goto Error;
    }

    @@@kp@@@
    pDev = hDev;
    devAddrDesc.dwNumAddrSpaces = pDev->dwNumAddrSpaces;
    devAddrDesc.pAddrDesc = pDev->pAddrDesc;

    dwStatus = WDC_KernelPlugInOpen(hDev,
        kp_name && *kp_name ? kp_name : KP_$$$nu$$$_DRIVER_NAME, &devAddrDesc);
    if (WD_STATUS_SUCCESS != dwStatus)
        TraceLog("$$$nu$$$_DeviceOpen: Failed opening KernelPlugin\n");
    @@@kp@@@

    /* Validate device information */
    if (!DeviceValidate((PWDC_DEVICE)hDev))
        goto Error;

    @@@kp@@@
    TraceLog("$$$nu$$$_DeviceOpen: Opened a $$$nu$$$ device (handle 0x%p)\n"
        "Device is %s using a Kernel PlugIn driver (%s)\n", hDev,
        (WDC_IS_KP(hDev))? "" : "not" , KP_$$$nu$$$_DRIVER_NAME);

    @@@kp@@@
    /* Return handle to the new device */
    return hDev;

Error:
    if (hDev)
        $$$nu$$$_DeviceClose(hDev);
    else
        free(pDevCtx);

    return NULL;
}

/* Close a device handle */
BOOL $$$nu$$$_DeviceClose(WDC_DEVICE_HANDLE hDev)
{
    DWORD dwStatus;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    P$$$nu$$$_DEV_CTX pDevCtx;

    TraceLog("$$$nu$$$_DeviceClose: Entered. Device handle [0x%p]\n", hDev);

    /* Validate the WDC device handle */
    if (!hDev)
    {
        ErrLog("$$$nu$$$_DeviceClose: Error - NULL device handle\n");
        return FALSE;
    }

    pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(pDev);

    /* Disable interrupts (if enabled) */
    if (WDC_IntIsEnabled(hDev))
    {
        dwStatus = $$$nu$$$_IntDisable(hDev);
        if (WD_STATUS_SUCCESS != dwStatus)
        {
            ErrLog("Failed disabling interrupts. Error 0x%lx - %s\n", dwStatus,
                Stat2Str(dwStatus));
        }
    }

    /* Close the device */
    dwStatus = WDC_PciDeviceClose(hDev);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed closing a WDC device handle (0x%p). Error 0x%lx - %s\n",
            hDev, dwStatus, Stat2Str(dwStatus));
    }

    /* Free $$$nu$$$ device context memory */
    if (pDevCtx)
        free(pDevCtx);

    return (WD_STATUS_SUCCESS == dwStatus);
}

/* Validate device information */
static BOOL DeviceValidate(const PWDC_DEVICE pDev)
{
    DWORD i, dwNumAddrSpaces = pDev->dwNumAddrSpaces;

    /* TODO: You can modify the implementation of this function in order to
             verify that the device has all expected resources. */

    /* Verify that the device has at least one active address space */
    for (i = 0; i < dwNumAddrSpaces; i++)
    {
        if (WDC_AddrSpaceIsActive(pDev, i))
            return TRUE;
    }

    ErrLog("Device does not have any active memory or I/O address spaces\n");
    return FALSE;
}

/* -----------------------------------------------
    Interrupts
   ----------------------------------------------- */

static void HandleEngineInterrupt($$$nu$$$_DMA_STRUCT *p$$$nl$$$Dma, UINT32 val)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)p$$$nl$$$Dma->hDev;
    P$$$nu$$$_DEV_CTX pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(pDev);
    $$$nu$$$_INT_RESULT intResult;

    BZERO(intResult);
    intResult.u32IntStatus = val;

    if (!p$$$nl$$$Dma->fToDevice)
        WDC_DMASyncIo(p$$$nl$$$Dma->pDma);

    $$$nu$$$_EngineStatusRead(p$$$nl$$$Dma, TRUE, &intResult.u32DmaStatus);
    $$$nu$$$_DmaTransferStop(p$$$nl$$$Dma);

    intResult.hDma = p$$$nl$$$Dma;

    WDC_ReadAddr32(pDev, pDevCtx->dwConfigBarNum,
        $$$nu$$$_CHANNEL_OFFSET(p$$$nl$$$Dma->dwChannel,
        p$$$nl$$$Dma->fToDevice ?  $$$nu$$$_H2C_CHANNEL_COMPLETED_DESC_COUNT_OFFSET :
        $$$nu$$$_C2H_CHANNEL_COMPLETED_DESC_COUNT_OFFSET),
        &val);

    TraceLog("$$$nu$$$_IntHandler: Completed DMA descriptors %d\n", val);

    intResult.dwCounter = pDev->Int.dwCounter;
    intResult.dwLost = pDev->Int.dwLost;
    intResult.waitResult = (WD_INTERRUPT_WAIT_RESULT)pDev->Int.fStopped;

    intResult.fIsMessageBased =
        (WDC_GET_ENABLED_INT_TYPE(pDev) == INTERRUPT_MESSAGE ||
        WDC_GET_ENABLED_INT_TYPE(pDev) == INTERRUPT_MESSAGE_X) ?
        TRUE : FALSE;
    intResult.dwLastMessage = WDC_GET_ENABLED_INT_LAST_MSG(pDev);
    intResult.pData = p$$$nl$$$Dma->pData;

    /* Execute the diagnostics application's interrupt handler routine */
    pDevCtx->funcDiagIntHandler((WDC_DEVICE_HANDLE)pDev, &intResult);
}

/* Interrupt handler routine */
static void DLLCALLCONV $$$nu$$$_IntHandler(PVOID pData)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)pData;
    P$$$nu$$$_DEV_CTX pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(pDev);
    $$$nu$$$_DMA_STRUCT *p$$$nl$$$Dma = NULL;
    UINT32 i, u32IntRequest = pDevCtx->pTrans[0].Data.Dword;

    /* Disable interrupts of completed engines. If level sensitive interrupts
     * are used, interrupts should be disabled by transfer commands or by
     * kernel plugin */
    $$$nu$$$_ChannelInterruptsDisable(pDev, u32IntRequest);

    for (i = 0; i < $$$nu$$$_CHANNELS_NUM * 2; i++)
    {
        if (u32IntRequest & 1 << i)
        {
            p$$$nl$$$Dma = pDevCtx->pEnginesArr[i];
            if (p$$$nl$$$Dma)
                HandleEngineInterrupt(p$$$nl$$$Dma, u32IntRequest);
            else
                ErrLog("Engine [%d] context not found\n", i);
        }
    }
}

/* Enable interrupts */
DWORD $$$nu$$$_IntEnable(WDC_DEVICE_HANDLE hDev, $$$nu$$$_INT_HANDLER funcIntHandler)
{
    DWORD dwStatus;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    P$$$nu$$$_DEV_CTX pDevCtx;
    WDC_ADDR_DESC *pAddrDesc;
    WD_TRANSFER *pTrans = NULL;

    TraceLog("$$$nu$$$_IntEnable: Entered. Device handle [0x%p]\n", hDev);

    /* Validate the WDC device handle */
    if (!IsValidDevice(pDev, "$$$nu$$$_IntEnable"))
        return WD_INVALID_PARAMETER;

    pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(pDev);

    /* Check whether interrupts are already enabled */
    if (WDC_IntIsEnabled(hDev))
    {
        ErrLog("Interrupts are already enabled ...\n");
        return WD_OPERATION_ALREADY_DONE;
    }

#if 0
    /* TODO: Uncomment in case level sensitive interrupts are used */
    #define NUM_TRANS_CMDS 3
    pTrans = (WD_TRANSFER *)calloc(NUM_TRANS_CMDS, sizeof(WD_TRANSFER));
    if (!pTrans)
    {
        ErrLog("Failed allocating memory for interrupt transfer commands\n");
        return WD_INSUFFICIENT_RESOURCES;
    }

    pAddrDesc = WDC_GET_ADDR_DESC(pDev, pDevCtx->dwConfigBarNum);

    /* Read IRQ Block Channel Interrupt Request register */
    pTrans[0].pPort = pAddrDesc->pAddr +
        $$$nu$$$_IRQ_BLOCK_CHANNEL_INT_REQUEST_OFFSET;
    pTrans[0].cmdTrans = WDC_ADDR_IS_MEM(pAddrDesc) ? RM_DWORD : RP_DWORD;

    #define $$$nu$$$_CHANNEL_INT_MASK 0x000000FF
    /* Check if it was a DMA completion interrupt */
    pTrans[1].cmdTrans = CMD_MASK;
    pTrans[1].Data.Dword = $$$nu$$$_CHANNEL_INT_MASK;

    /* Disable all channel interrupts.
     * Note: If kernel plugin is used only the relevant channel's interrupts can
     * be disabled */
    pTrans[2].pPort = pAddrDesc->pAddr +
        $$$nu$$$_IRQ_BLOCK_CHANNEL_INT_ENABLE_MASK_W1C_OFFSET;
    pTrans[2].Data.Dword = 0xFFFFFFFF;
    pTrans[2].cmdTrans = WDC_ADDR_IS_MEM(pAddrDesc) ? WM_DWORD : WP_DWORD;
#else
    /* Define a transfer command to read the interrupt status. This is needed in
     * order to determine in user mode ISR the DMA engines that completed DMA
     * transfer */
    #define NUM_TRANS_CMDS 1
    pTrans = (WD_TRANSFER *)calloc(NUM_TRANS_CMDS, sizeof(WD_TRANSFER));
    if (!pTrans)
    {
        ErrLog("Failed allocating memory for interrupt transfer commands\n");
        return WD_INSUFFICIENT_RESOURCES;
    }

    pAddrDesc = WDC_GET_ADDR_DESC(pDev, pDevCtx->dwConfigBarNum);

    /* Read IRQ Block Channel Interrupt Request register */
    pTrans[0].pPort = pAddrDesc->pAddr +
        $$$nu$$$_IRQ_BLOCK_CHANNEL_INT_REQUEST_OFFSET;
    pTrans[0].cmdTrans = WDC_ADDR_IS_MEM(pAddrDesc) ? RM_DWORD : RP_DWORD;
#endif

    /* Store the diag interrupt handler routine, which will be executed by
       $$$nu$$$_IntHandler() when an interrupt is received */
    pDevCtx->funcDiagIntHandler = funcIntHandler;

    /* Enable interrupts */
    dwStatus = WDC_IntEnable(hDev, pTrans, NUM_TRANS_CMDS, INTERRUPT_CMD_COPY,
        $$$nu$$$_IntHandler, (PVOID)pDev, WDC_IS_KP(hDev));

    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed enabling interrupts. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
        free(pTrans);
        return dwStatus;
    }

    pDevCtx->pTrans = pTrans;
    pDevCtx->dwEnabledIntType = WDC_GET_ENABLED_INT_TYPE(hDev);

    TraceLog("$$$nu$$$_IntEnable: '%s' Interrupts enabled\n",
        WDC_IntType2Str(pDevCtx->dwEnabledIntType));

    if (pDevCtx->dwEnabledIntType == INTERRUPT_LEVEL_SENSITIVE)
    {
        UINT32 u32WriteVal;
        BYTE bIntPin;

        dwStatus = WDC_PciReadCfg8(hDev, PCI_IPR, &bIntPin);
        if (dwStatus != WD_STATUS_SUCCESS)
        {
            ErrLog("Failed reading interrupt pin value. Error 0x%lx - %s\n",
                dwStatus, Stat2Str(dwStatus));
            goto Error;
        }

        u32WriteVal = (bIntPin << 24) | (bIntPin << 16) |
            (bIntPin << 8) | bIntPin;

        /* Write Interrupt Pin value to IRQ Block Channel Vector Number
           registers */
        WDC_WriteAddr32(hDev, pDevCtx->dwConfigBarNum,
            $$$nu$$$_IRQ_BLOCK_CHANNEL_VECTOR_1_OFFSET, u32WriteVal);
        WDC_WriteAddr32(hDev, pDevCtx->dwConfigBarNum,
            $$$nu$$$_IRQ_BLOCK_CHANNEL_VECTOR_2_OFFSET, u32WriteVal);
    }

    return WD_STATUS_SUCCESS;

Error:
    dwStatus = WDC_IntDisable(hDev);
    if (dwStatus != WD_STATUS_SUCCESS)
    {
        ErrLog("Failed disabling interrupts. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
    }

    return dwStatus;
}

/* Disable interrupts */
DWORD $$$nu$$$_IntDisable(WDC_DEVICE_HANDLE hDev)
{
    DWORD dwStatus;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    P$$$nu$$$_DEV_CTX pDevCtx;

    TraceLog("$$$nu$$$_IntDisable: Entered. Device handle [0x%p]\n", hDev);

    /* Validate the WDC device handle */
    if (!IsValidDevice(pDev, "$$$nu$$$_IntDisable"))
        return WD_INVALID_PARAMETER;

    pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(pDev);

    /* Check whether interrupts are already disabled */
    if (!WDC_IntIsEnabled(hDev))
    {
        ErrLog("Interrupts are already disabled ...\n");
        return WD_OPERATION_ALREADY_DONE;
    }

    $$$nu$$$_ChannelInterruptsDisable(hDev, 0xFFFFFFFF);
    $$$nu$$$_UserInterruptsDisable(hDev, 0xFFFFFFFF);

    /* Disable interrupts */
    dwStatus = WDC_IntDisable(hDev);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed disabling interrupts. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
    }

    if (pDevCtx->pTrans)
        free(pDevCtx->pTrans);

    return dwStatus;
}

DWORD $$$nu$$$_UserInterruptsEnable(WDC_DEVICE_HANDLE hDev, UINT32 mask)
{
    P$$$nu$$$_DEV_CTX pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(hDev);

    return WDC_WriteAddr32(hDev, pDevCtx->dwConfigBarNum,
        $$$nu$$$_IRQ_BLOCK_USER_INT_ENABLE_MASK_W1S_OFFSET, mask);
}

DWORD $$$nu$$$_UserInterruptsDisable(WDC_DEVICE_HANDLE hDev, UINT32 mask)
{
    P$$$nu$$$_DEV_CTX pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(hDev);

    return WDC_WriteAddr32(hDev, pDevCtx->dwConfigBarNum,
        $$$nu$$$_IRQ_BLOCK_USER_INT_ENABLE_MASK_W1C_OFFSET, mask);
}

DWORD $$$nu$$$_ChannelInterruptsEnable(WDC_DEVICE_HANDLE hDev, UINT32 mask)
{
    P$$$nu$$$_DEV_CTX pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(hDev);

    return WDC_WriteAddr32(hDev, pDevCtx->dwConfigBarNum,
        $$$nu$$$_IRQ_BLOCK_CHANNEL_INT_ENABLE_MASK_W1S_OFFSET, mask);
}

DWORD $$$nu$$$_ChannelInterruptsDisable(WDC_DEVICE_HANDLE hDev, UINT32 mask)
{
    P$$$nu$$$_DEV_CTX pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(hDev);

    return WDC_WriteAddr32(hDev, pDevCtx->dwConfigBarNum,
        $$$nu$$$_IRQ_BLOCK_CHANNEL_INT_ENABLE_MASK_W1C_OFFSET, mask);
}

/* Check whether interrupts are enabled for the given device */
BOOL $$$nu$$$_IntIsEnabled(WDC_DEVICE_HANDLE hDev)
{
    /* Validate the WDC device handle */
    if (!IsValidDevice((PWDC_DEVICE)hDev, "$$$nu$$$_IntIsEnabled"))
        return FALSE;

    /* Check whether interrupts are already enabled */
    return WDC_IntIsEnabled(hDev);
}

/* -----------------------------------------------
    Direct Memory Access (DMA)
   ----------------------------------------------- */

DWORD $$$nu$$$_EngineStatusRead($$$nu$$$_DMA_HANDLE hDma, BOOL fClear, UINT32 *pStatus)
{
    $$$nu$$$_DMA_STRUCT *p$$$nl$$$Dma = ($$$nu$$$_DMA_STRUCT *)hDma;
    P$$$nu$$$_DEV_CTX pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(p$$$nl$$$Dma->hDev);
    DWORD dwOffset;

    if (fClear)
    {
        dwOffset = $$$nu$$$_CHANNEL_OFFSET(p$$$nl$$$Dma->dwChannel,
            p$$$nl$$$Dma->fToDevice ? $$$nu$$$_H2C_CHANNEL_STATUS_RC_OFFSET :
            $$$nu$$$_C2H_CHANNEL_STATUS_RC_OFFSET);
    }
    else
    {
        dwOffset = $$$nu$$$_CHANNEL_OFFSET(p$$$nl$$$Dma->dwChannel,
            p$$$nl$$$Dma->fToDevice ? $$$nu$$$_H2C_CHANNEL_STATUS_OFFSET :
            $$$nu$$$_C2H_CHANNEL_STATUS_OFFSET);
    }

    return WDC_ReadAddr32(p$$$nl$$$Dma->hDev, pDevCtx->dwConfigBarNum, dwOffset,
        pStatus);
}

static DWORD ValidateTransferParams(WDC_DEVICE_HANDLE hDev, BOOL fToDevice,
    DWORD dwChannel)
{
    P$$$nu$$$_DEV_CTX pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(hDev);
    DWORD dwStatus;
    UINT32 engine_id_reg, expected_engine_id, engine_id, engine_channel_num;

    if (dwChannel > ($$$nu$$$_CHANNELS_NUM - 1))
    {
        ErrLog("Wrong channel (%d), should be between 0 and 3\n", dwChannel);
        return WD_INVALID_PARAMETER;
    }

    dwStatus = WDC_ReadAddr32(hDev, pDevCtx->dwConfigBarNum,
        $$$nu$$$_CHANNEL_OFFSET(dwChannel, fToDevice ?
        $$$nu$$$_H2C_CHANNEL_IDENTIFIER_OFFSET :
        $$$nu$$$_C2H_CHANNEL_IDENTIFIER_OFFSET),
        &engine_id_reg);
    if (dwStatus != WD_STATUS_SUCCESS)
    {
        ErrLog("Failed reading engine id register\n");
        return dwStatus;
    }

    expected_engine_id = fToDevice ? $$$nu$$$_H2C_ID : $$$nu$$$_C2H_ID;
    engine_id = $$$nu$$$_ENGINE_ID(engine_id_reg);
    if (engine_id != expected_engine_id)
    {
        ErrLog("Wrong ID, expected 0x%x, read 0x%x\n", expected_engine_id,
            engine_id);
        return WD_INVALID_PARAMETER;
    }

    engine_channel_num = $$$nu$$$_ENGINE_CHANNEL_NUM(engine_id_reg);
    if (dwChannel != engine_channel_num)
    {
        ErrLog("Wrong channel, expected 0x%x, read 0x%x\n", dwChannel,
            engine_channel_num);
        return WD_INVALID_PARAMETER;
    }

    TraceLog("engine_id 0x%x, engine_channel_num %d\n", engine_id,
        engine_channel_num);

    return WD_STATUS_SUCCESS;
}

static BOOL EngineIsStreaming(WDC_DEVICE_HANDLE hDev, DWORD dwChannel,
    BOOL fToDevice)
{
    P$$$nu$$$_DEV_CTX pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(hDev);
    UINT32 engine_id_reg;

    WDC_ReadAddr32(hDev, pDevCtx->dwConfigBarNum,
        $$$nu$$$_CHANNEL_OFFSET(dwChannel, fToDevice ?
        $$$nu$$$_H2C_CHANNEL_IDENTIFIER_OFFSET :
        $$$nu$$$_C2H_CHANNEL_IDENTIFIER_OFFSET),
        &engine_id_reg);

    return (BOOL)(engine_id_reg & 0x8000);
}

static DWORD LockDmaBuffer(WDC_DEVICE_HANDLE hDev, BOOL fToDevice, PVOID *ppBuf,
    DWORD dwBytes, WD_DMA **ppDma)
{
    DWORD dwStatus, dwOptions;

    *ppBuf = __valloc(dwBytes); /* Make sure that the buffer is aligned */
    if (!*ppBuf)
    {
        ErrLog("Memory allocation failure\n");
        return WD_INSUFFICIENT_RESOURCES;
    }

    dwOptions = DMA_ALLOW_64BIT_ADDRESS |
        (fToDevice ? DMA_TO_DEVICE : DMA_FROM_DEVICE);

    dwStatus = WDC_DMASGBufLock(hDev, *ppBuf, dwOptions, dwBytes, ppDma);
    if (dwStatus != WD_STATUS_SUCCESS)
    {
        ErrLog("Failed locking DMA buffer. Error 0x%lX - %s\n", dwStatus,
            Stat2Str(dwStatus));
        goto Error;
    }

    return WD_STATUS_SUCCESS;

Error:
    free(*ppBuf);
    return dwStatus;
}

static DWORD EngineCtrlRegisterSet(WDC_DEVICE_HANDLE hDev, DWORD dwChannel,
    BOOL fToDevice, UINT32 val)
{
    P$$$nu$$$_DEV_CTX pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(hDev);
    DWORD offset = $$$nu$$$_CHANNEL_OFFSET(dwChannel,
        fToDevice ? $$$nu$$$_H2C_CHANNEL_CONTROL_OFFSET :
        $$$nu$$$_C2H_CHANNEL_CONTROL_OFFSET);

    return WDC_WriteAddr32(hDev, pDevCtx->dwConfigBarNum, offset, val);
}

static DWORD EnableDmaInterrupts(WDC_DEVICE_HANDLE hDev, DWORD dwChannel,
    BOOL fStreaming, BOOL fToDevice)
{
    P$$$nu$$$_DEV_CTX pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(hDev);
    DWORD offset;
    UINT32 val;

    /* Error interrupts */
    val = $$$nu$$$_CTRL_IE_DESC_ALIGN_MISMATCH |
        $$$nu$$$_CTRL_IE_MAGIC_STOPPED |
        $$$nu$$$_CTRL_IE_READ_ERROR |
        $$$nu$$$_CTRL_IE_DESC_ERROR;

    /* Enable completion interrupts */
    val |= $$$nu$$$_CTRL_IE_DESC_STOPPED | $$$nu$$$_CTRL_IE_DESC_COMPLETED;
    if (fStreaming)
        val |= $$$nu$$$_CTRL_IE_IDLE_STOPPED;

    offset = $$$nu$$$_CHANNEL_OFFSET(dwChannel, fToDevice ?
        $$$nu$$$_H2C_CHANNEL_INT_ENABLE_MASK_OFFSET :
        $$$nu$$$_C2H_CHANNEL_INT_ENABLE_MASK_OFFSET);

    WDC_WriteAddr32(hDev, pDevCtx->dwConfigBarNum, offset, val);

    /* Make sure channel interrupts are enabled */
    $$$nu$$$_ChannelInterruptsEnable(hDev, 0xFFFFFFFF);

    return WD_STATUS_SUCCESS;
}

static void DmaDescDump($$$nu$$$_DMA_STRUCT *p$$$nl$$$Dma)
{
    $$$nu$$$_DMA_DESC *desc = ($$$nu$$$_DMA_DESC *)p$$$nl$$$Dma->pDescBuf;
    DWORD i;

    for (i = 0; i < p$$$nl$$$Dma->pDma->dwPages; i++)
    {
        TraceLog("DmaDescDump: desc[%ld].u32Control 0x%x\n", i,
            desc[i].u32Control);
        TraceLog("DmaDescDump: desc[%ld].dwBytes 0x%x\n", i, desc[i].u32Bytes);
        TraceLog("DmaDescDump: desc[%ld].u64SrcAddr 0x%llx\n", i,
            desc[i].u64SrcAddr);
        TraceLog("DmaDescDump: desc[%ld].u64DstAddr 0x%llx\n", i,
            desc[i].u64DstAddr);
        TraceLog("DmaDescDump: desc[%ld].u64NextAddr 0x%llx\n", i,
            desc[i].u64NextDesc);
    }
}

static void DmaTransferBuild($$$nu$$$_DMA_STRUCT *p$$$nl$$$Dma)
{
    P$$$nu$$$_DEV_CTX pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(p$$$nl$$$Dma->hDev);
    $$$nu$$$_DMA_DESC *desc = ($$$nu$$$_DMA_DESC *)p$$$nl$$$Dma->pDescBuf;
    UINT64 offset = p$$$nl$$$Dma->u64FPGAOffset;
    DWORD i;

    for (i = 0; i < p$$$nl$$$Dma->pDma->dwPages; i++)
    {
        if (p$$$nl$$$Dma->fToDevice)
        {
            desc[i].u64SrcAddr = p$$$nl$$$Dma->pDma->Page[i].pPhysicalAddr;
            desc[i].u64DstAddr = offset;
        }
        else
        {
            desc[i].u64SrcAddr = offset;
            desc[i].u64DstAddr = p$$$nl$$$Dma->pDma->Page[i].pPhysicalAddr;
        }

        /* Buffer size should not exceed 0x0FFFFFFF bytes, but this should not
         * happen when using s/g DMA buffer */
        desc[i].u32Bytes = p$$$nl$$$Dma->pDma->Page[i].dwBytes;
        if (!p$$$nl$$$Dma->fNonIncMode)
            offset += desc[i].u32Bytes;

        if (i == p$$$nl$$$Dma->pDma->dwPages - 1) /* Last descriptor */
        {
            desc[i].u32Control |= $$$nu$$$_DESC_STOPPED | $$$nu$$$_DESC_EOP |
                $$$nu$$$_DESC_COMPLETED;
        }
    }


    WDC_WriteAddr32(p$$$nl$$$Dma->hDev, pDevCtx->dwConfigBarNum,
        $$$nu$$$_CHANNEL_OFFSET(p$$$nl$$$Dma->dwChannel,
        p$$$nl$$$Dma->fToDevice ? $$$nu$$$_H2C_SGDMA_DESC_LOW_OFFSET :
        $$$nu$$$_C2H_SGDMA_DESC_LOW_OFFSET),
        DMA_ADDR_LOW(p$$$nl$$$Dma->pDmaDesc->Page[0].pPhysicalAddr));
    WDC_WriteAddr32(p$$$nl$$$Dma->hDev, pDevCtx->dwConfigBarNum,
        $$$nu$$$_CHANNEL_OFFSET(p$$$nl$$$Dma->dwChannel,
        p$$$nl$$$Dma->fToDevice ? $$$nu$$$_H2C_SGDMA_DESC_HIGH_OFFSET :
        $$$nu$$$_C2H_SGDMA_DESC_HIGH_OFFSET),
        DMA_ADDR_HIGH(p$$$nl$$$Dma->pDmaDesc->Page[0].pPhysicalAddr));

    WDC_WriteAddr32(p$$$nl$$$Dma->hDev, pDevCtx->dwConfigBarNum,
        $$$nu$$$_CHANNEL_OFFSET(p$$$nl$$$Dma->dwChannel,
        p$$$nl$$$Dma->fToDevice ? $$$nu$$$_H2C_SGDMA_DESC_ADJACENT_OFFSET :
        $$$nu$$$_C2H_SGDMA_DESC_ADJACENT_OFFSET),
        0); /* TODO: Set adjacent descriptors */

    WDC_DMASyncCpu(p$$$nl$$$Dma->pDmaDesc);
}

static DWORD ConfigureDmaDesc($$$nu$$$_DMA_STRUCT *p$$$nl$$$Dma)
{
    DWORD dwStatus, i, dwPages = p$$$nl$$$Dma->pDma->dwPages;
    DWORD dwSize = dwPages * sizeof($$$nu$$$_DMA_DESC);
    DMA_ADDR desc_phys;
    $$$nu$$$_DMA_DESC *desc_virt;

    dwStatus = WDC_DMAContigBufLock(p$$$nl$$$Dma->hDev,
        &p$$$nl$$$Dma->pDescBuf,
        DMA_ALLOW_64BIT_ADDRESS | DMA_TO_DEVICE,
        dwSize,
        &p$$$nl$$$Dma->pDmaDesc);
    if (dwStatus != WD_STATUS_SUCCESS)
    {
        ErrLog("Failed locking DMA descriptors buffer. Error 0x%lX - %s\n",
            dwStatus, Stat2Str(dwStatus));
        return dwStatus;
    }

    desc_phys = p$$$nl$$$Dma->pDmaDesc->Page[0].pPhysicalAddr;
    desc_virt = ($$$nu$$$_DMA_DESC *)p$$$nl$$$Dma->pDescBuf;
    memset(desc_virt, 0, dwSize);

    TraceLog("ConfigureDmaDesc: dwPages %ld\n", dwPages);

    for (i = 0; i < dwPages; i++)
    {
        desc_phys += sizeof($$$nu$$$_DMA_DESC);

        desc_virt[i].u32Control = $$$nu$$$_DESC_MAGIC; /* Descriptor magic number */

        if (i != dwPages - 1)
            desc_virt[i].u64NextDesc = (UINT64)desc_phys;

        /* TODO: Set adjacent descriptors number */
    }

    DmaTransferBuild(p$$$nl$$$Dma);
    DmaDescDump(p$$$nl$$$Dma);

    return WD_STATUS_SUCCESS;
}

static DWORD CheckAlignment($$$nu$$$_DMA_STRUCT *p$$$nl$$$Dma)
{
    P$$$nu$$$_DEV_CTX pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(p$$$nl$$$Dma->hDev);
    UINT32 u32AlignmentsReg, u32Align, u32Granularity;
    UINT32 u32BufLsb, u32OffsetLsb, u32SizeLsb;
    DWORD dwStatus;

    dwStatus = WDC_ReadAddr32(p$$$nl$$$Dma->hDev,
        pDevCtx->dwConfigBarNum,
        $$$nu$$$_CHANNEL_OFFSET(p$$$nl$$$Dma->dwChannel,
        p$$$nl$$$Dma->fToDevice ? $$$nu$$$_H2C_CHANNEL_ALIGNMENTS_OFFSET :
        $$$nu$$$_C2H_CHANNEL_ALIGNMENTS_OFFSET),
        &u32AlignmentsReg);
    if (dwStatus != WD_STATUS_SUCCESS)
    {
        ErrLog("Failed reading alignments register\n");
        return dwStatus;
    }

    TraceLog("u32AlignmentsReg 0x%x\n", u32AlignmentsReg);

    if (!u32AlignmentsReg)
    {
        TraceLog("Alignments register not set\n");
        return WD_STATUS_SUCCESS;
    }

    u32Align = (u32AlignmentsReg & 0x00FF0000) >> 16;
    u32Granularity = (u32AlignmentsReg & 0x0000FF00) >> 8;
    TraceLog("u32Align %d, u32Granularity %d\n", u32Align, u32Granularity);

    u32BufLsb = (UINT32)((UPTR)p$$$nl$$$Dma->pBuf & (u32Align - 1));
    u32OffsetLsb = (UINT32)(p$$$nl$$$Dma->u64FPGAOffset) & (u32Align - 1);
    u32SizeLsb = (UINT32)p$$$nl$$$Dma->dwBytes & ((UINT32)u32Granularity - 1);

    if (p$$$nl$$$Dma->fStreaming || p$$$nl$$$Dma->fNonIncMode)
    {
        if (u32BufLsb != 0)
        {
            ErrLog("Buffer not aligned (%p)\n", p$$$nl$$$Dma->pBuf);
            return WD_INVALID_PARAMETER;
        }

        if (u32SizeLsb != 0)
        {
            ErrLog("Buffer size %ld not multiple of %d\n", p$$$nl$$$Dma->dwBytes,
                u32Granularity);
            return WD_INVALID_PARAMETER;
        }

        if (!p$$$nl$$$Dma->fStreaming && u32OffsetLsb != 0)
        {
            ErrLog("FPGA offset %lx not aligned\n", p$$$nl$$$Dma->u64FPGAOffset);
            return WD_INVALID_PARAMETER;
        }
    }
    else if (u32BufLsb != u32OffsetLsb)
    {
        ErrLog("Buffer alignment 0x%p and FPGA offset alignment 0x%lX do not "
            "match\n", p$$$nl$$$Dma->pBuf, p$$$nl$$$Dma->u64FPGAOffset);
        return WD_INVALID_PARAMETER;
    }

    return WD_STATUS_SUCCESS;
}

DWORD $$$nu$$$_DmaTransferStart($$$nu$$$_DMA_HANDLE hDma)
{
    $$$nu$$$_DMA_STRUCT *p$$$nl$$$Dma = ($$$nu$$$_DMA_STRUCT *)hDma;
    P$$$nu$$$_DEV_CTX pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(p$$$nl$$$Dma->hDev);
    UINT32 val;
    DWORD dwStatus;

    if (!p$$$nl$$$Dma->fPolling)
    {
        dwStatus = EnableDmaInterrupts(p$$$nl$$$Dma->hDev, p$$$nl$$$Dma->dwChannel,
            p$$$nl$$$Dma->fStreaming, p$$$nl$$$Dma->fToDevice);
        if (dwStatus != WD_STATUS_SUCCESS)
        {
            ErrLog("Failed enabling DMA interrupts. Error 0x%lX - %s\n",
                dwStatus, Stat2Str(dwStatus));
            return dwStatus;
        }

        /* Dummy read to flush pending writes */
        WDC_ReadAddr32(p$$$nl$$$Dma->hDev, pDevCtx->dwConfigBarNum,
            $$$nu$$$_IRQ_BLOCK_CHANNEL_INT_REQUEST_OFFSET, &val);
    }
    else
    {
        $$$nu$$$_DMA_POLL_WB *pWB = ($$$nu$$$_DMA_POLL_WB *)p$$$nl$$$Dma->pWBBuf;
        pWB->u32CompletedDescs = 0;
    }

    if (p$$$nl$$$Dma->fToDevice)
        WDC_DMASyncCpu(p$$$nl$$$Dma->pDma);

    val = $$$nu$$$_CTRL_RUN_STOP |
        $$$nu$$$_CTRL_IE_READ_ERROR |
        $$$nu$$$_CTRL_IE_DESC_ERROR |
        $$$nu$$$_CTRL_IE_DESC_ALIGN_MISMATCH |
        $$$nu$$$_CTRL_IE_MAGIC_STOPPED;

    if (p$$$nl$$$Dma->fPolling)
    {
        val |= $$$nu$$$_CTRL_POLL_MODE_WB;
    }
    else
    {
        val |= $$$nu$$$_CTRL_IE_DESC_STOPPED | $$$nu$$$_CTRL_IE_DESC_COMPLETED;
        if (p$$$nl$$$Dma->fStreaming && !p$$$nl$$$Dma->fToDevice)
            val |= $$$nu$$$_CTRL_IE_IDLE_STOPPED;
    }

    if (p$$$nl$$$Dma->fNonIncMode)
        val |= $$$nu$$$_CTRL_NON_INCR_ADDR;

    dwStatus = EngineCtrlRegisterSet(p$$$nl$$$Dma->hDev, p$$$nl$$$Dma->dwChannel,
        p$$$nl$$$Dma->fToDevice, val);
    if (dwStatus != WD_STATUS_SUCCESS)
    {
        ErrLog("Failed starting DMA transfer\n");
        return dwStatus;
    }

    /* Dummy read to flush all previous writes */
    WDC_ReadAddr32(p$$$nl$$$Dma->hDev, pDevCtx->dwConfigBarNum,
        $$$nu$$$_CHANNEL_OFFSET(p$$$nl$$$Dma->dwChannel,
        p$$$nl$$$Dma->fToDevice ? $$$nu$$$_H2C_CHANNEL_STATUS_OFFSET :
        $$$nu$$$_C2H_CHANNEL_STATUS_OFFSET),
        &val);

    return WD_STATUS_SUCCESS;
}

DWORD $$$nu$$$_DmaTransferStop($$$nu$$$_DMA_HANDLE hDma)
{
    $$$nu$$$_DMA_STRUCT *p$$$nl$$$Dma = ($$$nu$$$_DMA_STRUCT *)hDma;
    UINT32 val = $$$nu$$$_CTRL_IE_DESC_ALIGN_MISMATCH |
        $$$nu$$$_CTRL_IE_MAGIC_STOPPED |
        $$$nu$$$_CTRL_IE_READ_ERROR |
        $$$nu$$$_CTRL_IE_DESC_ERROR;

    if (p$$$nl$$$Dma->fPolling)
    {
         val |= $$$nu$$$_CTRL_POLL_MODE_WB;
    }
    else
    {
         val |= $$$nu$$$_CTRL_IE_DESC_STOPPED | $$$nu$$$_CTRL_IE_DESC_COMPLETED;
         if (p$$$nl$$$Dma->fStreaming && !p$$$nl$$$Dma->fToDevice)
             val |= $$$nu$$$_CTRL_IE_IDLE_STOPPED;
    }

    return EngineCtrlRegisterSet(p$$$nl$$$Dma->hDev, p$$$nl$$$Dma->dwChannel,
        p$$$nl$$$Dma->fToDevice, val);
}

DWORD $$$nu$$$_DmaPollCompletion($$$nu$$$_DMA_HANDLE hDma)
{
    $$$nu$$$_DMA_STRUCT *p$$$nl$$$Dma = ($$$nu$$$_DMA_STRUCT *)hDma;
    $$$nu$$$_DMA_POLL_WB *pWB;
    DWORD dwStatus = WD_STATUS_SUCCESS;

    if (!p$$$nl$$$Dma->pWBDma || !p$$$nl$$$Dma->pWBBuf)
    {
        ErrLog("Polling WB address not defined\n");
        return WD_INVALID_PARAMETER;
    }

    pWB = ($$$nu$$$_DMA_POLL_WB *)p$$$nl$$$Dma->pWBBuf;
    while (pWB->u32CompletedDescs < p$$$nl$$$Dma->pDma->dwPages)
    {
        WDC_DMASyncIo(p$$$nl$$$Dma->pWBDma);

        if (pWB->u32CompletedDescs & $$$nu$$$_WB_ERR_MASK)
        {
            UINT32 val;

            $$$nu$$$_EngineStatusRead(p$$$nl$$$Dma, TRUE, &val);
            ErrLog("$$$nu$$$_DmaPollCompletion: DMA Transfer failed, "
                "DMA status 0x%08x\n", val);
            dwStatus = WD_OPERATION_FAILED;
            break;
        }
    }

    $$$nu$$$_DmaTransferStop(p$$$nl$$$Dma);

    if (!p$$$nl$$$Dma->fToDevice)
        WDC_DMASyncIo(p$$$nl$$$Dma->pDma);

    TraceLog("$$$nu$$$_DmaPollCompletion: completed descs %ld\n",
        pWB->u32CompletedDescs);

    return dwStatus;
}

static DWORD ConfigureWriteBackAddress($$$nu$$$_DMA_STRUCT *p$$$nl$$$Dma)
{
    DWORD dwStatus;
    P$$$nu$$$_DEV_CTX pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(p$$$nl$$$Dma->hDev);

    dwStatus = WDC_DMAContigBufLock(p$$$nl$$$Dma->hDev,
        &p$$$nl$$$Dma->pWBBuf,
        DMA_FROM_DEVICE | DMA_ALLOW_64BIT_ADDRESS,
        sizeof($$$nu$$$_DMA_POLL_WB),
        &p$$$nl$$$Dma->pWBDma);
    if (dwStatus != WD_STATUS_SUCCESS)
    {
        ErrLog("Failed allocating DMA for polling WB\n");
        return dwStatus;
    }

    WDC_WriteAddr32(p$$$nl$$$Dma->hDev,
        pDevCtx->dwConfigBarNum,
        $$$nu$$$_CHANNEL_OFFSET(p$$$nl$$$Dma->dwChannel,
        p$$$nl$$$Dma->fToDevice ? $$$nu$$$_H2C_CHANNEL_POLL_LOW_WRITE_BACK_ADDR_OFFSET :
        $$$nu$$$_C2H_CHANNEL_POLL_LOW_WRITE_BACK_ADDR_OFFSET),
        DMA_ADDR_LOW(p$$$nl$$$Dma->pWBDma->Page[0].pPhysicalAddr));
    WDC_WriteAddr32(p$$$nl$$$Dma->hDev,
        pDevCtx->dwConfigBarNum,
        $$$nu$$$_CHANNEL_OFFSET(p$$$nl$$$Dma->dwChannel,
        p$$$nl$$$Dma->fToDevice ? $$$nu$$$_H2C_CHANNEL_POLL_HIGH_WRITE_BACK_ADDR_OFFSET :
        $$$nu$$$_C2H_CHANNEL_POLL_HIGH_WRITE_BACK_ADDR_OFFSET),
        DMA_ADDR_HIGH(p$$$nl$$$Dma->pWBDma->Page[0].pPhysicalAddr));

    return WD_STATUS_SUCCESS;
}

/* Open a DMA handle: Allocate and initialize a $$$nu$$$ DMA information structure,
 * including allocation of a scatter/gather DMA buffer */
DWORD $$$nu$$$_DmaOpen(WDC_DEVICE_HANDLE hDev, $$$nu$$$_DMA_HANDLE *phDma,
    DWORD dwBytes, UINT64 u64FPGAOffset, BOOL fToDevice, DWORD dwChannel,
    BOOL fPolling, BOOL fNonIncMode, PVOID pData)
{
    P$$$nu$$$_DEV_CTX pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(hDev);
    $$$nu$$$_DMA_STRUCT *p$$$nl$$$Dma = NULL;
    DWORD idx = ENGINE_IDX(dwChannel, fToDevice);
    DWORD dwStatus;

    TraceLog("$$$nu$$$_DmaOpen: Entered. Device handle [0x%p], dwBytes [%d], "
        "fToDevice [%d], dwChannel [%d], fPolling [%d]\n", hDev, dwBytes,
        fToDevice, dwChannel, fPolling);

    if (!phDma)
        return WD_INVALID_PARAMETER;

    dwStatus = ValidateTransferParams(hDev, fToDevice, dwChannel);
    if (dwStatus != WD_STATUS_SUCCESS)
    {
        ErrLog("Failed validating transfer params. Error 0x%lX - %s\n",
            dwStatus, Stat2Str(dwStatus));
        return dwStatus;
    }

    if (pDevCtx->pEnginesArr[idx])
    {
        ErrLog("DMA handle already open for this channel\n");
        *phDma = pDevCtx->pEnginesArr[idx];
        return WD_OPERATION_ALREADY_DONE;
    }

    p$$$nl$$$Dma = ($$$nu$$$_DMA_STRUCT *)calloc(1, sizeof($$$nu$$$_DMA_STRUCT));
    if (!p$$$nl$$$Dma)
    {
        ErrLog("Memory allocation failure\n");
        return WD_INSUFFICIENT_RESOURCES;
    }

    p$$$nl$$$Dma->fStreaming = EngineIsStreaming(hDev, dwChannel, fToDevice);

    dwStatus = LockDmaBuffer(hDev, fToDevice, &p$$$nl$$$Dma->pBuf, dwBytes,
        &p$$$nl$$$Dma->pDma);
    if (dwStatus != WD_STATUS_SUCCESS)
        goto Error;

    p$$$nl$$$Dma->hDev = hDev;
    p$$$nl$$$Dma->dwBytes = dwBytes;
    p$$$nl$$$Dma->dwChannel = dwChannel;
    p$$$nl$$$Dma->u64FPGAOffset = u64FPGAOffset;
    p$$$nl$$$Dma->fPolling = fPolling;
    p$$$nl$$$Dma->fToDevice = fToDevice;
    p$$$nl$$$Dma->fNonIncMode = fNonIncMode;
    p$$$nl$$$Dma->pData = pData;
    *phDma = ($$$nu$$$_DMA_HANDLE)p$$$nl$$$Dma;

    WDC_WriteAddr32(hDev, pDevCtx->dwConfigBarNum,
        $$$nu$$$_CHANNEL_OFFSET(dwChannel, fToDevice ?
        $$$nu$$$_H2C_CHANNEL_CONTROL_W1C_OFFSET :
        $$$nu$$$_C2H_CHANNEL_CONTROL_W1C_OFFSET),
        $$$nu$$$_CTRL_NON_INCR_ADDR);

    if (dwStatus != WD_STATUS_SUCCESS)
    {
        ErrLog("Failed %s non-incrementing address mode\n",
            fNonIncMode ? "setting" : "clearing");
        goto Error;
    }

    dwStatus = CheckAlignment(p$$$nl$$$Dma);
    if (dwStatus != WD_STATUS_SUCCESS)
    {
        ErrLog("Alignment validation failed\n");
        goto Error;
    }

    if (fPolling)
    {
        dwStatus = ConfigureWriteBackAddress(p$$$nl$$$Dma);
        if (dwStatus != WD_STATUS_SUCCESS)
        {
            ErrLog("Failed configuring WriteBack address. Error 0x%lX - %s\n",
                dwStatus, Stat2Str(dwStatus));
            goto Error;
        }
    }

    dwStatus = ConfigureDmaDesc(p$$$nl$$$Dma);
    if (dwStatus != WD_STATUS_SUCCESS)
    {
        ErrLog("Failed configuring DMA descriptors buffer. Error 0x%lX - %s\n",
            dwStatus, Stat2Str(dwStatus));
        goto Error;
    }

    pDevCtx->pEnginesArr[ENGINE_IDX(dwChannel, fToDevice)] = p$$$nl$$$Dma;

    TraceLog("Opened DMA: handle %p, fPolling %d, fToDevice %d, dwChannel %ld, "
        "dwBytes %ld, u64FPGAOffset %ld, fStreaming %d, fNonIncMode %d\n",
        p$$$nl$$$Dma, p$$$nl$$$Dma->fPolling, p$$$nl$$$Dma->fToDevice, p$$$nl$$$Dma->dwChannel,
        p$$$nl$$$Dma->dwBytes, p$$$nl$$$Dma->u64FPGAOffset, p$$$nl$$$Dma->fStreaming,
        p$$$nl$$$Dma->fNonIncMode);

    return WD_STATUS_SUCCESS;

Error:
    if (p$$$nl$$$Dma->pDmaDesc)
        WDC_DMABufUnlock(p$$$nl$$$Dma->pDmaDesc);
    if (p$$$nl$$$Dma->pDma)
        WDC_DMABufUnlock(p$$$nl$$$Dma->pDma);
    if (p$$$nl$$$Dma->pBuf)
        __vfree(p$$$nl$$$Dma->pBuf);
    if (p$$$nl$$$Dma)
        free(p$$$nl$$$Dma);
    return dwStatus;
}

DWORD $$$nu$$$_DmaClose($$$nu$$$_DMA_HANDLE hDma)
{
    $$$nu$$$_DMA_STRUCT *p$$$nl$$$Dma = ($$$nu$$$_DMA_STRUCT *)hDma;
    P$$$nu$$$_DEV_CTX pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(p$$$nl$$$Dma->hDev);
    DWORD idx = ENGINE_IDX(p$$$nl$$$Dma->dwChannel, p$$$nl$$$Dma->fToDevice);
    DWORD dwStatus = WD_STATUS_SUCCESS;

    if (p$$$nl$$$Dma->pWBDma)
    {
        dwStatus = WDC_DMABufUnlock(p$$$nl$$$Dma->pWBDma);
        if (dwStatus != WD_STATUS_SUCCESS)
        {
            ErrLog("Failed unlocking DMA polling WB buffer. Error 0x%lX - %s\n",
                dwStatus, Stat2Str(dwStatus));
        }
    }

    if (p$$$nl$$$Dma->pDmaDesc)
    {
        dwStatus = WDC_DMABufUnlock(p$$$nl$$$Dma->pDmaDesc);
        if (dwStatus != WD_STATUS_SUCCESS)
        {
            ErrLog("Failed unlocking DMA descriptors buffer. "
                "Error 0x%lX - %s\n", dwStatus, Stat2Str(dwStatus));
        }
    }

    if (p$$$nl$$$Dma->pDma)
    {
        dwStatus = WDC_DMABufUnlock(p$$$nl$$$Dma->pDma);
        if (dwStatus != WD_STATUS_SUCCESS)
        {
            ErrLog("Failed unlocking DMA buffer. Error 0x%lX - %s\n", dwStatus,
                Stat2Str(dwStatus));
        }
    }

    if (p$$$nl$$$Dma->pBuf)
        __vfree(p$$$nl$$$Dma->pBuf);

    free(p$$$nl$$$Dma);
    pDevCtx->pEnginesArr[idx] = NULL;

    return dwStatus;
}

/* Returns DMA direction. TRUE - host to device, FALSE - device to host */
BOOL $$$nu$$$_DmaIsToDevice($$$nu$$$_DMA_HANDLE hDma)
{
    return (($$$nu$$$_DMA_STRUCT *)hDma)->fToDevice;
}

/* Returns pointer to the allocated virtual buffer and buffer size in bytes */
PVOID $$$nu$$$_DmaBufferGet($$$nu$$$_DMA_HANDLE hDma, DWORD *pBytes)
{
    $$$nu$$$_DMA_STRUCT *p$$$nl$$$Dma = ($$$nu$$$_DMA_STRUCT *)hDma;

    if (!hDma || !pBytes)
        return NULL;

    *pBytes = p$$$nl$$$Dma->dwBytes;
    return p$$$nl$$$Dma->pBuf;
}

/* -----------------------------------------------
    Plug-and-play and power management events
   ----------------------------------------------- */
/* Plug-and-play or power management event handler routine */
static void $$$nu$$$_EventHandler(WD_EVENT *pEvent, PVOID pData)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)pData;
    P$$$nu$$$_DEV_CTX pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(pDev);

    TraceLog("$$$nu$$$_EventHandler: Entered. pData [0x%p], dwAction [0x%lx]\n",
        pData, pEvent->dwAction);

    /* Execute the diagnostics application's event handler function */
    pDevCtx->funcDiagEventHandler((WDC_DEVICE_HANDLE)pDev, pEvent->dwAction);
}

/* Register a plug-and-play or power management event */
DWORD $$$nu$$$_EventRegister(WDC_DEVICE_HANDLE hDev,
    $$$nu$$$_EVENT_HANDLER funcEventHandler)
{
    DWORD dwStatus;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    P$$$nu$$$_DEV_CTX pDevCtx;
    DWORD dwActions = WD_ACTIONS_ALL;
    /* TODO: Modify the above to set up the plug-and-play/power management
             events for which you wish to receive notifications.
             dwActions can be set to any combination of the WD_EVENT_ACTION
             flags defined in windrvr.h. */

    TraceLog("$$$nu$$$_EventRegister: Entered. Device handle [0x%p]\n", hDev);

    /* Validate the WDC device handle */
    if (!IsValidDevice(pDev, "$$$nu$$$_EventRegister"))
        return WD_INVALID_PARAMETER;

    pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(pDev);

    /* Check whether the event is already registered */
    if (WDC_EventIsRegistered(hDev))
    {
        ErrLog("$$$nu$$$_EventRegister: Events are already registered ...\n");
        return WD_OPERATION_ALREADY_DONE;
    }

    /* Store the diag event handler routine to be executed from
     * $$$nu$$$_EventHandler() upon an event */
    pDevCtx->funcDiagEventHandler = funcEventHandler;

    /* Register the event */
    dwStatus = WDC_EventRegister(hDev, dwActions, $$$nu$$$_EventHandler, hDev,
        WDC_IS_KP(hDev));

    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("$$$nu$$$_EventRegister: Failed to register events. "
            "Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        return dwStatus;
    }

    TraceLog("$$$nu$$$_EventRegister: Events registered\n");

    return WD_STATUS_SUCCESS;
}

/* Unregister a plug-and-play or power management event */
DWORD $$$nu$$$_EventUnregister(WDC_DEVICE_HANDLE hDev)
{
    DWORD dwStatus;

    TraceLog("$$$nu$$$_EventUnregister: Entered. Device handle [0x%p]\n", hDev);

    /* Validate the WDC device handle */
    if (!IsValidDevice((PWDC_DEVICE)hDev, "$$$nu$$$_EventUnregister"))
        return WD_INVALID_PARAMETER;

    /* Check whether the event is currently registered */
    if (!WDC_EventIsRegistered(hDev))
    {
        ErrLog("$$$nu$$$_EventUnregister: Cannot unregister events. No events "
            "currently registered ...\n");
        return WD_OPERATION_ALREADY_DONE;
    }

    /* Unregister the event */
    dwStatus = WDC_EventUnregister(hDev);

    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("$$$nu$$$_EventUnregister: Failed to unregister events. "
            "Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
    }

    return dwStatus;
}

/* Check whether a given plug-and-play or power management event is registered
 */
BOOL $$$nu$$$_EventIsRegistered(WDC_DEVICE_HANDLE hDev)
{
    /* Validate the WDC device handle */
    if (!IsValidDevice((PWDC_DEVICE)hDev, "$$$nu$$$_EventIsRegistered"))
        return FALSE;

    /* Check whether the event is registered */
    return WDC_EventIsRegistered(hDev);
}
#endif

/* -----------------------------------------------
    Address spaces information
   ----------------------------------------------- */
/* Get number of address spaces */
DWORD $$$nu$$$_GetNumAddrSpaces(WDC_DEVICE_HANDLE hDev)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;

    /* Validate the WDC device handle */
    if (!IsValidDevice(pDev, "$$$nu$$$_GetNumAddrSpaces"))
        return 0;

    /* Return the number of address spaces for the device */
    return pDev->dwNumAddrSpaces;
}

/* Get address space information */
BOOL $$$nu$$$_GetAddrSpaceInfo(WDC_DEVICE_HANDLE hDev,
    $$$nu$$$_ADDR_SPACE_INFO *pAddrSpaceInfo)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    WDC_ADDR_DESC *pAddrDesc;
    DWORD dwAddrSpace, dwMaxAddrSpace;
    BOOL fIsMemory;

    /* Validate the WDC device handle */
    if (!IsValidDevice(pDev, "$$$nu$$$_GetAddrSpaceInfo"))
        return FALSE;

    dwAddrSpace = pAddrSpaceInfo->dwAddrSpace;
    dwMaxAddrSpace = pDev->dwNumAddrSpaces - 1;

    if (dwAddrSpace > dwMaxAddrSpace)
    {
        ErrLog("$$$nu$$$_GetAddrSpaceInfo: Error - Address space %ld "
            "is out of range (0 - %ld)\n", dwAddrSpace, dwMaxAddrSpace);
        return FALSE;
    }

    pAddrDesc = &pDev->pAddrDesc[dwAddrSpace];

    fIsMemory = WDC_ADDR_IS_MEM(pAddrDesc);

    snprintf(pAddrSpaceInfo->sName, MAX_NAME - 1, "BAR %ld", dwAddrSpace);
    snprintf(pAddrSpaceInfo->sType, MAX_TYPE - 1, fIsMemory ? "Memory" : "I/O");

    if (WDC_AddrSpaceIsActive(pDev, dwAddrSpace))
    {
        WD_ITEMS *pItem = &pDev->cardReg.Card.Item[pAddrDesc->dwItemIndex];
        PHYS_ADDR pAddr = fIsMemory ? pItem->I.Mem.pPhysicalAddr :
            pItem->I.IO.pAddr;

        snprintf(pAddrSpaceInfo->sDesc, MAX_DESC - 1,
            "0x%0*"PRI64"X - 0x%0*"PRI64"X (0x%"PRI64"x bytes)",
            (int)WDC_SIZE_64 * 2, pAddr, (int)WDC_SIZE_64 * 2,
            pAddr + pAddrDesc->qwBytes - 1, pAddrDesc->qwBytes);
    }
    else
    {
        snprintf(pAddrSpaceInfo->sDesc, MAX_DESC - 1, "Inactive address space");
    }

    /* TODO: You can modify the code above to set a different address space
     * name/description. */

    return TRUE;
}

/* Get configuration BAR number */
DWORD $$$nu$$$_ConfigBarNumGet(WDC_DEVICE_HANDLE hDev)
{
    P$$$nu$$$_DEV_CTX pDevCtx;;

    if (!IsValidDevice(hDev, "$$$nu$$$_ConfigBarNumGet"))
        return (DWORD)-1;

    pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(hDev);

    return pDevCtx->dwConfigBarNum;
}

/* -----------------------------------------------
    Debugging and error handling
   ----------------------------------------------- */
/* Log a debug error message */
static void ErrLog(const CHAR *sFormat, ...)
{
    va_list argp;

    va_start(argp, sFormat);
    vsnprintf(gs$$$nu$$$_LastErr, sizeof(gs$$$nu$$$_LastErr) - 1, sFormat, argp);
#if defined(DEBUG)
    #if defined(__KERNEL__)
        WDC_Err("KP $$$nu$$$ lib: %s", gs$$$nu$$$_LastErr);
     #else
        WDC_Err("$$$nu$$$ lib: %s", gs$$$nu$$$_LastErr);
    #endif
#endif
    va_end(argp);
}

/* Log a debug trace message */
static void TraceLog(const CHAR *sFormat, ...)
{
#if defined(DEBUG)
    CHAR sMsg[256];
    va_list argp;

    va_start(argp, sFormat);
    vsnprintf(sMsg, sizeof(sMsg) - 1, sFormat, argp);
    #if defined(__KERNEL__)
        WDC_Trace("KP $$$nu$$$ lib: %s", sMsg);
    #else
        WDC_Trace("$$$nu$$$ lib: %s", sMsg);
    #endif
    va_end(argp);
#else
    UNUSED_VAR(sFormat);
#endif
}

/* Get last error */
const char *$$$nu$$$_GetLastErr(void)
{
    return gs$$$nu$$$_LastErr;
}

