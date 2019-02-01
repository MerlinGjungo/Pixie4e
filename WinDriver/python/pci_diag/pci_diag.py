# Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com

import sys, os
sys.path.append(os.path.dirname(os.path.abspath(__file__)) + "/..")
from pci_lib import *
from wdlib.diag_lib import *
from wdlib.wdc_diag_lib import *
from wdlib.wds_diag_lib import *
#endif # ifndef ISA

gpDevCtx = PCI_DEV_CTX()

# --------------------------------------------------
#    PCI configuration registers information
# --------------------------------------------------
# Configuration registers information array

gPCI_CfgRegs = [ \
    WDC_REG(WDC_AD_CFG_SPACE, PCI_VID, WDC_SIZE_16, WDC_READ_WRITE, "VID",
        "Vendor ID"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_DID, WDC_SIZE_16, WDC_READ_WRITE, "DID",
        "Device ID"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_CR, WDC_SIZE_16, WDC_READ_WRITE, "CMD",
        "Command"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_SR, WDC_SIZE_16, WDC_READ_WRITE, "STS",
        "Status"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_REV, WDC_SIZE_32, WDC_READ_WRITE, "RID_CLCD",
        "Revision ID &\nClass Code"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_CCSC, WDC_SIZE_8, WDC_READ_WRITE, "SCC",
        "Sub Class Code"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_CCBC, WDC_SIZE_8, WDC_READ_WRITE, "BCC",
        "Base Class Code"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_CLSR, WDC_SIZE_8, WDC_READ_WRITE, "CALN",
        "Cache Line Size"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_LTR, WDC_SIZE_8, WDC_READ_WRITE, "LAT",
        "Latency Timer"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_HDR, WDC_SIZE_8, WDC_READ_WRITE, "HDR",
        "Header Type"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_BISTR, WDC_SIZE_8, WDC_READ_WRITE, "BIST",
        "Built-in Self Test"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_BAR0, WDC_SIZE_32, WDC_READ_WRITE, "BADDR0",
        "Base Address 0"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_BAR1, WDC_SIZE_32, WDC_READ_WRITE, "BADDR1",
        "Base Address 1"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_BAR2, WDC_SIZE_32, WDC_READ_WRITE, "BADDR2",
        "Base Address 2"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_BAR3, WDC_SIZE_32, WDC_READ_WRITE, "BADDR3",
        "Base Address 3"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_BAR4, WDC_SIZE_32, WDC_READ_WRITE, "BADDR4",
        "Base Address 4"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_BAR5, WDC_SIZE_32, WDC_READ_WRITE, "BADDR5",
        "Base Address 5"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_CIS, WDC_SIZE_32, WDC_READ_WRITE, "CIS",
        "CardBus CIS\npointer"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_SVID, WDC_SIZE_16, WDC_READ_WRITE, "SVID",
        "Sub-system\nVendor ID"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_SDID, WDC_SIZE_16, WDC_READ_WRITE, "SDID",
        "Sub-system\nDevice ID"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_EROM, WDC_SIZE_32, WDC_READ_WRITE, "EROM",
        "Expansion ROM\nBase Address"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_CAP, WDC_SIZE_8, WDC_READ_WRITE, "NEW_CAP",
        "New Capabilities\nPointer"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_ILR, WDC_SIZE_32, WDC_READ_WRITE, "INTLN",
        "Interrupt Line"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_IPR, WDC_SIZE_32, WDC_READ_WRITE, "INTPIN",
        "Interrupt Pin"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_MGR, WDC_SIZE_32, WDC_READ_WRITE, "MINGNT",
        "Minimum Required\nBurst Period"),
    WDC_REG(WDC_AD_CFG_SPACE, PCI_MLR, WDC_SIZE_32, WDC_READ_WRITE, "MAXLAT",
        "Maximum Latency")]

