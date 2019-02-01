# Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com

from ctypes import *
from wdlib.wdc_lib import *
from wdlib.$$$nl$$$_regs import *
import sys

def TraceLog(s):
    wdapi_va.WDC_Trace("$$$nu$$$ lib TRACE: %s\n" % s )
    #print ("$$$nu$$$ lib TRACE: " + s)

def ErrLog(s):
    wdapi_va.WDC_Err("$$$nu$$$ lib ERROR: %s\n" % s)
    #print ("$$$nu$$$ lib ERROR: " + s)

KP_$$$nu$$$_DRIVER_NAME = "KP_$$$nu$$$"

# Kernel PlugIn messages - used in WDC_CallKerPlug() calls (user mode)
# KP_$$$nu$$$_Call() (kernel mode)
KP_$$$nu$$$_MSG_VERSION = 1

# Kernel PlugIn messages status
KP_$$$nu$$$_STATUS_OK          = 0x1
KP_$$$nu$$$_STATUS_MSG_NO_IMPL = 0x1000
KP_$$$nu$$$_STATUS_FAIL        = 0x1001

#ifndef ISA
# Default vendor and device IDs (0 == all)
# TODO: Replace the ID values with your device's vendor and device IDs
$$$nu$$$_DEFAULT_VENDOR_ID = 0x0 # Vendor ID
$$$nu$$$_DEFAULT_DEVICE_ID = 0x0 # Device ID
#else # ifdef ISA
## ISA address spaces information
#$$$nu$$$_ADDR_SPACES_NUM = 0 # Number of address spaces

## Base address spaces numbers
@@@BaseAddrSpaceNum@@@

## Physical base addresses
@@@PhysicalBaseAddresses@@@

## Size (in bytes) of address spaces
@@@SizeOfAddrSpaces@@@

##ifdef HAS_INTS
## Interrupts information
#$$$nu$$$_INTS_NUM = 1 # Number of interrupts

## Interrupt Request (IRQ)
# @@@Irqs@@@

## Interrupt registration options
@@@InterruptOptions@@@
##endif # ifdef HAS_INTS

## TODO: Add address space info
## Total number of items - address spaces, interrupts and bus items
#$$$nu$$$_ITEMS_NUM = $$$nu$$$_ADDR_SPACES_NUM + 1
##endif # ifdef ISA


$$$nu$$$_DEFAULT_LICENSE_STRING = "$$$lic$$$"
$$$nu$$$_DEFAULT_DRIVER_NAME = WD_DEFAULT_DRIVER_NAME_BASE

# Kernel PlugIn version information struct
class KP_$$$nu$$$_VERSION(Structure): _fields_ = \
    [("dwVer", DWORD),
    ("cVer", CHAR * 100)]

# Device address description struct
PWDC_ADDR_DESC = POINTER(WDC_ADDR_DESC)
class $$$nu$$$_DEV_ADDR_DESC(Structure): _fields_ = \
    [("dwNumAddrSpaces",DWORD),    # Total number of device address spaces
    ("pAddrDesc", PWDC_ADDR_DESC)] # Array of device address spaces information

# Address space information struct
MAX_TYPE = 8
class $$$nu$$$_ADDR_SPACE_INFO(Structure): _fields_ = \
    [("dwAddrSpace", DWORD),
    ("sType", CHAR * MAX_TYPE),
    ("sName", CHAR * MAX_NAME),
    ("sDesc", CHAR * MAX_DESC)]

# Interrupt result information struct
class $$$nu$$$_INT_RESULT(Structure): _fields_ = \
    [("dwCounter", DWORD), # Number of interrupts received
    ("dwLost", DWORD),     # Number of interrupts not yet handled
    ("waitResult", WD_INTERRUPT_WAIT_RESULT), # See WD_INTERRUPT_WAIT_RESULT values
                                              #    in windrvr.h
    ("dwEnabledIntType", DWORD), # Interrupt type that was actually enabled
                                 #   (MSI/MSI-X / Level Sensitive / Edge-Triggered)
    ("dwLastMessage", DWORD)]    # Message data of the last received MSI/MSI-X
                                 # (Windows Vista and higher) N/A to line-based
                                 #  interrupts.

