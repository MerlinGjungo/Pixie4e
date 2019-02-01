/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

/************************************************************************
*  File: kp_pci.c
*
*  Kernel PlugIn driver for accessing PCI devices.
*  The code accesses hardware using WinDriver's WDC library.
*
*  Note: This code sample is provided AS-IS and as a guiding sample only.
*************************************************************************/

#include "kpstdlib.h"
#include "wd_kp.h"
#include "utils.h"
#include "wdc_defs.h"
#include "bits.h"
#include "pci_regs.h"
#include "../pci_lib.h"

/*************************************************************
  Functions prototypes
 *************************************************************/
BOOL __cdecl KP_PCI_Open(KP_OPEN_CALL *kpOpenCall, HANDLE hWD, PVOID pOpenData,
    PVOID *ppDrvContext);
BOOL __cdecl KP_PCI_Open_32_64(KP_OPEN_CALL *kpOpenCall, HANDLE hWD,
    PVOID pOpenData, PVOID *ppDrvContext);
void __cdecl KP_PCI_Close(PVOID pDrvContext);
void __cdecl KP_PCI_Call(PVOID pDrvContext, WD_KERNEL_PLUGIN_CALL *kpCall);
BOOL __cdecl KP_PCI_IntEnable(PVOID pDrvContext, WD_KERNEL_PLUGIN_CALL *kpCall,
    PVOID *ppIntContext);
void __cdecl KP_PCI_IntDisable(PVOID pIntContext);
BOOL __cdecl KP_PCI_IntAtIrql(PVOID pIntContext, BOOL *pfIsMyInterrupt);
DWORD __cdecl KP_PCI_IntAtDpc(PVOID pIntContext, DWORD dwCount);
BOOL __cdecl KP_PCI_IntAtIrqlMSI(PVOID pIntContext, ULONG dwLastMessage,
    DWORD dwReserved);
DWORD __cdecl KP_PCI_IntAtDpcMSI(PVOID pIntContext, DWORD dwCount,
    ULONG dwLastMessage, DWORD dwReserved);
BOOL __cdecl KP_PCI_Event(PVOID pDrvContext, WD_EVENT *wd_event);
static void KP_PCI_Err(const CHAR *sFormat, ...);
static void KP_PCI_Trace(const CHAR *sFormat, ...);
#define PTR32 UINT32

typedef struct {
    UINT32 dwNumAddrSpaces; /* Total number of device address spaces */
    PTR32  pAddrDesc;       /* Array of device address spaces information */
} PCI_DEV_ADDR_DESC_32B;

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

    kpInit->funcOpen = KP_PCI_Open;
    kpInit->funcOpen_32_64 = KP_PCI_Open_32_64;
#if defined(WINNT)
    strcpy(kpInit->cDriverName, KP_PCI_DRIVER_NAME);
#else
    strncpy(kpInit->cDriverName, KP_PCI_DRIVER_NAME,
        sizeof(kpInit->cDriverName));
#endif
    kpInit->cDriverName[sizeof(kpInit->cDriverName) - 1] = 0;

    return TRUE;
}

/* KP_PCI_Open is called when WD_KernelPlugInOpen() is called from the user
   mode. pDrvContext will be passed to the rest of the Kernel PlugIn callback
   functions. */
BOOL __cdecl KP_PCI_Open(KP_OPEN_CALL *kpOpenCall, HANDLE hWD, PVOID pOpenData,
    PVOID *ppDrvContext)
{
    PCI_DEV_ADDR_DESC *pDevAddrDesc;
    WDC_ADDR_DESC *pAddrDesc;
    DWORD dwSize;
    DWORD dwStatus;

    /* Initialize the PCI library */
    dwStatus = PCI_LibInit();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        KP_PCI_Err("KP_PCI_Open: Failed to initialize the PCI library. "
            "Last error [%s]\n", PCI_GetLastErr());
        return FALSE;
    }

    KP_PCI_Trace("KP_PCI_Open: Entered. PCI library initialized\n");

    kpOpenCall->funcClose = KP_PCI_Close;
    kpOpenCall->funcCall = KP_PCI_Call;
    kpOpenCall->funcIntEnable = KP_PCI_IntEnable;
    kpOpenCall->funcIntDisable = KP_PCI_IntDisable;
    kpOpenCall->funcIntAtIrql = KP_PCI_IntAtIrql;
    kpOpenCall->funcIntAtDpc = KP_PCI_IntAtDpc;
    kpOpenCall->funcIntAtIrqlMSI = KP_PCI_IntAtIrqlMSI;
    kpOpenCall->funcIntAtDpcMSI = KP_PCI_IntAtDpcMSI;
    kpOpenCall->funcEvent = KP_PCI_Event;

    /* Create a copy of device information in the driver context */
    dwSize = sizeof(PCI_DEV_ADDR_DESC);
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

    KP_PCI_Trace("KP_PCI_Open: Kernel PlugIn driver opened successfully\n");

    return TRUE;

