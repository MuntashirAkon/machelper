/*
 * help.h
 *
 * Shows help
 *
 * Author: Muntashir Al-Islam <muntashir.islam96@gmail.com>
 * Date: 22 feb, 2016
 * Copyright: 2016 (c) All rights reserved.
 * License: MIT License
 */
#include "constants.h"

#ifndef MACHELPER_HELP_H
#define MACHELPER_HELP_H

int show_help(unsigned int id); // Always return EXIT_SUCCESS

int show_help(unsigned int id){
    switch (id) {
        case USB:
        case HDD:
            printf("\e[4mUSAGE:\e[0m machelper usb|hdd [option] ...\n"
                           "\e[4mOPTION:\e[0m\n"
                           "\t-config <name>\t\tChoose a laptop config.plist\n"
                           "\t-ethernet <name>\tChoose a ethernet kext\n"
                           "\t-help\t\t\tShow this help menu\n"
                           "\t-raid\t\t\tIf SATA is in RAID mode\n"
                           "\t-excludeOs\t\tExclude Mac OS installation (for USB only)\n"
                           "\t-os <name>\t\tChoose an OS (default: El Capitan)\n"
                           "\t-method <name>\t\tChoose a method (default: BaseBinaries Clone)\n"
                           "\t-src <source>\t\t(Only for RawImage Method) Image Source\n"
                           "\t-mbrPatch <path>\tPath to the folder where OSInstaller and\n"
                           "\t\t\t\tOSInstall.mpkg located\n"
                           "\t-replaceKernel <source>\tSource of the kernel to be replaced\n"
                           "\t-excludeClover\t\t(For real Mac) Exclude Clover Installation\n"
                           "\e[4mNote:\e[0m Keep the <name> blank to show help.\n"
            );
            break;
        case MOUNT_EFI:
            printf("\e[4mUSAGE:\e[0m machelper mount_efi [-i|-help] [-mountPoint <path>]\n"
                           "\t-help\t\t\tShow this help menu\n"
                           "\t-i\t\t\tInteractive way of mounting EFI which allows\n"
                           "\t\t\t\tuser to select the EFI partition from a list.\n"
                           "\t-mountPoint <path>\tSpecify a mount point to mount the EFI\n"
                           "\t\t\t\tpartition of the HDD/SSD.\n"
            );
            break;
        case KEXT:
            printf("\e[4mUSAGE:\e[0m machelper kext <option>\n"
                           "\e[4mOPTION:\e[0m\n"
                           "\t-help\t\t\tShow this help menu\n"
                           "\t-install <source>\tInstall a kext, fix permissions & rebuil\n"
                           "\t-uninstall <source>\tUninstall a kext\n"
                           "\t-fixPermissions\t\tFix permissions of the Kexts at /S/L/E\n"
                           "\t-rebuildCache\t\tRebuild Kernel Cache\n"
            );
            break;
        case EXTRACT_DSDT:
            printf("\e[4mUSAGE:\e[0m machelper mount_efi [-disassemble|-help] <dest>\n"
                           "\t<dest>\t\t\tDestination to save DSDT/SSDTs\n"
                           "\t-disassemble\t\tDisassemble DSDT/SSDTs too\n"
                           "\t-help\t\t\tShow this help menu\n"
            );
            break;
        case RECOVERY:
            printf("\e[4mUSAGE:\e[0m machelper recovery <option>\n"
                           "\e[4mOPTION:\e[0m\n"
                           "\t-copyClover\t\tCopy Clover EFI from USB to HDD/SSD\n"
                           "\t-disableHibernation\tDisable Hibernation to fix sleep problems\n"
                           "\t-fixAppleId\t\tReset network preferences to fix Apple ID\n"
                           "\t-help\t\t\tShow this help menu\n"
                           "\t-init\t\t\tChecks and mounts root when booting with \n"
                           "\t\t\t\tSingle User Mode. This is essentially a shortcut\n"
                           "\t\t\t\tfor `fsck -y` and `mount -uw /`\n"
                           "\t-installOnUSB\t\tInstall 'machelper' on USB (only for \n"
                           "\t\t\t\tBaseBinaries Clone Method)\n"
                           "\t-listUnsigned\t\tList unsigned kext(s)\n"
                           "\t-removeKext <source>\tUninstall/Remove a kext (backup to ~/Desktop)\n"
                           "\t-replaceKernel <source>\tReplace Kernel (backup to ~/Desktop)\n"
            );
            break;
        case CONFIG:
            printf("\e[4mUSAGE:\e[0m machelper usb|hdd -config <name>\n"
                           "\e[4mNAME:\e[0m\n"
                           "\t\e[1mHD3000_1366x768\e[0m\t\tIntel HD3000 (1366x768)\n"
                           "\t\e[1mHD3000_1366x768_7series\e[0m\tIntel HD3000 (1366x768) 7series\n"
                           "\t\e[1mHD3000_1600x900\e[0m\t\tIntel HD3000 (1600x900)\n"
                           "\t\e[1mHD3000_1600x900_7series\e[0m\tIntel HD3000 (1600x900) 7series\n"
                           "\t\e[1mHD4000_1366x768\e[0m\t\tIntel HD4000 (1366x768)\n"
                           "\t\e[1mHD4000_1366x768_6series\e[0m\tIntel HD4000 (1366x768) 6series\n"
                           "\t\e[1mHD4000_1600x900\e[0m\t\tIntel HD4000 (1600x900)\n"
                           "\t\e[1mHD4000_1600x900_6series\e[0m\tIntel HD4000 (1600x900) 6series\n"
                           "\t\e[1mHD4600_4400_4200\e[0m\tIntel HD4600, 4400, 4200\n"
                           "\t\e[1mHD5000_5100_5200\e[0m\tIntel HD5000, 5100, 5200\n"
                           "\t\e[1mHD5600\e[0m\t\tIntel HD5600\n"
                           "\t\e[1mHD5300_5500_6000\e[0m\tIntel HD5300, 5500, 6000\n"
                           "\t\e[1mHD5600\e[0m\t\t\t\tIntel HD5600\n"
                           "\e[4mNOTE:\e[0m There are \e[1mtwo \e[4mDropTables\e[0m in the config.plist file. Use the alternate if needed with DropOEM and Generate equals true. (From my experience, Broadwell PCs often require the \e[1malternate DropTables\e[0m with \e[1mDropOEM=true\e[0m and \e[1mGenerate=true\e[0m.) It should be furthur noted that choosing a config that needs \e[1mCI Patch\e[0m, \e[1mFakePCIID\e[0m along with \e[1mIntel HD Graphics\e[0m will be installed at /EFI/CLOVER/kexts/Other/.\n"
            );
            break;
        case ETHERNET:
            printf("\e[4mUSAGE:\e[0m machelper usb|hdd -ethernet <name>\n"
                           "\e[4mNAME:\e[0m\n"
                           "\t\e[1mRealtekRTL8100\e[0m\t\tFor Realtek RTL8100, RTL8101E, RTL8102E,\n"
                           "\t\t\t\tRTL8103E, RTL8105E, RTL8106E, RTL8106EUS,\n"
                           "\t\t\t\tRTL8401E and RTL8402\n"
                           "\t\e[1mRealtekRTL8111\e[0m\t\tFor Realtek RTL8111x and RTL8168x\n"
                           "\t\e[1mIntelMausiEthernet\e[0m\tFor Intel 82578LM, 82578LC, 82578DM,\n"
                           "\t\t\t\t82578DC, 82579LM, 82579V, I217LM,\n"
                           "\t\t\t\tI217V, I218LM, I218V, I218LM2,\n"
                           "\t\t\t\tI218V2 and I218LM3\n"
                           "\t\e[1mAtherosE2200Ethernet\e[0m\tFor Atheros AR816x, AR817x,\n"
                           "\t\t\t\tKiller E220x and Killer 2400\n"
                           "\t\e[1mBroadcomBCM57xx\e[0m\t\tFor Broadcom BCM57xx\n"
                           "\t\e[1mAppleIntelE1000e\e[0m\tAppleIntel E1000e\n"
                           "\e[4mNOTE:\e[0m if you have got a kext for your ethernet, please send it to me.\n"
            );
            break;
        case MBR_PATCH:
            printf("\e[4mUSAGE:\e[0m machelper usb|hdd -mbrPatch <path>\n"
                           "Here <path> is the destination (folder) where OSInstaller and\n"
                           "OSInstall.mpkg are located.\n"
                           "\e[4mNote:\e[0m Two files must be contained in the folder (<path>).\n"
            );
            break;
        case METHOD:
            printf("\e[4mUSAGE:\e[0m machelper usb|hdd -method <name>\n"
                           "\e[4mNAME:\e[0m\n"
                           "\t\e[1mBaseBinaries\e[0m\t\tBaseBinaries Clone (InstallESD) Method (Default)\n"
                           "\t\e[1mcreateinstallmedia\e[0m\tCreateInstallMedia Method (Creates Recovery Partition)\n"
                           "\t\e[1mRawImage\e[0m\t\tUse raw image specified by \e[1m-src <source>\e[0m\n"
                           "\t\t\t\tSupported extensions: img, iso, raw, <volume>\n"
                           "\e[4mNote:\e[0m RawImage Method is just a shortcut for BaseBinaries Clone Method. "
                           "That means, RawImage is just an image file containing the files that are extracted using "
                           "the BaseBinaries Clone Method. So, if you have 'Install OS X <OS X Name>.app', you should "
                           "follow the BaseBinaries Clone Method, instead of RawImage Method.\n"
            );
            break;
        case OS:
            printf("\e[4mUSAGE:\e[0m machelper usb|hdd -os <name>\n"
                           "\e[4mNAME:\e[0m\n"
                           "\t\e[1mMavericks\e[0m\t\tOS X Mavericks (10.9.x) Image\n"
                           "\t\e[1mYosemite\e[0m\t\tOS X Yosemite (10.10.x) Image\n"
                           "\t\e[1mEl Capitan\e[0m\t\tOS X El Capitan (10.11.x) Image [default]\n"
            );
            break;
        case RAW_IMG_SRC:
            printf("\e[4mUSAGE:\e[0m machelper usb|hdd -method RawImage -src <source>\n"
                           "Here <source> is the path of the raw image \e[4malong with the kernel file\e[0m.\n"
            );
            break;
        case REPLACE_KERNEL:
            printf("\e[4mUSAGE:\e[0m machelper usb|hdd -replaceKernel <source>\n"
                           "Here <source> is the path of the kernel \e[4malong with the kernel file\e[0m.\n"
            );
            break;
        case MOUNT_POINT:
            printf("\e[4mUSAGE:\e[0m machelper mount_efi [-i] [-mountPoint <path>]\n"
                           "Here <path> is the folder to be used as a mount point to mount the EFI.\n"
                           "\e[4mNote:\e[0m You must create that folder.\n"
            );
            break;
        case INSTALL_KEXT:
        case UNINSTALL_KEXT:
            printf("\e[4mUSAGE:\e[0m machelper kext -install|-uninstall <source>\n"
                           "Here <source> is the kext file to be installed at /S/L/E.\n"
            );
            break;
        case REMOVE_KEXT:
        case REPLACE_KERNEL_RECOVERY:
            printf("\e[4mUSAGE:\e[0m machelper recovery -removeKext|-replaceKernel <source>\n"
                           "Here <source> is the kext file to be installed or replaced.\n"
            );
            break;
        case DEFAULT:
        default: // DEFAULT
            printf("\e[1mMac OS Installation Helper\e[0m\n"
                           "2016 (c) Muntashir Akon\n\n"
                           "\e[4mUSAGE:\e[0m machelper <verb> [option] ...\n"
                           "\e[4mVERB:\e[0m\n"
                           "\t\e[1musb\e[0m\t\t\tCreate a bootable USB\n"
                           "\t\e[1mhdd\e[0m\t\t\tConfigure HDD/SSD\n"
                           "\t\e[1mmount_efi\e[0m\t\tMount EFI Partition\n"
                           "\t\e[1mkext\e[0m\t\t\tkext (Kernel Extension) Management\n"
                           "\t\e[1mextract_dsdt\e[0m\t\tExtracts DSDT/SSDTs\n"
                           "\t\e[1mrecovery\e[0m\t\tRecovery Options for machelper\n"
                           "\t\e[1mversion\e[0m\t\t\tShow the current machelper version\n"
                           "\e[4mNote:\e[0m Type -help after each <verb> to show help.\n"
            );
    }
    return EXIT_SUCCESS;
}

#endif //MACHELPER_HELP_H
