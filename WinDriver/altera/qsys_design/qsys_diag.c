/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

/*****************************************************************************
*  File: qsys_diag.c
*
*  Sample user-mode diagnostics application for accessing Altera PCI Express
*  cards with Qsys design, using the WinDriver WDC API.
*  The sample was tested with Altera's Stratix IV GX development kit.
*  For more information on the Qsys design, refer to Altera's
*  "PCI Express in Qsys Example Designs" wiki page:
*  http://alterawiki.com/wiki/PCI_Express_in_Qsys_Example_Designs
*  You can download the WinDriver Development kit from here:
*  https://www.jungo.com/st/do/download_new.php?product=WinDriver&tag=GrandMenu
*
*  Note: This code sample is provided AS-IS and as a guiding sample only.
******************************************************************************/

#include <stdio.h>
#include "wdc_defs.h"
#include "wdc_lib.h"
#include "utils.h"
#include "status_strings.h"
#include "samples/shared/diag_lib.h"
#include "samples/shared/wdc_diag_lib.h"
#include "pci_regs.h"
#include "qsys_lib.h"

#if defined(UNIX)
    #include <sys/time.h>
#endif

/*************************************************************
  General definitions
 *************************************************************/
/* Error messages display */
#define QSYS_ERR printf

/*************************************************************
  Global variables
 *************************************************************/
/* User's input command */
static CHAR gsInput[256];

/* --------------------------------------------------
    Qsys configuration registers information
   -------------------------------------------------- */
/* Configuration registers information array */
const WDC_REG gQSYS_CfgRegs[] = {
    { WDC_AD_CFG_SPACE, PCI_VID, WDC_SIZE_16, WDC_READ_WRITE, "VID",
        "Vendor ID" },
    { WDC_AD_CFG_SPACE, PCI_DID, WDC_SIZE_16, WDC_READ_WRITE, "DID",
        "Device ID" },
    { WDC_AD_CFG_SPACE, PCI_CR, WDC_SIZE_16, WDC_READ_WRITE, "CMD", "Command" },
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
#define QSYS_CFG_REGS_NUM sizeof(gQSYS_CfgRegs) / sizeof(WDC_REG)
/* TODO: For read-only or write-only registers, change the direction field of
         the relevant registers in gQSYS_CfgRegs to WDC_READ or WDC_WRITE. */
/* NOTE: You can define additional configuration registers in gQSYS_CfgRegs.
 */
const WDC_REG *gpQSYS_CfgRegs = gQSYS_CfgRegs;

/* -----------------------------------------------
    Qsys run-time registers information
   ----------------------------------------------- */
/* Run-time registers information array */
/* const WDC_REG gQSYS_Regs[]; */
const WDC_REG *gpQSYS_Regs = NULL;
/* TODO: You can remove the comment from the gQSYS_Regs array declaration and
   fill the array with run-time registers information for your device. If you
   select to do so, be sure to set gpQSYS_Regs to point to gQSYS_Regs:
const WDC_REG *gpQSYS_Regs = gQSYS_Regs;
*/
#define QSYS_REGS_NUM 0

/*************************************************************
  Static functions prototypes
 *************************************************************/
/* -----------------------------------------------
    Main diagnostics menu
   ----------------------------------------------- */
/* Main diagnostics menu */
static void MenuMain(WDC_DEVICE_HANDLE *phDev);

/* -----------------------------------------------
    Device find, open and close
   ----------------------------------------------- */
static WDC_DEVICE_HANDLE DeviceFindAndOpen(DWORD dwVendorId, DWORD dwDeviceId);
static BOOL DeviceFind(DWORD dwVendorId, DWORD dwDeviceId, WD_PCI_SLOT *pSlot);
static WDC_DEVICE_HANDLE DeviceOpen(const WD_PCI_SLOT *pSlot);
static void DeviceClose(WDC_DEVICE_HANDLE hDev, QSYS_PDMA_STRUCT pDma);

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
    Read/write the run-time registers
   ----------------------------------------------- */
static void MenuReadWriteRegs(WDC_DEVICE_HANDLE hDev);

/* -----------------------------------------------
    Interrupts
   ----------------------------------------------- */
static void MenuInterrupts(WDC_DEVICE_HANDLE hDev);
static void DiagIntHandler(WDC_DEVICE_HANDLE hDev,
    QSYS_INT_RESULT *pIntResult);

/* -----------------------------------------------
    Plug-and-play and power management events
   ----------------------------------------------- */
static void MenuEvents(WDC_DEVICE_HANDLE hDev);
static void DiagEventHandler(WDC_DEVICE_HANDLE hDev, DWORD dwAction);

/* -----------------------------------------------
    Direct Memory Access (DMA)
   ---------------------------------------------- */
static void MenuDmaPolling(WDC_DEVICE_HANDLE hDev, QSYS_PDMA_STRUCT pDma);
static void DmaOpen(WDC_DEVICE_HANDLE hDev, QSYS_PDMA_STRUCT pDma);
static void DmaClose(WDC_DEVICE_HANDLE hDev, QSYS_PDMA_STRUCT pDma);
static void DmaTransferVerify(QSYS_PDMA_STRUCT pDma, PVOID pBuf, PVOID pBufOrig,
    BOOL fIsToDevice);

/* -----------------------------------------------
    Performance Tests
   ----------------------------------------------- */
static void MenuPerformance(WDC_DEVICE_HANDLE hDev, QSYS_PDMA_STRUCT pDma);
static void PerformanceTestDma(WDC_DEVICE_HANDLE hDev, QSYS_PDMA_STRUCT pDma);

/*************************************************************
  Functions implementation
 *************************************************************/
int main(void)
{
    WDC_DEVICE_HANDLE hDev = NULL;
    DWORD dwStatus;

    printf("\n");
    printf("Qsys diagnostic utility.\n");
    printf("Application accesses hardware using " WD_PROD_NAME ".\n");

    /* Initialize the Qsys library */
    dwStatus = QSYS_LibInit();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        QSYS_ERR("altera_diag: Failed to initialize the Qsys library: %s",
            QSYS_GetLastErr());
        return dwStatus;
    }

    /* Find and open a Qsys device (by default ID) */
    hDev = DeviceFindAndOpen(QSYS_DEFAULT_VENDOR_ID,
        QSYS_DEFAULT_DEVICE_ID);

    if (hDev && !QSYS_IsQsysRevision(hDev))
    {
        QSYS_ERR("altera_diag: This is not SOPC/Qsys design\n");
        return 0;
    }

    /* Display main diagnostics menu for communicating with the device */
    MenuMain(&hDev);

    /* Perform necessary cleanup before exiting the program: */
    /* Close the device handle */
    if (hDev)
        DeviceClose(hDev, NULL);

    /* Uninitialize libraries */
    dwStatus = QSYS_LibUninit();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        QSYS_ERR("altera_diag: Failed to uninitialize the Qsys library: %s",
            QSYS_GetLastErr());
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
    MENU_MAIN_DMA_POLLING,
    MENU_MAIN_ENABLE_DISABLE_INT,
    MENU_MAIN_EVENTS,
    MENU_MAIN_PERFORMANCE,
    MENU_MAIN_EXIT = DIAG_EXIT_MENU
};

