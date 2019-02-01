/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

/****************************************************************************
*  File: $$$nl$$$_lib.c
*
*  Implementation of a sample library for accessing Xilinx PCI Express cards
*  with BMD design, using the WinDriver WDC API.
*  The sample was tested with Xilinx's Virtex and Spartan development kits.
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

/* $$$nu$$$ device information struct */
typedef struct {
    $$$nu$$$_INT_HANDLER funcDiagIntHandler;     /* Interrupt handler routine */
    $$$nu$$$_EVENT_HANDLER funcDiagEventHandler; /* Event handler routine */
    $$$nu$$$_DMA_HANDLE hDma;    /* Handle to a $$$nu$$$ DMA information struct */
    PVOID pBuf;             /* Pointer to a user-mode DMA data buffer */
    BOOL fIsRead; /* DMA direction: host-to-device=read; device-to-host=write */
    UINT32 u32Pattern;      /* 32-bit data pattern (used for DMA data) */
    DWORD dwBufNumItems;    /* Size of the pBuf buffer, in units of UINT32 */
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
DWORD $$$nu$$$_LibInit(void)
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
        $$$nu$$$_DEFAULT_LICENSE_STRING);
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
    @@@kp@@@
    pDev = hDev;
    devAddrDesc.dwNumAddrSpaces = pDev->dwNumAddrSpaces;
    devAddrDesc.pAddrDesc = pDev->pAddrDesc;

    /* Open a handle to a Kernel PlugIn driver */
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
/* Interrupt handler routine */
static void DLLCALLCONV $$$nu$$$_IntHandler(PVOID pData)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)pData;
    P$$$nu$$$_DEV_CTX pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(pDev);
    $$$nu$$$_INT_RESULT intResult;

    BZERO(intResult);
    intResult.dwCounter = pDev->Int.dwCounter;
    intResult.dwLost = pDev->Int.dwLost;
    intResult.waitResult = (WD_INTERRUPT_WAIT_RESULT)pDev->Int.fStopped;

    intResult.fIsMessageBased =
        (WDC_GET_ENABLED_INT_TYPE(pDev) == INTERRUPT_MESSAGE ||
        WDC_GET_ENABLED_INT_TYPE(pDev) == INTERRUPT_MESSAGE_X) ?
        TRUE : FALSE;
    intResult.dwLastMessage = WDC_GET_ENABLED_INT_LAST_MSG(pDev);

    intResult.fIsRead = pDevCtx->fIsRead;
    intResult.pBuf = pDevCtx->pBuf;
    intResult.u32Pattern = pDevCtx->u32Pattern;
    intResult.dwBufNumItems = pDevCtx->dwBufNumItems;

    /* Execute the diagnostics application's interrupt handler routine */
    pDevCtx->funcDiagIntHandler((WDC_DEVICE_HANDLE)pDev, &intResult);
}

/* Enable interrupts */
DWORD $$$nu$$$_IntEnable(WDC_DEVICE_HANDLE hDev,
    $$$nu$$$_INT_HANDLER funcIntHandler)
{
    DWORD dwStatus;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    P$$$nu$$$_DEV_CTX pDevCtx;

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

    /* Message-Signaled Interrupts (MSI) / Extended Message-Signaled Interrupts
       (MSI-X) do not need to be acknowledged, so transfer commands are not
       required */
    #define NUM_TRANS_CMDS 0

    /* Store the diag interrupt handler routine, which will be executed by
       $$$nu$$$_IntHandler() when an interrupt is received */
    pDevCtx->funcDiagIntHandler = funcIntHandler;

    /* Enable interrupts */
    dwStatus = WDC_IntEnable(hDev, NULL, NUM_TRANS_CMDS, 0, $$$nu$$$_IntHandler,
        (PVOID)pDev, WDC_IS_KP(hDev));

    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed enabling interrupts. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));

        return dwStatus;
    }

    /* TODO: You can add code here to write to the device in order to
             physically enable the hardware interrupts. */

    TraceLog("$$$nu$$$_IntEnable: Interrupts enabled\n");

    return WD_STATUS_SUCCESS;
}

