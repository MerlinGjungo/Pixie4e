/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

/*****************************************************************************
*  File: $$$nl$$$_lib.c
*
*  Implementation of a sample library for accessing Altera PCI Express cards
*  with Qsys design, using the WinDriver WDC API.
*  The sample was tested with Altera's Stratix IV GX development kit.
*  For more information on the Qsys design, refer to Altera's
*  "PCI Express in Qsys Example Designs" wiki page:
*  http://alterawiki.com/wiki/PCI_Express_in_Qsys_Example_Designs
*  You can download the WinDriver Development kit from here:
*  https://www.jungo.com/st/do/download_new.php?product=WinDriver&tag=GrandMenu
*
*  Note: This code sample is provided AS-IS and as a guiding sample only.
******************************************************************************/

#if defined(__KERNEL__)
    #include "kpstdlib.h"
#else
    #include <stdio.h>
    #include <stdarg.h>
#endif
#include "wdc_defs.h"
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

/* $$$nl$$$ device information struct */
typedef struct {
    $$$nu$$$_INT_HANDLER    funcDiagIntHandler;    /* Interrupt handler routine */
    $$$nu$$$_EVENT_HANDLER  funcDiagEventHandler;  /* Event handler routine */
    $$$nu$$$_PDMA_STRUCT    pDma; /* Pointer to a $$$nl$$$ DMA information struct */
} $$$nu$$$_DEV_CTX, *P$$$nu$$$_DEV_CTX;
/* TODO: You can add fields to store additional device-specific information. */

/* Last error information string */
static CHAR gs$$$nu$$$_LastErr[256];

/* Library initialization reference count */
static DWORD LibInit_count = 0;

/*************************************************************
  Static functions prototypes and inline implementation
 *************************************************************/
#if !defined(__KERNEL__)
static BOOL DeviceValidate(const PWDC_DEVICE pDev);
static void DLLCALLCONV $$$nu$$$_IntHandler(PVOID pData);
static void $$$nu$$$_EventHandler(WD_EVENT *pEvent, PVOID pData);
#endif
static BOOL IsItemExists(PWDC_DEVICE pDev, ITEM_TYPE item);
static void ErrLog(const CHAR *sFormat, ...);
static void TraceLog(const CHAR *sFormat, ...);

/* Validate a WDC device handle */
static inline BOOL IsValidDevice(PWDC_DEVICE pDev, const CHAR *sFunc)
{
    if (!pDev || !WDC_GetDevContext(pDev))
    {
        snprintf(gs$$$nu$$$_LastErr, sizeof(gs$$$nu$$$_LastErr) - 1, "%s: NULL "
            "device %s\n", sFunc, !pDev ? "handle" : "context");
        ErrLog(gs$$$nu$$$_LastErr);
        return FALSE;
    }

    return TRUE;
}

/*************************************************************
  Functions implementation
 *************************************************************/
/* -----------------------------------------------
    $$$nl$$$ and WDC libraries initialize/uninitialize
   ----------------------------------------------- */