/* Main diagnostics menu */
static void MenuMain(WDC_DEVICE_HANDLE *phDev)
{
    DWORD option;
    QSYS_DMA_STRUCT dma;

    BZERO(dma);

    do
    {
        printf("\n");
        printf("Qsys main menu\n");
        printf("--------------\n");
        printf("%d. Scan PCI bus\n", MENU_MAIN_SCAN_PCI_BUS);
        printf("%d. Find and open a Qsys device\n", MENU_MAIN_FIND_AND_OPEN);
        if (*phDev)
        {
            printf("%d. Read/write memory and I/O addresses on the device\n",
                MENU_MAIN_RW_ADDR);
            printf("%d. Read/write the device's configuration space\n",
                MENU_MAIN_RW_CFG_SPACE);
            printf("%d. Read/write the run-time registers\n",
                MENU_MAIN_RW_REGS);
            printf("%d. Open/close Direct Memory Access (DMA)\n",
                MENU_MAIN_DMA_POLLING);
            printf("%d. Enable/disable the device's interrupts\n",
                MENU_MAIN_ENABLE_DISABLE_INT);
            printf("%d. Register/unregister plug-and-play and power management "
                "events\n", MENU_MAIN_EVENTS);
            printf("%d. Run performance tests\n", MENU_MAIN_PERFORMANCE);
        }
        printf("%d. Exit\n", MENU_MAIN_EXIT);

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option,
            *phDev ? MENU_MAIN_PERFORMANCE : MENU_MAIN_FIND_AND_OPEN))
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
            case MENU_MAIN_FIND_AND_OPEN: /* Find and open a Qsys device */
                if (*phDev)
                    DeviceClose(*phDev, &dma);
                *phDev = DeviceFindAndOpen(0, 0);
                break;
            case MENU_MAIN_RW_ADDR: /* Read/write memory and I/O addresses */
                MenuReadWriteAddr(*phDev);
                break;
            case MENU_MAIN_RW_CFG_SPACE: /* Read/write the Qsys configuration
                                            space */
                MenuReadWriteCfgSpace(*phDev);
                break;
            case MENU_MAIN_RW_REGS: /* Read/write the run-time registers */
                    MenuReadWriteRegs(*phDev);
                break;
            case MENU_MAIN_DMA_POLLING: /* Open/close polling-based DMA */
                MenuDmaPolling(*phDev, &dma);
                break;
            case MENU_MAIN_ENABLE_DISABLE_INT: /* Enable/disable interrupts */
                MenuInterrupts(*phDev);
                break;
            case MENU_MAIN_EVENTS: /* Register/unregister plug-and-play and
                                      power management events */
                MenuEvents(*phDev);
                break;
            case MENU_MAIN_PERFORMANCE: /* Run performance tests */
                MenuPerformance(*phDev, &dma);
                break;
        }
    } while (MENU_MAIN_EXIT != option);
}

