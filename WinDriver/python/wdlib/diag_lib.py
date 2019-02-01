# Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com

from ctypes import *
from windrvr import *
import sys, string

BYTES_IN_LINE      = 16
HEX_CHARS_PER_BYTE = 3
HEX_STOP_POS       = BYTES_IN_LINE * HEX_CHARS_PER_BYTE
DIAG_INPUT_CANCEL = -1,
DIAG_INPUT_FAIL = 0,
DIAG_INPUT_SUCCESS = 1
DIAG_EXIT_MENU = 99
DIAG_CANCEL = 'x'

#************************************************************
#  Functions implementation
# ***********************************************************

def DIAG_PrintHexBuffer(pBuf, dwBytes):
    if not pBuf or not dwBytes:
        print("DIAG_PrintHexBuffer: Error - %s\n" %
            ("NULL buffer pointer" if not pBuf else "Empty buffer"))
        return

    for offset in range(dwBytes.value):
        print("%c%c" % (pBuf[offset].encode("hex")[0].upper(),
            pBuf[offset].encode("hex")[1].upper())),
        if offset % BYTES_IN_LINE == BYTES_IN_LINE - 1:
            print

def DIAG_GetHexChar():
    ch = sys.stdin.read(1)
    if not ch in string.hexdigits:
        return -1

    return int(ch, 16)

def DIAG_GetHexBuffer(numBytes):
    bytesRead = 0

    pData = bytearray(numBytes)
    while bytesRead < numBytes:
        ch = DIAG_GetHexChar()
        if ch < 0:
            continue
        res = ch << 4

        ch = DIAG_GetHexChar()
        if ch < 0:
            continue

        res += ch

        pData[bytesRead] = res
        bytesRead += 1

    # Advance to new line
    while ch != '\n' and ch != '\r':
        ch = sys.stdin.read(1)

    # Return the number of bytes that was read
    return pData, bytesRead

# Get menu option from user
def DIAG_GetMenuOption(dwMax):
    dwOption = raw_input("Enter option: ")
    try:
        dwOption = long(dwOption)
    except:
        return (0, DIAG_CANCEL)

    if dwOption < 1:
        print("Invalid option\n")
        return (dwOption, DIAG_INPUT_FAIL)

    if DIAG_EXIT_MENU == dwOption:
        return (dwOption, DIAG_INPUT_SUCCESS)

    if not dwMax:
        return (dwOption, DIAG_INPUT_SUCCESS)

    if dwOption > dwMax:
        print("Invalid option: Option must be %s%ld, or %d to exit\n" %
            ("" if 1 == dwMax else "between 1 - ", dwMax, DIAG_EXIT_MENU))
        return (dwOption, DIAG_INPUT_FAIL)

    return (dwOption, DIAG_INPUT_SUCCESS)

class MaxVal:
    SI8  = 2 ** 7  - 1
    UI8  = 2 ** 8  - 1
    SI16 = 2 ** 15 - 1
    UI16 = 2 ** 16 - 1
    SI32 = 2 ** 31 - 1
    UI32 = 2 ** 32 - 1
    SI64 = 2 ** 63 - 1
    UI64 = 2 ** 64 - 1

def DIAG_InputNum(sInputText, fHex, dwSize, minInput, maxInput):
    fCheckRange = (maxInput > minInput)
    print("%s (to cancel press '%c'): %s" %
        (("Enter input" if (not sInputText) or sInputText == "" else sInputText,
        DIAG_CANCEL, "0x" if fHex else ""))),

    sInput = raw_input()
    if len(sInput) < 1:
        return ("", DIAG_INPUT_FAIL)
    if DIAG_CANCEL == sInput[0].lower():
        return (sInput, DIAG_CANCEL)

    try:
        if fHex:
            pInput = long(sInput, 16)
        else:
            pInput = long(sInput)
    except:
        return (sInput, DIAG_CANCEL)

    if fCheckRange:
        if pInput < minInput or pInput > maxInput:
            print("Invalid input: Input must be between "),
            if fHex:
                print("0x%X and 0x%X\n" % (minInput, maxInput))
            else:
                print("%d and %d\n"% (minInput, maxInput))
            return (sInput, DIAG_CANCEL)

    if sys.platform != "win32":
        if dwSize == sizeof(DWORD) and pInput < MaxVal.UI64:
            return (DWORD(pInput), DIAG_INPUT_SUCCESS)
    if dwSize == sizeof(BYTE) and pInput < MaxVal.UI8:
        return (BYTE(pInput), DIAG_INPUT_SUCCESS)
    elif dwSize == sizeof(WORD) and pInput < MaxVal.UI16:
        return (WORD(pInput), DIAG_INPUT_SUCCESS)
    elif dwSize == sizeof(UINT32) and pInput < MaxVal.UI32:
        return (UINT32(pInput), DIAG_INPUT_SUCCESS)
    elif dwSize == sizeof(UINT64) and pInput < MaxVal.UI64:
        return (UINT64(pInput), DIAG_INPUT_SUCCESS)

    return (pInput, DIAG_INPUT_SUCCESS)