/* Initialize the Altera $$$nl$$$ and WDC libraries */
DWORD $$$nu$$$_LibInit(void)
{
    DWORD dwStatus;

    /* Increase the library's reference count; initialize the library only once
     */
    if (++LibInit_count > 1)
        return WD_STATUS_SUCCESS;

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

/* Uninitialize the Altera $$$nl$$$ and WDC libraries */
DWORD $$$nu$$$_LibUninit(void)
{
    DWORD dwStatus;

    /* Decrease the library's reference count; uninitialize the library only
     * when there are no more open handles to the library */
    if (--LibInit_count > 0)
        return WD_STATUS_SUCCESS;

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
WDC_DEVICE_HANDLE $$$nu$$$_DeviceOpen(WD_PCI_CARD_INFO *pDeviceInfo,
        char* kp_name)
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

    /* Allocate memory for the $$$nl$$$ device context */
    pDevCtx = (P$$$nu$$$_DEV_CTX)malloc(sizeof($$$nu$$$_DEV_CTX));
    if (!pDevCtx)
    {
        ErrLog("Failed allocating memory for $$$nl$$$ device context\n");
        return NULL;
    }

    BZERO(*pDevCtx);

    /* Open a WDC device handle */
    dwStatus = WDC_PciDeviceOpen(&hDev, pDeviceInfo, pDevCtx);
    if (dwStatus == WD_FAILED_KERNEL_MAPPING)
    {
        DWORD i;

        for (i = 0; i < pDeviceInfo->Card.dwItems; i++)
        {
            /* We do not need to map to kernel because there is no access to
             * resource in kernel mode */
            if (pDeviceInfo->Card.Item[i].item == ITEM_MEMORY)
                pDeviceInfo->Card.Item[i].I.Mem.dwOptions =
                    WD_ITEM_MEM_DO_NOT_MAP_KERNEL;
        }
        dwStatus = WDC_PciDeviceOpen(&hDev, pDeviceInfo, pDevCtx);
    }
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
    TraceLog("$$$nu$$$_DeviceOpen: Opened a $$$nl$$$ device (handle 0x%p)\n"
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

    TraceLog("$$$nu$$$_DeviceClose entered. Device handle: 0x%p\n", hDev);

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
            ErrLog("Failed disabling interrupts. Error 0x%lx - %s\n",
                dwStatus, Stat2Str(dwStatus));
        }
    }

    /* Close the device */
    dwStatus = WDC_PciDeviceClose(hDev);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed closing a WDC device handle (0x%p). Error 0x%lx - %s\n",
            hDev, dwStatus, Stat2Str(dwStatus));
    }

    /* Free $$$nl$$$ device context memory */
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

    /* In this sample we accept the device even if it doesn't have any
     * address spaces */
    TraceLog("Device does not have any active memory or I/O address spaces\n");
    return TRUE;
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
    intResult.dwEnabledIntType = WDC_GET_ENABLED_INT_TYPE(pDev);
    intResult.dwLastMessage = WDC_GET_ENABLED_INT_LAST_MSG(pDev);

    /* Execute the diagnostics application's interrupt handler routine */
    pDevCtx->funcDiagIntHandler((WDC_DEVICE_HANDLE)pDev, &intResult);
}

/* Check whether a given device contains an item of the specified type */
static BOOL IsItemExists(PWDC_DEVICE pDev, ITEM_TYPE item)
{
    DWORD i, dwNumItems = pDev->cardReg.Card.dwItems;

    for (i = 0; i < dwNumItems; i++)
    {
        if ((ITEM_TYPE)(pDev->cardReg.Card.Item[i].item) == item)
            return TRUE;
    }

    return FALSE;
}

/* Enable interrupts */
DWORD $$$nu$$$_IntEnable(WDC_DEVICE_HANDLE hDev,
    $$$nu$$$_INT_HANDLER funcIntHandler)
{
    DWORD dwStatus;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    P$$$nu$$$_DEV_CTX pDevCtx;

    TraceLog("$$$nu$$$_IntEnable entered. Device handle: 0x%p\n", hDev);

    /* Validate the WDC device handle */
    if (!IsValidDevice(pDev, "$$$nu$$$_IntEnable"))
        return WD_INVALID_PARAMETER;

    /* Verify that the device has an interrupt item */
    if (!IsItemExists(pDev, ITEM_INTERRUPT))
        return WD_OPERATION_FAILED;

    pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(pDev);

    /* Check whether interrupts are already enabled */
    if (WDC_IntIsEnabled(hDev))
    {
        ErrLog("Interrupts are already enabled ...\n");
        return WD_OPERATION_ALREADY_DONE;
    }

    /* Define the number of interrupt transfer commands to use */
    #define NUM_TRANS_CMDS 0

    /* TODO: In order to correctly handle PCI interrupts, you need to
             ADD CODE HERE to set up transfer commands to read/write the
             relevant register(s) to correctly acknowledge the interrupts, as
             dictated by your hardware's specifications.
             When adding transfer commands, be sure to also modify the
             definition of NUM_TRANS_CMDS (above) accordingly.

     *************************************************************************
     * NOTE: If you attempt to enable interrupts without first modifying the
     * code to correctly acknowledge your device's level-sensitive interrupts,
     * as explained above, the OS will HANG when a level sensitive interrupt
     * occurs!
     *************************************************************************/

    /* Store the diag interrupt handler routine, which will be executed by
       $$$nu$$$_IntHandler() when an interrupt is received */
    pDevCtx->funcDiagIntHandler = funcIntHandler;

    /* Enable interrupts */
    /* NOTE: When adding read transfer commands, set the INTERRUPT_CMD_COPY
             flag in the 4th argument passed to WDC_IntEnable() (dwOptions) */
    dwStatus = WDC_IntEnable(hDev, NULL, 0, 0,
        $$$nu$$$_IntHandler, (PVOID)pDev, WDC_IS_KP(hDev));
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed enabling interrupts. Error 0x%lx - %s\n", dwStatus,
            Stat2Str(dwStatus));

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

    TraceLog("$$$nu$$$_IntDisable entered. Device handle: 0x%p\n", hDev);

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

    /* Check whether interrupts are enabled */
    return WDC_IntIsEnabled(hDev);
}

