/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

/************************************************************************
*  File: kp_$$$nl$$$.c
*
*  Kernel PlugIn driver for accessing $$$nu$$$ devices.
*  The code accesses hardware using WinDriver's WDC library.
@@@CODE_GEN@@@
*
*  Note: This code sample is provided AS-IS and as a guiding sample only.
*************************************************************************/

#include "kpstdlib.h"
#include "wd_kp.h"
#include "utils.h"
#include "wdc_defs.h"
#include "bits.h"
#ifndef ISA
#include "$$$nl$$$_regs.h"
#endif /* ifndef ISA */
#include "../$$$nl$$$_lib.h"

/*************************************************************
  Functions prototypes
 *************************************************************/
BOOL __cdecl KP_$$$nu$$$_Open(KP_OPEN_CALL *kpOpenCall, HANDLE hWD, PVOID pOpenData,
    PVOID *ppDrvContext);
@@@32on64@@@
BOOL __cdecl KP_$$$nu$$$_Open_32_64(KP_OPEN_CALL *kpOpenCall, HANDLE hWD,
    PVOID pOpenData, PVOID *ppDrvContext);
@@@32on64@@@
void __cdecl KP_$$$nu$$$_Close(PVOID pDrvContext);
void __cdecl KP_$$$nu$$$_Call(PVOID pDrvContext, WD_KERNEL_PLUGIN_CALL *kpCall);
BOOL __cdecl KP_$$$nu$$$_IntEnable(PVOID pDrvContext, WD_KERNEL_PLUGIN_CALL *kpCall,
    PVOID *ppIntContext);
void __cdecl KP_$$$nu$$$_IntDisable(PVOID pIntContext);
BOOL __cdecl KP_$$$nu$$$_IntAtIrql(PVOID pIntContext, BOOL *pfIsMyInterrupt);
DWORD __cdecl KP_$$$nu$$$_IntAtDpc(PVOID pIntContext, DWORD dwCount);
BOOL __cdecl KP_$$$nu$$$_IntAtIrqlMSI(PVOID pIntContext, ULONG dwLastMessage,
    DWORD dwReserved);
DWORD __cdecl KP_$$$nu$$$_IntAtDpcMSI(PVOID pIntContext, DWORD dwCount,
    ULONG dwLastMessage, DWORD dwReserved);
#ifndef ISA
BOOL __cdecl KP_$$$nu$$$_Event(PVOID pDrvContext, WD_EVENT *wd_event);
#endif /* ifndef ISA */
static void KP_$$$nu$$$_Err(const CHAR *sFormat, ...);
static void KP_$$$nu$$$_Trace(const CHAR *sFormat, ...);
@@@32on64@@@
#define PTR32 UINT32

typedef struct {
    UINT32 dwNumAddrSpaces; /* Total number of device address spaces */
    PTR32  pAddrDesc;       /* Array of device address spaces information */
} $$$nu$$$_DEV_ADDR_DESC_32B;
@@@32on64@@@

/*************************************************************
  Functions implementation
 *************************************************************/

/* KP_Init is called when the Kernel PlugIn driver is loaded.
   This function sets the name of the Kernel PlugIn driver and the driver's
   open callback function(s). */
BOOL __cdecl KP_Init(KP_INIT *kpInit)
{
    /* Verify that the version of the WinDriver Kernel PlugIn library
       is identical to that of the windrvr.h and wd_kp.h files */
    if (WD_VER != kpInit->dwVerWD)
    {
        /* Rebuild your Kernel PlugIn driver project with the compatible
           version of the WinDriver Kernel PlugIn library (kp_nt<version>.lib)
           and windrvr.h and wd_kp.h files */

        return FALSE;
    }

    kpInit->funcOpen = KP_$$$nu$$$_Open;
@@@32on64@@@
    kpInit->funcOpen_32_64 = KP_$$$nu$$$_Open_32_64;
@@@32on64@@@
#if defined(WINNT)
    strcpy(kpInit->cDriverName, KP_$$$nu$$$_DRIVER_NAME);
#else
    strncpy(kpInit->cDriverName, KP_$$$nu$$$_DRIVER_NAME,
        sizeof(kpInit->cDriverName));
#endif
    kpInit->cDriverName[sizeof(kpInit->cDriverName) - 1] = 0;

    return TRUE;
}

