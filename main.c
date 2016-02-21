/**
 * machelper
 * 
 * machelper is a command line tool to
 * create a bootable USB using Mac OS and CLOVER EFI.
 * It works only on Mac/Hackintosh and the bootable USB only
 * supports Intel Laptops and EFI BIOS mode with
 * GPT (GUID Partition Scheme) HDD/SSD.
 * It also helps with post installation bootloader
 * installation and configurations for Intel Laptops.
 *
 * Here I have used my own customized repository for
 * making it stable. You can use your own repository
 * if you want.
 *
 * My repo: https://github.com/MuntashirAkon/Mac-OS-Installation-Helper
 *
 * Examples:
 * - Without any options
 *      machelper usb
 * - With options
 *      machelper usb -config HD4400 -ethernet RTL8100E
 *
 * Known Bugs: none so far
 *
 * Author: Muntashir Al-Islam <muntashir.islam96@gmail.com>
 * Version: 0.3.0beta
 * Date: 21 feb, 2016
 * Copyright: 2016 (c) All rights reserved.
 * License: MIT License
 * Editor: CLion
 */
// Include Essential Header Files
#include<stdio.h>
#include<string.h>
#include<stdlib.h> // system()
#include<unistd.h> // access()

#define TRUE 1
#define FALSE 0

// Function Declarations
int check_arg(char arg_name[]);
int show_help(int class); // Always return 0
int run_installer(char type[]);
int mount_efi(char mount_point[], int interactive);
int extract_dsdt(char destination[], int disassemble);
int kext_admin(char class, char src[]);
int recovery(char class, char src[]);
int show_error(char subject[]); // Always return -1