/* -----------------------------------------------
    Direct Memory Access (DMA)
   ----------------------------------------------- */
/* Validate a $$$nl$$$ DMA information structure handle */
static BOOL IsValidDmaHandle($$$nu$$$_PDMA_STRUCT pDma, CHAR *sFunc)
{
    /* Validate the DMA handle and the WDC device handle */
    BOOL ret = (pDma && IsValidDevice(pDma->hDev, sFunc)) ? TRUE : FALSE;

    if (!pDma)
        ErrLog("%s: NULL DMA Handle\n", sFunc);

    return ret;
}

/* Open a DMA handle: Allocate a contiguous DMA buffer and initialize the given
 * $$$nl$$$ DMA information structure */
DWORD $$$nu$$$_DmaOpen(WDC_DEVICE_HANDLE hDev, $$$nu$$$_PDMA_STRUCT pDma,
    DWORD dwOptions, DWORD dwNumTransItems)
{
    DWORD dwStatus;
    P$$$nu$$$_DEV_CTX pDevCtx;

    /* Validate the WDC device handle */
    if (!IsValidDevice((PWDC_DEVICE)hDev, "$$$nu$$$_DmaOpen"))
        return WD_INVALID_PARAMETER;

    /* Check whether the device already has an open DMA handle */
    pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(hDev);
    if (pDevCtx->pDma)
    {
        ErrLog("$$$nu$$$_DmaOpen: Error - DMA already open\n");
        return WD_OPERATION_ALREADY_DONE;
    }

    /* Store the device handle in the $$$nl$$$ DMA information structure */
    pDma->hDev = hDev;

    /* Set the total DMA buffer size (in bytes), including additional
     * $$$nu$$$_DMA_DONE_BYTES bytes for the DMA transfer completion detection
     * descriptor */
    pDma->dwBufSize =
        dwNumTransItems * $$$nu$$$_DMA_ITEM_BYTES + $$$nu$$$_DMA_DONE_BYTES;

    /* Allocate and lock a contiguous DMA buffer */
    dwStatus = WDC_DMAContigBufLock(hDev, &pDma->pBuf, dwOptions,
        pDma->dwBufSize, &pDma->pDma);

    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("$$$nu$$$_DmaOpen: Failed locking a DMA buffer. "
            "Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        return dwStatus;
    }

    /* Set the device's target DMA address */
    pDma->dwTargetAddr = $$$nu$$$_DMA_BAR_ACCESS;

    /* Update the device context's DMA handle */
    pDevCtx->pDma = pDma;

    return WD_STATUS_SUCCESS;
}

