/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

/************************************************************************
*  File: p9050_diag.c
*
*  Sample user-mode diagnostics application for accessing PLX 9050
*  devices using WinDriver's API and the plx_lib and plx_diag_lib
*  libraries.
*
*  Note: This code sample is provided AS-IS and as a guiding sample only.
*************************************************************************/

#include <stdio.h>
#include "samples/shared/diag_lib.h"
#include "samples/shared/wdc_diag_lib.h"
#include "../diag_lib/plx_diag_lib.h"

/*************************************************************
  General definitions
 *************************************************************/
/* Default vendor and device IDs */
#define P9050_DEFAULT_VENDOR_ID 0x10b5 /* Vendor ID */
#define P9050_DEFAULT_DEVICE_ID 0x9050 /* Device ID */

/* PLX 9050 is a master device */
#define IS_MASTER FALSE

/*************************************************************
  Static functions prototypes
 *************************************************************/
/* -----------------------------------------------
    Main diagnostics menu
   ----------------------------------------------- */
static void MenuMain(WDC_DEVICE_HANDLE *phDev);

/* -----------------------------------------------
    Interrupt handling
   ----------------------------------------------- */
static void DiagIntHandler(WDC_DEVICE_HANDLE hDev, PLX_INT_RESULT *pIntResult);

/* ----------------------------------------------------
    Plug-and-play and power management events handling
   ---------------------------------------------------- */
static void DiagEventHandler(WDC_DEVICE_HANDLE hDev, DWORD dwAction);

/*************************************************************
  Functions implementation
 *************************************************************/
int main(void)
{
    DWORD dwStatus;
    WDC_DEVICE_HANDLE hDev = NULL;

    printf("\n");
    printf("PLX 9050 diagnostic utility.\n");
    printf("Application accesses hardware using " WD_PROD_NAME ".\n");

    /* Initialize the PLX library */
    dwStatus = PLX_LibInit();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        PLX_DIAG_ERR("p9050_diag: Failed to initialize the PLX library: %s",
            PLX_GetLastErr());
        return dwStatus;
    }

    /* Find and open a PLX 9050 device (by default ID) */
    if (P9050_DEFAULT_VENDOR_ID)
    {
        hDev = PLX_DIAG_DeviceFindAndOpen(P9050_DEFAULT_VENDOR_ID,
            P9050_DEFAULT_DEVICE_ID, IS_MASTER);
        if (!hDev)
        {
            PLX_DIAG_ERR("p9050_diag: Failed locating and opening a handle to "
                "device (VID 0x%x DID 0x%x)\n", P9050_DEFAULT_VENDOR_ID,
                P9050_DEFAULT_DEVICE_ID);
        }
    }

    /* Display main diagnostics menu for communicating with the device */
    MenuMain(&hDev);

    /* Perform necessary cleanup before exiting the program */
    if (hDev)
        PLX_DeviceClose(hDev);

    dwStatus = PLX_LibUninit();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        PLX_DIAG_ERR("p9050_diag: Failed to uninit the PLX library: %s",
            PLX_GetLastErr());
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
    MENU_MAIN_ENABLE_DISABLE_INT,
    MENU_MAIN_EVENTS,
    MENU_MAIN_EEPROM,
    MENU_MAIN_EXIT = DIAG_EXIT_MENU,
};

/* Main diagnostics menu */
static void MenuMain(WDC_DEVICE_HANDLE *phDev)
{
    DWORD option;
    BOOL fEEPROMIsPresent = FALSE;

    do
    {
        printf("\n");
        printf("PLX 9050 main menu\n");
        printf("-------------------\n");
        printf("%d. Scan PCI bus\n", MENU_MAIN_SCAN_PCI_BUS);
        printf("%d. Find and open a PLX device\n", MENU_MAIN_FIND_AND_OPEN);
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
            printf("%d. Register/unregister plug-and-play and power management "
                "events\n", MENU_MAIN_EVENTS);
            fEEPROMIsPresent = PLX_EEPROMIsPresent(*phDev);
            if (fEEPROMIsPresent)
            {
                printf("%d. Access the serial EEPROM on the board\n",
                    MENU_MAIN_EEPROM);
            }
        }
        printf("%d. Exit\n", MENU_MAIN_EXIT);

        if (DIAG_INPUT_SUCCESS != DIAG_GetMenuOption(&option, *phDev ?
            (fEEPROMIsPresent ? MENU_MAIN_EEPROM : MENU_MAIN_EVENTS)
            : MENU_MAIN_FIND_AND_OPEN))
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
            *phDev = PLX_DIAG_DeviceFindAndOpen(0, 0, IS_MASTER);
            if (!*phDev)
            {
                PLX_DIAG_ERR("p9050_diag: Failed locating and opening a handle "
                    "to device (VID 0x%x DID 0x%x)\n", P9050_DEFAULT_VENDOR_ID,
                    P9050_DEFAULT_DEVICE_ID);
            }
            break;
        case MENU_MAIN_RW_ADDR: /* Read/write memory and I/O addresses */
            PLX_DIAG_MenuReadWriteAddr(*phDev);
            break;
        case MENU_MAIN_RW_CFG_SPACE: /* Read/Write the device's configuration
                                      * space */
            PLX_DIAG_MenuReadWriteCfgSpace(*phDev);
            break;
        case MENU_MAIN_RW_REGS: /* Read/write the run-time registers */
            PLX_DIAG_MenuReadWriteRegs(*phDev);
            break;
        case MENU_MAIN_ENABLE_DISABLE_INT: /* Enable/disable interrupts */
            PLX_DIAG_MenuInterrupts(*phDev, DiagIntHandler, NULL);
            break;
        case MENU_MAIN_EVENTS: /* Register/unregister plug-and-play and power
                                * management events */
            PLX_DIAG_MenuEvents(*phDev, DiagEventHandler);
            break;
        case MENU_MAIN_EEPROM:
            if (!fEEPROMIsPresent)
            {
                printf("Invalid option\n");
                break;
            }
            PLX_DIAG_MenuEEPROM(*phDev, BIT7);
            break;
        }
    } while (MENU_MAIN_EXIT != option);
}

/* -----------------------------------------------
    Interrupt handling
   ----------------------------------------------- */
/* Diagnostics interrupt handler routine */
static void DiagIntHandler(WDC_DEVICE_HANDLE hDev, PLX_INT_RESULT *pIntResult)
{
    /* TODO: You can modify this function in order to implement your own
             diagnostics interrupt handler routine */

    printf("\n");
    printf("-----------------------------------------------------------\n");
    printf("Got interrupt number %ld\n", pIntResult->dwCounter);
    printf("Data read from registers when the interrupt occurred:\n");
    printf("INTCSR: 0x%X\n", (WORD)pIntResult->u32INTCSR);
    printf("-----------------------------------------------------------\n");
    printf("\n");
}

/* ----------------------------------------------------
    Plug-and-play and power management events handling
   ---------------------------------------------------- */
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

