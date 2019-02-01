/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

/***************************************************************************
*  File: p6466_diag.c
*
*  Sample user-mode diagnostics application for accessing PLX 6466
*  devices using WinDriver's API and the plx_lib_6466, plx_diag_lib_6466,
*  plx_lib and plx_diag_lib libraries.
*
*  Note: This code sample is provided AS-IS and as a guiding sample only.
****************************************************************************/

#include <stdio.h>
#include "utils.h"
#include "status_strings.h"
#include "samples/shared/diag_lib.h"
#include "samples/shared/wdc_diag_lib.h"
#include "../diag_lib/plx_diag_lib.h"
#include "wdc_lib.h"
#include "../lib/plx_lib.h"
#include "bits.h"
#include "lib_6466/plx_lib_6466.h"

/*************************************************************
  General definitions
 *************************************************************/
/* Default vendor and device IDs */
#define P6466_DEFAULT_VENDOR_ID 0x10b5 /* Vendor ID */
#define P6466_DEFAULT_DEVICE_ID 0x6540 /* Device ID */

/*************************************************************
  Global variables
 *************************************************************/
/* User's input command */
static CHAR gsInput[256];

/* Configuration registers information array */
static const WDC_REG gPLX_CfgRegs[] = {
    { WDC_AD_CFG_SPACE, PLX6466_VID, WDC_SIZE_16, WDC_READ_WRITE, "VID",
        "Vendor ID" },
    { WDC_AD_CFG_SPACE, PLX6466_DID, WDC_SIZE_16, WDC_READ_WRITE, "DID",
        "Device ID" },
    { WDC_AD_CFG_SPACE, PLX6466_CR, WDC_SIZE_16, WDC_READ_WRITE, "CMD",
        "Command" },
    { WDC_AD_CFG_SPACE, PLX6466_PSR, WDC_SIZE_16, WDC_READ_WRITE, "PSTS",
        "Primary Status" },
    { WDC_AD_CFG_SPACE, PLX6466_REV, WDC_SIZE_32, WDC_READ_WRITE, "RID_CLCD",
        "Revision ID & Class Code" },
    { WDC_AD_CFG_SPACE, PLX6466_CCSC, WDC_SIZE_8, WDC_READ_WRITE, "SCC",
        "Sub Class Code" },
    { WDC_AD_CFG_SPACE, PLX6466_CCBC, WDC_SIZE_8, WDC_READ_WRITE, "BCC",
        "Base Class Code" },
    { WDC_AD_CFG_SPACE, PLX6466_CLSR, WDC_SIZE_8, WDC_READ_WRITE, "CALN",
        "Cache Line Size" },
    { WDC_AD_CFG_SPACE, PLX6466_PLTR, WDC_SIZE_8, WDC_READ_WRITE, "PLAT",
        "Primary Latency Timer" },
    { WDC_AD_CFG_SPACE, PLX6466_HDR, WDC_SIZE_8, WDC_READ_WRITE, "HDR",
        "Header Type" },
    { WDC_AD_CFG_SPACE, PLX6466_BISTR, WDC_SIZE_8, WDC_READ_WRITE, "BIST",
        "Built-in Self Test" },
    { WDC_AD_CFG_SPACE, PLX6466_BAR0, WDC_SIZE_32, WDC_READ_WRITE, "BADDR0",
        "Base Address 0" },
    { WDC_AD_CFG_SPACE, PLX6466_BAR1, WDC_SIZE_32, WDC_READ_WRITE, "BADDR1",
        "Base Address 1" },
    { WDC_AD_CFG_SPACE, PLX6466_PBN, WDC_SIZE_8, WDC_READ_WRITE, "PBN",
        "Primary bus number" },
    { WDC_AD_CFG_SPACE, PLX6466_SBN, WDC_SIZE_8, WDC_READ_WRITE, "SBN",
        "Secondary bus number" },
    { WDC_AD_CFG_SPACE, PLX6466_SUBN, WDC_SIZE_8, WDC_READ_WRITE, "SUBN",
        "Subordinate bus number" },
    { WDC_AD_CFG_SPACE, PLX6466_SLTR, WDC_SIZE_8, WDC_READ_WRITE, "SLTR",
        "Secondary latency timer" },
    { WDC_AD_CFG_SPACE, PLX6466_IOB, WDC_SIZE_8, WDC_READ_WRITE, "IOB",
        "I/O Base" },
    { WDC_AD_CFG_SPACE, PLX6466_IOL, WDC_SIZE_8, WDC_READ_WRITE, "IOL",
        "I/O Limit" },
    { WDC_AD_CFG_SPACE, PLX6466_SSR, WDC_SIZE_8, WDC_READ_WRITE, "SSR",
        "Secondary Status register" },
    { WDC_AD_CFG_SPACE, PLX6466_MEMB, WDC_SIZE_16, WDC_READ_WRITE, "MEMB",
        "Memory Base" },
    { WDC_AD_CFG_SPACE, PLX6466_MEML, WDC_SIZE_16, WDC_READ_WRITE, "MEML",
        "Memory Limit" },
    { WDC_AD_CFG_SPACE, PLX6466_PMB, WDC_SIZE_16, WDC_READ_WRITE, "PMB",
        "Prefetchable Memory Base" },
    { WDC_AD_CFG_SPACE, PLX6466_PML, WDC_SIZE_16, WDC_READ_WRITE, "PML",
        "Prefetchable Memory Limit" },
    { WDC_AD_CFG_SPACE, PLX6466_PMBU, WDC_SIZE_32, WDC_READ_WRITE, "PMBU",
        "Prefetchable Memory Base Upper 32 bits" },
    { WDC_AD_CFG_SPACE, PLX6466_PMLU, WDC_SIZE_32, WDC_READ_WRITE, "PMLU",
        "Prefetchable Memory Limit Upper 32 bits" },
    { WDC_AD_CFG_SPACE, PLX6466_CAP, WDC_SIZE_8, WDC_READ_WRITE, "NEW_CAP",
        "New Capabilities Pointer" },
    { WDC_AD_CFG_SPACE, PLX6466_IPR, WDC_SIZE_8, WDC_READ_WRITE, "INTPIN",
        "Interrupt Pin" },
    /* PLX-specific configuration registers */
    { WDC_AD_CFG_SPACE, PLX6466_PMCI, WDC_SIZE_8, WDC_READ_WRITE, "PMCI",
        "Power Management Capability ID" },
    { WDC_AD_CFG_SPACE, PLX6466_PMNCP, WDC_SIZE_8, WDC_READ_WRITE, "PMNCP",
        "Power Management Next Capability Pointer" },
    { WDC_AD_CFG_SPACE, PLX6466_PMC, WDC_SIZE_16, WDC_READ_WRITE, "PMCAP",
        "Power Management Capabilities" },
    { WDC_AD_CFG_SPACE, PLX6466_PMCS, WDC_SIZE_16, WDC_READ_WRITE, "PMCSR",
        "Power Management Control/Status" },
    { WDC_AD_CFG_SPACE, PLX6466_HSCL, WDC_SIZE_8, WDC_READ_WRITE, "HS_CAPID",
        "Hot Swap Capability ID" },
    { WDC_AD_CFG_SPACE, PLX6466_HSNCP, WDC_SIZE_8, WDC_READ_WRITE, "HS_NEXT",
        "Hot Swap Next Capability Pointer" },
    { WDC_AD_CFG_SPACE, PLX6466_HSCS, WDC_SIZE_8, WDC_READ_WRITE, "HS_CSR",
        "Hot Swap Control/Status" },
    { WDC_AD_CFG_SPACE, PLX6466_VCI, WDC_SIZE_8, WDC_READ_WRITE, "VPD_CAPID",
        "PCI Vital Product Data Capability ID" },
    { WDC_AD_CFG_SPACE, PLX6466_VNCP, WDC_SIZE_8, WDC_READ_WRITE, "VPD_NEXT",
        "PCI Vital Product Next Capability Pointer" },
    { WDC_AD_CFG_SPACE, PLX6466_VPDA, WDC_SIZE_16, WDC_READ_WRITE, "VPD_ADDR",
        "PCI Vital Product Data Address" },
    { WDC_AD_CFG_SPACE, PLX6466_VPDD, WDC_SIZE_32, WDC_READ_WRITE, "VPD_DATA",
        "PCI VPD Data" },
    
    /* NOTE - add here more registers according to the configuration space */
};

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
static WDC_DEVICE_HANDLE PLX6466_DIAG_DeviceFindAndOpen(DWORD dwVendorId,
    DWORD dwDeviceId);

