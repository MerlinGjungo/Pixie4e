from ctypes import *
from windrvr import *
from wdc_defs import *

#**************************************************************
#  General definitions
# *************************************************************

MAX_NAME = 128
MAX_DESC = 128
MAX_NAME_DISPLAY = 22

# Handle to device information struct
WDC_DEVICE_HANDLE = PVOID

def WDC_IS_KP(pDev):
    if not isinstance(pDev, PWDC_DEVICE):
        pDev = cast(pDev, PWDC_DEVICE)
    return bool(pDev.contents.kerPlug.hKernelPlugIn)

# PCI scan results
class WDC_PCI_SCAN_RESULT(Structure): _fields_ = \
    [("dwNumDevices", DWORD),                   # Number of matching devices
    ("deviceId", WD_PCI_ID * WD_PCI_CARDS),     # Array of matching device IDs
    ("deviceSlot", WD_PCI_SLOT * WD_PCI_CARDS)] # Array of matching device locations

class WDC_PCI_SCAN_CAPS_RESULT(Structure): _fields_ = \
    [("dwNumCaps", DWORD),                    # Number of matching devices
    ("pciCaps", WD_PCI_CAP * WD_PCI_MAX_CAPS)]     # Array of matching PCI capabilities


# Driver open options
# Basic driver open flags
WDC_DRV_OPEN_CHECK_VER = 0x1 # Compare source files WinDriver version
                                    # with that of the running WinDriver kernel

WDC_DRV_OPEN_REG_LIC   = 0x2 # Register WinDriver license
# Convenient driver open options
WDC_DRV_OPEN_BASIC     = 0x0 # No option -> perform only the basic open
                             # driver tasks, which are always performed
                             # by WDC_DriverOpen (mainly - open a handle
                             # to WinDriver)
WDC_DRV_OPEN_KP  = WDC_DRV_OPEN_BASIC # Kernel PlugIn driver open
                                            # options <=> basic
WDC_DRV_OPEN_ALL  = (WDC_DRV_OPEN_CHECK_VER | WDC_DRV_OPEN_REG_LIC)
#if defined(__KERNEL__)
#     WDC_DRV_OPEN_DEFAULT WDC_DRV_OPEN_KP
#else
WDC_DRV_OPEN_DEFAULT = WDC_DRV_OPEN_ALL
#endif
WDC_DRV_OPEN_OPTIONS = DWORD

# Basic driver open flags

WDC_DRV_OPEN_CHECK_VER = 0x1 # Compare source files WinDriver version
                             # with that of the running WinDriver kernel
WDC_DRV_OPEN_REG_LIC =  0x2  # Register WinDriver license
                             # Convenient driver open options
WDC_DRV_OPEN_BASIC   =  0x0  # No option -> perform only the basic open
                             # driver tasks, which are always performed
                             # by WDC_DriverOpen (mainly - open a handle
                             # to WinDriver)

# Debug information display options
WDC_DBG_OUT_DBM     = 0x1   # Send WDC debug messages to the
                            # Debug Monitor
WDC_DBG_OUT_FILE    = 0x2   # Send WDC debug messages to a debug file
                            # (default: stderr) [User-mode only]
WDC_DBG_LEVEL_ERR   = 0x10  # Display only error WDC debug messages
WDC_DBG_LEVEL_TRACE = 0x20  # Display error and trace
                            # WDC debug messages
WDC_DBG_NONE        = 0x100 # Do not print debug messages

# Convenient debug options combinations/defintions
WDC_DBG_DEFAULT   =  (WDC_DBG_OUT_DBM | WDC_DBG_LEVEL_TRACE)

WDC_DBG_DBM_ERR   = (WDC_DBG_OUT_DBM | WDC_DBG_LEVEL_ERR)
WDC_DBG_DBM_TRACE = (WDC_DBG_OUT_DBM | WDC_DBG_LEVEL_TRACE)

WDC_DBG_FILE_ERR   = (WDC_DBG_OUT_FILE | WDC_DBG_LEVEL_ERR)
WDC_DBG_FILE_TRACE = (WDC_DBG_OUT_FILE | WDC_DBG_LEVEL_TRACE)

WDC_DBG_DBM_FILE_ERR  = \
        (WDC_DBG_OUT_DBM | WDC_DBG_OUT_FILE | WDC_DBG_LEVEL_ERR)
WDC_DBG_DBM_FILE_TRACE = \
        (WDC_DBG_OUT_DBM | WDC_DBG_OUT_FILE | WDC_DBG_LEVEL_TRACE)
WDC_DBG_FULL = \
        (WDC_DBG_OUT_DBM | WDC_DBG_OUT_FILE | WDC_DBG_LEVEL_TRACE)

WDC_DBG_OPTIONS = DWORD

#  -----------------------------------------------
#   Memory / I/O / Registers
#  -----------------------------------------------
#WDC_DIRECTION
WDC_WRITE      = 1
WDC_READ       = 2
WDC_READ_WRITE = 3
WDC_DIRECTION = DWORD

# Read/write address options
#WDC_ADDR_RW_OPTIONS
WDC_ADDR_RW_DEFAULT = 0x0    # Default: memory resource - direct access
                             # autoincrement on block transfers
WDC_ADDR_RW_NO_AUTOINC = 0x4 # Hold device address constant while
                             # reading/writing a block

# Memory/I/O address size and access mode definitions (size - in bytes)
WDC_SIZE_8 = sizeof(BYTE)
WDC_SIZE_16 = sizeof(WORD)
WDC_SIZE_32 = sizeof(UINT32)
WDC_SIZE_64 = sizeof(UINT64)
WDC_ADDR_SIZE = DWORD

WDC_MODE_8 = WDC_SIZE_8
WDC_MODE_16 = WDC_SIZE_16
WDC_MODE_32 = WDC_SIZE_32
WDC_MODE_64 = WDC_SIZE_64

# Get DMA buffer global handle
def WDC_DMAGetGlobalHandle(pDma):
    return pDma.contents.hDma

# Platform specific loading of the WinDriver user mode API shared library
DLLCALLCONV = CFUNCTYPE
WDAPI_DLL_VER = "wdapi" + WD_VER_ITOA
if sys.platform == "win32":
    wdapi = windll.LoadLibrary(WDAPI_DLL_VER + ".dll")
    DLLCALLCONV = WINFUNCTYPE
elif sys.platform == "cygwin":
    wdapi = cdll.LoadLibrary(WDAPI_DLL_VER + ".dll")
else:
    wdapi = cdll.LoadLibrary("lib" + WDAPI_DLL_VER + ".so")

# variadic functions in win 32bit require cdll loading
if sys.platform == "win32" and platform.architecture()[0] == "32bit":
    wdapi_va = cdll.LoadLibrary(WDAPI_DLL_VER + ".dll")
else:
    wdapi_va = wdapi

# TODO: some wdapi functions might require percise restype definitions in order
# to avoid segmentation faults
wdapi.Stat2Str.restype = c_char_p