/* -----------------------------------------------
    Device find, open and close
   ----------------------------------------------- */
/* Find and open a Qsys device */
static WDC_DEVICE_HANDLE DeviceFindAndOpen(DWORD dwVendorId, DWORD dwDeviceId)
{
    WD_PCI_SLOT slot;

    /* Find device */
    if (!DeviceFind(dwVendorId, dwDeviceId, &slot))
        return NULL;

    /* Open a device handle */
    return DeviceOpen(&slot);
}

/* Find a Qsys device */
static BOOL DeviceFind(DWORD dwVendorId, DWORD dwDeviceId, WD_PCI_SLOT *pSlot)
{
    DWORD dwStatus;
    DWORD i, dwNumDevices;
    WDC_PCI_SCAN_RESULT scanResult;

    if (dwVendorId == 0)
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

    /* Scan PCI devices */
    BZERO(scanResult);
    dwStatus = WDC_PciScanDevices(dwVendorId, dwDeviceId, &scanResult);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        QSYS_ERR("DeviceFind: Failed scanning the PCI bus.\n"
            "Error: 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        return FALSE;
    }

    dwNumDevices = scanResult.dwNumDevices;
    if (!dwNumDevices)
    {
        QSYS_ERR("No matching device was found for search criteria "
            "(Vendor ID 0x%lX, Device ID 0x%lX)\n", dwVendorId, dwDeviceId);
        return FALSE;
    }

    /* Display matching devices information */
    printf("\nFound %ld matching device%s "
        "[Vendor ID 0x%lX%s, Device ID 0x%lX%s]:\n",
        dwNumDevices, dwNumDevices > 1 ? "s" : "",
        dwVendorId, dwVendorId ? "" : " (ALL)",
        dwDeviceId, dwDeviceId ? "" : " (ALL)");

    for (i = 0; i < dwNumDevices; i++)
    {
        printf("\n%2ld. Vendor ID: 0x%lX, Device ID: 0x%lX\n",
            i + 1,
            scanResult.deviceId[i].dwVendorId,
            scanResult.deviceId[i].dwDeviceId);

        WDC_DIAG_PciDeviceInfoPrint(&scanResult.deviceSlot[i], FALSE);
    }
    printf("\n");

    /* Select device */
    if (dwNumDevices > 1)
    {
        sprintf(gsInput, "Select a device (1 - %ld): ", dwNumDevices);
        i = 0;
        if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD((PVOID)&i,
            gsInput, FALSE, 1, dwNumDevices))
        {
            return FALSE;
        }
    }

    *pSlot = scanResult.deviceSlot[i - 1];

    return TRUE;
}

/* Open a handle to a Qsys device */
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
        QSYS_ERR("DeviceOpen: Failed retrieving the device's resources "
            "information.\nError 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        return NULL;
    }

    /* NOTE: You can modify the device's resources information here, if
       necessary (mainly the deviceInfo.Card.Items array or the items number -
       deviceInfo.Card.dwItems) in order to register only some of the resources
       or register only a portion of a specific address space, for example. */

    /* Open a handle to the device */
    hDev = QSYS_DeviceOpen(&deviceInfo, NULL);
    if (!hDev)
    {
        QSYS_ERR("DeviceOpen: Failed opening a handle to the device: %s",
            QSYS_GetLastErr());
        return NULL;
    }

    return hDev;
}