// Global Variable Declarations
char version[15] = "0.3.0beta";
// char verbs[6][15] = {"usb", "hdd", "mount_efi", "kext", "extract_dsdt", "recovery"}
int total_args = 11;
char arg_names[11][15] = {"-config", "-ethernet", "-help", "-raid", "-excludeOs", "-os", "-method", "-excludeClover", "-replaceKernel", "-mbrPatch", "-src"};
char arg_values[11][100] = {"", "", "", "0", "0", "El Capitan", "BaseBinaries", "0", "", "", ""}; // Index 1 is for -config, 2 for -ethernet, etc
char os_names[3][15] = {"Mavericks", "Yosemite", "El Capitan"}; // Supported Mac OS names
char methods[3][20] = {"BaseBinaries", "createinstallmedia", "RawImage"}; // Installation methods
char ethernets[6][30] = {"RealtekRTL8100", "RealtekRTL8111", "IntelMausiEthernet", "AtherosE2200Ethernet", "BroadcomBCM57xx", "AppleIntelE1000e"};
char configs[12][30] = {"HD3000_1366x768", "HD3000_1366x768_7series", "HD3000_1600x900", "HD3000_1600x900_7series", "HD4000_1366x768", "HD4000_1366x768_6series", "HD4000_1600x900", "HD4000_1600x900_6series", "HD4600_4400_4200", "HD5000_5100_5200", "HD5300_5500_5600_6000", "HD5600"};

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"
int main(int args, char *argv[]){
    char confirm;
    switch (args) {
        case 1:
            show_help(0);
            return 0;
        default:
            if (strcmp(argv[1],"usb") == 0 || strcmp(argv[1],"hdd") == 0) { // usb/hdd
                if (args == 2) {
                    printf("You did not choose to use any config.plist or ethernet. ");
                } else {
                    for (int i=2; i<args; ) {
                        if (check_arg(argv[i]) == TRUE) {
                            if (strcmp(argv[i], arg_names[2]) == 0) { // help
                                show_help(3);
                                return 0;
                            } else if (strcmp(argv[i], arg_names[0]) == 0) { // config
                                if (args > i+1) {
                                    for (int j=0; j<12; j++) {
                                        if (strcmp(configs[j], argv[i+1]) == 0) {
                                            strcpy(arg_values[0], argv[i+1]);
                                        }
                                    }
                                    i+=2;
                                }else{
                                    show_help(1);
                                    return 0;
                                }
                            } else if (strcmp(argv[i], arg_names[1]) == 0) { // ethernet
                                if (args > i+1) {
                                    for (int j=0; j<6; j++) {
                                        if (strcmp(ethernets[j], argv[i+1]) == 0) {
                                            strcpy(arg_values[1], argv[i+1]);
                                        }
                                    }
                                    i+=2;
                                }else{
                                    show_help(2);
                                    return 0;
                                }
                            } else if (strcmp(argv[i], arg_names[5]) == 0) { // os
                                if (args > i+1) {
                                    for (int j=0; j<3; j++) {
                                        if (strcmp(os_names[j], argv[i+1]) == 0) {
                                            strcpy(arg_values[5], argv[i+1]);
                                        }
                                    }
                                    i+=2;
                                }else{
                                    show_help(5);
                                    return 0;
                                }
                            } else if (strcmp(argv[i], arg_names[6]) == 0) { // method
                                if (args > i+1) {
                                    for (int j=0; j<3; j++) {
                                        if (strcmp(methods[j], argv[i+1]) == 0) {
                                            strcpy(arg_values[6], argv[i+1]);
                                        }
                                    }
                                    i+=2;
                                }else{
                                    show_help(6);
                                    return 0;
                                }
                            } else if (strcmp(argv[i], arg_names[8]) == 0) { // replace kernel
                                if (args > i+1) {
                                    strcpy(arg_values[8], argv[i+1]);
                                    i+=2;
                                }else{
                                    show_help(8);
                                    return 0;
                                }
                            } else if (strcmp(argv[i], arg_names[9]) == 0) { // mbr patch
                                if (args > i+1) {
                                    strcpy(arg_values[9], argv[i+1]);
                                    i+=2;
                                }else{
                                    show_help(9);
                                    return 0;
                                }
                            } else if (strcmp(argv[i], arg_names[10]) == 0) { // src (for RawImage method)
                                if (args > i+1) {
                                    strcpy(arg_values[10], argv[i+1]);
                                    i+=2;
                                }else{
                                    show_help(7);
                                    return 0;
                                }
                            } else {
                                if (strcmp(arg_names[3], argv[i]) == 0) { // raid
                                    arg_values[3][0] = '1';
                                }else if (strcmp(arg_names[4], argv[i]) == 0){ // excludeOs
                                    arg_values[4][0] = '1';
                                }else if (strcmp(arg_names[7], argv[i]) == 0){ // excludeClover
                                    arg_values[7][0] = '1';
                                }
                                i++;
                            }
                        }else return show_error("Invalid Argument(s). Add -help to show help.");
                    }
                    // Set method to BaseBinaries if -mbrPatch and/or -replaceKernel is set
                    if (strcmp(arg_values[8], "") != 0 || strcmp(arg_values[9], "") != 0) {
                        strcpy(arg_values[6], methods[0]);
                    }
                    // Overwrite BaseBinaries to createinstallmedia, if -excludeClover is given
                    if (arg_values[7][0] == '1') {
                        strcpy(arg_values[6], methods[1]);
                    }
                    // Print a summery
                    printf("\e[1;4mSummery:\e[0m\n"
                           "Installation    : %s\n"
                           , argv[1]);
                    if (arg_values[7][0] == '1' && argv[1][0] != 'h') { // Exclude config, ethernet, raid, etc for Real Mac
                        printf("Exclude Clover : %s\n", arg_values[7]);
                    }else{ // Opp
                        printf(
                               "Config.plist    : %s\n"
                               "Ethernet\t: %s\n"
                               "Raid\t\t: %s\n"
                        , arg_values[0], arg_values[1], arg_values[3]);
                    }
                    if (argv[1][0] != 'h') { // Show installation info only if usb
                        printf("Exclude OS\t: %s\n", arg_values[4]);
                        if (arg_values[4][0] != '1') { // If choose to install OS X
                            printf("Method\t\t: %s\n", arg_values[6]);
                            if (arg_values[6][0] != 'R'){ // Show mac version, if not RawImage
                                printf("Mac Version\t: %s\n", arg_values[5]);
                            }else{ // if RawImage, show Image path
                                printf("Raw Image Dir   : %s\n", arg_values[10]);
                            }
                            if (arg_values[7][0] != '1') { // If not exclude Clover
                                printf(
                                       "MBR Patch\t: %s\n"
                                       "Kernel Dir\t: %s\n"
                                       , arg_values[8], arg_values[9]);
                                       
                            }
                        }
                    }
                    printf("\n");
                }
                printf("Are you sure? (y/n): ");
                scanf("%c", &confirm);
                if (confirm == 'y') {
                    return run_installer(argv[1]);
                }else return -1;
            } else if (strcmp(argv[1], "mount_efi") == 0) { // Mount EFI
                if (args == 2) { // Default procedure
                    return mount_efi("", 0);
                }else if (args == 3) { // show -mountPoint help | help | interactive way
                    if (strcmp(argv[2], "-help") == 0) return show_help(10);
                    else if (strcmp(argv[2], "-mountPoint") == 0) return show_help(11);
                    else if (strcmp(argv[2], "-i") == 0) return mount_efi("", 1);
                    else return show_help(10);
                }else if (args == 4){ // mount EFI with -mountPoint | interactive | -mountPoint help
                    if (strcmp(argv[2], "-mountPoint") == 0) return mount_efi(argv[3], 0);
                    else if (strcmp(argv[2], "-i") == 0 && strcmp(argv[3], "-mountPoint") == 0) return show_help(11);
                    else return show_help(11);
                }else if (args == 5) {
                    if (strcmp(argv[2], "-i") == 0 && strcmp(argv[3], "-mountPoint") == 0) return mount_efi(argv[4], 1);
                    else return show_help(11);
                }else return show_help(10);
            } else if (strcmp(argv[1], "extract_dsdt") == 0) { // Extract DSDT/SSDTs
                if (args == 3) { // default | help
                    if (strcmp(argv[2], "-help") == 0) return show_help(4);
                    else{
                        if (argv[2][0] == '-'){
                            show_help(4);
                            return -1;
                        }
                        else return extract_dsdt(argv[2], 0);
                    }
                }else if (args == 4){ // also disassemble | extract
                    if (strcmp(argv[2], "-disassemble") == 0) return extract_dsdt(argv[3], 1);
                    else return show_help(4);
                }else return show_help(4);
            } else if (strcmp(argv[1], "kext") == 0) { // Kext Admin
                if (args == 3) { // help | -install/-uninstall help | -rebuildCache | -fixPermission
                    // if (strcmp(argv[2], "-help") == 0) return show_help(12);
                    if (strcmp(argv[2], "-install") == 0) return show_help(13);
                    else if (strcmp(argv[2], "-uninstall") == 0) return show_help(13);
                    else if (strcmp(argv[2], "-rebuildCache") == 0) return kext_admin('r', "");
                    else if (strcmp(argv[2], "-fixPermissions") == 0) return kext_admin('f', "");
                    else return show_help(12);
                }else if (args == 4){ // -install | -uninstall
                    if (strcmp(argv[2], "-install") == 0) return kext_admin('i', argv[3]);
                    else if (strcmp(argv[2], "-uninstall") == 0) return kext_admin('u', argv[3]);
                    else return show_help(12);
                }else show_help(12);
            } else if (strcmp(argv[1], "recovery") == 0) { // Recovery
                if (args == 3) { // help | -removeKext/-replaceKernel help | -copyClover | -listUnsigned | -installOnUSB | -disableHibernation | -fixAppleId
                    // if (strcmp(argv[2], "-help") == 0) return show_help(14);
                    if (strcmp(argv[2], "-removeKext") == 0) return show_help(15);
                    else if (strcmp(argv[2], "-replaceKernel") == 0) return show_help(15);
                    else if (strcmp(argv[2], "-copyClover") == 0) return recovery('c', "");
                    else if (strcmp(argv[2], "-listUnsigned") == 0) return recovery('l', "");
                    else if (strcmp(argv[2], "-installOnUSB") == 0) return recovery('i', "");
                    else if (strcmp(argv[2], "-disableHibernation") == 0) return recovery('d', "");
                    else if (strcmp(argv[2], "-init") == 0) return recovery('m', "");
                    else if (strcmp(argv[2], "-fixAppleId") == 0) return recovery('f', "");
                    else return show_help(14);
                }else if (args == 4){ // -install | -uninstall
                    if (strcmp(argv[2], "-removeKext") == 0) return recovery('u', argv[3]);
                    else if (strcmp(argv[2], "-replaceKernel") == 0) return recovery('r', argv[3]);
                    else return show_help(14);
                }else return show_help(14);
            } else if (strcmp(argv[1], "version") == 0) {
                printf("Version %s\n", version);
                return 0;
            } else return show_help(0);
    }
    return 0;
}
#pragma clang diagnostic pop