/* KP_$$$nu$$$_Open is called when WD_KernelPlugInOpen() is called from the user
   mode. pDrvContext will be passed to the rest of the Kernel PlugIn callback
   functions. */
BOOL __cdecl KP_$$$nu$$$_Open(KP_OPEN_CALL *kpOpenCall, HANDLE hWD, PVOID pOpenData,
    PVOID *ppDrvContext)
{
    $$$nu$$$_DEV_ADDR_DESC *pDevAddrDesc;
    WDC_ADDR_DESC *pAddrDesc;
    DWORD dwSize;
    DWORD dwStatus;

    /* Initialize the $$$nu$$$ library */
    dwStatus = $$$nu$$$_LibInit();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        KP_$$$nu$$$_Err("KP_$$$nu$$$_Open: Failed to initialize the $$$nu$$$ library. "
            "Last error [%s]\n", $$$nu$$$_GetLastErr());
        return FALSE;
    }

    KP_$$$nu$$$_Trace("KP_$$$nu$$$_Open: Entered. $$$nu$$$ library initialized\n");

    kpOpenCall->funcClose = KP_$$$nu$$$_Close;
    kpOpenCall->funcCall = KP_$$$nu$$$_Call;
    kpOpenCall->funcIntEnable = KP_$$$nu$$$_IntEnable;
    kpOpenCall->funcIntDisable = KP_$$$nu$$$_IntDisable;
    kpOpenCall->funcIntAtIrql = KP_$$$nu$$$_IntAtIrql;
    kpOpenCall->funcIntAtDpc = KP_$$$nu$$$_IntAtDpc;
    kpOpenCall->funcIntAtIrqlMSI = KP_$$$nu$$$_IntAtIrqlMSI;
    kpOpenCall->funcIntAtDpcMSI = KP_$$$nu$$$_IntAtDpcMSI;
#ifndef ISA
    kpOpenCall->funcEvent = KP_$$$nu$$$_Event;
#endif /* ifndef ISA */

    /* Create a copy of device information in the driver context */
    dwSize = sizeof($$$nu$$$_DEV_ADDR_DESC);
    pDevAddrDesc = malloc(dwSize);
    if (!pDevAddrDesc)
        goto malloc_error;

    COPY_FROM_USER(pDevAddrDesc, pOpenData, dwSize);

    dwSize = sizeof(WDC_ADDR_DESC) * pDevAddrDesc->dwNumAddrSpaces;
    pAddrDesc = malloc(dwSize);
    if (!pAddrDesc)
        goto malloc_error;

    COPY_FROM_USER(pAddrDesc, pDevAddrDesc->pAddrDesc, dwSize);
    pDevAddrDesc->pAddrDesc = pAddrDesc;

    *ppDrvContext = pDevAddrDesc;

    KP_$$$nu$$$_Trace("KP_$$$nu$$$_Open: Kernel PlugIn driver opened successfully\n");

    return TRUE;

malloc_error:
    KP_$$$nu$$$_Err("KP_$$$nu$$$_Open: Failed allocating [%ld] bytes\n", dwSize);
    if (pDevAddrDesc)
        free(pDevAddrDesc);
    $$$nu$$$_LibUninit();
    return FALSE;
}

@@@32on64@@@
/* KP_$$$nu$$$_Open_32_64 is called when WD_KernelPlugInOpen() is called from a
   32-bit user mode application to open a handle to a 64-bit Kernel PlugIn.
   pDrvContext will be passed to the rest of the Kernel PlugIn callback
   functions. */
