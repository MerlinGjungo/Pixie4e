# Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com

from diag_lib import *
from wdc_lib import *

#*****************************************************************************
#  File: wds_diag_lib.py - Implementation of shared WDS all devices' user-mode
#        diagnostics API.
#
#  Note: This code sample is provided AS-IS and as a guiding sample only.
#*****************************************************************************

# -------------------------------------------------------------------------
#    IPC
# -------------------------------------------------------------------------
# IPC API functions are not part of the standard WinDriver API, and not
# included in the standard version of WinDriver. The functions are part of
# "WinDriver for Server" API and require "WinDriver for Server" license.
# Note that "WinDriver for Server" APIs are included in WinDriver evaluation
# version.

# IPC scan processes results
class WDS_IPC_SCAN_RESULT(Structure): _fields_ = \
    [("dwNumProcs", DWORD), # Number of matching processes
     ("procInfo", WD_IPC_PROCESS * WD_IPC_MAX_PROCS)] # Array of processes info

# IPC message received
class WDS_IPC_MSG_RX(Structure): _fields_ = \
    [("dwSenderUID", DWORD), # Number of matching processes
    ("dwMsgID", DWORD),
    ("qwMsgData", UINT64)]

IPC_MSG_RX_HANDLER = DLLCALLCONV(None, POINTER(WDS_IPC_MSG_RX), PVOID)
#typedef void (*IPC_MSG_RX_HANDLER)(WDS_IPC_MSG_RX *pIpcRxMsg, void *pData);

# Example IPC messages IDs
IPC_MSG_KERNEL_BUFFER_READY     = 1
                                 # Kernel buffer (Created with
                                 # WDS_SharedBufferAlloc()) ready to be
                                 # shared between processes. Kernel Buffer
                                 # handle is passed in the qwMsgData

IPC_MSG_CONTIG_DMA_BUFFER_READY = 2
                                 # Kernel buffer (Created with
                                 # wdapi.WDC_DMAContigBufLock()) ready to be
                                 # shared between processes. DMA Buffer
                                 # handle is passed in the qwMsgData

# TODO: Modify/Add values to communicate between processes

#************************************************************
#  General definitions
#************************************************************
# Error messages display

def WDS_DIAG_ERR(s):
    wdapi_va.WDC_Err("PCI lib ERROR: %s\n" % s)
    print ("WDS_DIAG_ERR: " + s)

DEFAULT_PROCESS_NAME = "Diagnostic program"

# Unique identifier of the processes group to avoid getting messages from
# processes made under WinDriver by other developers that use the same driver
# name.
# WinDriver developers are encouraged to change their driver name before
# distribution to avoid this issue entirely.

DEFAULT_PROCESS_GROUP_ID = 0x12345678
SAMPLE_BUFFER_DATA = "This is a sample buffer data"

#***********************************************************
#  Global variables
#************************************************************

# User's shared kernel buffer handle
pSharedKerBuf = None # Static global pointer is used
                     # only for sample simplicity

# Get kernel buffer global handle
def WDS_SharedBufferGetGlobalHandle(pKerBuf):
    return pKerBuf.contents.hKerBuf

# -----------------------------------------------
#    Shared Buffer
# -----------------------------------------------

# Shared Buffer menu options
(   MENU_SB_ALLOC_CONTIG,
    MENU_SB_ALLOC_NON_CONTIG,
    MENU_SB_FREE ) = xrange(1,4)
MENU_SB_EXIT = DIAG_EXIT_MENU

def WDS_DIAG_SharedBufferFree(pKerBuf):
    if not pKerBuf:
        return

    dwStatus = wdapi.WDS_SharedBufferFree(pKerBuf)
    if WD_STATUS_SUCCESS != dwStatus:
        WDS_DIAG_ERR("WDS_DIAG_SharedBufferFree: Failed freeing shared buffer "
            "memory. Error [0x%lx - %s]" % (dwStatus, wdapi.Stat2Str(dwStatus)))
        return

    pKerBuf = None
    print("Shared buffer memory freed")

gpKerBuf = None

