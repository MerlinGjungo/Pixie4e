# Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com

import sys, os
from wdc_lib import *
from wdu_lib import *
from diag_lib import *
import time

TRANSFER_TIMEOUT             = 30000   # In msecs
PERF_STREAM_BUFFER_SIZE      = 5120000 # In bytes
PERF_DEVICE_TRANSFER_SIZE    = 10240   # In bytes
PERF_PERFORMANCE_SAMPLE_TIME = 10000   # In msecs
PERF_TRANSFER_ITERATIONS     = 15000
MAX_BUFFER_SIZE              = 4096

def TRACE(s):
    wdapi_va.WDC_Trace("USB lib Trace: " + s)
    print ("USB lib: " + s)

def ERR(s):
    wdapi_va.WDC_Err("USB lib Error: " + s)
    print ("USB lib: " + s)

class USB_LISTEN_PIPE(Structure): _fields_ = \
    [("Handle", WDU_DEVICE_HANDLE),
    ("Pipe", WDU_PIPE_INFO),
    ("pContext", PVOID),
    ("fStopped", BOOL),
    ("hThread", HANDLE),
    ("dwError", DWORD),
    ("fStreamMode", BOOL),
    ("dwBytesToTransfer", DWORD),
    ("dwOptions", DWORD)]

# Function: pipeType2Str()
#     Returns a string identifying the pipe type
#   Parameters:
#     pipeType [in] pipe type -- USB_PIPE_TYPE enum value
#   Return Value:
#     String containing the description of the pipe type
def pipeType2Str(pipeType):
    return "Control" if pipeType == PIPE_TYPE_CONTROL \
    else "Isochronous" if pipeType == PIPE_TYPE_ISOCHRONOUS \
    else "Bulk" if pipeType == PIPE_TYPE_BULK \
    else "Interrupt" if pipeType == PIPE_TYPE_INTERRUPT \
    else "unknown"

# Function: StopListeningToPipe()
#     Stops listening to a USB device pipe
#   Parameters:
#     pListenPipe [in] pointer to the pipe to which to stop listening
#   Return Value:
#     None
def StopListeningToPipe(ListenPipe):
    if not ListenPipe.hThread:
        return

    print("Stop listening to pipe\n")
    ListenPipe.fStopped = True

    if ListenPipe.fStreamMode:
        wdapi.WDU_StreamClose(HANDLE(ListenPipe.Handle))
    else:
        wdapi.WDU_HaltTransfer(ListenPipe.Handle, ListenPipe.Pipe.dwNumber)

    wdapi.ThreadWait(HANDLE(ListenPipe.hThread))
    ListenPipe.hThread = None

# Function: PipeListenHandler()
#    Callback function that listens to a pipe continuously when there is data
#     available on the pipe
#   Parameters:
#     pParam [in] pointer to the pipe to which to listen
#   Return Value:
#     None
def PipeListenHandler(pParam):
    pListenPipe = cast(pParam, POINTER(USB_LISTEN_PIPE))
    dwBufsize = pListenPipe.contents.dwBytesToTransfer

    hDevice = pListenPipe.contents.Handle
    buf = create_string_buffer(dwBufsize)

    while True:
        dwBytesTransferred = DWORD()

        if pListenPipe.contents.fStreamMode:
            dwError = wdapi.WDU_StreamRead(pListenPipe.contents.Handle, buf,
                dwBufsize, byref(dwBytesTransferred))
        else:
            dwError = wdapi.WDU_Transfer(pListenPipe.contents.Handle,
                pListenPipe.contents.Pipe.dwNumber, True,
                pListenPipe.contents.dwOptions, buf, dwBufsize,
                byref(dwBytesTransferred), None, TRANSFER_TIMEOUT)

        if pListenPipe.contents.fStopped:
            break

        if dwError:
            pListenPipe.contents.dwError = dwError
            print("Listen ended due to an error, press <Enter> to stop.\n")
            break

        DIAG_PrintHexBuffer(buf, dwBytesTransferred)


