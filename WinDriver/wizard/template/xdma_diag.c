/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

/****************************************************************************
*  File: $$$nl$$$_diag.c
*
*  Sample user-mode diagnostics application for accessing Xilinx PCI Express
*  cards with XDMA support, using the WinDriver WDC API.
*
*  Note: This code sample is provided AS-IS and as a guiding sample only.
*****************************************************************************/

#include <stdio.h>
#include "wdc_lib.h"
#include "utils.h"
#include "status_strings.h"
#include "samples/shared/diag_lib.h"
#include "samples/shared/wdc_diag_lib.h"
#include "pci_regs.h"
#include "$$$nl$$$_lib.h"
#include "$$$nl$$$_diag_transfer.h"

/*************************************************************
  General definitions
 *************************************************************/
/* Error messages display */
int $$$nu$$$_printf(char *fmt, ... )
#if defined(LINUX)
    __attribute__ ((format (printf, 1, 2)))
#endif
;

#define $$$nu$$$_ERR $$$nu$$$_printf

/*************************************************************
  Global variables
 *************************************************************/
/* User's input command */
static CHAR gsInput[256];

/* --------------------------------------------------
    $$$nu$$$ configuration registers information
   -------------------------------------------------- */
/* Configuration registers information array */
static const WDC_REG g$$$nu$$$_CfgRegs[] = {
    { WDC_AD_CFG_SPACE, PCI_VID, WDC_SIZE_16, WDC_READ_WRITE, "VID",
        "Vendor ID" },
    { WDC_AD_CFG_SPACE, PCI_DID, WDC_SIZE_16, WDC_READ_WRITE, "DID",
        "Device ID" },
    { WDC_AD_CFG_SPACE, PCI_CR, WDC_SIZE_16, WDC_READ_WRITE, "CMD",
        "Command" },
    { WDC_AD_CFG_SPACE, PCI_SR, WDC_SIZE_16, WDC_READ_WRITE, "STS", "Status" },
    { WDC_AD_CFG_SPACE, PCI_REV, WDC_SIZE_32, WDC_READ_WRITE, "RID_CLCD",
        "Revision ID & Class Code" },
    { WDC_AD_CFG_SPACE, PCI_CCSC, WDC_SIZE_8, WDC_READ_WRITE, "SCC",
        "Sub Class Code" },
    { WDC_AD_CFG_SPACE, PCI_CCBC, WDC_SIZE_8, WDC_READ_WRITE, "BCC",
        "Base Class Code" },
    { WDC_AD_CFG_SPACE, PCI_CLSR, WDC_SIZE_8, WDC_READ_WRITE, "CALN",
        "Cache Line Size" },
    { WDC_AD_CFG_SPACE, PCI_LTR, WDC_SIZE_8, WDC_READ_WRITE, "LAT",
        "Latency Timer" },
    { WDC_AD_CFG_SPACE, PCI_HDR, WDC_SIZE_8, WDC_READ_WRITE, "HDR",
        "Header Type" },
    { WDC_AD_CFG_SPACE, PCI_BISTR, WDC_SIZE_8, WDC_READ_WRITE, "BIST",
        "Built-in Self Test" },
    { WDC_AD_CFG_SPACE, PCI_BAR0, WDC_SIZE_32, WDC_READ_WRITE, "BADDR0",
        "Base Address 0" },
    { WDC_AD_CFG_SPACE, PCI_BAR1, WDC_SIZE_32, WDC_READ_WRITE, "BADDR1",
        "Base Address 1" },
    { WDC_AD_CFG_SPACE, PCI_BAR2, WDC_SIZE_32, WDC_READ_WRITE, "BADDR2",
        "Base Address 2" },
    { WDC_AD_CFG_SPACE, PCI_BAR3, WDC_SIZE_32, WDC_READ_WRITE, "BADDR3",
        "Base Address 3" },
    { WDC_AD_CFG_SPACE, PCI_BAR4, WDC_SIZE_32, WDC_READ_WRITE, "BADDR4",
        "Base Address 4" },
    { WDC_AD_CFG_SPACE, PCI_BAR5, WDC_SIZE_32, WDC_READ_WRITE, "BADDR5",
        "Base Address 5" },
    { WDC_AD_CFG_SPACE, PCI_CIS, WDC_SIZE_32, WDC_READ_WRITE, "CIS",
        "CardBus CIS Pointer" },
    { WDC_AD_CFG_SPACE, PCI_SVID, WDC_SIZE_16, WDC_READ_WRITE, "SVID",
        "Sub-system Vendor ID" },
    { WDC_AD_CFG_SPACE, PCI_SDID, WDC_SIZE_16, WDC_READ_WRITE, "SDID",
        "Sub-system Device ID" },
    { WDC_AD_CFG_SPACE, PCI_EROM, WDC_SIZE_32, WDC_READ_WRITE, "EROM",
        "Expansion ROM Base Address" },
    { WDC_AD_CFG_SPACE, PCI_CAP, WDC_SIZE_8, WDC_READ_WRITE, "NEW_CAP",
        "New Capabilities Pointer" },
    { WDC_AD_CFG_SPACE, PCI_ILR, WDC_SIZE_32, WDC_READ_WRITE, "INTLN",
        "Interrupt Line" },
    { WDC_AD_CFG_SPACE, PCI_IPR, WDC_SIZE_32, WDC_READ_WRITE, "INTPIN",
        "Interrupt Pin" },
    { WDC_AD_CFG_SPACE, PCI_MGR, WDC_SIZE_32, WDC_READ_WRITE, "MINGNT",
        "Minimum Required Burst Period" },
    { WDC_AD_CFG_SPACE, PCI_MLR, WDC_SIZE_32, WDC_READ_WRITE, "MAXLAT",
        "Maximum Latency" },
    };

