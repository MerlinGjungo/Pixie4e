/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

/****************************************************************************
*  File: lscdma_diag.c
*
*  Sample user-mode diagnostics application for accessing Lattice PCI Express
*  cards with SGDMA support, using the WinDriver WDC API.
*
*  Note: This code sample is provided AS-IS and as a guiding sample only.
*****************************************************************************/

#include <stdio.h>
#include "wdc_defs.h"
#include "wdc_lib.h"
#include "utils.h"
#include "status_strings.h"
#include "samples/shared/diag_lib.h"
#include "samples/shared/wdc_diag_lib.h"
#include "pci_regs.h"
#include "lscdma_lib.h"

/*************************************************************
  General definitions
 *************************************************************/
/* Error messages display */
#define LSCDMA_ERR printf

/*************************************************************
  Global variables
 *************************************************************/
/* User's input command */
static CHAR gsInput[256];

/* --------------------------------------------------
    LSCDMA configuration registers information
   -------------------------------------------------- */
/* Configuration registers information array */
const WDC_REG gLSCDMA_CfgRegs[] = {
    { WDC_AD_CFG_SPACE, PCI_VID, WDC_SIZE_16, WDC_READ_WRITE, "VID", "Vendor ID" },
    { WDC_AD_CFG_SPACE, PCI_DID, WDC_SIZE_16, WDC_READ_WRITE, "DID", "Device ID" },
    { WDC_AD_CFG_SPACE, PCI_CR, WDC_SIZE_16, WDC_READ_WRITE, "CMD", "Command" },
    { WDC_AD_CFG_SPACE, PCI_SR, WDC_SIZE_16, WDC_READ_WRITE, "STS", "Status" },
    { WDC_AD_CFG_SPACE, PCI_REV, WDC_SIZE_32, WDC_READ_WRITE, "RID_CLCD", "Revision ID & Class Code" },
    { WDC_AD_CFG_SPACE, PCI_CCSC, WDC_SIZE_8, WDC_READ_WRITE, "SCC", "Sub Class Code" },
    { WDC_AD_CFG_SPACE, PCI_CCBC, WDC_SIZE_8, WDC_READ_WRITE, "BCC", "Base Class Code" },
    { WDC_AD_CFG_SPACE, PCI_CLSR, WDC_SIZE_8, WDC_READ_WRITE, "CALN", "Cache Line Size" },
    { WDC_AD_CFG_SPACE, PCI_LTR, WDC_SIZE_8, WDC_READ_WRITE, "LAT", "Latency Timer" },
    { WDC_AD_CFG_SPACE, PCI_HDR, WDC_SIZE_8, WDC_READ_WRITE, "HDR", "Header Type" },
    { WDC_AD_CFG_SPACE, PCI_BISTR, WDC_SIZE_8, WDC_READ_WRITE, "BIST", "Built-in Self Test" },
    { WDC_AD_CFG_SPACE, PCI_BAR0, WDC_SIZE_32, WDC_READ_WRITE, "BADDR0", "Base Address 0" },
    { WDC_AD_CFG_SPACE, PCI_BAR1, WDC_SIZE_32, WDC_READ_WRITE, "BADDR1", "Base Address 1" },
    { WDC_AD_CFG_SPACE, PCI_BAR2, WDC_SIZE_32, WDC_READ_WRITE, "BADDR2", "Base Address 2" },
    { WDC_AD_CFG_SPACE, PCI_BAR3, WDC_SIZE_32, WDC_READ_WRITE, "BADDR3", "Base Address 3" },
    { WDC_AD_CFG_SPACE, PCI_BAR4, WDC_SIZE_32, WDC_READ_WRITE, "BADDR4", "Base Address 4" },
    { WDC_AD_CFG_SPACE, PCI_BAR5, WDC_SIZE_32, WDC_READ_WRITE, "BADDR5", "Base Address 5" },
    { WDC_AD_CFG_SPACE, PCI_CIS, WDC_SIZE_32, WDC_READ_WRITE, "CIS", "CardBus CIS Pointer" },
    { WDC_AD_CFG_SPACE, PCI_SVID, WDC_SIZE_16, WDC_READ_WRITE, "SVID", "Sub-system Vendor ID" },
    { WDC_AD_CFG_SPACE, PCI_SDID, WDC_SIZE_16, WDC_READ_WRITE, "SDID", "Sub-system Device ID" },
    { WDC_AD_CFG_SPACE, PCI_EROM, WDC_SIZE_32, WDC_READ_WRITE, "EROM", "Expansion ROM Base Address" },
    { WDC_AD_CFG_SPACE, PCI_CAP, WDC_SIZE_8, WDC_READ_WRITE, "NEW_CAP", "New Capabilities Pointer" },
    { WDC_AD_CFG_SPACE, PCI_ILR, WDC_SIZE_32, WDC_READ_WRITE, "INTLN", "Interrupt Line" },
    { WDC_AD_CFG_SPACE, PCI_IPR, WDC_SIZE_32, WDC_READ_WRITE, "INTPIN", "Interrupt Pin" },
    { WDC_AD_CFG_SPACE, PCI_MGR, WDC_SIZE_32, WDC_READ_WRITE, "MINGNT", "Minimum Required Burst Period" },
    { WDC_AD_CFG_SPACE, PCI_MLR, WDC_SIZE_32, WDC_READ_WRITE, "MAXLAT", "Maximum Latency" },
    };