/* Disable interrupts */
DWORD $$$nu$$$_IntDisable(WDC_DEVICE_HANDLE hDev)
{
    DWORD dwStatus;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;

    TraceLog("$$$nu$$$_IntDisable: Entered. Device handle [0x%p]\n", hDev);

    /* Validate the WDC device handle */
    if (!IsValidDevice(pDev, "$$$nu$$$_IntDisable"))
        return WD_INVALID_PARAMETER;

    /* Check whether interrupts are already disabled */
    if (!WDC_IntIsEnabled(hDev))
    {
        ErrLog("Interrupts are already disabled ...\n");
        return WD_OPERATION_ALREADY_DONE;
    }

    /* TODO: You can add code here to write to the device in order to
             physically disable the hardware interrupts. */

    /* Disable interrupts */
    dwStatus = WDC_IntDisable(hDev);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed disabling interrupts. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
    }

    return dwStatus;
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
/* Validate a $$$nu$$$ DMA information structure handle */
static BOOL IsValidDmaHandle($$$nu$$$_DMA_HANDLE hDma, CHAR *sFunc)
{
    /* Validate the DMA handle and the WDC device handle */
    BOOL ret = (hDma && IsValidDevice(hDma->hDev, sFunc)) ? TRUE : FALSE;

    if (!hDma)
        ErrLog("%s: NULL DMA Handle\n", sFunc);

    return ret;
}

/* Open a DMA handle: Allocate and initialize a $$$nu$$$ DMA information structure,
 * including allocation of a contiguous DMA buffer */
DWORD $$$nu$$$_DmaOpen(WDC_DEVICE_HANDLE hDev, PVOID *ppBuf, DWORD dwOptions,
    DWORD dwBytes, $$$nu$$$_DMA_HANDLE *ppDmaHandle)
{
    DWORD dwStatus;
    P$$$nu$$$_DEV_CTX pDevCtx;
    $$$nu$$$_DMA_HANDLE p$$$nu$$$Dma = NULL;

    /* Validate the WDC device handle */
    if (!IsValidDevice((PWDC_DEVICE)hDev, "$$$nu$$$_DmaOpen"))
        return WD_INVALID_PARAMETER;

    /* Validate DMA buffer pointer */
    if (!ppBuf)
    {
        ErrLog("$$$nu$$$_DmaOpen: Error - NULL DMA buffer pointer\n");
        return WD_INVALID_PARAMETER;
    }

    /* Check whether the device already has an open DMA handle */
    pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(hDev);
    if (pDevCtx->hDma)
    {
        ErrLog("$$$nu$$$_DmaOpen: Error - DMA already open\n");
        return WD_OPERATION_ALREADY_DONE;
    }

    /* Allocate a $$$nu$$$ DMA information structure */
    p$$$nu$$$Dma = ($$$nu$$$_DMA_STRUCT *)calloc(1, sizeof($$$nu$$$_DMA_STRUCT));
    if (!p$$$nu$$$Dma)
    {
        ErrLog("$$$nu$$$_DmaOpen: Failed allocating memory for a $$$nu$$$ DMA "
            "information structure\n");
        return WD_INSUFFICIENT_RESOURCES;
    }
    /* Store the device handle in the $$$nu$$$ DMA information structure */
    p$$$nu$$$Dma->hDev = hDev;

    /* Allocate and lock a contiguous DMA buffer */
    dwStatus = WDC_DMAContigBufLock(hDev, ppBuf, dwOptions, dwBytes,
        &p$$$nu$$$Dma->pDma);

    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("$$$nu$$$_DmaOpen: Failed locking a DMA buffer. "
            "Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        goto Error;
    }

    *ppDmaHandle = ($$$nu$$$_DMA_HANDLE)p$$$nu$$$Dma;

    /* Update the device context's DMA handle */
    pDevCtx->hDma = p$$$nu$$$Dma;
    pDevCtx->pBuf = *ppBuf;

    return WD_STATUS_SUCCESS;

Error:
    if (p$$$nu$$$Dma)
        $$$nu$$$_DmaClose(($$$nu$$$_DMA_HANDLE)p$$$nu$$$Dma);

    return dwStatus;
}