def MenuSharedBuffer():
    global gpKerBuf

    while True:
        dwOptions = 0

        print("")
        print("Shared Buffer Operations")
        print("    E.g. for communicating with Kernel-Plugin")
        print("-----------")

        print("%d. Allocate contiguous shared buffer" % MENU_SB_ALLOC_CONTIG)
        print("%d. Allocate non-contiguous shared buffer" %
            MENU_SB_ALLOC_NON_CONTIG)
        print("%d. Free shared buffer" % MENU_SB_FREE)
        print("%d. Exit menu" % MENU_SB_EXIT)
        print("")
        (option, dwStatus) = DIAG_GetMenuOption(MENU_SB_FREE)
        if DIAG_INPUT_FAIL == dwStatus:
            continue

        if option == MENU_SB_EXIT:
            break
        elif option == MENU_SB_ALLOC_CONTIG or \
            option == MENU_SB_ALLOC_NON_CONTIG:
            # Allocate contig or non-contig shared buffer
            if option == MENU_SB_ALLOC_NON_CONTIG:
                dwOptions |= KER_BUF_ALLOC_NON_CONTIG
            else:
                dwOptions |= KER_BUF_ALLOC_CONTIG

            (size, dwStatus) = DIAG_InputNum("Enter memory allocation size in "
                "bytes (32 bit uint) ", True, sizeof(DWORD), 1, 0xFFFFFFFF)
            if DIAG_INPUT_SUCCESS != dwStatus:
                continue

            # Free shared buffer memory before trying the new allocation
            WDS_DIAG_SharedBufferFree(gpKerBuf)
            gpKerBuf = POINTER(WD_KERNEL_BUFFER)()

            dwStatus = wdapi.WDS_SharedBufferAlloc(size.value, dwOptions,
                byref(gpKerBuf))
            if WD_STATUS_SUCCESS == dwStatus:
                print("Shared buffer allocated. User addr [0x%lx], "
                    "kernel addr [0x%lx], size [%lu(0x%lx)]" %
                    (gpKerBuf.contents.pUserAddr, gpKerBuf.contents.pKernelAddr,
                    size.value, size.value))
            else:
                WDS_DIAG_ERR("MenuSharedBuffer: Failed allocating shared "
                    "buffer memory. size [%ld], Error [0x%lx - %s]" %
                    (size.value, dwStatus, wdapi.Stat2Str(dwStatus)))

        elif option == MENU_SB_FREE: # Free shared buffer memory
            WDS_DIAG_SharedBufferFree(gpKerBuf)
            gpKerBuf = None

    # Free shared buffer before exiting
    WDS_DIAG_SharedBufferFree(gpKerBuf)
    gpKerBuf = None

#   -----------------------------------------------
#    IPC - Inter process Communication
#   -----------------------------------------------

def ipc_msg_event_cb(pIpcRxMsg, pData):
    print("\nReceived an IPC message:\n"
        "msgID [0x%lx], msgData [0x%lx] from process [0x%lx]" %
        (pIpcRxMsg.contents.dwMsgID, pIpcRxMsg.contents.qwMsgData,
        pIpcRxMsg.contents.dwSenderUID))

    # Important: Acquiring and using any resource (E.g. kernel/DMA buffer)
    # should be done from a deferred procedure to avoid jamming the IPC
    # incoming messages.
    # Notice you can pass private context at WDS_IpcRegister() and use it here
    # (pData) for signalling local thread for example.

    # The following implementation is for sample purposes only!

    if pIpcRxMsg.contents.dwMsgID == IPC_MSG_KERNEL_BUFFER_READY:
        pKerBuf = POINTER(WD_KERNEL_BUFFER)()

        print("This is a shared kernel buffer, getting it...")
        dwStatus = wdapi.WDS_SharedBufferGet(pIpcRxMsg.contents.qwMsgData,
            byref(pKerBuf))
        if WD_STATUS_SUCCESS != dwStatus:
            WDS_DIAG_ERR("ipc_msg_event_cb: Failed getting shared kernel "
                "buffer. Error [0x%lx - %s]" % (dwStatus,
                wdapi.Stat2Str(dwStatus)))
            return

        print("Got a shared kernel buffer. UserAddr [0x%lx], "
            "KernelAddr [0x%lx], size [%lu]" % (pKerBuf.contents.pUserAddr,
            pKerBuf.contents.pKernelAddr, pKerBuf.contents.qwBytes))

        # Here we read SAMPLE_BUFFER_DATA from the received buffer
        sample_buffer_len = len(SAMPLE_BUFFER_DATA)
        if pKerBuf.contents.qwBytes > sample_buffer_len + 1:
            print("Sample data from kernel buffer [%s]" %
                string_at(pKerBuf.contents.pUserAddr, sample_buffer_len))
        else:
            print("Kernel buffer was too short for sample data")

        # For sample purpose we immediately release the buffer
        wdapi.WDS_SharedBufferFree(pKerBuf)

    elif pIpcRxMsg.contents.dwMsgID == IPC_MSG_CONTIG_DMA_BUFFER_READY:
        pDma = POINTER(WD_DMA)()
        print("This is a DMA buffer, getting it...")

        dwStatus = wdapi.WDC_DMABufGet(DWORD(pIpcRxMsg.contents.qwMsgData),
            byref(pDma))
        if WD_STATUS_SUCCESS != dwStatus:
            WDS_DIAG_ERR("ipc_msg_event_cb: Failed getting DMA buffer. "
                "Error [0x%lx - %s]" % (dwStatus, wdapi.Stat2Str(dwStatus)))
            return

        print("Got a DMA buffer. UserAddr [%lx], pPhysicalAddr [0x%lx], size "
            "[%ld(0x%lx)]" % (pDma.contents.pUserAddr,
            pDma.contents.Page[0].pPhysicalAddr, pDma.contents.Page[0].dwBytes,
            pDma.contents.Page[0].dwBytes))

        # For sample purpose we immediately release the buffer
        wdapi.WDC_DMABufUnlock(pDma)
    else:
        print("Unknown IPC msgID [0x%lx], msgData [0x%lx] from "
            "process [0x%lx]" % (pIpcRxMsg.contents.dwMsgID,
            pIpcRxMsg.contents.qwMsgData, pIpcRxMsg.contents.dwSenderUID))