# TODO: You can add fields to $$$nu$$$_INT_RESULT to store any additional
#        information that you wish to pass to your diagnostics interrupt
#         handler routine (DiagIntHandler() in $$$nl$$$_diag.py).

# $$$nu$$$ diagnostics interrupt handler function type
#typedef void (*$$$nu$$$_INT_HANDLER)(WDC_DEVICE_HANDLE hDev,
#    $$$nu$$$_INT_RESULT *pIntResult)
$$$nu$$$_INT_HANDLER = DLLCALLCONV(None, WDC_DEVICE_HANDLE, $$$nu$$$_INT_RESULT)

# $$$nu$$$ diagnostics plug-and-play and power management events handler function
# type
#typedef void (*$$$nu$$$_EVENT_HANDLER)(WDC_DEVICE_HANDLE hDev, DWORD dwAction)
$$$nu$$$_EVENT_HANDLER = DLLCALLCONV(None, WDC_DEVICE_HANDLE, DWORD)

@@@regs_lib_enums@@@
# $$$nu$$$ device information struct
class $$$nu$$$_DEV_CTX(Structure): _fields_ = \
    [
    #ifdef HAS_INTS
    @@@pIntTransCmds@@@
    ("funcDiagIntHandler", $$$nu$$$_INT_HANDLER),    # Interrupt handler routine
    #endif # ifdef HAS_INTS
    #ifndef ISA
    ("funcDiagEventHandler", $$$nu$$$_EVENT_HANDLER), # Event handler routine
    #else # ifdef ISA
    #ifndef HAS_INTS
    #("pData", PVOID)
    # TODO: Set pData to point to any device-specific data that you wish to
    #        store or replace pData with your own device context information
    #endif # ifndef HAS_INTS
    #endif # ifdef ISA
    ]
P$$$nu$$$_DEV_CTX = POINTER($$$nu$$$_DEV_CTX)

class KP_$$$nu$$$_VERSION(Structure): _fields_ = \
    [("dwVer", DWORD),
     ("cVer", CHAR * 100)]

# *************************************************************
#  Global variables definitions
# *************************************************************
# Library initialization reference count
LibInit_count = 0

# Validate a device handle
def IsValidDevice(hDev, sFunc):
    if not hDev or not wdapi.WDC_GetDevContext(hDev):
        ErrLog ("%s: NULL device %s\n" % (sFunc, "handle" if not pDev else
            "context"))
        return False

    return True

def $$$nu$$$_LibInit():
    try:
        # Increase the library's reference count initialize the library only once
        LibInit_count += 1
    except:
        LibInit_count = 0
    if LibInit_count > 1:
        return WD_STATUS_SUCCESS

    #ifdef WD_DRIVER_NAME_CHANGE
    # Set the driver name
    if not wdapi.WD_DriverName($$$nu$$$_DEFAULT_DRIVER_NAME):
        ErrLog("Failed to set the driver name for WDC library.\n")
        return WD_SYSTEM_INTERNAL_ERROR
    #endif

    # Set WDC library's debug options
    # (default: level=TRACE redirect output to the Debug Monitor)
    dwStatus = wdapi.WDC_SetDebugOptions(WDC_DBG_DEFAULT, 0)
    if WD_STATUS_SUCCESS != dwStatus:
        ErrLog("Failed to initialize debug options for WDC library.\n"
            "Error 0x%lx - %s\n" % (dwStatus, wdapi.Stat2Str(dwStatus)))
        return dwStatus

    # Open a handle to the driver and initialize the WDC library
    dwStatus = wdapi.WDC_DriverOpen(WDC_DRV_OPEN_DEFAULT,
        $$$nu$$$_DEFAULT_LICENSE_STRING)
    if WD_STATUS_SUCCESS != dwStatus:
        ErrLog("Failed to initialize the WDC library. Error 0x%lx - %s\n" %
            (dwStatus, wdapi.Stat2Str(dwStatus)))
        return dwStatus

    return WD_STATUS_SUCCESS