#define LSCDMA_CFG_REGS_NUM (sizeof(gLSCDMA_CfgRegs) / sizeof(WDC_REG))
/* NOTE: You can define additional configuration registers in gLSCDMA_CfgRegs. */

/* -----------------------------------------------
    LSCDMA run-time registers information
   ----------------------------------------------- */
/* Run-time registers information array */
const WDC_REG gLSCDMA_Regs[] =
{
    /* GPIO registers */
    { AD_PCI_BAR0, GPIO_ID_REG_OFFSET, WDC_SIZE_32, WDC_READ, "GPIO_ID_REG", "GPIO ID register" },
    { AD_PCI_BAR0, GPIO_SCRACH_OFFSET, WDC_SIZE_32, WDC_READ_WRITE, "SCRACH", "Scratch pad" },
    { AD_PCI_BAR0, GPIO_LED14SEG_OFFSET, WDC_SIZE_16, WDC_READ_WRITE, "LED14SEG", "14 segment LED" },
    { AD_PCI_BAR0, GPIO_DIPSW_OFFSET, WDC_SIZE_16, WDC_READ_WRITE, "DIPSW", "DIP switch value" },
    { AD_PCI_BAR0, GPIO_CNTRCTRL_OFFSET, WDC_SIZE_32, WDC_READ_WRITE, "CNTRCTRL", "Generic down counter control" },
    { AD_PCI_BAR0, GPIO_CNTRVAL_OFFSET, WDC_SIZE_32, WDC_READ_WRITE, "CNTRVAL", "Counter value" },
    { AD_PCI_BAR0, GPIO_CNTRRELOAD_OFFSET, WDC_SIZE_32, WDC_READ_WRITE, "CNTRRELOAD", "Counter reload value" },
    { AD_PCI_BAR0, GPIO_DMAREQ_OFFSET, WDC_SIZE_32, WDC_READ_WRITE, "DMAREQ", "DMA Request & DMA Ack (per channel)" },
    { AD_PCI_BAR0, GPIO_WR_CNTR_OFFSET, WDC_SIZE_32, WDC_READ_WRITE, "WR_CNTR", "DMA Write Counter" },
    { AD_PCI_BAR0, GPIO_RD_CNTR_OFFSET, WDC_SIZE_32, WDC_READ_WRITE, "RD_CNTR", "DMA Read Counter" },

    /* Interrupt controller registers */
    { AD_PCI_BAR0, INTCTL_ID_OFFSET, WDC_SIZE_32, WDC_READ_WRITE, "INTCTL_ID", "Interrupt controller ID" },
    { AD_PCI_BAR0, INTCTL_CTRL_OFFSET, WDC_SIZE_32, WDC_READ_WRITE, "INTCTL_CTRL", "Interrupt control register" },
    { AD_PCI_BAR0, INTCTL_STATUS_OFFSET, WDC_SIZE_32, WDC_READ_WRITE, "INTCTL_STATUS", "Interrupt status register" },
    { AD_PCI_BAR0, INTCTL_ENABLE_OFFSET, WDC_SIZE_32, WDC_READ_WRITE, "INTCTL_ENABLE", "Interrupt mask for all sources" },

    /* SGDMA registers */
    { AD_PCI_BAR0, SGDMA_IPID_OFFSET, WDC_SIZE_32, WDC_READ, "SGDMA_IPID", "SGDMA IP identification register" },
    { AD_PCI_BAR0, SGDMA_IPVER_OFFSET, WDC_SIZE_32, WDC_READ, "SGDMA_IPVER", "SGDMA IP version register" },
    { AD_PCI_BAR0, SGDMA_GCONTROL_OFFSET, WDC_SIZE_32, WDC_READ_WRITE, "SGDMA_GCONTROL", "SGDMA global control register" },
    { AD_PCI_BAR0, SGDMA_GSTATUS_OFFSET, WDC_SIZE_32, WDC_READ_WRITE, "SGDMA_GSTATUS", "SGDMA global status register" },
    { AD_PCI_BAR0, SGDMA_GEVENT_OFFSET, WDC_SIZE_32, WDC_READ_WRITE, "SGDMA_GEVENT", "SGDMA global channel event register and mask" },
    { AD_PCI_BAR0, SGDMA_GERROR_OFFSET, WDC_SIZE_32, WDC_READ_WRITE, "SGDMA_GERROR", "SGDMA global channel error register and mask" },
    { AD_PCI_BAR0, SGDMA_GARBITER_OFFSET, WDC_SIZE_32, WDC_READ_WRITE, "SGDMA_GARBITER", "SGDMA global arbiter control register" },
    { AD_PCI_BAR0, SGDMA_GAUX_OFFSET, WDC_SIZE_32, WDC_READ_WRITE, "SGDMA_GAUX", "SGDMA auxiliary inputs and outputs" },
};
#define LSCDMA_REGS_NUM (sizeof(gLSCDMA_Regs) / sizeof(WDC_REG))
/* NOTE: You can define additional run-time registers in gLSCDMA_CfgRegs. */

