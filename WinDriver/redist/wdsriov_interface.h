/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

/* \n * This program is free software; you can redistribute it and\/or modify it \n * under the terms of the GNU General Public License version 2 as published by\n * the Free Software Foundation.\n * This program is distributed in the hope that it will be useful, but WITHOUT\n * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or\n * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License v2 for\n * more details.\n * You should have received a copy of the GNU General Public License along with\n * this program. If not, contact Jungo Connectivity Ltd. at\n * support@jungo.com\n */

#ifndef _WDSRIOV_INTERFACE_H_
#define _WDSRIOV_INTERFACE_H_

#if defined(LINUX_SRIOV_MODULE) || defined(LINUX_SRIOV_SUPPORT)
    #ifdef WD_DRIVER_NAME_CHANGE
        #define WD_SRIOV_FUNC_NAME(func) %DRIVER_NAME%_##func
    #else
        #define WD_SRIOV_FUNC_NAME(func) windrvr1281_##func
    #endif
#else
    #define WD_SRIOV_FUNC_NAME(func) func
#endif

#if defined(__cplusplus)
    extern "C" {
#endif

int WD_SRIOV_FUNC_NAME(OS_pci_enable_sriov)(void *pdev, int nr_virtfn);

void WD_SRIOV_FUNC_NAME(OS_pci_disable_sriov)(void *pdev);

int WD_SRIOV_FUNC_NAME(OS_pci_sriov_is_vf)(void *pdev);

int WD_SRIOV_FUNC_NAME(OS_pci_sriov_is_assigned)(void *pdev);

int WD_SRIOV_FUNC_NAME(OS_pci_sriov_get_num_vf)(void *pdev);

int WD_SRIOV_FUNC_NAME(OS_pci_sriov_vf_get_owner)(void *pdev,
    unsigned long *dwBus, unsigned long *dwSlot, unsigned long *dwFunc);

#ifdef __cplusplus
}
#endif

#endif /* _WDSRIOV_INTERFACE_H_ */