/* Prepare a device for a DMA transfer */
BOOL $$$nu$$$_DmaDevicePrepare($$$nu$$$_PDMA_STRUCT pDma, BOOL fIsToDevice)
{
    DWORD i, dwTransDescrptSize;
    UINT32 u32A2PMask, u32Data;
    WDC_DEVICE_HANDLE hDev = pDma->hDev;

    /* Validate the DMA handle */
    if (!IsValidDmaHandle(pDma, "$$$nu$$$_DmaDevicePrepare"))
        return FALSE;

    /* Try to check the address translation path through */
    /* (0x1000 is the offset for the translation register) */
    WDC_WriteAddr32(hDev, AD_PCI_BAR2, 0x1000, 0xFFFFFFFC);
    /* Read the path through mask */
    u32A2PMask = $$$nu$$$_ReadReg32(hDev, AD_PCI_BAR2, 0x1000);

    /* Program the address translation table */
    /* The PCIe core limits the data size to 1 MByte, so it only needs a 20-bit
    *  address. */

    /* Set the lower 32 bits of the DMA address */
    WDC_WriteAddr32(hDev, AD_PCI_BAR2, 0x1000,
        (DWORD)pDma->pDma->Page[0].pPhysicalAddr & u32A2PMask);
    /* Set the upper 32 bits of the DMA address */
    WDC_WriteAddr32(hDev, AD_PCI_BAR2, 0x1004, 0x0); /* Currently limited at
                                                      * hard IP */

    /* Clear the DMA controller and status */
    u32Data = $$$nu$$$_ReadReg32(hDev, AD_PCI_BAR2, 0x06000000);

    if (fIsToDevice)
    {
        WDC_WriteAddr32(hDev, AD_PCI_BAR2, 0x06000000, 0x200);
        u32Data = $$$nu$$$_ReadReg32(hDev, AD_PCI_BAR2, 0x06000000);
    }

    /* Issue a dispatcher reset */
    WDC_WriteAddr32(hDev, AD_PCI_BAR2, 0x06000004, 0x02);
    /* Clear the status */
    WDC_WriteAddr32(hDev, AD_PCI_BAR2, 0x06000000, 0x0);
    /* Set Interrupt Request (IRQ) enable */
    WDC_WriteAddr32(hDev, AD_PCI_BAR2, 0x06000004, 0x10);

    /* Read the mSGDMA descriptor FIFO dispatcher */
    u32Data = $$$nu$$$_ReadReg32(hDev, AD_PCI_BAR2, 0x06000000);

    /* Clear the status */
    u32Data = $$$nu$$$_ReadReg32(hDev, AD_PCI_BAR2, 0x06000000);
    WDC_WriteAddr32(hDev, AD_PCI_BAR2, 0x06000000, 0x0); /* Clear the status */
    u32Data = $$$nu$$$_ReadReg32(hDev, AD_PCI_BAR2, 0x06000000);

    /* Calculate the size, in bytes, of an mSGDMA transfer descriptor */
    dwTransDescrptSize = (pDma->dwBufSize - $$$nu$$$_DMA_DONE_BYTES) /
        ($$$nu$$$_DMA_NUM_DESCRIPTORS - 1);

    /* Configure the device's mSGDMA descriptors */
    /* Set the base addresses and sizes of the DMA-transfer descriptor(s): */
    for (i = 0; i < $$$nu$$$_DMA_NUM_DESCRIPTORS - 1; i++)
    {
        if (fIsToDevice) /* Host-to-device (read) DMA buffer */
        {
            /* Set the descriptor's base read address */
            pDma->dwReadAddresses[i] =
                (DWORD)pDma->pDma->Page[0].pPhysicalAddr & ~u32A2PMask;
            /* Set the descriptor's base write address to the device's target
             * DMA address -- $$$nu$$$_DMA_BAR_ACCESS (set in
             * $$$nu$$$_DmaOpen()), which is the SOPC address for DDR/OCM */
            pDma->dwWriteAddresses[i] = pDma->dwTargetAddr;
        }
        else /* Device-to-host (write) DMA buffer */
        {
            /* Set the descriptor's base read address to the device's target
             * DMA address -- $$$nu$$$_DMA_BAR_ACCESS (set in
             * $$$nu$$$_DmaOpen()), which is the SOPC address for DDR/OCM */
            pDma->dwReadAddresses[i] = pDma->dwTargetAddr;
            /* Set the descriptor's base write address */
            pDma->dwWriteAddresses[i] =
                (DWORD)pDma->pDma->Page[0].pPhysicalAddr & ~u32A2PMask;
        }
        /* Set the descriptor's size, in bytes */
        pDma->dwDescrptSizes[i] = dwTransDescrptSize;
    }

    /* Set the base addresses and size of the DMA transfer completion detection
     * descriptor: */
    /* Set the descriptor's base read address to the beginning of the DMA
     * transfer data */
    pDma->dwReadAddresses[$$$nu$$$_DMA_NUM_DESCRIPTORS - 1] = pDma->dwTargetAddr;
    /* Set the descriptor's base write address to directly after the end of the
     * last DMA data transfer descriptor */
    pDma->dwWriteAddresses[$$$nu$$$_DMA_NUM_DESCRIPTORS - 1] =
        ((DWORD)pDma->pDma->Page[0].pPhysicalAddr & ~u32A2PMask) +
        (pDma->dwBufSize - $$$nu$$$_DMA_DONE_BYTES);
    /* Set the descriptor's size, in bytes */
    pDma->dwDescrptSizes[$$$nu$$$_DMA_NUM_DESCRIPTORS - 1] = $$$nu$$$_DMA_DONE_BYTES;

    UNUSED_VAR(u32Data); /* Not used on sample code */
    return TRUE;
}

