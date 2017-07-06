<?php
/**
 * Shows help
 *
 * Author: Muntashir Al-Islam <muntashir.islam96@gmail.com>
 * Date: 22 feb, 2016
 * Copyright: 2016 (c) All rights reserved.
 * License: MIT License
 */
use MacHelper\EFIMounter;

require_once __DIR__ . '/constants.php';
require_once __DIR__ . '/autoload.php';

//function show_help($id){
function show_help($verb = null){

    switch ($verb){
        case HELP_EFI:
            // TODO: interactive mode should be added
            $mount_point   = 'mount_point';
            $efi_partition = 'efi_partition';
            $help = /** @lang text */
                "\e[4mUSAGE:\e[0m ".PROGRAM_NAME." ".VERB_EFI." [option] ...\n" .
                "\e[4mOPTIONS:\e[0m\n" .
                "\e[1m ".EFI_LIST."\e[0m\tList all the EFI partitions\n" .
                "\e[1m ".EFI_MOUNT."\e[0m [\e[4m$mount_point\e[0m] [\e[4m$efi_partition\e[0m]\tMount EFI partition\n" .
                "\t\e[4m$mount_point\e[0m is a directory where the \e[4m$efi_partition\e[0m will be mounted.\n" .
                "\t\e[4m$efi_partition\e[0m is a disk identifier of the efi partition\n" .
                "\twhich can be obtained by \e[1mefi list\e[0m. If one or both of the options\n" .
                "\tare omitted, default will be used.\n" .
                "\tDefault \e[4m$mount_point\e[0m: " . EFIMounter::DEFAULT_EFI_DIR . "\n" .
                "\tDefault \e[4m$efi_partition\e[0m: the efi partition of the root disk\n" .
                "\tNOTE: If \e[4m$mount_point\e[0m doesn't exist, default will be used.\n" .
                "\e[1m ".EFI_UNMOUNT."\e[0m [\e[4m$mount_point\e[0m|\e[4m$efi_partition\e[0m]\t(also \e[1m".EFI_EJECT."\e[0m) Unmount EFI partition\n" .
                "\e[1m ".EFI_HELP."\e[0m\tThis help menu\n";
            break;
        case HELP_KEXT:
            $help = /** @lang text */
                "\e[4mUSAGE:\e[0m ".PROGRAM_NAME." ".VERB_KEXT." [option] ...\n" .
                "\e[4mOPTIONS:\e[0m\n" .
                "\e[1m ".KEXT_INSTALL."\e[0m \e[4msle\e[0m|\e[4mle\e[0m <kext ...>\tInstall kexts\n" .
                "\t\e[4msle\e[0m means the kext is to be installed at /System/Library/Extensions.\n" .
                "\t\e[4mle\e[0m means the kext is to be installed at /Library/Extensions.\n" .
                "\t<kext ...> are the kext file paths, separated by a space\n" .
                "\tDefault installation path is \e[4mle\e[0m\n" .
                "\tThe backup directory is the ~/Desktop folder if replaced.\n" .
                "\e[1m ".KEXT_UNINSTALL."\e[0m [\e[4msle\e[0m|\e[4mle\e[0m] <kext>\t(also\e[1m ".KEXT_PURGE."\e[0m) Uninstall a kext\n" .
                "\t<kext> can be either only the kext name or the full path.\n" .
                "\tFor the later case, the [\e[4msle\e[0m|\e[4mle\e[0m] is optional as\n" .
                "\tthe installation path is determined automatically.\n" .
                "\tThe backup directory is the ~/Desktop folder.\n" .
                "\e[1m ".KEXT_FIX_PERMISSIONS."\e[0m [\e[4msle\e[0m|\e[4mle\e[0m]\tFix permissions, default: \e[4msle\e[0m\n" .
                "\e[1m ".KEXT_REBUILD_CACHE."\e[0m\tRebuild kernel cache\n" .
                "\e[1m ".KEXT_HELP."\e[0m\t\tThis help menu\n" .
                "\e[4mNote:\e[0m Any kext installation fixes it's permissions automatically, " .
                "and installation or deletion of any kext or fixing permissions cause the kernel cache to be rebuilt.\n" .
                "\e[4mNote 2:\e[0m The current kext is backed up by default at ~/Desktop if it's present already.\n";
            break;
        case HELP_DSDT:
            $target      = 'target';
            $source      = 'source';
            $disassemble = 'disassemble';
            $force       = 'force';
            $help        = /** @lang text */
                "\e[4mUSAGE:\e[0m ".PROGRAM_NAME." ".VERB_DSDT." [option] ...\n" .
                "\e[4mOPTIONS:\e[0m\n" .
                "\e[1m ".DSDT_CHECK."\e[0m\tCheck if this method can properly extract original DSDT/SSDTs.\n" .
                "\e[1m ".DSDT_EXTRACT."\e[0m [\e[4m$disassemble\e[0m|\e[4m$force\e[0m] [\e[4m$target\e[0m]\tExtract original DSDT/SSDTs\n" .
                "\t\e[4m$disassemble\e[0m not only extract but also disassemble the DSDT/SSDTs\n" .
                "\t\e[4m$force\e[0m Extract DSDT/SSDTs even though they are not original\n" .
                "\t\e[4m$target\e[0m is a directory where DSDT/SSDTs will be saved\n" .
                "\tDefault \e[4m$target\e[0m: ~/Desktop/ACPI\n" .
                "\e[1m ".DSDT_DISASSEMBLE."\e[0m \e[4m$source\e[0m [\e[4m$target\e[0m]\tDisassemble DSDT/SSDTs (ie. *.aml to *.dsl)\n" .
                "\t\e[4m$source\e[0m is the directory containing DSDT/SSDTs\n" .
                "\e[1m ".DSDT_HELP."\e[0m\tThis help menu\n" .
                "\e[4mNote:\e[0m run \e[1m".VERB_DSDT." ".DSDT_CHECK."\e[0m to see if the extraction of the original DSDT/SSDTs is possible as it's also used " .
                "internally. This program cannot determine this if you're running Hackintosh in a Legacy drive.\n";
            break;
        default:
            $help = /** @lang text */
                "\e[4mUSAGE:\e[0m ".PROGRAM_NAME." <verb> [option] ...\n" .
                "\e[4mVERBS:\e[0m\n" .
                "\e[1m ".VERB_INSTALL."\e[0m\tInstall Clover, create bootable USB, etc\n" .
                "\e[1m ".VERB_EFI."\e[0m\t\tMount EFI Partition(s)\n" .
                "\e[1m ".VERB_MOUNT_EFI."\e[0m\t(alias of \e[1m".VERB_EFI." mount\e[0m) Mount the EFI Partition of the root disk\n" .
                "\e[1m ".VERB_KEXT."\e[0m\t\tKext (Kernel Extension) Management\n" .
                "\e[1m ".VERB_DSDT."\e[0m\t\tExtract and/or disassemble DSDT/SSDTs\n" .
                "\e[1m ".VERB_HACKUPDATER."\e[0m\t(also \e[1m".VERB_HU."\e[0m) Hackupdater utilities\n" .
                "\e[1m ".VERB_VERSION."\e[0m\t(also\e[1m ".VERB_INFO."\e[0m) Show the current machelper version\n" .
                "\e[4mNote:\e[0m Type\e[1m help\e[0m after each <verb> to show help.\n" .
                "\e[4mNote 2:\e[0m This program ONLY supports EFI method and GUID partition scheme.\n" .
                "\e[4mNote 3:\e[0m This program must be run WITHOUT sudo permission. It'll prompt for password if needed.\n";
    }
    fputs(STDOUT, $help);
//    switch ($id) {
//        case USB:
//        case HDD:
//            printf("\e[4mUSAGE:\e[0m machelper usb|hdd [option] ...\n"
//                           "\e[4mOPTION:\e[0m\n"
//                           "\t-config <name>\t\tChoose a laptop config.plist\n"
//                           "\t-ethernet <name>\tChoose a ethernet kext\n"
//                           "\t-help\t\t\tShow this help menu\n"
//                           "\t-raid\t\t\tIf SATA is in RAID mode\n"
//                           "\t-excludeOs\t\tExclude Mac OS installation (for USB only)\n"
//                           "\t-os <name>\t\tChoose an OS (default: El Capitan)\n"
//                           "\t-method <name>\t\tChoose a method (default: BaseBinaries Clone)\n"
//                           "\t-src <source>\t\t(Only for RawImage Method) Image Source\n"
//                           "\t-mbrPatch <path>\tPath to the folder where OSInstaller and\n"
//                           "\t\t\t\tOSInstall.mpkg located\n"
//                           "\t-replaceKernel <source>\tSource of the kernel to be replaced\n"
//                           "\t-excludeClover\t\t(For real Mac) Exclude Clover Installation\n"
//                           "\e[4mNote:\e[0m Keep the <name> blank to show help.\n"
//            );
//            break;
//
//        case RECOVERY:
//            printf("\e[4mUSAGE:\e[0m machelper recovery <option>\n"
//                           "\e[4mOPTION:\e[0m\n"
//                           "\t-copyClover\t\tCopy Clover EFI from USB to HDD/SSD\n"
//                           "\t-disableHibernation\tDisable Hibernation to fix sleep problems\n"
//                           "\t-fixAppleId\t\tReset network preferences to fix Apple ID\n"
//                           "\t-help\t\t\tShow this help menu\n"
//                           "\t-init\t\t\tChecks and mounts root when booting with \n"
//                           "\t\t\t\tSingle User Mode. This is essentially a shortcut\n"
//                           "\t\t\t\tfor `fsck -y` and `mount -uw /`\n"
//                           "\t-installOnUSB\t\tInstall 'machelper' on USB (only for \n"
//                           "\t\t\t\tBaseBinaries Clone Method)\n"
//                           "\t-listUnsigned\t\tList unsigned kext(s)\n"
//                           "\t-removeKext <source>\tUninstall/Remove a kext (backup to ~/Desktop)\n"
//                           "\t-replaceKernel <source>\tReplace Kernel (backup to ~/Desktop)\n"
//            );
//            break;
//        case CONFIG:
//            printf("\e[4mUSAGE:\e[0m machelper usb|hdd -config <name>\n"
//                           "\e[4mNAME:\e[0m\n"
//                           "\t\e[1mHD3000_1366x768\e[0m\t\tIntel HD3000 (1366x768)\n"
//                           "\t\e[1mHD3000_1366x768_7series\e[0m\tIntel HD3000 (1366x768) 7series\n"
//                           "\t\e[1mHD3000_1600x900\e[0m\t\tIntel HD3000 (1600x900)\n"
//                           "\t\e[1mHD3000_1600x900_7series\e[0m\tIntel HD3000 (1600x900) 7series\n"
//                           "\t\e[1mHD4000_1366x768\e[0m\t\tIntel HD4000 (1366x768)\n"
//                           "\t\e[1mHD4000_1366x768_6series\e[0m\tIntel HD4000 (1366x768) 6series\n"
//                           "\t\e[1mHD4000_1600x900\e[0m\t\tIntel HD4000 (1600x900)\n"
//                           "\t\e[1mHD4000_1600x900_6series\e[0m\tIntel HD4000 (1600x900) 6series\n"
//                           "\t\e[1mHD4600_4400_4200\e[0m\tIntel HD4600, 4400, 4200\n"
//                           "\t\e[1mHD5000_5100_5200\e[0m\tIntel HD5000, 5100, 5200\n"
//                           "\t\e[1mHD5600\e[0m\t\tIntel HD5600\n"
//                           "\t\e[1mHD5300_5500_6000\e[0m\tIntel HD5300, 5500, 6000\n"
//                           "\t\e[1mHD5600\e[0m\t\t\t\tIntel HD5600\n"
//                           "\e[4mNOTE:\e[0m There are \e[1mtwo \e[4mDropTables\e[0m in the config.plist file. Use the alternate if needed with DropOEM and Generate equals true. (From my experience, Broadwell PCs often require the \e[1malternate DropTables\e[0m with \e[1mDropOEM=true\e[0m and \e[1mGenerate=true\e[0m.) It should be furthur noted that choosing a config that needs \e[1mCI Patch\e[0m, \e[1mFakePCIID\e[0m along with \e[1mIntel HD Graphics\e[0m will be installed at /EFI/CLOVER/kexts/Other/.\n"
//            );
//            break;
//        case ETHERNET:
//            printf("\e[4mUSAGE:\e[0m machelper usb|hdd -ethernet <name>\n"
//                           "\e[4mNAME:\e[0m\n"
//                           "\t\e[1mRealtekRTL8100\e[0m\t\tFor Realtek RTL8100, RTL8101E, RTL8102E,\n"
//                           "\t\t\t\tRTL8103E, RTL8105E, RTL8106E, RTL8106EUS,\n"
//                           "\t\t\t\tRTL8401E and RTL8402\n"
//                           "\t\e[1mRealtekRTL8111\e[0m\t\tFor Realtek RTL8111x and RTL8168x\n"
//                           "\t\e[1mIntelMausiEthernet\e[0m\tFor Intel 82578LM, 82578LC, 82578DM,\n"
//                           "\t\t\t\t82578DC, 82579LM, 82579V, I217LM,\n"
//                           "\t\t\t\tI217V, I218LM, I218V, I218LM2,\n"
//                           "\t\t\t\tI218V2 and I218LM3\n"
//                           "\t\e[1mAtherosE2200Ethernet\e[0m\tFor Atheros AR816x, AR817x,\n"
//                           "\t\t\t\tKiller E220x and Killer 2400\n"
//                           "\t\e[1mBroadcomBCM57xx\e[0m\t\tFor Broadcom BCM57xx\n"
//                           "\t\e[1mAppleIntelE1000e\e[0m\tAppleIntel E1000e\n"
//                           "\e[4mNOTE:\e[0m if you have got a kext for your ethernet, please send it to me.\n"
//            );
//            break;
//        case MBR_PATCH:
//            printf("\e[4mUSAGE:\e[0m machelper usb|hdd -mbrPatch <path>\n"
//                           "Here <path> is the destination (folder) where OSInstaller and\n"
//                           "OSInstall.mpkg are located.\n"
//                           "\e[4mNote:\e[0m Two files must be contained in the folder (<path>).\n"
//            );
//            break;
//        case METHOD:
//            printf("\e[4mUSAGE:\e[0m machelper usb|hdd -method <name>\n"
//                           "\e[4mNAME:\e[0m\n"
//                           "\t\e[1mBaseBinaries\e[0m\t\tBaseBinaries Clone (InstallESD) Method (Default)\n"
//                           "\t\e[1mcreateinstallmedia\e[0m\tCreateInstallMedia Method (Creates Recovery Partition)\n"
//                           "\t\e[1mRawImage\e[0m\t\tUse raw image specified by \e[1m-src <source>\e[0m\n"
//                           "\t\t\t\tSupported extensions: img, iso, raw, <volume>\n"
//                           "\e[4mNote:\e[0m RawImage Method is just a shortcut for BaseBinaries Clone Method. "
//                           "That means, RawImage is just an image file containing the files that are extracted using "
//                           "the BaseBinaries Clone Method. So, if you have 'Install OS X <OS X Name>.app', you should "
//                           "follow the BaseBinaries Clone Method, instead of RawImage Method.\n"
//            );
//            break;
//        case OS:
//            printf("\e[4mUSAGE:\e[0m machelper usb|hdd -os <name>\n"
//                           "\e[4mNAME:\e[0m\n"
//                           "\t\e[1mMavericks\e[0m\t\tOS X Mavericks (10.9.x) Image\n"
//                           "\t\e[1mYosemite\e[0m\t\tOS X Yosemite (10.10.x) Image\n"
//                           "\t\e[1mEl Capitan\e[0m\t\tOS X El Capitan (10.11.x) Image [default]\n"
//            );
//            break;
//        case RAW_IMG_SRC:
//            printf("\e[4mUSAGE:\e[0m machelper usb|hdd -method RawImage -src <source>\n"
//                           "Here <source> is the path of the raw image \e[4malong with the kernel file\e[0m.\n"
//            );
//            break;
//        case REPLACE_KERNEL:
//            printf("\e[4mUSAGE:\e[0m machelper usb|hdd -replaceKernel <source>\n"
//                           "Here <source> is the path of the kernel \e[4malong with the kernel file\e[0m.\n"
//            );
//            break;
//
//        case REMOVE_KEXT:
//        case REPLACE_KERNEL_RECOVERY:
//            printf("\e[4mUSAGE:\e[0m machelper recovery -removeKext|-replaceKernel <source>\n"
//                           "Here <source> is the kext file to be installed or replaced.\n"
//            );
//            break;
//        case DEFAULT:
//        default: // DEFAULT
//            printf(
//                           "\e[4mUSAGE:\e[0m machelper <verb> [option] ...\n"
//                           "\e[4mVERB:\e[0m\n"
//                           "\t\e[1musb\e[0m\t\t\tCreate a bootable USB\n"
//                           "\t\e[1mhdd\e[0m\t\t\tConfigure HDD/SSD\n"
//                           "\t\e[1mrecovery\e[0m\t\tRecovery Options for machelper\n";
//            );
//    }
//    return EXIT_SUCCESS;
}