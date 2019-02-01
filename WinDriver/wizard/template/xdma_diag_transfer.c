/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

/****************************************************************************
*  File: $$$nl$$$_diag_transfer.c
*
*  Common functions for user-mode diagnostics application for accessing
*  Xilinx PCI Express cards with XDMA support, using the WinDriver WDC API.
*
*  Note: This code sample is provided AS-IS and as a guiding sample only.
*****************************************************************************/

#include "$$$nl$$$_diag_transfer.h"

int $$$nu$$$_printf(char *fmt, ...)
#if defined(LINUX)
    __attribute__ ((format (printf, 1, 2)))
#endif
    ;


#define $$$nu$$$_OUT $$$nu$$$_printf
#define $$$nu$$$_ERR $$$nu$$$_printf

/* Interrupt handler routine for DMA performance testing */
void Diag$$$nl$$$DmaPerfIntHandler(WDC_DEVICE_HANDLE hDev,
    $$$nu$$$_INT_RESULT *pIntResult)
{
    DWORD dwStatus = OsEventSignal((HANDLE)pIntResult->pData);

    if (dwStatus != WD_STATUS_SUCCESS)
    {
        $$$nu$$$_ERR("Failed signalling DMA completion. Error 0x%lx - %s\n",
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
        $$$nu$$$_ERR("Error reading timer frequency\n");
        return (DWORD)-1;
    }

    return (DWORD)((end->QuadPart - start->QuadPart) * 1000 /
        ctr_freq.QuadPart);
#endif
}

typedef struct {
    WDC_DEVICE_HANDLE hDev;
    $$$nu$$$_DMA_HANDLE hDma;
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
        dwStatus = $$$nu$$$_DmaTransferStart(ctx->hDma);
        if (dwStatus != WD_STATUS_SUCCESS)
        {
            $$$nu$$$_ERR("\nFailed starting DMA transfer. Error 0x%lx - %s\n",
                dwStatus, Stat2Str(dwStatus));
            break;
        }

        if (ctx->fPolling)
        {
            dwStatus = $$$nu$$$_DmaPollCompletion(ctx->hDma);
            if (dwStatus != WD_STATUS_SUCCESS)
            {
                $$$nu$$$_ERR("\nFailed polling for DMA completion. "
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
                    $$$nu$$$_ERR("Timeout occurred\n");
                    break;
                }
                $$$nu$$$_DmaTransferStop(ctx->hDma);
                time_elapsed = 0;
                u64BytesTransferred = 0;
                get_cur_time(&time_start);
                continue;
            }
            else if (dwStatus != WD_STATUS_SUCCESS)
            {
                $$$nu$$$_ERR("\nFailed waiting for completion event. "
                    "Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
                break;
            }
        }

        u64BytesTransferred += (UINT64)ctx->dwBytes;
        get_cur_time(&time_end_temp);
        time_elapsed = time_diff(&time_end_temp, &time_start);
        if (time_elapsed == -1)
        {
            $$$nu$$$_ERR("Performance test failed\n");
            return;
        }
    }
     if (!time_elapsed)
    {
        $$$nu$$$_OUT("DMA %s performance test failed\n",
                ctx->fToDevice ? "host-to-device" : "device-to-host");
        return;
    }

    $$$nu$$$_OUT("\n\n");

    $$$nu$$$_OUT("DMA %s performance results:\n"
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
        $$$nu$$$_ERR("\nFailed starting performance thread. Error 0x%lx - %s\n",
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
        $$$nu$$$_ERR("Memory allocation error\n");
        return NULL;
    }

    if (!fPolling)
    {
        if ($$$nu$$$_IntIsEnabled(hDev))
            $$$nu$$$_IntDisable(hDev);

        dwStatus = OsEventCreate(&ctx->hOsEvent);
        if (dwStatus != WD_STATUS_SUCCESS)
        {
            $$$nu$$$_ERR("\nFailed creating event. Error 0x%lx - %s\n",
                dwStatus, Stat2Str(dwStatus));
            goto Error;
        }

        if (!$$$nu$$$_IntIsEnabled(hDev))
        {
            dwStatus = $$$nu$$$_IntEnable(hDev, Diag$$$nl$$$DmaPerfIntHandler);
            if (dwStatus != WD_STATUS_SUCCESS)
            {
                $$$nu$$$_ERR("\nFailed enabling interrupts. Error 0x%lx - %s\n",
                    dwStatus, Stat2Str(dwStatus));
                goto Error;
            }
        }
    }

    dwStatus = $$$nu$$$_DmaOpen(hDev, &ctx->hDma, dwBytes, u64Offset, fToDevice, 0,
        fPolling, FALSE, ctx->hOsEvent);
    if (dwStatus != WD_STATUS_SUCCESS)
    {
        $$$nu$$$_ERR("\nFailed to open DMA handle. Error 0x%lx - %s\n", dwStatus,
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
    if (!fPolling && $$$nu$$$_IntIsEnabled(hDev))
        $$$nu$$$_IntDisable(hDev);
    if (ctx->hOsEvent)
        OsEventClose(ctx->hOsEvent);
    free(ctx);
    return NULL;
}

void DmaPerfThreadUninit(DMA_PERF_THREAD_CTX *ctx)
{
    DWORD dwStatus;

    $$$nu$$$_DmaTransferStop(ctx->hDma);

    if (!ctx->fPolling)
    {
        $$$nu$$$_IntDisable(ctx->hDev);
        OsEventClose(ctx->hOsEvent);
    }

    dwStatus = $$$nu$$$_DmaClose(ctx->hDma);
    if (dwStatus != WD_STATUS_SUCCESS)
    {
        $$$nu$$$_ERR("\nFailed closing DMA handle. Error 0x%lx - %s\n", dwStatus,
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
        $$$nu$$$_ERR("Failed initializing performance thread context\n");
        return;
    }

    hThread = DmaPerformanceThreadStart(ctx);
    if (!hThread)
    {
        $$$nu$$$_ERR("Failed starting performance thread\n");
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
        $$$nu$$$_ERR("Failed initializing performance thread context\n");
        return;
    }

    pCtxFromDev = DmaPerfThreadInit(hDev, dwBytes, (UINT64)(dwBytes * 2),
        fPolling, dwSeconds, FALSE);
    if (!pCtxFromDev)
    {
        $$$nu$$$_ERR("Failed initializing performance thread context\n");
        goto Exit;
    }

    hThreadToDev = DmaPerformanceThreadStart(pCtxToDev);
    if (!hThreadToDev)
        $$$nu$$$_ERR("Failed starting DMA host-to-device performance thread\n");

    hThreadFromDev = DmaPerformanceThreadStart(pCtxFromDev);
    if (!hThreadFromDev)
        $$$nu$$$_ERR("Failed starting DMA device-to-host performance thread\n");

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

void $$$nu$$$_DIAG_DmaPerformance(WDC_DEVICE_HANDLE hDev, DWORD dwOption,
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
    $$$nu$$$_OUT("\nRunning DMA %s performance test, wait %ld seconds "
        "to finish...\n", direction, dwSeconds);
#else
    $$$nu$$$_OUT("\nRunning DMA %s performance test, wait %ld seconds "
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

    $$$nu$$$_OUT("Buffer:\n\n");
    for (i = 0; i < dwBytes / sizeof(UINT32); i++)
    {
        $$$nu$$$_OUT("%08x ", buf[i]);
        if (i && !(i % 32))
            $$$nu$$$_OUT("\n");
    }
    $$$nu$$$_OUT("\n\n");
}

static void DumpDmaBuffer($$$nu$$$_DMA_HANDLE hDma)
{
    PVOID pBuf;
    DWORD dwBytes;

    pBuf = $$$nu$$$_DmaBufferGet(hDma, &dwBytes);
    if (!pBuf || !dwBytes)
    {
        $$$nu$$$_OUT("Invalid DMA buffer\n");
        return;
    }
    DumpBuffer(pBuf, dwBytes);
}

/* Interrupt handler routine */
static void Diag$$$nl$$$TransferIntHandler(WDC_DEVICE_HANDLE hDev,
    $$$nu$$$_INT_RESULT *pIntResult)
{
    UNUSED_VAR(hDev);

    $$$nu$$$_OUT("\n###\n%s Interrupt #%ld received, DMA status 0x%08x, "
        "interrupt status 0x%08x\n",
        pIntResult->fIsMessageBased ?  "Message Signalled" : "Level Sensitive",
        pIntResult->dwCounter, pIntResult->u32DmaStatus,
        pIntResult->u32IntStatus);

    if (pIntResult->fIsMessageBased)
        $$$nu$$$_OUT("MSI data 0x%lx\n", pIntResult->dwLastMessage);

    $$$nu$$$_OUT("###\n\n");

    if (pIntResult->hDma)
    {
        DWORD dwStatus = OsEventSignal((HANDLE)pIntResult->pData);

        if (dwStatus != WD_STATUS_SUCCESS)
        {
            $$$nu$$$_ERR("Failed signalling DMA completion. Error 0x%lx - %s\n",
                dwStatus, Stat2Str(dwStatus));
        }
    }
    else
    {
        $$$nu$$$_OUT("Error: DMA handle is NULL\n");
    }
}

/* Open DMA handle and perform DMA transfer */
$$$nu$$$_DMA_HANDLE $$$nu$$$_DIAG_DmaOpen(WDC_DEVICE_HANDLE hDev, BOOL fPolling,
        DWORD dwChannel, BOOL fToDevice, UINT32 u32Pattern, DWORD dwNumPackets,
        UINT64 u64FPGAOffset)
{
    $$$nu$$$_DMA_HANDLE hDma = NULL;
    DWORD dwStatus;
    HANDLE hOsEvent = NULL;

    if (!fPolling)
    {
        dwStatus = OsEventCreate(&hOsEvent);
        if (dwStatus != WD_STATUS_SUCCESS)
        {
            $$$nu$$$_ERR("\nFailed creating event. Error 0x%lx - %s\n",
                dwStatus, Stat2Str(dwStatus));
            return NULL;
        }

        if (!$$$nu$$$_IntIsEnabled(hDev))
        {
            dwStatus = $$$nu$$$_IntEnable(hDev, Diag$$$nl$$$TransferIntHandler);
            if (dwStatus != WD_STATUS_SUCCESS)
            {
                $$$nu$$$_ERR("Failed enabling interrupts, (%s)\n",
                    $$$nu$$$_GetLastErr());
                goto Exit;
            }
        }
    }

    /* Pass hOsEvent as last parameter, to enable signalling when DMA
     * completion interrupt occurs */
    dwStatus = $$$nu$$$_DmaOpen(hDev, &hDma, (dwNumPackets * sizeof(UINT32)),
        u64FPGAOffset, fToDevice, dwChannel, fPolling, FALSE, hOsEvent);
    if (dwStatus != WD_STATUS_SUCCESS)
    {
        $$$nu$$$_ERR("\nFailed to open DMA handle. Error 0x%lx - %s\n", dwStatus,
            Stat2Str(dwStatus));
        return NULL;
    }

    if (fToDevice)
    {
        DWORD dwBytes;
        DWORD i;
        UINT32 *pu32Buf =
            (UINT32 *)$$$nu$$$_DmaBufferGet(hDma, &dwBytes);

        for (i = 0; i < dwNumPackets; i++)
            pu32Buf[i] = u32Pattern;
    }

    dwStatus = $$$nu$$$_DmaTransferStart(hDma);
    if (dwStatus != WD_STATUS_SUCCESS)
    {
        $$$nu$$$_ERR("\nFailed starting DMA transfer. Error 0x%lx - %s\n", dwStatus,
            Stat2Str(dwStatus));
    }

    if (fPolling)
    {
        dwStatus = $$$nu$$$_DmaPollCompletion(hDma);
        if (dwStatus != WD_STATUS_SUCCESS)
        {
            $$$nu$$$_ERR("\nFailed polling for DMA completion. Error 0x%lx - %s\n",
                dwStatus, Stat2Str(dwStatus));
            goto Exit;
        }
    }
    else
    {
        dwStatus = OsEventWait(hOsEvent, 5);
        if (dwStatus == WD_TIME_OUT_EXPIRED)
        {
            $$$nu$$$_ERR("\nInterrupt time out. Error 0x%lx - %s\n", dwStatus,
                Stat2Str(dwStatus));
            goto Exit;
        }
        else if (dwStatus != WD_STATUS_SUCCESS)
        {
            $$$nu$$$_ERR("\nFailed waiting for completion event. "
                "Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
            goto Exit;
        }
    }
    if (!fToDevice)
        DumpDmaBuffer(hDma);
    $$$nu$$$_OUT("\nDMA transfer completed successfully\n");

Exit:
    if (!fPolling)
        OsEventClose(hOsEvent);
    return hDma;
}

/* Close DMA */
void $$$nu$$$_DIAG_DmaClose(WDC_DEVICE_HANDLE hDev, $$$nu$$$_DMA_HANDLE hDma)
{
    DWORD dwStatus = $$$nu$$$_DmaTransferStop(hDma);

    if (dwStatus != WD_STATUS_SUCCESS)
    {
        $$$nu$$$_ERR("\nFailed stopping DMA transfer. Error 0x%lx - %s\n", dwStatus,
            Stat2Str(dwStatus));
    }

    if ($$$nu$$$_IntIsEnabled(hDev))
    {
        dwStatus = $$$nu$$$_IntDisable(hDev);
        $$$nu$$$_OUT("DMA interrupts disable%s\n",
            (WD_STATUS_SUCCESS == dwStatus) ? "d" : " failed");

    }

    dwStatus = $$$nu$$$_DmaClose(hDma);
    if (dwStatus != WD_STATUS_SUCCESS)
    {
        $$$nu$$$_ERR("\nFailed to close DMA handle. Error 0x%lx - %s\n", dwStatus,
            Stat2Str(dwStatus));
    }
}