/* Close a DMA handle: Unlock and free a DMA buffer */
BOOL $$$nu$$$_DmaClose($$$nu$$$_PDMA_STRUCT pDma)
{
    DWORD dwStatus = WD_STATUS_SUCCESS;
    P$$$nu$$$_DEV_CTX pDevCtx;

    TraceLog("$$$nu$$$_DmaClose entered.");

    /* Validate the DMA handle */
    if (!IsValidDmaHandle(pDma, "$$$nu$$$_DmaClose"))
        return FALSE;

    TraceLog(" Device handle: 0x%p\n", pDma->hDev);

    /* Clear the DMA */
    WDC_WriteAddr32(pDma->hDev, AD_PCI_BAR2, 0x06000000, 0x200);
    WDC_WriteAddr32(pDma->hDev, AD_PCI_BAR2, 0x06000000, 0x200);

    if (pDma->pDma)
    {
        /* Unlock and free the DMA buffer */
        dwStatus = WDC_DMABufUnlock(pDma->pDma);
        if (WD_STATUS_SUCCESS != dwStatus)
        {
            ErrLog("$$$nu$$$_DmaClose: Failed unlocking DMA buffer. "
                "Error 0x%lX - %s\n", dwStatus, Stat2Str(dwStatus));
        }
    }
    else
    {
        TraceLog("$$$nu$$$_DmaClose: DMA is not currently open ... "
            "(device handle 0x%p, DMA handle 0x%p)\n", pDma->hDev, pDma);
    }

    /* Clear the device context's DMA handle */
    pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(pDma->hDev);
    pDevCtx->pDma = NULL;
    return TRUE;
}

/* Synchronize all CPU caches with the DMA buffer */
DWORD $$$nu$$$_DmaSyncCpu($$$nu$$$_PDMA_STRUCT pDma)
{
    return WDC_DMASyncCpu(pDma->pDma);
}

/* Synchronize the I/O caches with the DMA buffer */
DWORD $$$nu$$$_DmaSyncIo($$$nu$$$_PDMA_STRUCT pDma)
{
    return WDC_DMASyncIo(pDma->pDma);
}