PIPE_THREAD_CALLBACK = DLLCALLCONV(None, PVOID)
# Reference to callback to keep it alive (Without this the program will crash
# upon handler call)
_PipeListenHandler = PIPE_THREAD_CALLBACK(PipeListenHandler)

# Function: StartListeningToPipe()
#     Starts listening to a USB device pipe
#   Parameters:
#     pListenPipe [in] pointer to the pipe to which to listen
#   Return Value:
#     None
def StartListeningToPipe(ListenPipe):
    # Start the running thread
    ListenPipe.fStopped = False
    print("Start listening to pipe")

    if ListenPipe.fStreamMode:
        ListenPipe.dwError = wdapi.WDU_StreamStart(ListenPipe.Handle)
        if ListenPipe.dwError:
            ERR("StartListeningToPipe: wdapi.WDU_StreamStart() failed. error"
                " 0x%lx (\"%s\")" % (ListenPipe.dwError,
                wdapi.Stat2Str(ListenPipe.dwError)))
            return

    ListenPipe.dwError = wdapi.ThreadStart(byref(ListenPipe,
        USB_LISTEN_PIPE.hThread.offset),
        _PipeListenHandler, PVOID(addressof(ListenPipe)))

# Function: ListenToPipe()
#     Listening to a USB device pipe
#   Parameters:
#     hDevice      [in] Handle to the USB device
#     pPipe        [in] Pointer to the pipe for listening
#     fStreamMode  [in] True - Streaming mode
#     dwBufferSize [in] Kernel buffer size on streaming mode
#     fUserKeyWait [in] True - Wait for user key before starting
#   Return Value:
#     None

listenPipe = USB_LISTEN_PIPE()
def ListenToPipe(hDevice, pPipe, fStreamMode, dwBufferSize, fUserKeyWait):
    global listenPipe
    listenPipe.Pipe = pPipe
    listenPipe.fStreamMode = fStreamMode

    if pPipe.type == PIPE_TYPE_ISOCHRONOUS:
        listenPipe.dwBytesToTransfer = pPipe.dwMaximumPacketSize * 8
        # 8 minimum packets for high speed transfers
        listenPipe.dwOptions |= USB_ISOCH_FULL_PACKETS_ONLY
    else:
        listenPipe.dwBytesToTransfer = pPipe.dwMaximumPacketSize

    if fStreamMode:
        dwError = wdapi.WDU_StreamOpen(hDevice, pPipe.dwNumber, dwBufferSize,
            listenPipe.dwBytesToTransfer, True, listenPipe.dwOptions,
            TRANSFER_TIMEOUT, byref(listenPipe))
        if dwError:
            ERR("ListenToPipe: WDU_StreamOpen() failed. error 0x%lx (\"%s\")\n"
                % (dwError, wdapi.Stat2Str(dwError)))
            return
    else:
        listenPipe.Handle = hDevice

    if fUserKeyWait:
        print("Press <Enter> to start listening. While listening, press "
            "<Enter> to stop\n\n")
        raw_input()

    else:
        print("Listening Started. While listening, press <Enter> to stop\n\n")

    StartListeningToPipe(listenPipe)
    if listenPipe.dwError:
        ERR("ListenToPipe: Error listening to pipe 0x%lx. error 0x%lx (\"%s\")"
            % (pPipe.dwNumber, listenPipe.dwError,
            wdapi.Stat2Str(listenPipe.dwError)))
        return

    while raw_input() != "": # Waiting for <Enter> key
        continue

    StopListeningToPipe(listenPipe)
    if listenPipe.dwError:
        ERR("ListenToPipe: Transfer failed. error 0x%lx (\"%s\")" %
            (listenPipe.dwError, wdapi.Stat2Str(listenPipe.dwError)))