/* Prepare a device for a DMA transfer */
BOOL $$$nu$$$_DmaDevicePrepare($$$nu$$$_DMA_HANDLE hDma, BOOL fIsRead, WORD wTLPNumItems,
    WORD dwNumItems, UINT32 u32Pattern, BOOL fEnable64bit, BYTE bTrafficClass)
{
    UINT32 u32TLPs, u32LowerAddr;
    BYTE bUpperAddr;
    WDC_DEVICE_HANDLE hDev;
    P$$$nu$$$_DEV_CTX pDevCtx;

    /* Validate the DMA handle */
    if (!IsValidDmaHandle(hDma, "$$$nu$$$_DmaDevicePrepare"))
        return FALSE;

    hDev = hDma->hDev;

    /* Assert Initiator Reset */
    $$$nu$$$_WriteReg32(hDev, $$$nu$$$_DSCR_OFFSET, 0x1);

    /* De-assert Initiator Reset */
    $$$nu$$$_WriteReg32(hDev, $$$nu$$$_DSCR_OFFSET, 0x0);

    /* Get the lower 32 bits of the DMA address */
    u32LowerAddr = (UINT32)hDma->pDma->Page[0].pPhysicalAddr;
    /* Get the upper 8 bits of the DMA address */
    bUpperAddr = (BYTE)((hDma->pDma->Page[0].pPhysicalAddr >> 32) & 0xFF);

    /* Set the DMA Transaction Layer Packets (TLPs) */
    u32TLPs = (wTLPNumItems & 0x1FFF) | /* u32TLPs[0:12] - TLP size */
        ((bTrafficClass & 0x7) << 16) | /* u32TLPs[16:18] - TLP traffic class */
        (fEnable64bit ? BIT19 : 0) |    /* u32TLPs[19] - enable 64-bit TLP */
        (bUpperAddr << 24);           /* u32TLPs[24:31] - the 33:39 upper bits
                                         * of the DMA address */

    if (fIsRead)
    {
        /* Set the lower 32 bits of the DMA address */
        $$$nu$$$_WriteReg32(hDev, $$$nu$$$_RDMATLPA_OFFSET, u32LowerAddr);

        /* Set the size, traffic class, 64-bit enable, and upper 8 bits of the
         * DMA address */
        $$$nu$$$_WriteReg32(hDev, $$$nu$$$_RDMATLPS_OFFSET, u32TLPs);

        /* Set TLP count */
        $$$nu$$$_WriteReg16(hDev, $$$nu$$$_RDMATLPC_OFFSET, dwNumItems);

        /* Set DMA read data pattern */
        $$$nu$$$_WriteReg32(hDev, $$$nu$$$_RDMATLPP_OFFSET, u32Pattern);
    }
    else
    {
        /* Set the lower 32 bits of the DMA address */
        $$$nu$$$_WriteReg32(hDev, $$$nu$$$_WDMATLPA_OFFSET, u32LowerAddr);

        /* Set the size, traffic class, 64-bit enable, and upper 8 bits of the
         * DMA address */
        $$$nu$$$_WriteReg32(hDev, $$$nu$$$_WDMATLPS_OFFSET, u32TLPs);

        /* Set TLP count */
        $$$nu$$$_WriteReg16(hDev, $$$nu$$$_WDMATLPC_OFFSET, dwNumItems);

        /* Set DMA read data pattern */
        $$$nu$$$_WriteReg32(hDev, $$$nu$$$_WDMATLPP_OFFSET, u32Pattern);
    }

    /* Initialize device context DMA fields: */
    pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(hDev);
    /* Set the DMA direction (host-to-device=read; device-to-host=write) */
    pDevCtx->fIsRead = fIsRead;
    /* Set the DMA data pattern */
    pDevCtx->u32Pattern = u32Pattern;
    /* Set the total DMA transfer size, in units of UINT32 */
    pDevCtx->dwBufNumItems = (DWORD)dwNumItems * (DWORD)wTLPNumItems;

    return TRUE;
}

/* Close a DMA handle: Unlock and free a DMA buffer and the containing $$$nu$$$ DMA
 * information structure */
