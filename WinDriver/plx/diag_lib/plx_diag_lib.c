/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

/*******************************************************************************
*  File: plx_diag_lib.c - Implementation of shared PLX diagnostics library for
*                         accessing PLX devices from the user-mode.
*
*  Note: This code sample is provided AS-IS and as a guiding sample only.
*******************************************************************************/

#include <stdio.h>
#include "utils.h"
#include "status_strings.h"
#include "samples/shared/diag_lib.h"
#include "samples/shared/wdc_diag_lib.h"
#include "plx_diag_lib.h"

/*************************************************************
  Global variables
 *************************************************************/
/* User's input command */
CHAR gsInput[256];

/* Configuration registers information array */
static const WDC_REG gPLX_CfgRegs[] = {
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
    { WDC_AD_CFG_SPACE, PCI_CIS, WDC_SIZE_32, WDC_READ_WRITE, "CIS", "CardBus CIS pointer" },
    { WDC_AD_CFG_SPACE, PCI_SVID, WDC_SIZE_16, WDC_READ_WRITE, "SVID", "Sub-system Vendor ID" },
    { WDC_AD_CFG_SPACE, PCI_SDID, WDC_SIZE_16, WDC_READ_WRITE, "SDID", "Sub-system Device ID" },
    { WDC_AD_CFG_SPACE, PCI_EROM, WDC_SIZE_32, WDC_READ_WRITE, "EROM", "Expansion ROM Base Address" },
    { WDC_AD_CFG_SPACE, PCI_CAP, WDC_SIZE_8, WDC_READ_WRITE, "NEW_CAP", "New Capabilities Pointer" },
    { WDC_AD_CFG_SPACE, PCI_ILR, WDC_SIZE_32, WDC_READ_WRITE, "INTLN", "Interrupt Line" },
    { WDC_AD_CFG_SPACE, PCI_IPR, WDC_SIZE_32, WDC_READ_WRITE, "INTPIN", "Interrupt Pin" },
    { WDC_AD_CFG_SPACE, PCI_MGR, WDC_SIZE_32, WDC_READ_WRITE, "MINGNT", "Minimum Required Burst Period" },
    { WDC_AD_CFG_SPACE, PCI_MLR, WDC_SIZE_32, WDC_READ_WRITE, "MAXLAT", "Maximum Latency" },
    /* PLX-specific configuration registers */
    { WDC_AD_CFG_SPACE, 0x40, WDC_SIZE_8, WDC_READ_WRITE, "PMCAPID", "Power Management Capability ID" },
    { WDC_AD_CFG_SPACE, 0x41, WDC_SIZE_8, WDC_READ_WRITE, "PMNEXT", "Power Management Next Capability Pointer" },
    { WDC_AD_CFG_SPACE, 0x42, WDC_SIZE_16, WDC_READ_WRITE, "PMCAP", "Power Management Capabilities" },
    { WDC_AD_CFG_SPACE, 0x44, WDC_SIZE_16, WDC_READ_WRITE, "PMCSR", "Power Management Control/Status" },
    { WDC_AD_CFG_SPACE, 0x48, WDC_SIZE_8, WDC_READ_WRITE, "HS_CAPID", "Hot Swap Capability ID" },
    { WDC_AD_CFG_SPACE, 0x49, WDC_SIZE_8, WDC_READ_WRITE, "HS_NEXT", "Hot Swap Next Capability Pointer" },
    { WDC_AD_CFG_SPACE, 0x4A, WDC_SIZE_8, WDC_READ_WRITE, "HS_CSR", "Hot Swap Control/Status" },
    { WDC_AD_CFG_SPACE, 0x4C, WDC_SIZE_8, WDC_READ_WRITE, "VPD_CAPID", "PCI Vital Product Data Control" },
    { WDC_AD_CFG_SPACE, 0x4D, WDC_SIZE_8, WDC_READ_WRITE, "VPD_NEXT", "PCI Vital Product Next Capability Pointer" },
    { WDC_AD_CFG_SPACE, 0x4E, WDC_SIZE_16, WDC_READ_WRITE, "VPD_ADDR", "PCI Vital Product Data Address" },
    { WDC_AD_CFG_SPACE, 0x50, WDC_SIZE_32, WDC_READ_WRITE, "VPD_DATA", "PCI VPD Data" },
};