#define $$$nu$$$_CFG_REGS_NUM (sizeof(g$$$nu$$$_CfgRegs)/sizeof(WDC_REG))

/* -----------------------------------------------
    $$$nu$$$ config block registers information
   ----------------------------------------------- */
/* Config block registers information array.
 * Note: The address space will be set after opening the device */
static WDC_REG g$$$nu$$$_ConfigRegs[] = {
    { (DWORD)-1, $$$nu$$$_CONFIG_BLOCK_IDENTIFIER_OFFSET, WDC_SIZE_32, WDC_READ,
        "Identifier", "" },
    { (DWORD)-1, $$$nu$$$_CONFIG_BLOCK_BUSDEV_OFFSET, WDC_SIZE_16, WDC_READ,
        "BusDev", "" },
    { (DWORD)-1, $$$nu$$$_CONFIG_BLOCK_PCIE_MAX_PAYLOAD_SIZE_OFFSET, WDC_SIZE_8,
        WDC_READ, "PCIE Max Payload Size", "" },
    { (DWORD)-1, $$$nu$$$_CONFIG_BLOCK_PCIE_MAX_READ_REQUEST_SIZE_OFFSET,
        WDC_SIZE_8, WDC_READ, "PCIE Max Read Request Size", "" },
    { (DWORD)-1, $$$nu$$$_CONFIG_BLOCK_SYSTEM_ID_OFFSET, WDC_SIZE_16, WDC_READ,
        "System ID", "" },
    { (DWORD)-1, $$$nu$$$_CONFIG_BLOCK_MSI_ENABLE_OFFSET, WDC_SIZE_8, WDC_READ,
        "MSI Enable", "" },
    { (DWORD)-1, $$$nu$$$_CONFIG_BLOCK_PCIE_DATA_WIDTH_OFFSET, WDC_SIZE_8, WDC_READ,
        "PCIE Data Width", "" },
    { (DWORD)-1, $$$nu$$$_CONFIG_PCIE_CONTROL_OFFSET, WDC_SIZE_8, WDC_READ_WRITE,
        "PCIE Control", "" },
    { (DWORD)-1, $$$nu$$$_CONFIG_AXI_USER_MAX_PAYLOAD_SIZE_OFFSET, WDC_SIZE_8,
        WDC_READ_WRITE, "AXI User Max Payload Size", "" },
    { (DWORD)-1, $$$nu$$$_CONFIG_AXI_USER_MAX_READ_REQUSEST_SIZE_OFFSET, WDC_SIZE_8,
        WDC_READ_WRITE, "AXI User Max Read Request Size", "" },
    { (DWORD)-1, $$$nu$$$_CONFIG_WRITE_FLUSH_TIMEOUT_OFFSET, WDC_SIZE_8,
        WDC_READ_WRITE, "Write Flush Timeout", "" },
};

#define $$$nu$$$_CONFIG_REGS_NUM (sizeof(g$$$nu$$$_ConfigRegs)/sizeof(WDC_REG))

/*************************************************************
  Static functions prototypes
 *************************************************************/
/* -----------------------------------------------
    Main diagnostics menu
   ----------------------------------------------- */
/* Main menu */
static void MenuMain(WDC_DEVICE_HANDLE *phDev);

/* -----------------------------------------------
    Device find, open and close
   ----------------------------------------------- */
static WDC_DEVICE_HANDLE DeviceFindAndOpen(DWORD dwVendorId, DWORD dwDeviceId);
static BOOL DeviceFind(DWORD dwVendorId, DWORD dwDeviceId, WD_PCI_SLOT *pSlot);
static WDC_DEVICE_HANDLE DeviceOpen(const WD_PCI_SLOT *pSlot);
static void DeviceClose(WDC_DEVICE_HANDLE hDev);