/* -----------------------------------------------
    Read/write the configuration space
   ----------------------------------------------- */
static void PLX6466_DIAG_MenuReadWriteCfgSpace(WDC_DEVICE_HANDLE hDev);

/* ----------------------------------------------------
    Plug-and-play and power management events handling
   ---------------------------------------------------- */
static void PLX6466_DIAG_MenuEvents(WDC_DEVICE_HANDLE hDev,
    PLX_EVENT_HANDLER DiagEventHandler);
static void DiagEventHandler(WDC_DEVICE_HANDLE hDev, DWORD dwAction);

/* -----------------------------------------------
    Access the serial EEPROM
   ----------------------------------------------- */
static void PLX6466_DIAG_MenuEEPROM(WDC_DEVICE_HANDLE hDev);

/*************************************************************
  Functions implementation
 *************************************************************/
int main(void)
{
    DWORD dwStatus;
    WDC_DEVICE_HANDLE hDev = NULL;
    
    printf("\n");
    printf("PLX 6466 diagnostic utility.\n");
    printf("Application accesses hardware using " WD_PROD_NAME ".\n");

    /* Initialize the PLX library */
    dwStatus = PLX_LibInit();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        PLX_DIAG_ERR("p6466_diag: Failed to initialize the PLX library: %s",
            PLX_GetLastErr());
        return dwStatus;
    }

    /* Find and open a PLX 6466 device (by default ID) */
    if (P6466_DEFAULT_VENDOR_ID)
    {
        hDev = PLX6466_DIAG_DeviceFindAndOpen(P6466_DEFAULT_VENDOR_ID,
            P6466_DEFAULT_DEVICE_ID);

        if (!hDev)
        {
            PLX_DIAG_ERR("p6466_diag: Failed locating and opening a handle to"
                " device (VID 0x%x DID 0x%x)\n", P6466_DEFAULT_VENDOR_ID,
                P6466_DEFAULT_DEVICE_ID);
        }
    }

    /* Display main diagnostics menu for communicating with the device */
    MenuMain(&hDev);

    if (hDev)
        PLX_DeviceClose(hDev);
 
    dwStatus = PLX_LibUninit();
    if (WD_STATUS_SUCCESS != dwStatus)
        PLX_DIAG_ERR("p6466_diag: Failed to uninit the PLX library: %s",
            PLX_GetLastErr());
    
    return dwStatus;
}
/* -----------------------------------------------
    Main diagnostics menu
   ----------------------------------------------- */