/* PLX run-time registers information array */
static const WDC_REG gPLX_M_Regs[] = {
    { PLX_ADDR_REG, 0x00, WDC_SIZE_32, WDC_READ_WRITE, "LAS0RR", "Local Addr Space 0 Range for PCI-to-Local Bus" },
    { PLX_ADDR_REG, 0x04, WDC_SIZE_32, WDC_READ_WRITE, "LAS0BA", "Local BAR (Remap) for PCI-to-Local Addr Space 0" },
    { PLX_ADDR_REG, 0x08, WDC_SIZE_32, WDC_READ_WRITE, "MARBR", "Mode/DMA Arbitration" },
    { PLX_ADDR_REG, 0x0C, WDC_SIZE_8, WDC_READ_WRITE, "BIGEND", "Big/Little Endian Descriptor" },
    { PLX_ADDR_REG, 0x0D, WDC_SIZE_8, WDC_READ_WRITE, "LMISC", "Local Miscellananeous Control" },
    { PLX_ADDR_REG, 0x0E, WDC_SIZE_8, WDC_READ_WRITE, "PROT_AREA", "Serial EEPROM Write-Protected Addr Boundary" },
    { PLX_ADDR_REG, 0x10, WDC_SIZE_32, WDC_READ_WRITE, "EROMRR", "Expansion ROM Range" },
    { PLX_ADDR_REG, 0x14, WDC_SIZE_32, WDC_READ_WRITE, "EROMBA", "EROM Local BAR (Remap) & BREQ0 Control" },
    { PLX_ADDR_REG, 0x18, WDC_SIZE_32, WDC_READ_WRITE, "LBRD0", "Local Addr Space 0 Bus Region Descriptor" },
    { PLX_ADDR_REG, 0x1C, WDC_SIZE_32, WDC_READ_WRITE, "DMRR", "Local Range for PCI initiatior-to-PCI" },
    { PLX_ADDR_REG, 0x20, WDC_SIZE_32, WDC_READ_WRITE, "DMLBAM", "Local Bus Addr for PCI Initiatior-to-PCI Mem" },
    { PLX_ADDR_REG, 0x24, WDC_SIZE_32, WDC_READ_WRITE, "DMLBAI", "Local BAR for PCI Initiatior-to-PCI I/O" },
    { PLX_ADDR_REG, 0x28, WDC_SIZE_32, WDC_READ_WRITE, "DMPBAM", "PCI BAR (Remap) for Initiatior-to-PCI Mem" },
    { PLX_ADDR_REG, 0x2C, WDC_SIZE_32, WDC_READ_WRITE, "DMCFGA", "PCI Config Addr for PCI Initiatior-to-PCI I/O" },
    { PLX_ADDR_REG, 0x30, WDC_SIZE_32, WDC_READ_WRITE, "OPQIS", "Outbound Post Queue Interrupt Status" },
    { PLX_ADDR_REG, 0x34, WDC_SIZE_32, WDC_READ_WRITE, "OPQIM", "Outbound Post Queue Interrupt Mask" },
    { PLX_ADDR_REG, 0x40, WDC_SIZE_32, WDC_READ_WRITE, "IQP", "Inbound Queue Post" },
    { PLX_ADDR_REG, 0x44, WDC_SIZE_32, WDC_READ_WRITE, "OQP", "Outbound Queue Post" },
    { PLX_ADDR_REG, 0x40, WDC_SIZE_32, WDC_READ_WRITE, "MBOX0_NO_I2O", "Mailbox 0 (I2O disabled)" },
    { PLX_ADDR_REG, 0x44, WDC_SIZE_32, WDC_READ_WRITE, "MBOX1_NO_I2O", "Mailbox 1 (I2O disabled)" },
    { PLX_ADDR_REG, 0x78, WDC_SIZE_32, WDC_READ_WRITE, "MBOXO", "Mailbox 0" },
    { PLX_ADDR_REG, 0x7C, WDC_SIZE_32, WDC_READ_WRITE, "MBOX1", "Mailbox 1" },
    { PLX_ADDR_REG, 0x48, WDC_SIZE_32, WDC_READ_WRITE, "MBOX2", "Mailbox 2" },
    { PLX_ADDR_REG, 0x4C, WDC_SIZE_32, WDC_READ_WRITE, "MBOX3", "Mailbox 3" },
    { PLX_ADDR_REG, 0x50, WDC_SIZE_32, WDC_READ_WRITE, "MBOX4", "Mailbox 4" },
    { PLX_ADDR_REG, 0x54, WDC_SIZE_32, WDC_READ_WRITE, "MBOX5", "Mailbox 5" },
    { PLX_ADDR_REG, 0x58, WDC_SIZE_32, WDC_READ_WRITE, "MBOX6", "Mailbox 6" },
    { PLX_ADDR_REG, 0x5C, WDC_SIZE_32, WDC_READ_WRITE, "MBOX7", "Mailbox 7" },
    { PLX_ADDR_REG, 0x60, WDC_SIZE_32, WDC_READ_WRITE, "P2LDBELL", "PCI-to-Local Doorbell" },
    { PLX_ADDR_REG, 0x64, WDC_SIZE_32, WDC_READ_WRITE, "L2PDBELL", "Local-to-PCI Doorbell" },
    { PLX_ADDR_REG, 0x68, WDC_SIZE_32, WDC_READ_WRITE, "INTCSR", "Interrupt Control/Status"  },
    { PLX_ADDR_REG, 0x6C, WDC_SIZE_32, WDC_READ_WRITE, "CNTRL", "Serial EEPROM/User I/O/Init Ctr & PCI Cmd Codes" },
    { PLX_ADDR_REG, 0x70, WDC_SIZE_32, WDC_READ_WRITE, "PCIHIDR", "PCI Hardcoded Configuration ID" },
    { PLX_ADDR_REG, 0x74, WDC_SIZE_16, WDC_READ_WRITE, "PCIHREV", "PCI Hardcoded Revision ID" },
    { PLX_ADDR_REG, 0x80, WDC_SIZE_32, WDC_READ_WRITE, "DMAMODE0", "DMA Channel 0 Mode" },
    { PLX_ADDR_REG, 0x84, WDC_SIZE_32, WDC_READ_WRITE, "DMAPADR0", "DMA Channel 0 PCI Address" },
    { PLX_ADDR_REG, 0x88, WDC_SIZE_32, WDC_READ_WRITE, "DMALADR0", "DMA Channel 0 Local Address" },
    { PLX_ADDR_REG, 0x8C, WDC_SIZE_32, WDC_READ_WRITE, "DMASIZ0", "DMA Channel 0 Transfer Size (bytes)" },
    { PLX_ADDR_REG, 0x90, WDC_SIZE_32, WDC_READ_WRITE, "DMADPR0", "DMA Channel 0 Descriptor Pointer" },
    { PLX_ADDR_REG, 0x94, WDC_SIZE_32, WDC_READ_WRITE, "DMAMODE1", "DMA Channel 1 Mode" },
    { PLX_ADDR_REG, 0x98, WDC_SIZE_32, WDC_READ_WRITE, "DMAPADR1", "DMA Channel 1 PCI Address" },
    { PLX_ADDR_REG, 0x9C, WDC_SIZE_32, WDC_READ_WRITE, "DMALADR1", "DMA Channel 1 Local Address" },
    { PLX_ADDR_REG, 0xA0, WDC_SIZE_32, WDC_READ_WRITE, "DMASIZ1", "DMA Channel 1 Transfer Size (bytes)" },
    { PLX_ADDR_REG, 0xA4, WDC_SIZE_32, WDC_READ_WRITE, "DMADPR1", "DMA Channel 1 Descriptor Pointer" },
    { PLX_ADDR_REG, 0xA8, WDC_SIZE_8, WDC_READ_WRITE, "DMACSR0", "DMA Channel 0 Command/Status" },
    { PLX_ADDR_REG, 0xA9, WDC_SIZE_8, WDC_READ_WRITE, "DMACSR1", "DMA Channel 1 Command/Status" },
    { PLX_ADDR_REG, 0xAC, WDC_SIZE_32, WDC_READ_WRITE, "DMAARB", "DMA Arbitration" },
    { PLX_ADDR_REG, 0xB0, WDC_SIZE_32, WDC_READ_WRITE, "DMATHR", "DMA Threshold (Channel 0 only)" },
    { PLX_ADDR_REG, 0xB4, WDC_SIZE_32, WDC_READ_WRITE, "DMADAC0", "DMA 0 PCI Dual Address Cycle Address" },
    { PLX_ADDR_REG, 0xB8, WDC_SIZE_32, WDC_READ_WRITE, "DMADAC1", "DMA 1 PCI Dual Address Cycle Address" },
    { PLX_ADDR_REG, 0xC0, WDC_SIZE_32, WDC_READ_WRITE, "MQCR", "Messaging Queue Configuration" },
    { PLX_ADDR_REG, 0xC4, WDC_SIZE_32, WDC_READ_WRITE, "QBAR", "Queue Base Address" },
    { PLX_ADDR_REG, 0xC8, WDC_SIZE_32, WDC_READ_WRITE, "IFHPR", "Inbound Free Head Pointer" },
    { PLX_ADDR_REG, 0xCC, WDC_SIZE_32, WDC_READ_WRITE, "IFTPR", "Inbound Free Tail Pointer" },
    { PLX_ADDR_REG, 0xD0, WDC_SIZE_32, WDC_READ_WRITE, "IPHPR", "Inbound Post Head Pointer" },
    { PLX_ADDR_REG, 0xD4, WDC_SIZE_32, WDC_READ_WRITE, "IPTPR", "Inbound Post Tail Pointer" },
    { PLX_ADDR_REG, 0xD8, WDC_SIZE_32, WDC_READ_WRITE, "OFHPR", "Outbound Free Head Pointer" },
    { PLX_ADDR_REG, 0xDC, WDC_SIZE_32, WDC_READ_WRITE, "OFTPR", "Outbound Free Tail Pointer" },
    { PLX_ADDR_REG, 0xE0, WDC_SIZE_32, WDC_READ_WRITE, "OPHPR", "Outbound Post Head Pointer" },
    { PLX_ADDR_REG, 0xE4, WDC_SIZE_32, WDC_READ_WRITE, "OPTPR", "Outbound Post Tail Pointer" },
    { PLX_ADDR_REG, 0xE8, WDC_SIZE_32, WDC_READ_WRITE, "QSR", "Queue Status/Control" },
    { PLX_ADDR_REG, 0xF0, WDC_SIZE_32, WDC_READ_WRITE, "LAS1RR", "Local Addr Space 1 Range for PCI-to-Local Bus" },
    { PLX_ADDR_REG, 0xF4, WDC_SIZE_32, WDC_READ_WRITE, "LAS1BA", "Local Addr Space 1 Local BAR (Remap)" },
    { PLX_ADDR_REG, 0xF8, WDC_SIZE_32, WDC_READ_WRITE, "LBRD1", "Local Addr Space 1 Bus Region Descriptor" },
    { PLX_ADDR_REG, 0xFC, WDC_SIZE_32, WDC_READ_WRITE, "DMDAC", "PCI Initiatior PCI Dual Address Cycle" },
};