/*************************************************************
  Static functions prototypes
 *************************************************************/
/* -----------------------------------------------
    Main diagnostics menu
   ----------------------------------------------- */
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
    Read/write the run-time registers
   ----------------------------------------------- */
static void MenuReadWriteRegs(WDC_DEVICE_HANDLE hDev);

/* -----------------------------------------------
    Interrupt handling
   ----------------------------------------------- */
static void MenuInterrupts(WDC_DEVICE_HANDLE hDev);
static void DiagIntHandler(WDC_DEVICE_HANDLE hDev, LSCDMA_INT_RESULT *pIntResult);

/* -----------------------------------------------
    Direct Memory Access (DMA)
   ---------------------------------------------- */
static void MenuDma(WDC_DEVICE_HANDLE hDev);

/* ----------------------------------------------------
    Plug-and-play and power management events handling
   ---------------------------------------------------- */
static void MenuEvents(WDC_DEVICE_HANDLE hDev);
static void DiagEventHandler(WDC_DEVICE_HANDLE hDev, DWORD dwAction);

/*************************************************************
  Functions implementation
 *************************************************************/
int main(void)
{
    WDC_DEVICE_HANDLE hDev = NULL;
    DWORD dwStatus;

    printf("\n");
    printf("LSCDMA diagnostic utility.\n");
    printf("Application accesses hardware using " WD_PROD_NAME "\n");

    /* Initialize the LSCDMA library */
    dwStatus = LSCDMA_LibInit();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        LSCDMA_ERR("lscdma_diag: Failed to initialize the LSCDMA library: %s",
            LSCDMA_GetLastErr());
        return dwStatus;
    }

    /* Find and open a LSCDMA device (by default ID) */
    hDev = DeviceFindAndOpen(LSCDMA_DEFAULT_VENDOR_ID, LSCDMA_DEFAULT_DEVICE_ID);

    /* Display main diagnostics menu for communicating with the device */
    MenuMain(&hDev);

    /* Perform necessary cleanup before exiting the program */
    if (hDev)
        DeviceClose(hDev);

    dwStatus = LSCDMA_LibUninit();
    if (WD_STATUS_SUCCESS != dwStatus)
        LSCDMA_ERR("lscdma_diag: Failed to uninit the LSCDMA library: %s", LSCDMA_GetLastErr());

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
    MENU_MAIN_ENABLE_DISABLE_INT,
    MENU_MAIN_DMA,
    MENU_MAIN_EVENTS,
    MENU_MAIN_EXIT = DIAG_EXIT_MENU,
};