def $$$nu$$$_LibUninit():
    global LibInit_count
    # Decrease the library's reference count uninitialize the library only
    # when there are no more open handles to the library
    LibInit_count -= 1
    if LibInit_count > 0:
        return WD_STATUS_SUCCESS

    # Uninitialize the WDC library and close the handle to WinDriver
    dwStatus = wdapi.WDC_DriverClose()
    if WD_STATUS_SUCCESS != dwStatus:
        ErrLog("Failed to uninit the WDC library. Error 0x%lx - %s\n" %
            (dwStatus, wdapi.Stat2Str(dwStatus)))

    return dwStatus

# -----------------------------------------------
#    Device open/close
# -----------------------------------------------
# Open a device handle

def $$$nu$$$_DeviceOpen(deviceInfo):
    global gpDevCtx
    hDev = WDC_DEVICE_HANDLE()
    gpDevCtx = $$$nu$$$_DEV_CTX()
    @@@kp@@@
    devAddrDesc = $$$nu$$$_DEV_ADDR_DESC()
    @@@kp@@@
#ifndef ISA
    # Validate arguments
    if not deviceInfo:
        ErrLog("$$$nu$$$_DeviceOpen: Error - NULL device information "
            "struct pointer\n")
        return None

    # Open a device handle
    dwStatus = wdapi.WDC_$$$nl$$$DeviceOpen(byref(hDev), byref(deviceInfo),
        byref(gpDevCtx))
#else # ifdef ISA
#    # Set the device's resources information
#    SetDeviceResources(deviceInfo)
#
#    # Open a device handle
#    dwStatus = wdapi.WDC_IsaDeviceOpen(byref(hDev), byref(deviceInfo),
#        byref(gpDevCtx))

#endif # ifdef ISA
    if WD_STATUS_SUCCESS != dwStatus:
        ErrLog("Failed opening a WDC device handle. Error 0x%lx - %s\n" %
            (dwStatus, wdapi.Stat2Str(dwStatus)))
        return $$$nu$$$_DeviceOpen_err(hDev)
    pDev = cast(hDev, PWDC_DEVICE)

    @@@kp@@@
    devAddrDesc.dwNumAddrSpaces = pDev.contents.dwNumAddrSpaces
    devAddrDesc.pAddrDesc = pDev.contents.pAddrDesc

    # Open a handle to a Kernel PlugIn driver
    wdapi.WDC_KernelPlugInOpen(hDev, KP_$$$nu$$$_DRIVER_NAME, byref(devAddrDesc))
    @@@kp@@@
#ifndef ISA
    # Validate device information
    if not DeviceValidate(pDev):
        return $$$nu$$$_DeviceOpen_err(hDev)
#endif # ifndef ISA
    # Return handle to the new device
    TraceLog("$$$nu$$$_DeviceOpen: Opened a $$$nu$$$ device (handle 0x%lx)\n"
        "Device is %susing a Kernel PlugIn driver (%s)\n" % (hDev.value,
        "" if WDC_IS_KP(pDev) else "not" , KP_$$$nu$$$_DRIVER_NAME))
    return hDev

def $$$nu$$$_DeviceOpen_err(hDev):
    if hDev:
        $$$nu$$$_DeviceClose(hDev)

    return None

# Close device handle
def $$$nu$$$_DeviceClose(hDev):
    global gpDevCtx
    TraceLog("$$$nu$$$_DeviceClose: Entered. Device handle 0x%lx\n" % hDev.value)

    # Validate the device handle
    if not hDev:
        ErrLog("$$$nu$$$_DeviceClose: Error - NULL device handle\n")
        return False

#ifdef HAS_INTS
    # Disable interrupts (if enabled)
    if wdapi.WDC_IntIsEnabled(hDev):
        dwStatus = $$$nu$$$_IntDisable(hDev)
        if WD_STATUS_SUCCESS != dwStatus:
            ErrLog("Failed disabling interrupts. Error 0x%lx - %s\n" %
                (dwStatus, wdapi.Stat2Str(dwStatus)))