static const WDC_REG gPLX_T_Regs[] = {
    { PLX_ADDR_REG, 0x00, WDC_SIZE_32, WDC_READ_WRITE, "LAS0RR", "Local Addr Space 0 Range" },
    { PLX_ADDR_REG, 0x04, WDC_SIZE_32, WDC_READ_WRITE, "LAS1RR", "Local Addr Space 1 Range" },
    { PLX_ADDR_REG, 0x08, WDC_SIZE_32, WDC_READ_WRITE, "LAS2RR", "Local Addr Space 2 Range" },
    { PLX_ADDR_REG, 0x0C, WDC_SIZE_32, WDC_READ_WRITE, "LAS3RR", "Local Addr Space 3 Range" },
    { PLX_ADDR_REG, 0x10, WDC_SIZE_32, WDC_READ_WRITE, "EROMRR", "Expansion ROM Range" },
    { PLX_ADDR_REG, 0x14, WDC_SIZE_32, WDC_READ_WRITE, "LAS0BA", "Local Addr Space 0 Local BAR (Remap)" },
    { PLX_ADDR_REG, 0x18, WDC_SIZE_32, WDC_READ_WRITE, "LAS1BA", "Local Addr Space 1 Local BAR (Remap)" },
    { PLX_ADDR_REG, 0x1C, WDC_SIZE_32, WDC_READ_WRITE, "LAS2BA", "Local Addr Space 2 Local BAR (Remap)" },
    { PLX_ADDR_REG, 0x20, WDC_SIZE_32, WDC_READ_WRITE, "LAS3BA", "Local Addr Space 3 Local BAR (Remap)" },
    { PLX_ADDR_REG, 0x24, WDC_SIZE_32, WDC_READ_WRITE, "EROMBA", "Expansion ROM Local BAR (Remap)" },
    { PLX_ADDR_REG, 0x28, WDC_SIZE_32, WDC_READ_WRITE, "LAS0BRD", "Local Addr Space 0 Bus Region Descriptors" },
    { PLX_ADDR_REG, 0x2C, WDC_SIZE_32, WDC_READ_WRITE, "LAS1BRD", "Local Addr Space 1 Bus Region Descriptors" },
    { PLX_ADDR_REG, 0x30, WDC_SIZE_32, WDC_READ_WRITE, "LAS2BRD", "Local Addr Space 2 Bus Region Descriptors" },
    { PLX_ADDR_REG, 0x34, WDC_SIZE_32, WDC_READ_WRITE, "LAS3BRD", "Local Addr Space 3 Bus Region Descriptors" },
    { PLX_ADDR_REG, 0x38, WDC_SIZE_32, WDC_READ_WRITE, "EROMBRD", "Expansion ROM Bus Region Descriptors" },
    { PLX_ADDR_REG, 0x3C, WDC_SIZE_32, WDC_READ_WRITE, "CS0BASE", "Chip Select 0 Base Address" },
    { PLX_ADDR_REG, 0x40, WDC_SIZE_32, WDC_READ_WRITE, "CS1BASE", "Chip Select 1 Base Address" },
    { PLX_ADDR_REG, 0x44, WDC_SIZE_32, WDC_READ_WRITE, "CS2BASE", "Chip Select 2 Base Address" },
    { PLX_ADDR_REG, 0x48, WDC_SIZE_32, WDC_READ_WRITE, "CS3BASE", "Chip Select 3 Base Address" },
    { PLX_ADDR_REG, 0x4C, WDC_SIZE_16, WDC_READ_WRITE, "INTCSR", "Interrupt Control/Status" },
    { PLX_ADDR_REG, 0x4E, WDC_SIZE_16, WDC_READ_WRITE, "PROT_AREA", "Serial EEPROM Write-Protected Addr Boundary" },
    { PLX_ADDR_REG, 0x50, WDC_SIZE_32, WDC_READ_WRITE, "CNTRL", "PCI Target Response; Serial EEPROM; Init Ctr" },
    { PLX_ADDR_REG, 0x54, WDC_SIZE_32, WDC_READ_WRITE, "GPIOC", "General Purpose I/O Control" },
    { PLX_ADDR_REG, 0x70, WDC_SIZE_32, WDC_READ_WRITE, "PMDATASEL", "Hidden 1 Power Management Data Select" },
    { PLX_ADDR_REG, 0x74, WDC_SIZE_32, WDC_READ_WRITE, "PMDATASCALE", "Hidden 2 Power Management Data Scale" },
};

/*************************************************************
  Static functions prototypes
 *************************************************************/
static void SetAddrSpace(WDC_DEVICE_HANDLE hDev, PLX_ADDR *pAddrSpace);

static BOOL DMAOpenGetInput(PLX_DIAG_DMA *pDma, BOOL *pfPolling,
    UINT32 *pu32LocalAddr, PDWORD pdwBytes, PDWORD pdwOptions);

/*************************************************************
  Functions implementation
 *************************************************************/

/* -----------------------------------------------
    Device find, open and close
   ----------------------------------------------- */
/* Find and open a PLX device */
WDC_DEVICE_HANDLE PLX_DIAG_DeviceFindAndOpen(DWORD dwVendorId, DWORD dwDeviceId,
    BOOL fIsMaster)
{
    WDC_DEVICE_HANDLE pDeviceHandle=NULL;
    WD_PCI_SLOT slot;

    if (!PLX_DIAG_DeviceFind(dwVendorId, dwDeviceId, &slot))
        return NULL;

    if ((PLX_DeviceOpenBySlot(&pDeviceHandle, &slot, fIsMaster)))
        return NULL;

    return pDeviceHandle;
}