/* -----------------------------------------------
    Read/write memory and I/O addresses
   ----------------------------------------------- */
static void MenuReadWriteAddr(WDC_DEVICE_HANDLE hDev);
static void SetAddrSpace(WDC_DEVICE_HANDLE hDev, PDWORD pdwAddrSpace);

/* -----------------------------------------------
    Read/write the configuration space
   ----------------------------------------------- */
static void MenuReadWriteCfgSpace(WDC_DEVICE_HANDLE hDev);

/* -----------------------------------------------
    Read/write the config block registers
   ----------------------------------------------- */
static void MenuReadWriteConfigRegs(WDC_DEVICE_HANDLE hDev);

/* -----------------------------------------------
    Direct Memory Access (DMA)
   ---------------------------------------------- */
static void MenuDma(WDC_DEVICE_HANDLE hDev);
static void MenuDmaSingleTransfer(WDC_DEVICE_HANDLE hDev);
static void MenuDmaPerformance(WDC_DEVICE_HANDLE hDev);
static BOOL MenuDmaCompletionMethodGetInput(BOOL *pfPolling);

/* -----------------------------------------------
    Plug-and-play and power management events
   ----------------------------------------------- */
static void MenuEvents(WDC_DEVICE_HANDLE hDev);
static void DiagEventHandler(WDC_DEVICE_HANDLE hDev, DWORD dwAction);

/*************************************************************
  Functions implementation
 *************************************************************/
int main(void)
{
    WDC_DEVICE_HANDLE hDev = NULL;
    DWORD dwStatus, i, dwConfigBarNum;

    printf("\n");
    printf("$$$nu$$$ diagnostic utility.\n");
    printf("Application accesses hardware using " WD_PROD_NAME ".\n");

    /* Initialize the $$$nu$$$ library */
    dwStatus = $$$nu$$$_LibInit(NULL);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        $$$nu$$$_ERR("$$$nl$$$_diag: Failed to initialize the $$$nu$$$ library: %s",
            $$$nu$$$_GetLastErr());
        return dwStatus;
    }

    /* Find and open a $$$nu$$$ device (by default ID) */
    hDev = DeviceFindAndOpen($$$nu$$$_DEFAULT_VENDOR_ID, $$$nu$$$_DEFAULT_DEVICE_ID);

    /* Get the configuration BAR number */
    dwConfigBarNum = $$$nu$$$_ConfigBarNumGet(hDev);
    for (i = 0; i < $$$nu$$$_CONFIG_REGS_NUM; i++)
        g$$$nu$$$_ConfigRegs[i].dwAddrSpace = dwConfigBarNum;

    /* Display main diagnostics menu for communicating with the device and
     * performing DMA transfers */
    MenuMain(&hDev);

    /* Perform necessary cleanup before exiting the program: */
    /* Close the device handle */
    if (hDev)
        DeviceClose(hDev);

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
    MENU_MAIN_SCAN_PCI_BUS = 1,
    MENU_MAIN_FIND_AND_OPEN,
    MENU_MAIN_RW_ADDR,
    MENU_MAIN_RW_CFG_SPACE,
    MENU_MAIN_RW_REGS,
    MENU_MAIN_DMA,
    MENU_MAIN_EVENTS,
    MENU_MAIN_EXIT = DIAG_EXIT_MENU
};

/* Main diagnostics menu */
static void MenuMain(WDC_DEVICE_HANDLE *phDev)
{
    DWORD option;

    do
    {
        printf("\n");
        printf("$$$nu$$$ main menu\n");
        printf("--------------\n");
        printf("%d. Scan PCI bus\n", MENU_MAIN_SCAN_PCI_BUS);
        printf("%d. Find and open a $$$nu$$$ device\n", MENU_MAIN_FIND_AND_OPEN);
        if (*phDev)
        {
            printf("%d. Read/write memory and I/O addresses on the device\n",
                MENU_MAIN_RW_ADDR);
            printf("%d. Read/write the device's configuration space\n",
                MENU_MAIN_RW_CFG_SPACE);
            printf("%d. Read/write the configuration block registers\n",
                MENU_MAIN_RW_REGS);
            printf("%d. Direct Memory Access (DMA)\n", MENU_MAIN_DMA);
            printf("%d. Register/unregister plug-and-play and power management "
                "events\n", MENU_MAIN_EVENTS);
        }
        printf("%d. Exit\n", MENU_MAIN_EXIT);

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option,
            *phDev ? MENU_MAIN_EVENTS : MENU_MAIN_FIND_AND_OPEN))
        {
            continue;
        }

        switch (option)
        {
            case MENU_MAIN_EXIT: /* Exit menu */
                break;
            case MENU_MAIN_SCAN_PCI_BUS: /* Scan PCI bus */
                WDC_DIAG_PciDevicesInfoPrintAll(FALSE);
                break;
            case MENU_MAIN_FIND_AND_OPEN: /* Find and open a $$$nu$$$ device */
                if (*phDev)
                    DeviceClose(*phDev);
                *phDev = DeviceFindAndOpen(0, 0);
                break;
            case MENU_MAIN_RW_ADDR: /* Read/write memory and I/O addresses */
                MenuReadWriteAddr(*phDev);
                break;
            case MENU_MAIN_RW_CFG_SPACE: /* Read/write the configuration space
                                          */
                MenuReadWriteCfgSpace(*phDev);
                break;
            case MENU_MAIN_RW_REGS: /* Read/write the config block registers */
                MenuReadWriteConfigRegs(*phDev);
                break;
            case MENU_MAIN_DMA:
                MenuDma(*phDev);
                break;
            case MENU_MAIN_EVENTS: /* Register/unregister plug-and-play and
                                      power management events */
                MenuEvents(*phDev);
                break;
        }
    } while (MENU_MAIN_EXIT != option);
}