gPCI_ext_CfgRegs =\
    [WDC_REG(WDC_AD_CFG_SPACE, PCIE_CAP_ID, WDC_SIZE_8, WDC_READ_WRITE,
        "PCIE_CAP_ID", "PCI Express\nCapability ID"),
    WDC_REG(WDC_AD_CFG_SPACE, NEXT_CAP_PTR, WDC_SIZE_8, WDC_READ_WRITE,
        "NEXT_CAP_PTR", "Next Capabiliy Pointer"),
    WDC_REG(WDC_AD_CFG_SPACE, CAP_REG, WDC_SIZE_16, WDC_READ_WRITE, "CAP_REG",
        "Capabilities Register"),
    WDC_REG(WDC_AD_CFG_SPACE, DEV_CAPS, WDC_SIZE_32, WDC_READ_WRITE, "DEV_CAPS",
        "Device Capabilities"),
    WDC_REG(WDC_AD_CFG_SPACE, DEV_CTL, WDC_SIZE_16, WDC_READ_WRITE, "DEV_CTL",
        "Device Control"),
    WDC_REG(WDC_AD_CFG_SPACE, DEV_STS, WDC_SIZE_16, WDC_READ_WRITE, "DEV_STS",
        "Device Status"),
    WDC_REG(WDC_AD_CFG_SPACE, LNK_CAPS, WDC_SIZE_32, WDC_READ_WRITE, "LNK_CAPS",
        "Link Capabilities"),
    WDC_REG(WDC_AD_CFG_SPACE, LNK_CTL, WDC_SIZE_16, WDC_READ_WRITE, "LNK_CTL",
        "Link Control"),
    WDC_REG(WDC_AD_CFG_SPACE, LNK_STS, WDC_SIZE_16, WDC_READ_WRITE, "LNK_STS",
        "Link Status"),
    WDC_REG(WDC_AD_CFG_SPACE, SLOT_CAPS, WDC_SIZE_32, WDC_READ_WRITE,
        "SLOT_CAPS", "Slot Capabilities"),
    WDC_REG(WDC_AD_CFG_SPACE, SLOT_CTL, WDC_SIZE_16, WDC_READ_WRITE, "SLOT_CTL",
        "Slot Control"),
    WDC_REG(WDC_AD_CFG_SPACE, SLOT_STS, WDC_SIZE_16, WDC_READ_WRITE, "SLOT_STS",
        "Slot Status"),
    WDC_REG(WDC_AD_CFG_SPACE, ROOT_CAPS, WDC_SIZE_16, WDC_READ_WRITE,
        "ROOT_CAPS", "Root Capabilities"),
    WDC_REG(WDC_AD_CFG_SPACE, ROOT_CTL, WDC_SIZE_16, WDC_READ_WRITE, "ROOT_CTL",
        "Root Control"),
    WDC_REG(WDC_AD_CFG_SPACE, ROOT_STS, WDC_SIZE_32, WDC_READ_WRITE, "ROOT_STS",
        "Root Status"),
    WDC_REG(WDC_AD_CFG_SPACE, DEV_CAPS2, WDC_SIZE_32, WDC_READ_WRITE,
        "DEV_CAPS2", "Device Capabilities 2"),
    WDC_REG(WDC_AD_CFG_SPACE, DEV_CTL2, WDC_SIZE_16, WDC_READ_WRITE, "DEV_CTL2",
        "Device Control 2"),
    WDC_REG(WDC_AD_CFG_SPACE, DEV_STS2, WDC_SIZE_16, WDC_READ_WRITE, "DEV_STS2",
        "Device Status 2"),
    WDC_REG(WDC_AD_CFG_SPACE, LNK_CAPS2, WDC_SIZE_32, WDC_READ_WRITE,
        "LNK_CAPS2", "Link Capabilities 2"),
    WDC_REG(WDC_AD_CFG_SPACE, LNK_CTL2, WDC_SIZE_16, WDC_READ_WRITE, "LNK_CTL2",
        "Link Control 2"),
    WDC_REG(WDC_AD_CFG_SPACE, LNK_STS2, WDC_SIZE_16, WDC_READ_WRITE, "LNK_STS2",
        "Link Status 2"),
    WDC_REG(WDC_AD_CFG_SPACE, SLOT_CAPS2, WDC_SIZE_32, WDC_READ_WRITE,
        "SLOT_CAPS2", "Slot Capabilities 2"),
    WDC_REG(WDC_AD_CFG_SPACE, SLOT_CTL2, WDC_SIZE_16, WDC_READ_WRITE,
        "SLOT_CTL2", "Slot Control 2"),
    WDC_REG(WDC_AD_CFG_SPACE, SLOT_STS2, WDC_SIZE_16, WDC_READ_WRITE,
        "SLOT_STS2", "Slot Status 2")]

PCI_CFG_EXT_REGS_NUM = len(gPCI_ext_CfgRegs)
PCI_CFG_REGS_NUM = len(gPCI_CfgRegs)

# TODO: For read-only or write-only registers, change the direction field of
#        the relevant registers in gPCI_CfgRegs to WDC_READ or WDC_WRITE.
# NOTE: You can define additional configuration registers in gPCI_CfgRegs.

#endif # ifndef ISA

#************************************************************
#  Static functions prototypes
# ************************************************************

hDev = WDC_DEVICE_HANDLE()
def main():
    global hDev

    print("PCI diagnostic utility.")
    print("Application accesses hardware using " + WD_PROD_NAME)
    print("and a Kernel PlugIn driver (%s)." % KP_PCI_DRIVER_NAME)
    if sys.platform == "win32":
        print("NOTICE: Some functionalities may not work without having\n"
            "        installed an INF file for the requested device. You can\n"
            "        generate an INF file using the DriverWizard.")

    # Initialize the PCI library
    dwStatus = PCI_LibInit()
    if WD_STATUS_SUCCESS != dwStatus:
        WDC_DIAG_ERR("pci_diag: Failed to initialize the PCI library: 0x%lX - "
            "%s" % (dwStatus, wdapi.Stat2Str(dwStatus)))
        return dwStatus

    wdapi_va.PrintDbgMessage(D_ERROR, S_PCI, "WinDriver user mode version %s" %
        WD_VERSION_STR)

    # Find and open a PCI device (by default ID)
    if PCI_DEFAULT_VENDOR_ID:
        hDev = DeviceFindAndOpen(PCI_DEFAULT_VENDOR_ID, PCI_DEFAULT_DEVICE_ID)

#else # ifdef ISA
#    # Open a handle to the device
#    hDev = PCI_DeviceOpen(WD_CARD())
#    if not hDev:
#        PCI_ERR("pci_diag: Failed opening a handle to the device")
#        return None;
#endif # ifdef ISA
    # Display main diagnostics menu for communicating with the device
    MenuMain(hDev)

    # Perform necessary cleanup before exiting the program:
    # Close the device handle

    if hDev:
        DeviceClose(hDev)

    if wdapi.WDS_IsIpcRegistered():
        wdapi.WDS_IpcUnRegister()
#else # ifdef ISA
#    if hDev and not PCI_DeviceClose(hDev):
#        PCI_ERR("pci_diag: Failed closing ISA device");
#endif # ifndef ISA

    # Uninitialize libraries
    dwStatus = PCI_LibUninit()
    if WD_STATUS_SUCCESS != dwStatus:
        WDC_DIAG_ERR("pci_diag: Failed to uninitialize the PCI library")

    return dwStatus

# -----------------------------------------------
#    Main diagnostics menu
#   -----------------------------------------------

