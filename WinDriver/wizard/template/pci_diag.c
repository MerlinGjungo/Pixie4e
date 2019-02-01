/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

/************************************************************************
*  File: $$$nl$$$_diag.c
*
*  Sample user-mode diagnostics application for accessing $$$nu$$$
*  devices, possibly via a Kernel PlugIn driver using WinDriver's API.
@@@CODE_GEN@@@
*
*  Note: This code sample is provided AS-IS and as a guiding sample only.
*************************************************************************/

#include <stdio.h>
#include "wdc_defs.h"
#include "wdc_lib.h"

#ifndef ISA
#include "wds_lib.h"
#endif /* ifndef ISA */

#include "utils.h"
#include "status_strings.h"
#include "samples/shared/diag_lib.h"
#include "samples/shared/wdc_diag_lib.h"

#ifndef ISA
#include "samples/shared/wds_diag_lib.h"
#endif /* ifndef ISA */

#include "$$$nl$$$_regs.h"
#include "$$$nl$$$_lib.h"

/*************************************************************
  General definitions
 *************************************************************/
/* Error messages display */
#define $$$nu$$$_ERR printf

/*************************************************************
  Global variables
 *************************************************************/
/* User's input command */
static CHAR gsInput[256];

#ifndef ISA
/* --------------------------------------------------
    $$$nu$$$ configuration registers information
   -------------------------------------------------- */
/* Configuration registers information array */
static const WDC_REG g$$$nu$$$_CfgRegs[] = {
    { WDC_AD_CFG_SPACE, $$$nu$$$_VID, WDC_SIZE_16, WDC_READ_WRITE, "VID",
        "Vendor ID" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_DID, WDC_SIZE_16, WDC_READ_WRITE, "DID",
        "Device ID" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_CR, WDC_SIZE_16, WDC_READ_WRITE, "CMD", "Command" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_SR, WDC_SIZE_16, WDC_READ_WRITE, "STS", "Status" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_REV, WDC_SIZE_32, WDC_READ_WRITE, "RID_CLCD",
        "Revision ID &\nClass Code" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_CCSC, WDC_SIZE_8, WDC_READ_WRITE, "SCC",
        "Sub Class Code" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_CCBC, WDC_SIZE_8, WDC_READ_WRITE, "BCC",
        "Base Class Code" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_CLSR, WDC_SIZE_8, WDC_READ_WRITE, "CALN",
        "Cache Line Size" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_LTR, WDC_SIZE_8, WDC_READ_WRITE, "LAT",
        "Latency Timer" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_HDR, WDC_SIZE_8, WDC_READ_WRITE, "HDR",
        "Header Type" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_BISTR, WDC_SIZE_8, WDC_READ_WRITE, "BIST",
        "Built-in Self Test" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_BAR0, WDC_SIZE_32, WDC_READ_WRITE, "BADDR0",
        "Base Address 0" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_BAR1, WDC_SIZE_32, WDC_READ_WRITE, "BADDR1",
        "Base Address 1" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_BAR2, WDC_SIZE_32, WDC_READ_WRITE, "BADDR2",
        "Base Address 2" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_BAR3, WDC_SIZE_32, WDC_READ_WRITE, "BADDR3",
        "Base Address 3" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_BAR4, WDC_SIZE_32, WDC_READ_WRITE, "BADDR4",
        "Base Address 4" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_BAR5, WDC_SIZE_32, WDC_READ_WRITE, "BADDR5",
        "Base Address 5" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_CIS, WDC_SIZE_32, WDC_READ_WRITE, "CIS",
        "CardBus CIS\npointer" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_SVID, WDC_SIZE_16, WDC_READ_WRITE, "SVID",
        "Sub-system\nVendor ID" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_SDID, WDC_SIZE_16, WDC_READ_WRITE, "SDID",
        "Sub-system\nDevice ID" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_EROM, WDC_SIZE_32, WDC_READ_WRITE, "EROM",
        "Expansion ROM\nBase Address" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_CAP, WDC_SIZE_8, WDC_READ_WRITE, "NEW_CAP",
        "New Capabilities\nPointer" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_ILR, WDC_SIZE_32, WDC_READ_WRITE, "INTLN",
        "Interrupt Line" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_IPR, WDC_SIZE_32, WDC_READ_WRITE, "INTPIN",
        "Interrupt Pin" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_MGR, WDC_SIZE_32, WDC_READ_WRITE, "MINGNT",
        "Minimum Required\nBurst Period" },
    { WDC_AD_CFG_SPACE, $$$nu$$$_MLR, WDC_SIZE_32, WDC_READ_WRITE, "MAXLAT",
        "Maximum Latency" },
};