malloc_error:
    KP_PCI_Err("KP_PCI_Open: Failed allocating [%ld] bytes\n", dwSize);
    if (pDevAddrDesc)
        free(pDevAddrDesc);
    PCI_LibUninit();
    return FALSE;
}

/* KP_PCI_Open_32_64 is called when WD_KernelPlugInOpen() is called from a
   32-bit user mode application to open a handle to a 64-bit Kernel PlugIn.
   pDrvContext will be passed to the rest of the Kernel PlugIn callback
   functions. */
BOOL __cdecl KP_PCI_Open_32_64(KP_OPEN_CALL *kpOpenCall, HANDLE hWD,
    PVOID pOpenData, PVOID *ppDrvContext)
{
    PCI_DEV_ADDR_DESC *pDevAddrDesc;
    PCI_DEV_ADDR_DESC_32B devAddrDesc_32;
    WDC_ADDR_DESC *pAddrDesc;
    DWORD dwSize;
    DWORD dwStatus;

    /* Initialize the PCI library */
    dwStatus = PCI_LibInit();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        KP_PCI_Err("KP_PCI_Open_32_64: Failed to initialize the PCI library. "
            "Last error [%s]\n", PCI_GetLastErr());
        return FALSE;
    }

    KP_PCI_Trace("KP_PCI_Open_32_64: Entered. PCI library initialized\n");

    kpOpenCall->funcClose = KP_PCI_Close;
    kpOpenCall->funcCall = KP_PCI_Call;
    kpOpenCall->funcIntEnable = KP_PCI_IntEnable;
    kpOpenCall->funcIntDisable = KP_PCI_IntDisable;
    kpOpenCall->funcIntAtIrql = KP_PCI_IntAtIrql;
    kpOpenCall->funcIntAtDpc = KP_PCI_IntAtDpc;
    kpOpenCall->funcIntAtIrqlMSI = KP_PCI_IntAtIrqlMSI;
    kpOpenCall->funcIntAtDpcMSI = KP_PCI_IntAtDpcMSI;
    kpOpenCall->funcEvent = KP_PCI_Event;

    /* Copy device information sent from a 32-bit user application */
    COPY_FROM_USER(&devAddrDesc_32, pOpenData, sizeof(PCI_DEV_ADDR_DESC_32B));

    /* Create a copy of the device information in the driver context */
    dwSize = sizeof(PCI_DEV_ADDR_DESC);
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

    KP_PCI_Trace("KP_PCI_Open_32_64: Kernel PlugIn driver opened "
        "successfully\n");

    return TRUE;

malloc_error:
    KP_PCI_Err("KP_PCI_Open_32_64: Failed allocating [%ld] bytes\n", dwSize);
    if (pDevAddrDesc)
        free(pDevAddrDesc);
    PCI_LibUninit();
    return FALSE;
}

/* KP_PCI_Close is called when WD_KernelPlugInClose() is called from
   the user mode */
void __cdecl KP_PCI_Close(PVOID pDrvContext)
{
    DWORD dwStatus;

    KP_PCI_Trace("KP_PCI_Close: Entered\n");

    /* Uninit the PCI library */
    dwStatus = PCI_LibUninit();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        KP_PCI_Err("KP_PCI_Close: Failed to uninit the PCI library. "
            "Last error [%s]\n", PCI_GetLastErr());
    }

    /* Free the memory allocated for the driver context */
    if (pDrvContext)
    {
        if (((PCI_DEV_ADDR_DESC *)pDrvContext)->pAddrDesc)
            free(((PCI_DEV_ADDR_DESC *)pDrvContext)->pAddrDesc);
        free(pDrvContext);
    }
}

/* KP_PCI_Call is called when WD_KernelPlugInCall() is called from the user
 * mode */