# Main diagnostics menu
def MenuMain(hDev):
    # Main menu options
    (MENU_MAIN_SCAN_BUS,
    MENU_MAIN_FIND_AND_OPEN,
    MENU_MAIN_SHARED_BUFFER,
    MENU_MAIN_IPC,
    MENU_MAIN_RW_CFG_SPACE,
    MENU_MAIN_EVENTS,
    MENU_MAIN_RW_ADDR,
    MENU_MAIN_ENABLE_DISABLE_INT,
    MENU_MAIN_ALLOC_FREE_DMA,
    # @@@regs_enum_opt_py@@@
    ) = xrange(1, 10)
    # @@@regs_enum_opt_py@@@
    MENU_MAIN_EXIT = DIAG_EXIT_MENU

    while True:
        print
        print("PCI main menu")
        print("-------------")
#ifdef ISA
#    if True:
#else # ifndef ISA
        print("%d. Scan PCI bus" % MENU_MAIN_SCAN_BUS)
        print("%d. Find and open a PCI device" % MENU_MAIN_FIND_AND_OPEN)
        print("%d. Allocate/free Shared Buffer" % MENU_MAIN_SHARED_BUFFER)
        print("%d. Manage IPC" % MENU_MAIN_IPC)
        if hDev:
            print("%d. Read/write the PCI configuration space" %
                  MENU_MAIN_RW_CFG_SPACE)
            print("%d. Register/unregister plug-and-play and power management "
                  "events" % MENU_MAIN_EVENTS)
#endif # ifndef ISA
            print("%d. Read/write memory and I/O addresses on the device" %
                  MENU_MAIN_RW_ADDR)
            print("%d. Enable/disable the device's interrupts" %
                  MENU_MAIN_ENABLE_DISABLE_INT)
            print("%d. Allocate/free memory for DMA" %
                  MENU_MAIN_ALLOC_FREE_DMA)
        print("%d. Exit" % MENU_MAIN_EXIT)
        if not "PCI_REGS_NUM" in globals():
            (option, dwStatus) = DIAG_GetMenuOption(MENU_MAIN_ALLOC_FREE_DMA if\
                hDev else MENU_MAIN_IPC)
        else:
            (option, dwStatus) = DIAG_GetMenuOption(MENU_MAIN_RW_REGS if hDev \
                else MENU_MAIN_IPC)

        if DIAG_INPUT_FAIL == dwStatus:
            continue

        if option == MENU_MAIN_EXIT: # Exit menu
            break
        elif option == MENU_MAIN_SCAN_BUS: # Scan bus
            WDC_DIAG_PciDevicesInfoPrintAll(False)

        elif option == MENU_MAIN_FIND_AND_OPEN: # Find and open a PCI device
            if hDev:
                DeviceClose(hDev)
            hDev = DeviceFindAndOpen(0, 0)
        elif option == MENU_MAIN_SHARED_BUFFER: # Handle Shared Buffer Operations
            MenuSharedBuffer()
        elif option == MENU_MAIN_IPC: # Register/unregister Inter-Process
                                   # Communication
            MenuIpc()
        elif option == MENU_MAIN_RW_CFG_SPACE: # Read/write the device's configuration
                                           #    space
            MenuReadWriteCfgSpace(hDev)
        elif option == MENU_MAIN_EVENTS: # Register/unregister plug-and-play and power
                                      # management events
            MenuEvents(hDev)
#endif # ifndef ISA
        elif option == MENU_MAIN_RW_ADDR: # Read/write memory and I/O addresses
            MenuReadWriteAddr(hDev)
        elif option == MENU_MAIN_ALLOC_FREE_DMA: # Allocate/free DMA memory
            MenuDma(hDev)
        elif option == MENU_MAIN_ENABLE_DISABLE_INT: # Enable/disable interrupts
            MenuInterrupts(hDev)


# -----------------------------------------------
#    Device find, open and close
#   -----------------------------------------------
# Find and open a PCI device
def DeviceFindAndOpen(dwVendorId, dwDeviceId):
    # Find device
    slot = DeviceFind(dwVendorId, dwDeviceId)
    if not slot:
        return None

    # Open a device handle
    return DeviceOpen(slot)

# Find a PCI device
def DeviceFind(dwVendorId, dwDeviceId):
    scanResult = WDC_PCI_SCAN_RESULT()

    if not dwVendorId:
        # Get vendor ID
        (dwVendorId, dwStatus) = DIAG_InputNum("Enter vendor ID", True,
            sizeof(DWORD), 0, 0)
        if DIAG_INPUT_SUCCESS != dwStatus:
            return None

        # Get device ID
        (dwDeviceId, dwStatus) = DIAG_InputNum("Enter device ID", True,
            sizeof(DWORD), 0, 0)
        if DIAG_INPUT_SUCCESS != dwStatus:
            return None

    # Scan PCI devices
    dwStatus = wdapi.WDC_PciScanDevices(dwVendorId, dwDeviceId,
        byref(scanResult))
    if WD_STATUS_SUCCESS != dwStatus:
        WDC_DIAG_ERR("DeviceFind: Failed scanning the PCI bus.\n"
            "Error [0x%lx - %s]\n" % (dwStatus, wdapi.Stat2Str(dwStatus)))
        return None

    dwNumDevices = scanResult.dwNumDevices
    if not dwNumDevices:
        print("No matching PCI device was found for search criteria "
            "(Vendor ID 0x%lX, Device ID 0x%lX)\n" % (dwVendorId, dwDeviceId))
        return None

    # Display matching devices information
    print("\nFound %ld matching device%s "
        "[ Vendor ID 0x%lX%s, Device ID 0x%lX%s ]:\n" %
        (dwNumDevices, "s" if dwNumDevices > 1 else "",
        dwVendorId.value, "" if dwVendorId else " (ALL)",
        dwDeviceId.value, "" if dwDeviceId else " (ALL)"))

    for i in range(dwNumDevices):
        print("%2ld. Vendor ID: 0x%lX, Device ID: 0x%lX" % (i + 1,
            scanResult.deviceId[i].dwVendorId,
            scanResult.deviceId[i].dwDeviceId))
        WDC_DIAG_PciDeviceInfoPrint(scanResult.deviceSlot[i], False)

    # Select device
    if dwNumDevices > 1:
        i = 0
        gs = "Select a device (1 - %ld): " % dwNumDevices
        (i, dwStatus) = DIAG_InputNum(gs, False, 0, 0, 0)
        if DIAG_INPUT_SUCCESS != dwStatus:
            return None

    return scanResult.deviceSlot[i - 1]