static const WDC_REG g$$$nu$$$_ext_CfgRegs[] = {
    { WDC_AD_CFG_SPACE, $$$nu$$$E_CAP_ID, WDC_SIZE_8, WDC_READ_WRITE, "$$$nu$$$E_CAP_ID",
        "$$$nu$$$ Express\nCapability ID" },
    { WDC_AD_CFG_SPACE, NEXT_CAP_PTR, WDC_SIZE_8, WDC_READ_WRITE, "NEXT_CAP_PTR",
        "Next Capabiliy Pointer" },
    { WDC_AD_CFG_SPACE, CAP_REG, WDC_SIZE_16, WDC_READ_WRITE, "CAP_REG",
        "Capabilities Register" },
    { WDC_AD_CFG_SPACE, DEV_CAPS, WDC_SIZE_32, WDC_READ_WRITE, "DEV_CAPS",
        "Device Capabilities" },
    { WDC_AD_CFG_SPACE, DEV_CTL, WDC_SIZE_16, WDC_READ_WRITE, "DEV_CTL",
        "Device Control" },
    { WDC_AD_CFG_SPACE, DEV_STS, WDC_SIZE_16, WDC_READ_WRITE, "DEV_STS",
        "Device Status" },
    { WDC_AD_CFG_SPACE, LNK_CAPS, WDC_SIZE_32, WDC_READ_WRITE, "LNK_CAPS",
        "Link Capabilities" },
    { WDC_AD_CFG_SPACE, LNK_CTL, WDC_SIZE_16, WDC_READ_WRITE, "LNK_CTL",
        "Link Control" },
    { WDC_AD_CFG_SPACE, LNK_STS, WDC_SIZE_16, WDC_READ_WRITE, "LNK_STS",
        "Link Status" },
    { WDC_AD_CFG_SPACE, SLOT_CAPS, WDC_SIZE_32, WDC_READ_WRITE, "SLOT_CAPS",
        "Slot Capabilities" },
    { WDC_AD_CFG_SPACE, SLOT_CTL, WDC_SIZE_16, WDC_READ_WRITE, "SLOT_CTL",
        "Slot Control" },
    { WDC_AD_CFG_SPACE, SLOT_STS, WDC_SIZE_16, WDC_READ_WRITE, "SLOT_STS",
        "Slot Status" },
    { WDC_AD_CFG_SPACE, ROOT_CAPS, WDC_SIZE_16, WDC_READ_WRITE, "ROOT_CAPS",
        "Root Capabilities" },
    { WDC_AD_CFG_SPACE, ROOT_CTL, WDC_SIZE_16, WDC_READ_WRITE, "ROOT_CTL",
        "Root Control" },
    { WDC_AD_CFG_SPACE, ROOT_STS, WDC_SIZE_32, WDC_READ_WRITE, "ROOT_STS",
        "Root Status" },
    { WDC_AD_CFG_SPACE, DEV_CAPS2, WDC_SIZE_32, WDC_READ_WRITE, "DEV_CAPS2",
        "Device Capabilities 2" },
    { WDC_AD_CFG_SPACE, DEV_CTL2, WDC_SIZE_16, WDC_READ_WRITE, "DEV_CTL2",
        "Device Control 2" },
    { WDC_AD_CFG_SPACE, DEV_STS2, WDC_SIZE_16, WDC_READ_WRITE, "DEV_STS2",
        "Device Status 2" },
    { WDC_AD_CFG_SPACE, LNK_CAPS2, WDC_SIZE_32, WDC_READ_WRITE, "LNK_CAPS2",
        "Link Capabilities 2" },
    { WDC_AD_CFG_SPACE, LNK_CTL2, WDC_SIZE_16, WDC_READ_WRITE, "LNK_CTL2",
        "Link Control 2" },
    { WDC_AD_CFG_SPACE, LNK_STS2, WDC_SIZE_16, WDC_READ_WRITE, "LNK_STS2",
        "Link Status 2" },
    { WDC_AD_CFG_SPACE, SLOT_CAPS2, WDC_SIZE_32, WDC_READ_WRITE, "SLOT_CAPS2",
        "Slot Capabilities 2" },
    { WDC_AD_CFG_SPACE, SLOT_CTL2, WDC_SIZE_16, WDC_READ_WRITE, "SLOT_CTL2",
        "Slot Control 2" },
    { WDC_AD_CFG_SPACE, SLOT_STS2, WDC_SIZE_16, WDC_READ_WRITE, "SLOT_STS2",
        "Slot Status 2" },
};

#define $$$nu$$$_CFG_EXT_REGS_NUM sizeof(g$$$nu$$$_ext_CfgRegs) / sizeof(WDC_REG)
#define $$$nu$$$_CFG_REGS_NUM sizeof(g$$$nu$$$_CfgRegs) / sizeof(WDC_REG)

/* TODO: For read-only or write-only registers, change the direction field of
         the relevant registers in g$$$nu$$$_CfgRegs to WDC_READ or WDC_WRITE. */
/* NOTE: You can define additional configuration registers in g$$$nu$$$_CfgRegs. */
const WDC_REG *gp$$$nu$$$_CfgRegs = g$$$nu$$$_CfgRegs;

#endif /* ifndef ISA */

@@@regs_info@@@

/*************************************************************
  Static functions prototypes
 *************************************************************/
/* -----------------------------------------------
    Main diagnostics menu
   ----------------------------------------------- */
/* Main menu */
static void MenuMain(WDC_DEVICE_HANDLE *phDev);

#ifndef ISA
/* -----------------------------------------------
    Device find, open and close
   ----------------------------------------------- */
static WDC_DEVICE_HANDLE DeviceFindAndOpen(DWORD dwVendorId, DWORD dwDeviceId);
static BOOL DeviceFind(DWORD dwVendorId, DWORD dwDeviceId, WD_$$$nu$$$_SLOT *pSlot);
static WDC_DEVICE_HANDLE DeviceOpen(const WD_$$$nu$$$_SLOT *pSlot);
static void DeviceClose(WDC_DEVICE_HANDLE hDev);
#endif /* ifndef ISA */

/* -----------------------------------------------
    Read/write memory and I/O addresses
   ----------------------------------------------- */
static void MenuReadWriteAddr(WDC_DEVICE_HANDLE hDev);
static void SetAddrSpace(WDC_DEVICE_HANDLE hDev, PDWORD pdwAddrSpace);

#ifndef ISA
/* -----------------------------------------------
    Read/write the configuration space
   ----------------------------------------------- */
static void MenuReadWriteCfgSpace(WDC_DEVICE_HANDLE hDev);
#endif /* ifndef ISA */
@@@regs_menu_rw_header@@@

#ifdef HAS_INTS
/* -----------------------------------------------
    Interrupt handling
   ----------------------------------------------- */
static void MenuInterrupts(WDC_DEVICE_HANDLE hDev);
static void DiagIntHandler(WDC_DEVICE_HANDLE hDev, $$$nu$$$_INT_RESULT *pIntResult);
#endif /* ifdef HAS_INTS */

/* -----------------------------------------------
    DMA memory handling
   ----------------------------------------------- */
/* Allocate/free DMA memory menu */
static void MenuDma(WDC_DEVICE_HANDLE hDev);

#ifndef ISA
/* ----------------------------------------------------
    Plug-and-play and power management events handling
   ---------------------------------------------------- */
static void MenuEvents(WDC_DEVICE_HANDLE hDev);
static void DiagEventHandler(WDC_DEVICE_HANDLE hDev, DWORD dwAction);
#endif /* ifndef ISA */

/*************************************************************
  Functions implementation
 *************************************************************/
int main(void)
{
    WDC_DEVICE_HANDLE hDev = NULL;
    DWORD dwStatus;

    printf("\n");
    printf("$$$nu$$$ diagnostic utility.\n");
    printf("Application accesses hardware using " WD_PROD_NAME "\n");
@@@kp@@@
    printf("and a Kernel PlugIn driver (%s).\n", KP_$$$nu$$$_DRIVER_NAME);
@@@kp@@@

    /* Initialize the $$$nu$$$ library */
    dwStatus = $$$nu$$$_LibInit();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        $$$nu$$$_ERR("$$$nl$$$_diag: Failed to initialize the $$$nu$$$ library: %s",
            $$$nu$$$_GetLastErr());
        return dwStatus;
    }

#ifndef ISA
    PrintDbgMessage(D_ERROR, S_$$$nu$$$, "WinDriver user mode version %s\n",
        WD_VERSION_STR);

    /* Find and open a $$$nu$$$ device (by default ID) */
    if ($$$nu$$$_DEFAULT_VENDOR_ID)
        hDev = DeviceFindAndOpen($$$nu$$$_DEFAULT_VENDOR_ID, $$$nu$$$_DEFAULT_DEVICE_ID);
#else /* ifdef ISA */
    /* Open a handle to the device */
    hDev = $$$nu$$$_DeviceOpen();
    if (!hDev)
    {
        $$$nu$$$_ERR("$$$nl$$$_diag: Failed opening a handle to the device: %s",
            $$$nu$$$_GetLastErr());
        return 1;
    }