/* Start a DMA transfer */
BOOL $$$nu$$$_DmaStart($$$nu$$$_PDMA_STRUCT pDma, BOOL fIsToDevice)
{
    DWORD i, control_bits = 0;
    WDC_DEVICE_HANDLE hDev = pDma->hDev;

    /* Validate the DMA handle */
    if (!IsValidDmaHandle(pDma, "$$$nu$$$_DmaStart"))
        return FALSE;

    if (!fIsToDevice) /* Device-to-host (write) DMA transfer */
    {
        /* Read the mSGDMA descriptor FIFO dispatcher */
        $$$nu$$$_ReadReg32(hDev, AD_PCI_BAR2, 0x06000000);
    }

    /* Synchronize CPU cache with the DMA buffer */
    $$$nu$$$_DmaSyncCpu(pDma);

    /* Configure the device's mSGDMA descriptors to start a DMA transfer */
    for (i = 0; i < $$$nu$$$_DMA_NUM_DESCRIPTORS; i++)
    {
        /* Wait until another descriptor can be written to the mSGDMA */
        while (($$$nu$$$_ReadReg32(hDev, AD_PCI_BAR2, 0x06000000) & 0x04) != 0)
            ;
        /* Configure the mSGDMA control registers dispatcher */
        control_bits = (i == ($$$nu$$$_DMA_NUM_DESCRIPTORS - 1)) ?
                (1 << 14) : (1 << 24);
            /* Bit 14 holds the Interrupt Request (IRQ) number
             * Bit 24 holds the early done bit */
        WDC_WriteAddr32(hDev, AD_PCI_BAR2, 0x06000020,
            pDma->dwReadAddresses[i]);
        WDC_WriteAddr32(hDev, AD_PCI_BAR2, 0x06000024,
            pDma->dwWriteAddresses[i]);
        WDC_WriteAddr32(hDev, AD_PCI_BAR2, 0x06000028, pDma->dwDescrptSizes[i]);
        WDC_WriteAddr32(hDev, AD_PCI_BAR2, 0x0600002C,
            control_bits | (1 << 31));
    }

    return TRUE;
}

/* Detect the completion of a DMA transfer */
BOOL $$$nu$$$_DmaIsDone($$$nu$$$_PDMA_STRUCT pDma, UINT32 u32ExpectedData)
{
    UINT32 u32Data;

    /* Validate the WDC device handle */
    if (!IsValidDevice(pDma->hDev, "$$$nu$$$_DmaIsDone"))
        return FALSE;

    /* Detect DMA transfer completion by comparing the data in the last 4 bytes
     * ($$$nu$$$_DMA_DONE_DETECT_BYTES) of the host's DMA transfer completion
     * detection descriptor, to the expected data */
    u32Data = ((UINT32 *)(pDma->pBuf))[pDma->dwBufSize / $$$nu$$$_DMA_ITEM_BYTES -
        $$$nu$$$_DMA_DONE_DETECT_NUM_ITEMS];
    return (u32ExpectedData == u32Data) ? TRUE : FALSE;
}

/* Poll for DMA transfer completion */
BOOL $$$nu$$$_DmaPollCompletion($$$nu$$$_PDMA_STRUCT pDma, UINT32 u32ExpectedData)
{
    DWORD i, dwTimeout = 10000000; /* 10 seconds */

    /* Validate the DMA handle */
    if (!IsValidDmaHandle(pDma, "$$$nu$$$_DmaPollCompletion"))
        return FALSE;

    /* Poll for DMA completion */
    for (i = 0; i < dwTimeout; i += 2)
    {
        /* Check for DMA completion */
        if ($$$nu$$$_DmaIsDone(pDma, u32ExpectedData))
        {
            /* Synchronize the I/O caches with the DMA buffer */
            $$$nu$$$_DmaSyncIo(pDma);
            return TRUE;
        }

        /* Wait 2 miscroseconds */
        WDC_Sleep(2, 0);
    }

    return FALSE;
}

/* Verify DMA transfer completion in the device */
BOOL $$$nu$$$_DmaTransferVerify(WDC_DEVICE_HANDLE hDev)
{
    UINT32 u32Data;

    /* Validate the WDC device handle */
    if (!IsValidDevice(hDev, "$$$nu$$$_DmaTransferVerify"))
        return FALSE;

    /* Verify transfer completion by reading the mSGDMA descriptor FIFO
     * dispatcher */
    u32Data = $$$nu$$$_ReadReg32(hDev, AD_PCI_BAR2, 0x06000000);
    return (u32Data == 0x202) ? TRUE : FALSE;
}

/* -----------------------------------------------
    $$$nl$$$ Revision Verification
   ----------------------------------------------- */
/* Verify $$$nl$$$ revision */
BOOL $$$nu$$$_Is$$$nl$$$Revision(WDC_DEVICE_HANDLE hDev)
{
    UINT32 u32RevisionID, u32RevisionBit = 0;

    /* Read the revision bit */
    WDC_PciReadCfg32(hDev, 0x8, &u32RevisionBit);
    /* Extract the revision ID */
    u32RevisionID = u32RevisionBit & 0x0F;
    /* Verify the revision ID */
    return (u32RevisionID == $$$nu$$$_REVISION) ? TRUE : FALSE;
}