/* Main menu options */
enum {
    MENU_MAIN_SCAN_PCI_BUS = 1,
    MENU_MAIN_FIND_AND_OPEN,
    MENU_MAIN_RW_CFG_SPACE,
    MENU_MAIN_EVENTS,
    MENU_MAIN_EEPROM,
    MENU_MAIN_EXIT = DIAG_EXIT_MENU,
};

/* Main diagnostics menu */
static void MenuMain(WDC_DEVICE_HANDLE *phDev)
{
    DWORD option;

    do
    {
        printf("\n");
        printf("PLX 6466 main menu\n");
        printf("-------------------\n");
        printf("%d. Scan PCI bus\n", MENU_MAIN_SCAN_PCI_BUS);
        printf("%d. Find and open a PLX device\n", MENU_MAIN_FIND_AND_OPEN);
        if (*phDev)
        {
            printf("%d. Read/write the device's PCI configuration space\n",
                MENU_MAIN_RW_CFG_SPACE);
            printf("%d. Register/unregister plug-and-play and power management"
                " events\n", MENU_MAIN_EVENTS);
            printf("%d. Access the serial EEPROM on the board\n",
                MENU_MAIN_EEPROM);
        }
        printf("%d. Exit\n", MENU_MAIN_EXIT);
        
        if (DIAG_INPUT_SUCCESS != DIAG_GetMenuOption(&option, *phDev ?
            MENU_MAIN_EEPROM : MENU_MAIN_FIND_AND_OPEN))
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
        case MENU_MAIN_FIND_AND_OPEN: /* Find and open a PLX device */
            /* Close open device handle (if exists) */
            if (*phDev)
                PLX_DeviceClose(*phDev);

            /* Open a new device handle */
            *phDev = PLX6466_DIAG_DeviceFindAndOpen(0, 0);
            if (!*phDev)
            {
                PLX_DIAG_ERR("p6466_diag: Failed locating and opening a handle"
                    "to device (VID 0x%x DID 0x%x)\n", P6466_DEFAULT_VENDOR_ID,
                    P6466_DEFAULT_DEVICE_ID);   
            }       
            break;
        case MENU_MAIN_RW_CFG_SPACE: /* Read/Write the device's configuration
                                        space */
            PLX6466_DIAG_MenuReadWriteCfgSpace(*phDev);
            break;
        case MENU_MAIN_EVENTS: /* Register/unregister plug-and-play and power
                                  management events */
            PLX6466_DIAG_MenuEvents(*phDev, DiagEventHandler);
            break;
        case MENU_MAIN_EEPROM:
            PLX6466_DIAG_MenuEEPROM(*phDev);
            break;
        }
    } while (MENU_MAIN_EXIT != option);
}