#endif # ifdef HAS_INTS

    # Close the device handle
    dwStatus = wdapi.WDC_$$$nl$$$DeviceClose(hDev)
    if WD_STATUS_SUCCESS != dwStatus:
        ErrLog("Failed closing a WDC device handle (0x%lx). Error 0x%lx - %s\n",
            hDev, dwStatus, wdapi.Stat2Str(dwStatus))

    if gpDevCtx:
        gpDevCtx = $$$nu$$$_DEV_CTX()

    return WD_STATUS_SUCCESS == dwStatus

#ifndef ISA
# Validate device information
def DeviceValidate(pDev):
    # NOTE: You can modify the implementation of this function in order to
    #         verify that the device has the resources you expect to find.

    # Verify that the device has at least one active address space
    for i in range(pDev.contents.dwNumAddrSpaces):
        if wdapi.WDC_AddrSpaceIsActive(pDev, i):
            TraceLog("Address space number %d is active" % i)
            return True

    # In this sample we accept the device even if it doesn't have any
    # address spaces
    TraceLog("Device does not have any active memory or I/O address spaces\n")
    return True
#else # ifdef ISA
#def SetDeviceResources(DeviceInfo):
#    DeviceInfo.dwItems = $$$nu$$$_ITEMS_NUM
#    pItem = DeviceInfo.Item[0]
#    # Bus
#    pItem.item = ITEM_BUS
#    pItem.I.Bus.dwBusType = WD_BUS_ISA

@@@devResources@@@
#endif # ifdef ISA

#ifdef HAS_INTS
# -----------------------------------------------
#    Interrupts
# -----------------------------------------------

# Reference to $$$nu$$$_INT_RESULT to keep it alive
# WARNING: Without this the program will crash upon handler call
gf_intResult = $$$nu$$$_INT_RESULT()

# Interrupt handler routine
def $$$nu$$$_IntHandler(pData):
    pDev = cast(pData, PWDC_DEVICE)
    pDevCtx = cast(wdapi.WDC_GetDevContext(pData), P$$$nu$$$_DEV_CTX)

    gf_intResult.dwCounter = pDev.contents.Int.dwCounter
    gf_intResult.dwLost = pDev.contents.Int.dwLost
    gf_intResult.waitResult = pDev.contents.Int.fStopped
    gf_intResult.dwEnabledIntType = WDC_GET_ENABLED_INT_TYPE(pDev)
    gf_intResult.dwLastMessage = WDC_GET_ENABLED_INT_LAST_MSG(pDev)

    # Execute the diagnostics application's interrupt handler routine
    pDevCtx.contents.funcDiagIntHandler(cast(pData, WDC_DEVICE_HANDLE),
        gf_intResult)

# Check whether a given device contains an item of the specified type
def IsItemExists(pDev, item):
    for i in range(pDev.contents.cardReg.Card.dwItems):
        if pDev.contents.cardReg.Card.Item[i].item == item:
            return True

    return False

#defined as global to keep the variable alive
gpTrans = (WD_TRANSFER * WD_TRANSFER_CMDS)()

# Enable interrupts
def $$$nu$$$_IntEnable(hDev, funcIntHandler):
    pDev = cast(hDev, PWDC_DEVICE)
    dwNumTransCmds = 0
    dwOptions = 0
    global gpTrans

    TraceLog("$$$nu$$$_IntEnable: Entered. Device handle 0x%lx\n" % hDev.value)

    # Validate the device handle
    if not IsValidDevice(hDev, "$$$nu$$$_IntEnable"):
        return WD_INVALID_PARAMETER

    # Verify that the device has an interrupt item
    if not IsItemExists(pDev, ITEM_INTERRUPT):
        return WD_OPERATION_FAILED

    pDevCtx = cast(wdapi.WDC_GetDevContext(hDev), P$$$nu$$$_DEV_CTX)

    # Check whether interrupts are already enabled
    if wdapi.WDC_IntIsEnabled(hDev):
        ErrLog("Interrupts are already enabled ...\n")
        return WD_OPERATION_ALREADY_DONE

    @@@transfer_cmds@@@

    # When using a Kernel PlugIn, acknowledge interrupts in kernel mode
    if not WDC_IS_KP(pDev):
        # TODO: Change this value, if needed
        dwNumTransCmds = 2

        # This sample demonstrates how to set up two transfer commands, one
        # for reading the device's INTCSR register (as defined in g$$$nu$$$_Regs)
        # and one for writing to it to acknowledge the interrupt. The transfer
        # commands will be executed by WinDriver in the kernel when an
        # interrupt occurs.