/* Find a PLX device */
BOOL PLX_DIAG_DeviceFind(DWORD dwVendorId, DWORD dwDeviceId, WD_PCI_SLOT *pSlot)
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
        PLX_DIAG_ERR("PLX_DIAG_DeviceFind: Failed scanning the PCI bus.\n"
            "Error: 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        return FALSE;
    }

    dwNumDevices = scanResult.dwNumDevices;
    if (!dwNumDevices)
    {
        printf("No matching PLX device was found for search criteria "
            "(Vendor ID 0x%lX, Device ID 0x%lX)\n",
            dwVendorId, dwDeviceId);

        return FALSE;
    }

    printf("\n");
    printf("Found %ld matching device%s [ Vendor ID 0x%lX%s, "
        "Device ID 0x%lX%s ]:\n",
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

/* -----------------------------------------------
    Read/write memory and I/O addresses
   ----------------------------------------------- */
/* Read/write address menu options */
enum {
    PLX_DIAG_MENU_RW_ADDR_SET_ADDR_SPACE = 1,
    PLX_DIAG_MENU_RW_ADDR_SET_MODE,
    PLX_DIAG_MENU_RW_ADDR_SET_TRANS_TYPE,
    PLX_DIAG_MENU_RW_ADDR_READ_OFFSET,
    PLX_DIAG_MENU_RW_ADDR_WRITE_OFFSET,
    PLX_DIAG_MENU_RW_ADDR_READ_LOCAL,
    PLX_DIAG_MENU_RW_ADDR_WRITE_LOCAL,
    PLX_DIAG_MENU_RW_ADDR_EXIT = DIAG_EXIT_MENU,
};

#define ACTIVE_ADDR_SPACE_NEEDS_INIT 0xFF

/* Read/write memory or I/O space address menu */
void PLX_DIAG_MenuReadWriteAddr(WDC_DEVICE_HANDLE hDev)
{
    DWORD option;
    static PLX_ADDR addrSpace = ACTIVE_ADDR_SPACE_NEEDS_INIT;
    static WDC_ADDR_MODE mode = WDC_MODE_32;
    static BOOL fBlock = FALSE;

    /* Initialize active address space */
    if (ACTIVE_ADDR_SPACE_NEEDS_INIT == addrSpace)
    {
        DWORD dwNumAddrSpaces = PLX_GetNumAddrSpaces(hDev);

        /* Find the first active address space */
        for (addrSpace = 0; addrSpace < dwNumAddrSpaces; addrSpace++)
        {
            if (WDC_AddrSpaceIsActive(hDev, addrSpace))
                break;
        }

        /* Sanity check */
        if (addrSpace == dwNumAddrSpaces)
        {
            PLX_DIAG_ERR("PLX_DIAG_MenuReadWriteAddr: Error - no active "
                "address spaces found\n");
            addrSpace = ACTIVE_ADDR_SPACE_NEEDS_INIT;
            return;
        }
    }

    do
    {
        printf("\n");
        printf("Read/write the device's memory and I/O ranges\n");
        printf("---------------------------------------------\n");
        printf("%d. Change active address space for read/write "
            "(currently: BAR %ld)\n", PLX_DIAG_MENU_RW_ADDR_SET_ADDR_SPACE,
            addrSpace);
        printf("%d. Change active read/write mode (currently: %s)\n",
            PLX_DIAG_MENU_RW_ADDR_SET_MODE,
            (WDC_MODE_8 == mode) ? "8 bit" : (WDC_MODE_16 == mode) ? "16 bit" :
            (WDC_MODE_32 == mode) ? "32 bit" : "64 bit");
        printf("%d. Toggle active transfer type (currently: %s)\n",
            PLX_DIAG_MENU_RW_ADDR_SET_TRANS_TYPE,
            fBlock ? "block transfers" : "non-block transfers");
        printf("%d. Read from an offset in the active address space\n",
            PLX_DIAG_MENU_RW_ADDR_READ_OFFSET);
        printf("%d. Write to an offset in the active address space\n",
            PLX_DIAG_MENU_RW_ADDR_WRITE_OFFSET);
        printf("%d. Read from a local address in the active address space\n",
            PLX_DIAG_MENU_RW_ADDR_READ_LOCAL);
        printf("%d. Write to a local address in the active address space\n",
            PLX_DIAG_MENU_RW_ADDR_WRITE_LOCAL);
        printf("%d. Exit menu\n", PLX_DIAG_MENU_RW_ADDR_EXIT);
        printf("\n");

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option,
            PLX_DIAG_MENU_RW_ADDR_WRITE_LOCAL))
        {
            continue;
        }

        switch (option)
        {
        case PLX_DIAG_MENU_RW_ADDR_EXIT: /* Exit menu */
            break;
        case PLX_DIAG_MENU_RW_ADDR_SET_ADDR_SPACE: /* Set active address space
                                                    * for read/write address
                                                    * requests */
        {
            SetAddrSpace(hDev, &addrSpace);
            break;
        }
        case PLX_DIAG_MENU_RW_ADDR_SET_MODE: /* Set active mode for read/write
                                              * address requests */
            WDC_DIAG_SetMode(&mode);
            break;
        case PLX_DIAG_MENU_RW_ADDR_SET_TRANS_TYPE: /* Toggle active transfer
                                                    * type */
            fBlock = !fBlock;
            break;
        case PLX_DIAG_MENU_RW_ADDR_READ_OFFSET:  /* Read from a memory or I/O
                                                  * address offset */
        case PLX_DIAG_MENU_RW_ADDR_WRITE_OFFSET: /* Write to a memory or I/O
                                                  * address offset */
        {
            WDC_DIRECTION direction =
                (PLX_DIAG_MENU_RW_ADDR_READ_OFFSET == option) ?
                WDC_READ : WDC_WRITE;

            if (fBlock)
                WDC_DIAG_ReadWriteBlock(hDev, direction, addrSpace);
            else
                WDC_DIAG_ReadWriteAddr(hDev, direction, addrSpace, mode);

            break;
        }
        case PLX_DIAG_MENU_RW_ADDR_READ_LOCAL:  /* Read from a local memory or
                                                 * I/O address */
        case PLX_DIAG_MENU_RW_ADDR_WRITE_LOCAL: /* Write to a local memory or
                                                 * I/O address */
        {
            WDC_DIRECTION direction =
                (PLX_DIAG_MENU_RW_ADDR_READ_LOCAL == option) ?
                WDC_READ : WDC_WRITE;

            if (fBlock)
                PLX_DIAG_ReadWriteAddrLocalBlock(hDev, direction, addrSpace);
            else
                PLX_DIAG_ReadWriteAddrLocal(hDev, direction, addrSpace, mode);

            break;
        }
        }
    } while (PLX_DIAG_MENU_RW_ADDR_EXIT != option);
}

static void SetAddrSpace(WDC_DEVICE_HANDLE hDev, PLX_ADDR *pAddrSpace)
{
    PLX_ADDR addrSpace;
    DWORD dwNumAddrSpaces = PLX_GetNumAddrSpaces(hDev);
    PLX_ADDR_SPACE_INFO addrSpaceInfo;

    printf("\n");
    printf("Select an active address space:\n");
    printf("-------------------------------\n");

    for (addrSpace = 0; addrSpace < dwNumAddrSpaces; addrSpace++)
    {
        BZERO(addrSpaceInfo);
        addrSpaceInfo.dwAddrSpace = addrSpace;
        if (!PLX_GetAddrSpaceInfo(hDev, &addrSpaceInfo))
        {
            PLX_DIAG_ERR("SetAddrSpace: Error - Failed to get address space "
                "information: %s", PLX_GetLastErr());
            return;
        }

        printf("%ld. %-*s %-*s %s\n",
            addrSpace + 1,
            MAX_NAME_DISPLAY, addrSpaceInfo.sName,
            MAX_TYPE - 1, addrSpaceInfo.sType,
            addrSpaceInfo.sDesc);
    }
    printf("\n");

    if (DIAG_INPUT_SUCCESS != DIAG_InputNum((PVOID)&addrSpace,
        "Enter option", FALSE, sizeof(addrSpace), 1, dwNumAddrSpaces))
    {
        return;
    }

    addrSpace--;
    if (!WDC_AddrSpaceIsActive(hDev, addrSpace))
    {
        printf("You have selected an inactive address space\n");
        return;
    }

    *pAddrSpace = addrSpace;
}