#endif /* ifdef ISA */

    /* Display main diagnostics menu for communicating with the device */
    MenuMain(&hDev);

    /* Perform necessary cleanup before exiting the program: */
    /* Close the device handle */
#ifndef ISA
    if (hDev)
        DeviceClose(hDev);
#else /* ifdef ISA */
    if (hDev && !$$$nu$$$_DeviceClose(hDev))
    {
        $$$nu$$$_ERR("$$$nl$$$_diag: Failed closing ISA device: %s",
            $$$nu$$$_GetLastErr());
    }
#endif /* ifdef ISA */

#ifndef ISA
    if (WDS_IsIpcRegistered())
        WDS_IpcUnRegister();
#endif /* ifndef ISA */

    /* Uninitialize libraries */
    dwStatus = $$$nu$$$_LibUninit();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        $$$nu$$$_ERR("$$$nl$$$_diag: Failed to uninitialize the $$$nu$$$ library: %s",
            $$$nu$$$_GetLastErr());
    }

    return dwStatus;
}

/* -----------------------------------------------
    Main diagnostics menu
   ----------------------------------------------- */
/* Main menu options */
enum {
#ifndef ISA
    MENU_MAIN_SCAN_BUS = 1,
    MENU_MAIN_FIND_AND_OPEN,
    MENU_MAIN_SHARED_BUFFER,
    MENU_MAIN_IPC,
    MENU_MAIN_RW_CFG_SPACE,
    MENU_MAIN_EVENTS,
    MENU_MAIN_RW_ADDR,
#else /* ifdef ISA */
    MENU_MAIN_RW_ADDR = 1,
    MENU_MAIN_SHARED_BUFFER,
    MENU_MAIN_IPC,
#endif /* ifdef ISA */
#ifdef HAS_INTS
    MENU_MAIN_ENABLE_DISABLE_INT,
#endif /* ifdef HAS_INTS */
    MENU_MAIN_ALLOC_FREE_DMA,
    @@@regs_enum_opt@@@
    MENU_MAIN_EXIT = DIAG_EXIT_MENU,
};

/* Main diagnostics menu */
static void MenuMain(WDC_DEVICE_HANDLE *phDev)
{
    DWORD option;

    do
    {
        printf("\n");
        printf("$$$nu$$$ main menu\n");
        printf("-------------\n");
#ifndef ISA
        printf("%d. Scan $$$nu$$$ bus\n", MENU_MAIN_SCAN_BUS);
        printf("%d. Find and open a $$$nu$$$ device\n", MENU_MAIN_FIND_AND_OPEN);
        printf("%d. Allocate/free Shared Buffer\n", MENU_MAIN_SHARED_BUFFER);
        printf("%d. Manage IPC\n", MENU_MAIN_IPC);
#endif /* ifndef ISA */
#ifndef ISA
        if (*phDev)
        {
#endif /* ifndef ISA */
#ifndef ISA
            printf("%d. Read/write the $$$nu$$$ configuration space\n",
                MENU_MAIN_RW_CFG_SPACE);
            printf("%d. Register/unregister plug-and-play and power management "
                "events\n", MENU_MAIN_EVENTS);
#endif /* ifndef ISA */
            printf("%d. Read/write memory and I/O addresses on the device\n",
                MENU_MAIN_RW_ADDR);
#ifdef HAS_INTS
            printf("%d. Enable/disable the device's interrupts\n",
                MENU_MAIN_ENABLE_DISABLE_INT);
#endif /* ifdef HAS_INTS */
            printf("%d. Allocate/free memory for DMA\n",
                MENU_MAIN_ALLOC_FREE_DMA);
            @@@regs_menu_print@@@
#ifndef ISA
        }
#endif /* ifndef ISA */
        printf("%d. Exit\n", MENU_MAIN_EXIT);
#ifndef ISA

    #ifndef $$$nu$$$_REGS_NUM
        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option,
            *phDev ? MENU_MAIN_ALLOC_FREE_DMA : MENU_MAIN_IPC))
        {
            continue;
        }
    #else /* ifdef $$$nu$$$_REGS_NUM */
        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option,
            *phDev ? MENU_MAIN_RW_REGS : MENU_MAIN_IPC))
        {
            continue;
        }
    #endif /* ifdef $$$nu$$$_REGS_NUM */
#else /* ifdef ISA */
        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option,
            MENU_MAIN_ALLOC_FREE_DMA))
        {
            continue;
        }
#endif /* ifdef ISA */

        switch (option)
        {
        case MENU_MAIN_EXIT: /* Exit menu */
            break;
#ifndef ISA
        case MENU_MAIN_SCAN_BUS: /* Scan bus */
            WDC_DIAG_$$$nl$$$DevicesInfoPrintAll(FALSE);
            break;
        case MENU_MAIN_FIND_AND_OPEN: /* Find and open a $$$nu$$$ device */
            if (*phDev)
                DeviceClose(*phDev);
            *phDev = DeviceFindAndOpen(0, 0);
            break;

        case MENU_MAIN_SHARED_BUFFER: /* Handle Shared Buffer Operations */
            MenuSharedBuffer();
            break;

        case MENU_MAIN_IPC: /* Register/unregister Inter-Process
                             * Communication */
            MenuIpc();
            break;
        case MENU_MAIN_RW_CFG_SPACE: /* Read/write the device's configuration
                                        space */
            MenuReadWriteCfgSpace(*phDev);
            break;
        case MENU_MAIN_EVENTS: /* Register/unregister plug-and-play and power
                                  management events */
            MenuEvents(*phDev);
            break;
#endif /* ifndef ISA */
        case MENU_MAIN_RW_ADDR: /* Read/write memory and I/O addresses */
            MenuReadWriteAddr(*phDev);
            break;
        case MENU_MAIN_ALLOC_FREE_DMA: /* Allocate/free DMA memory */
            MenuDma(*phDev);
            break;
        @@@regs_case@@@
#ifdef HAS_INTS
        case MENU_MAIN_ENABLE_DISABLE_INT: /* Enable/disable interrupts */
            MenuInterrupts(*phDev);
            break;
#endif /* ifdef HAS_INTS */
        }
    } while (MENU_MAIN_EXIT != option);
}

#ifndef ISA
/* -----------------------------------------------
    Device find, open and close
   ----------------------------------------------- */
/* Find and open a $$$nu$$$ device */
static WDC_DEVICE_HANDLE DeviceFindAndOpen(DWORD dwVendorId, DWORD dwDeviceId)
{
    WD_$$$nu$$$_SLOT slot;

    /* Find device */
    if (!DeviceFind(dwVendorId, dwDeviceId, &slot))
        return NULL;

    /* Open a device handle */
    return DeviceOpen(&slot);
}