#ifndef ISA
        # TODO: If $$$nu$$$ interrupts are level sensitive interrupts, they must be
        # acknowledged in the kernel immediately when they are received. Since
        # the information for acknowledging the interrupts is
        # hardware-specific, YOU MUST MODIFY THE CODE below and set up transfer
        # commands in order to correctly acknowledge the interrupts on your
        # device, as dictated by your hardware's specifications.
        # If the device supports both MSI/MSI-X and level sensitive interrupts,
        # you must set up transfer commands in order to allow your code to run
        # correctly on systems other than Windows Vista and higher and Linux.
        # Since MSI/MXI-X does not require acknowledgment of the interrupt, to
        # support only MSI/MSI-X handling (for hardware and OSs that support
        # this), you can avoid defining transfer commands, or specify
        # kernel-mode commands to be performed upon interrupt generation
        # according to your specific needs.
#endif # ifndef ISA

        #*****************************************************************
        # NOTE: If you attempt to use this code without first modifying it in
        # order to correctly acknowledge your device's level-sensitive
        # interrupts, as explained above, the OS will HANG when a level
        # sensitive interrupt occurs!
        #******************************************************************

        # Prepare the interrupt transfer commands.
        #
        # The transfer commands will be executed by WinDriver's ISR
        # which runs in kernel mode at interrupt level.

        # TODO: Change the offset of INTCSR and the $$$nu$$$ address space, if
        #       needed
        # #1: Read status from the INTCSR register
        pAddrDesc = WDC_GET_ADDR_DESC(pDev, INTCSR_ADDR_SPACE)
        gpTrans[0].pPort = pAddrDesc.pAddr + INTCSR
        # Read from a 32-bit register
        gpTrans[0].cmdTrans = RM_DWORD if WDC_ADDR_IS_MEM(pAddrDesc) else \
            RP_DWORD

        # #2: Write ALL_INT_MASK to the INTCSR register to acknowledge the
        #     interrupt
        gpTrans[1].pPort = gpTrans[0].pPort # In this example both commands
                                    # access the same address
                                    # (register)
        # Write to a 32-bit register
        gpTrans[1].cmdTrans = WM_DWORD if WDC_ADDR_IS_MEM(pAddrDesc) else \
            WP_DWORD
        gpTrans[1].Data.Dword = ALL_INT_MASK

        # Copy the results of "read" transfer commands back to user mode
        dwOptions = INTERRUPT_CMD_COPY

    # Store the diag interrupt handler routine, which will be executed by
    #   $$$nu$$$_IntHandler() when an interrupt is received
    pDevCtx.contents.funcDiagIntHandler = funcIntHandler

    # Enable interrupts
    dwStatus = wdapi.WDC_IntEnable(hDev, gpTrans, dwNumTransCmds, dwOptions,
        DLLCALLCONV(None, PVOID)($$$nu$$$_IntHandler), hDev, WDC_IS_KP(pDev))
    if WD_STATUS_SUCCESS != dwStatus:
        ErrLog("Failed enabling interrupts. Error 0x%lx - %s\n" % (dwStatus,
            wdapi.Stat2Str(dwStatus)))
        if gpTrans:
            gpTrans = (WD_TRANSFER * WD_TRANSFER_CMDS)()

        return dwStatus

    # Store the interrupt transfer commands in the device context
    pDevCtx.pIntTransCmds = gpTrans
    @@@transfer_cmds@@@
    # TODO: You can add code here to write to the device in order to
    #         physically enable the hardware interrupts.

    TraceLog("$$$nu$$$_IntEnable: Interrupts enabled\n")

    return WD_STATUS_SUCCESS