/* -----------------------------------------------
    Device find, open and close
   ----------------------------------------------- */
/* Find and open a $$$nu$$$ device */
static WDC_DEVICE_HANDLE DeviceFindAndOpen(DWORD dwVendorId, DWORD dwDeviceId)
{
    WD_PCI_SLOT slot;

    /* Find device */
    if (!DeviceFind(dwVendorId, dwDeviceId, &slot))
        return NULL;

    /* Open a device handle */
    return DeviceOpen(&slot);
}

/* Find a $$$nu$$$ device */
static BOOL DeviceFind(DWORD dwVendorId, DWORD dwDeviceId, WD_PCI_SLOT *pSlot)
{
    DWORD dwStatus;
    DWORD i, dwNumDevices;
    WDC_PCI_SCAN_RESULT scanResult;

    if (!dwVendorId)
    {
        /* Get vendor ID */
        if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD(&dwVendorId,
            "Enter vendor ID", TRUE, 0, 0))
        {
            return FALSE;
        }

        /* Get device ID */
        if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD(&dwDeviceId,
            "Enter device ID", TRUE, 0, 0))
        {
            return FALSE;
        }
    }

    /* Scan PCI devices */
    BZERO(scanResult);
    dwStatus = WDC_PciScanDevices(dwVendorId, dwDeviceId, &scanResult);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        $$$nu$$$_ERR("DeviceFind: Failed scanning the PCI bus.\n"
            "Error: 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        return FALSE;
    }

    dwNumDevices = scanResult.dwNumDevices;
    if (!dwNumDevices)
    {
        $$$nu$$$_ERR("No matching device was found for search criteria "
            "(Vendor ID 0x%lX, Device ID 0x%lX)\n", dwVendorId, dwDeviceId);
        return FALSE;
    }

    /* Display matching devices information */
    printf("\nFound %ld matching device(s) "
        "[Vendor ID 0x%lX%s, Device ID 0x%lX%s]:\n",
        dwNumDevices, dwVendorId, dwVendorId ? "" : " (ALL)",
        dwDeviceId, dwDeviceId ? "" : " (ALL)");

    for (i = 0; i < dwNumDevices; i++)
    {
        printf("\n%2ld. Vendor ID: 0x%lX, Device ID: 0x%lX\n", i + 1,
            scanResult.deviceId[i].dwVendorId,
            scanResult.deviceId[i].dwDeviceId);

        WDC_DIAG_PciDeviceInfoPrint(&scanResult.deviceSlot[i], FALSE);
    }
    printf("\n");

    /* Select device */
    if (dwNumDevices > 1)
    {
        sprintf(gsInput, "Select a device (1 - %ld): ", dwNumDevices);
        if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD(&i, gsInput, FALSE, 1,
            dwNumDevices))
        {
            return FALSE;
        }
    }

    *pSlot = scanResult.deviceSlot[i - 1];

    return TRUE;
}

/* Open a handle to a $$$nu$$$ device */
static WDC_DEVICE_HANDLE DeviceOpen(const WD_PCI_SLOT *pSlot)
{
    WDC_DEVICE_HANDLE hDev;
    DWORD dwStatus;
    WD_PCI_CARD_INFO deviceInfo;

    /* Retrieve the device's resources information */
    BZERO(deviceInfo);
    deviceInfo.pciSlot = *pSlot;
    dwStatus = WDC_PciGetDeviceInfo(&deviceInfo);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        $$$nu$$$_ERR("DeviceOpen: Failed retrieving the device's resources "
            "information.\nError 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        return NULL;
    }

    /* NOTE: You can modify the device's resources information here, if
       necessary (mainly the deviceInfo.Card.Items array or the items number -
       deviceInfo.Card.dwItems) in order to register only some of the resources
       or register only a portion of a specific address space, for example. */

    /* Open a handle to the device */
    hDev = $$$nu$$$_DeviceOpen(&deviceInfo, NULL);
    if (!hDev)
    {
        $$$nu$$$_ERR("DeviceOpen: Failed opening a handle to the device: %s\n",
            $$$nu$$$_GetLastErr());
        return NULL;
    }

    return hDev;
}

