# Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com

import sys, os
sys.path.append(os.path.dirname(os.path.abspath(__file__)) + "/..")
from wdlib.$$$nl$$$_diag_lib import *

# TODO: Change the following definitions to match your device.
DEFAULT_VENDOR_ID  = 0x0
DEFAULT_PRODUCT_ID = 0x0

DEFAULT_LICENSE_STRING = "$$$lic$$$"

# TODO: Change the following definition to your driver's name
DEFAULT_DRIVER_NAME  = WD_DEFAULT_DRIVER_NAME_BASE

INFINITE              = 0xffffffff
ATTACH_EVENT_TIMEOUT  = 30     # in seconds
TRANSFER_TIMEOUT      = 30000  # in msecs
WD_PYTHON_$$$nu$$$_DEVICES = 256

class DEVICE_CONTEXT(Structure): _fields_ = \
    [("hDevice", WDU_DEVICE_HANDLE),
    ("dwVendorId", DWORD),
    ("dwProductId", DWORD),
    ("dwInterfaceNum", DWORD),
    ("dwAlternateSetting", DWORD)]

class DRIVER_CONTEXT(Structure): _fields_ = \
    [("hEvent", HANDLE),
    ("hMutex", HANDLE),
    ("dwDeviceCount", DWORD),
    ("dwActiveDev", DWORD),
    ("hDeviceUnusedEvent", HANDLE),
    ("DevCtxArray", (DEVICE_CONTEXT * WD_PYTHON_$$$nu$$$_DEVICES))]

hDriver = WDU_DRIVER_HANDLE()

# Driver context must be global in order for Python not to free it during
# the program's run
DrvCtx = DRIVER_CONTEXT()

def DeviceAttach(hDevice, pDeviceInfo, pUserData):
    pDrvCtx = cast(pUserData, POINTER(DRIVER_CONTEXT))
    pActiveAltSetting = \
        pDeviceInfo.contents.pActiveInterface[0].contents.pActiveAltSetting
    dwInterfaceNum = pActiveAltSetting.contents.Descriptor.bInterfaceNumber
    dwAlternateSetting = pActiveAltSetting.contents.Descriptor.bAlternateSetting

    # NOTE: To change the alternate setting, call wdapi.WDU_SetInterface() here
    '''
    # TODO: Replace with the requested number:
    #dwAlternateSetting = %alternate_setting_number%

    dwAttachError = wdapi.WDU_SetInterface(hDevice, dwInterfaceNum,
        dwAlternateSetting)
    if dwAttachError:
        ERR("DeviceAttach: WDU_SetInterface() failed (num. %ld, alternate %ld) "
            "device 0x%lx. error 0x%lx (\"%s\")\n" % (dwInterfaceNum,
            dwAlternateSetting, hDevice, dwAttachError,
            wdapi.Stat2Str(dwAttachError)))
        return False
    '''

    TRACE("\nDeviceAttach: Received and accepted attach for vendor id 0x%x, "
        "product id 0x%x, interface %ld, device handle 0x%lx\n" %
        (pDeviceInfo.contents.Descriptor.idVendor,
        pDeviceInfo.contents.Descriptor.idProduct, dwInterfaceNum, hDevice))

    # Add our device to device array
    wdapi.OsMutexLock(HANDLE(pDrvCtx.contents.hMutex))

    for i in range(WD_PYTHON_$$$nu$$$_DEVICES):
        if not pDrvCtx.contents.DevCtxArray[i].dwVendorId and \
            not pDrvCtx.contents.DevCtxArray[i].dwProductId:
            break

    pDrvCtx.contents.DevCtxArray[i].hDevice = WDU_DEVICE_HANDLE(hDevice)
    pDrvCtx.contents.DevCtxArray[i].dwInterfaceNum = DWORD(dwInterfaceNum)
    pDrvCtx.contents.DevCtxArray[i].dwVendorId = \
        pDeviceInfo.contents.Descriptor.idVendor
    pDrvCtx.contents.DevCtxArray[i].dwProductId = \
        pDeviceInfo.contents.Descriptor.idProduct
    pDrvCtx.contents.DevCtxArray[i].dwAlternateSetting = \
        DWORD(dwAlternateSetting)

    pDrvCtx.contents.dwDeviceCount += 1
    wdapi.OsMutexUnlock(HANDLE(pDrvCtx.contents.hMutex))

    wdapi.OsEventSignal(HANDLE(pDrvCtx.contents.hEvent))

    # Accept control over this device
    return True