void PLX_DIAG_ReadWriteAddrLocal(WDC_DEVICE_HANDLE hDev,
    WDC_DIRECTION direction, PLX_ADDR addrSpace, WDC_ADDR_MODE mode)
{
    DWORD dwStatus;
    DWORD dwLocalAddr;
    BYTE bData = 0;
    WORD wData = 0;
    UINT32 u32Data = 0;
    UINT64 u64Data = 0;

    if (!hDev)
    {
        PLX_DIAG_ERR("PLX_DIAG_ReadWriteAddrLocal: Error - NULL WDC device "
            "handle\n");
        return;
    }

    if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD(&dwLocalAddr,
        "Enter local address", TRUE, 0, 0))
    {
        return;
    }

    if ((WDC_WRITE == direction) &&
        !WDC_DIAG_InputWriteData((WDC_MODE_8 == mode) ? (PVOID)&bData :
        (WDC_MODE_16 == mode) ? (PVOID)&wData :
        (WDC_MODE_32 == mode) ? (PVOID)&u32Data : (PVOID)&u64Data,
        WDC_ADDR_MODE_TO_SIZE(mode)))
    {
        return;
    }

    switch (mode)
    {
    case WDC_MODE_8:
        dwStatus = (WDC_READ == direction) ?
            PLX_ReadAddrLocal8(hDev, addrSpace, dwLocalAddr, &bData) :
            PLX_WriteAddrLocal8(hDev, addrSpace, dwLocalAddr, bData);
        if (WD_STATUS_SUCCESS == dwStatus)
        {
            printf("%s 0x%X %s local address 0x%lX in BAR %ld\n",
                (WDC_READ == direction) ? "Read" : "Wrote", (UINT32)bData,
                (WDC_READ == direction) ? "from" : "to", dwLocalAddr,
                addrSpace);
        }
        break;
    case WDC_MODE_16:
        dwStatus = (WDC_READ == direction) ?
            PLX_ReadAddrLocal16(hDev, addrSpace, dwLocalAddr, &wData) :
            PLX_WriteAddrLocal16(hDev, addrSpace, dwLocalAddr, wData);
        if (WD_STATUS_SUCCESS == dwStatus)
        {
            printf("%s 0x%hX %s local address 0x%lX in BAR %ld\n",
                (WDC_READ == direction) ? "Read" : "Wrote", wData,
                (WDC_READ == direction) ? "from" : "to", dwLocalAddr,
                addrSpace);
        }
        break;
    case WDC_MODE_32:
        dwStatus = (WDC_READ == direction) ?
            PLX_ReadAddrLocal32(hDev, addrSpace, dwLocalAddr, &u32Data) :
            PLX_WriteAddrLocal32(hDev, addrSpace, dwLocalAddr, u32Data);
        if (WD_STATUS_SUCCESS == dwStatus)
        {
            printf("%s 0x%X %s local address 0x%lX in BAR %ld\n",
                (WDC_READ == direction) ? "Read" : "Wrote", u32Data,
                (WDC_READ == direction) ? "from" : "to", dwLocalAddr,
                addrSpace);
        }
        break;
    case WDC_MODE_64:
        dwStatus = (WDC_READ == direction) ?
            PLX_ReadAddrLocal64(hDev, addrSpace, dwLocalAddr, &u64Data) :
            PLX_WriteAddrLocal64(hDev, addrSpace, dwLocalAddr, u64Data);
        if (WD_STATUS_SUCCESS == dwStatus)
        {
            printf("%s 0x%"PRI64"X %s local address 0x%lX in BAR %ld\n",
                (WDC_READ == direction) ? "Read" : "Wrote", u64Data,
                (WDC_READ == direction) ? "from" : "to", dwLocalAddr,
                addrSpace);
        }
        break;
    default:
        PLX_DIAG_ERR("PLX_DIAG_ReadWriteAddrLocal: Error - Invalid mode (%d)\n",
            mode);
        return;
    }

    if (WD_STATUS_SUCCESS != dwStatus)
    {
        printf("Failed to %s local address 0x%lX in BAR %ld. "
            "Error 0x%lx - %s\n", (WDC_READ == direction) ? "read from" :
            "write to", dwLocalAddr, addrSpace, dwStatus, Stat2Str(dwStatus));
    }
}

void PLX_DIAG_ReadWriteAddrLocalBlock(WDC_DEVICE_HANDLE hDev,
    WDC_DIRECTION direction, PLX_ADDR addrSpace)
{
    DWORD dwStatus;
    DWORD dwLocalAddr, dwBytes;
    const CHAR *sDir = (WDC_READ == direction) ? "read" : "write";
    PVOID pBuf = NULL;
    WDC_ADDR_MODE mode;
    WDC_ADDR_RW_OPTIONS options;
    BOOL fAutoInc;

    if (!hDev)
    {
        PLX_DIAG_ERR("PLX_DIAG_ReadWriteAddrLocalBlock: Error - NULL WDC "
            "device handle\n");
        return;
    }

    if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD(&dwLocalAddr,
        "Enter local address", TRUE, 0, 0))
    {
        return;
    }

    sprintf(gsInput, "Enter number of bytes to %s", sDir);
    if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD(&dwBytes, gsInput, TRUE, 0, 0))
        return;

    if (!dwBytes)
    {
        printf("The number of bytes to %s must be bigger than 0", sDir);
        goto Exit;
    }

    pBuf = malloc(dwBytes);
    if (!pBuf)
    {
        PLX_DIAG_ERR("PLX_DIAG_ReadWriteAddrLocalBlock: Failed allocating %s "
            "data buffer\n", sDir);
        goto Exit;
    }
    memset(pBuf, 0, dwBytes);

    if (WDC_WRITE == direction)
    {
        printf("Enter data to write (hex format): 0x");
        if (!DIAG_GetHexBuffer(pBuf, dwBytes))
            goto Exit;
    }

    if (!WDC_DIAG_SetMode(&mode))
        goto Exit;

    sprintf(gsInput, "Do you wish to increment the address after each %s block "
        "(%ld bytes)\n(0 - No, Otherwise - Yes)? ",
        sDir, WDC_ADDR_MODE_TO_SIZE(mode));
    if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD(&fAutoInc, gsInput,
        FALSE, 0, 0))
    {
        goto Exit;
    }

    options = fAutoInc ? 0 : WDC_ADDR_RW_NO_AUTOINC;

    dwStatus = direction == WDC_READ ?
        PLX_ReadAddrLocalBlock(hDev, addrSpace, dwLocalAddr, dwBytes, pBuf,
            mode, options) :
        PLX_WriteAddrLocalBlock(hDev, addrSpace, dwLocalAddr, dwBytes, pBuf,
            mode, options);

    if (WD_STATUS_SUCCESS == dwStatus)
    {
        printf("%s %ld bytes %s local address 0x%lX\n",
            (WDC_READ == direction) ? "Read" : "Wrote", dwBytes,
            (WDC_READ == direction) ? "from" : "to", dwLocalAddr);

        if (WDC_READ == direction)
        {
            printf("Data read from local address 0x%lX (hex format):\n",
                dwLocalAddr);
            DIAG_PrintHexBuffer(pBuf, dwBytes, FALSE);
        }
    }
    else
    {
        printf("Failed to %s %ld bytes %s local address 0x%lX. "
            "Error 0x%lx - %s\n", sDir, dwBytes, (WDC_READ == direction) ?
            "from" : "to", dwLocalAddr, dwStatus, Stat2Str(dwStatus));
    }