/* Close a handle to a $$$nu$$$ device */
static void DeviceClose(WDC_DEVICE_HANDLE hDev)
{
    /* Validate the WDC device handle */
    if (!hDev)
        return;

    /* Close the WDC device handle */
    if (!$$$nu$$$_DeviceClose(hDev))
    {
        $$$nu$$$_ERR("DeviceClose: Failed closing $$$nu$$$ device: %s",
            $$$nu$$$_GetLastErr());
    }
}

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
        DWORD dwNumAddrSpaces = $$$nu$$$_GetNumAddrSpaces(hDev);

        /* Find the first active address space */
        for (dwAddrSpace = 0; dwAddrSpace < dwNumAddrSpaces; dwAddrSpace++)
        {
            if (WDC_AddrSpaceIsActive(hDev, dwAddrSpace))
                break;
        }

        /* Sanity check */
        if (dwAddrSpace == dwNumAddrSpaces)
        {
            $$$nu$$$_ERR("MenuReadWriteAddr: Error - no active address spaces "
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
        printf("%d. Change active address space for read/write "
            "(currently: BAR %ld)\n", MENU_RW_ADDR_SET_ADDR_SPACE, dwAddrSpace);
        printf("%d. Change active read/write mode (currently: %s)\n",
            MENU_RW_ADDR_SET_MODE,
            (WDC_MODE_8 == mode) ? "8 bit" : (WDC_MODE_16 == mode) ? "16 bit" :
            (WDC_MODE_32 == mode) ? "32 bit" : "64 bit");
        printf("%d. Toggle active transfer type (currently: %s)\n",
            MENU_RW_ADDR_SET_TRANS_TYPE,
            (fBlock ? "block transfers" : "non-block transfers"));
        printf("%d. Read from active address space\n", MENU_RW_ADDR_READ);
        printf("%d. Write to active address space\n", MENU_RW_ADDR_WRITE);
        printf("%d. Exit menu\n", MENU_RW_ADDR_EXIT);
        printf("\n");

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option,
            MENU_RW_ADDR_WRITE))
        {
            continue;
        }

        switch (option)
        {
            case MENU_RW_ADDR_EXIT: /* Exit menu */
                break;
            case MENU_RW_ADDR_SET_ADDR_SPACE: /* Set active address space for
                                                 read/write address requests */
            {
                SetAddrSpace(hDev, &dwAddrSpace);
                break;
            }
            case MENU_RW_ADDR_SET_MODE: /* Set active mode for read/write
                                           address requests */
                WDC_DIAG_SetMode(&mode);
                break;
            case MENU_RW_ADDR_SET_TRANS_TYPE: /* Toggle active transfer type */
                fBlock = !fBlock;
                break;
            case MENU_RW_ADDR_READ:  /* Read from a memory or I/O address */
            case MENU_RW_ADDR_WRITE: /* Write to a memory or I/O address */
            {
                WDC_DIRECTION direction =
                    (MENU_RW_ADDR_READ == option) ? WDC_READ : WDC_WRITE;

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
    DWORD dwNumAddrSpaces = $$$nu$$$_GetNumAddrSpaces(hDev);

    printf("\n");
    printf("Select an active address space:\n");
    printf("-------------------------------\n");

    for (dwAddrSpace = 0; dwAddrSpace < dwNumAddrSpaces; dwAddrSpace++)
    {
        $$$nu$$$_ADDR_SPACE_INFO addrSpaceInfo;

        BZERO(addrSpaceInfo);
        addrSpaceInfo.dwAddrSpace = dwAddrSpace;
        if (!$$$nu$$$_GetAddrSpaceInfo(hDev, &addrSpaceInfo))
        {
            $$$nu$$$_ERR("SetAddrSpace: Error - Failed to get address space "
                "information: %s", $$$nu$$$_GetLastErr());
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
    MENU_RW_CFG_SPACE_EXIT = DIAG_EXIT_MENU,
};

/* Read/write configuration space menu */
static void MenuReadWriteCfgSpace(WDC_DEVICE_HANDLE hDev)
{
    DWORD option;

    do {
        /* Display predefined registers information */
        printf("\n");
        printf("Configuration registers:\n");
        printf("------------------------\n");
        WDC_DIAG_RegsInfoPrint(hDev, g$$$nu$$$_CfgRegs, $$$nu$$$_CFG_REGS_NUM,
            WDC_DIAG_REG_PRINT_ALL & ~WDC_DIAG_REG_PRINT_ADDR_SPACE, FALSE);

        printf("\nRead/write the device's configuration space\n");
        printf("--------------------------------------------\n");
        printf("%d. Read from an offset\n", MENU_RW_CFG_SPACE_READ_OFFSET);
        printf("%d. Write to an offset\n", MENU_RW_CFG_SPACE_WRITE_OFFSET);
        printf("%d. Read all configuration registers defined for the "
            "device (see list above)\n", MENU_RW_CFG_SPACE_READ_ALL_REGS);
        printf("%d. Read from a named register\n", MENU_RW_CFG_SPACE_READ_REG);
        printf("%d. Write to a named register\n", MENU_RW_CFG_SPACE_WRITE_REG);
        printf("%d. Exit menu\n", MENU_RW_CFG_SPACE_EXIT);
        printf("\n");

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option,
            $$$nu$$$_CFG_REGS_NUM ? MENU_RW_CFG_SPACE_WRITE_REG :
            MENU_RW_CFG_SPACE_WRITE_OFFSET))
        {
            continue;
        }

        switch (option)
        {
            case MENU_RW_CFG_SPACE_EXIT: /* Exit menu */
                break;
            case MENU_RW_CFG_SPACE_READ_OFFSET: /* Read from a configuration
                                                   space offset */
                WDC_DIAG_ReadWriteBlock(hDev, WDC_READ, WDC_AD_CFG_SPACE);
                break;
            case MENU_RW_CFG_SPACE_WRITE_OFFSET: /* Write to a configuration
                                                    space offset */
                WDC_DIAG_ReadWriteBlock(hDev, WDC_WRITE, WDC_AD_CFG_SPACE);
                break;
            case MENU_RW_CFG_SPACE_READ_ALL_REGS:
                WDC_DIAG_ReadRegsAll(hDev, g$$$nu$$$_CfgRegs, $$$nu$$$_CFG_REGS_NUM,
                    TRUE, FALSE);
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
        }
    } while (MENU_RW_CFG_SPACE_EXIT != option);
}

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

/* Read/write config block registers menu */
static void MenuReadWriteConfigRegs(WDC_DEVICE_HANDLE hDev)
{
    DWORD option;

    do {
        /* Display predefined registers information */
        printf("\n");
        printf("$$$nu$$$ config block registers:\n");
        printf("--------------------------\n");
        WDC_DIAG_RegsInfoPrint(hDev, g$$$nu$$$_ConfigRegs, $$$nu$$$_CONFIG_REGS_NUM,
            WDC_DIAG_REG_PRINT_ALL, FALSE);

        printf("\n");
        printf("Read/write the $$$nu$$$ config block registers\n");
        printf("-----------------------------------------\n");
        printf("%d. Read all config block registers "
            "(see list above)\n", MENU_RW_REGS_READ_ALL);
        printf("%d. Read from a register\n", MENU_RW_REGS_READ_REG);
        printf("%d. Write to a register\n", MENU_RW_REGS_WRITE_REG);
        printf("%d. Exit menu\n", MENU_RW_REGS_EXIT);
        printf("\n");

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option,
            MENU_RW_REGS_WRITE_REG))
        {
            continue;
        }

        switch (option)
        {
            case MENU_RW_REGS_EXIT: /* Exit menu */
                break;
            case MENU_RW_REGS_READ_ALL:
                WDC_DIAG_ReadRegsAll(hDev, g$$$nu$$$_ConfigRegs,
                    $$$nu$$$_CONFIG_REGS_NUM, FALSE, FALSE);
                break;
            case MENU_RW_REGS_READ_REG:  /* Read from a register */
                WDC_DIAG_ReadWriteReg(hDev, g$$$nu$$$_ConfigRegs,
                    $$$nu$$$_CONFIG_REGS_NUM, WDC_READ, FALSE);
                break;
            case MENU_RW_REGS_WRITE_REG: /* Write to a register */
                WDC_DIAG_ReadWriteReg(hDev, g$$$nu$$$_ConfigRegs,
                    $$$nu$$$_CONFIG_REGS_NUM, WDC_WRITE, FALSE);
                break;
        }
    } while (MENU_RW_REGS_EXIT != option);
}