# Function: PrintPipe()
#     Prints pipe information (helper function)
#   Parameters:
#     pPipe [in] pointer to the pipe information to print
#   Return Value:
#     None
def PrintPipe(pPipe):
    print("  pipe num. 0x%lx: packet size %ld, type %s, dir %s, interval %ld "
        "(ms)" % (pPipe.dwNumber, pPipe.dwMaximumPacketSize,
        pipeType2Str(pPipe.type), "In" if pPipe.direction == WDU_DIR_IN else
        "Out" if pPipe.direction == WDU_DIR_OUT else "In & Out",
        pPipe.dwInterval))

# Function: PrintPipe0Info()
#     Prints control pipe (pipe 0) information
#   Parameters:
#     pDevice [in] pointer to the USB device to which the control pipe belongs
#   Return Value:
#     None
def PrintPipe0Info(pDevice):
    print("\nControl pipe:")
    PrintPipe(pDevice.contents.Pipe0)

# Function: PrintPipesInfo()
#     Prints the pipes information for the specified alternate setting
#   Parameters:
#     pAltSet [in] pointer to the alternate setting information
#   Return Value:
#     None
def PrintPipesInfo(pAltSet):
    pPipe = pAltSet.contents.pPipes

    if not pAltSet.contents.Descriptor.bNumEndpoints:
        print("  no pipes are defined for this device other than the default "
            "pipe (number 0).")
        return

    print("Alternate Setting: %d" %
        pAltSet.contents.Descriptor.bAlternateSetting)

    for p in range(pAltSet.contents.Descriptor.bNumEndpoints):
        PrintPipe(pPipe[p])

def PrintDevicePipesInfo(pDevice):
    PrintPipe0Info(pDevice)
    # Iterate over interfaces and print all pipes in their active alternate
    # settings
    for k in range(pDevice.contents.pActiveConfig.contents.dwNumInterfaces):
        pIfc = pDevice.contents.pActiveInterface[k]
        if not pIfc:
            break
        print("Interface %d" %
            pIfc.contents.pActiveAltSetting.contents.Descriptor.bInterfaceNumber)
        PrintPipesInfo(pIfc.contents.pActiveAltSetting)

# Function: PrintDevicePipesInfoByHandle()
#     Prints the pipes information for all the active device pipes
#   Parameters:
#     hDevice [in] handle to the USB device
#   Return Value:
#     None
def PrintDevicePipesInfoByHandle(hDevice):
    Device = WDU_DEVICE()
    pDevice = POINTER(WDU_DEVICE)(Device)

    dwError = wdapi.WDU_GetDeviceInfo(hDevice, byref(pDevice))
    if dwError:
        ERR("PrintDevicePipesInfoByHandle: WDU_GetDeviceInfo() failed. "
            "error 0x%lx (\"%s\")\n" % (dwError, wdapi.Stat2Str(dwError)))
        return

    PrintDevicePipesInfo(pDevice)

# Function: PrintEndpoints()
#     Prints the endpoints (pipes) information for the specified alternate
#     setting (helper function for PrintDeviceConfigurations())
#   Parameters:
#     pAltSet [in] pointer to the alternate setting information
#   Return Value:
#     None
def PrintEndpoints(AltSet):
    for i in range(AltSet.Descriptor.bNumEndpoints):
        Endp = AltSet.pEndpointDescriptors[i]
        print("    end-point address: 0x%02x, attributes: 0x%x, max packet %d,"
            " Interval: %d" % (Endp.bEndpointAddress, Endp.bmAttributes,
            Endp.wMaxPacketSize, Endp.bInterval))

# Function: PrintDeviceConfigurations()
#     Prints the device's configurations information
#   Parameters:
#     hDevice [in] handle to the USB device
#   Return Value:
#     None