def CheckKPDriverVer(hDev):
    kpVer = KP_PCI_VERSION()
    dwKPResult = DWORD()

    # Get Kernel PlugIn Driver version
    dwStatus = wdapi.WDC_CallKerPlug(hDev, KP_PCI_MSG_VERSION, byref(kpVer),
        byref(dwKPResult))
    if WD_STATUS_SUCCESS != dwStatus:
        WDC_DIAG_ERR("Failed sending a \'Get Version\' message [0x%x] to the "
            "Kernel-PlugIn driver [%s]. Error [0x%lx - %s]\n" %
            (KP_PCI_MSG_VERSION, KP_PCI_DRIVER_NAME, dwStatus,
            wdapi.Stat2Str(dwStatus)))

    elif KP_PCI_STATUS_OK != dwKPResult.value:
        WDC_DIAG_ERR("Kernel-PlugIn \'Get Version\' message [0x%x] failed. "
            "Kernel PlugIn status [0x%lx]\n" % (KP_PCI_MSG_VERSION,
                dwKPResult.value))
        dwStatus = WD_INCORRECT_VERSION
    else:
        print("Using [%s] Kernel-Plugin driver version [%ld.%02ld - %s]\n" %
            (KP_PCI_DRIVER_NAME, kpVer.dwVer / 100, kpVer.dwVer % 100,
            kpVer.cVer))

    return dwStatus


# Open a handle to a PCI device
def DeviceOpen(slot):
    hDev = WDC_DEVICE_HANDLE()
    deviceInfo = WD_PCI_CARD_INFO()

    # Retrieve the device's resources information
    deviceInfo.pciSlot = slot
    dwStatus = wdapi.WDC_PciGetDeviceInfo(byref(deviceInfo))
    if WD_STATUS_SUCCESS != dwStatus:
        WDC_DIAG_ERR("DeviceOpen: Failed retrieving the device's resources "
            "information. Error [0x%lx - %s]\n" % (dwStatus,
            wdapi.Stat2Str(dwStatus)))
        return None

    # NOTE: If necessary, you can modify the device's resources information
    #   here - mainly the information stored in the deviceInfo.Card.Items array,
    #   and the number of array items stored in deviceInfo.Card.dwItems.
    #   For example:
    #   - Edit the deviceInfo.Card.Items array and/or deviceInfo.Card.dwItems,
    #    to register only some of the resources or to register only a portion
    #    of a specific address space.
    #   - Set the fNotSharable field of one or more items in the
    #    deviceInfo.Card.Items array to 1, to block sharing of the related
    #    resources and ensure that they are locked for exclusive use.

    # Open a handle to the device
    hDev = PCI_DeviceOpen(deviceInfo)
    if not hDev:
        WDC_DIAG_ERR("DeviceOpen: Failed opening a handle to the device")
        return None

    # Get Kernel PlugIn driver version
    if WDC_IS_KP(hDev):
        CheckKPDriverVer(hDev)

    return hDev

# Close handle to a PCI device
def DeviceClose(hDev):
    # Validate the WDC device handle
    if not hDev:
        return

    # Close the WDC device handle
    if not PCI_DeviceClose(hDev):
        WDC_DIAG_ERR("DeviceClose: Failed closing PCI device")
#endif # ifndef ISA

# -----------------------------------------------
#    Read/write memory and I/O addresses
#   -----------------------------------------------
# Read/write address menu options
(   MENU_RW_ADDR_SET_ADDR_SPACE,
    MENU_RW_ADDR_SET_MODE,
    MENU_RW_ADDR_SET_TRANS_TYPE,
    MENU_RW_ADDR_READ,
    MENU_RW_ADDR_WRITE) = xrange(1, 6)
MENU_RW_ADDR_EXIT = DIAG_EXIT_MENU

ACTIVE_ADDR_SPACE_NEEDS_INIT = 0xFF

# Read/write memory or I/O space address menu
MenuReadWriteAddr_dwAddrSpace = ACTIVE_ADDR_SPACE_NEEDS_INIT
MenuReadWriteAddr_mode = WDC_MODE_32
def MenuReadWriteAddr(hDev):
    fBlock = False
    global MenuReadWriteAddr_dwAddrSpace
    global MenuReadWriteAddr_mode
    # Initialize active address space
    if ACTIVE_ADDR_SPACE_NEEDS_INIT == MenuReadWriteAddr_dwAddrSpace:
        dwNumAddrSpaces = PCI_GetNumAddrSpaces(hDev)
#else # ifdef ISA
#        dwNumAddrSpaces = PCI_ADDR_SPACES_NUM;
#endif # ifdef ISA
        # Find the first active address space
        for MenuReadWriteAddr_dwAddrSpace in range(dwNumAddrSpaces):
            if wdapi.WDC_AddrSpaceIsActive(hDev,
                    MenuReadWriteAddr_dwAddrSpace):
                break

        # Sanity check
        if MenuReadWriteAddr_dwAddrSpace == dwNumAddrSpaces:
            WDC_DIAG_ERR("MenuReadWriteAddr: Error - No active address spaces "
                "found\n")
            MenuReadWriteAddr_dwAddrSpace = ACTIVE_ADDR_SPACE_NEEDS_INIT
            return

    while True:
        print("Read/write the device's memory and I/O ranges")
        print("----------------------------------------------")
        print("%d. Change active address space for read/write" %
            MENU_RW_ADDR_SET_ADDR_SPACE),
        print("(currently: BAR %ld)" % MenuReadWriteAddr_dwAddrSpace)