/* Find a $$$nu$$$ device */
static BOOL DeviceFind(DWORD dwVendorId, DWORD dwDeviceId, WD_$$$nu$$$_SLOT *pSlot)
{
    DWORD dwStatus;
    DWORD i, dwNumDevices;
    WDC_$$$nu$$$_SCAN_RESULT scanResult;

    if (!dwVendorId)
    {
        /* Get vendor ID */
        if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD((PVOID)&dwVendorId,
            "Enter vendor ID", TRUE, 0, 0))
        {
            return FALSE;
        }

        /* Get device ID */
        if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD((PVOID)&dwDeviceId,
            "Enter device ID", TRUE, 0, 0))
        {
            return FALSE;
        }
    }

    /* Scan $$$nu$$$ devices */
    BZERO(scanResult);
    dwStatus = WDC_$$$nl$$$ScanDevices(dwVendorId, dwDeviceId, &scanResult);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        $$$nu$$$_ERR("DeviceFind: Failed scanning the $$$nu$$$ bus.\n"
            "Error [0x%lx - %s]\n", dwStatus, Stat2Str(dwStatus));
        return FALSE;
    }

    dwNumDevices = scanResult.dwNumDevices;
    if (!dwNumDevices)
    {
        printf("No matching $$$nu$$$ device was found for search criteria "
            "(Vendor ID 0x%lX, Device ID 0x%lX)\n", dwVendorId, dwDeviceId);

        return FALSE;
    }

    /* Display matching devices information */
    printf("\nFound %ld matching device%s "
        "[ Vendor ID 0x%lX%s, Device ID 0x%lX%s ]:\n",
        dwNumDevices, dwNumDevices > 1 ? "s" : "",
        dwVendorId, dwVendorId ? "" : " (ALL)",
        dwDeviceId, dwDeviceId ? "" : " (ALL)");

    for (i = 0; i < dwNumDevices; i++)
    {
        printf("\n");
        printf("%2ld. Vendor ID: 0x%lX, Device ID: 0x%lX\n", i + 1,
            scanResult.deviceId[i].dwVendorId,
            scanResult.deviceId[i].dwDeviceId);

        WDC_DIAG_$$$nl$$$DeviceInfoPrint(&scanResult.deviceSlot[i], FALSE);
    }
    printf("\n");

    /* Select device */
    if (dwNumDevices > 1)
    {
        sprintf(gsInput, "Select a device (1 - %ld): ", dwNumDevices);
        i = 0;
        if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD((PVOID)&i, gsInput, FALSE, 1,
            dwNumDevices))
        {
            return FALSE;
        }
    }

    *pSlot = scanResult.deviceSlot[i - 1];

    return TRUE;
}

@@@kp@@@
static DWORD CheckKPDriverVer(WDC_DEVICE_HANDLE hDev)
{
    KP_$$$nu$$$_VERSION kpVer;
    DWORD dwStatus;
    DWORD dwKPResult = 0;

    /* Get Kernel PlugIn Driver version */
    BZERO(kpVer);
    dwStatus = WDC_CallKerPlug(hDev, KP_$$$nu$$$_MSG_VERSION, &kpVer, &dwKPResult);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        $$$nu$$$_ERR("Failed sending a \'Get Version\' message [0x%x] to the "
            "Kernel-PlugIn driver [%s]. Error [0x%lx - %s]\n",
            KP_$$$nu$$$_MSG_VERSION, KP_$$$nu$$$_DRIVER_NAME, dwStatus,
            Stat2Str(dwStatus));
    }
    else if (KP_$$$nu$$$_STATUS_OK != dwKPResult)
    {
        $$$nu$$$_ERR("Kernel-PlugIn \'Get Version\' message [0x%x] failed. "
            "Kernel PlugIn status [0x%lx]\n", KP_$$$nu$$$_MSG_VERSION, dwKPResult);
        dwStatus = WD_INCORRECT_VERSION;
    }
    else
    {
        printf("Using [%s] Kernel-Plugin driver version [%ld.%02ld - %s]\n",
            KP_$$$nu$$$_DRIVER_NAME, kpVer.dwVer / 100, kpVer.dwVer % 100,
            kpVer.cVer);
    }

    return dwStatus;
}
@@@kp@@@

/* Open a handle to a $$$nu$$$ device */
static WDC_DEVICE_HANDLE DeviceOpen(const WD_$$$nu$$$_SLOT *pSlot)
{
    WDC_DEVICE_HANDLE hDev;
    DWORD dwStatus;
    WD_$$$nu$$$_CARD_INFO deviceInfo;

    /* Retrieve the device's resources information */
    BZERO(deviceInfo);
    deviceInfo.$$$nl$$$Slot = *pSlot;
    dwStatus = WDC_$$$nl$$$GetDeviceInfo(&deviceInfo);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        $$$nu$$$_ERR("DeviceOpen: Failed retrieving the device's resources "
            "information. Error [0x%lx - %s]\n", dwStatus, Stat2Str(dwStatus));
        return NULL;
    }

    /* NOTE: If necessary, you can modify the device's resources information
       here - mainly the information stored in the deviceInfo.Card.Items array,
       and the number of array items stored in deviceInfo.Card.dwItems.
       For example:
       - Edit the deviceInfo.Card.Items array and/or deviceInfo.Card.dwItems,
         to register only some of the resources or to register only a portion
         of a specific address space.
       - Set the fNotSharable field of one or more items in the
         deviceInfo.Card.Items array to 1, to block sharing of the related
         resources and ensure that they are locked for exclusive use.
    */

    /* Open a handle to the device */
    hDev = $$$nu$$$_DeviceOpen(&deviceInfo);
    if (!hDev)
    {
        $$$nu$$$_ERR("DeviceOpen: Failed opening a handle to the device: %s",
            $$$nu$$$_GetLastErr());
        return NULL;
    }

@@@kp@@@
    /* Get Kernel PlugIn driver version */
    if (WDC_IS_KP(hDev))
        CheckKPDriverVer(hDev);
@@@kp@@@

    return hDev;
}

/* Close handle to a $$$nu$$$ device */
static void DeviceClose(WDC_DEVICE_HANDLE hDev)
{
    /* Validate the WDC device handle */
    if (!hDev)
        return;

    /* Close the WDC device handle */
    if (!$$$nu$$$_DeviceClose(hDev))
        $$$nu$$$_ERR("DeviceClose: Failed closing $$$nu$$$ device: %s", $$$nu$$$_GetLastErr());
}
#endif /* ifndef ISA */

/* -----------------------------------------------
    Read/write memory and I/O addresses
   ----------------------------------------------- */
/* Read/write address menu options */
enum {
    MENU_RW_ADDR_SET_ADDR_SPACE = 1,
    MENU_RW_ADDR_SET_MODE,
    MENU_RW_ADDR_SET_TRANS_TYPE,
    MENU_RW_ADDR_READ,
    MENU_RW_ADDR_WRITE,
    MENU_RW_ADDR_EXIT = DIAG_EXIT_MENU,
};