/* -----------------------------------------------
    Plug-and-play and power management events
   ----------------------------------------------- */
/* Plug-and-play or power management event handler routine */
static void $$$nu$$$_EventHandler(WD_EVENT *pEvent, PVOID pData)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)pData;
    P$$$nu$$$_DEV_CTX pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(pDev);

    TraceLog("$$$nu$$$_EventHandler entered, pData: 0x%p, dwAction 0x%lx\n",
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

    TraceLog("$$$nu$$$_EventRegister entered. Device handle: 0x%p\n", hDev);

    /* Validate the WDC device handle */
    if (!IsValidDevice(pDev, "$$$nu$$$_EventRegister"))
        return WD_INVALID_PARAMETER;

    pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(pDev);

    /* Check whether the event is already registered */
    if (WDC_EventIsRegistered(hDev))
    {
        ErrLog("Events are already registered ...\n");
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
        ErrLog("Failed to register events. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
        return dwStatus;
    }

    TraceLog("Events registered\n");

    return WD_STATUS_SUCCESS;
}

/* Unregister a plug-and-play or power management event */
DWORD $$$nu$$$_EventUnregister(WDC_DEVICE_HANDLE hDev)
{
    DWORD dwStatus;

    TraceLog("$$$nu$$$_EventUnregister entered. Device handle: 0x%p\n", hDev);

    /* Validate the WDC device handle */
    if (!IsValidDevice((PWDC_DEVICE)hDev, "$$$nu$$$_EventUnregister"))
        return WD_INVALID_PARAMETER;

    /* Check whether the event is currently registered */
    if (!WDC_EventIsRegistered(hDev))
    {
        ErrLog("Cannot unregister events - no events currently "
            "registered ...\n");
        return WD_OPERATION_ALREADY_DONE;
    }

    /* Unregister the event */
    dwStatus = WDC_EventUnregister(hDev);

    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed to unregister events. Error 0x%lx - %s\n", dwStatus,
            Stat2Str(dwStatus));
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

#if defined(DEBUG)
    if (!pAddrSpaceInfo)
    {
        ErrLog("$$$nu$$$_GetAddrSpaceInfo: Error - NULL address space information"
            " pointer\n");
        return FALSE;
    }
#endif

    dwAddrSpace = pAddrSpaceInfo->dwAddrSpace;
    dwMaxAddrSpace = pDev->dwNumAddrSpaces - 1;

    if (dwAddrSpace > dwMaxAddrSpace)
    {
        ErrLog("$$$nu$$$_GetAddrSpaceInfo: Error - Address space %ld is out of "
            "range (0 - %ld)\n", dwAddrSpace, dwMaxAddrSpace);
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
UINT32 $$$nu$$$_ReadReg32(WDC_DEVICE_HANDLE hDev, DWORD dwAddrSpace, DWORD dwOffset)
{
    UINT32 u32Data;

    WDC_ReadAddr32(hDev, dwAddrSpace, dwOffset, &u32Data);
    return u32Data;
}

/* Read from a 16-bit register */
WORD $$$nu$$$_ReadReg16(WDC_DEVICE_HANDLE hDev, DWORD dwAddrSpace, DWORD dwOffset)
{
    WORD wData;

    WDC_ReadAddr16(hDev, dwAddrSpace, dwOffset, &wData);
    return wData;
}

/* Read from an 8-bit register */
BYTE $$$nu$$$_ReadReg8(WDC_DEVICE_HANDLE hDev, DWORD dwAddrSpace, DWORD dwOffset)
{
    BYTE bData;

    WDC_ReadAddr8(hDev, dwAddrSpace, dwOffset, &bData);
    return bData;
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
    WDC_Err("$$$nl$$$ lib: %s", $$$nu$$$_GetLastErr());
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
    WDC_Trace("$$$nl$$$ lib: %s", sMsg);
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