#else # ifdef ISA
#        print("(currently: AddrSpace %ld)" % MenuReadWriteAddr_dwAddrSpace)
#endif # ifdef ISA
        print("%d. Change active read/write mode (currently: %s)" % \
            (MENU_RW_ADDR_SET_MODE, \
            "8 bit" if WDC_MODE_8 == MenuReadWriteAddr_mode \
            else "16 bit" if WDC_MODE_16 == MenuReadWriteAddr_mode \
            else "32 bit" if WDC_MODE_32 == MenuReadWriteAddr_mode \
            else "64 bit"))
        print("%d. Toggle active transfer type (currently: %s)" % \
            (MENU_RW_ADDR_SET_TRANS_TYPE, \
            "block transfers" if fBlock else "non-block transfers"))
        print("%d. Read from active address space" % MENU_RW_ADDR_READ)
        print("%d. Write to active address space" % MENU_RW_ADDR_WRITE)
        print("%d. Exit menu\n" % MENU_RW_ADDR_EXIT)

        (option, dwStatus) = DIAG_GetMenuOption(MENU_RW_ADDR_WRITE)
        if DIAG_INPUT_FAIL == dwStatus:
            continue

        if option == MENU_RW_ADDR_EXIT: # Exit menu
            break
        elif option == MENU_RW_ADDR_SET_ADDR_SPACE: # Set active address space for
                                                  # read/write address requests
            MenuReadWriteAddr_dwAddrSpace = SetAddrSpace(hDev)
        elif option == MENU_RW_ADDR_SET_MODE: # Set active mode for read/write address
                                            # requests
            (temp, dwStatus) = WDC_DIAG_SetMode()
            if dwStatus == DIAG_INPUT_SUCCESS:
                MenuReadWriteAddr_mode = temp

        elif option == MENU_RW_ADDR_SET_TRANS_TYPE: # Toggle active transfer type
            fBlock = not fBlock
        elif option == MENU_RW_ADDR_READ or option == MENU_RW_ADDR_WRITE:
        # Read or Write to a memory or I/O address
            direction = WDC_READ if option == MENU_RW_ADDR_READ else WDC_WRITE
            if fBlock:
                WDC_DIAG_ReadWriteBlock(hDev, direction,
                                      MenuReadWriteAddr_dwAddrSpace)
            else:
                WDC_DIAG_ReadWriteAddr(hDev, direction,
                                      MenuReadWriteAddr_dwAddrSpace,
                                      MenuReadWriteAddr_mode)

# Set address space
def SetAddrSpace(hDev):
    dwNumAddrSpaces = PCI_GetNumAddrSpaces(hDev)
#else # ifdef ISA
#    dwNumAddrSpaces = PCI_ADDR_SPACES_NUM
#endif # ifdef ISA
    addrSpaceInfo = PCI_ADDR_SPACE_INFO()

    print("\n")
    print("Select an active address space:")
    print("-------------------------------")

    for dwAddrSpace in range(dwNumAddrSpaces):
        addrSpaceInfo.dwAddrSpace = dwAddrSpace
        if not PCI_GetAddrSpaceInfo(hDev, addrSpaceInfo):
            WDC_DIAG_ERR("SetAddrSpace: Error - Failed to get address space "
                "information.")
            return None

        print("%ld. %-*s %-*s %s" % (dwAddrSpace + 1, MAX_NAME_DISPLAY,
            addrSpaceInfo.sName, MAX_TYPE - 1, addrSpaceInfo.sType,
            addrSpaceInfo.sDesc))

    (dwAddrSpace, dwStatus) = DIAG_InputNum("Enter option", False,
        sizeof(DWORD), 1, dwNumAddrSpaces)
    if DIAG_INPUT_SUCCESS != dwStatus:
        return None

    dwAddrSpace.value -= 1
    if not wdapi.WDC_AddrSpaceIsActive(hDev, dwAddrSpace):
        print("You have selected an inactive address space")
        return None

    return dwAddrSpace.value

# -----------------------------------------------
#    Read/write the configuration space
#   -----------------------------------------------
# Read/write the configuration space menu options
(   MENU_RW_CFG_SPACE_READ_OFFSET,
    MENU_RW_CFG_SPACE_WRITE_OFFSET,
    MENU_RW_CFG_SPACE_READ_ALL_REGS,
    MENU_RW_CFG_SPACE_READ_REG,
    MENU_RW_CFG_SPACE_WRITE_REG,
    MENU_RW_CFG_SPACE_EXT_READ_REG,
    MENU_RW_CFG_SPACE_EXT_WRITE_REG,
    MENU_RW_CFG_SPACE_SCAN_CAP) = xrange(1,9)
MENU_RW_CFG_SPACE_EXIT = DIAG_EXIT_MENU