BOOL __cdecl KP_$$$nu$$$_Open_32_64(KP_OPEN_CALL *kpOpenCall, HANDLE hWD,
    PVOID pOpenData, PVOID *ppDrvContext)
{
    $$$nu$$$_DEV_ADDR_DESC *pDevAddrDesc;
    $$$nu$$$_DEV_ADDR_DESC_32B devAddrDesc_32;
    WDC_ADDR_DESC *pAddrDesc;
    DWORD dwSize;
    DWORD dwStatus;

    /* Initialize the $$$nu$$$ library */
    dwStatus = $$$nu$$$_LibInit();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        KP_$$$nu$$$_Err("KP_$$$nu$$$_Open_32_64: Failed to initialize the $$$nu$$$ library. "
            "Last error [%s]\n", $$$nu$$$_GetLastErr());
        return FALSE;
    }

    KP_$$$nu$$$_Trace("KP_$$$nu$$$_Open_32_64: Entered. $$$nu$$$ library initialized\n");

    kpOpenCall->funcClose = KP_$$$nu$$$_Close;
    kpOpenCall->funcCall = KP_$$$nu$$$_Call;
    kpOpenCall->funcIntEnable = KP_$$$nu$$$_IntEnable;
    kpOpenCall->funcIntDisable = KP_$$$nu$$$_IntDisable;
    kpOpenCall->funcIntAtIrql = KP_$$$nu$$$_IntAtIrql;
    kpOpenCall->funcIntAtDpc = KP_$$$nu$$$_IntAtDpc;
    kpOpenCall->funcIntAtIrqlMSI = KP_$$$nu$$$_IntAtIrqlMSI;
    kpOpenCall->funcIntAtDpcMSI = KP_$$$nu$$$_IntAtDpcMSI;
#ifndef ISA
    kpOpenCall->funcEvent = KP_$$$nu$$$_Event;
#endif /* ifndef ISA */

    /* Copy device information sent from a 32-bit user application */
    COPY_FROM_USER(&devAddrDesc_32, pOpenData, sizeof($$$nu$$$_DEV_ADDR_DESC_32B));

    /* Create a copy of the device information in the driver context */
    dwSize = sizeof($$$nu$$$_DEV_ADDR_DESC);
    pDevAddrDesc = malloc(dwSize);
    if (!pDevAddrDesc)
        goto malloc_error;

    /* Copy the 32-bit data to a 64-bit struct */
    pDevAddrDesc->dwNumAddrSpaces = devAddrDesc_32.dwNumAddrSpaces;
    dwSize = sizeof(WDC_ADDR_DESC) * pDevAddrDesc->dwNumAddrSpaces;
    pAddrDesc = malloc(dwSize);
    if (!pAddrDesc)
        goto malloc_error;

    COPY_FROM_USER(pAddrDesc, (PVOID)(KPTR)devAddrDesc_32.pAddrDesc, dwSize);
    pDevAddrDesc->pAddrDesc = pAddrDesc;

    *ppDrvContext = pDevAddrDesc;

    KP_$$$nu$$$_Trace("KP_$$$nu$$$_Open_32_64: Kernel PlugIn driver opened "
        "successfully\n");

    return TRUE;

malloc_error:
    KP_$$$nu$$$_Err("KP_$$$nu$$$_Open_32_64: Failed allocating [%ld] bytes\n", dwSize);
    if (pDevAddrDesc)
        free(pDevAddrDesc);
    $$$nu$$$_LibUninit();
    return FALSE;
}
@@@32on64@@@

/* KP_$$$nu$$$_Close is called when WD_KernelPlugInClose() is called from
   the user mode */
void __cdecl KP_$$$nu$$$_Close(PVOID pDrvContext)
{
    DWORD dwStatus;

    KP_$$$nu$$$_Trace("KP_$$$nu$$$_Close: Entered\n");

    /* Uninit the $$$nu$$$ library */
    dwStatus = $$$nu$$$_LibUninit();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        KP_$$$nu$$$_Err("KP_$$$nu$$$_Close: Failed to uninit the $$$nu$$$ library. "
            "Last error [%s]\n", $$$nu$$$_GetLastErr());
    }

    /* Free the memory allocated for the driver context */
    if (pDrvContext)
    {
        if ((($$$nu$$$_DEV_ADDR_DESC *)pDrvContext)->pAddrDesc)
            free((($$$nu$$$_DEV_ADDR_DESC *)pDrvContext)->pAddrDesc);
        free(pDrvContext);
    }
}

/* KP_$$$nu$$$_Call is called when WD_KernelPlugInCall() is called from the user
 * mode */