/* Main diagnostics menu */
static void MenuMain(WDC_DEVICE_HANDLE *phDev)
{
    DWORD option;

    do
    {
        printf("\n");
        printf("LSCDMA main menu\n");
        printf("--------------\n");
        printf("%d. Scan PCI bus\n", MENU_MAIN_SCAN_PCI_BUS);
        printf("%d. Find and open a LSCDMA device\n", MENU_MAIN_FIND_AND_OPEN);
        if (*phDev)
        {
            printf("%d. Read/write memory and I/O addresses on the device\n",
                MENU_MAIN_RW_ADDR);
            printf("%d. Read/write the device's configuration space\n",
                MENU_MAIN_RW_CFG_SPACE);
            printf("%d. Read/write the run-time registers\n",
                MENU_MAIN_RW_REGS);
            printf("%d. Enable/disable the device's interrupts\n",
                MENU_MAIN_ENABLE_DISABLE_INT);
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
        case MENU_MAIN_FIND_AND_OPEN: /* Find and open a LSCDMA device */
            if (*phDev)
                DeviceClose(*phDev);
            *phDev = DeviceFindAndOpen(0, 0);
            break;
        case MENU_MAIN_RW_ADDR: /* Read/write memory and I/O addresses */
            MenuReadWriteAddr(*phDev);
            break;
        case MENU_MAIN_RW_CFG_SPACE: /* Read/Write the LSCDMA configuration space */
            MenuReadWriteCfgSpace(*phDev);
            break;
        case MENU_MAIN_RW_REGS: /* Read/write the run-time registers */
            MenuReadWriteRegs(*phDev);
            break;
        case MENU_MAIN_ENABLE_DISABLE_INT: /* Enable/disable interrupts */
            MenuInterrupts(*phDev);
            break;
        case MENU_MAIN_DMA:
            MenuDma(*phDev);
            break;
        case MENU_MAIN_EVENTS: /* Register/unregister plug-and-play and power management events */
            MenuEvents(*phDev);
            break;
        }
    } while (MENU_MAIN_EXIT != option);
}

/* -----------------------------------------------
    Device find, open and close
   ----------------------------------------------- */
/* Find and open a LSCDMA device */
static WDC_DEVICE_HANDLE DeviceFindAndOpen(DWORD dwVendorId, DWORD dwDeviceId)
{
    WD_PCI_SLOT slot;

    if (!DeviceFind(dwVendorId, dwDeviceId, &slot))
        return NULL;

    return DeviceOpen(&slot);
}

/* Find a LSCDMA device */
static BOOL DeviceFind(DWORD dwVendorId, DWORD dwDeviceId, WD_PCI_SLOT *pSlot)
{
    DWORD dwStatus;
    DWORD i, dwNumDevices;
    WDC_PCI_SCAN_RESULT scanResult;

    if (dwVendorId == 0)
    {
        if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD((PVOID)&dwVendorId,
            "Enter vendor ID", TRUE, 0, 0))
        {
            return FALSE;
        }

        if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD((PVOID)&dwDeviceId,
            "Enter device ID", TRUE, 0, 0))
        {
            return FALSE;
        }
    }

    BZERO(scanResult);
    dwStatus = WDC_PciScanDevices(dwVendorId, dwDeviceId, &scanResult);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        LSCDMA_ERR("DeviceFind: Failed scanning the PCI bus.\n"
            "Error: 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        return FALSE;
    }

    dwNumDevices = scanResult.dwNumDevices;
    if (!dwNumDevices)
    {
        LSCDMA_ERR("No matching device was found for search criteria "
            "(Vendor ID 0x%lX, Device ID 0x%lX)\n",
            dwVendorId, dwDeviceId);

        return FALSE;
    }

    printf("\n");
    printf("Found %ld matching device%s [ Vendor ID 0x%lX%s, Device ID 0x%lX%s ]:\n",
        dwNumDevices, dwNumDevices > 1 ? "s" : "",
        dwVendorId, dwVendorId ? "" : " (ALL)",
        dwDeviceId, dwDeviceId ? "" : " (ALL)");

    for (i = 0; i < dwNumDevices; i++)
    {
        printf("\n");
        printf("%2ld. Vendor ID: 0x%lX, Device ID: 0x%lX\n",
            i + 1,
            scanResult.deviceId[i].dwVendorId,
            scanResult.deviceId[i].dwDeviceId);

        WDC_DIAG_PciDeviceInfoPrint(&scanResult.deviceSlot[i], FALSE);
    }
    printf("\n");

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