def PrintDeviceConfigurations(hDevice):
    Device = WDU_DEVICE()
    pDevice = POINTER(WDU_DEVICE)(Device)

    # No need to use WDU_PutDeviceInfo() as Python will free the memory.
    dwError = wdapi.WDU_GetDeviceInfo(hDevice, byref(pDevice))
    if dwError:
        ERR("PrintDeviceConfigurations: WDU_GetDeviceInfo failed. error 0x%lx "
            "(\"%s\")" % (dwError, wdapi.Stat2Str(dwError)))
        return

    print("This device has %d configurations:" %
        pDevice.contents.Descriptor.bNumConfigurations)
    for iConf in range(pDevice.contents.Descriptor.bNumConfigurations):
        print("  %d. Configuration value %d (has %ld interfaces)" %
            (iConf,
            pDevice.contents.pConfigs[iConf].Descriptor.bConfigurationValue,
            pDevice.contents.pConfigs[iConf].dwNumInterfaces))

    iConf = 0

    if pDevice.contents.Descriptor.bNumConfigurations > 1:
        print("Please enter the configuration index to display "
            "(dec - zero based): ")

        (iConf, dwStatus) = DIAG_InputNum("Please enter the" \
            " pipe number (hex)", True, sizeof(DWORD), 0, 0)
        if DIAG_INPUT_SUCCESS != dwStatus:
            return
        iConf = iConf.value

        if iConf >= pDevice.contents.Descriptor.bNumConfigurations:
            print("ERROR: Invalid configuration index, valid values are "
                "0-%d" % pDevice.contents.Descriptor.bNumConfigurations)
            return

    pConf = pDevice.contents.pConfigs[iConf]

    print("The configuration indexed %d has %ld interface(s):" % (iConf,
        pConf.dwNumInterfaces))

    for ifc in range(pConf.dwNumInterfaces):
        pInterface = pConf.pInterfaces[ifc]
        print("interface no. %d has %ld alternate settings:" %
            (pInterface.pAlternateSettings[0].Descriptor.bInterfaceNumber,
            pInterface.dwNumAltSettings))
        for alt in range(pInterface.dwNumAltSettings):
            pAltSet = pInterface.pAlternateSettings[alt]
            print("  alternate: %d, endpoints: %d, class: 0x%x, "
                "subclass: 0x%x, protocol: 0x%x" %
                (pAltSet.Descriptor.bAlternateSetting,
                pAltSet.Descriptor.bNumEndpoints,
                pAltSet.Descriptor.bInterfaceClass,
                pAltSet.Descriptor.bInterfaceSubClass,
                pAltSet.Descriptor.bInterfaceProtocol))
            PrintEndpoints(pAltSet)
        print("")
    print("")

# Function: get_cur_time()
#     Retrieves the current time, in OS units
#   Parameters:
#     time [out] pointer to the OS time, in OS units
#   Return Value:
#     True if successful, False otherwise
def get_cur_time():
    return time.time()

# Function: time_diff()
#     Calculates a time difference in milliseconds
#   Parameters:
#     end   [in] End time, in OS units
#     start [in] Start time, in OS units
#   Return Value:
#     The time difference, in milliseconds.
def time_diff(end, start):
    return 1000 * (end-start)

(   MENU_RW_READ_PIPE,
    MENU_RW_WRITE_PIPE,
    MENU_RW_LISTEN_PIPE,
    MENU_RW_STREAM_READ_SPEED,
    MENU_RW_STREAM_WRITE_SPEED,
    MENU_RW_CHANGE_MODE) = xrange(1,7)
MENU_RW_EXIT = 99

# Function: FindPipeInDevice()
#     Finds a pipe in any of the device's active interfaces
#   Parameters:
#     pDevice       [in] pointer to the USB device
#     dwPipeNumber [in] the pipe number to look for
#   Return Value:
#     a pointer to the requested pipe, or NULL if no matching pipe was found
def FindPipeInDevice(pDevice, dwPipeNumber):
    if dwPipeNumber == 0:
        return pDevice.contents.Pipe0

    for i in range(pDevice.contents.pActiveConfig.contents.dwNumInterfaces):
        pIfc = pDevice.contents.pActiveInterface[i]
        if not pIfc:
            break
        pAltSet = pIfc.contents.pActiveAltSetting
        for j in range(pAltSet.contents.Descriptor.bNumEndpoints):
            if pAltSet.contents.pPipes[j].dwNumber == dwPipeNumber:
                return pAltSet.contents.pPipes[j]
    return None