/* -----------------------------------------------
    Device find, open and close
   ----------------------------------------------- */
/* Find and open a PLX device */
static WDC_DEVICE_HANDLE PLX6466_DIAG_DeviceFindAndOpen(DWORD dwVendorId,
    DWORD dwDeviceId)
{
    WDC_DEVICE_HANDLE pDeviceHandle=NULL;
    WD_PCI_SLOT slot;
    
    if (!PLX_DIAG_DeviceFind(dwVendorId, dwDeviceId, &slot))
        return NULL;

    if ((PLX6466_DeviceOpenBySlot(&pDeviceHandle, &slot)))
        return NULL;
    
    return pDeviceHandle;   
}

/* -----------------------------------------------
    Read/write the configuration space
   ----------------------------------------------- */
/* Read/write the configuration space menu options */
enum {
    PLX_DIAG_MENU_RW_CFG_SPACE_READ_OFFSET = 1,
    PLX_DIAG_MENU_RW_CFG_SPACE_WRITE_OFFSET,
    PLX_DIAG_MENU_RW_CFG_SPACE_READ_ALL_REGS,
    PLX_DIAG_MENU_RW_CFG_SPACE_READ_REG,
    PLX_DIAG_MENU_RW_CFG_SPACE_WRITE_REG,
    PLX_DIAG_MENU_RW_CFG_SPACE_EXIT = DIAG_EXIT_MENU,
};

/* Display read/write configuration space menu */
static void PLX6466_DIAG_MenuReadWriteCfgSpace(WDC_DEVICE_HANDLE hDev)
{
    DWORD option;
    const WDC_REG *pCfgRegs = gPLX_CfgRegs;
    DWORD dwCfgRegsNum = sizeof(gPLX_CfgRegs) / sizeof(*pCfgRegs);

    do {
        /* Display pre-defined registers' information */
        if (dwCfgRegsNum)
        {
            printf("\n");
            printf("Configuration registers:\n");
            printf("------------------------\n");
            WDC_DIAG_RegsInfoPrint(hDev, pCfgRegs, dwCfgRegsNum,
                WDC_DIAG_REG_PRINT_ALL & ~WDC_DIAG_REG_PRINT_ADDR_SPACE, FALSE);
        }

        printf("\n");
        printf("Read/write the device's configuration space\n");
        printf("--------------------------------------------\n");
        printf("%d. Read from a specific offset in the configuration space\n",
            PLX_DIAG_MENU_RW_CFG_SPACE_READ_OFFSET);
        printf("%d. Write to a specific offset in the configuration space\n",
            PLX_DIAG_MENU_RW_CFG_SPACE_WRITE_OFFSET);
        if (dwCfgRegsNum)
        {
            printf("%d. Read all configuration registers defined for the "
                "device (see list above)\n", 
                PLX_DIAG_MENU_RW_CFG_SPACE_READ_ALL_REGS);
            printf("%d. Read from a specific register\n",
                PLX_DIAG_MENU_RW_CFG_SPACE_READ_REG);
            printf("%d. Write to a specific register\n",
                PLX_DIAG_MENU_RW_CFG_SPACE_WRITE_REG);
        }
        printf("%d. Exit menu\n", PLX_DIAG_MENU_RW_CFG_SPACE_EXIT);
        printf("\n");

        if (DIAG_INPUT_SUCCESS != DIAG_GetMenuOption(&option, dwCfgRegsNum ?
            PLX_DIAG_MENU_RW_CFG_SPACE_WRITE_REG :
            PLX_DIAG_MENU_RW_CFG_SPACE_WRITE_OFFSET))
        {
            printf("\n");
            printf("Press ENTER to return to the menu\n");
            fgets(gsInput, sizeof(gsInput), stdin);
            continue;
        }

        switch (option)
        {
        case PLX_DIAG_MENU_RW_CFG_SPACE_EXIT:
            /* Exit menu */
            break;
        case PLX_DIAG_MENU_RW_CFG_SPACE_READ_OFFSET:
            /* Read from a configuration space offset */
            WDC_DIAG_ReadWriteBlock(hDev, WDC_READ, WDC_AD_CFG_SPACE);
            break;
        case PLX_DIAG_MENU_RW_CFG_SPACE_WRITE_OFFSET:
            /* Write to a configuration space offset */
            WDC_DIAG_ReadWriteBlock(hDev, WDC_WRITE, WDC_AD_CFG_SPACE);
            break;
        case PLX_DIAG_MENU_RW_CFG_SPACE_READ_ALL_REGS:
            WDC_DIAG_ReadRegsAll(hDev, pCfgRegs, dwCfgRegsNum, TRUE, FALSE);
            break;
        case PLX_DIAG_MENU_RW_CFG_SPACE_READ_REG:
            /* Read from a configuration register */
            WDC_DIAG_ReadWriteReg(hDev, pCfgRegs, dwCfgRegsNum, WDC_READ, TRUE);
            break;
        case PLX_DIAG_MENU_RW_CFG_SPACE_WRITE_REG:
            /* Write to a configuration register */
            WDC_DIAG_ReadWriteReg(hDev, pCfgRegs, dwCfgRegsNum, WDC_WRITE,
                TRUE);
            break;
        }
    } while (PLX_DIAG_MENU_RW_CFG_SPACE_EXIT != option);
}