void __cdecl KP_PCI_Call(PVOID pDrvContext, WD_KERNEL_PLUGIN_CALL *kpCall)
{
    KP_PCI_Trace("KP_PCI_Call: Entered. Message [0x%lx]\n", kpCall->dwMessage);

    kpCall->dwResult = KP_PCI_STATUS_OK;

    switch (kpCall->dwMessage)
    {
    case KP_PCI_MSG_VERSION: /* Get the version of the Kernel PlugIn driver */
        {
            KP_PCI_VERSION *pUserKPVer = (KP_PCI_VERSION *)(kpCall->pData);
            KP_PCI_VERSION kernelKPVer;

            BZERO(kernelKPVer);
            kernelKPVer.dwVer = 100;
#define DRIVER_VER_STR "My Driver V1.00"
            memcpy(kernelKPVer.cVer, DRIVER_VER_STR, sizeof(DRIVER_VER_STR));
            COPY_TO_USER(pUserKPVer, &kernelKPVer, sizeof(KP_PCI_VERSION));
            kpCall->dwResult = KP_PCI_STATUS_OK;
        }
        break;

    default:
        kpCall->dwResult = KP_PCI_STATUS_MSG_NO_IMPL;
    }

    /* NOTE: You can modify the messages above and/or add your own
             Kernel PlugIn messages.
             When changing/adding messages, be sure to also update the
             messages definitions in ../pci_lib.h. */
}

/* KP_PCI_IntEnable is called when WD_IntEnable() is called from the user mode
   with a Kernel PlugIn handle.
   The interrupt context (pIntContext) will be passed to the rest of the
   Kernel PlugIn interrupt functions.
   The function returns TRUE if interrupts are enabled successfully. */
BOOL __cdecl KP_PCI_IntEnable(PVOID pDrvContext, WD_KERNEL_PLUGIN_CALL *kpCall,
    PVOID *ppIntContext)
{
    KP_PCI_Trace("KP_PCI_IntEnable: Entered\n");

    /* You can allocate specific memory for each interrupt in *ppIntContext */

    /* In this sample we will set the interrupt context to the driver context,
       which has been set in KP_PCI_Open to hold the device information. */
    *ppIntContext = pDrvContext;

    /* TODO: You can add code here to write to the device in order
             to physically enable the hardware interrupts */

    return TRUE;
}

/* KP_PCI_IntDisable is called when WD_IntDisable() is called from the
   user mode with a Kernel PlugIn handle */
void __cdecl KP_PCI_IntDisable(PVOID pIntContext)
{
    /* Free any memory allocated in KP_PCI_IntEnable() here */
}

/* KP_PCI_IntAtIrql returns TRUE if deferred interrupt processing (DPC) for
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
BOOL __cdecl KP_PCI_IntAtIrql(PVOID pIntContext, BOOL *pfIsMyInterrupt)
{
    static DWORD dwIntCount = 0; /* Interrupts count */
    PCI_DEV_ADDR_DESC *pDevAddrDesc = (PCI_DEV_ADDR_DESC *)pIntContext;
    WDC_ADDR_DESC *pAddrDesc = &pDevAddrDesc->pAddrDesc[INTCSR_ADDR_SPACE];

#define USE_MULTI_TRANSFER
#if defined USE_MULTI_TRANSFER
    /* Define the number of interrupt transfer commands to use */
    WD_TRANSFER trans[2];

    /*
       This sample demonstrates how to set up two transfer commands, one for
       reading the device's INTCSR register (as defined in gPCI_Regs) and one
       for writing to it to acknowledge the interrupt.

       TODO: PCI interrupts are level sensitive interrupts and must be
             acknowledged in the kernel immediately when they are received.
             Since the information for acknowledging the interrupts is
             hardware-specific, YOU MUST MODIFY THE CODE below and set up
             transfer commands in order to correctly acknowledge the interrupts
             on your device, as dictated by your hardware's specifications.

       *************************************************************************
       * NOTE: If you attempt to use this code without first modifying it in   *
       *       order to correctly acknowledge your device's interrupts, as     *
       *       explained above, the OS will HANG when an interrupt occurs!     *
       *************************************************************************
    */

    BZERO(trans);

    /* Prepare the interrupt transfer commands */

    /* #1: Read status from the INTCSR register */
    trans[0].pPort = pAddrDesc->pAddr + INTCSR;
    /* 32bit read: */
    trans[0].cmdTrans = WDC_ADDR_IS_MEM(pAddrDesc) ? RM_DWORD : RP_DWORD;

    /* #2: Write ALL_INT_MASK to the INTCSR register to acknowledge the
           interrupt */
    /* In this example both commands access the same address (register): */
    trans[1].pPort = trans[0].pPort;
    /* 32bit write: */
    trans[1].cmdTrans = WDC_ADDR_IS_MEM(pAddrDesc) ? WM_DWORD : WP_DWORD;
    trans[1].Data.Dword = ALL_INT_MASK;

    /* Execute the transfer commands */
    WDC_MultiTransfer(trans, 2);