def DeviceDetach(hDevice, pUserData):
    pDrvCtx = cast(pUserData, POINTER(DRIVER_CONTEXT))
    bDetachActiveDev = False

    TRACE("\nDeviceDetach: Received detach for device handle 0x%lx\n" % hDevice)

    wdapi.OsMutexLock(pDrvCtx.contents.hMutex)

    #find device to remove
    for i in range(WD_PYTHON_$$$nu$$$_DEVICES):
        if pDrvCtx.contents.DevCtxArray[i].hDevice == hDevice:
            break
    if i == WD_PYTHON_$$$nu$$$_DEVICES:
        ERR("\nDeviceDetach: No device to remove")
        return

    if i == DrvCtx.dwActiveDev:
        bDetachActiveDev = True
        #find other device to activate if there is more than one connected
        if pDrvCtx.contents.dwDeviceCount > 1:
            for j in range(WD_PYTHON_$$$nu$$$_DEVICES):
                if pDrvCtx.contents.DevCtxArray[j].hDevice:
                    DrvCtx.dwActiveDev = j
        else:
            pDrvCtx.contents.dwActiveDev = 0

    pDrvCtx.contents.DevCtxArray[i] = DEVICE_CONTEXT()
    pDrvCtx.contents.dwDeviceCount -= 1
    wdapi.OsMutexUnlock(pDrvCtx.contents.hMutex)

    if bDetachActiveDev:
        # When hDeviceUnusedEvent is not signaled, hDevice is possibly in use,
        # and therefore the detach callback needs to wait on it until it is
        # certain that it cannot be used.
        # When it is signaled - hDevice is no longer used.
        if pDrvCtx.contents.dwDeviceCount <= 0:
            TRACE("No devices are connected.\nPlease connect a device or press "
                "CTRL+C to quit application")
        wdapi.OsEventWait(pDrvCtx.contents.hDeviceUnusedEvent, INFINITE)

def FastStreamingRead(DrvCtx):
    Device = WDU_DEVICE()
    pDevice = POINTER(WDU_DEVICE)(Device)
    dwBufferSize = 0x20000

    (dwInterfaceNumber, dwStatus) = DIAG_InputNum("Please enter the interface"\
        " number (dec): ", False, sizeof(DWORD), 0, 0)
    if DIAG_INPUT_SUCCESS != dwStatus:
        return

    (dwAlternateSetting, dwStatus) = DIAG_InputNum("Please enter the"\
        " alternate setting index (dec): ", False, sizeof(DWORD), 0, 0)
    if DIAG_INPUT_SUCCESS != dwStatus:
        return

    (dwPipeNum, dwStatus) = DIAG_InputNum("Please enter the"\
        " pipe number (hex): ", True, sizeof(DWORD), 0, 0)
    if DIAG_INPUT_SUCCESS != dwStatus:
        return

    wdapi.OsMutexLock(HANDLE(DrvCtx.hMutex))
    hDevice = DrvCtx.DevCtxArray[DrvCtx.dwActiveDev].hDevice
    wdapi.OsMutexUnlock(HANDLE(DrvCtx.hMutex))

    dwError = wdapi.WDU_SetInterface(hDevice, dwInterfaceNumber,
        dwAlternateSetting)
    if dwError:
        ERR("FastStreamingRead: WDU_SetInterface() failed. error 0x%lx "
            "(\"%s\")" % (dwError, wdapi.Stat2Str(dwError)))
        return
    else:
        TRACE("FastStreamingRead: WDU_SetInterface() completed "
            "successfully")
        DrvCtx.DevCtxArray[DrvCtx.dwActiveDev].dwInterfaceNum = \
            dwInterfaceNumber
        DrvCtx.DevCtxArray[DrvCtx.dwActiveDev].dwAlternateSetting = \
            dwAlternateSetting

    dwError = wdapi.WDU_GetDeviceInfo(hDevice, byref(pDevice))
    if dwError:
        ERR("FastStreamingRead: WDU_GetDeviceInfo() failed. error 0x%lx "
            "(\"%s\")" % (dwError, wdapi.Stat2Str(dwError)))
        return

    # Search for the pipe
    pPipe = FindPipeInDevice(pDevice, dwPipeNum.value)
    if not pPipe:
        ERR("FastStreamingRead: Pipe number 0x%lx does not exist" % \
            dwPipeNum.value)
        return

    if not dwPipeNum or pPipe.type == PIPE_TYPE_CONTROL:
        ERR("FastStreamingRead: Cannot listen to control pipes")
        return

    ListenToPipe(hDevice, pPipe, True, dwBufferSize, False)

