/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

/*
 * File - windrvr.c
 *
 * WinDriver driver name
 */

#if defined(__KERNEL__)
    #include "kpstdlib.h"
#else
    #include <stdio.h>
#endif
#include <windrvr.h>

#if defined(WIN32)
    #define snprintf _snprintf
/* Defines and includes related to SecureBoot checking */
    #define REGBUFSIZE 32
    #define SECBOOT_REG_PATH "SYSTEM\\CurrentControlSet\\Control" \
                "\\SecureBoot\\State"
    #define SECBOOT_REG_NAME "UEFISecureBootEnabled"
#endif

#ifdef __cplusplus
extern "C" {
#endif
const char* DLLCALLCONV WD_DriverName(const char *sName)
{
    static const char *sDriverName = WD_DEFAULT_DRIVER_NAME;
    static char sTmpName[WD_MAX_DRIVER_NAME_LENGTH];

    if (!sName)
        return sDriverName;

    BZERO(sTmpName);
    snprintf(sTmpName, sizeof(sTmpName), "%s%s", WD_DRIVER_NAME_PREFIX,
        sName);

    /* Driver name can only be set once */
    if (strcmp(sDriverName, WD_DEFAULT_DRIVER_NAME) &&
        strcmp(sDriverName, sTmpName))
    {
        return NULL;
    }

    sDriverName = sTmpName;

    return sDriverName;
}

#if !defined(__KERNEL__)

OS_TYPE DLLCALLCONV get_os_type(void)
{
#if defined(WIN32)
    static OS_TYPE os_type = OS_NOT_SET;
    OSVERSIONINFO lVerInfo;

    if (os_type)
        return os_type;

    lVerInfo.dwOSVersionInfoSize = sizeof(lVerInfo);
    if (!GetVersionEx(&lVerInfo))
    {
        os_type = OS_CAN_NOT_DETECT;
    }
    else if (lVerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
    {
        if (lVerInfo.dwMajorVersion <= 4)
            os_type = OS_WIN_NT_4;
        else if (lVerInfo.dwMajorVersion == 5)
            os_type = OS_WIN_NT_5;
        else if (lVerInfo.dwMajorVersion == 6)
            os_type = OS_WIN_NT_6;
        else if (lVerInfo.dwMajorVersion == 10)
            os_type = OS_WIN_NT_10;
        else
            os_type = OS_CAN_NOT_DETECT;
    }
    else
    {
        os_type = OS_CAN_NOT_DETECT;
    }

    return os_type;
#elif defined(LINUX)
    return OS_LINUX;
#else
    return OS_CAN_NOT_DETECT;
#endif
}

/*  If Secure boot is supported it can either be enabled (dwVal = 1)
    or disabled (dwVal = 0). If it's not supported the SECBOOT_REG_PATH
    is not a valid path, hence RegOpenKeyEx fails */
DWORD DLLCALLCONV check_secureBoot_enabled(void)
{
#if defined(WIN32) && !defined (ARM)
    HKEY key;
    DWORD dwDataType;
    DWORD dwStatus;
    DWORD dwSize = REGBUFSIZE;
    DWORD dwVal;

    dwStatus = (DWORD)RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        TEXT(SECBOOT_REG_PATH), 0,KEY_QUERY_VALUE, &key);

    if (dwStatus == ERROR_SUCCESS)
    {
        dwStatus = (DWORD)RegQueryValueEx(key, TEXT(SECBOOT_REG_NAME), NULL,
            &dwDataType, (PBYTE)&dwVal, &dwSize);

        if (dwVal != 1)
            dwStatus = WD_WINDRIVER_STATUS_ERROR;
    }

    RegCloseKey(key);
    return dwStatus;

#else
    return WD_NOT_IMPLEMENTED;
#endif
}

#ifdef __cplusplus
}
#endif
#endif /* !defined(__KERNEL__) */

