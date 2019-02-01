/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

/****************************************************************************
*  File: bmd_diag.c
*
*  Sample user-mode diagnostics application for accessing Xilinx PCI Express
*  cards with BMD design, using the WinDriver WDC API.
*  The sample was tested with Xilinx's Virtex and Spartan development kits.
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
#include "bmd_lib.h"

/*************************************************************
  General definitions
 *************************************************************/
/* Error messages display */
#define BMD_ERR printf

/*************************************************************
  Global variables
 *************************************************************/
/* User's input command */
static CHAR gsInput[256];

/* --------------------------------------------------
    BMD configuration registers information
   -------------------------------------------------- */
/* Configuration registers information array */
static const WDC_REG gBMD_CfgRegs[] = {
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

#define BMD_CFG_REGS_NUM (sizeof(gBMD_CfgRegs)/sizeof(WDC_REG))

/* TODO: For read-only or write-only registers, change the direction field of
 *       the relevant registers in gBMD_CfgRegs to WDC_READ or WDC_WRITE. */
/* NOTE: You can define additional configuration registers in gBMD_CfgRegs. */
    const WDC_REG *gpBMD_CfgRegs = gBMD_CfgRegs;

/* -----------------------------------------------
    BMD run-time registers information
   ----------------------------------------------- */
/* Run-time registers information array */
static const WDC_REG gBMD_Regs[] = {
    { BMD_SPACE, BMD_DSCR_OFFSET, WDC_SIZE_32, WDC_READ_WRITE, "DSCR",
        "Device Control Status Register" },
    { BMD_SPACE, BMD_DDMACR_OFFSET, WDC_SIZE_32, WDC_READ_WRITE,
        "DDMACR", "Device DMA Control Status Register" },
    { BMD_SPACE, BMD_WDMATLPA_OFFSET, WDC_SIZE_32, WDC_READ_WRITE,
        "WDMATLPA", "Write DMA TLP Address" },
    { BMD_SPACE, BMD_WDMATLPS_OFFSET, WDC_SIZE_32, WDC_READ_WRITE,
        "WDMATLPS", "Write DMA TLP Size" },
    { BMD_SPACE, BMD_WDMATLPC_OFFSET, WDC_SIZE_32, WDC_READ_WRITE,
        "WDMATLPC", "Write DMA TLP Count" },
    { BMD_SPACE, BMD_WDMATLPP_OFFSET, WDC_SIZE_32, WDC_READ_WRITE,
        "WDMATLPP", "Write DMA Data Pattern" },
    { BMD_SPACE, BMD_RDMATLPP_OFFSET, WDC_SIZE_32, WDC_READ_WRITE,
        "RDMATLPP", "Read DMA Expected Data Pattern" },
    { BMD_SPACE, BMD_RDMATLPA_OFFSET, WDC_SIZE_32, WDC_READ_WRITE,
        "RDMATLPA", "Read DMA TLP Address" },
    { BMD_SPACE, BMD_RDMATLPS_OFFSET, WDC_SIZE_32, WDC_READ_WRITE,
        "RDMATLPS", "Read DMA TLP Size" },
    { BMD_SPACE, BMD_RDMATLPC_OFFSET, WDC_SIZE_32, WDC_READ_WRITE,
        "RDMATLPC", "Read DMA TLP Count" },
    { BMD_SPACE, BMD_WDMAPERF_OFFSET, WDC_SIZE_32, WDC_READ, "WDMAPERF",
        "Write DMA Performance" },
    { BMD_SPACE, BMD_RDMAPERF_OFFSET, WDC_SIZE_32, WDC_READ, "RDMAPERF",
        "Read DMA Performance" },
    { BMD_SPACE, BMD_RDMASTAT_OFFSET, WDC_SIZE_32, WDC_READ, "RDMASTAT",
        "Read DMA Status" },
    { BMD_SPACE, BMD_NRDCOMP_OFFSET, WDC_SIZE_32, WDC_READ, "NRDCOMP",
        "Number of Read Completion w/ Data" },
    { BMD_SPACE, BMD_RCOMPDSIZE_OFFSET, WDC_SIZE_32, WDC_READ,
        "RCOMPDSIZE", "Read Completion Data Size" },
    { BMD_SPACE, BMD_DLWSTAT_OFFSET, WDC_SIZE_32, WDC_READ, "DLWSTAT",
        "Device Link Width Status" },
    { BMD_SPACE, BMD_DLTRSSTAT_OFFSET, WDC_SIZE_32, WDC_READ,
        "DLTRSSTAT", "Device Link Transaction Size Status" },
    { BMD_SPACE, BMD_DMISCCONT_OFFSET, WDC_SIZE_32, WDC_READ_WRITE,
        "DMISCCONT", "Device Miscellaneous Control" }
    };

    const WDC_REG *gpBMD_Regs = gBMD_Regs;

#define BMD_REGS_NUM (sizeof(gBMD_Regs)/sizeof(gBMD_Regs[0]))

typedef struct {
    BMD_DMA_HANDLE hDma;
    PVOID pBuf;
} DMA_STRUCT, *PDMA_STRUCT;

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
static void DeviceClose(WDC_DEVICE_HANDLE hDev, PDMA_STRUCT pDma);

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
    Plug-and-play and power management events
   ----------------------------------------------- */
static void MenuEvents(WDC_DEVICE_HANDLE hDev);
static void DiagEventHandler(WDC_DEVICE_HANDLE hDev, DWORD dwAction);

/* -----------------------------------------------
    Direct Memory Access (DMA)
   ---------------------------------------------- */
static void MenuDmaPolling(WDC_DEVICE_HANDLE hDev, PDMA_STRUCT pDma);
static void MenuDmaInterrupts(WDC_DEVICE_HANDLE hDev, PDMA_STRUCT pDma);
static void DmaOpen(WDC_DEVICE_HANDLE hDev, PDMA_STRUCT pDma, BOOL fPolling);
static void DmaClose(WDC_DEVICE_HANDLE hDev, PDMA_STRUCT pDma);
static void DiagDmaIntHandler(WDC_DEVICE_HANDLE hDev,
    BMD_INT_RESULT *pIntResult);
static void DmaTransferVerify(WDC_DEVICE_HANDLE hdev, PVOID pBuf,
    DWORD dwNumItems, UINT32 u32Pattern, BOOL fIsRead);

/*************************************************************
  Functions implementation
 *************************************************************/
int main(void)
{
    WDC_DEVICE_HANDLE hDev = NULL;
    DWORD dwStatus;

    printf("\n");
    printf("BMD diagnostic utility.\n");
    printf("Application accesses hardware using " WD_PROD_NAME ".\n");

    /* Initialize the BMD library */
    dwStatus = BMD_LibInit();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        BMD_ERR("bmd_diag: Failed to initialize the BMD library: %s",
            BMD_GetLastErr());
        return dwStatus;
    }

    /* Find and open a BMD device (by default ID) */
    hDev = DeviceFindAndOpen(BMD_DEFAULT_VENDOR_ID, BMD_DEFAULT_DEVICE_ID);

    /* Display main diagnostics menu for communicating with the device */
    MenuMain(&hDev);

    /* Perform necessary cleanup before exiting the program: */
    /* Close the device handle */
    if (hDev)
        DeviceClose(hDev, NULL);

    /* Uninitialize libraries */
    dwStatus = BMD_LibUninit();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        BMD_ERR("bmd_diag: Failed to uninitialize the BMD library: %s",
            BMD_GetLastErr());
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
    MENU_MAIN_DMA_INTERRUPTS,
    MENU_MAIN_EVENTS,
    MENU_MAIN_EXIT = DIAG_EXIT_MENU
};

