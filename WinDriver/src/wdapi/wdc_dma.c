/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

/*
 *  File: wdc_dma.c
 *  Implementation of WDC DMA API
 */

#include "utils.h"
#include "wdc_lib.h"
#include "wdc_defs.h"
#include "wdc_err.h"
#include "status_strings.h"


/*
 * Static and inline functions implementations
 */
static DWORD DMABufLock(PWDC_DEVICE pDev, PHYS_ADDR qwAddr, PVOID *ppBuf,
    DWORD dwOptions, DWORD dwDMABufSize, WD_DMA **ppDma)
{
    DWORD dwStatus;
    WD_DMA *pDma;
    DWORD dwPagesNeeded = 0, dwAllocSize;
    BOOL fSG;
    BOOL fReserved;

    if (!WdcIsValidPtr(ppDma, "NULL address of DMA struct pointer") ||
        !WdcIsValidPtr(ppBuf, "NULL address of DMA buffer pointer"))
    {
        return WD_INVALID_PARAMETER;
    }

    fSG = !(dwOptions & DMA_KERNEL_BUFFER_ALLOC);
    fReserved = (dwOptions & DMA_RESERVED_MEM);

    dwAllocSize = sizeof(WD_DMA);
    if (fSG)
    {
        /*
         * Since the first and last page do not necessarily start (or end) on a
         * PAGE_SIZE boundary, add 1 page to support the worst case scenario
         */
        dwPagesNeeded = ((dwDMABufSize + GetPageSize() - 1)/GetPageSize()) + 1;

        if (WD_DMA_PAGES < dwPagesNeeded)
        {
            dwAllocSize += ((DWORD)sizeof(WD_DMA_PAGE) * (dwPagesNeeded -
                WD_DMA_PAGES));
            dwOptions |= DMA_LARGE_BUFFER;
        }
    }

    pDma = (WD_DMA *)malloc(dwAllocSize);
    if (!pDma)
    {
        WdcSetLastErrStr("Failed allocating memory for a DMA struct\n");
        return WD_INSUFFICIENT_RESOURCES;
    }

    memset(pDma, 0, dwAllocSize);
    pDma->dwBytes = dwDMABufSize;
    pDma->dwOptions = dwOptions;
    pDma->hCard = pDev ? WDC_GET_CARD_HANDLE(pDev) : 0;

    if (fSG)
    {
        pDma->pUserAddr = *ppBuf;

        if (dwOptions & DMA_LARGE_BUFFER)
            pDma->dwPages = dwPagesNeeded;
    }

    if (fReserved)
         pDma->Page[0].pPhysicalAddr = qwAddr;

    dwStatus = WD_DMALock(WDC_GetWDHandle(), pDma);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        free(pDma);
        WdcSetLastErrStr("Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        return dwStatus;
    }

    /* If a contiguous/reserved kernel buffer was locked, update buffer */
    if (!fSG)
        *ppBuf = pDma->pUserAddr;

    *ppDma = pDma;

    return WD_STATUS_SUCCESS;
}

/*
 * Functions implementations
 */
DWORD DLLCALLCONV WDC_DMAContigBufLock(WDC_DEVICE_HANDLE hDev, PVOID *ppBuf,
    DWORD dwOptions, DWORD dwDMABufSize, WD_DMA **ppDma)
{
    DWORD dwStatus;

    dwOptions |= DMA_KERNEL_BUFFER_ALLOC;

    dwStatus = DMABufLock((PWDC_DEVICE)hDev, 0, ppBuf, dwOptions, dwDMABufSize,
        ppDma);
    if (WD_STATUS_SUCCESS != dwStatus)
        WDC_Err("WDC_DMAContigBufLock: %s", WdcGetLastErrStr());

    return dwStatus;
}

