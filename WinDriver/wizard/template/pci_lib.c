/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

/************************************************************************
*  File: $$$nl$$$_lib.c
*
*  Library for accessing $$$nu$$$ devices, possibly using a Kernel PlugIn driver.
*  The code accesses hardware using WinDriver's WDC library.
@@@CODE_GEN@@@
*
*  Note: This code sample is provided AS-IS and as a guiding sample only.
*************************************************************************/

@@@kp@@@
#ifdef __KERNEL__
#include "kpstdlib.h"
#else
@@@kp@@@
#include <stdio.h>
#include <stdarg.h>
@@@kp@@@
#endif
@@@kp@@@
#include "wdc_defs.h"
#include "utils.h"
#include "status_strings.h"
#include "$$$nl$$$_regs.h"
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
#ifdef HAS_INTS
    @@@pIntTransCmds@@@
    $$$nu$$$_INT_HANDLER   funcDiagIntHandler;   /* Interrupt handler routine */
#endif /* ifdef HAS_INTS */
#ifndef ISA
    $$$nu$$$_EVENT_HANDLER funcDiagEventHandler; /* Event handler routine */
#else /* ifdef ISA */
    #ifndef HAS_INTS
    PVOID pData;
    /* TODO: Set pData to point to any device-specific data that you wish to
             store or replace pData with your own device context information */
    #endif /* ifndef HAS_INTS */
#endif /* ifdef ISA */

} $$$nu$$$_DEV_CTX, *P$$$nu$$$_DEV_CTX;
/* TODO: You can add fields to store additional device-specific information. */

/*************************************************************
  Global variables definitions
 *************************************************************/
/* Last error information string */
static CHAR gs$$$nu$$$_LastErr[256];

/* Library initialization reference count */
static DWORD LibInit_count = 0;

/*************************************************************
  Static functions prototypes and inline implementation
 *************************************************************/

#ifndef ISA
#ifndef __KERNEL__
    static BOOL DeviceValidate(const PWDC_DEVICE pDev);
    static void $$$nu$$$_EventHandler(WD_EVENT *pEvent, PVOID pData);
#endif
#else /* ifdef ISA */
#ifndef __KERNEL__
    static void SetDeviceResources(WD_CARD *pDeviceInfo);
#endif
#endif /* ifdef ISA */
#ifdef HAS_INTS
static void DLLCALLCONV $$$nu$$$_IntHandler(PVOID pData);
#endif /* ifdef HAS_INTS */
static void ErrLog(const CHAR *sFormat, ...);
static void TraceLog(const CHAR *sFormat, ...);