# Disable interrupts
def $$$nu$$$_IntDisable(hDev):
    TraceLog("$$$nu$$$_IntDisable entered. Device handle 0x%lx\n" % hDev.value)

    # Validate the device handle
    if not IsValidDevice(hDev, "$$$nu$$$_IntDisable"):
        return WD_INVALID_PARAMETER

    pDevCtx = cast(wdapi.WDC_GetDevContext(hDev), P$$$nu$$$_DEV_CTX)

    # Check whether interrupts are already enabled
    if not wdapi.WDC_IntIsEnabled(hDev):
        ErrLog("Interrupts are already disabled ...\n")
        return WD_OPERATION_ALREADY_DONE

    # TODO: You can add code here to write to the device in order to
    #         physically disable the hardware interrupts.

    # Disable interrupts
    dwStatus = wdapi.WDC_IntDisable(hDev)
    if WD_STATUS_SUCCESS != dwStatus:
        ErrLog("Failed disabling interrupts. Error 0x%lx - %s\n" %
            (dwStatus, wdapi.Stat2Str(dwStatus)))

    return dwStatus

# Check whether interrupts are enabled for the given device
def $$$nu$$$_IntIsEnabled(hDev):
    # Validate the device handle
    if not IsValidDevice(cast(hDev, PWDC_DEVICE), "$$$nu$$$_IntIsEnabled"):
        return False

    # Check whether interrupts are already enabled
    return wdapi.WDC_IntIsEnabled(hDev)
#endif # ifdef HAS_INTS

#ifndef ISA
# -----------------------------------------------
#    Plug-and-play and power management events
# -----------------------------------------------

# Plug-and-play or power management event handler routine
def $$$nu$$$_EventHandler(pEvent, pData):
    hDev = cast(pData, WDC_DEVICE_HANDLE)
    pDevCtx = cast(wdapi.WDC_GetDevContext(hDev), P$$$nu$$$_DEV_CTX)

    TraceLog("$$$nu$$$_EventHandler entered, pData 0x%lx, dwAction 0x%lx\n" % \
        (pData, pEvent.contents.dwAction))

    # Execute the diagnostics application's event handler function
    pDevCtx.contents.funcDiagEventHandler(hDev, pEvent.contents.dwAction)

# Reference to callback to keep it alive
# WARNING: Without this the program will crash upon handler call!
gf_$$$nu$$$_EventHandler = \
    DLLCALLCONV(None, POINTER(WD_EVENT), PVOID)($$$nu$$$_EventHandler)

# Register a plug-and-play or power management event
def $$$nu$$$_EventRegister(hDev, funcEventHandler):
    pDev = cast(hDev, PWDC_DEVICE)
    dwActions = WD_ACTIONS_ALL
    # TODO: Modify the above to set up the plug-and-play/power management
    #       events for which you wish to receive notifications.
    #       dwActions can be set to any combination of the WD_EVENT_ACTION
    #       flags defined in windrvr.py

    TraceLog("$$$nu$$$_EventRegister entered. Device handle 0x%lx\n" % hDev.value)

    # Validate the device handle
    if not IsValidDevice(hDev, "$$$nu$$$_EventRegister"):
        return WD_INVALID_PARAMETER

    pDevCtx = cast(wdapi.WDC_GetDevContext(hDev), P$$$nu$$$_DEV_CTX)

    # Check whether the event is already registered
    if wdapi.WDC_EventIsRegistered(hDev):
        ErrLog("Events are already registered ...\n")
        return WD_OPERATION_ALREADY_DONE

    # Store the diag event handler routine to be executed from
    # $$$nu$$$_EventHandler() upon an event
    pDevCtx.contents.funcDiagEventHandler = funcEventHandler

    # Register the event
    pDev = cast(hDev, PWDC_DEVICE)

    dwStatus = wdapi.WDC_EventRegister(hDev, dwActions,
        gf_$$$nu$$$_EventHandler, hDev, WDC_IS_KP(pDev))

    if WD_STATUS_SUCCESS != dwStatus:
        ErrLog("Failed to register events. Error 0x%lx - %s\n" %
            (dwStatus, wdapi.Stat2Str(dwStatus)))
        return dwStatus

    TraceLog("Events registered\n")
    return WD_STATUS_SUCCESS