# Read/write configuration space menu
def MenuReadWriteCfgSpace(hDev):
    fExpress = (wdapi.WDC_GetPciExpressGen(hDev) != 0)
    while True:
        # Display predefined registers information
        print("\nConfiguration registers:")
        print("------------------------")
        WDC_DIAG_RegsInfoPrint(hDev, gPCI_CfgRegs, PCI_CFG_REGS_NUM,
            WDC_DIAG_REG_PRINT_ALL & ~WDC_DIAG_REG_PRINT_ADDR_SPACE, False)
        if fExpress:
            WDC_DIAG_RegsInfoPrint(hDev, gPCI_ext_CfgRegs, PCI_CFG_EXT_REGS_NUM,
                WDC_DIAG_REG_PRINT_ALL & ~WDC_DIAG_REG_PRINT_ADDR_SPACE, True)

        print("\nRead/write the device's configuration space")
        print("--------------------------------------------")
        print("%d. Read from an offset" % MENU_RW_CFG_SPACE_READ_OFFSET)
        print("%d. Write to an offset" % MENU_RW_CFG_SPACE_WRITE_OFFSET)
        print("%d. Read all configuration registers defined for the device "
            "(see list above)" % MENU_RW_CFG_SPACE_READ_ALL_REGS)
        print("%d. Read from a named register" % MENU_RW_CFG_SPACE_READ_REG)
        print("%d. Write to a named register" % MENU_RW_CFG_SPACE_WRITE_REG)

        if fExpress:
            print("%d. Read from a named PCI Express register" %
                MENU_RW_CFG_SPACE_EXT_READ_REG)
            print("%d. Write to a named PCI Express register" %
                MENU_RW_CFG_SPACE_EXT_WRITE_REG)

        print("%d. Scan PCI/PCIe capabilities" % MENU_RW_CFG_SPACE_SCAN_CAP)
        print("%d. Exit menu\n" % MENU_RW_CFG_SPACE_EXIT)

        selection = DIAG_GetMenuOption(MENU_RW_CFG_SPACE_SCAN_CAP)
        if DIAG_INPUT_FAIL == selection[1]:
            continue
        option = selection[0]

        if option == MENU_RW_CFG_SPACE_EXIT: # Exit menu
            break
        elif option == MENU_RW_CFG_SPACE_READ_OFFSET: # Read from a
                                                  # configuration space offset
            WDC_DIAG_ReadWriteBlock(hDev, WDC_READ, WDC_AD_CFG_SPACE)
        elif option == MENU_RW_CFG_SPACE_WRITE_OFFSET: # Write to a
                                                  # configuration space offset
            WDC_DIAG_ReadWriteBlock(hDev, WDC_WRITE, WDC_AD_CFG_SPACE)
        elif option == MENU_RW_CFG_SPACE_READ_ALL_REGS:
            WDC_DIAG_ReadRegsAll(hDev, gPCI_CfgRegs, PCI_CFG_REGS_NUM, True,
                  False)
            if fExpress:
                 WDC_DIAG_ReadRegsAll(hDev, gPCI_ext_CfgRegs,
                     PCI_CFG_EXT_REGS_NUM, True, True)
        elif option == MENU_RW_CFG_SPACE_READ_REG:  # Read from a configuration
                                                  # register
            WDC_DIAG_ReadWriteReg(hDev, gPCI_CfgRegs, PCI_CFG_REGS_NUM,
                WDC_READ, True)
        elif option == MENU_RW_CFG_SPACE_WRITE_REG: # Write to a configuration
                                                  # register
            WDC_DIAG_ReadWriteReg(hDev, gPCI_CfgRegs, PCI_CFG_REGS_NUM,
                WDC_WRITE, True)
        elif option == MENU_RW_CFG_SPACE_EXT_WRITE_REG: # Write to a configuration
                                                     # PCI Express register
            WDC_DIAG_ReadWriteReg(hDev, gPCI_ext_CfgRegs, PCI_CFG_EXT_REGS_NUM,
                WDC_WRITE, True)
        elif option == MENU_RW_CFG_SPACE_EXT_READ_REG:  # Read from a configuration
                                                     # PCI Express register
            WDC_DIAG_ReadWriteReg(hDev, gPCI_ext_CfgRegs, PCI_CFG_EXT_REGS_NUM,
                WDC_READ, True)
        elif option == MENU_RW_CFG_SPACE_SCAN_CAP: # Scan PCI/PCIe capabilities
            WDC_DIAG_ScanPCICapabilities(hDev, fExpress)

#endif # ifndef ISA
# -----------------------------------------------
#    Read/write the run-time registers
#   -----------------------------------------------
# Read/write the run-time registers menu options
(    MENU_RW_REGS_READ_ALL ,
    MENU_RW_REGS_READ_REG,
    MENU_RW_REGS_WRITE_REG) = xrange(1, 4)
MENU_RW_REGS_EXIT = DIAG_EXIT_MENU

# -----------------------------------------------
#   Interrupt handling
# -----------------------------------------------

# Diagnostics interrupt handler routine
def DiagIntHandler(hDev, pIntResult):
    # TODO: You can modify this function in order to implement your own
    #         diagnostics interrupt handler routine

    print("Got interrupt number %ld" % pIntResult.dwCounter)
    print("Interrupt Type: %s" %
        WDC_DIAG_IntTypeDescriptionGet(pIntResult.dwEnabledIntType))
    if WDC_INT_IS_MSI(pIntResult.dwEnabledIntType):
        print("Message Data: 0x%lx" % pIntResult.dwLastMessage)

# Reference to callback to keep it alive (Without this the program will crash
# upon handler call)
gf_DiagIntHandler = PCI_INT_HANDLER(DiagIntHandler)

# Interrupts menu options
MENU_INT_ENABLE_DISABLE = 1
MENU_INT_EXIT = DIAG_EXIT_MENU