#define ACTIVE_ADDR_SPACE_NEEDS_INIT 0xFF

/* Read/write memory or I/O space address menu */
static void MenuReadWriteAddr(WDC_DEVICE_HANDLE hDev)
{
    DWORD option;
    static DWORD dwAddrSpace = ACTIVE_ADDR_SPACE_NEEDS_INIT;
    static WDC_ADDR_MODE mode = WDC_MODE_32;
    static BOOL fBlock = FALSE;

    /* Initialize active address space */
    if (ACTIVE_ADDR_SPACE_NEEDS_INIT == dwAddrSpace)
    {
#ifndef ISA
        DWORD dwNumAddrSpaces = $$$nu$$$_GetNumAddrSpaces(hDev);
#else /* ifdef ISA */
        DWORD dwNumAddrSpaces = $$$nu$$$_ADDR_SPACES_NUM;
#endif /* ifdef ISA */
        /* Find the first active address space */
        for (dwAddrSpace = 0; dwAddrSpace < dwNumAddrSpaces; dwAddrSpace++)
        {
            if (WDC_AddrSpaceIsActive(hDev, dwAddrSpace))
                break;
        }

        /* Sanity check */
        if (dwAddrSpace == dwNumAddrSpaces)
        {
            $$$nu$$$_ERR("MenuReadWriteAddr: Error - No active address spaces "
                "found\n");
            dwAddrSpace = ACTIVE_ADDR_SPACE_NEEDS_INIT;
            return;
        }
    }

    do
    {
        printf("\n");
        printf("Read/write the device's memory and I/O ranges\n");
        printf("----------------------------------------------\n");
        printf("%d. Change active address space for read/write ",
            MENU_RW_ADDR_SET_ADDR_SPACE);
#ifndef ISA
        printf("(currently: BAR %ld)\n", dwAddrSpace);
#else /* ifdef ISA */
        printf("(currently: AddrSpace %ld)\n", dwAddrSpace);
#endif /* ifdef ISA */
        printf("%d. Change active read/write mode (currently: %s)\n",
            MENU_RW_ADDR_SET_MODE,
            (WDC_MODE_8 == mode) ? "8 bit" : (WDC_MODE_16 == mode) ? "16 bit" :
            (WDC_MODE_32 == mode) ? "32 bit" : "64 bit");
        printf("%d. Toggle active transfer type (currently: %s)\n",
            MENU_RW_ADDR_SET_TRANS_TYPE,
            fBlock ? "block transfers" : "non-block transfers");
        printf("%d. Read from active address space\n", MENU_RW_ADDR_READ);
        printf("%d. Write to active address space\n", MENU_RW_ADDR_WRITE);
        printf("%d. Exit menu\n", MENU_RW_ADDR_EXIT);
        printf("\n");

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option, MENU_RW_ADDR_WRITE))
            continue;

        switch (option)
        {
        case MENU_RW_ADDR_EXIT: /* Exit menu */
            break;
        case MENU_RW_ADDR_SET_ADDR_SPACE: /* Set active address space for
                                             read/write address requests */
            SetAddrSpace(hDev, &dwAddrSpace);
            break;
        case MENU_RW_ADDR_SET_MODE: /* Set active mode for read/write address
                                       requests */
            WDC_DIAG_SetMode(&mode);
            break;
        case MENU_RW_ADDR_SET_TRANS_TYPE: /* Toggle active transfer type */
             fBlock = !fBlock;
            break;
        case MENU_RW_ADDR_READ:  /* Read from a memory or I/O address */
        case MENU_RW_ADDR_WRITE: /* Write to a memory or I/O address */
        {
            WDC_DIRECTION direction =
                (MENU_RW_ADDR_READ == option ? WDC_READ : WDC_WRITE);

            if (fBlock)
                WDC_DIAG_ReadWriteBlock(hDev, direction, dwAddrSpace);
            else
                WDC_DIAG_ReadWriteAddr(hDev, direction, dwAddrSpace, mode);

            break;
        }
        }
    } while (MENU_RW_ADDR_EXIT != option);
}

/* Set address space */
static void SetAddrSpace(WDC_DEVICE_HANDLE hDev, PDWORD pdwAddrSpace)
{
    DWORD dwAddrSpace;
#ifndef ISA
    DWORD dwNumAddrSpaces = $$$nu$$$_GetNumAddrSpaces(hDev);
#else /* ifdef ISA */
    DWORD dwNumAddrSpaces = $$$nu$$$_ADDR_SPACES_NUM;
#endif /* ifdef ISA */
    $$$nu$$$_ADDR_SPACE_INFO addrSpaceInfo;

    printf("\n");
    printf("Select an active address space:\n");
    printf("-------------------------------\n");

    for (dwAddrSpace = 0; dwAddrSpace < dwNumAddrSpaces; dwAddrSpace++)
    {
        BZERO(addrSpaceInfo);
        addrSpaceInfo.dwAddrSpace = dwAddrSpace;
        if (!$$$nu$$$_GetAddrSpaceInfo(hDev, &addrSpaceInfo))
        {
            $$$nu$$$_ERR("SetAddrSpace: Error - Failed to get address space "
                "information. Last error [%s]", $$$nu$$$_GetLastErr());
            return;
        }

        printf("%ld. %-*s %-*s %s\n",
            dwAddrSpace + 1,
            MAX_NAME_DISPLAY, addrSpaceInfo.sName,
            MAX_TYPE - 1, addrSpaceInfo.sType,
            addrSpaceInfo.sDesc);
    }
    printf("\n");

    if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD((PVOID)&dwAddrSpace,
        "Enter option", FALSE, 1, dwNumAddrSpaces))
    {
        return;
    }

    dwAddrSpace--;
    if (!WDC_AddrSpaceIsActive(hDev, dwAddrSpace))
    {
        printf("You have selected an inactive address space\n");
        return;
    }

    *pdwAddrSpace = dwAddrSpace;
}

#ifndef ISA
/* -----------------------------------------------
    Read/write the configuration space
   ----------------------------------------------- */
/* Read/write the configuration space menu options */
enum {
    MENU_RW_CFG_SPACE_READ_OFFSET = 1,
    MENU_RW_CFG_SPACE_WRITE_OFFSET,
    MENU_RW_CFG_SPACE_READ_ALL_REGS,
    MENU_RW_CFG_SPACE_READ_REG,
    MENU_RW_CFG_SPACE_WRITE_REG,
    MENU_RW_CFG_SPACE_EXT_READ_REG,
    MENU_RW_CFG_SPACE_EXT_WRITE_REG,
    MENU_RW_CFG_SPACE_SCAN_CAP,
    MENU_RW_CFG_SPACE_EXIT = DIAG_EXIT_MENU,
};

