/* Jungo Connectivity Confidential. Copyright (c) 2018 Jungo Connectivity Ltd.  https://www.jungo.com */

/* \n * This program is free software; you can redistribute it and\/or modify it \n * under the terms of the GNU General Public License version 2 as published by\n * the Free Software Foundation.\n * This program is distributed in the hope that it will be useful, but WITHOUT\n * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or\n * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License v2 for\n * more details.\n * You should have received a copy of the GNU General Public License along with\n * this program. If not, contact Jungo Connectivity Ltd. at\n * support@jungo.com\n */

#ifndef _LINUX_COMMON_H_
#define _LINUX_COMMON_H_

#include <linux/version.h>

#ifndef VERSION_CODE
    #define VERSION_CODE(vers,rel,seq) ( ((vers)<<16) | ((rel)<<8) | (seq) )
#endif

/* Only allow major releases */
#if LINUX_VERSION_CODE < VERSION_CODE(2,6,0) /* not < 2.6 */
    #error "This kernel is too old: not supported by this file"
#endif
#if LINUX_VERSION_CODE > VERSION_CODE(4,17,3)
    #error "This kernel is too recent: not supported by this file"
#endif

/* Starting kernel 2.6.18, autoconf.h is included in the build
 * command line */
#if LINUX_VERSION_CODE < VERSION_CODE(2,6,18)
    #include <linux/config.h>
#endif

#endif /* _LINUX_COMMON_H_ */