/* Close a handle to a Qsys device */
static void DeviceClose(WDC_DEVICE_HANDLE hDev, QSYS_PDMA_STRUCT pDma)
{
    /* Validate the WDC device handle */
    if (!hDev)
        return;

     /* Close the DMA handle (if open) */
     if (pDma)
         DmaClose(hDev, pDma);

    /* Close the WDC device handle */
    if (!QSYS_DeviceClose(hDev))
    {
        QSYS_ERR("DeviceClose: Failed closing Qsys device: %s",
            QSYS_GetLastErr());
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
        DWORD dwNumAddrSpaces = QSYS_GetNumAddrSpaces(hDev);

        /* Find the first active address space */
        for (dwAddrSpace = 0; dwAddrSpace < dwNumAddrSpaces; dwAddrSpace++)
        {
            if (WDC_AddrSpaceIsActive(hDev, dwAddrSpace))
                break;
        }

        /* Sanity check */
        if (dwAddrSpace == dwNumAddrSpaces)
        {
            QSYS_ERR("MenuReadWriteAddr: Error - no active address spaces "
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
    DWORD dwNumAddrSpaces = QSYS_GetNumAddrSpaces(hDev);
    QSYS_ADDR_SPACE_INFO addrSpaceInfo;

    printf("\n");
    printf("Select an active address space:\n");
    printf("-------------------------------\n");

    for (dwAddrSpace = 0; dwAddrSpace < dwNumAddrSpaces; dwAddrSpace++)
    {
        BZERO(addrSpaceInfo);
        addrSpaceInfo.dwAddrSpace = dwAddrSpace;
        if (!QSYS_GetAddrSpaceInfo(hDev, &addrSpaceInfo))
        {
            QSYS_ERR("SetAddrSpace: Error - Failed to get address space "
                "information: %s", QSYS_GetLastErr());
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
        WDC_DIAG_RegsInfoPrint(hDev, gQSYS_CfgRegs, QSYS_CFG_REGS_NUM,
            WDC_DIAG_REG_PRINT_ALL & ~WDC_DIAG_REG_PRINT_ADDR_SPACE, FALSE);

        printf("Read/write the device's configuration space\n");
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
            QSYS_CFG_REGS_NUM ? MENU_RW_CFG_SPACE_WRITE_REG :
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
                WDC_DIAG_ReadRegsAll(hDev, gpQSYS_CfgRegs,
                    QSYS_CFG_REGS_NUM, TRUE, FALSE);
                break;
            case MENU_RW_CFG_SPACE_READ_REG:  /* Read from a configuration
                                                 register */
                WDC_DIAG_ReadWriteReg(hDev, gpQSYS_CfgRegs,
                    QSYS_CFG_REGS_NUM, WDC_READ, TRUE);
                break;
            case MENU_RW_CFG_SPACE_WRITE_REG: /* Write to a configuration
                                                 register */
                WDC_DIAG_ReadWriteReg(hDev, gpQSYS_CfgRegs,
                    QSYS_CFG_REGS_NUM, WDC_WRITE, TRUE);
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

/* Read/write run-time registers menu */
static void MenuReadWriteRegs(WDC_DEVICE_HANDLE hDev)
{
    DWORD option;

    do {
        /* Display predefined registers information */
        printf("\n");
        printf("Qsys run-time registers:\n");
        printf("--------------------------\n");
        WDC_DIAG_RegsInfoPrint(hDev, gpQSYS_Regs, QSYS_REGS_NUM,
            WDC_DIAG_REG_PRINT_ALL, FALSE);

        printf("\n");
        printf("Read/write the Qsys run-time registers\n");
        printf("-----------------------------------------\n");
        printf("%d. Read all run-time registers defined for the device (see "
            "list above)\n", MENU_RW_REGS_READ_ALL);
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
                WDC_DIAG_ReadRegsAll(hDev, gpQSYS_Regs, QSYS_REGS_NUM,
                    FALSE, FALSE);
                break;
            case MENU_RW_REGS_READ_REG:  /* Read from a register */
                WDC_DIAG_ReadWriteReg(hDev, gpQSYS_Regs, QSYS_REGS_NUM,
                    WDC_READ, FALSE);
                break;
            case MENU_RW_REGS_WRITE_REG: /* Write to a register */
                WDC_DIAG_ReadWriteReg(hDev, gpQSYS_Regs, QSYS_REGS_NUM,
                    WDC_WRITE, FALSE);
                break;
        }
    } while (MENU_RW_REGS_EXIT != option);
}

/* -----------------------------------------------
    Interrupt handling
   ----------------------------------------------- */
/* Interrupts menu options */
enum {
    MENU_INT_ENABLE_DISABLE = 1,
    MENU_INT_EXIT = DIAG_EXIT_MENU,
};

/* Enable/disable interrupts menu */
static void MenuInterrupts(WDC_DEVICE_HANDLE hDev)
{
    DWORD option, dwIntOptions;
    BOOL fIntEnable, fIsMsi;

    dwIntOptions = WDC_GET_INT_OPTIONS(hDev);
    fIsMsi = WDC_INT_IS_MSI(dwIntOptions);
    if (dwIntOptions & INTERRUPT_LEVEL_SENSITIVE)
    {
        /* TODO: You can remove this message after you have modified the
           implementation of QSYS_IntEnable() in qsys_lib.c to correctly
           acknowledge level-sensitive interrupts (see guidelines in
           QSYS_IntEnable()). */
        printf("\n");
        printf("WARNING!!!\n");
        printf("----------\n");
        printf("Your hardware has level sensitive interrupts.\n");
        printf("Before enabling the interrupts, %s first modify the source "
           "code of QSYS_IntEnable(), in the file qsys_lib.c, to correctly"
           "acknowledge\n%s interrupts when they occur, as dictated by the "
           "hardware's specification.\n",
           fIsMsi ? "it's recommended that you" : "you must",
           fIsMsi ? "level sensitive" : "");    }

    do
    {
        fIntEnable = !QSYS_IntIsEnabled(hDev);

        printf("\n");
        printf("Interrupts\n");
        printf("-----------\n");
        printf("%d. %s interrupts\n", MENU_INT_ENABLE_DISABLE,
            fIntEnable ? "Enable" : "Disable");
        printf("%d. Exit menu\n", MENU_INT_EXIT);
        printf("\n");

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option,
            MENU_RW_ADDR_WRITE))
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
                    DWORD dwStatus = QSYS_IntEnable(hDev, DiagIntHandler);

                    if (WD_STATUS_SUCCESS == dwStatus)
                    {
                        printf("Interrupts enabled\n");
                    }
                    else
                    {
                        QSYS_ERR("Failed enabling interrupts. Error 0x%lx - "
                            "%s\n", dwStatus, Stat2Str(dwStatus));
                    }
                }
                else
                {
                    if (WD_STATUS_SUCCESS == QSYS_IntDisable(hDev))
                    {
                        printf("Interrupts disabled\n");
                    }
                    else
                    {
                        QSYS_ERR("Failed disabling interrupts: %s",
                            QSYS_GetLastErr());
                    }
                }
                break;
        }
    } while (MENU_INT_EXIT != option);
}

/* Diagnostics interrupt handler routine */
static void DiagIntHandler(WDC_DEVICE_HANDLE hDev, QSYS_INT_RESULT *pIntResult)
{
    UNUSED_VAR(hDev);

    /* TODO: You can modify this function in order to implement your own
       diagnostics interrupt handler routine.
       For example, if you select to modify the code to use interrupts to
       detect DMA transfer completion, you can call DmaTransferVerify() from
       the interrupt handler routine. */

    printf("Got interrupt number %ld\n", pIntResult->dwCounter);
    printf("Interrupt Type: %s\n",
        WDC_DIAG_IntTypeDescriptionGet(pIntResult->dwEnabledIntType));
    if (WDC_INT_IS_MSI(pIntResult->dwEnabledIntType))
        printf("Message Data: 0x%lx\n", pIntResult->dwLastMessage);
}

/* -----------------------------------------------
    Direct Memory Access (DMA)
   ----------------------------------------------- */
/* DMA menu options */
enum {
    MENU_DMA_OPEN_CLOSE = 1,
    MENU_DMA_EXIT = DIAG_EXIT_MENU
};

/* Default number of DMA transfer data packets for a DMA performance test */
#define PERFORMANCE_DMA_NUM_PKTS_DEFAULT 8192 /* 8,192 packets = 65,536 bytes */

/* Open/close polling-based DMA menu */
static void MenuDmaPolling(WDC_DEVICE_HANDLE hDev, QSYS_PDMA_STRUCT pDma)
{
    DWORD option;
    BOOL fIsDmaOpen;

    do
    {
        fIsDmaOpen = pDma->pDma ? TRUE : FALSE;

        printf("\nOpen/close Direct Memory Access (DMA)\n"
           "using polling completion method\n");
        printf("--------------------------------\n");
        printf("%d. %s DMA\n", MENU_DMA_OPEN_CLOSE,
            fIsDmaOpen ? "Close" : "Open");
        printf("%d. Exit menu\n\n", MENU_DMA_EXIT);

        if (DIAG_INPUT_FAIL ==
            DIAG_GetMenuOption(&option, MENU_DMA_OPEN_CLOSE))
        {
            continue;
        }

        switch (option)
        {
            case MENU_DMA_EXIT: /* Exit menu */
                break;

            case MENU_DMA_OPEN_CLOSE: /* Open/close DMA menu option */
                if (fIsDmaOpen)
                    DmaClose(hDev, pDma);
                else
                    DmaOpen(hDev, pDma);
                break;
        }
    } while (MENU_DMA_EXIT != option);
}

/* DMA user input menu */
static BOOL MenuDmaOpenGetInput(PDWORD pdwNumPkts, UINT32 *pu32Pattern,
    PDWORD pdwOptions, BOOL *pfUserPattern)
{
    DWORD option, dwNumPktsDefault = 0;

    /* Get DMA direction and set the DMA options accordingly */
    printf("\nSelect DMA direction:\n");
    printf("1. From device\n");
    printf("2. To device\n");
    printf("%d. Cancel\n", DIAG_EXIT_MENU);
    if ((DIAG_INPUT_SUCCESS != DIAG_GetMenuOption(&option, 2)) ||
        (DIAG_EXIT_MENU == option))
    {
        return FALSE;
    }
    *pdwOptions = (1 == option) ? DMA_FROM_DEVICE : DMA_TO_DEVICE;

    /* Get the number of packets for the DMA transfer */
    sprintf(gsInput, "\nEnter number of %d-byte DMA packets",
        QSYS_DMA_PKT_BYTES);
    if (*pdwNumPkts) /* Default number of packets provided */
    {
        dwNumPktsDefault = *pdwNumPkts;
        sprintf(gsInput, "%s, or 0 to use the default (%ld packets)",
            gsInput, dwNumPktsDefault);
    }
    if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD(pdwNumPkts, gsInput, FALSE,
        dwNumPktsDefault ? 0 : 1, 0xffffffff))
    {
        return FALSE;
    }
    if (!*pdwNumPkts)
        *pdwNumPkts = dwNumPktsDefault;

    /* Determine the data pattern type -- user-defined or automatic (serial) */
    *pfUserPattern = FALSE;
    printf("\nSelect DMA data pattern:\n");
    printf("1. Automatic serial data (0, 1, 2, ...)\n");
    printf("2. Your pattern\n");
    printf("%d. Cancel\n", DIAG_EXIT_MENU);
    if ((DIAG_INPUT_SUCCESS !=
        DIAG_GetMenuOption(&option, 2)) || (DIAG_EXIT_MENU == option))
    {
        return FALSE;
    }

    if (option == 2)
    {
        /* Get user data pattern */
        sprintf(gsInput, "\nEnter a %lu-byte DMA data pattern",
            QSYS_DMA_ITEM_BYTES);
        if (DIAG_INPUT_SUCCESS !=
            DIAG_InputUINT32(pu32Pattern, gsInput, TRUE, 0, 0))
        {
            return FALSE;
        }
        *pfUserPattern = TRUE;
    }

    printf("\n");

    return TRUE;
}