/* Read/write configuration space menu */
static void MenuReadWriteCfgSpace(WDC_DEVICE_HANDLE hDev)
{
    DWORD option;
    BOOL fExpress = WDC_Get$$$nl$$$ExpressGen(hDev) != 0;
    do {
        /* Display predefined registers information */
        printf("\n");
        printf("Configuration registers:\n");
        printf("------------------------\n");
        WDC_DIAG_RegsInfoPrint(hDev, g$$$nu$$$_CfgRegs, $$$nu$$$_CFG_REGS_NUM,
            WDC_DIAG_REG_PRINT_ALL & ~WDC_DIAG_REG_PRINT_ADDR_SPACE, FALSE);
        if (fExpress)
        {
            WDC_DIAG_RegsInfoPrint(hDev, g$$$nu$$$_ext_CfgRegs, $$$nu$$$_CFG_EXT_REGS_NUM,
                WDC_DIAG_REG_PRINT_ALL & ~WDC_DIAG_REG_PRINT_ADDR_SPACE, TRUE);
        }
        printf("\n");
        printf("Read/write the device's configuration space\n");
        printf("--------------------------------------------\n");
        printf("%d. Read from an offset\n", MENU_RW_CFG_SPACE_READ_OFFSET);
        printf("%d. Write to an offset\n", MENU_RW_CFG_SPACE_WRITE_OFFSET);
        printf("%d. Read all configuration registers defined for the device "
            "(see list above)\n", MENU_RW_CFG_SPACE_READ_ALL_REGS);
        printf("%d. Read from a named register\n", MENU_RW_CFG_SPACE_READ_REG);
        printf("%d. Write to a named register\n", MENU_RW_CFG_SPACE_WRITE_REG);
        if (fExpress)
        {
            printf("%d. Read from a named $$$nu$$$ Express register\n",
                MENU_RW_CFG_SPACE_EXT_READ_REG);
            printf("%d. Write to a named $$$nu$$$ Express register\n",
                MENU_RW_CFG_SPACE_EXT_WRITE_REG);
        }
        printf("%d. Scan $$$nu$$$/$$$nu$$$e capabilities\n", MENU_RW_CFG_SPACE_SCAN_CAP);
        printf("%d. Exit menu\n", MENU_RW_CFG_SPACE_EXIT);
        printf("\n");

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option,
            MENU_RW_CFG_SPACE_SCAN_CAP))
        {
            continue;
        }

        switch (option)
        {
        case MENU_RW_CFG_SPACE_EXIT: /* Exit menu */
            break;
        case MENU_RW_CFG_SPACE_READ_OFFSET: /* Read from a configuration space
                                               offset */
            WDC_DIAG_ReadWriteBlock(hDev, WDC_READ, WDC_AD_CFG_SPACE);
            break;
        case MENU_RW_CFG_SPACE_WRITE_OFFSET: /* Write to a configuration space
                                                offset */
            WDC_DIAG_ReadWriteBlock(hDev, WDC_WRITE, WDC_AD_CFG_SPACE);
            break;
        case MENU_RW_CFG_SPACE_READ_ALL_REGS:
            WDC_DIAG_ReadRegsAll(hDev, g$$$nu$$$_CfgRegs, $$$nu$$$_CFG_REGS_NUM,
                TRUE, FALSE);
            if (fExpress)
            {
                WDC_DIAG_ReadRegsAll(hDev, g$$$nu$$$_ext_CfgRegs,
                    $$$nu$$$_CFG_EXT_REGS_NUM, TRUE, TRUE);
            }
            break;
        case MENU_RW_CFG_SPACE_READ_REG:  /* Read from a configuration
                                             register */
            WDC_DIAG_ReadWriteReg(hDev, g$$$nu$$$_CfgRegs, $$$nu$$$_CFG_REGS_NUM,
                WDC_READ, TRUE);
            break;
        case MENU_RW_CFG_SPACE_WRITE_REG: /* Write to a configuration
                                             register */
            WDC_DIAG_ReadWriteReg(hDev, g$$$nu$$$_CfgRegs, $$$nu$$$_CFG_REGS_NUM,
                WDC_WRITE, TRUE);
            break;
        case MENU_RW_CFG_SPACE_EXT_WRITE_REG: /* Write to a configuration
                                             $$$nu$$$ Express register */
            WDC_DIAG_ReadWriteReg(hDev, g$$$nu$$$_ext_CfgRegs, $$$nu$$$_CFG_EXT_REGS_NUM,
                WDC_WRITE, TRUE);
            break;
        case MENU_RW_CFG_SPACE_EXT_READ_REG:  /* Read from a configuration
                                             $$$nu$$$ Express register */
            WDC_DIAG_ReadWriteReg(hDev, g$$$nu$$$_ext_CfgRegs, $$$nu$$$_CFG_EXT_REGS_NUM,
                WDC_READ, TRUE);
            break;
        case MENU_RW_CFG_SPACE_SCAN_CAP: /* Scan $$$nu$$$/$$$nu$$$e capabilities */
            WDC_DIAG_Scan$$$nu$$$Capabilities(hDev);
            break;
        }
    } while (MENU_RW_CFG_SPACE_EXIT != option);
}

#endif /* ifndef ISA */
/* -----------------------------------------------
    Read/write the run-time registers
   ----------------------------------------------- */
/* Read/write the run-time registers menu options */
enum {
    MENU_RW_REGS_READ_ALL = 1,
    MENU_RW_REGS_READ_REG,
    MENU_RW_REGS_WRITE_REG,
    MENU_RW_REGS_EXIT = DIAG_EXIT_MENU,
};
@@@regs_menu_rw_func@@@
#ifdef HAS_INTS
/* -----------------------------------------------
    Interrupt handling
   ----------------------------------------------- */
/* Interrupts menu options */
enum {
    MENU_INT_ENABLE_DISABLE = 1,
    MENU_INT_EXIT = DIAG_EXIT_MENU,
};