Exit:
    if (pBuf)
        free(pBuf);

    printf("\n");
    printf("Press ENTER to return to the menu\n");
    fgets(gsInput, sizeof(gsInput), stdin);
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
void PLX_DIAG_MenuReadWriteCfgSpace(WDC_DEVICE_HANDLE hDev)
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
        case PLX_DIAG_MENU_RW_CFG_SPACE_EXIT: /* Exit menu */
            break;
        case PLX_DIAG_MENU_RW_CFG_SPACE_READ_OFFSET: /* Read from a
                                                      * configuration space
                                                      * offset */
            WDC_DIAG_ReadWriteBlock(hDev, WDC_READ, WDC_AD_CFG_SPACE);
            break;
        case PLX_DIAG_MENU_RW_CFG_SPACE_WRITE_OFFSET: /* Write to a
                                                       * configuration space
                                                       * offset */
            WDC_DIAG_ReadWriteBlock(hDev, WDC_WRITE, WDC_AD_CFG_SPACE);
            break;
        case PLX_DIAG_MENU_RW_CFG_SPACE_READ_ALL_REGS:
            WDC_DIAG_ReadRegsAll(hDev, pCfgRegs, dwCfgRegsNum, TRUE, FALSE);
            break;
        case PLX_DIAG_MENU_RW_CFG_SPACE_READ_REG:  /* Read from a configuration
                                                    * register */
            WDC_DIAG_ReadWriteReg(hDev, pCfgRegs, dwCfgRegsNum, WDC_READ, TRUE);
            break;
        case PLX_DIAG_MENU_RW_CFG_SPACE_WRITE_REG: /* Write to a configuration
                                                    * register */
            WDC_DIAG_ReadWriteReg(hDev, pCfgRegs, dwCfgRegsNum, WDC_WRITE,
                TRUE);
            break;
        }
    } while (PLX_DIAG_MENU_RW_CFG_SPACE_EXIT != option);
}

/* -----------------------------------------------
    Read/write the run-time registers
   ----------------------------------------------- */
/* Read/write the run-time registers menu options */
enum {
    PLX_DIAG_MENU_RW_REGS_READ_ALL = 1,
    PLX_DIAG_MENU_RW_REGS_READ_REG,
    PLX_DIAG_MENU_RW_REGS_WRITE_REG,
    PLX_DIAG_MENU_RW_REGS_EXIT = DIAG_EXIT_MENU,
};

/* Display read/write run-time registers menu */
void PLX_DIAG_MenuReadWriteRegs(WDC_DEVICE_HANDLE hDev)
{
    DWORD option;
    const WDC_REG *pRegs = PLX_IsMaster(hDev) ? gPLX_M_Regs : gPLX_T_Regs;
    DWORD dwRegsNum = (PLX_IsMaster(hDev) ?
        sizeof(gPLX_M_Regs) : sizeof(gPLX_T_Regs)) / sizeof(*pRegs);

    do {
        /* Display pre-defined registers' information */
        printf("\n");
        printf("PLX run-time registers:\n");
        printf("-----------------------\n");
        WDC_DIAG_RegsInfoPrint(hDev, pRegs, dwRegsNum, WDC_DIAG_REG_PRINT_ALL, FALSE);

        printf("\n");
        printf("Read/write the PLX run-time registers\n");
        printf("--------------------------------------\n");
        printf("%d. Read all run-time registers defined for the device "
            "(see list above)\n", PLX_DIAG_MENU_RW_REGS_READ_ALL);
        printf("%d. Read from a specific register\n",
            PLX_DIAG_MENU_RW_REGS_READ_REG);
        printf("%d. Write to a specific register\n",
            PLX_DIAG_MENU_RW_REGS_WRITE_REG);
        printf("%d. Exit menu\n", PLX_DIAG_MENU_RW_REGS_EXIT);
        printf("\n");

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option,
            PLX_DIAG_MENU_RW_REGS_WRITE_REG))
        {
            printf("\n");
            printf("Press ENTER to return to the menu\n");
            fgets(gsInput, sizeof(gsInput), stdin);
            continue;
        }

        switch (option)
        {
        case PLX_DIAG_MENU_RW_REGS_EXIT: /* Exit menu */
            break;
        case PLX_DIAG_MENU_RW_REGS_READ_ALL:
            WDC_DIAG_ReadRegsAll(hDev, pRegs, dwRegsNum, FALSE, FALSE);
            break;
        case PLX_DIAG_MENU_RW_REGS_READ_REG:  /* Read from a register */
            WDC_DIAG_ReadWriteReg(hDev, pRegs, dwRegsNum, WDC_READ, FALSE);
            break;
        case PLX_DIAG_MENU_RW_REGS_WRITE_REG: /* Write to a register */
            WDC_DIAG_ReadWriteReg(hDev, pRegs, dwRegsNum, WDC_WRITE, FALSE);
            break;
        }
    } while (PLX_DIAG_MENU_RW_REGS_EXIT != option);
}

/* -----------------------------------------------
    Direct Memory Access (DMA)
   ----------------------------------------------- */
/* DMA menu options */
enum {
    PLX_DIAG_MENU_DMA_OPEN_CH_0 = 1,
    PLX_DIAG_MENU_DMA_OPEN_CH_1,
    PLX_DIAG_MENU_DMA_CLOSE = 1,
    PLX_DIAG_MENU_DMA_EXIT = DIAG_EXIT_MENU,
};

/* DMA menu */
void PLX_DIAG_MenuDma(WDC_DEVICE_HANDLE hDev, PLX_DIAG_DMA *pDma,
    PLX_INT_HANDLER MasterDiagDmaIntHandler)
{
    DWORD option;

    do
    {
        printf("\n");
        printf("Direct Memory Access (DMA)\n");
        printf("---------------------------\n");
        if (pDma->hDma)
        {
            printf("%d. Close DMA handle\n", PLX_DIAG_MENU_DMA_CLOSE);
        }
        else
        {
            printf("%d. Open DMA - channel %d\n", PLX_DIAG_MENU_DMA_OPEN_CH_0,
                PLX_DMA_CHANNEL_0);
            printf("%d. Open DMA - channel %d\n", PLX_DIAG_MENU_DMA_OPEN_CH_1,
                PLX_DMA_CHANNEL_1);
        }
        printf("%d. Exit menu\n", PLX_DIAG_MENU_DMA_EXIT);
        printf("\n");

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option,
            pDma->hDma ? PLX_DIAG_MENU_DMA_CLOSE : PLX_DIAG_MENU_DMA_OPEN_CH_1))
        {
            continue;
        }

        /* If DMA is open and user selected close option - close open DMA
         * channels */
        if (pDma->hDma && (PLX_DIAG_MENU_DMA_CLOSE == option))
        {
            PLX_DIAG_DMAClose(hDev, pDma);
            continue;
        }

        switch (option)
        {
        case PLX_DIAG_MENU_DMA_EXIT: /* Exit menu */
            break;
        case PLX_DIAG_MENU_DMA_OPEN_CH_0: /* Open DMA channel 0 */
            PLX_DIAG_DMAOpen(hDev, pDma, MasterDiagDmaIntHandler,
                PLX_DMA_CHANNEL_0);
            break;
        case PLX_DIAG_MENU_DMA_OPEN_CH_1: /* Open DMA channel 1 */
            PLX_DIAG_DMAOpen(hDev, pDma, MasterDiagDmaIntHandler,
                PLX_DMA_CHANNEL_1);
            break;
        }
    } while (PLX_DIAG_MENU_DMA_EXIT != option);
}