def DeviceDiagMenu(DrvCtx):
    while True:
        if not DrvCtx.dwDeviceCount:
            print("No Devices are currently connected.")
            if sys.platform == "win32":
                print("Please make sure that you've generated and installed an "
                    "INF file for this device using WinDriver DriverWizard.")
            print("Press Enter to re check or enter EXIT to exit")
            line = raw_input()

            if line == "EXIT":
                break

        dwActiveDev = DrvCtx.dwActiveDev
        wdapi.OsMutexLock(HANDLE(DrvCtx.hMutex))

        if not DrvCtx.dwDeviceCount:
            wdapi.OsMutexUnlock(HANDLE(DrvCtx.hMutex))
            continue

        print("")
        print("Main Menu (active Dev/Prod/Interface/Alt. Setting: "
            "0x%lx/0x%lx/%ld/%ld)" % \
            (DrvCtx.DevCtxArray[dwActiveDev].dwVendorId,
            DrvCtx.DevCtxArray[dwActiveDev].dwProductId,
            DrvCtx.DevCtxArray[dwActiveDev].dwInterfaceNum,
            DrvCtx.DevCtxArray[dwActiveDev].dwAlternateSetting))
        print("----------")
        print("1. Display device configurations")
        print("2. Change interface alternate setting")
        print("3. Reset Pipe")
        print("4. Read/Write from pipes")
        if sys.platform == "win32": # Streaming is supported only on Windows
            print("5. Fast streaming read")
        if DrvCtx.dwDeviceCount > 1:
            print("6. Select device")
        if sys.platform == "win32":
            print("7. Selective Suspend")
        print("8. Display device serial number")
        print("9. Refresh")
        print("99. Exit")

        wdapi.OsMutexUnlock(HANDLE(DrvCtx.hMutex))

        (cmd, dwStatus) = DIAG_GetMenuOption(9)
        if DIAG_INPUT_FAIL == dwStatus:
            continue

        if not DrvCtx.DevCtxArray[dwActiveDev]:
            continue

        wdapi.OsEventReset(HANDLE(DrvCtx.hDeviceUnusedEvent))

        wdapi.OsMutexLock(HANDLE(DrvCtx.hMutex))
        hDevice = WDU_DEVICE_HANDLE(DrvCtx.DevCtxArray[dwActiveDev].hDevice)
        wdapi.OsMutexUnlock(HANDLE(DrvCtx.hMutex))

        if cmd == 1:
            PrintDeviceConfigurations(hDevice)

        elif cmd == 2:
            (dwInterfaceNumber, dwStatus) = DIAG_InputNum("Please enter the" \
                " interface number (dec): ", False, sizeof(DWORD), 0, 0)
            if DIAG_INPUT_SUCCESS != dwStatus:
                continue

            (dwAlternateSetting, dwStatus) = DIAG_InputNum("Please enter the" \
                " alternate setting index (dec): ", False, sizeof(DWORD), 0, 0)
            if DIAG_INPUT_SUCCESS != dwStatus:
                continue

            dwError = wdapi.WDU_SetInterface(hDevice, dwInterfaceNumber,
                dwAlternateSetting)
            if dwError:
                ERR("DeviceDiagMenu: WDU_SetInterface() failed. error"
                    "  0x%lx (\"%s\")\n" % (dwError, wdapi.Stat2Str(dwError)))
                continue

            TRACE("DeviceDiagMenu: WDU_SetInterface() completed "
                    "successfully\n")
            DrvCtx.DevCtxArray[dwActiveDev].dwInterfaceNum = dwInterfaceNumber
            DrvCtx.DevCtxArray[dwActiveDev].dwAlternateSetting = \
                dwAlternateSetting

        elif cmd == 3:
            PrintDevicePipesInfoByHandle(hDevice)
            (dwPipeNum, dwStatus) = DIAG_InputNum("Please enter the"\
                " pipe number (hex): ", True, sizeof(DWORD), 0, 0)
            if DIAG_INPUT_SUCCESS != dwStatus:
                continue
            print("")

            dwError = wdapi.WDU_ResetPipe(hDevice, dwPipeNum)
            if dwError:
                ERR("DeviceDiagMenu: WDU_ResetPipe() failed. error 0x%lx "
                    "(\"%s\")\n" % (dwError, wdapi.Stat2Str(dwError)))
            else:
                TRACE("DeviceDiagMenu: WDU_ResetPipe() completed "
                    "successfully\n")
        elif cmd == 4:
            ReadWritePipesMenu(hDevice)
        elif cmd == 5:
            FastStreamingRead(DrvCtx)
        elif cmd == 6:
            wdapi.OsMutexLock(HANDLE(DrvCtx.hMutex))
            if DrvCtx.dwDeviceCount > 1:
                availableDevs = []
                for i in range(WD_PYTHON_$$$nu$$$_DEVICES):
                    if DrvCtx.DevCtxArray[i].dwVendorId and \
                        DrvCtx.DevCtxArray[i].dwProductId:
                        print("  %ld. Vendor id: 0x%lx, Product id: 0x%lx, "
                            "Interface number: %ld, Alt. Setting: %ld" % (i + 1,
                            DrvCtx.DevCtxArray[i].dwVendorId,
                            DrvCtx.DevCtxArray[i].dwProductId,
                            DrvCtx.DevCtxArray[i].dwInterfaceNum,
                            DrvCtx.DevCtxArray[i].dwAlternateSetting))
                        availableDevs.append(i + 1)

                (dwDeviceNum, dwStatus) = DIAG_InputNum("Please enter the "
                    "device number (1 - %ld, dec): " % max(availableDevs), \
                    False, sizeof(DWORD), 1, max(availableDevs))
                if DIAG_INPUT_SUCCESS != dwStatus:
                    wdapi.OsMutexUnlock(HANDLE(DrvCtx.hMutex))
                    continue

                if dwDeviceNum.value not in availableDevs:
                    wdapi.OsMutexUnlock(HANDLE(DrvCtx.hMutex))
                    print "Invalid selection"
                    continue

                DrvCtx.dwActiveDev = DWORD(dwDeviceNum.value - 1)
                wdapi.OsMutexUnlock(HANDLE(DrvCtx.hMutex))
        elif cmd == 7:
            SelectiveSuspendMenu(hDevice)
        elif cmd == 8:
            PrintDeviceSerialNumberByHandle(hDevice)
        elif cmd == 9:
            # Nothing to do
            pass

        elif cmd == 99:
            break

        # Finished using hDevice
        wdapi.OsEventSignal(HANDLE(DrvCtx.hDeviceUnusedEvent))