/* Enable/Disable interrupts menu */
static void MenuInterrupts(WDC_DEVICE_HANDLE hDev)
{
    DWORD option, dwIntOptions;
    BOOL fIntEnable, fIsMsi;

    dwIntOptions = WDC_GET_INT_OPTIONS(hDev);
    fIsMsi = WDC_INT_IS_MSI(dwIntOptions);
    @@@has_level_int@@@
    if (dwIntOptions & INTERRUPT_LEVEL_SENSITIVE)
    {
        /* TODO: You can remove this message after you have modified the
           implementation of $$$nu$$$_IntEnable() in $$$nl$$$_lib.c to correctly
           acknowledge level-sensitive interrupts (see guidelines in
           $$$nu$$$_IntEnable()). */
        printf("\n");
        printf("WARNING!!!\n");
        printf("----------\n");
        printf("Your hardware has level sensitive interrupts.\n");
        printf("Before enabling the interrupts, %s first modify the source "
            "code\n of $$$nu$$$_IntEnable(), in the file $$$nl$$$_lib.c, to correctly "
            "acknowledge\n%s interrupts when they occur, as dictated by "
            "the hardware's specification.\n",
            fIsMsi ? "it is recommended that" : "you must",
            fIsMsi ? "level sensitive" : "");
    }

    @@@has_level_int@@@
    do
    {
        fIntEnable = !$$$nu$$$_IntIsEnabled(hDev);

        printf("\n");
        printf("Interrupts\n");
        printf("-----------\n");
        printf("%d. %s interrupts\n", MENU_INT_ENABLE_DISABLE,
            fIntEnable ? "Enable" : "Disable");
        printf("%d. Exit menu\n", MENU_INT_EXIT);
        printf("\n");

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option,
            MENU_INT_ENABLE_DISABLE))
        {
            continue;
        }

        switch (option)
        {
        case MENU_INT_EXIT: /* Exit menu */
            break;
        case MENU_INT_ENABLE_DISABLE: /* Enable/disable interrupts */
            if (fIntEnable)
            {
                DWORD dwStatus = $$$nu$$$_IntEnable(hDev, DiagIntHandler);

                if (WD_STATUS_SUCCESS == dwStatus)
                {
                    printf("Interrupts enabled\n");
                }
                else
                {
                    $$$nu$$$_ERR("Failed enabling interrupts. Error [0x%lx - %s]\n",
                        dwStatus, Stat2Str(dwStatus));
                }
            }
            else
            {
                if (WD_STATUS_SUCCESS == $$$nu$$$_IntDisable(hDev))
                {
                    printf("Interrupts disabled\n");
                }
                else
                {
                    $$$nu$$$_ERR("Failed disabling interrupts: %s",
                        $$$nu$$$_GetLastErr());
                }
            }
            break;
        }
    } while (MENU_INT_EXIT != option);
}

/* Diagnostics interrupt handler routine */
static void DiagIntHandler(WDC_DEVICE_HANDLE hDev, $$$nu$$$_INT_RESULT *pIntResult)
{
    /* TODO: You can modify this function in order to implement your own
             diagnostics interrupt handler routine */

    printf("Got interrupt number %ld\n", pIntResult->dwCounter);
    printf("Interrupt Type: %s\n",
        WDC_DIAG_IntTypeDescriptionGet(pIntResult->dwEnabledIntType));
    if (WDC_INT_IS_MSI(pIntResult->dwEnabledIntType))
        printf("Message Data: 0x%lx\n", pIntResult->dwLastMessage);
}
#endif /* ifdef HAS_INTS */
/* -----------------------------------------------
    DMA memory handling
   ----------------------------------------------- */
/* DMA menu options */
enum {
    MENU_DMA_ALLOCATE_CONTIG = 1,
    MENU_DMA_ALLOCATE_SG,
    MENU_DMA_RESERVED_MEM,
#ifndef ISA
    MENU_DMA_SHARE_CONTIG_BUF,
#endif /* ifndef ISA */
    MENU_DMA_FREE_MEM,
    MENU_DMA_EXIT = DIAG_EXIT_MENU,
};

static void FreeDmaMem(PVOID *ppBuf, WD_DMA **ppDma)
{
    DWORD dwStatus;
    BOOL fIsSG; /* Is Scatter Gather DMA */

    if (!(*ppDma))
        return;

    fIsSG = !((*ppDma)->dwOptions & DMA_KERNEL_BUFFER_ALLOC);

    dwStatus = WDC_DMABufUnlock(*ppDma);
    if (WD_STATUS_SUCCESS == dwStatus)
    {
        printf("DMA memory freed\n");
    }
    else
    {
        $$$nu$$$_ERR("Failed trying to free DMA memory. Error [0x%lx - %s]\n",
            dwStatus, Stat2Str(dwStatus));
    }

    if (fIsSG)
        free(*ppBuf);

    *ppBuf = NULL;
    *ppDma = NULL;
}