void __cdecl KP_$$$nu$$$_Call(PVOID pDrvContext, WD_KERNEL_PLUGIN_CALL *kpCall)
{
    KP_$$$nu$$$_Trace("KP_$$$nu$$$_Call: Entered. Message [0x%lx]\n", kpCall->dwMessage);

    kpCall->dwResult = KP_$$$nu$$$_STATUS_OK;

    switch (kpCall->dwMessage)
    {
    case KP_$$$nu$$$_MSG_VERSION: /* Get the version of the Kernel PlugIn driver */
        {
            KP_$$$nu$$$_VERSION *pUserKPVer = (KP_$$$nu$$$_VERSION *)(kpCall->pData);
            KP_$$$nu$$$_VERSION kernelKPVer;

            BZERO(kernelKPVer);
            kernelKPVer.dwVer = 100;
#define DRIVER_VER_STR "My Driver V1.00"
            memcpy(kernelKPVer.cVer, DRIVER_VER_STR, sizeof(DRIVER_VER_STR));
            COPY_TO_USER(pUserKPVer, &kernelKPVer, sizeof(KP_$$$nu$$$_VERSION));
            kpCall->dwResult = KP_$$$nu$$$_STATUS_OK;
        }
        break;

    default:
        kpCall->dwResult = KP_$$$nu$$$_STATUS_MSG_NO_IMPL;
    }

    /* NOTE: You can modify the messages above and/or add your own
             Kernel PlugIn messages.
             When changing/adding messages, be sure to also update the
             messages definitions in ../$$$nl$$$_lib.h. */
}

/* KP_$$$nu$$$_IntEnable is called when WD_IntEnable() is called from the user mode
   with a Kernel PlugIn handle.
   The interrupt context (pIntContext) will be passed to the rest of the
   Kernel PlugIn interrupt functions.
   The function returns TRUE if interrupts are enabled successfully. */
BOOL __cdecl KP_$$$nu$$$_IntEnable(PVOID pDrvContext, WD_KERNEL_PLUGIN_CALL *kpCall,
    PVOID *ppIntContext)
{
    KP_$$$nu$$$_Trace("KP_$$$nu$$$_IntEnable: Entered\n");

    /* You can allocate specific memory for each interrupt in *ppIntContext */

    /* In this sample we will set the interrupt context to the driver context,
       which has been set in KP_$$$nu$$$_Open to hold the device information. */
    *ppIntContext = pDrvContext;

    /* TODO: You can add code here to write to the device in order
             to physically enable the hardware interrupts */

    return TRUE;
}

/* KP_$$$nu$$$_IntDisable is called when WD_IntDisable() is called from the
   user mode with a Kernel PlugIn handle */
void __cdecl KP_$$$nu$$$_IntDisable(PVOID pIntContext)
{
    /* Free any memory allocated in KP_$$$nu$$$_IntEnable() here */
}

/* KP_$$$nu$$$_IntAtIrql returns TRUE if deferred interrupt processing (DPC) for
   level-sensitive interrupts is required.
   The function is called at HIGH IRQL - at physical interrupt handler.
   Most library calls are NOT allowed at this level, for example:
   NO   WDC_xxx() or WD_xxx calls, apart from the WDC read/write address or
        register functions, WDC_MultiTransfer(), WD_Transfer(),
        WD_MultiTransfer() or WD_DebugAdd().
   NO   malloc().
   NO   free().
   YES  WDC read/write address or configuration space functions,
        WDC_MultiTransfer(), WD_Transfer(), WD_MultiTransfer() or
        WD_DebugAdd(), or wrapper functions that call these functions.
   YES  specific kernel OS functions (such as WinDDK functions) that can
        be called from HIGH IRQL. [Note that the use of such functions may
        break the code's portability to other OSs.] */
BOOL __cdecl KP_$$$nu$$$_IntAtIrql(PVOID pIntContext, BOOL *pfIsMyInterrupt)
{
    static DWORD dwIntCount = 0; /* Interrupts count */
    $$$nu$$$_DEV_ADDR_DESC *pDevAddrDesc = ($$$nu$$$_DEV_ADDR_DESC *)pIntContext;
@@@kp_IntAtIrql@@@
    /* This sample schedules a DPC once in every 5 interrupts.
       TODO: You can modify the implementation to schedule the DPC as needed. */
    dwIntCount++;
    if (!(dwIntCount % 5))
        return TRUE;

    return FALSE;
}