eventTable = WDU_EVENT_TABLE()
def DriverInit(matchTables, dwNumMatchTables, sDriverName, sLicense, DrvCtx):
    global eventTable

    # Set Driver Name
    if not wdapi.WD_DriverName(sDriverName):
        ERR("DriverInit: Failed setting driver name to %s, exiting\n" %
            sDriverName)
        return WD_SYSTEM_INTERNAL_ERROR

    dwError = wdapi.OsEventCreate(byref(DrvCtx, DRIVER_CONTEXT.hEvent.offset))
    if dwError:
        ERR("DriverInit: OsEventCreate() failed on event 0x%lx. error 0x%lx "
            "(\"%s\")\n" % (DrvCtx.hEvent, dwError, wdapi.Stat2Str(dwError)))
        return dwError

    dwError = wdapi.OsMutexCreate(byref(DrvCtx, DRIVER_CONTEXT.hMutex.offset))
    if dwError:
        ERR("DriverInit: OsMutexCreate() failed on mutex 0x%lx. error 0x%lx "
            "(\"%s\")\n" % (DrvCtx.hMutex, dwError, wdapi.Stat2Str(dwError)))
        return dwError

    dwError = wdapi.OsEventCreate(byref(DrvCtx,
        DRIVER_CONTEXT.hDeviceUnusedEvent.offset))
    if dwError:
        ERR("DriverInit: OsEventCreate() failed on event 0x%lx. error 0x%lx "
            "(\"%s\")\n" % (DrvCtx.hDeviceUnusedEvent, dwError,
            wdapi.Stat2Str(dwError)))
        return dwError

    wdapi.OsEventSignal(HANDLE(DrvCtx.hDeviceUnusedEvent))

    eventTable.pfDeviceAttach = WDU_ATTACH_CALLBACK(DeviceAttach)
    eventTable.pfDeviceDetach = WDU_DETACH_CALLBACK(DeviceDetach)
    eventTable.pUserData = PVOID(addressof(DrvCtx))

    dwError = wdapi.WDU_Init(byref(hDriver), byref(matchTables),
        dwNumMatchTables, byref(eventTable), sLicense, WD_ACKNOWLEDGE)
    if dwError:
        ERR("DriverInit: Failed to initialize $$$nu$$$ driver. error 0x%lx "
            "(\"%s\")\n" % (dwError, wdapi.Stat2Str(dwError)))
        return dwError

    return WD_STATUS_SUCCESS