# Enable/Disable interrupts menu
def MenuInterrupts(hDev):
    dwIntOptions = WDC_GET_INT_OPTIONS(hDev)
    fIsMsi = WDC_INT_IS_MSI(dwIntOptions)
    if dwIntOptions & INTERRUPT_LEVEL_SENSITIVE:
        # TODO: You can remove this message after you have modified the
        #  implementation of PCI_IntEnable() in pci_lib.c to correctly
        #   acknowledge level-sensitive interrupts (see guidelines in
        #   PCI_IntEnable()).
        print("\n")
        print("WARNING!!!")
        print("----------")
        print("Your hardware has level sensitive interrupts.")
        print("Before enabling the interrupts, %s first modify the source "
            "code\n of PCI_IntEnable(), in the file pci_lib.c, to correctly "
            "acknowledge\n%s interrupts when they occur, as dictated by "
            "the hardware's specification." %
            ("it is recommended that" if fIsMsi else "you must",
            "level sensitive" if fIsMsi else ""))

    while True:
        fIntEnable = not PCI_IntIsEnabled(hDev)

        print("\nInterrupts")
        print("-----------")
        print("%d. %s interrupts" % (MENU_INT_ENABLE_DISABLE, \
        "Enable" if fIntEnable else "Disable"))
        print("%d. Exit menu\n" % MENU_INT_EXIT)

        (option, dwStatus) = DIAG_GetMenuOption(MENU_INT_ENABLE_DISABLE)
        if DIAG_INPUT_FAIL == dwStatus:
            continue

        if option == MENU_INT_EXIT: # Exit menu
            break
        elif option == MENU_INT_ENABLE_DISABLE: # Enable/disable interrupts
            if fIntEnable:
                  dwStatus = PCI_IntEnable(hDev, gf_DiagIntHandler)
                  if WD_STATUS_SUCCESS == dwStatus:
                      print("Interrupts enabled")
                  else:
                      WDC_DIAG_ERR("Failed enabling interrupts. Error [0x%lx - "
                          "%s]\n" % (dwStatus, wdapi.Stat2Str(dwStatus)))
            else:
                  if WD_STATUS_SUCCESS == PCI_IntDisable(hDev):
                      print("Interrupts disabled")
                  else:
                      WDC_DIAG_ERR("Failed disabling interrupts")

# -----------------------------------------------
#    DMA memory handling
# -----------------------------------------------

# DMA menu options
(   MENU_DMA_ALLOCATE_CONTIG,
    MENU_DMA_ALLOCATE_SG,
    MENU_DMA_RESERVED_MEM,
    MENU_DMA_SHARE_CONTIG_BUF,
    MENU_DMA_FREE_MEM) = xrange (1,6)
MENU_DMA_EXIT = DIAG_EXIT_MENU

def FreeDmaMem(pDma):
    if pDma:
        dwStatus = wdapi.WDC_DMABufUnlock(pDma)
        if WD_STATUS_SUCCESS == dwStatus:
            print("DMA memory freed")
            pDma = POINTER(WD_DMA)()
        else:
            WDC_DIAG_ERR("Failed trying to free DMA memory. Error [0x%lx - "
                "%s]\n" % (dwStatus, wdapi.Stat2Str(dwStatus)))

    return pDma

# Allocate/free DMA memory menu
def MenuDma(hDev):
    pBuf = PVOID()
    pDma = POINTER(WD_DMA)()
    while True:
        print
        print("DMA memory")
        print("-----------")
        print("%d. Allocate contiguous memory" % MENU_DMA_ALLOCATE_CONTIG)
        print("%d. Allocate scatter-gather memory" % MENU_DMA_ALLOCATE_SG)
        print("%d. Use reserved memory" % MENU_DMA_RESERVED_MEM)

        if pDma and pDma.contents.dwOptions & DMA_KERNEL_BUFFER_ALLOC:
            print("%d. Send buffer through IPC to all group processes" %
                 MENU_DMA_SHARE_CONTIG_BUF)
#endif # ifndef ISA
        print("%d. Free DMA memory" % MENU_DMA_FREE_MEM)
        print("%d. Exit menu\n" % MENU_DMA_EXIT)

        (option, dwStatus) = DIAG_GetMenuOption(MENU_DMA_FREE_MEM)
        if DIAG_INPUT_FAIL == dwStatus:
            continue

        if option == MENU_DMA_EXIT:
            break
        if option == MENU_DMA_RESERVED_MEM:
            (qwAddr, dwStatus) = DIAG_InputNum("Enter reserved memory address"
            " (64 bit hex uint) ", True, sizeof(UINT64), 1, 0xFFFFFFFFFFFFFFFF)
            if DIAG_INPUT_SUCCESS != dwStatus:
                  continue
        elif option == MENU_DMA_ALLOCATE_CONTIG or \
            option == MENU_DMA_ALLOCATE_SG or \
            option == MENU_DMA_RESERVED_MEM:
            (size, dwStatus) = DIAG_InputNum("Enter memory allocation size in "
            "bytes (32 bit uint) ", True, sizeof(DWORD), 1, 0xFFFFFFFF)
            if DIAG_INPUT_SUCCESS != dwStatus:
                continue

            size = size.value
            # Free DMA memory before trying the new allocation
            pDma = FreeDmaMem(pDma)

        if option == MENU_DMA_ALLOCATE_CONTIG: # Allocate contiguous memory
            dwStatus = wdapi.WDC_DMAContigBufLock(hDev, byref(pBuf), 0, size,
                byref(pDma))
            if WD_STATUS_SUCCESS == dwStatus:
                print("Contiguous memory allocated. user addr [0x%lx], "
                    "physical addr [0x%lx], size [%ld(0x%lx)]\n" %
                    (pDma.contents.pUserAddr,
                    pDma.contents.Page[0].pPhysicalAddr,
                    pDma.contents.Page[0].dwBytes,
                    pDma.contents.Page[0].dwBytes))
            else:
                WDC_DIAG_ERR("Failed allocating contiguous memory. size "
                    "[0x%lx], Error [0x%lx - %s]\n" % (size, dwStatus,
                    wdapi.Stat2Str(dwStatus)))
        elif option == MENU_DMA_ALLOCATE_SG: # Allocate scatter-gather memory
            pBuf = (c_byte * size)() # Allocate a buffer of size "size"
            if not pBuf:
                WDC_DIAG_ERR("Failed allocating user memory for SG. size [%lx]"
                    % size)
                continue
            dwStatus = wdapi.WDC_DMASGBufLock(hDev, byref(pBuf), 0, size,
                byref(pDma))
            if WD_STATUS_SUCCESS == dwStatus:
                print("SG memory allocated. user addr [0x%lx], size [0x%lx]" %
                    (addressof(pBuf), size))
                print("Pages physical addresses:")
                for i in range(pDma.contents.dwPages):
                    print("%lu) physical addr [0x%lx], size [%ld(0x%lx)]" %
                        (i + 1, pDma.contents.Page[i].pPhysicalAddr,
                        pDma.contents.Page[i].dwBytes,
                        pDma.contents.Page[i].dwBytes))
            else:
                WDC_DIAG_ERR("Failed allocating SG memory. size [%ld], Error "
                    "[0x%lx - %s]\n" % (size, dwStatus,
                    wdapi.Stat2Str(dwStatus)))
        elif option == MENU_DMA_RESERVED_MEM:
            dwStatus = wdapi.WDC_DMAReservedBufLock(hDev, qwAddr, byref(pBuf),
                0, size, byref(pDma))
            if WD_STATUS_SUCCESS == dwStatus:
                print(("Reserved memory claimed. user addr [%lx], bus addr "
                    "[0x%lx], size [%ld(0x%lx)]\n") % (addressof(pBuf.contents),
                    pDma.contents.Page[0].pPhysicalAddr,
                    pDma.contents.Page[0].dwBytes,
                    pDma.contents.Page[0].dwBytes))
            else:
                WDC_DIAG_ERR("Failed claiming reserved memory. size [%ld], "
                    "Error [0x%lx - %s]\n" % (size, dwStatus,
                    wdapi.Stat2Str(dwStatus)))
        elif option == MENU_DMA_SHARE_CONTIG_BUF:
            WDS_DIAG_IpcSendDmaContigToGroup(pDma)