/* -----------------------------------------------
    Direct Memory Access (DMA)
   ---------------------------------------------- */

/* DMA user input menu */
static BOOL MenuDmaTransferGetInput(DWORD *pdwChannel, BOOL *pfToDevice,
    UINT32 *pu32Pattern, DWORD *pdwNumPackets, UINT64 *pu64FPGAOffset,
    BOOL *pfPolling)
{
    DWORD option;

    if (!MenuDmaCompletionMethodGetInput(pfPolling))
        return FALSE;

    /* Get DMA direction and set the DMA options accordingly */
    printf("\nSelect DMA direction:");
    printf("\n---------------------\n");
    printf("1. From device\n");
    printf("2. To device\n");
    printf("%d. Cancel\n", DIAG_EXIT_MENU);
    if ((DIAG_INPUT_SUCCESS != DIAG_GetMenuOption(&option, 2)) ||
        (DIAG_EXIT_MENU == option))
    {
        return FALSE;
    }
    *pfToDevice = (1 == option) ? FALSE : TRUE;

    /* Get DMA buffer pattern for host to device transfer */
    if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD(pdwChannel,
        "\nSelect DMA channel (0 - 3)", FALSE, 0, 3))
    {
        return FALSE;
    }

    if (*pfToDevice)
    {
        /* Get DMA buffer pattern for host to device transfer */
        if (DIAG_INPUT_SUCCESS != DIAG_InputUINT32(pu32Pattern,
            "\nEnter DMA data pattern as 32 bit packet", TRUE, 0, 0))
        {
            return FALSE;
        }
    }

    /* Get data pattern */
    if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD(pdwNumPackets,
        "\nEnter number of packets to transfer (32 bit packets)", TRUE, 0, 0))
    {
        return FALSE;
    }
    if (*pdwNumPackets == 0)
    {
        $$$nu$$$_ERR("Illegal number of packets\n");
        return FALSE;
    }

    if (DIAG_INPUT_SUCCESS != DIAG_InputUINT64(pu64FPGAOffset,
        "\nEnter FPGA offset for transfer", TRUE, 0, 0))
    {
        return FALSE;
    }


    printf("\n");

    return TRUE;
}