/* Validate a device handle */
static inline BOOL IsValidDevice(PWDC_DEVICE pDev, const CHAR *sFunc)
{
    if (!pDev || !(P$$$nu$$$_DEV_CTX)(pDev->pCtx))
    {
        snprintf(gs$$$nu$$$_LastErr, sizeof(gs$$$nu$$$_LastErr) - 1,
            "%s: NULL device %s\n", sFunc, !pDev ? "handle" : "context");
        ErrLog(gs$$$nu$$$_LastErr);
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
/* Initialize the $$$nu$$$ and WDC libraries */
DWORD $$$nu$$$_LibInit(void)
{
    DWORD dwStatus;

    /* Increase the library's reference count; initialize the library only once
     */
    if (++LibInit_count > 1)
        return WD_STATUS_SUCCESS;

#ifdef WD_DRIVER_NAME_CHANGE
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
    dwStatus = WDC_DriverOpen(WDC_DRV_OPEN_DEFAULT, $$$nu$$$_DEFAULT_LICENSE_STRING);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed to initialize the WDC library. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
        return dwStatus;
    }

    return WD_STATUS_SUCCESS;
}

/* Uninitialize the $$$nu$$$ and WDC libraries */
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

@@@kp@@@
#ifndef __KERNEL__
@@@kp@@@
/* -----------------------------------------------
    Device open/close
   ----------------------------------------------- */
/* Open a device handle */
#ifndef ISA
WDC_DEVICE_HANDLE $$$nu$$$_DeviceOpen(const WD_$$$nu$$$_CARD_INFO *pDeviceInfo)
#else /* ifdef ISA */
WDC_DEVICE_HANDLE $$$nu$$$_DeviceOpen(void)
#endif /* ifdef ISA */
{
    DWORD dwStatus;
    P$$$nu$$$_DEV_CTX pDevCtx = NULL;
    WDC_DEVICE_HANDLE hDev = NULL;
@@@kp@@@
    $$$nu$$$_DEV_ADDR_DESC devAddrDesc;
    PWDC_DEVICE pDev;
@@@kp@@@
#ifndef ISA
    /* Validate arguments */
    if (!pDeviceInfo)
    {
        ErrLog("$$$nu$$$_DeviceOpen: Error - NULL device information "
            "struct pointer\n");
        return NULL;
    }
#else /* ifdef ISA */
    WD_CARD deviceInfo;
#endif /* ifdef ISA */
    /* Allocate memory for the $$$nu$$$ device context */
    pDevCtx = (P$$$nu$$$_DEV_CTX)malloc(sizeof($$$nu$$$_DEV_CTX));
    if (!pDevCtx)
    {
        ErrLog("Failed allocating memory for $$$nu$$$ device context\n");
        return NULL;
    }

    BZERO(*pDevCtx);

#ifndef ISA
    /* Open a device handle */
    dwStatus = WDC_$$$nl$$$DeviceOpen(&hDev, pDeviceInfo, pDevCtx);
#else /* ifdef ISA */
    /* Set the device's resources information */
    SetDeviceResources(&deviceInfo);

    /* Open a device handle */
    dwStatus = WDC_IsaDeviceOpen(&hDev, &deviceInfo, pDevCtx);
#endif /* ifdef ISA */
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed opening a WDC device handle. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
        goto Error;
    }
@@@kp@@@
    pDev = (PWDC_DEVICE)hDev;
    devAddrDesc.dwNumAddrSpaces = pDev->dwNumAddrSpaces;
    devAddrDesc.pAddrDesc = pDev->pAddrDesc;

    /* Open a handle to a Kernel PlugIn driver */
    WDC_KernelPlugInOpen(hDev, KP_$$$nu$$$_DRIVER_NAME, &devAddrDesc);
@@@kp@@@
#ifndef ISA
    /* Validate device information */
    if (!DeviceValidate((PWDC_DEVICE)hDev))
        goto Error;
#endif /* ifndef ISA */
    /* Return handle to the new device */
    TraceLog("$$$nu$$$_DeviceOpen: Opened a $$$nu$$$ device (handle 0x%p)\n"
        "Device is %s using a Kernel PlugIn driver (%s)\n", hDev,
        (WDC_IS_KP(hDev))? "" : "not" , KP_$$$nu$$$_DRIVER_NAME);
    return hDev;

Error:
    if (hDev)
        $$$nu$$$_DeviceClose(hDev);
    else
        free(pDevCtx);

    return NULL;
}