gf_ipc_msg_event_cb = IPC_MSG_RX_HANDLER(ipc_msg_event_cb)

# Register process to IPC service
def WDS_DIAG_IpcRegister():
    (dwSubGroupID, dwStatus) = DIAG_InputNum("Enter process SubGroup ID (hex)",
        True, sizeof(DWORD), 0, 0xFFFFFFFF)
    if DIAG_INPUT_SUCCESS != dwStatus:
        return 0

    dwStatus = wdapi.WDS_IpcRegister(DEFAULT_PROCESS_NAME,
        DEFAULT_PROCESS_GROUP_ID, dwSubGroupID, WD_IPC_ALL_MSG,
        gf_ipc_msg_event_cb, None ) # Your cb ctx
    if WD_STATUS_SUCCESS != dwStatus:
        WDS_DIAG_ERR("WDS_DIAG_IpcRegister: Failed registering process to IPC. "
            "Error [0x%lx - %s]" % (dwStatus, wdapi.Stat2Str(dwStatus)))
        return 0

    print("Registration completed successfully")
    return dwSubGroupID.value

def WDS_DIAG_IpcScanProcs():
    ipcScanResult = WDS_IPC_SCAN_RESULT()

    dwStatus = wdapi.WDS_IpcScanProcs(byref(ipcScanResult))
    if WD_STATUS_SUCCESS != dwStatus:
        WDS_DIAG_ERR("WDS_DIAG_IpcScanProcs: Failed scanning registered "
            "processes. Error [0x%lx - %s]" % (dwStatus,
            wdapi.Stat2Str(dwStatus)))
        return

    if ipcScanResult.dwNumProcs:
        print("Found %ld processes in current group" % ipcScanResult.dwNumProcs)
        for i in range(ipcScanResult.dwNumProcs):
            print("  %lu) Name: %s, SubGroup ID: 0x%lx, UID: 0x%lx" % (i + 1,
                ipcScanResult.procInfo[i].cProcessName,
                ipcScanResult.procInfo[i].dwSubGroupID,
                ipcScanResult.procInfo[i].hIpc))
    else:
        print("No processes found in current group")

def WDS_DIAG_IpcKerBufRelease():
    global pSharedKerBuf

    if not pSharedKerBuf:
        return

    # Notice that once a buffer that was acquired by a different process is
    # freed, its kernel resources are kept as long as the other processes did
    # not release the buffer.
    dwStatus = wdapi.WDS_SharedBufferFree(pSharedKerBuf)
    if WD_STATUS_SUCCESS != dwStatus:
        WDS_DIAG_ERR("WDS_DIAG_IpcKerBufRelease: Failed freeing shared "
            "buffer. Error [0x%lx - %s]" % (dwStatus, wdapi.Stat2Str(dwStatus)))
        return

    pSharedKerBuf = None
    print("Kernel buffer freed")