// Checks whether the arguments are valid
int check_arg(char arg_name[]){
    for (int i=0; i<total_args; i++) {
        if (strcmp(arg_name,arg_names[i]) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

// Shows help
int show_help(int class){ // class 1 = config, 2 = ethernet, 3 = usb/hdd, 4 = extract dsdt, 5 = os, 6 = method, 7 = Raw Image Src, 8 = replace kernel, 9 = mbr patch, 10 = mount_efi, 11 = mount point, 12 = kext, 13 = install/uninstall kext, 14 = recovery, 15 = remove kext/replace kernel
    if (class == 0) { // default
        printf(
               "\e[1mMac OS Installation Helper\e[0m\n"
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
    }else if (class == 1){ // config
        printf(
               "\e[4mUSAGE:\e[0m machelper usb|hdd -config <name>\n"
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
               "\t\e[1mHD5600\e[0m\t\t\tIntel HD5600\n"
               "\e[4mNOTE:\e[0m There are \e[1mtwo \e[4mDropTables\e[0m in the config.plist file. Use the alternate if needed with DropOEM and Generate equals true. (From my experience, Broadwell PCs often require the \e[1malternate DropTables\e[0m with \e[1mDropOEM=true\e[0m and \e[1mGenerate=true\e[0m.) It should be furthur noted that choosing a config that needs \e[1mCI Patch\e[0m, \e[1mFakePCIID\e[0m along with \e[1mIntel HD Graphics\e[0m will be installed at /EFI/CLOVER/kexts/Other/.\n"
        );
    }else if (class == 2){ // ethernet
        printf(
               "\e[4mUSAGE:\e[0m machelper usb|hdd -ethernet <name>\n"
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
    }else if (class == 3) { // usb/hdd
        printf(
               "\e[4mUSAGE:\e[0m machelper usb|hdd [option] ...\n"
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
    }else if (class == 4){ // Extract DSDT/SSDTs
        printf(
               "\e[4mUSAGE:\e[0m machelper mount_efi [-disassemble|-help] <dest>\n"
               "\t<dest>\t\t\tDestination to save DSDT/SSDTs\n"
               "\t-disassemble\t\tDisassemble DSDT/SSDTs too\n"
               "\t-help\t\t\tShow this help menu\n"
               );
    }else if (class == 5){ // Mac OS Version
        printf(
               "\e[4mUSAGE:\e[0m machelper usb|hdd -os <name>\n"
               "\e[4mNAME:\e[0m\n"
               "\t\e[1mMavericks\e[0m\t\tOS X Mavericks (10.9.x) Image\n"
               "\t\e[1mYosemite\e[0m\t\tOS X Yosemite (10.10.x) Image\n"
               "\t\e[1mEl Capitan\e[0m\t\tOS X El Capitan (10.11.x) Image [default]\n"
        );
    }else if (class == 6){ // method
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
    }else if (class == 7){ // src for RawImage method
        printf(
               "\e[4mUSAGE:\e[0m machelper usb|hdd -method RawImage -src <source>\n"
               "Here <source> is the path of the raw image \e[4malong with the kernel file\e[0m.\n"
               );
    }else if (class == 8){ // replace kernel
        printf(
               "\e[4mUSAGE:\e[0m machelper usb|hdd -replaceKernel <source>\n"
               "Here <source> is the path of the kernel \e[4malong with the kernel file\e[0m.\n"
        );
    }else if (class == 9){ // mbr patch
        printf(
               "\e[4mUSAGE:\e[0m machelper usb|hdd -mbrPatch <path>\n"
               "Here <path> is the destination (folder) where OSInstaller and\n"
               "OSInstall.mpkg are located.\n"
               "\e[4mNote:\e[0m Two files must be contained in the folder (<path>).\n"
        );
    }else if (class == 10){ // mount efi
        printf(
               "\e[4mUSAGE:\e[0m machelper mount_efi [-i|-help] [-mountPoint <path>]\n"
               "\t-help\t\t\tShow this help menu\n"
               "\t-i\t\t\tInteractive way of mounting EFI which allows\n"
               "\t\t\t\tuser to select the EFI partition from a list.\n"
               "\t-mountPoint <path>\tSpecify a mount point to mount the EFI\n"
               "\t\t\t\tpartition of the HDD/SSD.\n"
        );
    }else if (class == 11){ // mount point
        printf(
               "\e[4mUSAGE:\e[0m machelper mount_efi [-i] [-mountPoint <path>]\n"
               "Here <path> is the folder to be used as a mount point to mount the EFI.\n"
               "\e[4mNote:\e[0m You must create that folder.\n"
        );
    }else if (class == 12){ // kext
        printf(
               "\e[4mUSAGE:\e[0m machelper kext <option>\n"
               "\e[4mOPTION:\e[0m\n"
               "\t-help\t\t\tShow this help menu\n"
               "\t-install <source>\tInstall a kext, fix permissions & rebuil\n"
               "\t-uninstall <source>\tUninstall a kext\n"
               "\t-fixPermissions\t\tFix permissions of the Kexts at /S/L/E\n"
               "\t-rebuildCache\t\tRebuild Kernel Cache\n"
        );
    }else if (class == 13){ // mount point
        printf(
               "\e[4mUSAGE:\e[0m machelper kext -install|-uninstall <source>\n"
               "Here <source> is the kext file to be installed at /S/L/E.\n"
        );
    }else if (class == 14){ // recovery
        printf(
               "\e[4mUSAGE:\e[0m machelper recovery <option>\n"
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
    }else if (class == 15){ // installOnUSB/removeKext
        printf(
               "\e[4mUSAGE:\e[0m machelper recovery -installOnUSB|-removeKext <source>\n"
               "Here <source> is the kext file to be installed at /S/L/E.\n"
               );
    }
    return 0;
}

// Run Shell Scripts to create bootable USB/configure HDD
int run_installer(char type[]){ // type = usb|hdd
    char _os_name[15]; // Could be Mavericks, Yosemite, El Capitan
    char _device_name[100] = "install_osx"; // Default USB device name is install_osx
    char _efi_name[] = "ESP"; // EFI partition mount point, default is ESP
    char _identifier[15] = "disk1"; // For USB only, default IDENTIFIER is disk1
    char _choice_file[16] = "choices."; // Choice file for Clover (format: choices.[type].[extension])
    char _clover_file[] = "Clover_v2.3k_r3330";
    char confirm[2] = "y";
    char root[] = "/tmp/helper"; // Temporary directory root
    char cmd[300];
    char installation_type;
    char method;
    if (strcmp(type, "hdd") == 0){
        installation_type = 'h';
    } else{
        installation_type = 'u';
    }
    switch (arg_values[6][0]) { // Set method
        case 'c': // createinstallmedia
            method = 'c';
            break;
        case 'R': // RawImage
            method = 'r';
            break;
        default: // Default: BaseBinaries
            method = 'b';
            break;
    }
    strcat(_choice_file, type); // Add type usb|hdd
    strcat(_choice_file, ".xml"); // Add extension .xml
    strcpy(_os_name, arg_values[5]); // Set _os_name
    
    printf("\e[4mNote:\e[0m Sometimes 'sudo' permission is required.\n");
    
    /* === USB Formatting === */
    // First warn user if trying to do it from single user mode
    // list disks to let user know the identifier of the USB
    if (system("diskutil list") != 0) return show_error("You cannot do this operation in single user mode!");
    // Unmount ESP
    sprintf(cmd, "/Volumes/%s", _efi_name);
    if (access(cmd, F_OK) == 0) {
        sprintf(cmd, "diskutil unmount %s", _efi_name); system(cmd);
    }
    // Skip USB formatting, if type is hdd as well as -excludeOs is given
    if (installation_type == 'u' && arg_values[4][0] != '1') {
        printf("Enter the name of the IDENTIFIER of your chosen USB drive to format, be careful, data you loose shall not be recoverable (e.g. disk1): ");
        scanf("%s", _identifier);
        if (strcmp(_identifier, "disk0") == 0) { // if IDENTIFIER can't be disk0
            return show_error("Installation location can't be HDD/SSD!");
        }
        printf("You selected %s. Are you sure? (y/n): ", _identifier);
        scanf("%s", confirm);
        if (confirm[0] == 'y') {
            // Format USB and create partition
            sprintf(cmd, "diskutil partitionDisk /dev/%s 1 GPT HFS+J \"%s\" R", _identifier, _device_name);
            system(cmd);
            printf("Format successful.\n");
        }else return show_error("Installation Aborted.");
    }else{ // Set _device_name
        printf("Enter the NAME of the partition to install Clover (i.e. 'Macintosh HD'): ");
        scanf("%s", _device_name);
        // Check if the partition is really existed
        // TODO: need to make it better
        sprintf(cmd, "/Volumes/%s", _device_name);
        if (access(cmd, F_OK) != 0) return show_error("The partition name you entered is not available.\nInstallation Aborted.");
        printf("You selected %s. Are you sure? (y/n): ", _device_name);
        scanf("%s", confirm);
        if (confirm[0] != 'y') return show_error("Installation Aborted.");
    }
    
    /* === Clover Installation === */
    // Create root dir
    sprintf(cmd, "mkdir %s", root);
    system(cmd);
    printf("Helper will now downloaded Clover EFI.\n");
    // Download Clover, if not already
    sprintf(cmd, "%s/%s.zip", root, _clover_file);
    if (access(cmd, F_OK) != 0) {
        sprintf(cmd, "curl -L http://downloads.sourceforge.net/project/cloverefiboot/Installer/%s.zip > %s/%s.zip", _clover_file, root, _clover_file);
        system(cmd);
    }
    // Download Choice file, if not already
    sprintf(cmd, "%s/%s", root, _choice_file);
    if (access(cmd, F_OK) != 0) {
        sprintf(cmd, "curl -L https://raw.githubusercontent.com/MuntashirAkon/Mac-OS-Installation-Helper/master/CloverInstallationChoices/%s > %s/%s", _choice_file, root, _choice_file);
        system(cmd);
    }
    printf("Unzipping...\n");
    // Unzip Clover
    sprintf(cmd, "unzip -d %s %s/%s.zip", root, root,_clover_file);
    system(cmd);
    printf("Installing Clover EFI into %s\n", _device_name);
    // Install Clover
    sprintf(cmd, "sudo installer -pkg %s/%s.pkg -target \"/Volumes/%s\" -applyChoiceChangesXML %s/%s", root, _clover_file, _device_name, root, _choice_file);
    if (system(cmd) != 0) return show_error("Clover Installation failed!");
    
    /* === Clover Configuration === */
    printf("Configuring Clover...\nDownloading essential files...\n");
    /* === Download Essential Files === */
    char kext_url[] = "https://github.com/MuntashirAkon/Mac-OS-Installation-Helper/raw/master/CloverKexts";
    // HFSPlus.efi
    sprintf(cmd, "%s/HFSPlus.efi", root);
    if (access(cmd, F_OK) != 0) {
        sprintf(cmd, "curl -L https://github.com/JrCs/CloverGrowerPro/raw/master/Files/HFSPlus/X64/HFSPlus.efi > %s/HFSPlus.efi", root);
        system(cmd);
    }
    // FakeSMC.kext.zip
    sprintf(cmd, "%s/FakeSMC.kext.zip", root);
    if (access(cmd, F_OK) != 0) {
        sprintf(cmd, "curl -L %s/FakeSMC.kext.zip > %s/FakeSMC.kext.zip", kext_url, root);
        system(cmd);
    }
    // VoodooPS2Controller.kext.zip
    sprintf(cmd, "%s/VoodooPS2Controller.kext.zip", root);
    if (access(cmd, F_OK) != 0) {
        sprintf(cmd, "curl -L %s/VoodooPS2Controller.kext.zip > %s/VoodooPS2Controller.kext.zip", kext_url, root);
        system(cmd);
    }
    // GenericUSBXHCI.kext.zip
    sprintf(cmd, "%s/GenericUSBXHCI.kext.zip", root);
    if (access(cmd, F_OK) != 0) {
        sprintf(cmd, "curl -L %s/GenericUSBXHCI.kext.zip > %s/GenericUSBXHCI.kext.zip", kext_url, root);
        system(cmd);
    }
    
    /* === Unzip & Configure === */
    // Delete Everything of /ESP/EFI/CLOVER/kexts/ except Other
    system("rm -R /Volumes/ESP/EFI/CLOVER/kexts/10*");
    char kext_location[100];
    sprintf(kext_location, "/Volumes/%s/EFI/CLOVER/kexts/Other/", _efi_name);
    printf("Unzipping & copying...\n");
    // Unzip & copy files
    sprintf(cmd, "unzip -d %s %s/FakeSMC.kext.zip", kext_location, root);
    system(cmd);
    sprintf(cmd, "unzip -d %s %s/VoodooPS2Controller.kext.zip", kext_location, root);
    system(cmd);
    sprintf(cmd, "unzip -d %s %s/GenericUSBXHCI.kext.zip", kext_location, root);
    system(cmd);
    // Copy HFSPlus.efi
    sprintf(cmd, "cp %s/HFSPlus.efi /Volumes/%s/EFI/CLOVER/drivers64UEFI/", root, _efi_name);
    system(cmd);
    
    /* === Special Tasks === */
    printf("Doing some special business...\n");
    if (arg_values[3][0] == '1') { // RAID Mode
        // HPRAIDInjector.kext.zip
        sprintf(cmd, "%s/HPRAIDInjector.kext.zip", root);
        if (access(cmd, F_OK) != 0) {
            sprintf(cmd, "curl -L %s/HPRAIDInjector.kext.zip > %s/HPRAIDInjector.kext.zip", kext_url, root);
            system(cmd);
        }
        // Unzip & copy
        sprintf(cmd, "unzip -d %s %s/HPRAIDInjector.kext.zip", kext_location, root); system(cmd);
    }
    if (strcmp(arg_values[0], "") != 0) { // Config
        // Download config.plist
        sprintf(cmd, "%s/config.plist", root);
        if (access(cmd,F_OK) != 0) {
            sprintf(cmd, "curl -L https://github.com/MuntashirAkon/Mac-OS-Installation-Helper/raw/master/CloverLaptopConfig/cofig_%s.plist > %s/config.plist", arg_values[0], root);
            system(cmd);
        }
        sprintf("cp %s/config.plist /Volumes/%s/EFI/CLOVER/config.plist", root, _device_name);
        system(cmd);
        // Enable CI in HD4200/4400/4600/5600
        if (strcmp(arg_values[0], "HD4600_4400_4200") == 0 || strcmp(arg_values[0], "HD5600") == 0) {
            // Download FakePCIID, if not already
            sprintf(cmd, "%s/FakePCIID.kext.zip", root);
            if (access(cmd, F_OK) != 0) {
                sprintf(cmd, "curl -L %s/FakePCIID.kext.zip > %s/FakePCIID.kext.zip", kext_url, root);
                system(cmd);
            }
            // Download FakePCIID_Intel_HD_Graphics, if not already
            sprintf(cmd, "%s/FakePCIID_Intel_HD_Graphics.kext.zip", root);
            if (access(cmd, F_OK) != 0) {
                sprintf(cmd, "curl -L %s/FakePCIID_Intel_HD_Graphics.kext.zip > %s/FakePCIID_Intel_HD_Graphics.kext.zip", kext_url, root);
                system(cmd);
            }
            // Unzip & copy
            sprintf(cmd, "unzip -d %s %s/FakePCIID.kext.zip", kext_location, root); system(cmd);
            sprintf(cmd, "unzip -d %s %s/FakePCIID_Intel_HD_Graphics.kext.zip", kext_location, root); system(cmd);
        }
    }
    if (strcmp(arg_values[1], "") != 0) { // Ethernet
        // For BroadcomBCM57xx, FakePCIID and FakePCIID_BCM57XX_as_BCM57765 have to be used
        if (strcmp(arg_values[1], "BroadcomBCM57xx") == 0) {
            // Download FakePCIID, if not already
            sprintf(cmd, "%s/FakePCIID.kext.zip", root);
            if (access(cmd, F_OK) != 0) {
                sprintf(cmd, "curl -L %s/FakePCIID.kext.zip > %s/FakePCIID.kext.zip", kext_url, root);
                system(cmd);
            }
            // Download FakePCIID_BCM57XX_as_BCM57765, if not already
            sprintf(cmd, "%s/FakePCIID_BCM57XX_as_BCM57765.kext.zip", root);
            if (access(cmd, F_OK) != 0) {
                sprintf(cmd, "curl -L %s/FakePCIID_BCM57XX_as_BCM57765.kext.zip > %s/FakePCIID_BCM57XX_as_BCM57765.kext.zip", kext_url, root); system(cmd);
            }
            // Unzip & copy
            sprintf(cmd, "unzip -d %s %s/FakePCIID.kext.zip", kext_location, root);
            system(cmd);
            sprintf(cmd, "unzip -d %s %s/FakePCIID_BCM57XX_as_BCM57765.kext.zip", kext_location, root);
            system(cmd);
        }else{ // Download the exact kext
            sprintf(cmd, "%s/%s.kext.zip", root, arg_values[1]);
            if (access(cmd, F_OK) != 0) { // Download if not already
                sprintf("curl -L %s/%s.kext.zip > %s/%s.kext.zip", kext_url, arg_values[1], root, arg_values[1]); system(cmd);
            }
            // Unzip & copy
            sprintf(cmd, "unzip -d %s %s/%s.kext.zip", kext_location, root, arg_values[1]); system(cmd);
        }
    }
    
    printf("Done.\n");

    /* === Mac OS Installation === */
    // Install Mac OS into USB only if -excludeOs is not given
    if (installation_type == 'u' && arg_values[4][0] == '0') {
        printf("Now Installing %s...\n", _os_name);
        if (method == 'c') {
            /* === createinstallmedia Method === */
            // Results in recovery partition
            // Only applies for Mavericks, Yosemite & El Capitan
            // Works in the native Mac OS too
            sprintf(cmd, "sudo \"/Applications/Install OS X %s.app/Contents/Resources/createinstallmedia\" --volume  \"/Volumes/%s\" --applicationpath \"/Applications/Install OS X %s.app\" --nointeraction", _os_name, _device_name, _os_name);
            if (system(cmd) != 0) return show_error("Installation Failed!");
            // Rename
            sprintf(cmd, "diskutil rename \"Install OS X %s\" \"%s\"", _os_name, _device_name);
            system(cmd);
        }else if (method == 'r'){
            /* === RawImage Method === */
            // Doesn't create a recovery partition
            // Note: probably BaseBinaries method in a raw file with kernel & /Extra folder
            // Supported file types: raw, iso, img
            // Need no version specification
            printf("Restoring Raw Image to the USB...\n");
            if (access(arg_values[10], F_OK) == 0) {
                // Unmount disk first
                sprintf(cmd, "diskutil unmountDisk /dev/%s", _identifier); system(cmd);
                // Restore the Raw Image to the usb
                printf("Note: Press \e[1mctrl+t\e[0m to show status.\n");
                sprintf(cmd, "sudo dd if=\"%s\" of=/dev/r%s bs=1m", arg_values[10], _identifier);
                if (system(cmd) != 0) return show_error("Installation Failed!");
                // Rename
                sprintf(cmd, "diskutil rename \"OS X Base System\" \"%s\"", _device_name); system(cmd);
                // Delete /Exrta folder & /kernel which are for legacy
                sprintf(cmd, "rm -R \"/Volumes/%s/Extra/\" \"Volumes/%s/kernel\"", _device_name, _device_name); system(cmd);
                // Eject the disk
                sprintf(cmd, "diskutil eject /dev/%s", _identifier);
            }else return show_error("Invalid source. Try again.\n");
        }else{ // method = 'b'
            /* === BaseBinaries Clone Method === */
            // Doesn't create a recovery partition
            // Not recommend for laptops
            // Applies for previous versions, too
            // Create a temp dir first
            system("mkdir /tmp/install_esd");
            // Mount InstallESD.dmg in installer package
            sprintf(cmd, "hdiutil attach \"/Applications/Install OS X %s.app/Contents/SharedSupport/InstallESD.dmg\" -readonly -mountpoint /tmp/install_esd -nobrowse", _os_name);
            if (system(cmd) != 0) return show_error("Mounting InstallESD.dmg Failed!");
            // Restore it to install_osx
            sprintf(cmd, "sudo asr restore -source /tmp/install_esd/BaseSystem.dmg -target \"/Volumes/%s/\" -erase -format HFS+ -noprompt", _device_name);
            if (system(cmd) != 0) return show_error("Installation Failed!");
            // Rename
            sprintf(cmd, "diskutil rename \"OS X Base System\" \"%s\"", _device_name); system(cmd);
            // Print something to stop bothering users
            printf("Doing some additional tasks...");
            // Remove Packages symlink
            sprintf(cmd, "rm \"/Volumes/%s/System/Installation/Packages\"", _device_name);
            system(cmd);
            printf("5...");
            // Copy Packages as folder
            sprintf(cmd, "cp -a /tmp/install_esd/Packages \"/Volumes/%s/System/Installation\"", _device_name);
            system(cmd);
            printf("60...");
            // Copy BaseSystem.dmg
            sprintf(cmd, "cp -a /tmp/install_esd/BaseSystem.dmg /tmp/install_esd/BaseSystem.chunklist \"/Volumes/%s\"", _device_name);
            system(cmd);
            printf("100\n");
            // Unmount InstallESD.dmg
            system("hdiutil detach /tmp/install_esd");
            // Remove tmp dir
            system("rmdir /tmp/install_esd");
            // Create kernel
            // Don't waste time for copying kernel if -replaceKernel is given
            if (strcmp(arg_values[8], "") == 0 || (strcmp(arg_values[8], "") != 0 && access(arg_values[8], F_OK) != 0)) {
                // Copy kernel
                // TODO: need to be implemented
            }
            
        }
        if (method == 'b' || method == 'r') { // MBR patch and kernel replace
            /* === Special (for BaseBinaries and/or RawImage) === */
            // MBR patch
            if (strcmp(arg_values[9], "") != 0 && access(arg_values[9], F_OK) == 0){
                printf("Applying MBR patch... ");
                sprintf(cmd, "cp \"%s/OSInstall.mpkg\" \"/Volumes/%s/System/Installation/Packages/OSInstall.mpkg\"", arg_values[9], _device_name);
                system(cmd);
                printf("...\n");
                sprintf(cmd, "cp \"%s/OSInstaller\" \"/Volumes/%s/System/Library/PrivateFrameworks/OSInstaller.framework/Versions/A/OSInstaller\"", arg_values[9], _device_name);
                system(cmd);
            }
            // Replace Kernel
            if (strcmp(arg_values[8], "") != 0 && access(arg_values[8], F_OK) == 0) {
                printf("Replacing kernel...\n");
                // Backup kernel first
                sprintf(cmd, "/Volumes/%s/System/Library/Kernels/kernel", _device_name);
                if (access(cmd, F_OK) == 0) { // Backup kernel if exists
                    sprintf(cmd, "cp \"/Volumes/%s/System/Library/Kernels/kernel\" ~/Desktop/kernel_backup", _device_name); system(cmd);
                }else{ // Create directory
                    sprintf(cmd, "mkdir \"/Volumes/%s/System/Library/Kernels/\"", _device_name); system(cmd);
                }
                // Replace/Add kernel
                sprintf(cmd, "cp \"%s\" \"/Volumes/%s/System/Library/Kernels/kernel\"", arg_values[8], _device_name);
            }
        }
        printf("Done.\n");
    }
    // Final Message
    printf("Your %s is now ready.\n"
           "NB: If you did not use a config.plist, you should configure your own config.plist "
           "based on your hardwares. Otherwise your USB/HDD may not work. "
           "The EFI partition should be mounted at /Volumes/ESP/ (Shown as EFI).\n", type);
    return 0;
}

// Mounts EFI partition
int mount_efi(char mount_point[], int interactive){
    char _identifier[15] = "disk0s1"; // Default IDENTIFIER
    char cmd[300];
    if (interactive) { // Allow user to choose disk in interactive mode
        // First warn user if trying to do it from single user mode
        if (system("diskutil list") != 0) return show_error("You cannot do this operation in single user mode!");
        printf("Enter the IDENTIFIER of the EFI partition to be mounted (e.g: disk0s1): ");
        scanf("%s", _identifier);
    }
    // Mount disk
    if (strcmp(mount_point, "") == 0) sprintf(cmd, "diskutil mount %s", _identifier);
    else{
        sprintf(cmd, "diskutil unmount force %s", _identifier); system(cmd);
        sprintf(cmd, "diskutil mount -mountPoint %s %s", mount_point, _identifier);
    }
    // First warn user if trying to do it from single user mode
    if (system(cmd) != 0) return show_error("You cannot do this operation in single user mode!");
    return 0;
}

// Extract and/or disassemble DSDT/SSDTs
int extract_dsdt(char destination[], int disassemble){
    char cmd[300];
    char root[] = "/tmp/helper"; // tmp dir
    sprintf(cmd, "mkdir %s", root); system(cmd); // Create tmp dir
    sprintf(cmd, "mkdir %s", destination); system(cmd); // Create destination
    printf("Downloading essential files...\n");
    // Download patchmatic.zip
    sprintf(cmd, "%s/patchmatic.zip", root);
    if (access(cmd, F_OK) != 0) {
        sprintf(cmd, "curl -L https://bitbucket.org/RehabMan/os-x-maciasl-patchmatic/downloads/RehabMan-patchmatic-2015-0107.zip > %s/patchmatic.zip", root);
        system(cmd);
    }
    // Extract patchmatic.zip
    sprintf(cmd, "unzip %s/patchmatic.zip -d %s", root, destination); system(cmd);
    // Download iasl.zip
    if (disassemble) {
        sprintf(cmd, "%s/iasl.zip", root); system(cmd);
        if (access(cmd, F_OK) != 0) {
            sprintf(cmd, "curl -L https://bitbucket.org/RehabMan/acpica/downloads/iasl.zip > %s/iasl.zip", root); system(cmd);
        }
        // extract iasl.zip
        sprintf(cmd, "unzip %s/iasl.zip -d %s", root, destination); system(cmd);
    }
    printf("Done.\nExtracting DSDT/SSDTs...\n");
    // Extract ACPI tables
    sprintf(cmd, "cd %s && ./patchmatic -extract && rm patchmatic", destination);
    if (system(cmd) != 0){
        sprintf(cmd, "rm -r %s", destination); system(cmd);
        return show_error("Extraction failed!");
    }
    // Renaming files
    // Not needed, already renamed by patchmatic
    //sprintf(cmd, "cd %s && for file in *; do if ! [ $file = \"iasl\" ]; then mv $file \"$file.aml\"; fi; done;", destination); system(cmd);
    printf("Done.\n");
    // Disassembling files
    if (disassemble) {
        printf("Disassembling files...\n");
        // Create refs.txt
        sprintf(cmd, "echo -e \"External(_SB_.PCI0.PEG0.PEGP.SGPO, MethodObj, 2)\nExternal(_SB_.PCI0.LPCB.H_EC.ECWT, MethodObj, 2)\nExternal(_SB_.PCI0.LPCB.H_EC.ECRD, MethodObj, 1)\nExternal(_GPE.MMTB, MethodObj, 0)\" > %s/refs.txt", destination);
        system(cmd);
        // Run iasl
        sprintf(cmd, "cd %s && ./iasl -da -dl -fe refs.txt *.aml && rm iasl refs.txt *.aml", destination);
        if (system(cmd) != 0) return show_error("Disassemble failed! Mark the pervious line for possible errors.");
        printf("Done.\n");
    }
    printf("DSDT/SSDTs Location: %s\n", destination);
    return 0;
}

// Kext administration
int kext_admin(char class, char src[]){
    // class 'i' = install, 'u' = uninstall, 'f' = fix permission, 'r' = rebuild cache
    // For class 'i'|'u': src is the *source* of the kext
    // And for class 'f': src is the *name* of the kext (this is for internal use only)
    char cmd[300];
    switch (class) {
        case 'i':
            // Install kext
            if (access(src, F_OK) == 0) {
                char kext_name[100];
                int kext_count = 0;
                long int i; // src may be very long
                int j;
                // src length - 1
                i = strlen(src) - 1;
                printf("Installing the kext...\n");
                // Delete the trailing slash first
                if (src[i] == '/') {
                    src[i] = '\0';
                    i--; // Decrease length by 1
                }
                // Grab the kext name
                for ( ; i >= 0;  i--) {
                    if (src[i] == '/') {
                        break;
                    }
                    kext_count++;
                }
                i++;
                for (j = 0; j < kext_count; j++) {
                    kext_name[j] = src[i + j];
                }
                kext_name[j] = '\0';
                
                // Backup kext if exists at /S/L/E
                sprintf(cmd, "if [ -d \"/System/Library/Extensions/%s\" ]; then sudo mv \"/System/Library/Extensions/%s\" ~/Desktop/%s; fi", kext_name, kext_name, kext_name);
                system(cmd);
                // Copy the kext to /S/L/E
                sprintf(cmd, "sudo cp -R \"%s\" \"/System/Library/Extensions/%s\"", src, kext_name);
                if (system(cmd) != 0) return show_error("Installing kext failed!");
                // Fix Permission, Rebuild Cache
                kext_admin('f', kext_name);
            }else return show_error("Inaccessible or invalid source.");
            break;
        case 'u':
            // Uninstall the kext
            if (access(src, F_OK) == 0) {
                printf("Removing the kext...\n");
                // Backup & remove the kext
                sprintf(cmd, "sudo mv \"%s\" ~/Desktop/", src);
                system(cmd);
                // Rebuild Cache
                kext_admin('r', "");
            }else return show_error("Inaccessible or invalid source.");
            break;
        case 'f':
            // Fix permission
            printf("Fixing permissions...\n");
            if (strcmp(src, "") != 0) { // If kext_name is set, only fix permission of the kext
                sprintf(cmd, "sudo chown -R root:wheel /System/Library/Extensions/%s", src);
            }else{ // Else fix permission of the whole /S/L/E
                sprintf(cmd, "sudo chown -R root:wheel /System/Library/Extensions/*");
            }
            if (system(cmd) != 0) return show_error("Fixing permission failed!");
            // Also, rebuid cache
            kext_admin('r', "");
            break;
        case 'r':
            // Rebuild cache
            printf("Rebuilding Cache...\n");
            if (system("sudo touch /S*/L*/Extensions && sudo kextcache -Boot -U /") != 0) return show_error("Rebuilding Kernel Cache failed!");
            printf("Done.\n");
            break;
        default:
            return show_error("Invalid Argument(s).");
    }
    return 0;
}

// Recovery tools
int recovery(char class, char src[]){
    // class 'c' = copyClover, 'u' = remove kext, 'l' = list unsigned, 'i' = installOnUSB, 'r' = replace kernel, 'd' = disable hibernation
    // 'f' = fix Apple Id
    char cmd[300];
    char _identifier[15];
    char location[100];
    char confirm[2]; // Solve wired problem related to scanf()
    switch (class) {
        case 'u':
            // Remove/uninstall kext
            kext_admin('u', src);
            break;
        case 'f':
            // Fix Apple ID
            // Resets network interfaces
            printf("Do you have a working Ethernet/Wifi? (y/n): ");
            scanf("%s", confirm);
            if (confirm[0] == 'y') {
                printf("Reseting network preferences...\n");
                system("cd /Library/Preferences/SystemConfiguration/ && rm NetworkInterfaces.plist preferences.plist");
            }else
                return show_error("You need to use Null Ethernet Network. Visit http://bit.ly/null-ethernet for details.");
            printf("Done.\nDo you want to restart now? (y/n): ");
            scanf("%s", confirm);
            if (confirm[0] == 'y')
                system("sudo shutdown -r now \"Restarting...\"");
            else
                printf("You choose not to restart now.\n");
            break;
        case 'm':
            // Initialize & mount
            // This is needed when booting into single user mode
            if (system("fsck -fy && mount -uw /") != 0) return show_error("Init failed! This usually occurs because of the user being not logged in single user mode.");
        case 'l':
            // List Unsigned kext(s)
            if (system("sudo touch /S*/L*/Extensions && sudo kextcache -Boot -U /") != 0) return show_error("Operation failed!");
            break;
        case 'i':
            // Install 'machelper' on USB
            // First warn user if trying to do it from single user mode
            if (system("diskutil list") != 0) return show_error("You cannot do this operation in single user mode!");
            printf("Enter the \e[1;30mIDENTIFIER\e[0m of the USB partition to install machelper (e.g. disk1s2): ");
            scanf("%s", _identifier);
            // Mount USB partition to usb_helper
            system("mkdir /tmp/usb_helper/");
            sprintf(cmd, "diskutil unmount force %s", _identifier); system(cmd);
            sprintf(cmd, "diskutil mount -mountPoint /tmp/usb_helper/ %s", _identifier); system(cmd);
            // Find machelper location (either on /usr/bin or $DIR)
            if (access("/usr/bin/machelper", F_OK) == 0) strcpy(location,"/usr/bin/machelper");
            else strcpy(location,"machelper");
            // Copy machelper
            printf("Copying machelper...\n");
            if (access("/tmp/usb_helper/usr/bin/", F_OK) == 0) {
                sprintf(cmd, "cp %s /tmp/usb_helper/usr/bin/", location);
                system(cmd);
            }
            // Unmount again
            sprintf(cmd, "diskutil unmount %s", _identifier); system(cmd);
            system("rmdir /tmp/usb_helper");
            break;
        case 'r':
            // Replace Kernel
            if (access(src, F_OK)) {
                // Backup current kernel
                printf("Backing up kernel...\n");
                system("cp /System/Library/Kernels/kernel ~/Desktop/kernel_backup");
                if (system(cmd) == 0) printf("Kernel is backed up at ~/Desktop/\n");;
                // Replace Kext
                printf("Replacing kernel...\n");
                sprintf(cmd, "sudo cp %s /System/Library/Kernels/kernel", src);
                if (system(cmd) != 0) return show_error("Replacing kernel failed!");
            }else return show_error("Unresolved source.");
            break;
        case 'd':
            // Disable Hibernation
            printf("Disabling Hibernation...\n");
            if(system("sudo pmset -a hibernatemode 0 && sudo rm /var/vm/sleepimage && sudo mkdir /var/vm/sleepimage") != 0) return show_error("Disabling hibernation failed!");
            break;
        case 'c':
            // Copy Clover from USB to HDD/SSD
            // First warn user if trying to do it from single user mode
            if (system("diskutil list") != 0) return show_error("You cannot do this operation in single user mode!");
            printf("\e[4mBe careful with this options\e[0m, as choosing wrong value(s) may cause important data to be wiped not knowingly.\n");
            // Get the IDENTIFIER of the USB
            printf("Enter the \e[1;30mIDENTIFIER\e[0m of the USB device (e.g. disk1): ");
            scanf("%s", _identifier);
            // Append 's1' with it
            strcat(_identifier, "s1");
            // Mount EFI of the USB
            sprintf(cmd, "diskutil unmount force %s", _identifier); system(cmd);
            system("mkdir /tmp/efi_usb");
            sprintf(cmd, "diskutil mount -mountPoint /tmp/efi_usb %s", _identifier); system(cmd);
            // Get the IDENTIFIER of the HDD
            printf("Enter the \e[1;30mIDENTIFIER\e[0m of the HDD/SSD (e.g. disk0): ");
            scanf("%s", _identifier);
            // Append 's1' with it
            strcat(_identifier, "s1");
            // Mount EFI of the HDD/SSD
            sprintf(cmd, "diskutil unmount force %s", _identifier); system(cmd);
            system("mkdir /tmp/efi_hdd");
            sprintf(cmd, "diskutil mount -mountPoint /tmp/efi_hdd %s", _identifier); system(cmd);
            // Copy Clover
            printf("Copying Clover...\n");
            if(access("/tmp/efi_hdd/EFI/CLOVER", F_OK) == 0){
                //  Warn user if /EFI/CLOVER already exists
                printf("It looks like Clover is already at it's right place. Do you really want to continue? (y/n): ");
                scanf("%s", location);
                // Discard copying, if user doesn't want to copy
                if(location[0] != 'y') return -1;
                system("mkdir /tmp/efi_hdd/EFI/");
            }
            if(system("cp -a /tmp/efi_usb/EFI/ /tmp/efi_hdd/EFI/") != 0) return show_error("Copying CLOVER failed!");
            // Unmount EFIs
            system("diskutil unmount /tmp/efi_usb && diskutil unmount /tmp/efi_hdd && rmdir /tmp/efi_usb /tmp/efi_hdd");
            break;
        default:
            return show_error("Invalid Argument(s).");
    }
    printf("Done.\n");
    return 0;
}

// Generate error message: something failed!
int show_error(char subject[]){
    printf("machelper: %s\n", subject);
    return -1;
}