/* Main diagnostics menu */
static void MenuMain(WDC_DEVICE_HANDLE *phDev)
{
    DWORD option;
    DMA_STRUCT dma;

    BZERO(dma);

    do
    {
        printf("\n");
        printf("BMD main menu\n");
        printf("--------------\n");
        printf("%d. Scan PCI bus\n", MENU_MAIN_SCAN_PCI_BUS);
        printf("%d. Find and open a BMD device\n", MENU_MAIN_FIND_AND_OPEN);
        if (*phDev)
        {
            printf("%d. Read/write memory and I/O addresses on the device\n",
                MENU_MAIN_RW_ADDR);
            printf("%d. Read/write the device's configuration space\n",
                MENU_MAIN_RW_CFG_SPACE);
            printf("%d. Read/write the run-time registers\n",
                MENU_MAIN_RW_REGS);
            printf("%d. Open/close Direct Memory Access (DMA) using "
                "polling completion method\n", MENU_MAIN_DMA_POLLING);
            printf("%d. Open/close Direct Memory Access (DMA) using "
                "interrupt completion method\n", MENU_MAIN_DMA_INTERRUPTS);
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
            case MENU_MAIN_FIND_AND_OPEN: /* Find and open a BMD device */
                if (*phDev)
            DeviceClose(*phDev, &dma);
                *phDev = DeviceFindAndOpen(0, 0);
                break;
            case MENU_MAIN_RW_ADDR: /* Read/write memory and I/O addresses */
                MenuReadWriteAddr(*phDev);
                break;
            case MENU_MAIN_RW_CFG_SPACE: /* Read/write the configuration space
                                          */
                MenuReadWriteCfgSpace(*phDev);
                break;
            case MENU_MAIN_RW_REGS: /* Read/write the run-time registers */
                MenuReadWriteRegs(*phDev);
                break;
            case MENU_MAIN_DMA_POLLING: /* Polling-based DMA */
                MenuDmaPolling(*phDev, &dma);
                break;
            case MENU_MAIN_DMA_INTERRUPTS: /* Interrupt-based DMA */
                MenuDmaInterrupts(*phDev, &dma);
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
/* Find and open a BMD device */
static WDC_DEVICE_HANDLE DeviceFindAndOpen(DWORD dwVendorId, DWORD dwDeviceId)
{
    WD_PCI_SLOT slot;

    /* Find device */
    if (!DeviceFind(dwVendorId, dwDeviceId, &slot))
        return NULL;

    /* Open a device handle */
    return DeviceOpen(&slot);
}

/* Find a BMD device */
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
        BMD_ERR("DeviceFind: Failed scanning the PCI bus.\n"
            "Error: 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        return FALSE;
    }

    dwNumDevices = scanResult.dwNumDevices;
    if (!dwNumDevices)
    {
        BMD_ERR("No matching device was found for search criteria "
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

/* Open a handle to a BMD device */
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
        BMD_ERR("DeviceOpen: Failed retrieving the device's resources "
            "information.\nError 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        return NULL;
    }

    /* NOTE: You can modify the device's resources information here, if
       necessary (mainly the deviceInfo.Card.Items array or the items number -
       deviceInfo.Card.dwItems) in order to register only some of the resources
       or register only a portion of a specific address space, for example. */

    /* Open a handle to the device */
    hDev = BMD_DeviceOpen(&deviceInfo, NULL);
    if (!hDev)
    {
        BMD_ERR("DeviceOpen: Failed opening a handle to the device: %s",
            BMD_GetLastErr());
        return NULL;
    }

    return hDev;
}

/* Close a handle to a BMD device */
static void DeviceClose(WDC_DEVICE_HANDLE hDev, PDMA_STRUCT pDma)
{
    /* Validate the WDC device handle */
    if (!hDev)
        return;

     /* Close the DMA handle (if open) */
     if (pDma)
         DmaClose(hDev, pDma);

    /* Close the WDC device handle */
    if (!BMD_DeviceClose(hDev))
    {
        BMD_ERR("DeviceClose: Failed closing BMD device: %s",
            BMD_GetLastErr());
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
        DWORD dwNumAddrSpaces = BMD_GetNumAddrSpaces(hDev);

        /* Find the first active address space */
        for (dwAddrSpace = 0; dwAddrSpace < dwNumAddrSpaces; dwAddrSpace++)
        {
            if (WDC_AddrSpaceIsActive(hDev, dwAddrSpace))
                break;
        }

        /* Sanity check */
        if (dwAddrSpace == dwNumAddrSpaces)
        {
            BMD_ERR("MenuReadWriteAddr: Error - no active address spaces "
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
    DWORD dwNumAddrSpaces = BMD_GetNumAddrSpaces(hDev);

    printf("\n");
    printf("Select an active address space:\n");
    printf("-------------------------------\n");

    for (dwAddrSpace = 0; dwAddrSpace < dwNumAddrSpaces; dwAddrSpace++)
    {
        BMD_ADDR_SPACE_INFO addrSpaceInfo;

        BZERO(addrSpaceInfo);
        addrSpaceInfo.dwAddrSpace = dwAddrSpace;
        if (!BMD_GetAddrSpaceInfo(hDev, &addrSpaceInfo))
        {
            BMD_ERR("SetAddrSpace: Error - Failed to get address space "
                "information: %s", BMD_GetLastErr());
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
        WDC_DIAG_RegsInfoPrint(hDev, gBMD_CfgRegs, BMD_CFG_REGS_NUM,
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
            BMD_CFG_REGS_NUM ? MENU_RW_CFG_SPACE_WRITE_REG :
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
                WDC_DIAG_ReadRegsAll(hDev, gBMD_CfgRegs, BMD_CFG_REGS_NUM,
                    TRUE, FALSE);
                break;
            case MENU_RW_CFG_SPACE_READ_REG:  /* Read from a configuration
                                                 register */
                WDC_DIAG_ReadWriteReg(hDev, gBMD_CfgRegs, BMD_CFG_REGS_NUM,
                    WDC_READ, TRUE);
                break;
            case MENU_RW_CFG_SPACE_WRITE_REG: /* Write to a configuration
                                                 register */
                WDC_DIAG_ReadWriteReg(hDev, gBMD_CfgRegs, BMD_CFG_REGS_NUM,
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

/* Read/write run-time registers menu */
static void MenuReadWriteRegs(WDC_DEVICE_HANDLE hDev)
{
    DWORD option;

    do {
        /* Display predefined registers information */
        printf("\n");
        printf("BMD run-time registers:\n");
        printf("--------------------------\n");
        WDC_DIAG_RegsInfoPrint(hDev, gBMD_Regs, BMD_REGS_NUM,
            WDC_DIAG_REG_PRINT_ALL, FALSE);

        printf("\n");
        printf("Read/write the BMD run-time registers\n");
        printf("-----------------------------------------\n");
        printf("%d. Read all run-time registers defined for the device "
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
                WDC_DIAG_ReadRegsAll(hDev, gBMD_Regs, BMD_REGS_NUM, FALSE,
                    FALSE);
                break;
            case MENU_RW_REGS_READ_REG:  /* Read from a register */
                WDC_DIAG_ReadWriteReg(hDev, gBMD_Regs, BMD_REGS_NUM,
                    WDC_READ, FALSE);
                break;
            case MENU_RW_REGS_WRITE_REG: /* Write to a register */
                WDC_DIAG_ReadWriteReg(hDev, gBMD_Regs, BMD_REGS_NUM,
                    WDC_WRITE, FALSE);
                break;
        }
    } while (MENU_RW_REGS_EXIT != option);
}

/* -----------------------------------------------
    Direct Memory Access (DMA)
   ----------------------------------------------- */
/* DMA menu options */
enum {
    MENU_DMA_OPEN_CLOSE = 1,
    MENU_DMA_EXIT = DIAG_EXIT_MENU
};

/* Diagnostics DMA interrupt handler routine */
static void DiagDmaIntHandler(WDC_DEVICE_HANDLE hDev,
    BMD_INT_RESULT *pIntResult)
{
    printf("\n###\nDMA %s based interrupt, received #%ld\n",
        pIntResult->fIsMessageBased ? "message" : "line",
        pIntResult->dwCounter);
    if (pIntResult->fIsMessageBased)
        printf("Message data 0x%lx\n", pIntResult->dwLastMessage);
    printf("###\n\n");
    DmaTransferVerify(hDev, pIntResult->pBuf, pIntResult->dwBufNumItems,
        pIntResult->u32Pattern, pIntResult->fIsRead);
}

/* Open/close DMA menu */
static void MenuDma(WDC_DEVICE_HANDLE hDev, PDMA_STRUCT pDma, BOOL fPolling)
{
    DWORD option;
    BOOL fIsDmaOpen;

    do
    {
        fIsDmaOpen = pDma->hDma ? TRUE : FALSE;

        printf("\nOpen/close Direct Memory Access (DMA)\n"
            "using %s completion method\n", fPolling ? "polling" : "interrupt");
        printf("----------------------------------\n");
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
                    DmaOpen(hDev, pDma, fPolling);
                break;
        }
    } while (MENU_DMA_EXIT != option);
}

/* Open/close polling-based DMA menu */
static void MenuDmaPolling(WDC_DEVICE_HANDLE hDev, PDMA_STRUCT pDma)
{
    MenuDma(hDev, pDma, TRUE);
}

/* Open/close interrupt-based DMA menu */
static void MenuDmaInterrupts(WDC_DEVICE_HANDLE hDev, PDMA_STRUCT pDma)
{
    MenuDma(hDev, pDma, FALSE);
}

/* DMA user input menu */
static BOOL MenuDmaOpenGetInput(PWORD pwNumTLPs, UINT32 *pu32Pattern,
    PDWORD pdwOptions)
{
    DWORD option;

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

    /* Get number of Transaction Layer Packets (TLPs) for the DMA transfer */
    if (DIAG_INPUT_SUCCESS != DIAG_InputWORD(pwNumTLPs,
        "\nEnter DMA TLP count", FALSE, 1, 65535))
    {
        return FALSE;
    }

    /* Get data pattern */
    if (DIAG_INPUT_SUCCESS != DIAG_InputUINT32(pu32Pattern,
        "\nEnter DMA data pattern", TRUE, 0, 0))
    {
        return FALSE;
    }

    printf("\n");

    return TRUE;
}

/* Verify a DMA transfer */
static void DmaTransferVerify(WDC_DEVICE_HANDLE hDev, PVOID pBuf,
    DWORD dwNumItems, UINT32 u32Pattern, BOOL fIsRead)
{
    DWORD i;

    if (fIsRead) /* Verify a host-to-device (read) DMA transfer */
    {
        /* Verify the transfer by reading the device's HW error bit */
        if (!BMD_DmaIsReadSucceed(hDev))
            goto Error;
    }
    else /* Verify a device-to-host (write) DMA transfer */
    {
        /* Verify the transfer by comparing the data in each element of the
         * host's DMA buffer (pBuf) to the original data pattern (u32Pattern) */
        for (i = 0; i < dwNumItems; i++)
        {
            if (((UINT32 *)(pBuf))[i] != u32Pattern)
            {
                BMD_ERR("Device-to-host (write) DMA data mismatch: host data "
                    "(pBuf[%ld]) = %08X, original data pattern (u32Pattern) = "
                    "%08X\n", i, ((UINT32 *)(pBuf))[i], u32Pattern);
                goto Error;
            }
        }

    }

    printf("DMA transfer completed and verified.\n");
    return;

Error:
    printf("DMA transfer verification failed.\n");
}

/* Open DMA */
static void DmaOpen(WDC_DEVICE_HANDLE hDev, PDMA_STRUCT pDma, BOOL fPolling)
{
    DWORD dwStatus, dwOptions, dwNumItems, i;
    WORD wTLPNumItems, wNumTLPs;
    UINT32 u32Pattern;
    BOOL fIsRead, fEnable64bit;
    BYTE bTrafficClass;

    /* Get user input */
    if (!MenuDmaOpenGetInput(&wNumTLPs, &u32Pattern, &dwOptions))
        return;

    /* Determine the DMA direction: host-to-device=read; device-to-host=write */
    fIsRead = dwOptions & DMA_FROM_DEVICE ? FALSE : TRUE;

    /* The BMD reference design does not support Scatter/Gather DMA, so we use
     * contiguous buffer DMA */
    dwOptions |= DMA_KERNEL_BUFFER_ALLOC;

    /* Calculate the DMA TLP (payload packet) size, in units of UINT32 */
    wTLPNumItems = BMD_DmaGetMaxPacketSize(hDev, fIsRead) / sizeof(UINT32);

    /* Calculate the total DMA transfer size, in units of UINT32 */
    dwNumItems = (DWORD)wNumTLPs * (DWORD)wTLPNumItems;

    /* Open a DMA handle */
    dwStatus = BMD_DmaOpen(hDev, &pDma->pBuf, dwOptions,
        dwNumItems * sizeof(UINT32), &pDma->hDma);

    if (WD_STATUS_SUCCESS != dwStatus)
    {
        printf("\nFailed to open DMA handle. Error 0x%lx - %s\n", dwStatus,
            Stat2Str(dwStatus));
        return;
    }

    printf("\nDMA handle was opened successfully (handle 0x%p)\n", pDma->hDma);
    printf("DMA TLP size, in units of UINT32: 0x%hx\n", wTLPNumItems);

    /* Initialize the DMA buffer using the user-defined pattern */
    for (i = 0; i < dwNumItems; i++)
    {
        if (fIsRead) /* Host-to-device (read) DMA buffer */
        {
            /* Initialize the host's DMA buffer using the given data pattern */
            ((UINT32 *)(pDma->pBuf))[i] = u32Pattern;
        }
        else /* Device-to-host (write) DMA buffer */
        {
            /* Initialize the host's DMA buffer with dummy data, to be
             * overwritten later by a device-to-host (write) DMA transfer */
            ((UINT32 *)(pDma->pBuf))[i] = 0xdeadbeaf;
        }
    }

    /* Prepare the device registers for DMA transfer */
    fEnable64bit = FALSE;
    bTrafficClass = 0;
    BMD_DmaDevicePrepare(pDma->hDma, fIsRead, wTLPNumItems, wNumTLPs,
        u32Pattern, fEnable64bit, bTrafficClass);

    if (!fPolling) /* Interrupt-based DMA */
    {
        /* Enable DMA interrupts */
        BMD_DmaIntEnable(hDev, fIsRead);

        if (!BMD_IntIsEnabled(hDev))
        {
            dwStatus = BMD_IntEnable(hDev, DiagDmaIntHandler);

            if (WD_STATUS_SUCCESS != dwStatus)
            {
                printf("\nFailed enabling DMA interrupts. Error 0x%lx - %s\n",
                    dwStatus, Stat2Str(dwStatus));
                goto Error;
            }

            printf("\nDMA interrupts enabled\n");
        }
    }
    else /* Polling-based DMA */
    {
        /* Disable DMA interrupts */
        BMD_DmaIntDisable(hDev, fIsRead);
    }

    /* Start DMA transfer */
    printf("Start DMA transfer\n");
    BMD_DmaStart(pDma->hDma, fIsRead);

    /* Poll for DMA completion (when using polling-based DMA) */
    if (fPolling)
    {
        if (BMD_DmaPollCompletion(pDma->hDma, fIsRead))
        {
            /* Verify the DMA transfer */
            DmaTransferVerify(hDev, pDma->pBuf, dwNumItems, u32Pattern,
                fIsRead);
        }
        else
            printf("DMA transfer completion polling timeout\n");
    }

    return;

Error:
    DmaClose(hDev, pDma);
}

/* Close DMA */
static void DmaClose(WDC_DEVICE_HANDLE hDev, PDMA_STRUCT pDma)
{
    DWORD dwStatus;

    if (!pDma)
        return;

    /* Disable DMA interrupts */
    if (BMD_IntIsEnabled(hDev))
    {
        dwStatus = BMD_IntDisable(hDev);
        printf("DMA interrupts disable%s\n",
            (WD_STATUS_SUCCESS == dwStatus) ? "d" : " failed");
    }

    if (pDma->hDma)
    {
        /* Close the device's DMA handle */
        BMD_DmaClose(pDma->hDma);
        printf("DMA closed (handle 0x%p)\n", pDma->hDma);
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
        fRegister = !BMD_EventIsRegistered(hDev);

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
                        BMD_EventRegister(hDev, DiagEventHandler))
                    {
                        printf("Events registered\n");
                    }
                    else
                    {
                        BMD_ERR("Failed to register events. Last error:\n%s",
                            BMD_GetLastErr());
                    }
                }
                else
                {
                    if (WD_STATUS_SUCCESS == BMD_EventUnregister(hDev))
                        printf("Events unregistered\n");
                    else
                    {
                        BMD_ERR("Failed to unregister events. Last Error:\n%s",
                            BMD_GetLastErr());
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