void PLX_DIAG_DMAOpen(WDC_DEVICE_HANDLE hDev, PLX_DIAG_DMA *pDma,
    PLX_INT_HANDLER MasterDiagDmaIntHandler, PLX_DMA_CHANNEL dmaChannel)
{
    DWORD dwStatus;
    UINT32 u32LocalAddr;
    WDC_ADDR_MODE mode = WDC_MODE_32; /* Local bus width */
    DWORD dwBytes;
    DWORD dwOptions;
    BOOL fPolling;

    /* Get input for user */
    if (!DMAOpenGetInput(pDma, &fPolling, &u32LocalAddr, &dwBytes, &dwOptions))
        return;

    /* Allocate buffer for Scatter/Gather DMA (if selected) */
    if (pDma->fSG)
    {
        pDma->pBuf = malloc(dwBytes);
        if (!pDma->pBuf)
        {
            PLX_DIAG_ERR("PLX_DIAG_DMAOpen: Failed allocating Scatter/Gather "
                "DMA data buffer\n");
            return;
        }
        memset(pDma->pBuf, 0, dwBytes);
    }
    else
    {
        dwOptions |= DMA_KERNEL_BUFFER_ALLOC;
    }

    /* Open DMA for selected channel */
    dwStatus = PLX_DMAOpen(hDev, u32LocalAddr, &pDma->pBuf, dwOptions,
        dwBytes, mode, dmaChannel, &pDma->hDma);

    printf("\n");
    if (WD_STATUS_SUCCESS == dwStatus)
    {
        printf("DMA for channel %d opened successfully (handle [%p])\n",
            dmaChannel, pDma->hDma);
    }
    else
    {
        printf("Failed to open DMA for channel %d. Error 0x%lx - %s\n",
            dmaChannel, dwStatus, Stat2Str(dwStatus));
        goto Error;
    }

    /* Enable DMA interrupts (if not polling) */
    if (!fPolling && !PLX_IntIsEnabled(hDev))
    {
        dwStatus = PLX_IntEnable(hDev, MasterDiagDmaIntHandler, pDma->hDma);
        printf("\n");
        if (WD_STATUS_SUCCESS == dwStatus)
        {
            printf("DMA interrupts enabled\n");
        }
        else
        {
            printf("Failed enabling DMA interrupts. Error 0x%lx - %s\n",
                dwStatus, Stat2Str(dwStatus));
            goto Error;
        }
    }

    /* Start DMA */
    PLX_DMAStart(hDev, pDma->hDma);
    printf("Started DMA on channel %d\n", dmaChannel);

    /* Poll for completion (if polling selected) */
    if (fPolling)
    {
        printf("\nPolling hardware for channel %d DMA completion ...\n",
            dmaChannel);
        if (PLX_DMAPollCompletion(hDev, pDma->hDma))
            printf("Channel %d DMA completed\n", dmaChannel);
        else
            printf("Channel %d DMA aborted\n", dmaChannel);
    }

    return;

Error:
    PLX_DIAG_DMAClose(hDev, pDma);
}

void PLX_DIAG_DMAClose(WDC_DEVICE_HANDLE hDev, PLX_DIAG_DMA *pDma)
{
    DWORD dwStatus;

    if (!pDma)
        return;

    if (PLX_IntIsEnabled(hDev))
    {
        dwStatus = PLX_IntDisable(hDev);
        printf("DMA interrupts disable%s\n",
            (WD_STATUS_SUCCESS == dwStatus) ? "d" : " failed");
    }

    if (pDma->hDma)
    {
        PLX_DMAClose(hDev, pDma->hDma);
        printf("DMA closed (handle [%p])\n", pDma->hDma);
    }

    if (pDma->fSG && pDma->pBuf)
        free(pDma->pBuf);

    BZERO(*pDma);
}

static BOOL DMAOpenGetInput(PLX_DIAG_DMA *pDma, BOOL *pfPolling,
    UINT32 *pu32LocalAddr, PDWORD pdwBytes, PDWORD pdwOptions)
{
    DWORD tmp;

    printf("\n");
    printf("Select DMA allocation type:\n");
    printf("1. Scatter/Gather\n");
    printf("2. Contiguous Buffer\n");
    printf("%d. Cancel\n", DIAG_EXIT_MENU);
    if ((DIAG_INPUT_SUCCESS != DIAG_GetMenuOption(&tmp, 2)) ||
        (DIAG_EXIT_MENU == tmp))
    {
        return FALSE;
    }
    pDma->fSG = (1 == tmp);

    printf("\n");
    printf("Select DMA completion method:\n");
    printf("1. Interrupts\n");
    printf("2. Polling\n");
    printf("%d. Cancel\n", DIAG_EXIT_MENU);
    if ((DIAG_INPUT_SUCCESS != DIAG_GetMenuOption(&tmp, 2)) ||
        (DIAG_EXIT_MENU == tmp))
    {
        return FALSE;
    }
    *pfPolling = (2 == tmp);

    printf("\n");
    printf("Select DMA direction:\n");
    printf("1. from device\n");
    printf("2. to device\n");
    printf("%d. Cancel\n", DIAG_EXIT_MENU);
    if ((DIAG_INPUT_SUCCESS != DIAG_GetMenuOption(&tmp, 2)) ||
        (DIAG_EXIT_MENU == tmp))
    {
        return FALSE;
    }
    *pdwOptions = (1 == tmp) ? DMA_FROM_DEVICE : DMA_TO_DEVICE;

    printf("\n");
    if (DIAG_INPUT_SUCCESS != DIAG_InputUINT32(pu32LocalAddr,
        "Enter local DMA address", TRUE, 0, 0))
    {
        return FALSE;
    }

    printf("\n");
    if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD(pdwBytes,
        "Enter size of DMA buffer, in bytes", TRUE, 0, 0))
    {
        return FALSE;
    }
    if (!*pdwBytes)
    {
        printf("Invalid input: Buffer size must be larger than 0\n");
        return FALSE;
    }
    printf("\n");

    return TRUE;
}

/* -----------------------------------------------
    Interrupt handling
   ----------------------------------------------- */
/* Interrupts menu options */
enum {
    PLX_DIAG_MENU_INT_ENABLE_DISABLE = 1,
    PLX_DIAG_MENU_INT_EXIT = DIAG_EXIT_MENU,
};