#endif # ifndef ISA
        elif option == MENU_DMA_FREE_MEM: # Free DMA memory
            pDma = FreeDmaMem(pDma)
            pBuf = PVOID()

    # Free DMA memory before exiting
    FreeDmaMem(pDma)

# ----------------------------------------------------
#    Plug-and-play and power management events handling
#   ----------------------------------------------------
# Events menu options

MENU_EVENTS_REGISTER_UNREGISTER = 1
MENU_EVENTS_EXIT = DIAG_EXIT_MENU

# Diagnostics plug-and-play and power management events handler routine
def DiagEventHandler(hDev, dwAction):
    # TODO: You can modify this function in order to implement your own
    #        diagnostics events handler routine.

    print("\nReceived event notification (device handle 0x%lX): " % hDev)

    if dwAction == WD_INSERT:
        print("WD_INSERT")
    elif dwAction == WD_REMOVE:
        print("WD_REMOVE")
    elif dwAction == WD_POWER_CHANGED_D0:
        print("WD_POWER_CHANGED_D0")
    elif dwAction == WD_POWER_CHANGED_D1:
        print("WD_POWER_CHANGED_D1")
    elif dwAction == WD_POWER_CHANGED_D2:
        print("WD_POWER_CHANGED_D2")
    elif dwAction == WD_POWER_CHANGED_D3:
        print("WD_POWER_CHANGED_D3")
    elif dwAction == WD_POWER_SYSTEM_WORKING:
        print("WD_POWER_SYSTEM_WORKING")
    elif dwAction == WD_POWER_SYSTEM_SLEEPING1:
        print("WD_POWER_SYSTEM_SLEEPING1")
    elif dwAction == WD_POWER_SYSTEM_SLEEPING2:
        print("WD_POWER_SYSTEM_SLEEPING2")
    elif dwAction == WD_POWER_SYSTEM_SLEEPING3:
        print("WD_POWER_SYSTEM_SLEEPING3")
    elif dwAction == WD_POWER_SYSTEM_HIBERNATE:
        print("WD_POWER_SYSTEM_HIBERNATE")
    elif dwAction == WD_POWER_SYSTEM_SHUTDOWN:
        print("WD_POWER_SYSTEM_SHUTDOWN")
    else:
        print("0x%lx\n" % dwAction)

# Reference to callback to keep it alive
# WARNING: Without this the program will crash upon handler call!
gf_DiagEventHandler = PCI_EVENT_HANDLER(DiagEventHandler)

# Register/unregister plug-and-play and power management events menu
def MenuEvents(hDev):
    while True:
        fRegister = not PCI_EventIsRegistered(hDev)

        print("\n")
        print("Plug-and-play and power management events")
        print("------------------------------------------")
        print("%d. %s events" % (MENU_EVENTS_REGISTER_UNREGISTER,
            "Register" if fRegister else "Unregister"))
        print("%d. Exit menu" % MENU_EVENTS_EXIT)

        (option, dwStatus) = DIAG_GetMenuOption(MENU_EVENTS_REGISTER_UNREGISTER)
        if DIAG_INPUT_FAIL == dwStatus:
            continue

        if option == MENU_EVENTS_EXIT: # Exit menu
            break
        elif option == MENU_EVENTS_REGISTER_UNREGISTER: # Register/unregister events
            if fRegister:
                if WD_STATUS_SUCCESS == PCI_EventRegister(hDev,
                    gf_DiagEventHandler):
                    print("Events registered")
                else:
                    WDC_DIAG_ERR("Failed to register events")
            else:
                if WD_STATUS_SUCCESS == PCI_EventUnregister(hDev):
                    print("Events unregistered")
                else:
                    WDC_DIAG_ERR("Failed to unregister events")

#endif # ifndef ISA

if __name__ == "__main__":
    main()