BOOL $$$nu$$$_DmaClose($$$nu$$$_DMA_HANDLE hDma)
{
    DWORD dwStatus = WD_STATUS_SUCCESS;
    P$$$nu$$$_DEV_CTX pDevCtx;

    TraceLog("$$$nu$$$_DmaClose: Entered\n");

    /* Validate the DMA handle */
    if (!IsValidDmaHandle(hDma, "$$$nu$$$_DmaClose"))
        return FALSE;

    TraceLog("$$$nu$$$_DmaClose: Device handle [0x%p]\n", hDma->hDev);

    if (hDma->pDma)
    {
        /* Unlock and free the DMA buffer */
        dwStatus = WDC_DMABufUnlock(hDma->pDma);
        if (WD_STATUS_SUCCESS != dwStatus)
        {
            ErrLog("$$$nu$$$_DmaClose: Failed unlocking DMA buffer. "
                "Error 0x%lX - %s\n", dwStatus, Stat2Str(dwStatus));
        }
    }
    else
    {
        TraceLog("$$$nu$$$_DmaClose: DMA is not currently open ... "
            "(device handle 0x%p, DMA handle 0x%p)\n", hDma->hDev, hDma);
    }

    /* Clear the device context's DMA struct and DMA buffer handles */
    pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(hDma->hDev);
    pDevCtx->hDma = NULL;
    pDevCtx->pBuf = NULL;

    /* Free the $$$nu$$$ DMA information structure */
    free(hDma);

    return TRUE;
}

static WORD code2size(BYTE bCode)
{
    if (bCode > 0x05)
        return 0;
    return (WORD)(128 << bCode);
}

/* Get maximum DMA packet size */
WORD $$$nu$$$_DmaGetMaxPacketSize(WDC_DEVICE_HANDLE hDev, BOOL fIsRead)
{
    UINT32 u32DLTRSSTAT;
    WORD wBytes;

    /* Validate the WDC device handle */
    if (!IsValidDevice(hDev, "$$$nu$$$_DmaGetMaxPacketSize"))
        return 0;

    /* Read encoded maximum payload sizes */
    u32DLTRSSTAT = $$$nu$$$_ReadReg32(hDev, $$$nu$$$_DLTRSSTAT_OFFSET);

    /* Convert encoded maximum payload sizes into byte count */
    if (fIsRead)
    {
        /* Bits 18:16 */
        wBytes = code2size((BYTE)((u32DLTRSSTAT >> 16) & 0x7));
    }
    else
    {
        /* Bits 2:0 */
        WORD wMaxCapPayload = code2size((BYTE)(u32DLTRSSTAT & 0x7));
        /* Bits 10:8 */
        WORD wMaxProgPayload = code2size((BYTE)((u32DLTRSSTAT >> 8) & 0x7));

        wBytes = MIN(wMaxCapPayload, wMaxProgPayload);
    }

    return wBytes;
}

/* Synchronize all CPU caches with the DMA buffer */
DWORD $$$nu$$$_DmaSyncCpu($$$nu$$$_DMA_HANDLE hDma)
{
    return WDC_DMASyncCpu(hDma->pDma);
}

/* Synchronize the I/O caches with the DMA buffer */
DWORD $$$nu$$$_DmaSyncIo($$$nu$$$_DMA_HANDLE hDma)
{
    return WDC_DMASyncIo(hDma->pDma);
}

/* Start a DMA transfer */
BOOL $$$nu$$$_DmaStart($$$nu$$$_DMA_HANDLE hDma, BOOL fIsRead)
{
    /* Validate the DMA handle */
    if (!IsValidDmaHandle(hDma, "$$$nu$$$_DmaStart"))
        return FALSE;

    /* Synchronize CPU cache with the DMA buffer */
    $$$nu$$$_DmaSyncCpu(hDma);

    /* Configure the device to start a DMA transfer */
    $$$nu$$$_WriteReg32(hDma->hDev, $$$nu$$$_DDMACR_OFFSET, fIsRead ? 0x10000 : 0x1);

    return TRUE;
}

/* Detect the completion of a DMA transfer */
BOOL $$$nu$$$_DmaIsDone(WDC_DEVICE_HANDLE hDev, BOOL fIsRead)
{
    UINT32 ddmacr;

    /* Validate the WDC device handle */
    if (!IsValidDevice(hDev, "$$$nu$$$_DmaIsDone"))
        return FALSE;

    /* Detect DMA transfer completion */
    ddmacr = $$$nu$$$_ReadReg32(hDev, $$$nu$$$_DDMACR_OFFSET);
    return (fIsRead ? ddmacr & BIT24 : ddmacr & BIT8) ? TRUE : FALSE;
}