/* Close device handle */
BOOL $$$nu$$$_DeviceClose(WDC_DEVICE_HANDLE hDev)
{
    DWORD dwStatus;
    P$$$nu$$$_DEV_CTX pDevCtx;

    TraceLog("$$$nu$$$_DeviceClose: Entered. Device handle 0x%p\n", hDev);

    /* Validate the device handle */
    if (!hDev)
    {
        ErrLog("$$$nu$$$_DeviceClose: Error - NULL device handle\n");
        return FALSE;
    }

    pDevCtx = (P$$$nu$$$_DEV_CTX)WDC_GetDevContext(hDev);

#ifdef HAS_INTS
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
#endif /* ifdef HAS_INTS */

    /* Close the device handle */
    dwStatus = WDC_$$$nl$$$DeviceClose(hDev);
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

#ifndef ISA
/* Validate device information */
static BOOL DeviceValidate(const PWDC_DEVICE pDev)
{
    DWORD i, dwNumAddrSpaces = pDev->dwNumAddrSpaces;

    /* NOTE: You can modify the implementation of this function in order to
             verify that the device has the resources you expect to find. */

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
#else /* ifdef ISA */
static void SetDeviceResources(WD_CARD *pDeviceInfo)
{
    WD_ITEMS *pItem;

    BZERO(*pDeviceInfo);

    pDeviceInfo->dwItems = $$$nu$$$_ITEMS_NUM;
    pItem = &pDeviceInfo->Item[0];
    /* Bus */
    pItem[0].item = ITEM_BUS;
    pItem[0].I.Bus.dwBusType = WD_BUS_ISA;

@@@devResources@@@
}
#endif /* ifdef ISA */

#ifdef HAS_INTS
/* -----------------------------------------------
    Interrupts
   ----------------------------------------------- */
/* Interrupt handler routine */
static void DLLCALLCONV $$$nu$$$_IntHandler(PVOID pData)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)pData;
    P$$$nu$$$_DEV_CTX pDevCtx = (P$$$nu$$$_DEV_CTX)(pDev->pCtx);
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
DWORD $$$nu$$$_IntEnable(WDC_DEVICE_HANDLE hDev, $$$nu$$$_INT_HANDLER funcIntHandler)
{
    DWORD dwStatus;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    P$$$nu$$$_DEV_CTX pDevCtx;

    @@@pAddrDesc@@@

    TraceLog("$$$nu$$$_IntEnable: Entered. Device handle 0x%p\n", hDev);

    /* Validate the device handle */
    if (!IsValidDevice(pDev, "$$$nu$$$_IntEnable"))
        return WD_INVALID_PARAMETER;

    /* Verify that the device has an interrupt item */
    if (!IsItemExists(pDev, ITEM_INTERRUPT))
        return WD_OPERATION_FAILED;

    pDevCtx = (P$$$nu$$$_DEV_CTX)(pDev->pCtx);

    /* Check whether interrupts are already enabled */
    if (WDC_IntIsEnabled(hDev))
    {
        ErrLog("Interrupts are already enabled ...\n");
        return WD_OPERATION_ALREADY_DONE;
    }

    @@@transfer_cmds@@@

    /* When using a Kernel PlugIn, acknowledge interrupts in kernel mode */
    if (!WDC_IS_KP(pDev))
    {
        /* TODO: Change this value, if needed */
        dwNumTransCmds = 2;

        /* This sample demonstrates how to set up two transfer commands, one
         * for reading the device's INTCSR register (as defined in g$$$nu$$$_Regs)
         * and one for writing to it to acknowledge the interrupt. The transfer
         * commands will be executed by WinDriver in the kernel when an
         * interrupt occurs.*/
#ifndef ISA
        /* TODO: If $$$nu$$$ interrupts are level sensitive interrupts, they must be
         * acknowledged in the kernel immediately when they are received. Since
         * the information for acknowledging the interrupts is
         * hardware-specific, YOU MUST MODIFY THE CODE below and set up transfer
         * commands in order to correctly acknowledge the interrupts on your
         * device, as dictated by your hardware's specifications.
         * If the device supports both MSI/MSI-X and level sensitive interrupts,
         * you must set up transfer commands in order to allow your code to run
         * correctly on systems other than Windows Vista and higher and Linux.
         * Since MSI/MSI-X does not require acknowledgment of the interrupt, to
         * support only MSI/MSI-X handling (for hardware and OSs that support
         * this), you can avoid defining transfer commands, or specify
         * kernel-mode commands to be performed upon interrupt generation
         * according to your specific needs. */
#endif /* ifndef ISA */
        /******************************************************************
         * NOTE: If you attempt to use this code without first modifying it in
         * order to correctly acknowledge your device's level-sensitive
         * interrupts, as explained above, the OS will HANG when a level
         * sensitive interrupt occurs!
         ********************************************************************/

        /* Allocate memory for the interrupt transfer commands */
        pTrans = (WD_TRANSFER *)calloc(dwNumTransCmds, sizeof(WD_TRANSFER));
        if (!pTrans)
        {
            ErrLog("Failed allocating memory for interrupt transfer "
                "commands\n");
            return WD_INSUFFICIENT_RESOURCES;
        }

        /* Prepare the interrupt transfer commands.
         *
         * The transfer commands will be executed by WinDriver's ISR
         * which runs in kernel mode at interrupt level.
         */

        /* TODO: Change the offset of INTCSR and the $$$nu$$$ address space, if
         *       needed */
        /* #1: Read status from the INTCSR register */
        pAddrDesc = WDC_GET_ADDR_DESC(pDev, INTCSR_ADDR_SPACE);

        pTrans[0].pPort = pAddrDesc->pAddr + INTCSR;
        /* Read from a 32-bit register */
        pTrans[0].cmdTrans = WDC_ADDR_IS_MEM(pAddrDesc) ? RM_DWORD : RP_DWORD;

        /* #2: Write ALL_INT_MASK to the INTCSR register to acknowledge the
         *     interrupt */
        pTrans[1].pPort = pTrans[0].pPort; /* In this example both commands
                                                access the same address
                                                (register) */
        /* Write to a 32-bit register */
        pTrans[1].cmdTrans = WDC_ADDR_IS_MEM(pAddrDesc) ? WM_DWORD : WP_DWORD;
        pTrans[1].Data.Dword = ALL_INT_MASK;

        /* Copy the results of "read" transfer commands back to user mode */
        dwOptions = INTERRUPT_CMD_COPY;
    }

    /* Store the diag interrupt handler routine, which will be executed by
       $$$nu$$$_IntHandler() when an interrupt is received */
    pDevCtx->funcDiagIntHandler = funcIntHandler;

    /* Enable interrupts */
    dwStatus = WDC_IntEnable(hDev, pTrans, dwNumTransCmds, dwOptions,
        $$$nu$$$_IntHandler, (PVOID)pDev, WDC_IS_KP(pDev));
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed enabling interrupts. Error 0x%lx - %s\n", dwStatus,
            Stat2Str(dwStatus));

        if (pTrans)
            free(pTrans);

        return dwStatus;
    }

    /* Store the interrupt transfer commands in the device context */
    pDevCtx->pIntTransCmds = pTrans;

    @@@transfer_cmds@@@
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
    P$$$nu$$$_DEV_CTX pDevCtx;

    TraceLog("$$$nu$$$_IntDisable entered. Device handle 0x%p\n", hDev);

    /* Validate the device handle */
    if (!IsValidDevice(pDev, "$$$nu$$$_IntDisable"))
        return WD_INVALID_PARAMETER;

    pDevCtx = (P$$$nu$$$_DEV_CTX)(pDev->pCtx);

    /* Check whether interrupts are already enabled */
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
@@@no_kp_ints_trns@@@
    /* Free the memory allocated for the interrupt transfer commands */
    if (pDevCtx->pIntTransCmds)
    {
        free(pDevCtx->pIntTransCmds);
        pDevCtx->pIntTransCmds = NULL;
    }