def TransferBytes(cmd, stream, pBuffer):
    dwBytesTransferred = 0
    while dwBytesTransferred < PERF_DEVICE_TRANSFER_SIZE * \
        PERF_TRANSFER_ITERATIONS:

        dwBytesTransferredSingle = DWORD()
        if cmd == MENU_RW_STREAM_READ_SPEED:
            dwError = wdapi.WDU_StreamRead(stream, pBuffer,
                PERF_DEVICE_TRANSFER_SIZE, byref(dwBytesTransferredSingle))
        else:
            dwError = wdapi.WDU_StreamWrite(stream, pBuffer,
                PERF_DEVICE_TRANSFER_SIZE, byref(dwBytesTransferredSingle))
        if dwError:
            ERR("ReadWritePipesMenu: Transfer failed. error 0x%lx "
                "(\"%s\")\n" % (dwError, wdapi.Stat2Str(dwError)))
            return (dwError, dwBytesTransferred)
        dwBytesTransferred += dwBytesTransferredSingle.value

    return (WD_STATUS_SUCCESS, dwBytesTransferred)

# Function: ReadWritePipesMenu()
#     Displays a menu to read/write from/to the device's pipes
#   Parameters:
#    hDevice [in] handle to the USB device
#   Return Value:
#     None

fStreamMode = True
def ReadWritePipesMenu(hDevice):
    Device = WDU_DEVICE()
    pDevice = POINTER(WDU_DEVICE)(Device)
    dwBytesTransferred = DWORD()
    stream = WDU_STREAM_HANDLE()
    dwBufferSize = 0x20000
    global fStreamMode

    if sys.platform == "win32":
        fStreamAble = True
    else:
        fStreamAble = False

    dwError = wdapi.WDU_GetDeviceInfo(hDevice, byref(pDevice))
    if dwError:
        ERR("ReadWritePipesMenu: WDU_GetDeviceInfo() failed. error 0x%lx "
            "(\"%s\")\n" % (dwError, wdapi.Stat2Str(dwError)))
        return

    PrintDevicePipesInfo(pDevice)

    if not fStreamAble:
        fStreamMode = False

    while True:
        print("")
        print("Read/Write from/to device's pipes using %s" %
            "Streaming Data Transfers" if fStreamMode\
            else "Single Blocking Transfers")
        print("---------------------")
        print("%d.  Read from pipe" % MENU_RW_READ_PIPE)
        print("%d.  Write to pipe" % MENU_RW_WRITE_PIPE)
        print("%d.  Listen to pipe (continuous read)" % MENU_RW_LISTEN_PIPE)

        if fStreamMode:
            print("%d.  Check streaming READ speed" %
                MENU_RW_STREAM_READ_SPEED)
            print("%d.  Check streaming WRITE speed" %
                MENU_RW_STREAM_WRITE_SPEED)

        # Toggle menu from regular to streaming
        if fStreamAble:
            print("%d.  Switch to %s mode" % (MENU_RW_CHANGE_MODE,
                "Single Blocking Transfers" if fStreamMode
                else "Streaming Data Transfers"))
        print("%d. Main menu\n" % MENU_RW_EXIT)

        (cmd, dwStatus) = DIAG_GetMenuOption(9)
        if DIAG_INPUT_FAIL == dwStatus:
            continue

        if cmd == MENU_RW_EXIT:
            break

        if cmd == MENU_RW_CHANGE_MODE:
            if fStreamAble:
                fStreamMode = not fStreamMode
            continue

        # Make sure the option entered is available for the current mode
        if (cmd < MENU_RW_READ_PIPE or
            (not fStreamMode and cmd > MENU_RW_LISTEN_PIPE) or
            cmd > MENU_RW_STREAM_WRITE_SPEED):
            continue

        (dwPipeNum, dwStatus) = DIAG_InputNum("Please enter the"\
            " pipe number (hex)", True, sizeof(DWORD), 0, 0)
        if DIAG_INPUT_SUCCESS != dwStatus:
            return
        dwPipeNum = dwPipeNum.value

        # Search for the pipe
        pPipe = FindPipeInDevice(pDevice, dwPipeNum)
        if not pPipe:
            print("The pipe number 0x%lx does not exist. Please try again." %
                dwPipeNum)
            continue

        if cmd == MENU_RW_READ_PIPE or cmd == MENU_RW_WRITE_PIPE:
            dwBytesTransferred = DWORD()
            if (not dwPipeNum) or pPipe.type == PIPE_TYPE_CONTROL:
                if fStreamMode:
                    ERR("Cannot perform stream transfer using control pipe.\n"
                        "please switch to Single Blocking Transfer mode "
                        "(option 6) or change the pipe number")
                    break
                print("Please enter setup packet (hex - 8 bytes): ")
                (sInput, bytesRead) = DIAG_GetHexBuffer(8)
                SetupPacket = (c_char * len(sInput)).from_buffer(sInput)
            else:
                SetupPacket = create_string_buffer(8)

            (dwSize, dwStatus) = DIAG_InputNum("Please enter the size of the"\
                " buffer (dec): ", False, sizeof(DWORD), 0, 0)
            if DIAG_INPUT_SUCCESS != dwStatus:
                break
            dwSize = dwSize.value

            pBuffer = create_string_buffer(dwSize)

            if dwSize:
                if cmd == MENU_RW_WRITE_PIPE:
                    print("Please enter the input buffer (hex): ")
                    (sInput, bytesRead) = DIAG_GetHexBuffer(dwSize)
                    pBuffer = (c_char * len(sInput)).from_buffer(sInput)

            if fStreamMode:
                while True:
                    dwError = wdapi.WDU_StreamOpen(hDevice, dwPipeNum,
                        dwBufferSize, dwSize, True, 0, TRANSFER_TIMEOUT,
                        byref(stream))
                    if dwError:
                        ERR("ReadWritePipesMenu: wdapi.WDU_StreamOpen() failed."
                            " error 0x%lx (\"%s\")\n" % (dwError,
                            wdapi.Stat2Str(dwError)))
                        break #goto End_transfer
                    dwError = wdapi.WDU_StreamStart(stream)
                    if dwError:
                        ERR("ReadWritePipesMenu: wdapi.WDU_StreamStart() "
                            "failed. error 0x%lx (\"%s\")\n" % (dwError,
                            wdapi.Stat2Str(dwError)))
                        break #goto End_transfer
                    dwError = wdapi.WDU_StreamRead(stream, pBuffer, dwSize,
                        byref(dwBytesTransferred)) if cmd == MENU_RW_READ_PIPE \
                        else wdapi.WDU_StreamWrite(stream, pBuffer, dwSize,
                        byref(dwBytesTransferred))
                    if dwError:
                        fIsRunning = BOOL()
                        dwLastError = DWORD()
                        dwBytesInBuffer = DWORD()

                        dwError = wdapi.WDU_StreamGetStatus(stream,
                            byref(fIsRunning), byref(dwLastError),
                            byref(dwBytesInBuffer))
                        if not dwError:
                            dwError = dwLastError
                    break

                #End_transfer:
                if stream:
                    dwError = wdapi.WDU_StreamClose(stream)
                    if dwError:
                        ERR("ReadWritePipesMenu: wdapi.WDU_StreamClose() "
                            "failed. error 0x%lx (\"%s\")\n" % (dwError,
                            wdapi.Stat2Str(dwError)))
                        continue
            else: #Single Blocking Transfer Mode
                dwError = wdapi.WDU_Transfer(hDevice, pPipe.dwNumber
                    if dwPipeNum else 0, cmd == MENU_RW_READ_PIPE, 0, pBuffer,
                    dwSize, byref(dwBytesTransferred), SetupPacket,
                    TRANSFER_TIMEOUT)

            #Print transfer results:
            if dwError:
                ERR("ReadWritePipesMenu: Transfer failed. error 0x%lx "
                      "(\"%s\")\n" % (dwError, wdapi.Stat2Str(dwError)))
            else:
                if cmd == MENU_RW_READ_PIPE and not dwBytesTransferred.value:
                    ERR("ReadWritePipesMenu: Transferred 0 bytes, try"
                    " increasing buffer size")
                else:
                    print("Transferred %ld bytes\n" % dwBytesTransferred.value)
                    if cmd == MENU_RW_READ_PIPE and pBuffer:
                        DIAG_PrintHexBuffer(pBuffer, dwBytesTransferred)
                        continue

        elif cmd == MENU_RW_LISTEN_PIPE:
            if not dwPipeNum or pPipe.type == PIPE_TYPE_CONTROL:
                print("Cannot listen to control pipes.\n")
                continue
            ListenToPipe(hDevice, pPipe, fStreamMode, dwBufferSize, True)
            continue

        elif cmd == MENU_RW_STREAM_READ_SPEED or \
            cmd == MENU_RW_STREAM_WRITE_SPEED:
            if not fStreamMode:
                break # We shouldn't reach this line

            if not dwPipeNum or pPipe.type == PIPE_TYPE_CONTROL:
                print("Cannot perform stream transfer with control pipe")
                continue

            print("The size of the buffer to transfer(dec): %d" %
                PERF_DEVICE_TRANSFER_SIZE)
            print("The size of the internal Rx/Tx stream buffer (dec): %d" %
                PERF_STREAM_BUFFER_SIZE)
            print("Making the transfer of %d times the buffer size, please "
                "wait ..." % PERF_TRANSFER_ITERATIONS)

            if cmd == MENU_RW_STREAM_WRITE_SPEED:
                pass
                # Here you can fill pBuffer with the right data for the
                # board

            dwError = wdapi.WDU_StreamOpen(hDevice, dwPipeNum,
                PERF_STREAM_BUFFER_SIZE, PERF_DEVICE_TRANSFER_SIZE, True, 0,
                TRANSFER_TIMEOUT, byref(stream))
            if dwError:
                ERR("ReadWritePipesMenu: wdapi.WDU_StreamOpen() failed. error "
                    "0x%lx (\"%s\")" % (dwError, wdapi.Stat2Str(dwError)))
                continue

            pBuffer = create_string_buffer(PERF_DEVICE_TRANSFER_SIZE)

            while True:
                dwError = wdapi.WDU_StreamStart(stream)
                if dwError:
                    ERR("ReadWritePipesMenu: WDU_StreamStart() failed. error "
                        "0x%lx (\"%s\")\n" % (dwError, wdapi.Stat2Str(dwError)))
                    break #goto End_perf_test

                streaming_time_start = get_cur_time()
                (dwError, dwBytesTransferred) = TransferBytes(cmd, stream,
                    pBuffer)
                if dwError:
                    break #goto End_perf_test

                # If write command, wait for all the data to be written
                if cmd == MENU_RW_STREAM_WRITE_SPEED:
                    dwError = wdapi.WDU_StreamFlush(stream)
                    if dwError:
                        ERR("ReadWritePipesMenu: Transfer failed. error 0x%lx "
                            "(\"%s\")\n" % (dwError, wdapi.Stat2Str(dwError)))
                        break #goto End_perf_test

                streaming_time_end = get_cur_time()
                perf_time_total = time_diff(streaming_time_end,
                    streaming_time_start)
                if perf_time_total == -1:
                    break #goto End_perf_test

                print("Transferred %ld bytes, elapsed time %ld[ms], "
                    "rate %ld[MB/sec]\n" % (dwBytesTransferred, perf_time_total,
                    # (bytes / msec) * sec / MB
                    (dwBytesTransferred / (perf_time_total + 1)) * 1000 / (1024
                    * 1024)))
                break

            #End_perf_test:
            dwError = wdapi.WDU_StreamClose(stream)
            if dwError:
                ERR("ReadWritePipesMenu: WDU_StreamClose() failed. error "
                    "0x%lx (\"%s\")\n" % (dwError, wdapi.Stat2Str(dwError)))

