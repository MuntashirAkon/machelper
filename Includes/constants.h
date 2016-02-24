/*
 * constants.h
 *
 * Contains some constant values to increase code readability.
 *
 * Author: Muntashir Al-Islam <muntashir.islam96@gmail.com>
 * Date: 22 feb, 2016
 * Copyright: 2016 (c) All rights reserved.
 * License: MIT License
 */

#ifndef MACHELPER_CONSTANTS_H
#define MACHELPER_CONSTANTS_H

#define EMPTY ""

// Define `id` constants (0 - 255)
// They must have to be unique
#define DEFAULT 0
#define USB 1
#define HDD 2
#define MOUNT_EFI 3
#define KEXT 4
#define EXTRACT_DSDT 5
#define RECOVERY 6

// USB|HDD
#define CONFIG 11
#define ETHERNET 12
#define MBR_PATCH 13
#define METHOD 14
#define OS 15
#define RAW_IMG_SRC 16
#define REPLACE_KERNEL 17

// MOUNT_EFI
#define MOUNT_POINT 31

// KEXT
#define INSTALL_KEXT 41
#define UNINSTALL_KEXT 42
#define FIX_PERMISSIONS 43
#define REBUILD_CACHE 44

// RECOVERY
#define COPY_CLOVER 61
#define DISABLE_HIBERNATION 62
#define EXPORT_KERNEL_LOG 63
#define FIX_APPLE_ID 64
#define INIT 65
#define INSTALL_ON_USB 66
#define LIST_UNSIGNED 67
#define REMOVE_KEXT 68
#define REPLACE_KERNEL_RECOVERY 69

// METHOD
#define CREATEINSTALLMEDIA 141
#define BASE_BINARIES_CLONE 142
#define RAW_IMG 143

#endif //MACHELPER_CONSTANTS_H