/* DMA transfer menu options */
enum {
    MENU_DMA_SINGLE_OPEN_CLOSE = 1,
    MENU_DMA_SINGLE_EXIT = DIAG_EXIT_MENU
};

static void MenuDmaSingleTransfer(WDC_DEVICE_HANDLE hDev)
{
    DWORD option;
    BOOL fIsDmaOpen;
    $$$nu$$$_DMA_HANDLE hDma = NULL;

    do
    {
        fIsDmaOpen = hDma ? TRUE : FALSE;

        printf("\nOpen/close Direct Memory Access (DMA)\n");
        printf("----------------------------------\n");
        printf("%d. %s DMA\n", MENU_DMA_SINGLE_OPEN_CLOSE,
            fIsDmaOpen ? "Close" : "Open");
        printf("%d. Exit menu\n\n", MENU_DMA_SINGLE_EXIT);

        if (DIAG_INPUT_FAIL ==
            DIAG_GetMenuOption(&option, MENU_DMA_SINGLE_OPEN_CLOSE))
        {
            continue;
        }

        switch (option)
        {
            case MENU_DMA_SINGLE_EXIT: /* Exit menu */
                break;
            case MENU_DMA_SINGLE_OPEN_CLOSE: /* Open/close DMA menu option */
                if (!fIsDmaOpen)
                {
                    BOOL fPolling, fToDevice;
                    DWORD dwNumPackets, dwChannel;
                    UINT32 u32Pattern;
                    UINT64 u64FPGAOffset;


                    if (!MenuDmaTransferGetInput(&dwChannel, &fToDevice,
                        &u32Pattern, &dwNumPackets, &u64FPGAOffset, &fPolling))
                        break;

                    hDma = $$$nu$$$_DIAG_DmaOpen(hDev, fPolling, dwChannel,
                        fToDevice, u32Pattern, dwNumPackets, u64FPGAOffset);
                    if (!hDma)
                        $$$nu$$$_ERR("Failed opening DMA handle\n");
                }
                else
                {
                    $$$nu$$$_DIAG_DmaClose(hDev, hDma);
                    hDma = NULL;
                }
                break;
        }
    } while (MENU_DMA_SINGLE_EXIT != option);

    if (hDma)
    {
        $$$nu$$$_DIAG_DmaClose(hDev, hDma);
        printf("Closed DMA handle\n");
    }
}

static BOOL MenuDmaCompletionMethodGetInput(BOOL *pfPolling)
{
    DWORD option;

    printf("\nSelect DMA completion method:");
    printf("\n-----------------------------\n");
    printf("1. Interrupts\n");
    printf("2. Polling\n");
    printf("%d. Cancel\n", DIAG_EXIT_MENU);
    if ((DIAG_INPUT_SUCCESS != DIAG_GetMenuOption(&option, 2)) ||
        (DIAG_EXIT_MENU == option))
    {
        return FALSE;
    }
    *pfPolling = (1 == option) ? FALSE : TRUE;
    return TRUE;
}