def WDS_DIAG_IpcKerBufAllocAndShare():
    global pSharedKerBuf
    dwOptions = DWORD(KER_BUF_ALLOC_CONTIG)

    # If kernel buffer was allocated in the past, release it
    WDS_DIAG_IpcKerBufRelease()

    pSharedKerBuf = POINTER(WD_KERNEL_BUFFER)()

    (size, dwStatus) = DIAG_InputNum("Enter new kernel buffer size to allocate"
        " and share with current group", True, sizeof(UINT64), 1, 0xFFFFFFFF)
    if DIAG_INPUT_SUCCESS != dwStatus:
        return

    dwStatus = wdapi.WDS_SharedBufferAlloc(size, dwOptions,
        byref(pSharedKerBuf))
    if WD_STATUS_SUCCESS != dwStatus:
        WDS_DIAG_ERR("WDS_DIAG_IpcKerBufAllocAndShare: Failed allocating "
            "shared kernel buffer. size [%lu], Error [0x%lx - %s]" % size.value,
            dwStatus, wdapi.Stat2Str(dwStatus))
        return

    print("Successful kernel buffer allocation. UserAddr [%#lx], "
        "KernelAddr [%#lx], size [%#lx]" % (pSharedKerBuf.contents.pUserAddr,
        pSharedKerBuf.contents.pKernelAddr, size.value))

    # Here we write SAMPLE_BUFFER_DATA to the new allocated buffer
    sample_buffer_len = len(SAMPLE_BUFFER_DATA)
    if size > sample_buffer_len + 1:
        memmove(pSharedKerBuf.contents.pUserAddr, SAMPLE_BUFFER_DATA,
            sample_buffer_len)
        print("Sample data written to kernel buffer\0")
    else:
        print("Kernel buffer is too short for sample data")

    dwStatus = wdapi.WDS_IpcMulticast(IPC_MSG_KERNEL_BUFFER_READY,
        WDS_SharedBufferGetGlobalHandle(pSharedKerBuf))
    if WD_STATUS_SUCCESS != dwStatus:
        WDS_DIAG_ERR("WDS_DIAG_IpcAllocAndShareBuf: Failed sending message. "
            "Error [0x%lx - %s]" % (dwStatus, wdapi.Stat2Str(dwStatus)))
        return

    print("Kernel buffer shared successfully")

# IPC menu options
(   MENU_IPC_REGISTER,
    MENU_IPC_UNREGISTER,
    MENU_IPC_GET_GROUP_IDS,
    MENU_IPC_SEND_UID_UNICAST,
    MENU_IPC_SEND_SUBGROUP_MULTICAST,
    MENU_IPC_SEND_MULTICAST,
    MENU_IPC_KER_BUF_ALLOC_AND_SHARE,
    MENU_IPC_KER_BUF_RELEASE ) = xrange(1, 9)
MENU_IPC_EXIT = DIAG_EXIT_MENU

def WDS_DIAG_IpcSend(ipc_menu_option):
    if ipc_menu_option == MENU_IPC_SEND_UID_UNICAST or \
        ipc_menu_option == MENU_IPC_SEND_SUBGROUP_MULTICAST:
        (recipientID, dwStatus) = DIAG_InputNum("Enter recipient%sID (hex)" %
            ("(s) SubGroup " if ipc_menu_option ==
            MENU_IPC_SEND_SUBGROUP_MULTICAST else " U"), True, sizeof(DWORD), 0,
            0xFFFFFFFF)
        if DIAG_INPUT_SUCCESS != dwStatus:
            return

    (messageID, dwStatus) = DIAG_InputNum("Enter your message ID (32Bit hex)",
        True, sizeof(DWORD), 0, 0xFFFFFFFF)
    if DIAG_INPUT_SUCCESS != dwStatus:
        return

    (messageData, dwStatus) = DIAG_InputNum("Enter your message (64Bit hex)",
        True, sizeof(UINT64), 0, 0xFFFFFFFFFFFFFFFF)
    if DIAG_INPUT_SUCCESS != dwStatus:
        return

    if ipc_menu_option == MENU_IPC_SEND_UID_UNICAST:
        dwStatus = wdapi.WDS_IpcUidUnicast(recipientID, messageID, messageData)
    elif ipc_menu_option == MENU_IPC_SEND_SUBGROUP_MULTICAST:
        dwStatus = wdapi.WDS_IpcSubGroupMulticast(recipientID, messageID,
            messageData)
    elif ipc_menu_option == MENU_IPC_SEND_MULTICAST:
        dwStatus = wdapi.WDS_IpcMulticast(messageID, messageData)

    if WD_STATUS_SUCCESS != dwStatus:
        WDS_DIAG_ERR("WDS_DIAG_IpcSend: Failed sending message. "
            "Error [0x%lx - %s]" % (dwStatus, wdapi.Stat2Str(dwStatus)))
        return

    print("Message sent successfully")

