/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

//////////////////////////////////////////////////////////////////////
// File - cmos_lib.h
//
// Library for accessing the CMOS on the motherboard directly,
// Code was generated by WinDriver DriverWizard.
// Application uses WinDriver to access hardware.
//
// Note: This code sample is provided AS-IS and as a guiding sample only.
//////////////////////////////////////////////////////////////////////

#ifndef _CMOS_LIB_H_
#define _CMOS_LIB_H_

// 'windrvr.h' is located in the WinDriver include directory,
#include "windrvr.h"
#include "bits.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    CMOS_IO = 0,
    CMOS_ITEMS = 1
} CMOS_ADDR;

// I/O ranges definitions
enum { CMOS_IO_ADDR = 0x70 };
enum { CMOS_IO_BYTES = 0x2 };

// CMOS register definitions
enum { CMOS_Address_OFFSET = 0x0 };
enum { CMOS_Data_OFFSET = 0x1 };

typedef struct CMOS_STRUCT *CMOS_HANDLE;

typedef struct CMOS_STRUCT
{
    HANDLE hWD;
    WD_CARD_REGISTER cardReg;
} CMOS_STRUCT;

BOOL CMOS_Open(CMOS_HANDLE *phCMOS);
void CMOS_Close(CMOS_HANDLE hCMOS);

BYTE CMOS_Read(CMOS_HANDLE hCMOS, BYTE addr);
void CMOS_Write(CMOS_HANDLE hCMOS, BYTE addr, BYTE data);

// this string is set to an error message, if one occurs
extern CHAR CMOS_ErrorString[];

#ifdef __cplusplus
}
#endif

#endif /* _CMOS_LIB_H_ */