/* DMA user input menu */
static BOOL MenuDmaPerformanceGetInput(BOOL *pfPolling, DWORD *pdwBytes,
    DWORD *pdwSeconds)
{
    DIAG_INPUT_RESULT inputResult;

    if (!MenuDmaCompletionMethodGetInput(pfPolling))
        return FALSE;

    inputResult= DIAG_InputDWORD(pdwBytes, "\nEnter single transfer buffer size"
        " in KBs", FALSE, 0, 0);
    switch (inputResult)
    {
    case DIAG_INPUT_SUCCESS:
        break;
    case DIAG_INPUT_FAIL:
        $$$nu$$$_ERR("\nInvalid transfer buffer size\n");
    case DIAG_INPUT_CANCEL:
        return FALSE;
    }

    *pdwBytes *= 1024;

    inputResult = DIAG_InputDWORD(pdwSeconds,"\nEnter test duration in seconds",
        FALSE, 0, 0);
    switch (inputResult)
    {
    case DIAG_INPUT_SUCCESS:
        break;
    case DIAG_INPUT_FAIL:
        $$$nu$$$_ERR("\nInvalid test duration\n");
    case DIAG_INPUT_CANCEL:
        return FALSE;
    }

    printf("\n");

    return TRUE;
}

/* DMA performance */
static void MenuDmaPerformance(WDC_DEVICE_HANDLE hDev)
{
    DWORD option, dwBytes, dwSeconds;
    BOOL fPolling;

    do {
        printf("\nDMA performance\n");
        printf("---------------\n");
        printf("%d. DMA host-to-device performance\n", MENU_DMA_PERF_TO_DEV);
        printf("%d. DMA device-to-host performance\n", MENU_DMA_PERF_FROM_DEV);
        printf("%d. DMA host-to-device and device-to-host performance running "
            "simultaneously\n", MENU_DMA_PERF_BIDIR);
        printf("%d. Exit menu\n\n", MENU_DMA_PERF_EXIT);

        if (DIAG_INPUT_FAIL ==
            DIAG_GetMenuOption(&option, MENU_DMA_PERF_BIDIR))
        {
            continue;
        }

        if (option == MENU_DMA_PERF_EXIT)
            break;

        if (!MenuDmaPerformanceGetInput(&fPolling, &dwBytes, &dwSeconds))
            continue;

        $$$nu$$$_DIAG_DmaPerformance(hDev, option, dwBytes, fPolling, dwSeconds);

    } while (option != MENU_DMA_PERF_EXIT);
}

/* DMA menu options */
enum {
    MENU_DMA_TRANSFER = 1,
    MENU_DMA_PERFORMANCE,
    MENU_DMA_EXIT = DIAG_EXIT_MENU
};

static void MenuDma(WDC_DEVICE_HANDLE hDev)
{
    DWORD option;

    do
    {
        printf("\n");
        printf("$$$nu$$$ DMA menu\n");
        printf("-------------\n");
        printf("%d. Perform DMA transfer\n", MENU_DMA_TRANSFER);
        printf("%d. Measure DMA performance\n", MENU_DMA_PERFORMANCE);
        printf("%d. Exit\n", MENU_MAIN_EXIT);

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option,
            MENU_DMA_PERFORMANCE))
        {
            continue;
        }

        switch (option)
        {
            case MENU_DMA_EXIT: /* Exit menu */
                break;
            case MENU_DMA_TRANSFER:
                MenuDmaSingleTransfer(hDev);
                break;
            case MENU_DMA_PERFORMANCE:
                MenuDmaPerformance(hDev);
                break;
        }
    } while (MENU_DMA_EXIT != option);
}

/* -----------------------------------------------
    Plug-and-play and power management events
   ----------------------------------------------- */
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
            case MENU_EVENTS_REGISTER_UNREGISTER: /* Register/unregister events
                                                   */
                if (fRegister)
                {
                    if (WD_STATUS_SUCCESS ==
                        $$$nu$$$_EventRegister(hDev, DiagEventHandler))
                    {
                        printf("Events registered\n");
                    }
                    else
                    {
                        $$$nu$$$_ERR("Failed to register events. Last error:\n%s",
                            $$$nu$$$_GetLastErr());
                    }
                }
                else
                {
                    if (WD_STATUS_SUCCESS == $$$nu$$$_EventUnregister(hDev))
                        printf("Events unregistered\n");
                    else
                    {
                        $$$nu$$$_ERR("Failed to unregister events. Last Error:\n%s",
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
             diagnostics events handler routine. */

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

int $$$nu$$$_printf(char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    return 0;
}