MENU_SUSPEND_ENABLE_DISABLE = 1
MENU_SUSPEND_EXIT = 99

# Function: SelectiveSuspendMenu()
#     Displays a menu to enter/exit Selective Suspend mode
#   Parameters:
#     hDevice [in] handle to the USB device
#   Return Value:
#     None
def SelectiveSuspendMenu(hDevice):
    dwError = 0
    fSuspended = False
    cmd = 0

    while cmd != MENU_SUSPEND_EXIT:
        print("")
        print("Toggle suspend mode")
        print("-----------------------------")
        print("%d. %s suspend mode" % (MENU_SUSPEND_ENABLE_DISABLE,
            "Leave" if fSuspended else "Enter"))
        print("%d. Main menu" % MENU_SUSPEND_EXIT)

        (cmd, dwStatus) = DIAG_GetMenuOption(1)
        if DIAG_INPUT_FAIL == dwStatus:
            continue

        if cmd == MENU_SUSPEND_ENABLE_DISABLE:
            if not fSuspended:
                dwError = wdapi.WDU_SelectiveSuspend(hDevice,
                    WDU_SELECTIVE_SUSPEND_SUBMIT)
        # Fall through
        elif cmd == MENU_SUSPEND_EXIT or cmd == MENU_SUSPEND_ENABLE_DISABLE:
            if fSuspended:
                dwError = wdapi.WDU_SelectiveSuspend(hDevice,
                    WDU_SELECTIVE_SUSPEND_CANCEL)
        else:
            continue

        if dwError:
            ERR("DeviceDiagMenu: WDU_SelectiveSuspend() failed. error 0x%lx "
                "(\"%s\")\n" % (dwError, wdapi.Stat2Str(dwError)))
        else:
            fSuspended = not fSuspended