/* Poll for DMA transfer completion */
BOOL $$$nu$$$_DmaPollCompletion($$$nu$$$_DMA_HANDLE hDma, BOOL fIsRead)
{
    DWORD i, dwTimeout = 10000000; /* 10 seconds */

    /* Validate the DMA handle */
    if (!IsValidDmaHandle(hDma, "$$$nu$$$_DmaPollCompletion"))
        return FALSE;

    for (i = 0; i < dwTimeout; i += 2)
    {
        /* Check for DMA completion */
        if ($$$nu$$$_DmaIsDone(hDma->hDev, fIsRead))
        {
            $$$nu$$$_DmaSyncIo(hDma);
            return TRUE;
        }

        /* Wait */
        WDC_Sleep(2, 0);
    }

    return FALSE;
}

/* Enable DMA interrupts */
BOOL $$$nu$$$_DmaIntEnable(WDC_DEVICE_HANDLE hDev, BOOL fIsRead)
{
    UINT32 ddmacr = $$$nu$$$_ReadReg32(hDev, $$$nu$$$_DDMACR_OFFSET);

    /* Validate the WDC device handle */
    if (!IsValidDevice(hDev, "$$$nu$$$_DmaIntEnable"))
        return FALSE;

    ddmacr &= fIsRead ? ~BIT23 : ~BIT7;
    $$$nu$$$_WriteReg32(hDev, $$$nu$$$_DDMACR_OFFSET, ddmacr);

    return TRUE;
}

/* Disable DMA interrupts */
BOOL $$$nu$$$_DmaIntDisable(WDC_DEVICE_HANDLE hDev, BOOL fIsRead)
{
    UINT32 ddmacr = $$$nu$$$_ReadReg32(hDev, $$$nu$$$_DDMACR_OFFSET);

    /* Validate the WDC device handle */
    if (!IsValidDevice(hDev, "$$$nu$$$_DmaIntDisable"))
        return FALSE;

    ddmacr |= fIsRead ? BIT23 : BIT7;
    $$$nu$$$_WriteReg32(hDev, $$$nu$$$_DDMACR_OFFSET, ddmacr);

    return TRUE;
}

/* Verify success of a host-to-device (read) DMA transfer */
BOOL $$$nu$$$_DmaIsReadSucceed(WDC_DEVICE_HANDLE hDev)
{
    UINT32 ddmacr;

    /* Validate the WDC device handle */
    if (!IsValidDevice(hDev, "$$$nu$$$_DmaIsReadSucceed"))
        return FALSE;

    /* Check for a successful host-to-device (read)DMA  transfer indication */
    ddmacr = $$$nu$$$_ReadReg32(hDev, $$$nu$$$_DDMACR_OFFSET);
    return ddmacr & BIT31 ? FALSE : TRUE;
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

/* -----------------------------------------------
    Read/write registers
   ----------------------------------------------- */
/* Read from a 32-bit register */
UINT32 $$$nu$$$_ReadReg32(WDC_DEVICE_HANDLE hDev, DWORD dwOffset)
{
    UINT32 u32Data;

    WDC_ReadAddr32(hDev, $$$nu$$$_SPACE, dwOffset, &u32Data);
    return u32Data;
}

/* Read from a 16-bit register */
WORD $$$nu$$$_ReadReg16(WDC_DEVICE_HANDLE hDev, DWORD dwOffset)
{
    WORD wData;

    WDC_ReadAddr16(hDev, $$$nu$$$_SPACE, dwOffset, &wData);
    return wData;
}

/* Read from an 8-bit register */
BYTE $$$nu$$$_ReadReg8(WDC_DEVICE_HANDLE hDev, DWORD dwOffset)
{
    BYTE bData;

    WDC_ReadAddr8(hDev, $$$nu$$$_SPACE, dwOffset, &bData);
    return bData;
}

/* Write to a 32-bit register */
void $$$nu$$$_WriteReg32(WDC_DEVICE_HANDLE hDev, DWORD dwOffset, UINT32 u32Data)
{
    WDC_WriteAddr32(hDev, $$$nu$$$_SPACE, dwOffset, u32Data);
}

/* Write to a 16-bit register */
void $$$nu$$$_WriteReg16(WDC_DEVICE_HANDLE hDev, DWORD dwOffset, WORD wData)
{
    WDC_WriteAddr16(hDev, $$$nu$$$_SPACE, dwOffset, wData);
}

/* Write to an 8-bit register */
void $$$nu$$$_WriteReg8(WDC_DEVICE_HANDLE hDev, DWORD dwOffset, BYTE bData)
{
    WDC_WriteAddr8(hDev, $$$nu$$$_SPACE, dwOffset, bData);
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