@@@no_kp_ints_trns@@@

    return dwStatus;
}

/* Check whether interrupts are enabled for the given device */
BOOL $$$nu$$$_IntIsEnabled(WDC_DEVICE_HANDLE hDev)
{
    /* Validate the device handle */
    if (!IsValidDevice((PWDC_DEVICE)hDev, "$$$nu$$$_IntIsEnabled"))
        return FALSE;

    /* Check whether interrupts are already enabled */
    return WDC_IntIsEnabled(hDev);
}
#endif /* ifdef HAS_INTS */

#ifndef ISA
/* -----------------------------------------------
    Plug-and-play and power management events
   ----------------------------------------------- */
/* Plug-and-play or power management event handler routine */
static void $$$nu$$$_EventHandler(WD_EVENT *pEvent, PVOID pData)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)pData;
    P$$$nu$$$_DEV_CTX pDevCtx = (P$$$nu$$$_DEV_CTX)(pDev->pCtx);

    TraceLog("$$$nu$$$_EventHandler entered, pData 0x%p, dwAction 0x%lx\n", pData,
        pEvent->dwAction);

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

    TraceLog("$$$nu$$$_EventRegister entered. Device handle 0x%p\n", hDev);

    /* Validate the device handle */
    if (!IsValidDevice(pDev, "$$$nu$$$_EventRegister"))
        return WD_INVALID_PARAMETER;

    pDevCtx = (P$$$nu$$$_DEV_CTX)(pDev->pCtx);

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

    TraceLog("$$$nu$$$_EventUnregister entered. Device handle 0x%p\n", hDev);

    /* Validate the device handle */
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
    /* Validate the device handle */
    if (!IsValidDevice((PWDC_DEVICE)hDev, "$$$nu$$$_EventIsRegistered"))
        return FALSE;

    /* Check whether the event is registered */
    return WDC_EventIsRegistered(hDev);
}
#endif /* ifndef ISA */
@@@kp@@@
#endif /* __KERNEL__ */
@@@kp@@@

/* -----------------------------------------------
    Address spaces information
   ----------------------------------------------- */
