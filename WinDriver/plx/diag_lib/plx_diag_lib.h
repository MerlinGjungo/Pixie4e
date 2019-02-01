/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

#ifndef _PLX_DIAG_LIB_H_
#define _PLX_DIAG_LIB_H_

/*
 * File: plx_diag_lib.h - Header of shared PLX diagnostics library for
 *                        accessing PLX devices from the user-mode.
 *
 * Note: This code sample is provided AS-IS and as a guiding sample only.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "wdc_lib.h"
#include "../lib/plx_lib.h"
#include "bits.h"

/*************************************************************
  General definitions
 *************************************************************/
/* Error messages display */
#define PLX_DIAG_ERR printf

/*************************************************************
  Functions prototypes
 *************************************************************/
/* -----------------------------------------------
    Device find, open and close
   ----------------------------------------------- */
WDC_DEVICE_HANDLE PLX_DIAG_DeviceFindAndOpen(DWORD dwVendorId, DWORD dwDeviceId,
    BOOL fIsMaster);
BOOL PLX_DIAG_DeviceFind(DWORD dwVendorId, DWORD dwDeviceId,
    WD_PCI_SLOT *pSlot);

/* -----------------------------------------------
    Read/write memory and I/O addresses
   ----------------------------------------------- */
void PLX_DIAG_MenuReadWriteAddr(WDC_DEVICE_HANDLE hDev);
void PLX_DIAG_ReadWriteAddrLocal(WDC_DEVICE_HANDLE hDev,
    WDC_DIRECTION direction, PLX_ADDR addrSpace, WDC_ADDR_MODE mode);
void PLX_DIAG_ReadWriteAddrLocalBlock(WDC_DEVICE_HANDLE hDev,
    WDC_DIRECTION direction, PLX_ADDR addrSpace);

/* -----------------------------------------------
    Read/write PCI configuration registers
   ----------------------------------------------- */
void PLX_DIAG_MenuReadWriteCfgSpace(WDC_DEVICE_HANDLE hDev);

/* -----------------------------------------------
    Read/write run-time registers
   ----------------------------------------------- */
void PLX_DIAG_MenuReadWriteRegs(WDC_DEVICE_HANDLE hDev);

/* -----------------------------------------------
    Direct Memory Access (DMA)
   ----------------------------------------------- */
typedef struct {
    PLX_DMA_HANDLE hDma;
    BOOL  fSG;
    PVOID pBuf;
} PLX_DIAG_DMA;

void PLX_DIAG_MenuDma(WDC_DEVICE_HANDLE hDev, PLX_DIAG_DMA *pDma,
    PLX_INT_HANDLER MasterDiagDmaIntHandler);
void PLX_DIAG_DMAOpen(WDC_DEVICE_HANDLE hDev, PLX_DIAG_DMA *pDma,
    PLX_INT_HANDLER MasterDiagDmaIntHandler, PLX_DMA_CHANNEL dmaChannel);
void PLX_DIAG_DMAClose(WDC_DEVICE_HANDLE hDev, PLX_DIAG_DMA *pDma);

/* -----------------------------------------------
    Interrupt handling
   ----------------------------------------------- */
void PLX_DIAG_MenuInterrupts(WDC_DEVICE_HANDLE hDev,
    PLX_INT_HANDLER DiagIntHandler, PVOID pData);

/* ----------------------------------------------------
    Plug-and-play and power management events handling
   ---------------------------------------------------- */
void PLX_DIAG_MenuEvents(WDC_DEVICE_HANDLE hDev,
    PLX_EVENT_HANDLER DiagEventHandler);

/* -----------------------------------------------
    Access the serial EEPROM
   ----------------------------------------------- */
void PLX_DIAG_MenuEEPROM(WDC_DEVICE_HANDLE hDev, DWORD EEPROMmsb);

/* -----------------------------------------------
    Reset board
   ----------------------------------------------- */
/* NOTE: Currently supported for master devices only (PLX 9054, 9056, 9080,
 * 9656) */
void PLX_DIAG_MenuResetBoard(WDC_DEVICE_HANDLE hDev);

#ifdef __cplusplus
}
#endif

#endif /* _PLX_DIAG_LIB_H_ */