/* Allocate/free DMA memory menu */
static void MenuDma(WDC_DEVICE_HANDLE hDev)
{
    DWORD option, size, dwStatus;
    UINT64 qwAddr;
    PVOID pBuf = NULL;
    WD_DMA *pDma = NULL;

    do
    {
        printf("\n");
        printf("DMA memory\n");
        printf("-----------\n");
        printf("%d. Allocate contiguous memory\n", MENU_DMA_ALLOCATE_CONTIG);
        printf("%d. Allocate scatter-gather memory\n", MENU_DMA_ALLOCATE_SG);
        printf("%d. Use reserved memory\n", MENU_DMA_RESERVED_MEM);
#ifndef ISA
        if (pDma && pDma->dwOptions & DMA_KERNEL_BUFFER_ALLOC)
        {
            printf("%d. Send buffer through IPC to all group processes\n",
                MENU_DMA_SHARE_CONTIG_BUF);
        }
#endif /* ifndef ISA */
        printf("%d. Free DMA memory\n", MENU_DMA_FREE_MEM);
        printf("%d. Exit menu\n", MENU_DMA_EXIT);
        printf("\n");

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option, MENU_DMA_FREE_MEM))
            continue;

        if (option == MENU_DMA_RESERVED_MEM)
        {
            printf("Warning: The address for the reserved memory should be "
                "calculated according to the values listed in registry key\n"
                "HKLM/HARDWARE/RESOURCEMAP/System Resources/Physical Memory.\n"
                "Any other address may result in a BSOD. For more details "
                "please refer to Tech Doc #129\n\n");
            sprintf(gsInput, "Enter reserved memory address "
                "(64 bit hex uint) ");
            qwAddr = 0;
            if (DIAG_INPUT_SUCCESS != DIAG_InputUINT64(&qwAddr, gsInput, TRUE,
                1, 0xFFFFFFFFFFFFFFFF))
            {
                continue;
            }
        }

        if (option == MENU_DMA_ALLOCATE_CONTIG ||
            option == MENU_DMA_ALLOCATE_SG ||
            option == MENU_DMA_RESERVED_MEM)
        {
            sprintf(gsInput, "Enter memory allocation size in bytes "
                "(32 bit uint) ");
            size = 0;
            if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD((PVOID)&size, gsInput,
                FALSE, 1, 0xFFFFFFFF))
            {
                continue;
            }

            /* Free DMA memory before trying the new allocation */
            FreeDmaMem(&pBuf, &pDma);
        }

        switch (option)
        {
        case MENU_DMA_ALLOCATE_CONTIG: /* Allocate contiguous memory */
            dwStatus = WDC_DMAContigBufLock(hDev, &pBuf, 0 /* dwOptions */,
                size, &pDma);
            if (WD_STATUS_SUCCESS == dwStatus)
            {
                printf("Contiguous memory allocated. user addr [%p], "
                    "physical addr [0x%"PRI64"x], size [%ld(0x%lx)]\n", pBuf,
                    pDma->Page[0].pPhysicalAddr, pDma->Page[0].dwBytes,
                    pDma->Page[0].dwBytes);
            }
            else
            {
                $$$nu$$$_ERR("Failed allocating contiguous memory. size [%ld], "
                    "Error [0x%lx - %s]\n", size, dwStatus, Stat2Str(dwStatus));
            }
            break;

        case MENU_DMA_ALLOCATE_SG: /* Allocate scatter-gather memory */
            pBuf = malloc(size);
            if (!pBuf)
            {
                $$$nu$$$_ERR("Failed allocating user memory for SG. size [%ld]\n",
                    size);
                continue;
            }

            dwStatus = WDC_DMASGBufLock(hDev, pBuf, 0 /* dwOptions */, size,
                &pDma);
            if (WD_STATUS_SUCCESS == dwStatus)
            {
                DWORD i;

                printf("SG memory allocated. user addr [%p], size [%ld]\n",
                    pBuf, size);

                printf("Pages physical addresses:\n");
                for (i = 0; i < pDma->dwPages; i++)
                {
                    printf("%lu) physical addr [0x%"PRI64"x], "
                        "size [%ld(0x%lx)]\n", i + 1,
                        pDma->Page[i].pPhysicalAddr, pDma->Page[i].dwBytes,
                        pDma->Page[i].dwBytes);
                }
            }
            else
            {
                $$$nu$$$_ERR("Failed allocating SG memory. size [%ld], "
                    "Error [0x%lx - %s]\n", size, dwStatus, Stat2Str(dwStatus));
                free(pBuf);
            }
            break;

        case MENU_DMA_RESERVED_MEM:
            dwStatus = WDC_DMAReservedBufLock(hDev, qwAddr, &pBuf,
                0 /* dwOptions */, size, &pDma);
            if (WD_STATUS_SUCCESS == dwStatus)
            {
                printf("Reserved memory claimed. user addr [%p], "
                    "bus addr [0x%"PRI64"x], size [%ld(0x%lx)]\n", pBuf,
                    pDma->Page[0].pPhysicalAddr, pDma->Page[0].dwBytes,
                    pDma->Page[0].dwBytes);
            }
            else
            {
                $$$nu$$$_ERR("Failed claiming reserved memory. size [%ld], "
                    "Error [0x%lx - %s]\n", size, dwStatus, Stat2Str(dwStatus));
            }
            break;

#ifndef ISA
        case MENU_DMA_SHARE_CONTIG_BUF:
            WDS_DIAG_IpcSendDmaContigToGroup(pDma);
            break;
#endif /* ifndef ISA */
        case MENU_DMA_FREE_MEM: /* Free DMA memory */
            FreeDmaMem(&pBuf, &pDma);
            break;
        }
    } while (MENU_DMA_EXIT != option);

    /* Free DMA memory before exiting */
    FreeDmaMem(&pBuf, &pDma);
}

#ifndef ISA
/* ----------------------------------------------------
    Plug-and-play and power management events handling
   ---------------------------------------------------- */
/* Events menu options */
enum {
    MENU_EVENTS_REGISTER_UNREGISTER = 1,
    MENU_EVENTS_EXIT = DIAG_EXIT_MENU,
};

/* Register/unregister plug-and-play and power management events menu */
static void MenuEvents(WDC_DEVICE_HANDLE hDev)
{
    DWORD option;
    BOOL fRegister;

    do
    {
        fRegister = !$$$nu$$$_EventIsRegistered(hDev);

        printf("\n");
        printf("Plug-and-play and power management events\n");
        printf("------------------------------------------\n");
        printf("%d. %s events\n", MENU_EVENTS_REGISTER_UNREGISTER,
            fRegister ? "Register" : "Unregister");
        printf("%d. Exit menu\n", MENU_EVENTS_EXIT);
        printf("\n");

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option,
            MENU_EVENTS_REGISTER_UNREGISTER))
        {
            continue;
        }

        switch (option)
        {
        case MENU_EVENTS_EXIT: /* Exit menu */
            break;
        case MENU_EVENTS_REGISTER_UNREGISTER: /* Register/unregister events */
            if (fRegister)
            {
                if (WD_STATUS_SUCCESS == $$$nu$$$_EventRegister(hDev,
                    DiagEventHandler))
                {
                    printf("Events registered\n");
                }
                else
                {
                    $$$nu$$$_ERR("Failed to register events. Last error [%s]",
                        $$$nu$$$_GetLastErr());
                }
            }
            else
            {
                if (WD_STATUS_SUCCESS == $$$nu$$$_EventUnregister(hDev))
                {
                    printf("Events unregistered\n");
                }
                else
                {
                    $$$nu$$$_ERR("Failed to unregister events. Last error [%s]",
                        $$$nu$$$_GetLastErr());
                }
            }
            break;
        }
    } while (MENU_EVENTS_EXIT != option);
}

/* Diagnostics plug-and-play and power management events handler routine */
static void DiagEventHandler(WDC_DEVICE_HANDLE hDev, DWORD dwAction)
{
    /* TODO: You can modify this function in order to implement your own
     *       diagnostics events handler routine. */

    printf("\nReceived event notification (device handle 0x%p): ", hDev);
    switch (dwAction)
    {
    case WD_INSERT:
        printf("WD_INSERT\n");
        break;
    case WD_REMOVE:
        printf("WD_REMOVE\n");
        break;
    case WD_POWER_CHANGED_D0:
        printf("WD_POWER_CHANGED_D0\n");
        break;
    case WD_POWER_CHANGED_D1:
        printf("WD_POWER_CHANGED_D1\n");
        break;
    case WD_POWER_CHANGED_D2:
        printf("WD_POWER_CHANGED_D2\n");
        break;
    case WD_POWER_CHANGED_D3:
        printf("WD_POWER_CHANGED_D3\n");
        break;
    case WD_POWER_SYSTEM_WORKING:
        printf("WD_POWER_SYSTEM_WORKING\n");
        break;
    case WD_POWER_SYSTEM_SLEEPING1:
        printf("WD_POWER_SYSTEM_SLEEPING1\n");
        break;
    case WD_POWER_SYSTEM_SLEEPING2:
        printf("WD_POWER_SYSTEM_SLEEPING2\n");
        break;
    case WD_POWER_SYSTEM_SLEEPING3:
        printf("WD_POWER_SYSTEM_SLEEPING3\n");
        break;
    case WD_POWER_SYSTEM_HIBERNATE:
        printf("WD_POWER_SYSTEM_HIBERNATE\n");
        break;
    case WD_POWER_SYSTEM_SHUTDOWN:
        printf("WD_POWER_SYSTEM_SHUTDOWN\n");
        break;
    default:
        printf("0x%lx\n", dwAction);
        break;
    }
}
#endif /* ifndef ISA */