/* Verify a DMA transfer */
static void DmaTransferVerify(QSYS_PDMA_STRUCT pDma, PVOID pBuf, PVOID pBufOrig,
    BOOL fIsToDevice)
{
    DWORD i, dwNumItems;

    /* Calculate the number of UINT32 DMA transfer items */
    dwNumItems = (pDma->dwBufSize - QSYS_DMA_DONE_BYTES) / QSYS_DMA_ITEM_BYTES;

    /* Verify DMA transfer completion in the device */
    if (!QSYS_DmaTransferVerify(pDma->hDev))
    {
        QSYS_ERR("QSYS_DmaTransferVerify failed\n");
        goto Error;
    }

    if (fIsToDevice) /* Verify data for a host-to-device (read) DMA transfer */
    {
        UINT32 u32ReadData, u32ExpectedData;

        /* Compare the data in each 32-bit device DMA register to the data in
         * the parallel host DMA buffer item; (this is slow, but it works) */
        for (i = 0; i < dwNumItems; i++)
        {
            u32ReadData = QSYS_ReadReg32(pDma->hDev, AD_PCI_BAR0,
                pDma->dwTargetAddr + i * QSYS_DMA_ITEM_BYTES);
            u32ExpectedData = ((UINT32 *)(pBuf))[i];
            if (u32ExpectedData != u32ReadData)
            {
                QSYS_ERR("Host-to-device (read) DMA data mismatch: device "
                    "data = %08X, expected data (pBuf[%ld]) = %08X\n",
                    u32ReadData, i, u32ExpectedData);
                goto Error;
            }
        }

    }
    else /* Verify data for a device-to-host (write) DMA transfer */
    {
        /* Compare the provided original data (pBufOrig) to the provided read
         * data (pBuf) */
        for (i = 0; i < dwNumItems; i++)
        {
            if (((UINT32 *)(pBuf))[i] != ((UINT32 *)(pBufOrig))[i])
            {
                QSYS_ERR("Device-to-host (write) DMA data mismatch: "
                    "host data (pBuf[%ld]) = %08X, expected data "
                    "(pBufOrig[%ld]) = %08X\n", i, ((UINT32 *)(pBuf))[i], i,
                    ((UINT32 *)(pBufOrig))[i]);
                goto Error;
            }
        }
    }

    printf("DMA transfer completed and verified.\n");
    return;

Error:
    printf("DMA transfer verification failed.\n");
}