#else
    /* NOTE: For memory registers you can replace the use of WDC_MultiTransfer()
       (or any other WD_xxx/WDC_xxx read/write function call) with direct
       memory access. For example, if INTCSR is a memory register, the code
       above can be replaced with the following: */

    UINT32 readData;
    PVOID pData = (DWORD *)(pAddrDesc->pAddr + INTCSR);

    /* Read status from the PCI_INTCSR register */
    readData = WDC_ReadMem32(pData, 0);

    /* Write to the PCI_INTCSR register to acknowledge the interrupt */
    WDC_WriteMem32(pData, 0, ALL_INT_MASK);
#endif
#undef USE_MULTI_TRANSFER

    /* If the data read from the hardware indicates that the interrupt belongs
       to you, you must set *pfIsMyInterrupt to TRUE.
       Otherwise, set it to FALSE (this will let ISR's of other drivers be
       invoked). */
    *pfIsMyInterrupt = FALSE;
    /* This sample schedules a DPC once in every 5 interrupts.
       TODO: You can modify the implementation to schedule the DPC as needed. */
    dwIntCount++;
    if (!(dwIntCount % 5))
        return TRUE;

    return FALSE;
}

/* KP_PCI_IntAtDpc is a Deferred Procedure Call for additional level-sensitive
   interrupt processing. This function is called if KP_PCI_IntAtIrql returned
   TRUE. KP_PCI_IntAtDpc returns the number of times to notify the user mode of
   the interrupt (i.e. return from WD_IntWait) */
DWORD __cdecl KP_PCI_IntAtDpc(PVOID pIntContext, DWORD dwCount)
{
    return dwCount;
}

/* KP_PCI_IntAtIrqlMSI returns TRUE if deferred interrupt processing (DPC) for
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
BOOL __cdecl KP_PCI_IntAtIrqlMSI(PVOID pIntContext, ULONG dwLastMessage,
    DWORD dwReserved)
{
    static DWORD dwIntCount = 0; /* Interrupts count */

    /* There is no need to acknowledge MSI/MSI-X. However, you can implement
       the same functionality here as done in the KP_PCI_IntAtIrql handler
       to read/write data from/to registers at HIGH IRQL. */

    /* This sample schedules a DPC once in every 5 interrupts.
       TODO: You can modify the implementation to schedule the DPC as needed. */
    dwIntCount++;
    if (!(dwIntCount % 5))
        return TRUE;

    return FALSE;
}

/* KP_PCI_IntAtDpcMSI is a Deferred Procedure Call for additional
   Message-Signaled-Interrupts (MSI) or Extended Message-Signaled Interrupts
   (MSI-X) processing.
   This function is called if KP_PCI_IntAtIrqlMSI returned TRUE.
   KP_PCI_IntAtDpcMSI returns the number of times to notify the user mode
   of the interrupt (i.e. return from WD_IntWait). */
DWORD __cdecl KP_PCI_IntAtDpcMSI(PVOID pIntContext, DWORD dwCount,
    ULONG dwLastMessage, DWORD dwReserved)
{
    return dwCount;
}

/* KP_PCI_Event is called when a Plug-and-Play/power management event for
   the device is received, if EventRegister() was first called from the
   user mode with the Kernel PlugIn handle. */
BOOL __cdecl KP_PCI_Event(PVOID pDrvContext, WD_EVENT *wd_event)
{
    return TRUE; /* Return TRUE to notify the user mode of the event */
}

/* -----------------------------------------------
    Debugging and error handling
   ----------------------------------------------- */
static void KP_PCI_Err(const CHAR *sFormat, ...)
{
#if defined(DEBUG)
    CHAR sMsg[256];
    va_list argp;

    va_start(argp, sFormat);
    vsnprintf(sMsg, sizeof(sMsg) - 1, sFormat, argp);
    WDC_Err("%s: %s", KP_PCI_DRIVER_NAME, sMsg);
    va_end(argp);
#endif
}

static void KP_PCI_Trace(const CHAR *sFormat, ...)
{
#if defined(DEBUG)
    CHAR sMsg[256];
    va_list argp;

    va_start(argp, sFormat);
    vsnprintf(sMsg, sizeof(sMsg) - 1, sFormat, argp);
    WDC_Trace("%s: %s", KP_PCI_DRIVER_NAME, sMsg);
    va_end(argp);
#endif
}