DWORD DLLCALLCONV WDC_DMASGBufLock(WDC_DEVICE_HANDLE hDev, PVOID pBuf,
    DWORD dwOptions, DWORD dwDMABufSize, WD_DMA **ppDma)
{
    DWORD dwStatus;

    if (dwOptions & DMA_KERNEL_BUFFER_ALLOC)
    {
        WDC_Err("WDC_DMASGBufLock: Error - The DMA_KERNEL_BUFFER_ALLOC flag "
            "should not be set when locking a Scatter/Gather DMA buffer\n");
        return WD_INVALID_PARAMETER;
    }

    dwStatus = DMABufLock((PWDC_DEVICE)hDev, 0, &pBuf, dwOptions, dwDMABufSize,
        ppDma);
    if (WD_STATUS_SUCCESS != dwStatus)
        WDC_Err("WDC_DMASGBufLock: %s\n", WdcGetLastErrStr());

    return dwStatus;
}

DWORD DLLCALLCONV WDC_DMAReservedBufLock(WDC_DEVICE_HANDLE hDev,
    PHYS_ADDR qwAddr, PVOID *ppBuf, DWORD dwOptions, DWORD dwDMABufSize,
    WD_DMA **ppDma)
{
    DWORD dwStatus;

    dwOptions |= DMA_RESERVED_MEM;
    dwOptions |= DMA_KERNEL_BUFFER_ALLOC;

    dwStatus = DMABufLock((PWDC_DEVICE)hDev, qwAddr, ppBuf, dwOptions,
        dwDMABufSize, ppDma);
    if (WD_STATUS_SUCCESS != dwStatus)
        WDC_Err("WDC_DMAReservedBufLock: %s\n", WdcGetLastErrStr());

    return dwStatus;
}

DWORD DLLCALLCONV WDC_DMASyncCpu(WD_DMA *pDma)
{
    DWORD dwStatus;

    dwStatus = WD_DMASyncCpu(WDC_GetWDHandle(), pDma);
    if (WD_STATUS_SUCCESS != dwStatus)
        WDC_Err("WDC_DMASyncCpu: %s\n", WdcGetLastErrStr());

    return dwStatus;
}

DWORD DLLCALLCONV WDC_DMASyncIo(WD_DMA *pDma)
{
    DWORD dwStatus;

    dwStatus = WD_DMASyncIo(WDC_GetWDHandle(), pDma);
    if (WD_STATUS_SUCCESS != dwStatus)
        WDC_Err("WDC_DMASyncIo: %s\n", WdcGetLastErrStr());

    return dwStatus;
}

DWORD DLLCALLCONV WDC_DMABufGet(DWORD hDma, WD_DMA **ppDma)
{
    DWORD dwStatus;
    WD_DMA *pDma;

    if (!WdcIsValidPtr(ppDma, "NULL address of WD_DMA struct pointer"))
    {
        return WD_INVALID_PARAMETER;
    }

    *ppDma = NULL;

    if (!hDma)
    {
        WDC_Err("WDC_DMABufGet: Invalid shared buffer handle\n");
        return WD_INVALID_HANDLE;
    }

    pDma = (WD_DMA *)malloc(sizeof(WD_DMA));
    if (!pDma)
    {
        WDC_Err("WDC_DMABufGet: Failed allocating memory\n");
        return WD_INSUFFICIENT_RESOURCES;
    }

    BZERO(*pDma);
    pDma->hDma = hDma;
    pDma->dwOptions = DMA_GET_EXISTING_BUF;

    dwStatus = WD_DMALock(WDC_GetWDHandle(), pDma);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        WDC_Err("WDC_DMABufGet: DMA lock failed. Error 0x%lx - %s\n", dwStatus,
            Stat2Str(dwStatus));
        free(pDma);
        return dwStatus;
    }

    *ppDma = pDma;

    return WD_STATUS_SUCCESS;
}

DWORD DLLCALLCONV WDC_DMABufUnlock(WD_DMA *pDma)
{
    DWORD dwStatus = WD_STATUS_SUCCESS;

    if (!WdcIsValidPtr(pDma, "NULL pointer to DMA struct"))
    {
        WDC_Err("WDC_DMABufUnlock: %s\n", WdcGetLastErrStr());
        return WD_INVALID_PARAMETER;
    }

    if (pDma->hDma)
    {
        dwStatus = WD_DMAUnlock(WDC_GetWDHandle(), pDma);

        if (WD_STATUS_SUCCESS != dwStatus)
        {
            WDC_Err("WDC_DMABufUnlock: Failed unlocking DMA buffer.\n"
                "Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        }
    }

    free(pDma);

    return dwStatus;
}