/* Open a handle to a LSCDMA device */
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
        LSCDMA_ERR("DeviceOpen: Failed retrieving the device's resources information.\n"
            "Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        return NULL;
    }

    /* NOTE: You can modify the device's resources information here, if
       necessary (mainly the deviceInfo.Card.Items array or the items number -
       deviceInfo.Card.dwItems) in order to register only some of the resources
       or register only a portion of a specific address space, for example. */

    /* Open a handle to the device */
    hDev = LSCDMA_DeviceOpen(&deviceInfo);
    if (!hDev)
    {
        LSCDMA_ERR("DeviceOpen: Failed opening a handle to the device: %s",
            LSCDMA_GetLastErr());
        return NULL;
    }

    return hDev;
}

/* Close handle to a LSCDMA device */
static void DeviceClose(WDC_DEVICE_HANDLE hDev)
{
    if (!hDev)
        return;

    if (!LSCDMA_DeviceClose(hDev))
    {
        LSCDMA_ERR("DeviceClose: Failed closing LSCDMA device: %s",
            LSCDMA_GetLastErr());
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
        DWORD dwNumAddrSpaces = LSCDMA_GetNumAddrSpaces(hDev);

        /* Find the first active address space */
        for (dwAddrSpace = 0; dwAddrSpace < dwNumAddrSpaces; dwAddrSpace++)
        {
            if (WDC_AddrSpaceIsActive(hDev, dwAddrSpace))
                break;
        }

        /* Sanity check */
        if (dwAddrSpace == dwNumAddrSpaces)
        {
            LSCDMA_ERR("MenuReadWriteAddr: Error - no active address spaces found\n");
            dwAddrSpace = ACTIVE_ADDR_SPACE_NEEDS_INIT;
            return;
        }
    }

    do
    {
        printf("\n");
        printf("Read/write the device's memory and I/O ranges\n");
        printf("---------------------------------------------\n");
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
        case MENU_RW_ADDR_SET_ADDR_SPACE: /* Set active address space for read/write address requests */
        {
            SetAddrSpace(hDev, &dwAddrSpace);
            break;
        }
        case MENU_RW_ADDR_SET_MODE: /* Set active mode for read/write address requests */
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

static void SetAddrSpace(WDC_DEVICE_HANDLE hDev, PDWORD pdwAddrSpace)
{
    DWORD dwAddrSpace;
    DWORD dwNumAddrSpaces = LSCDMA_GetNumAddrSpaces(hDev);
    LSCDMA_ADDR_SPACE_INFO addrSpaceInfo;

    printf("\n");
    printf("Select an active address space:\n");
    printf("-------------------------------\n");

    for (dwAddrSpace = 0; dwAddrSpace < dwNumAddrSpaces; dwAddrSpace++)
    {
        BZERO(addrSpaceInfo);
        addrSpaceInfo.dwAddrSpace = dwAddrSpace;
        if (!LSCDMA_GetAddrSpaceInfo(hDev, &addrSpaceInfo))
        {
            LSCDMA_ERR("SetAddrSpace: Error - Failed to get address space information: %s",
                LSCDMA_GetLastErr());
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

/* Display read/write configuration space menu */
static void MenuReadWriteCfgSpace(WDC_DEVICE_HANDLE hDev)
{
    DWORD option;

    do {
        /* Display pre-defined registers' information */
        printf("\n");
        printf("Configuration registers:\n");
        printf("------------------------\n");
        WDC_DIAG_RegsInfoPrint(hDev, gLSCDMA_CfgRegs, LSCDMA_CFG_REGS_NUM,
            WDC_DIAG_REG_PRINT_ALL & ~WDC_DIAG_REG_PRINT_ADDR_SPACE, FALSE);

        printf("\n");
        printf("Read/write the device's configuration space\n");
        printf("--------------------------------------------\n");
        printf("%d. Read from an offset\n", MENU_RW_CFG_SPACE_READ_OFFSET);
        printf("%d. Write to an offset\n", MENU_RW_CFG_SPACE_WRITE_OFFSET);
        printf("%d. Read all configuration registers defined for the device"
            " (see list above)\n", MENU_RW_CFG_SPACE_READ_ALL_REGS);
        printf("%d. Read from a named register\n", MENU_RW_CFG_SPACE_READ_REG);
        printf("%d. Write to a named register\n", MENU_RW_CFG_SPACE_WRITE_REG);
        printf("%d. Exit menu\n", MENU_RW_CFG_SPACE_EXIT);
        printf("\n");

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option,
            LSCDMA_CFG_REGS_NUM ? MENU_RW_CFG_SPACE_WRITE_REG :
            MENU_RW_CFG_SPACE_WRITE_OFFSET))
        {
            continue;
        }

        switch (option)
        {
        case MENU_RW_CFG_SPACE_EXIT: /* Exit menu */
            break;
        case MENU_RW_CFG_SPACE_READ_OFFSET: /* Read from a configuration space offset */
            WDC_DIAG_ReadWriteBlock(hDev, WDC_READ, WDC_AD_CFG_SPACE);
            break;
        case MENU_RW_CFG_SPACE_WRITE_OFFSET: /* Write to a configuration space offset */
            WDC_DIAG_ReadWriteBlock(hDev, WDC_WRITE, WDC_AD_CFG_SPACE);
            break;
        case MENU_RW_CFG_SPACE_READ_ALL_REGS:
            WDC_DIAG_ReadRegsAll(hDev, gLSCDMA_CfgRegs, LSCDMA_CFG_REGS_NUM,
                TRUE, FALSE);
            break;
        case MENU_RW_CFG_SPACE_READ_REG:  /* Read from a configuration register */
            WDC_DIAG_ReadWriteReg(hDev, gLSCDMA_CfgRegs, LSCDMA_CFG_REGS_NUM, WDC_READ, TRUE);
            break;
        case MENU_RW_CFG_SPACE_WRITE_REG: /* Write to a configuration register */
            WDC_DIAG_ReadWriteReg(hDev, gLSCDMA_CfgRegs, LSCDMA_CFG_REGS_NUM, WDC_WRITE, TRUE);
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

/* Display read/write run-time registers menu */
static void MenuReadWriteRegs(WDC_DEVICE_HANDLE hDev)
{
    DWORD option;

    do {
        /* Display pre-defined registers' information */
        printf("\n");
        printf("LSCDMA run-time registers:\n");
        printf("--------------------------\n");
        WDC_DIAG_RegsInfoPrint(hDev, gLSCDMA_Regs, LSCDMA_REGS_NUM,
            WDC_DIAG_REG_PRINT_ALL, FALSE);

        printf("\n");
        printf("Read/write the LSCDMA run-time registers\n");
        printf("-----------------------------------------\n");
        printf("%d. Read all run-time registers defined for the device (see list above)\n",
            MENU_RW_REGS_READ_ALL);
        printf("%d. Read from a specific register\n", MENU_RW_REGS_READ_REG);
        printf("%d. Write to a specific register\n", MENU_RW_REGS_WRITE_REG);
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
            WDC_DIAG_ReadRegsAll(hDev, gLSCDMA_Regs, LSCDMA_REGS_NUM, FALSE,
                FALSE);
            break;
        case MENU_RW_REGS_READ_REG:  /* Read from a register */
            WDC_DIAG_ReadWriteReg(hDev, gLSCDMA_Regs, LSCDMA_REGS_NUM, WDC_READ,
                FALSE);
            break;
        case MENU_RW_REGS_WRITE_REG: /* Write to a register */
            WDC_DIAG_ReadWriteReg(hDev, gLSCDMA_Regs, LSCDMA_REGS_NUM,
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

/* Enable/Disable interrupts menu */
static void MenuInterrupts(WDC_DEVICE_HANDLE hDev)
{
    DWORD option, dwIntOptions;
    BOOL fIntEnable, fIsMsi;

    dwIntOptions = WDC_GET_INT_OPTIONS(hDev);
    fIsMsi = WDC_INT_IS_MSI(dwIntOptions);
    if (dwIntOptions & INTERRUPT_LEVEL_SENSITIVE)
    {
        /* TODO: You can remove this message after you have modified the
           implementation of LSCDMA_IntEnable() in lscdma_lib.c to
           correctly acknowledge level-sensitive interrupts (see guidelines
           in LSCDMA_IntEnable()) */
        printf("\n");
        printf("WARNING!!!\n");
        printf("----------\n");
        printf("Your hardware has level sensitive interrupts.\n");
        printf("Before enabling the interrupts, %s first modify the source "
           "code of LSCDMA_IntEnable(), in the file lscdma_lib.c, to "
           "correctly acknowledge\n%s interrupts when they occur (as dictated by "
           "the hardware's specifications)\n",
           fIsMsi ? "it's recommended that you" : "you must",
           fIsMsi ? "level sensitive" : "");    }

    do
    {
        fIntEnable = !LSCDMA_IntIsEnabled(hDev);

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
                DWORD dwStatus = LSCDMA_IntEnable(hDev, DiagIntHandler);

                if (WD_STATUS_SUCCESS == dwStatus)
                    printf("Interrupts enabled\n");
                else
                {
                    LSCDMA_ERR("Failed enabling interrupts. Error 0x%lx - %s\n",
                       dwStatus, Stat2Str(dwStatus));
                }
            }
            else
            {
                if (WD_STATUS_SUCCESS == LSCDMA_IntDisable(hDev))
                    printf("Interrupts disabled\n");
                else
                    LSCDMA_ERR("Failed disabling interrupts: %s", LSCDMA_GetLastErr());
            }
            break;
        }
    } while (MENU_INT_EXIT != option);
}

/* Diagnostics interrupt handler routine */
static void DiagIntHandler(WDC_DEVICE_HANDLE hDev, LSCDMA_INT_RESULT *pIntResult)
{
    /* TODO: You can modify this function in order to implement your own
             diagnostics interrupt handler routine */

    printf("Got interrupt number %ld\n", pIntResult->dwCounter);
    printf("Interrupt Type: %s\n",
        WDC_DIAG_IntTypeDescriptionGet(pIntResult->dwEnabledIntType));
    if (WDC_INT_IS_MSI(pIntResult->dwEnabledIntType))
        printf("Message Data: 0x%lx\n", pIntResult->dwLastMessage);

    if (pIntResult->hDma)
    {
        printf("DMA transfer completed succesfully\n");

        if (!LSCDMA_DmaIstoDevice(pIntResult->hDma))
        {
            PVOID pBuf;
            DWORD dwBytes;

            pBuf = LSCDMA_DmaBufferGet(pIntResult->hDma, &dwBytes);
            DIAG_PrintHexBuffer(pBuf, dwBytes, FALSE);
        }
    }

    UNUSED_VAR(hDev);
}

/* -----------------------------------------------
    Direct Memory Access (DMA)
   ---------------------------------------------- */

/* DMA user input menu */
static BOOL MenuDmaOpenGetInput(BOOL *pfToDevice, DWORD *pdwPages,
    UINT32 *pu32Pattern)
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
    *pfToDevice = (1 == option) ? FALSE : TRUE;

    if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD(pdwPages,
        "\nEnter number of pages to transfer (4096 bytes per page)", FALSE, 1,
        *pfToDevice ? MAX_DMA_READ_DESCS : MAX_DMA_WRITE_DESCS))
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

    printf("\n");

    return TRUE;
}

static LSCDMA_DMA_HANDLE LSCDMA_DIAG_DmaOpen(WDC_DEVICE_HANDLE hDev)
{
    BOOL fToDevice;
    DWORD i, dwBytes, dwPages, dwStatus;
    UINT32 u32Pattern;
    UINT32 *pBuf;
    LSCDMA_DMA_HANDLE hDma = NULL;

    /* Get user input */
    if (!MenuDmaOpenGetInput(&fToDevice, &dwPages, &u32Pattern))
        return NULL;

    hDma = LSCDMA_DmaOpen(hDev, dwPages * 4096, fToDevice);
    if (!hDma)
    {
        LSCDMA_ERR("Failed opening DMA handle\n");
        return NULL;
    }

    dwStatus = LSCDMA_IntEnable(hDev, DiagIntHandler);
    if (dwStatus != WD_STATUS_SUCCESS)
    {
        LSCDMA_ERR("Failed enabling interrupts\n");
        goto Error;
    }
    LSCDMA_DmaInterruptsEnable(hDma);

    pBuf = (UINT32 *)LSCDMA_DmaBufferGet(hDma, &dwBytes);
    for (i = 0; i < (dwBytes / sizeof(UINT32)); i++)
        pBuf[i] = u32Pattern;

    LSCDMA_DmaStart(hDma);

    return hDma;

Error:
    LSCDMA_DmaClose(hDma);
    return NULL;
}

static void LSCDMA_DIAG_DmaClose(WDC_DEVICE_HANDLE hDev, LSCDMA_DMA_HANDLE hDma)
{
    DWORD dwStatus;

    LSCDMA_DMAStop(hDma);
    if (WDC_IntIsEnabled(hDev))
    {
        LSCDMA_DmaInterruptsDisable(hDma);
        dwStatus = LSCDMA_IntDisable(hDev);
        printf("DMA interrupts disable%s\n",
            (WD_STATUS_SUCCESS == dwStatus) ? "d" : " failed");
    }
    LSCDMA_DmaClose(hDma);
}

/* DMA transfer menu options */
enum {
    MENU_DMA_OPEN_CLOSE = 1,
    MENU_DMA_EXIT = DIAG_EXIT_MENU
};

static void MenuDma(WDC_DEVICE_HANDLE hDev)
{
    DWORD option;
    LSCDMA_DMA_HANDLE hDma = NULL;

    do
    {
        printf("\nOpen/Close Direct Memory Access (DMA)\n");
        printf("----------------------------------\n");
        printf("%d. %s DMA\n", MENU_DMA_OPEN_CLOSE, hDma ? "Close" : "Open");
        printf("%d. Exit menu\n\n", MENU_DMA_EXIT);

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option, MENU_DMA_OPEN_CLOSE))
            continue;

        switch (option)
        {
            case MENU_DMA_EXIT: /* Exit menu */
                break;
            case MENU_DMA_OPEN_CLOSE: /* Open/close DMA menu option */
                if (!hDma)
                {
                    hDma = LSCDMA_DIAG_DmaOpen(hDev);
                    if (!hDma)
                        LSCDMA_ERR("DMA open failed\n");
                }
                else
                {
                    LSCDMA_DIAG_DmaClose(hDev, hDma);
                    hDma = NULL;
                }
                break;
        }
    } while (MENU_DMA_EXIT != option);

    if (hDma)
    {
        LSCDMA_DIAG_DmaClose(hDev, hDma);
        printf("Closed DMA handle\n");
    }
}

/* ----------------------------------------------------
    Plug-and-play and power management events handling
   ---------------------------------------------------- */
/* Events menu options */
enum {
    MENU_EVENTS_REGISTER_UNREGISTER = 1,
    MENU_EVENTS_EXIT = DIAG_EXIT_MENU,
};

/* Register/unregister Plug-and-play and power management events */
static void MenuEvents(WDC_DEVICE_HANDLE hDev)
{
    DWORD option;
    BOOL fRegister;

    do
    {
        fRegister = !LSCDMA_EventIsRegistered(hDev);

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
                if (WD_STATUS_SUCCESS == LSCDMA_EventRegister(hDev, DiagEventHandler))
                    printf("Events registered\n");
                else
                    LSCDMA_ERR("Failed to register events. Last error:\n%s", LSCDMA_GetLastErr());
            }
            else
            {
                if (WD_STATUS_SUCCESS == LSCDMA_EventUnregister(hDev))
                    printf("Events unregistered\n");
                else
                    LSCDMA_ERR("Failed to unregister events. Last Error:\n%s", LSCDMA_GetLastErr());
            }
            break;
        }
    } while (MENU_EVENTS_EXIT != option);
}

/* Plug-and-play and power management events handler routine */
static void DiagEventHandler(WDC_DEVICE_HANDLE hDev, DWORD dwAction)
{
    /* TODO: You can modify this function in order to implement your own
             diagnostics events handler routine */

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