/* Open DMA and prepare the device */
static DWORD DmaOpenAndPrepare(WDC_DEVICE_HANDLE hDev, QSYS_PDMA_STRUCT pDma,
    BOOL *pfIsToDevice, PDWORD pdwNumItems, UINT32 *pu32Pattern,
    UINT32 *pu32ExpectedData, UINT32 **ppBufOrig, BOOL fRunPerfTest)
{
    DWORD dwStatus, dwOptions, dwNumPkts, dwBufItems;
    BOOL fUserData;
    UINT32 i;

    /* For a performance test, set default transfer packets number */
    if (fRunPerfTest)
        dwNumPkts = PERFORMANCE_DMA_NUM_PKTS_DEFAULT;

    /* Get user input */
    if (!MenuDmaOpenGetInput(&dwNumPkts, pu32Pattern, &dwOptions, &fUserData))
        return WD_WINDRIVER_STATUS_ERROR;

    /* Calculate the number of UINT32 DMA transfer items */
    *pdwNumItems = dwNumPkts * QSYS_DMA_PKT_NUM_ITEMS;

    /* Determine the DMA direction: host-to-device=read; device-to-host=write */
    *pfIsToDevice = dwOptions & DMA_FROM_DEVICE ? FALSE : TRUE;

    /* Qsys sample design does not support physical addresses higher than 16M */
    dwOptions |= DMA_KBUF_BELOW_16M;

    /* Open a DMA handle */
    dwStatus = QSYS_DmaOpen(hDev, pDma, dwOptions, *pdwNumItems);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        printf("\nFailed to open DMA handle. Error 0x%lx - %s\n", dwStatus,
            Stat2Str(dwStatus));
        return dwStatus;
    }

    printf("\nDMA handle was opened successfully\n");

    /* For device-to-host (write) DMA, allocate a data-verification buffer */
    if (!*pfIsToDevice)
        *ppBufOrig = (UINT32 *)malloc(*pdwNumItems * QSYS_DMA_ITEM_BYTES);

    /* Initialize DMA data */
    for (i = 0; i < *pdwNumItems ; i++)
    {
        /* Use user data, if provided; otherwise, use serial data (0, 1, ...) */
        UINT32 data = fUserData ? *pu32Pattern : i;

        if (*pfIsToDevice) /* Host-to-device (read) DMA buffer */
        {
            /* Initialize the host's DMA buffer with the data */
            ((UINT32 *)(pDma->pBuf))[i] = data;
        }
        else /* Device-to-host (write) DMA buffer */
        {
            /* Initialize the device's DMA buffer with the data */
            WDC_WriteAddr32(hDev, AD_PCI_BAR0,
                pDma->dwTargetAddr + i * QSYS_DMA_ITEM_BYTES, data);
            /* Store the data in the data-verification buffer */
            (*ppBufOrig)[i] = data;
            /* Initialize the host's DMA buffer with dummy data, to be
             * overwritten later by a device-to-host (write) DMA transfer */
            ((UINT32 *)(pDma->pBuf))[i] = 0xdeadbeaf;
        }
    }

    /* Clear the host's DMA transfer completion detection data */
    dwBufItems = pDma->dwBufSize / QSYS_DMA_ITEM_BYTES;
    for (i = *pdwNumItems; i < dwBufItems ; i++)
        ((UINT32 *)(pDma->pBuf))[i] = 0;

    /* Store the expected data that will be used to verify the DMA transfer
     * completion -- the data in the last 4 bytes (QSYS_DMA_DONE_DETECT_BYTES)
     * of the first 32 bytes (QSYS_DMA_DONE_BYTES) of the transfer data */
    if (*pfIsToDevice)
    {
        *pu32ExpectedData = ((UINT32 *)(pDma->pBuf))[QSYS_DMA_DONE_NUM_ITEMS -
            QSYS_DMA_DONE_DETECT_NUM_ITEMS];
    }
    else
    {
        *pu32ExpectedData = (*ppBufOrig)[QSYS_DMA_DONE_NUM_ITEMS -
            QSYS_DMA_DONE_DETECT_NUM_ITEMS];
    }

    /* Prepare the device for DMA transfer */
    QSYS_DmaDevicePrepare(pDma, *pfIsToDevice);

    /* If you select to modify the code to use interrupts to detect DMA
     * transfer completion, call QSYS_IntEnable() here to enable interrupts. */

    return dwStatus;
}