/* ----------------------------------------------------
    Plug-and-play and power management events handling
   ---------------------------------------------------- */
/* Events menu options */
enum {
    PLX_DIAG_MENU_EVENTS_REGISTER_UNREGISTER = 1,
    PLX_DIAG_MENU_EVENTS_EXIT = DIAG_EXIT_MENU,
};

/* Register/unregister Plug-and-play and power management events */
static void PLX6466_DIAG_MenuEvents(WDC_DEVICE_HANDLE hDev,
    PLX_EVENT_HANDLER DiagEventHandler)
{
    DWORD option;
    BOOL fRegister;

    do
    {
        fRegister = !PLX6466_EventIsRegistered(hDev);
        
        printf("\n");
        printf("Plug-and-play and power management events\n");
        printf("------------------------------------------\n");
        printf("%d. %s events\n", PLX_DIAG_MENU_EVENTS_REGISTER_UNREGISTER,
            fRegister ? "Register" : "Unregister");
        printf("%d. Exit menu\n", PLX_DIAG_MENU_EVENTS_EXIT);
        printf("\n");
        
        if (DIAG_INPUT_SUCCESS != DIAG_GetMenuOption(&option, 
            PLX_DIAG_MENU_EVENTS_REGISTER_UNREGISTER))
        {
            continue;
        }

        switch (option)
        {
        case PLX_DIAG_MENU_EVENTS_EXIT: /* Exit menu */
            break;
        case PLX_DIAG_MENU_EVENTS_REGISTER_UNREGISTER: /* Register/unregister
                                                          events */
            if (fRegister)
            {
                if (WD_STATUS_SUCCESS == PLX6466_EventRegister(hDev,
                    DiagEventHandler))
                {
                    printf("Events registered\n");
                }
                else
                {
                    PLX_DIAG_ERR("Failed to register events. Last error:\n%s",
                        PLX_GetLastErr());
                }
            }
            else
            {
                if (WD_STATUS_SUCCESS == PLX6466_EventUnregister(hDev))
                    printf("Events unregistered\n");
                else
                {
                    PLX_DIAG_ERR("Failed to unregister events. Last Error:\n%s",
                        PLX_GetLastErr());
                }
            }
            break;
        }
    } while (PLX_DIAG_MENU_EVENTS_EXIT != option);
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
    Access the serial EEPROM
   ----------------------------------------------- */
/* Serial EEPROM access menu options */
enum {
    PLX_DIAG_MENU_EEPROM_DISPLAY_CONTENT = 1,
    PLX_DIAG_MENU_EEPROM_READ,
    PLX_DIAG_MENU_EEPROM_WRITE,
    PLX_DIAG_MENU_EEPROM_EXIT = DIAG_EXIT_MENU,
};

/* Access the serial EEPROM on the board */
#define EEPROM_MAX_OFFSET 0xFF
static void PLX6466_DIAG_MenuEEPROM(WDC_DEVICE_HANDLE hDev)
{
    DWORD option;
    DWORD dwStatus;
    DWORD dwOffset;
    UINT32 u32Data = 0;

    if (!PLX6466_EEPROM_VPD_Validate(hDev))
    {
        printf("EEPROM can't be accessed\n");
    }
    do
    {
        printf("\n");
        printf("Access the board's serial EEPROM\n");
        printf("---------------------------------\n");
        printf("NOTE: EEPROM data is accessed via Vital Product Data (VPD) as"
            "DWORDs\n");
        printf("\n");
        printf("%d. Display EEPROM content\n",
            PLX_DIAG_MENU_EEPROM_DISPLAY_CONTENT);
        printf("%d. Read from the serial EEPROM\n", PLX_DIAG_MENU_EEPROM_READ);
        printf("%d. Write to the serial EEPROM\n", PLX_DIAG_MENU_EEPROM_WRITE);
        printf("%d. Exit menu\n", PLX_DIAG_MENU_EEPROM_EXIT);
        printf("\n");
        
        if (DIAG_INPUT_SUCCESS != DIAG_GetMenuOption(&option,
            PLX_DIAG_MENU_EEPROM_WRITE))
            continue;

        switch (option)
        {
        case PLX_DIAG_MENU_EEPROM_EXIT: /* Exit menu */
            break;
        case PLX_DIAG_MENU_EEPROM_DISPLAY_CONTENT:
            for (dwOffset = 0; dwOffset < EEPROM_MAX_OFFSET; dwOffset += 4)
            {
                if (!(dwOffset % 0x10))
                printf("\n %02lX: ", dwOffset);
                dwStatus = PLX6466_EEPROM_VPD_Read32(hDev, dwOffset, &u32Data);
                if (WD_STATUS_SUCCESS != dwStatus)
                {
                    printf("\nError occurred while reading offset 0x%lX of the"
                        " serial EEPROM.\n Error 0x%lx - %s\n", dwOffset,
                        dwStatus, Stat2Str(dwStatus));
                    break;
                }
                printf("%08X  ", u32Data);
            }
            printf("\n");
     
            break;
        case PLX_DIAG_MENU_EEPROM_READ: /* Read from the serial EEPROM */
            sprintf(gsInput, "Enter offset to read from (must be a multiple of"
                "%ld)", WDC_SIZE_32);
            if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD(&dwOffset, gsInput, TRUE,
                0, EEPROM_MAX_OFFSET))
            {
                break;
            }

            dwStatus = PLX6466_EEPROM_VPD_Read32(hDev, dwOffset, &u32Data);

            if (WD_STATUS_SUCCESS == dwStatus)
            {
                printf("Read 0x%X from offset 0x%lX of the serial EEPROM\n",
                    u32Data, dwOffset);
            }
            else
                printf("Failed reading from offset 0x%lX of the serial EEPROM."
                    "\n", dwOffset);
            
            break;
        case PLX_DIAG_MENU_EEPROM_WRITE: /* Write to the serial EEPROM */
            sprintf(gsInput, "Enter offset to write to (must be a multiple of"
                "%ld)", WDC_SIZE_32);
            if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD(&dwOffset, gsInput, TRUE,
                0, EEPROM_MAX_OFFSET))
            {
                break;
            }

            if (DIAG_INPUT_SUCCESS != DIAG_InputNum((PVOID)&u32Data,
                "Enter data to write", TRUE, sizeof(u32Data), 0, 0))
            {
                break;
            }
                
            dwStatus = PLX6466_EEPROM_VPD_Write32(hDev, dwOffset, u32Data);

            printf("%s 0x%X to offset 0x%lX of the serial EEPROM\n",
                (WD_STATUS_SUCCESS == dwStatus) ? "Wrote" : "Failed to write",
                u32Data, dwOffset);
            break;
        }
    } while (PLX_DIAG_MENU_EEPROM_EXIT != option);
}