# Function: PrintDeviceSerialNumberByHandle()
#     Prints the device serial number if available
#   Parameters:
#     hDevice [in] handle to the USB device
#   Return Value:
#     None
def PrintDeviceSerialNumberByHandle(hDevice):
    dwSerialDescSize = DWORD()
    Device = WDU_DEVICE()
    pDevice = POINTER(WDU_DEVICE)(Device)
    bSerialNum = create_string_buffer(0x100)

    #No need to use WDU_PutDeviceInfo() as Python will free the memory
    dwError = wdapi.WDU_GetDeviceInfo(hDevice, byref(pDevice))
    if dwError:
        ERR("PrintDeviceConfigurations: WDU_GetDeviceInfo failed. error 0x%lx "
            "(\"%s\")\n" % (dwError, wdapi.Stat2Str(dwError)))
        return

    if not pDevice.contents.Descriptor.iSerialNumber:
        print("Serial number is not available")
        return

    print("Serial number string descriptor index: [%d]" %
        pDevice.contents.Descriptor.iSerialNumber)

    dwError = wdapi.WDU_GetStringDesc(hDevice,
        pDevice.contents.Descriptor.iSerialNumber,
        bSerialNum, sizeof(bSerialNum), 0, byref(dwSerialDescSize))
    if dwError:
        ERR("PrintDeviceSerialNumberByHandle: WDU_GetStringDesc() failed. "
            "error 0x%lx (\"%s\")" % (dwError, wdapi.Stat2Str(dwError)))
        return

    print("Serial number: length [%ld], [%s]" % (dwSerialDescSize.value,
        bSerialNum[:dwSerialDescSize.value]))