/* Open DMA */
static void DmaOpen(WDC_DEVICE_HANDLE hDev, QSYS_PDMA_STRUCT pDma)
{
    DWORD dwStatus, dwNumItems = 0;
    UINT32 u32Pattern, u32ExpectedData, *pBufOrig = NULL;
    BOOL fIsToDevice;

    /* Open DMA and prepare the device */
    dwStatus = DmaOpenAndPrepare(hDev, pDma, &fIsToDevice, &dwNumItems,
        &u32Pattern, &u32ExpectedData, &pBufOrig, FALSE);

    if (WD_STATUS_SUCCESS != dwStatus)
        goto Exit;

    /* Start DMA */
    printf("Start DMA transfer\n");
    QSYS_DmaStart(pDma, fIsToDevice);

    /* Poll for DMA transfer completion */
    if (QSYS_DmaPollCompletion(pDma, u32ExpectedData))
    {
        /* Verify the DMA transfer */
        DmaTransferVerify(pDma, pDma->pBuf, pBufOrig, fIsToDevice);
    }
    else
    {
        printf("DMA transfer completion polling timeout\n");
    }

Exit:
    /* Free the device-to-host data-verification buffer (if allocated) */
    if (pBufOrig)
        free(pBufOrig);
}

/* Close DMA */
static void DmaClose(WDC_DEVICE_HANDLE hDev, QSYS_PDMA_STRUCT pDma)
{
    UNUSED_VAR(hDev);

    if (!pDma)
        return;

    /* If you select to modify the code to use interrupts to detect DMA
     * transfer completion, call QSYS_IntIsEnabled() here to check if DMA
     * interrupts are currently enabled, and if so, call QSYS_IntDisable() to
     * disable the interrupts. */

    if (pDma->pDma)
    {
        /* Close the device's DMA handle */
        QSYS_DmaClose(pDma);
        printf("DMA closed\n");
    }

    BZERO(*pDma);
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
        fRegister = !QSYS_EventIsRegistered(hDev);

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
                    if (WD_STATUS_SUCCESS == QSYS_EventRegister(hDev,
                        DiagEventHandler))
                    {
                        printf("Events registered\n");
                    }
                    else
                    {
                        QSYS_ERR("Failed to register events. Last error:\n%s",
                            QSYS_GetLastErr());
                    }
                }
                else
                {
                    if (WD_STATUS_SUCCESS == QSYS_EventUnregister(hDev))
                    {
                        printf("Events unregistered\n");
                    }
                    else
                    {
                        QSYS_ERR("Failed to unregister events. "
                            "Last Error:\n%s", QSYS_GetLastErr());
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

/* -----------------------------------------------
    Performance Tests
   ----------------------------------------------- */
/* Performance tests menu options */
enum {
    MENU_PERFORMANCE_DMA = 1,
    MENU_PERFORMANCE_EXIT = DIAG_EXIT_MENU,
};

/* Performance tests menu */
static void MenuPerformance(WDC_DEVICE_HANDLE hDev, QSYS_PDMA_STRUCT pDma)
{
    DWORD option;

    do
    {
        printf("\n");
        printf("Run Performance Tests\n");
        printf("---------------------\n");
        printf("%d. Test DMA performance\n", MENU_PERFORMANCE_DMA);
        printf("%d. Exit menu\n", MENU_PERFORMANCE_EXIT);
        printf("\n");

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option,
            MENU_PERFORMANCE_DMA))
        {
            continue;
        }

        switch (option)
        {
            case MENU_PERFORMANCE_EXIT: /* Exit menu */
                break;
            case MENU_PERFORMANCE_DMA:
                PerformanceTestDma(hDev, pDma);
                break;
        }
    } while (MENU_RW_ADDR_EXIT != option);
}

