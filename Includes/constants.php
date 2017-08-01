<?php
/**
 * Contains some constant values to increase code readability.
 *
 * Created by PhpStorm.
 * User: muntashir
 * Date: 12/23/16
 * Time: 8:25 AM
 */


define("ROOT","/tmp/helper"); // Temporary directory root

/*
 * Verbs
 */
// Install
define('VERB_INSTALL', 'install');
// EFI
define('VERB_EFI', 'efi');
// Mount EFI automatically
define('VERB_MOUNT_EFI', 'mount_efi');
// kext
define('VERB_KEXT', 'kext');
// DSDT
define('VERB_DSDT', 'dsdt');
// hackupdater == hu
define('VERB_HACKUPDATER', 'hackupdater');
define('VERB_HU', 'hu');
// Help
define('VERB_HELP', 'help');
// Version and Info == version
define('VERB_VERSION', 'version');
define('VERB_INFO', 'info');

/*
 * Options for VERB_EFI
 */
define('EFI_LIST', 'list');
define('EFI_MOUNT', 'mount');
// unmount == eject
define('EFI_UNMOUNT', 'unmount');
define('EFI_EJECT', 'eject');
define('EFI_HELP', 'help');

/*
 * Options for VERB_KEXT
 */
define('KEXT_INSTALL', 'install');
// uninstall == purge
define('KEXT_UNINSTALL', 'uninstall');
define('KEXT_PURGE', 'purge');
define('KEXT_FIX_PERMISSIONS', 'fixPermissions');
define('KEXT_REBUILD_CACHE', 'rebuildCache');
define('KEXT_HELP', 'help');

/*
 * Options for VERB_DSDT
 */
define('DSDT_CHECK', 'check');
define('DSDT_EXTRACT', 'extract');
define('DSDT_DISASSEMBLE', 'disassemble');
define('DSDT_HELP', 'help');

/*
 * Options for VERB_HACKUPDATER or VERB_HU
 */
define('HU_INSTALL', 'install');
define('HU_UNINSTALL', 'uninstall');
define('HU_ENABLE', 'enable');
define('HU_DISABLE', 'disable');
define('HU_STATUS', 'status');
define('HU_UPDATE', 'update');
define('HU_LIST', 'list');
define('HU_ADD', 'add');
define('HU_REMOVE', 'remove');
define('HU_HELP', 'help');

/*
 * Option for VERB_HELP
 */
define('HELP_EFI', 1);
define('HELP_KEXT', 2);
define('HELP_DSDT', 3);
define('HELP_HU', 4);

// Old

// Supported OSs
define('MAVERICKS',  1);
define('YOSEMITE',   2);
define('EL_CAPITAN', 3);
define('SIERRA',     4);

// Define `id` constants (0 - 255)
// They must have to be unique
define("DEFAULT",      0);
define("USB",          1);
define("HDD",          2);
define("RECOVERY",     6);

// USB|HDD 1|2
define("CONFIG",         1);
define("ETHERNET",       2);
define("MBR_PATCH",      3);
define("METHOD",         4);
define("OS",             5);
define("REPLACE_KERNEL", 6);
define("SKIP_CLOVER",    7);
define("SKIP_OS",        8);
define("RAID",           9);
define("DEVICE_TYPE",   10);

// RECOVERY 6
define("COPY_CLOVER",61);
define("DISABLE_HIBERNATION",62);
define("EXPORT_KERNEL_LOG",63);
define("FIX_APPLE_ID",64);
define("INIT",65);
define("INSTALL_ON_USB",66);
define("LIST_UNSIGNED",67);
define("REMOVE_KEXT",68);
define("REPLACE_KERNEL_RECOVERY",69);

// METHOD 4
define("CREATEINSTALLMEDIA",  1);
define("BASE_BINARIES_CLONE", 2);
define("RAW_IMG",             3);

// DEVICE_TYPE 10
define("LAPTOP",  1);
define("DESKTOP", 2);

// ETHERNET 2
define("AppleIntelE1000e",     1);
define("AtherosE2200Ethernet", 2);
define("BroadcomBCM57xx",      3);
define("IntelMausiEthernet",   4);
define("RealtekRTL8100",       5);
define("RealtekRTL8111",       6);