#ifndef ISA
/* Get number of address spaces */
DWORD $$$nu$$$_GetNumAddrSpaces(WDC_DEVICE_HANDLE hDev)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;

    /* Validate the device handle */
    if (!IsValidDevice(pDev, "$$$nu$$$_GetNumAddrSpaces"))
        return 0;

    /* Return the number of address spaces for the device */
    return pDev->dwNumAddrSpaces;
}
#endif /* ifndef ISA */

/* Get address space information */
BOOL $$$nu$$$_GetAddrSpaceInfo(WDC_DEVICE_HANDLE hDev,
    $$$nu$$$_ADDR_SPACE_INFO *pAddrSpaceInfo)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    WDC_ADDR_DESC *pAddrDesc;
    DWORD dwAddrSpace;
    BOOL fIsMemory;

    @@@devValidate@@@
    dwAddrSpace = pAddrSpaceInfo->dwAddrSpace;

    if (dwAddrSpace > pDev->dwNumAddrSpaces - 1)
    {
        ErrLog("$$$nu$$$_GetAddrSpaceInfo: Error - Address space %ld is "
            "out of range (0 - %ld)\n", dwAddrSpace, pDev->dwNumAddrSpaces - 1);
        return FALSE;
    }

    pAddrDesc = &pDev->pAddrDesc[dwAddrSpace];

    fIsMemory = WDC_ADDR_IS_MEM(pAddrDesc);

#ifndef ISA
    snprintf(pAddrSpaceInfo->sName, MAX_NAME - 1, "BAR %ld", dwAddrSpace);
#else /* ifdef ISA */
    snprintf(pAddrSpaceInfo->sName, MAX_NAME - 1, "AddrSpace %ld", dwAddrSpace);
#endif /* ifdef ISA */
    snprintf(pAddrSpaceInfo->sType, MAX_TYPE - 1, fIsMemory ? "Memory" : "I/O");

    if (WDC_AddrSpaceIsActive(pDev, dwAddrSpace))
    {
        WD_ITEMS *pItem = &pDev->cardReg.Card.Item[pAddrDesc->dwItemIndex];
        PHYS_ADDR pAddr = fIsMemory ? pItem->I.Mem.pPhysicalAddr :
            pItem->I.IO.pAddr;

        snprintf(pAddrSpaceInfo->sDesc, MAX_DESC - 1,
            "0x%0*"PRI64"X - 0x%0*"PRI64"X (0x%"PRI64"x bytes)",
            (int)WDC_SIZE_64 * 2, pAddr,
            (int)WDC_SIZE_64 * 2, pAddr + pAddrDesc->qwBytes - 1,
            pAddrDesc->qwBytes);
    }
    else
    {
        snprintf(pAddrSpaceInfo->sDesc, MAX_DESC - 1, "Inactive address space");
    }

    /* TODO: You can modify the code above to set a different address space
     * name/description. */

    return TRUE;
}
@@@regs_lib_rw_func@@@

/* -----------------------------------------------
    Debugging and error handling
   ----------------------------------------------- */
/* Log a debug error message */
static void ErrLog(const CHAR *sFormat, ...)
{
    va_list argp;

    va_start(argp, sFormat);
    vsnprintf(gs$$$nu$$$_LastErr, sizeof(gs$$$nu$$$_LastErr) - 1, sFormat, argp);
#ifdef DEBUG
@@@kp@@@
    #ifdef __KERNEL__
        WDC_Err("KP $$$nu$$$ lib: %s", gs$$$nu$$$_LastErr);
    #else
@@@kp@@@
        WDC_Err("$$$nu$$$ lib: %s", gs$$$nu$$$_LastErr);
@@@kp@@@
    #endif
@@@kp@@@
#endif
    va_end(argp);
}

/* Log a debug trace message */
static void TraceLog(const CHAR *sFormat, ...)
{
#ifdef DEBUG
    CHAR sMsg[256];
    va_list argp;

    va_start(argp, sFormat);
    vsnprintf(sMsg, sizeof(sMsg) - 1, sFormat, argp);
@@@kp@@@
#ifdef __KERNEL__
    WDC_Trace("KP $$$nu$$$ lib: %s", sMsg);
#else
@@@kp@@@
    WDC_Trace("$$$nu$$$ lib: %s", sMsg);
@@@kp@@@
#endif
@@@kp@@@
    va_end(argp);
#endif
}

/* Get last error */
const char *$$$nu$$$_GetLastErr(void)
{
    return gs$$$nu$$$_LastErr;
}