/* Time value type */
#if defined(UNIX)   /* Unix */
    typedef struct timeval TIME_TYPE;
#else               /* Windows */
    typedef LARGE_INTEGER TIME_TYPE;
#endif

/* Get the current time */
static void GetCurTime(TIME_TYPE *time)
{
#if defined(UNIX)   /* Unix */
    gettimeofday(time, NULL);
#else               /* Windows */
    QueryPerformanceCounter(time);
#endif
}

/* Calculate time difference (including conversion for MByte/sec values) */
static double TimeDiff(TIME_TYPE *end, TIME_TYPE *start)
{
#if defined(UNIX)   /* Unix */
    return (end->tv_usec - start->tv_usec) +
        (end->tv_sec - start->tv_sec) * 1000000;
#else               /* Windows */
    TIME_TYPE ctr_freq;

    if (!QueryPerformanceFrequency(&ctr_freq))
        return (double)-1;

    return (double)((end->QuadPart - start->QuadPart) * 1000000 /
        ctr_freq.QuadPart);
#endif
}

/* ===============================================
    DMA Performance Test
   =============================================== */

/* DMA performance test */
static void PerformanceTestDma(WDC_DEVICE_HANDLE hDev, QSYS_PDMA_STRUCT pDma)
{
    DWORD dwStatus, dwNumItems = 0;
    UINT32 u32Pattern, u32ExpectedData, *pBufOrig = NULL;
    BOOL fIsToDevice;
    double dblAcummTime;
    TIME_TYPE IStart, IEnd;

    /* Open DMA and prepare the device */
    dwStatus = DmaOpenAndPrepare(hDev, pDma, &fIsToDevice, &dwNumItems,
        &u32Pattern, &u32ExpectedData, &pBufOrig, TRUE);

    if (WD_STATUS_SUCCESS != dwStatus)
        goto Exit;

    /* Start DMA */
    printf("Start DMA transfer\n");
    /* Log the start-of-transfer time */
    GetCurTime(&IStart);
    /* Start the DMA transfer */
    QSYS_DmaStart(pDma, fIsToDevice);

    /* Poll for DMA transfer completion */
    if (QSYS_DmaPollCompletion(pDma, u32ExpectedData))
    {
        double dblMBps = 0;

        /* Log the end-of-transfer time */
        GetCurTime(&IEnd);
        /* Get the DMA transfer time */
        dblAcummTime = TimeDiff(&IEnd, &IStart);
        /* Calculate the DMA performance */
        dblMBps = (dwNumItems * QSYS_DMA_ITEM_BYTES) / dblAcummTime;
        printf("DMA %s transfer time: %f MBytes per second\n",
            fIsToDevice ? "to device" : "from device", dblMBps);

        /* Verify the DMA transfer */
        DmaTransferVerify(pDma, pDma->pBuf, pBufOrig, fIsToDevice);
    }
    else
    {
        printf("DMA transfer completion polling timeout\n");
    }

Exit:
    if (pDma->pDma && WD_OPERATION_ALREADY_DONE != dwStatus)
    {
        /* Close the device's DMA handle */
        QSYS_DmaClose(pDma);
        printf("DMA closed\n");
    }

    /* Free the device-to-host data-verification buffer (if allocated) */
    if (pBufOrig)
        free(pBufOrig);
}