def MenuIpc():
    dwSubGroupID = 0

    while True:
        isIpcRegistered = wdapi.WDS_IsIpcRegistered()
        print("")
        print("IPC management menu - "),
        if isIpcRegistered:
            print("Registered with SubGroup ID 0x%lx" % dwSubGroupID)
        else:
            print("Unregistered")
        print("--------------")

        if not isIpcRegistered:
            print("%d. Register processes" % MENU_IPC_REGISTER)
        else:
            print("%d. Un-Register process" % MENU_IPC_UNREGISTER)
            print("%d. Find current registered group processes" %
                MENU_IPC_GET_GROUP_IDS)
            print("%d. Unicast- Send message to a single process by "
                "unique ID" % MENU_IPC_SEND_UID_UNICAST)
            print("%d. Multicast- Send message to a subGroup" %
                MENU_IPC_SEND_SUBGROUP_MULTICAST)
            print("%d. Multicast- Send message to all processes in current "
                "group" % MENU_IPC_SEND_MULTICAST)
            print("%d. Allocate and share a kernel buffer with all processes "
                "in current group" % MENU_IPC_KER_BUF_ALLOC_AND_SHARE)
            print("%d. Free shared kernel buffer" % MENU_IPC_KER_BUF_RELEASE)

        print("%d. Exit" % MENU_IPC_EXIT)

        (option, dwStatus) = DIAG_GetMenuOption(MENU_IPC_KER_BUF_RELEASE if \
            isIpcRegistered else MENU_IPC_REGISTER)
        if DIAG_INPUT_FAIL == dwStatus:
            continue

        if option == MENU_IPC_EXIT: # Exit menu
            break
        elif option == MENU_IPC_REGISTER:
            if isIpcRegistered:
                print("Process already registered")
            else:
                dwSubGroupID = WDS_DIAG_IpcRegister()
        elif option == MENU_IPC_UNREGISTER:
            if isIpcRegistered:
                wdapi.WDS_IpcUnRegister()
            print("Process unregistered successfully")
        elif option == MENU_IPC_GET_GROUP_IDS:
            WDS_DIAG_IpcScanProcs()
        elif option == MENU_IPC_SEND_UID_UNICAST \
            or option == MENU_IPC_SEND_SUBGROUP_MULTICAST \
            or option == MENU_IPC_SEND_MULTICAST:
            WDS_DIAG_IpcSend(option)
        elif option == MENU_IPC_KER_BUF_ALLOC_AND_SHARE:
            WDS_DIAG_IpcKerBufAllocAndShare()
        elif option == MENU_IPC_KER_BUF_RELEASE:
            WDS_DIAG_IpcKerBufRelease()

def WDS_DIAG_IpcSendDmaContigToGroup(pDma):
    if not pDma:
        WDS_DIAG_ERR("send_dma_contig_buf_to_group: Error - DMA ctx is None")
        return WD_INVALID_PARAMETER

    if not (pDma.contents.dwOptions & DMA_KERNEL_BUFFER_ALLOC):
        WDS_DIAG_ERR("send_dma_contig_buf_to_group: Error - Sharing SG DMA is "
            "not supported")
        return WD_INVALID_PARAMETER

    dwStatus = wdapi.WDS_IpcMulticast(IPC_MSG_CONTIG_DMA_BUFFER_READY,
        WDC_DMAGetGlobalHandle(pDma))
    if WD_STATUS_SUCCESS != dwStatus:
        WDS_DIAG_ERR("send_dma_contig_buf_to_group: Failed sending message. "
            "Error [0x%lx - %s]" % (dwStatus, wdapi.Stat2Str(dwStatus)))
        return dwStatus

    print("DMA contiguous buffer handle sent successfully")
    return WD_STATUS_SUCCESS