/* Enable/Disable interrupts menu */
void PLX_DIAG_MenuInterrupts(WDC_DEVICE_HANDLE hDev,
    PLX_INT_HANDLER DiagIntHandler, PVOID pData)
{
    DWORD option;
    BOOL fIntEnable;

    do
    {
        fIntEnable = !PLX_IntIsEnabled(hDev);

        printf("\n");
        printf("Interrupts\n");
        printf("-----------\n");
        printf("%d. %s interrupts\n", PLX_DIAG_MENU_INT_ENABLE_DISABLE,
            fIntEnable ? "Enable" : "Disable");
        printf("%d. Exit menu\n", PLX_DIAG_MENU_INT_EXIT);
        printf("\n");

        if (DIAG_INPUT_SUCCESS != DIAG_GetMenuOption(&option,
            PLX_DIAG_MENU_INT_ENABLE_DISABLE))
        {
            continue;
        }

        switch (option)
        {
        case PLX_DIAG_MENU_INT_EXIT: /* Exit menu */
            break;
        case PLX_DIAG_MENU_INT_ENABLE_DISABLE: /* Enable/disable interrupts */
            if (fIntEnable)
            {
                if (WD_STATUS_SUCCESS == PLX_IntEnable(hDev, DiagIntHandler,
                    pData))
                {
                    printf("Interrupts enabled\n");
                }
                else
                {
                    PLX_DIAG_ERR("Failed enabling interrupts: %s",
                        PLX_GetLastErr());
                }
            }
            else
            {
                if (WD_STATUS_SUCCESS == PLX_IntDisable(hDev))
                    printf("Interrupts disabled\n");
                else
                    PLX_DIAG_ERR("Failed disabling interrupts: %s",
                        PLX_GetLastErr());
            }
            break;
        }
    } while (PLX_DIAG_MENU_INT_EXIT != option);
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
void PLX_DIAG_MenuEvents(WDC_DEVICE_HANDLE hDev,
    PLX_EVENT_HANDLER DiagEventHandler)
{
    DWORD option;
    BOOL fRegister;

    do
    {
        fRegister = !PLX_EventIsRegistered(hDev);

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
                                                        * events */
            if (fRegister)
            {
                if (WD_STATUS_SUCCESS == PLX_EventRegister(hDev,
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
                if (WD_STATUS_SUCCESS == PLX_EventUnregister(hDev))
                    printf("Events unregistered\n");
                else
                    PLX_DIAG_ERR("Failed to unregister events. Last Error:\n%s",
                        PLX_GetLastErr());
            }
            break;
        }
    } while (PLX_DIAG_MENU_EVENTS_EXIT != option);
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
void PLX_DIAG_MenuEEPROM(WDC_DEVICE_HANDLE hDev, DWORD EEPROMmsb)
{
    DWORD option;
    DWORD dwStatus;
    DWORD dwOffset;
    UINT32 u32Data = 0;
    WORD wData = 0;
    BOOL fVPDSupported = PLX_EEPROM_VPD_Validate(hDev);

    if (!PLX_EEPROMIsPresent(hDev))
    {
        printf("Cannot access the serial EEPROM - "
            "No serial EEPROM found on the board ...\n");
        return;
    }

    do
    {
        printf("\n");
        printf("Access the board's serial EEPROM\n");
        printf("---------------------------------\n");
        if (fVPDSupported)
        {
            printf("NOTE: EEPROM data is accessed via Vital Product Data (VPD) "
                "as DWORDs\n");
        }
        else
        {
            printf("NOTE: EEPROM data is accessed via run-time (RT) registers "
                "as WORDs\n");
        }
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
                dwStatus = fVPDSupported ? PLX_EEPROM_VPD_Read32(hDev, dwOffset,
                    &u32Data) :
                    PLX_EEPROM_RT_Read32(hDev, dwOffset, &u32Data, EEPROMmsb);
                if (WD_STATUS_SUCCESS != dwStatus)
                {
                    printf("\nError occurred while reading offset 0x%lX of the "
                        "serial EEPROM.\nError 0x%lx - %s\n", dwOffset,
                        dwStatus, Stat2Str(dwStatus));
                    break;
                }
                printf("%08X  ", u32Data);
            }
            printf("\n");

            break;
        case PLX_DIAG_MENU_EEPROM_READ: /* Read from the serial EEPROM */
            sprintf(gsInput, "Enter offset to read from (must be a multiple of "
                "%ld)", fVPDSupported ? WDC_SIZE_32 : WDC_SIZE_16);
            if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD(&dwOffset, gsInput, TRUE,
                0, EEPROM_MAX_OFFSET))
            {
                break;
            }

            dwStatus = fVPDSupported ? PLX_EEPROM_VPD_Read32(hDev, dwOffset,
                &u32Data) : PLX_EEPROM_RT_Read16(hDev, dwOffset, &wData,
                    EEPROMmsb);

            if (WD_STATUS_SUCCESS == dwStatus)
            {
                printf("Read 0x%X from offset 0x%lX of the serial EEPROM\n",
                    fVPDSupported ? u32Data : (UINT32)wData, dwOffset);
            }
            else
            {
                printf("Failed reading from offset 0x%lX of the serial "
                    "EEPROM.\n", dwOffset);
            }

            break;
        case PLX_DIAG_MENU_EEPROM_WRITE: /* Write to the serial EEPROM */
            sprintf(gsInput, "Enter offset to write to (must be a multiple of "
                "%ld)", fVPDSupported ? WDC_SIZE_32 : WDC_SIZE_16);
            if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD(&dwOffset, gsInput, TRUE,
                0, EEPROM_MAX_OFFSET))
            {
                break;
            }

            if (DIAG_INPUT_SUCCESS != DIAG_InputNum(fVPDSupported ?
                (PVOID)&u32Data : (PVOID)&wData, "Enter data to write", TRUE,
                fVPDSupported ? sizeof(u32Data) : sizeof(wData), 0, 0))
            {
                break;
            }

            dwStatus = fVPDSupported ? PLX_EEPROM_VPD_Write32(hDev, dwOffset,
                u32Data) : PLX_EEPROM_RT_Write16(hDev, dwOffset, wData,
                    EEPROMmsb);

            printf("%s 0x%X to offset 0x%lX of the serial EEPROM\n",
                (WD_STATUS_SUCCESS == dwStatus) ? "Wrote" : "Failed to write",
                fVPDSupported ? u32Data : (UINT32)wData, dwOffset);
            break;
        }
    } while (PLX_DIAG_MENU_EEPROM_EXIT != option);
}

/* -----------------------------------------------
    Reset board
   ----------------------------------------------- */
/* Reset board menu options */
enum {
    PLX_DIAG_MENU_RESET_BOARD_SOFT_RESET = 1,
    PLX_DIAG_MENU_RESET_BOARD_EXIT = DIAG_EXIT_MENU,
};

/* NOTE: Currently supported for master devices only (PLX 9054, 9056, 9080,
 * 9656) */
void PLX_DIAG_MenuResetBoard(WDC_DEVICE_HANDLE hDev)
{
    DWORD option;

    do {
        printf("\n");
        printf("Reset Board\n");
        printf("------------\n");
        printf("%d. Soft reset\n", PLX_DIAG_MENU_RESET_BOARD_SOFT_RESET);
        printf("%d. Exit menu\n", PLX_DIAG_MENU_RESET_BOARD_EXIT);
        printf("\n");

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option,
            PLX_DIAG_MENU_RESET_BOARD_SOFT_RESET))
        {
            continue;
        }

        switch (option)
        {
        case PLX_DIAG_MENU_RESET_BOARD_EXIT: /* Exit menu */
            break;
        case PLX_DIAG_MENU_RESET_BOARD_SOFT_RESET: /* Perform soft reset */
            printf("\n");
            printf("Performing soft reset ...\n");
            PLX_SoftResetMaster(hDev);
            break;
        }
    } while (PLX_DIAG_MENU_RESET_BOARD_EXIT != option);
}