/* KP_$$$nu$$$_IntAtDpc is a Deferred Procedure Call for additional level-sensitive
   interrupt processing. This function is called if KP_$$$nu$$$_IntAtIrql returned
   TRUE. KP_$$$nu$$$_IntAtDpc returns the number of times to notify the user mode of
   the interrupt (i.e. return from WD_IntWait) */
DWORD __cdecl KP_$$$nu$$$_IntAtDpc(PVOID pIntContext, DWORD dwCount)
{
    return dwCount;
}

/* KP_$$$nu$$$_IntAtIrqlMSI returns TRUE if deferred interrupt processing (DPC) for
   Message-Signaled Interrupts (MSI) or Extended Message-Signaled Interrupts
   (MSI-X) processing is required.
   The function is called at HIGH IRQL - at physical interrupt handler.
   Note: Do not use the dwReserved parameter.
   Most library calls are NOT allowed at this level, for example:
   NO   WDC_xxx() or WD_xxx calls, apart from the WDC read/write address or
        register functions, WDC_MultiTransfer(), WD_Transfer(),
        WD_MultiTransfer() or WD_DebugAdd().
   NO   malloc().
   NO   free().
   YES  WDC read/write address or configuration space functions,
        WDC_MultiTransfer(), WD_Transfer(), WD_MultiTransfer() or
        WD_DebugAdd(), or wrapper functions that call these functions.
   YES  specific kernel OS functions (such as WinDDK functions) that can
        be called from HIGH IRQL. [Note that the use of such functions may
        break the code's portability to other OSs.] */
BOOL __cdecl KP_$$$nu$$$_IntAtIrqlMSI(PVOID pIntContext, ULONG dwLastMessage,
    DWORD dwReserved)
{
    static DWORD dwIntCount = 0; /* Interrupts count */

    /* There is no need to acknowledge MSI/MSI-X. However, you can implement
       the same functionality here as done in the KP_$$$nu$$$_IntAtIrql handler
       to read/write data from/to registers at HIGH IRQL. */

    /* This sample schedules a DPC once in every 5 interrupts.
       TODO: You can modify the implementation to schedule the DPC as needed. */
    dwIntCount++;
    if (!(dwIntCount % 5))
        return TRUE;

    return FALSE;
}

/* KP_$$$nu$$$_IntAtDpcMSI is a Deferred Procedure Call for additional
   Message-Signaled-Interrupts (MSI) or Extended Message-Signaled Interrupts
   (MSI-X) processing.
   This function is called if KP_$$$nu$$$_IntAtIrqlMSI returned TRUE.
   KP_$$$nu$$$_IntAtDpcMSI returns the number of times to notify the user mode
   of the interrupt (i.e. return from WD_IntWait). */
DWORD __cdecl KP_$$$nu$$$_IntAtDpcMSI(PVOID pIntContext, DWORD dwCount,
    ULONG dwLastMessage, DWORD dwReserved)
{
    return dwCount;
}

#ifndef ISA
/* KP_$$$nu$$$_Event is called when a Plug-and-Play/power management event for
   the device is received, if EventRegister() was first called from the
   user mode with the Kernel PlugIn handle. */
BOOL __cdecl KP_$$$nu$$$_Event(PVOID pDrvContext, WD_EVENT *wd_event)
{
    return TRUE; /* Return TRUE to notify the user mode of the event */
}
#endif /* ifndef ISA */

/* -----------------------------------------------
    Debugging and error handling
   ----------------------------------------------- */
static void KP_$$$nu$$$_Err(const CHAR *sFormat, ...)
{
#if defined(DEBUG)
    CHAR sMsg[256];
    va_list argp;

    va_start(argp, sFormat);
    vsnprintf(sMsg, sizeof(sMsg) - 1, sFormat, argp);
    WDC_Err("%s: %s", KP_$$$nu$$$_DRIVER_NAME, sMsg);
    va_end(argp);
#endif
}

static void KP_$$$nu$$$_Trace(const CHAR *sFormat, ...)
{
#if defined(DEBUG)
    CHAR sMsg[256];
    va_list argp;

    va_start(argp, sFormat);
    vsnprintf(sMsg, sizeof(sMsg) - 1, sFormat, argp);
    WDC_Trace("%s: %s", KP_$$$nu$$$_DRIVER_NAME, sMsg);
    va_end(argp);
#endif
}