def DriverUninit(DrvCtx):
    if DrvCtx.hEvent:
        wdapi.OsEventClose(HANDLE(DrvCtx.hEvent))
    if DrvCtx.hMutex:
        wdapi.OsMutexClose(HANDLE(DrvCtx.hMutex))
    if DrvCtx.hDeviceUnusedEvent:
        wdapi.OsEventClose(HANDLE(DrvCtx.hDeviceUnusedEvent))
    if hDriver:
        wdapi.WDU_Uninit(hDriver)

def main_Exit(dwError, DrvCtx):
    DriverUninit(DrvCtx)
    return dwError

def main():
    fUseDefault = False
    matchTable = WDU_MATCH_TABLE()
    global DrvCtx

    wdapi_va.PrintDbgMessage(D_ERROR, S_$$$nu$$$, "WinDriver user mode version %s\n"\
        % WD_VERSION_STR)

    #if defined($$$nu$$$_DIAG_SAMPLE)
    # Get vendor ID
    (wVendorId, dwStatus) = DIAG_InputNum("Enter vendor ID (Default = 0x%x):" %
        DEFAULT_VENDOR_ID, True, sizeof(WORD), 0, 0)
    if DIAG_INPUT_SUCCESS != dwStatus:
        fUseDefault = True

    # Get device ID
    (wProductId, dwStatus) = DIAG_InputNum("Enter device ID (Default = 0x%x):" %
        DEFAULT_PRODUCT_ID, True, sizeof(WORD), 0, 0)
    if DIAG_INPUT_SUCCESS != dwStatus:
        fUseDefault = True
    #endif

    # Use defaults
    if fUseDefault:
        wVendorId = DEFAULT_VENDOR_ID
        wProductId = DEFAULT_PRODUCT_ID

    matchTable.wVendorId = wVendorId
    matchTable.wProductId = wProductId

    dwError = DriverInit(matchTable, 1, DEFAULT_DRIVER_NAME,
        DEFAULT_LICENSE_STRING, DrvCtx)

    if dwError:
        return main_Exit(dwError, DrvCtx)

    print("Please make sure the device is attached:\n")
    # Wait for the device to be attached
    dwError = wdapi.OsEventWait(HANDLE(DrvCtx.hEvent), ATTACH_EVENT_TIMEOUT)
    if dwError:
        if dwError == WD_TIME_OUT_EXPIRED:
            ERR("Timeout expired for connection with the device.\n"
                "Check that the device is connected and try again.\n")
        else:
            ERR("main: wdapi.OsEventWait() failed on event 0x%lx. error 0x%lx "
                "(\"%s\")\n" % (DrvCtx.hEvent, dwError, wdapi.Stat2Str(dwError)))
            return main_Exit(dwError, DrvCtx)

    DeviceDiagMenu(DrvCtx)
    main_Exit(dwError, DrvCtx)

if __name__ == "__main__":
    main()
