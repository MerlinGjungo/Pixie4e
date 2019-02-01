/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

/****************************************************************************
*  File: xdma_diag_transfer.c
*
*  Common functions for user-mode diagnostics application for accessing
*  Xilinx PCI Express cards with XDMA support, using the WinDriver WDC API.
*
*  Note: This code sample is provided AS-IS and as a guiding sample only.
*****************************************************************************/

#include "xdma_diag_transfer.h"

int XDMA_printf(char *fmt, ...)
#if defined(LINUX)
    __attribute__ ((format (printf, 1, 2)))
#endif
    ;


#define XDMA_OUT XDMA_printf
#define XDMA_ERR XDMA_printf

/* Interrupt handler routine for DMA performance testing */
void DiagXdmaDmaPerfIntHandler(WDC_DEVICE_HANDLE hDev,
    XDMA_INT_RESULT *pIntResult)
{
    DWORD dwStatus = OsEventSignal((HANDLE)pIntResult->pData);

    if (dwStatus != WD_STATUS_SUCCESS)
    {
        XDMA_ERR("Failed signalling DMA completion. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
    }

    UNUSED_VAR(hDev);
}

/* Function: get_cur_time()
     Retrieves the current time, in OS units
   Parameters:
     time [out] pointer to the OS time, in OS units
   Return Value:
     TRUE if successful, FALSE otherwise */
BOOL get_cur_time(TIME_TYPE *time)
{
#if defined(UNIX)
    return !gettimeofday(time, NULL);
#else /* WIN32 */
    return QueryPerformanceCounter(time);
#endif
}

/* Function: get_cur_time()
     Calculates a time difference in milliseconds
   Parameters:
     end   [in] End time, in OS units
     start [in] Start time, in OS units
   Return Value:
     The time difference, in milliseconds. */
DWORD time_diff(const TIME_TYPE *end, const TIME_TYPE *start)
{
#if defined(UNIX)
    return (end->tv_usec - start->tv_usec) / 1000 +
        (end->tv_sec - start->tv_sec) * 1000;
#else /* WIN32 */
    static TIME_TYPE ctr_freq = {0};

    if (!ctr_freq.QuadPart && !QueryPerformanceFrequency(&ctr_freq))
    {
        XDMA_ERR("Error reading timer frequency\n");
        return (DWORD)-1;
    }

    return (DWORD)((end->QuadPart - start->QuadPart) * 1000 /
        ctr_freq.QuadPart);
#endif
}

typedef struct {
    WDC_DEVICE_HANDLE hDev;
    XDMA_DMA_HANDLE hDma;
    DWORD dwBytes;
    BOOL fPolling;
    BOOL fToDevice;
    DWORD dwSeconds;
    HANDLE hOsEvent;
} DMA_PERF_THREAD_CTX;

void DmaPerfDevThread(void *pData)
{
    DMA_PERF_THREAD_CTX *ctx = (DMA_PERF_THREAD_CTX *)pData;
    TIME_TYPE time_start, time_end_temp;
    DWORD dwStatus, time_elapsed = 0, restarts = 0;
    UINT64 u64BytesTransferred = 0;

    get_cur_time(&time_start);
    while (time_elapsed < ctx->dwSeconds * 1000)
    {
        dwStatus = XDMA_DmaTransferStart(ctx->hDma);
        if (dwStatus != WD_STATUS_SUCCESS)
        {
            XDMA_ERR("\nFailed starting DMA transfer. Error 0x%lx - %s\n",
                dwStatus, Stat2Str(dwStatus));
            break;
        }

        if (ctx->fPolling)
        {
            dwStatus = XDMA_DmaPollCompletion(ctx->hDma);
            if (dwStatus != WD_STATUS_SUCCESS)
            {
                XDMA_ERR("\nFailed polling for DMA completion. "
                    "Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
                break;
            }
        }
        else
        {
            dwStatus = OsEventWait(ctx->hOsEvent, 1);
            if (dwStatus == WD_TIME_OUT_EXPIRED)
            {
#define MAX_RESTARTS 2
                /* In case of timeout try to restart the test because timeout
                 * may occur because of a missed interrupt */
                if (restarts++ >= MAX_RESTARTS)
                {
                    XDMA_ERR("Timeout occurred\n");
                    break;
                }
                XDMA_DmaTransferStop(ctx->hDma);
                time_elapsed = 0;
                u64BytesTransferred = 0;
                get_cur_time(&time_start);
                continue;
            }
            else if (dwStatus != WD_STATUS_SUCCESS)
            {
                XDMA_ERR("\nFailed waiting for completion event. "
                    "Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
                break;
            }
        }

        u64BytesTransferred += (UINT64)ctx->dwBytes;
        get_cur_time(&time_end_temp);
        time_elapsed = time_diff(&time_end_temp, &time_start);
        if (time_elapsed == -1)
        {
            XDMA_ERR("Performance test failed\n");
            return;
        }
    }
     if (!time_elapsed)
    {
        XDMA_OUT("DMA %s performance test failed\n",
                ctx->fToDevice ? "host-to-device" : "device-to-host");
        return;
    }

    XDMA_OUT("\n\n");

    XDMA_OUT("DMA %s performance results:\n"
        "---------------------------------------\n"
        "Transferred %"PRI64"d[MB], test duration %ld[ms], "
        "rate %"PRI64"d[MB/sec]\n",
        ctx->fToDevice ? "host-to-device" : "device-to-host",
        u64BytesTransferred / (UINT64)(1024 * 1024), time_elapsed,
        /* (bytes / msec) * sec / MB */
        ((u64BytesTransferred / (UINT64)(time_elapsed)) * 1000) /
        (1024 * 1024));
}

HANDLE DmaPerformanceThreadStart(DMA_PERF_THREAD_CTX *ctx)
{
    DWORD dwStatus;
    HANDLE hThread;

    dwStatus = ThreadStart(&hThread, (HANDLER_FUNC)DmaPerfDevThread, ctx);
    if (dwStatus != WD_STATUS_SUCCESS)
    {
        XDMA_ERR("\nFailed starting performance thread. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
        return NULL;
    }

    return hThread;
}

DMA_PERF_THREAD_CTX *DmaPerfThreadInit(WDC_DEVICE_HANDLE hDev,
    DWORD dwBytes, UINT64 u64Offset, BOOL fPolling, DWORD dwSeconds,
    DWORD fToDevice)
{
    DMA_PERF_THREAD_CTX *ctx = NULL;
    DWORD dwStatus;

    ctx = (DMA_PERF_THREAD_CTX *)calloc(1, sizeof(DMA_PERF_THREAD_CTX));
    if (!ctx)
    {
        XDMA_ERR("Memory allocation error\n");
        return NULL;
    }

    if (!fPolling)
    {
        if (XDMA_IntIsEnabled(hDev))
            XDMA_IntDisable(hDev);

        dwStatus = OsEventCreate(&ctx->hOsEvent);
        if (dwStatus != WD_STATUS_SUCCESS)
        {
            XDMA_ERR("\nFailed creating event. Error 0x%lx - %s\n",
                dwStatus, Stat2Str(dwStatus));
            goto Error;
        }

        if (!XDMA_IntIsEnabled(hDev))
        {
            dwStatus = XDMA_IntEnable(hDev, DiagXdmaDmaPerfIntHandler);
            if (dwStatus != WD_STATUS_SUCCESS)
            {
                XDMA_ERR("\nFailed enabling interrupts. Error 0x%lx - %s\n",
                    dwStatus, Stat2Str(dwStatus));
                goto Error;
            }
        }
    }

    dwStatus = XDMA_DmaOpen(hDev, &ctx->hDma, dwBytes, u64Offset, fToDevice, 0,
        fPolling, FALSE, ctx->hOsEvent);
    if (dwStatus != WD_STATUS_SUCCESS)
    {
        XDMA_ERR("\nFailed to open DMA handle. Error 0x%lx - %s\n", dwStatus,
            Stat2Str(dwStatus));
        goto Error;
    }

    ctx->hDev = hDev;
    ctx->fPolling = fPolling;
    ctx->dwBytes = dwBytes;
    ctx->fToDevice = fToDevice;
    ctx->dwSeconds = dwSeconds;

    return ctx;

Error:
    if (!fPolling && XDMA_IntIsEnabled(hDev))
        XDMA_IntDisable(hDev);
    if (ctx->hOsEvent)
        OsEventClose(ctx->hOsEvent);
    free(ctx);
    return NULL;
}

void DmaPerfThreadUninit(DMA_PERF_THREAD_CTX *ctx)
{
    DWORD dwStatus;

    XDMA_DmaTransferStop(ctx->hDma);

    if (!ctx->fPolling)
    {
        XDMA_IntDisable(ctx->hDev);
        OsEventClose(ctx->hOsEvent);
    }

    dwStatus = XDMA_DmaClose(ctx->hDma);
    if (dwStatus != WD_STATUS_SUCCESS)
    {
        XDMA_ERR("\nFailed closing DMA handle. Error 0x%lx - %s\n", dwStatus,
            Stat2Str(dwStatus));
    }

    free(ctx);
}

void DmaPerformanceSingleDir(WDC_DEVICE_HANDLE hDev, DWORD dwBytes,
    BOOL fPolling, DWORD dwSeconds, DWORD fToDevice)
{
    HANDLE hThread;
    DMA_PERF_THREAD_CTX *ctx;

    ctx = DmaPerfThreadInit(hDev, dwBytes, 0, fPolling, dwSeconds, fToDevice);
    if (!ctx)
    {
        XDMA_ERR("Failed initializing performance thread context\n");
        return;
    }

    hThread = DmaPerformanceThreadStart(ctx);
    if (!hThread)
    {
        XDMA_ERR("Failed starting performance thread\n");
        goto Exit;
    }

    ThreadWait(hThread);

Exit:
    DmaPerfThreadUninit(ctx);
}

void DmaPerformanceBiDir(WDC_DEVICE_HANDLE hDev, DWORD dwBytes,
    BOOL fPolling, DWORD dwSeconds)
{
    HANDLE hThreadToDev, hThreadFromDev;
    DMA_PERF_THREAD_CTX *pCtxToDev = NULL, *pCtxFromDev = NULL;

    pCtxToDev = DmaPerfThreadInit(hDev, dwBytes, 0, fPolling, dwSeconds, TRUE);
    if (!pCtxToDev)
    {
        XDMA_ERR("Failed initializing performance thread context\n");
        return;
    }

    pCtxFromDev = DmaPerfThreadInit(hDev, dwBytes, (UINT64)(dwBytes * 2),
        fPolling, dwSeconds, FALSE);
    if (!pCtxFromDev)
    {
        XDMA_ERR("Failed initializing performance thread context\n");
        goto Exit;
    }

    hThreadToDev = DmaPerformanceThreadStart(pCtxToDev);
    if (!hThreadToDev)
        XDMA_ERR("Failed starting DMA host-to-device performance thread\n");

    hThreadFromDev = DmaPerformanceThreadStart(pCtxFromDev);
    if (!hThreadFromDev)
        XDMA_ERR("Failed starting DMA device-to-host performance thread\n");

    if (hThreadToDev)
        ThreadWait(hThreadToDev);
    if (hThreadFromDev)
        ThreadWait(hThreadFromDev);

Exit:
    if (pCtxToDev)
        DmaPerfThreadUninit(pCtxToDev);
    if (pCtxFromDev)
        DmaPerfThreadUninit(pCtxFromDev);
}

void XDMA_DIAG_DmaPerformance(WDC_DEVICE_HANDLE hDev, DWORD dwOption,
    DWORD dwBytes, BOOL fPolling, DWORD dwSeconds)
{
#if 0
    char direction[32];

    /* done separately to allow printing of the complete message before
     * threads are started */
    switch (dwOption)
    {
    case MENU_DMA_PERF_TO_DEV:
        sprintf(direction, "host-to-device");
        break;
    case MENU_DMA_PERF_FROM_DEV:
        sprintf(direction, "device-to-host");
        break;
    case MENU_DMA_PERF_BIDIR:
        sprintf(direction, "bi-directional");
        break;
    }
    XDMA_OUT("\nRunning DMA %s performance test, wait %ld seconds "
        "to finish...\n", direction, dwSeconds);
#else
    XDMA_OUT("\nRunning DMA %s performance test, wait %ld seconds "
        "to finish...\n",
        dwOption == MENU_DMA_PERF_TO_DEV ? "host-to-device" :
        dwOption == MENU_DMA_PERF_FROM_DEV ? "device-to-host" :
        "bi-directional",
        dwSeconds);
#endif

    switch (dwOption)
    {
    case MENU_DMA_PERF_TO_DEV:
        DmaPerformanceSingleDir(hDev, dwBytes, fPolling, dwSeconds, TRUE);
        break;
    case MENU_DMA_PERF_FROM_DEV:
        DmaPerformanceSingleDir(hDev, dwBytes, fPolling, dwSeconds, FALSE);
        break;
    case MENU_DMA_PERF_BIDIR:
        DmaPerformanceBiDir(hDev, dwBytes, fPolling, dwSeconds);
        break;
    }
}

/* DMA Transfer functions */

static VOID DumpBuffer(UINT32 *buf, DWORD dwBytes)
{
    DWORD i;

    XDMA_OUT("Buffer:\n\n");
    for (i = 0; i < dwBytes / sizeof(UINT32); i++)
    {
        XDMA_OUT("%08x ", buf[i]);
        if (i && !(i % 32))
            XDMA_OUT("\n");
    }
    XDMA_OUT("\n\n");
}

static void DumpDmaBuffer(XDMA_DMA_HANDLE hDma)
{
    PVOID pBuf;
    DWORD dwBytes;

    pBuf = XDMA_DmaBufferGet(hDma, &dwBytes);
    if (!pBuf || !dwBytes)
    {
        XDMA_OUT("Invalid DMA buffer\n");
        return;
    }
    DumpBuffer(pBuf, dwBytes);
}

/* Interrupt handler routine */
static void DiagXdmaTransferIntHandler(WDC_DEVICE_HANDLE hDev,
    XDMA_INT_RESULT *pIntResult)
{
    UNUSED_VAR(hDev);

    XDMA_OUT("\n###\n%s Interrupt #%ld received, DMA status 0x%08x, "
        "interrupt status 0x%08x\n",
        pIntResult->fIsMessageBased ?  "Message Signalled" : "Level Sensitive",
        pIntResult->dwCounter, pIntResult->u32DmaStatus,
        pIntResult->u32IntStatus);

    if (pIntResult->fIsMessageBased)
        XDMA_OUT("MSI data 0x%lx\n", pIntResult->dwLastMessage);

    XDMA_OUT("###\n\n");

    if (pIntResult->hDma)
    {
        DWORD dwStatus = OsEventSignal((HANDLE)pIntResult->pData);

        if (dwStatus != WD_STATUS_SUCCESS)
        {
            XDMA_ERR("Failed signalling DMA completion. Error 0x%lx - %s\n",
                dwStatus, Stat2Str(dwStatus));
        }
    }
    else
    {
        XDMA_OUT("Error: DMA handle is NULL\n");
    }
}

/* Open DMA handle and perform DMA transfer */
XDMA_DMA_HANDLE XDMA_DIAG_DmaOpen(WDC_DEVICE_HANDLE hDev, BOOL fPolling,
        DWORD dwChannel, BOOL fToDevice, UINT32 u32Pattern, DWORD dwNumPackets,
        UINT64 u64FPGAOffset)
{
    XDMA_DMA_HANDLE hDma = NULL;
    DWORD dwStatus;
    HANDLE hOsEvent = NULL;

    if (!fPolling)
    {
        dwStatus = OsEventCreate(&hOsEvent);
        if (dwStatus != WD_STATUS_SUCCESS)
        {
            XDMA_ERR("\nFailed creating event. Error 0x%lx - %s\n",
                dwStatus, Stat2Str(dwStatus));
            return NULL;
        }

        if (!XDMA_IntIsEnabled(hDev))
        {
            dwStatus = XDMA_IntEnable(hDev, DiagXdmaTransferIntHandler);
            if (dwStatus != WD_STATUS_SUCCESS)
            {
                XDMA_ERR("Failed enabling interrupts, (%s)\n",
                    XDMA_GetLastErr());
                goto Exit;
            }
        }
    }

    /* Pass hOsEvent as last parameter, to enable signalling when DMA
     * completion interrupt occurs */
    dwStatus = XDMA_DmaOpen(hDev, &hDma, (dwNumPackets * sizeof(UINT32)),
        u64FPGAOffset, fToDevice, dwChannel, fPolling, FALSE, hOsEvent);
    if (dwStatus != WD_STATUS_SUCCESS)
    {
        XDMA_ERR("\nFailed to open DMA handle. Error 0x%lx - %s\n", dwStatus,
            Stat2Str(dwStatus));
        return NULL;
    }

    if (fToDevice)
    {
        DWORD dwBytes;
        DWORD i;
        UINT32 *pu32Buf =
            (UINT32 *)XDMA_DmaBufferGet(hDma, &dwBytes);

        for (i = 0; i < dwNumPackets; i++)
            pu32Buf[i] = u32Pattern;
    }

    dwStatus = XDMA_DmaTransferStart(hDma);
    if (dwStatus != WD_STATUS_SUCCESS)
    {
        XDMA_ERR("\nFailed starting DMA transfer. Error 0x%lx - %s\n", dwStatus,
            Stat2Str(dwStatus));
    }

    if (fPolling)
    {
        dwStatus = XDMA_DmaPollCompletion(hDma);
        if (dwStatus != WD_STATUS_SUCCESS)
        {
            XDMA_ERR("\nFailed polling for DMA completion. Error 0x%lx - %s\n",
                dwStatus, Stat2Str(dwStatus));
            goto Exit;
        }
    }
    else
    {
        dwStatus = OsEventWait(hOsEvent, 5);
        if (dwStatus == WD_TIME_OUT_EXPIRED)
        {
            XDMA_ERR("\nInterrupt time out. Error 0x%lx - %s\n", dwStatus,
                Stat2Str(dwStatus));
            goto Exit;
        }
        else if (dwStatus != WD_STATUS_SUCCESS)
        {
            XDMA_ERR("\nFailed waiting for completion event. "
                "Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
            goto Exit;
        }
    }
    if (!fToDevice)
        DumpDmaBuffer(hDma);
    XDMA_OUT("\nDMA transfer completed successfully\n");

Exit:
    if (!fPolling)
        OsEventClose(hOsEvent);
    return hDma;
}

/* Close DMA */
void XDMA_DIAG_DmaClose(WDC_DEVICE_HANDLE hDev, XDMA_DMA_HANDLE hDma)
{
    DWORD dwStatus = XDMA_DmaTransferStop(hDma);

    if (dwStatus != WD_STATUS_SUCCESS)
    {
        XDMA_ERR("\nFailed stopping DMA transfer. Error 0x%lx - %s\n", dwStatus,
            Stat2Str(dwStatus));
    }

    if (XDMA_IntIsEnabled(hDev))
    {
        dwStatus = XDMA_IntDisable(hDev);
        XDMA_OUT("DMA interrupts disable%s\n",
            (WD_STATUS_SUCCESS == dwStatus) ? "d" : " failed");

    }

    dwStatus = XDMA_DmaClose(hDma);
    if (dwStatus != WD_STATUS_SUCCESS)
    {
        XDMA_ERR("\nFailed to close DMA handle. Error 0x%lx - %s\n", dwStatus,
            Stat2Str(dwStatus));
    }
}