# Unregister a plug-and-play or power management event
def $$$nu$$$_EventUnregister(hDev):
    TraceLog("$$$nu$$$_EventUnregister entered. Device handle 0x%lx\n" % hDev.value)

    # Validate the device handle
    if not IsValidDevice(cast(hDev, PWDC_DEVICE), "$$$nu$$$_EventUnregister"):
        return WD_INVALID_PARAMETER

    # Check whether the event is currently registered */
    if not wdapi.WDC_EventIsRegistered(hDev):
        ErrLog("Cannot unregister events - no events currently "
            "registered ...\n")
        return WD_OPERATION_ALREADY_DONE

    # Unregister the event
    dwStatus = wdapi.WDC_EventUnregister(hDev)
    if WD_STATUS_SUCCESS != dwStatus:
        ErrLog("Failed to unregister events. Error 0x%lx - %s\n" % (dwStatus,
            wdapi.Stat2Str(dwStatus)))

    return dwStatus

# Check whether a given plug-and-play or power management event is registered

def $$$nu$$$_EventIsRegistered(hDev):
    # Validate the device handle
    if not IsValidDevice(hDev, "$$$nu$$$_EventIsRegistered"):
        return False

    # Check whether the event is registered #
    return wdapi.WDC_EventIsRegistered(hDev)

#endif # ifndef ISA

# -----------------------------------------------
#    Address spaces information
# -----------------------------------------------
#ifndef ISA
# Get number of address spaces
def $$$nu$$$_GetNumAddrSpaces(hDev):
    pDev = cast(hDev, PWDC_DEVICE)
    # Validate the device handle
    if not IsValidDevice(hDev, "$$$nu$$$_GetNumAddrSpaces"):
        return 0

    # Return the number of address spaces for the device
    return pDev.contents.dwNumAddrSpaces
#endif # ifndef ISA

# Get address space information
def $$$nu$$$_GetAddrSpaceInfo(hDev, addrSpaceInfo):
    pDev = cast(hDev, PWDC_DEVICE)

    @@@devValidate@@@
    dwAddrSpace = addrSpaceInfo.dwAddrSpace

    if dwAddrSpace > pDev.contents.dwNumAddrSpaces - 1:
        ErrLog("$$$nu$$$_GetAddrSpaceInfo: Error - Address space %ld is "
            "out of range (0 - %ld)\n" % (dwAddrSpace,
            pDev.contents.dwNumAddrSpaces - 1))
        return False

    addrDesc = pDev.contents.pAddrDesc[dwAddrSpace]

    fIsMemory = WDC_ADDR_IS_MEM(addrDesc)

#ifndef ISA
    addrSpaceInfo.sName = "BAR %ld" % dwAddrSpace
#else # ifdef ISA
#    addrSpaceInfo.sName = "AddrSpace %ld" % dwAddrSpace
#endif # ifdef ISA
    addrSpaceInfo.sType = "Memory" if fIsMemory else "I/O"

    if wdapi.WDC_AddrSpaceIsActive(pDev, dwAddrSpace):
        item = pDev.contents.cardReg.Card.Item[addrDesc.dwItemIndex]
        pAddr = item.I.Mem.pPhysicalAddr if fIsMemory \
            else item.I.IO.pAddr

        addrSpaceInfo.sDesc = (("0x%0*lX - 0x%0*lX (0x%lx bytes)") %
            (WDC_SIZE_64 * 2, pAddr, WDC_SIZE_64 * 2,
            pAddr + addrDesc.qwBytes - 1, addrDesc.qwBytes))
    else:
        addrSpaceInfo.sDesc = "Inactive address space"

    # TODO: You can modify the code above to set a different address space
    # name/description.

    return True
