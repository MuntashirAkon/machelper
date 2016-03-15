/*
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
 * Version: 1.0.0
 * Date: 21 feb, 2016
 * Copyright: 2016 (c) All rights reserved.
 * License: MIT License
 * Editor: CLion 1.2
 */
// Include Essential Header Files
#include <stdio.h>
#include <string.h>
#include <stdlib.h>  // system()
#include <unistd.h>  // access()
#include "Includes/help.h"
// #include "Includes/constants.h"

// Boolean
#define TRUE 1
#define FALSE 0

// argv[1] is always the verb
#define VERB argv[1]
// First argument/option, for readability's sake
#define ARG_ONE argv[2]

// Exit code for `command not found`
// #define CMD_404 127


// Function Declarations
int check_arg(char arg_name[]);
int run_installer(char type[]);
int mount_efi(char mount_point[], int interactive);
int extract_dsdt(char destination[], int disassemble);
int kext_admin(unsigned int id, char src[]);
int recovery(unsigned int id, char src[]);
int show_error(char subject[]); // Always return FAILURE

// Global Variable Declarations
char version[15] = "1.0.0";
// char verbs[7][15] = {"usb", "hdd", "mount_efi", "kext", "extract_dsdt", "recovery", "version"}

/*
 * Options for USB|HDD
 * option[0] = argument names for USB|HDD
 * option[1] = value of the argument of the same index
 * e.g. if arg is option[0][0], value is option[1][0]
 */
char options[2][11][50] = {
        {"-config", "-ethernet", "-help", "-raid", "-excludeOs", "-os", "-method", "-excludeClover", "-replaceKernel", "-mbrPatch", "-src"},
        {"", "", "", "0", "0", "El Capitan", "BaseBinaries", "0", "", "", ""}
};
/*
 * Currently Supported OS X names
 * The OS X names other than these are supported
 * But I simply doesn't provide any support for them
 * If you like to support them, just add them here
 * Note: if you add less than 10.9, createinstallmedia method is not supported
 */
char os_names[3][15] = {"Mavericks", "Yosemite", "El Capitan"};
/*
 * Installation methods
 * Currently three methods supported:
 * 1. BaseBinaries Clone Method: The oldest way
 * 2. createinstallmedia Method: The newest and recommended way
 * 3. Raw Image Method: The unrecommended way, which may be called vanilla installation method
 */
char methods[3][20] = {"BaseBinaries", "createinstallmedia", "RawImage"};
/*
 * Ethernet kext
 * Supported list of ethernet kexts
 * These kexts are located at my repo.
 */
char ethernets[6][30] = {"RealtekRTL8100", "RealtekRTL8111", "IntelMausiEthernet", "AtherosE2200Ethernet", "BroadcomBCM57xx", "AppleIntelE1000e"};
/*
 * Clover config
 *
 * These configurations are for Intel HD Graphics.
 * They not only replace the config.plist, but also
 * apply CI patch for some Graphics
 *
 * All the kexts can be found in my repo.
 */
char configs[12][30] = {"HD3000_1366x768", "HD3000_1366x768_7series", "HD3000_1600x900", "HD3000_1600x900_7series", "HD4000_1366x768", "HD4000_1366x768_6series", "HD4000_1600x900", "HD4000_1600x900_6series", "HD4600_4400_4200", "HD5000_5100_5200", "HD5300_5500_6000", "HD5600"};

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"
int main(int args, char *argv[]){
    char confirm;
    int i, j;
    switch (args) {
        case 1:
            return show_help(DEFAULT);
        default:
            if (strcmp(VERB,"usb") == 0 || strcmp(VERB,"hdd") == 0) { // usb|hdd
                if (args == 2) {
                    printf("You did not choose to use any config.plist or ethernet. ");
                } else {
                    for (i=2; i<args; ) {
                        if (check_arg(argv[i]) == TRUE) {
                            if (strcmp(argv[i], options[0][2]) == 0) { // help
                                return show_help(USB); // Also HDD
                            } else if (strcmp(argv[i], options[0][0]) == 0) { // config
                                if (args > i+1) {
                                    for (j=0; j<(sizeof(configs)/sizeof(int)); j++) {
                                        if (strcmp(configs[j], argv[i+1]) == 0) {
                                            strcpy(options[1][0], argv[i+1]);
                                        }
                                    }
                                    i+=2;
                                }else{
                                    return show_help(CONFIG);
                                }
                            } else if (strcmp(argv[i], options[0][1]) == 0) { // ethernet
                                if (args > i+1) {
                                    for (j=0; j<(sizeof(ethernets)/sizeof(int)); j++) {
                                        if (strcmp(ethernets[j], argv[i+1]) == 0) {
                                            strcpy(options[1][1], argv[i+1]);
                                        }
                                    }
                                    i+=2;
                                }else{
                                    show_help(ETHERNET);
                                }
                            } else if (strcmp(argv[i], options[0][5]) == 0) { // os
                                if (args > i+1) {
                                    for (j=0; j<(sizeof(os_names)/sizeof(int)); j++) {
                                        if (strcmp(os_names[j], argv[i+1]) == 0) {
                                            strcpy(options[1][5], argv[i+1]);
                                        }
                                    }
                                    i+=2;
                                }else{
                                    return show_help(OS);
                                }
                            } else if (strcmp(argv[i], options[0][6]) == 0) { // method
                                if (args > i+1) {
                                    for (j=0; j<(sizeof(methods)/sizeof(int)); j++) {
                                        if (strcmp(methods[j], argv[i+1]) == 0) {
                                            strcpy(options[1][6], argv[i+1]);
                                        }
                                    }
                                    i+=2;
                                }else{
                                    return show_help(METHOD);
                                }
                            } else if (strcmp(argv[i], options[0][8]) == 0) { // replace kernel
                                if (args > i+1) {
                                    strcpy(options[1][8], argv[i+1]);
                                    i+=2;
                                }else{
                                    return show_help(REPLACE_KERNEL);
                                }
                            } else if (strcmp(argv[i], options[0][9]) == 0) { // mbr patch
                                if (args > i+1) {
                                    strcpy(options[1][9], argv[i+1]);
                                    i+=2;
                                }else{
                                    return show_help(MBR_PATCH);
                                }
                            } else if (strcmp(argv[i], options[0][10]) == 0) { // src (for RawImage method)
                                if (args > i+1) {
                                    strcpy(options[1][10], argv[i+1]);
                                    i+=2;
                                }else{
                                    return show_help(RAW_IMG_SRC);
                                }
                            } else {
                                if (strcmp(options[0][3], argv[i]) == 0) { // raid
                                    options[1][3][0] = '1';
                                }else if (strcmp(options[0][4], argv[i]) == 0){ // excludeOs
                                    options[1][4][0] = '1';
                                }else if (strcmp(options[0][7], argv[i]) == 0){ // excludeClover
                                    options[1][7][0] = '1';
                                }
                                i++;
                            }
                        }else return show_error("Invalid Argument(s). Add -help to show help.");
                    }
                    // Set method to BaseBinaries if -mbrPatch and/or -replaceKernel is set
                    if (strcmp(options[1][8], EMPTY) != 0 || strcmp(options[1][9], EMPTY) != 0) {
                        strcpy(options[1][6], methods[0]);
                    }
                    // Overwrite BaseBinaries to createinstallmedia, if -excludeClover is given
                    if (options[1][7][0] == '1') {
                        strcpy(options[1][6], methods[1]);
                    }
                    // Print a summery
                    printf("\e[1;4mSummery:\e[0m\n"
                           "Installation    : %s\n"
                           , argv[1]);
                    if (options[1][7][0] == '1' && argv[1][0] != 'h') { // Exclude config, ethernet, raid, etc for Real Mac
                        printf("Exclude Clover : %s\n", options[1][7]);
                    }else{ // Opp
                        printf(
                               "Config.plist    : %s\n"
                               "Ethernet\t: %s\n"
                               "Raid\t\t: %s\n"
                        , options[1][0], options[1][1], options[1][3]);
                    }
                    if (argv[1][0] != 'h') { // Show installation info only if usb
                        printf("Exclude OS\t: %s\n", options[1][4]);
                        if (options[1][4][0] != '1') { // If choose to install OS X
                            printf("Method\t\t: %s\n", options[1][6]);
                            if (options[1][6][0] != 'R'){ // Show mac version, if not RawImage
                                printf("Mac Version\t: %s\n", options[1][5]);
                            }else{ // if RawImage, show Image path
                                printf("Raw Image Dir   : %s\n", options[1][10]);
                            }
                            if (options[1][7][0] != '1') { // If not exclude Clover
                                printf(
                                       "MBR Patch\t: %s\n"
                                       "Kernel Dir\t: %s\n"
                                       , options[1][8], options[1][9]);
                                       
                            }
                        }
                    }
                    printf("\n");
                }
                printf("Are you sure? (y/n): ");
                scanf("%c", &confirm);
                if (confirm == 'y') {
                    return run_installer(VERB);
                }else return show_error("Installation Aborted.");
            } else if (strcmp(VERB, "mount_efi") == 0) { // Mount EFI
                if (args == 2) { // Default procedure
                    return mount_efi(EMPTY, FALSE);
                }else if (args == 3) { // show -mountPoint help | help | interactive way
                    if (strcmp(ARG_ONE, "-help") == 0) return show_help(MOUNT_EFI);
                    else if (strcmp(ARG_ONE, "-mountPoint") == 0) return show_help(MOUNT_POINT);
                    else if (strcmp(ARG_ONE, "-i") == 0) return mount_efi(EMPTY, TRUE);
                    else return show_help(MOUNT_EFI);
                }else if (args == 4){ // mount EFI with -mountPoint | interactive | -mountPoint help
                    if (strcmp(ARG_ONE, "-mountPoint") == 0) return mount_efi(argv[3], FALSE);
                    else if (strcmp(ARG_ONE, "-i") == 0 && strcmp(argv[3], "-mountPoint") == 0) return show_help(MOUNT_POINT);
                    else return show_help(MOUNT_POINT);
                }else if (args == 5) {
                    if (strcmp(ARG_ONE, "-i") == 0 && strcmp(argv[3], "-mountPoint") == 0) return mount_efi(argv[4], TRUE);
                    else return show_help(MOUNT_POINT);
                }else return show_help(MOUNT_EFI);
            } else if (strcmp(VERB, "extract_dsdt") == 0) { // Extract DSDT/SSDTs
                if (args == 3) { // default | help
                    if (strcmp(ARG_ONE, "-help") == 0) return show_help(EXTRACT_DSDT);
                    else{
                        if (ARG_ONE[0] == '-'){
                            show_help(EXTRACT_DSDT);
                            return EXIT_FAILURE;
                        }else return extract_dsdt(ARG_ONE, FALSE);
                    }
                }else if (args == 4){ // also disassemble | extract
                    if (strcmp(ARG_ONE, "-disassemble") == 0) return extract_dsdt(argv[3], TRUE);
                    else return show_help(EXTRACT_DSDT);
                }else return show_help(EXTRACT_DSDT);
            } else if (strcmp(VERB, "kext") == 0) { // Kext Admin
                if (args == 3) { // help | -install/-uninstall help | -rebuildCache | -fixPermission
                    if (strcmp(ARG_ONE, "-help") == 0) return show_help(KEXT);
                    else if (strcmp(ARG_ONE, "-install") == 0) return show_help(INSTALL_KEXT);
                    else if (strcmp(ARG_ONE, "-uninstall") == 0) return show_help(UNINSTALL_KEXT);
                    else if (strcmp(ARG_ONE, "-rebuildCache") == 0) return kext_admin(REBUILD_CACHE, EMPTY);
                    else if (strcmp(ARG_ONE, "-fixPermissions") == 0) return kext_admin(FIX_PERMISSIONS, EMPTY);
                    else return show_help(KEXT);
                }else if (args == 4){ // -install | -uninstall
                    if (strcmp(ARG_ONE, "-install") == 0) return kext_admin(INSTALL_KEXT, argv[3]);
                    else if (strcmp(ARG_ONE, "-uninstall") == 0) return kext_admin(UNINSTALL_KEXT, argv[3]);
                    else return show_help(KEXT);
                }else show_help(KEXT);
            } else if (strcmp(VERB, "recovery") == 0) { // Recovery
                if (args == 3) { // help | -removeKext/-replaceKernel help | -copyClover | -listUnsigned | -installOnUSB | -disableHibernation | -fixAppleId
                    if (strcmp(ARG_ONE, "-help") == 0) return show_help(RECOVERY);
                    else if (strcmp(ARG_ONE, "-removeKext") == 0) return show_help(REMOVE_KEXT);
                    else if (strcmp(ARG_ONE, "-replaceKernel") == 0) return show_help(REPLACE_KERNEL_RECOVERY);
                    else if (strcmp(ARG_ONE, "-copyClover") == 0) return recovery(COPY_CLOVER, EMPTY);
                    else if (strcmp(ARG_ONE, "-listUnsigned") == 0) return recovery(LIST_UNSIGNED, EMPTY);
                    else if (strcmp(ARG_ONE, "-installOnUSB") == 0) return recovery(INSTALL_ON_USB, EMPTY);
                    else if (strcmp(ARG_ONE, "-disableHibernation") == 0) return recovery(DISABLE_HIBERNATION, EMPTY);
                    else if (strcmp(ARG_ONE, "-init") == 0) return recovery(INIT, EMPTY);
                    else if (strcmp(ARG_ONE, "-fixAppleId") == 0) return recovery(FIX_APPLE_ID, EMPTY);
                    else return show_help(RECOVERY);
                }else if (args == 4){ // -install | -uninstall
                    if (strcmp(ARG_ONE, "-removeKext") == 0) return recovery(REMOVE_KEXT, argv[3]);
                    else if (strcmp(ARG_ONE, "-replaceKernel") == 0) return recovery(REPLACE_KERNEL_RECOVERY, argv[3]);
                    else return show_help(RECOVERY);
                }else return show_help(RECOVERY);
            } else if (strcmp(VERB, "version") == 0) { // Version
                printf("Version %s\n", version);
                return EXIT_SUCCESS;
            } else return show_help(DEFAULT);
    }
    return EXIT_SUCCESS;
}
#pragma clang diagnostic pop


// Checks whether the argument is valid
int check_arg(char arg_name[]){
    int i;
    for (i=0; i<(sizeof(options[0])/sizeof(int)); i++) {
        if (strcmp(arg_name, options[0][i]) == 0) return TRUE;
    }
    return FALSE;
}

/// Create bootable USB/configure HDD
int run_installer(char type[]){ // type = usb|hdd
    char _os_name[15]; // Could be Mavericks, Yosemite, El Capitan
    char _device_name[100] = "install_osx"; // Default USB device name is install_osx
    char _efi_name[] = "ESP"; // EFI partition mount point, default is ESP
    char _identifier[15] = "disk1"; // For USB only, default IDENTIFIER is disk1
    char _choice_file[16] = "choices."; // Choice file for Clover (format: choices.[type].[extension])
    char _clover_file[] = "Clover";
    char confirm[2] = "y";
    char root[] = "/tmp/helper"; // Temporary directory root
    char cmd[400];
    int installation_type;
    int method;
    if (strcmp(type, "hdd") == 0){
        installation_type = HDD;
    } else{
        installation_type = USB;
    }
    switch (options[1][6][0]) { // Set method
        case 'c': // createinstallmedia
            method = CREATEINSTALLMEDIA;
            break;
        case 'R': // RawImage
            method = RAW_IMG;
            break;
        default: // Default: BaseBinaries
            method = BASE_BINARIES_CLONE;
            break;
    }
    strcat(_choice_file, type); // Add type usb|hdd
    strcat(_choice_file, ".xml"); // Add extension .xml
    strcpy(_os_name, options[1][5]); // Set _os_name

    printf("\e[4mNote:\e[0m Sometimes 'sudo' permission is required.\n");

    /* === USB Formatting === */
    // First warn user if trying to do it from single user mode
    // List disks to let user know the identifier of the USB
    if (system("diskutil list") != 0) return show_error("You cannot do this operation in single user mode!");
    // Unmount ESP
    sprintf(cmd, "/Volumes/%s", _efi_name);
    if (access(cmd, F_OK) == 0) {
        sprintf(cmd, "diskutil unmount force %s", _efi_name); system(cmd);
    }
    // Skip USB formatting, if type is hdd as well as -excludeOs is given
    if (installation_type == USB && options[1][4][0] != '1') {
        printf("Enter the name of the \e[1mIDENTIFIER\e[0m of your chosen USB drive to format, be careful, data you loose shall not be recoverable (e.g. disk1): ");
        scanf("%s", _identifier);
        if (strcmp(_identifier, "disk0") == 0) { // if IDENTIFIER can't be disk0
            return show_error("Installation location can't be HDD/SSD!");
        }
        printf("You selected %s. Are you sure? (y/n): ", _identifier);
        scanf("%s", confirm);
        if (confirm[0] == 'y') {
            // Format USB and create partition
            sprintf(cmd, "diskutil partitionDisk /dev/%s 1 GPT HFS+J \"%s\" R", _identifier, _device_name);
            if (system(cmd) != 0) return show_error("Formating USB failed!");
            printf("Format successful.\n");
        }else return show_error("Installation Aborted.");
    }else{ // Set _device_name
        printf("Enter the \e[1mNAME\e[0m of the partition to install Clover (e.g. Macintosh HD): ");
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
    // Ignore Installation on -excludeClover
    if (options[1][7][0] == '1') goto p;
    // Create root dir
    sprintf(cmd, "mkdir %s", root);
    system(cmd);
    printf("Helper will now download Clover EFI.\n");
    // Download Clover, if not already
    sprintf(cmd, "%s/%s.zip", root, _clover_file);
    if (access(cmd, F_OK) != 0) {
        sprintf(cmd, "curl -#L https://sourceforge.net/projects/cloverefiboot/files/latest/download > %s/%s.zip", root, _clover_file);
        // Warn user if it doesn't have an Internet connection
        // Also, delete `root` folder too
        if (system(cmd) !=0){
            sprintf(cmd, "rm -R %s", root); system(cmd);
            return show_error("Downloading Clover failed! Do you have an Internet connection?");
        }
    }
    // Download Choice file, if not already
    sprintf(cmd, "%s/%s", root, _choice_file);
    if (access(cmd, F_OK) != 0) {
        sprintf(cmd, "curl -sL https://raw.githubusercontent.com/MuntashirAkon/Mac-OS-Installation-Helper/master/CloverInstallationChoices/%s > %s/%s", _choice_file, root, _choice_file);
        system(cmd);
    }
    printf("Unzipping...\n");
    // Unzip Clover
    // Also checks if Clover is a valid zip file
    // If not, throw an error saying to try again
    // Also deletes the `root` folder
    sprintf(cmd, "unzip -oqq -d %s %s/%s.zip", root, root,_clover_file);
    if (system(cmd) !=0){
        sprintf(cmd, "rm -R %s", root); system(cmd);
        return show_error("Clover installation failed! Please try again.");
    }
    printf("Installing Clover EFI into %s...\n", _device_name);
    // Install Clover
    sprintf(cmd, "sudo installer -pkg %s/%s*.pkg -target \"/Volumes/%s\" -applyChoiceChangesXML %s/%s", root, _clover_file, _device_name, root, _choice_file);
    if (system(cmd) != 0) return show_error("Clover Installation failed!");

    /* === Clover Configuration === */
    printf("Configuring Clover...\nDownloading essential files...\n");
    /* === Download Essential Files === */
    char kext_url[] = "https://github.com/MuntashirAkon/Mac-OS-Installation-Helper/raw/master/CloverKexts";
    // HFSPlus.efi
    sprintf(cmd, "%s/HFSPlus.efi", root);
    if (access(cmd, F_OK) != 0) {
        sprintf(cmd, "curl -sL https://github.com/JrCs/CloverGrowerPro/raw/master/Files/HFSPlus/X64/HFSPlus.efi > %s/HFSPlus.efi", root);
        system(cmd);
    }
    // FakeSMC.kext.zip
    sprintf(cmd, "%s/FakeSMC.kext.zip", root);
    if (access(cmd, F_OK) != 0) {
        sprintf(cmd, "curl -sL %s/FakeSMC.kext.zip > %s/FakeSMC.kext.zip", kext_url, root);
        system(cmd);
    }
    // VoodooPS2Controller.kext.zip
    sprintf(cmd, "%s/VoodooPS2Controller.kext.zip", root);
    if (access(cmd, F_OK) != 0) {
        sprintf(cmd, "curl -sL %s/VoodooPS2Controller.kext.zip > %s/VoodooPS2Controller.kext.zip", kext_url, root);
        system(cmd);
    }
    // GenericUSBXHCI.kext.zip
    sprintf(cmd, "%s/GenericUSBXHCI.kext.zip", root);
    if (access(cmd, F_OK) != 0) {
        sprintf(cmd, "curl -sL %s/GenericUSBXHCI.kext.zip > %s/GenericUSBXHCI.kext.zip", kext_url, root);
        system(cmd);
    }

    /* === Unzip & Configure === */
    // Delete Everything of /ESP/EFI/CLOVER/kexts/ except Other
    system("rm -R /Volumes/ESP/EFI/CLOVER/kexts/10*");
    char kext_location[100];
    sprintf(kext_location, "/Volumes/%s/EFI/CLOVER/kexts/Other/", _efi_name);
    printf("Unzipping & copying...\n");
    /* Unzip & copy files */
    sprintf(cmd, "unzip -oqq -d %s %s/FakeSMC.kext.zip", kext_location, root); system(cmd);
    sprintf(cmd, "unzip -oqq -d %s %s/VoodooPS2Controller.kext.zip", kext_location, root); system(cmd);
    sprintf(cmd, "unzip -oqq -d %s %s/GenericUSBXHCI.kext.zip", kext_location, root); system(cmd);
    // Copy HFSPlus.efi
    sprintf(cmd, "cp %s/HFSPlus.efi /Volumes/%s/EFI/CLOVER/drivers64UEFI/", root, _efi_name); system(cmd);

    /* === Special Tasks === */
    printf("Doing some special business...\n");
    if (options[1][3][0] == '1') { // RAID Mode
        // HPRAIDInjector.kext.zip
        sprintf(cmd, "%s/HPRAIDInjector.kext.zip", root);
        if (access(cmd, F_OK) != 0) {
            sprintf(cmd, "curl -sL %s/HPRAIDInjector.kext.zip > %s/HPRAIDInjector.kext.zip", kext_url, root);
            system(cmd);
        }
        // Unzip & copy
        sprintf(cmd, "unzip -oqq -d %s %s/HPRAIDInjector.kext.zip", kext_location, root); system(cmd);
    }
    if (strcmp(options[1][0], EMPTY) != 0){ // Config
        // Download config.plist
        sprintf(cmd, "%s/config.plist", root);
        if (access(cmd, F_OK) != 0){
            sprintf(cmd, "curl -sL https://github.com/MuntashirAkon/Mac-OS-Installation-Helper/raw/master/CloverLaptopConfig/config_%s.plist > %s/config.plist", options[1][0], root); system(cmd);
        }
        sprintf(cmd, "cp %s/config.plist /Volumes/%s/EFI/CLOVER/config.plist", root, _efi_name); system(cmd);
        // Enable CI in HD4200/4400/4600/5600
        if (strcmp(options[1][0], "HD4600_4400_4200") == 0 || strcmp(options[1][0], "HD5600") == 0){
            // Download FakePCIID, if not already
            sprintf(cmd, "%s/FakePCIID.kext.zip", root);
            if (access(cmd, F_OK) != 0) {
                sprintf(cmd, "curl -sL %s/FakePCIID.kext.zip > %s/FakePCIID.kext.zip", kext_url, root);
                system(cmd);
            }
            // Download FakePCIID_Intel_HD_Graphics, if not already
            sprintf(cmd, "%s/FakePCIID_Intel_HD_Graphics.kext.zip", root);
            if (access(cmd, F_OK) != 0) {
                sprintf(cmd, "curl -sL %s/FakePCIID_Intel_HD_Graphics.kext.zip > %s/FakePCIID_Intel_HD_Graphics.kext.zip", kext_url, root);
                system(cmd);
            }
            // Unzip & copy
            sprintf(cmd, "unzip -oqq -d %s %s/FakePCIID.kext.zip", kext_location, root); system(cmd);
            sprintf(cmd, "unzip -oqq -d %s %s/FakePCIID_Intel_HD_Graphics.kext.zip", kext_location, root); system(cmd);
        }
    }
    if (strcmp(options[1][1], EMPTY) != 0) { // Ethernet
        // For BroadcomBCM57xx, FakePCIID and FakePCIID_BCM57XX_as_BCM57765 have to be used
        if (strcmp(options[1][1], "BroadcomBCM57xx") == 0) {
            // Download FakePCIID, if not already
            sprintf(cmd, "%s/FakePCIID.kext.zip", root);
            if (access(cmd, F_OK) != 0) {
                sprintf(cmd, "curl -sL %s/FakePCIID.kext.zip > %s/FakePCIID.kext.zip", kext_url, root);
                system(cmd);
            }
            // Download FakePCIID_BCM57XX_as_BCM57765, if not already
            sprintf(cmd, "%s/FakePCIID_BCM57XX_as_BCM57765.kext.zip", root);
            if (access(cmd, F_OK) != 0) {
                sprintf(cmd, "curl -sL %s/FakePCIID_BCM57XX_as_BCM57765.kext.zip > %s/FakePCIID_BCM57XX_as_BCM57765.kext.zip", kext_url, root); system(cmd);
            }
            // Unzip & copy
            sprintf(cmd, "unzip -oqq -d %s %s/FakePCIID.kext.zip", kext_location, root);
            system(cmd);
            sprintf(cmd, "unzip -oqq -d %s %s/FakePCIID_BCM57XX_as_BCM57765.kext.zip", kext_location, root);
            system(cmd);
        }else{ // Download the exact kext
            sprintf(cmd, "%s/%s.kext.zip", root, options[1][1]);
            if (access(cmd, F_OK) != 0) { // Download if not already
                sprintf(cmd, "curl -sL %s/%s.kext.zip > %s/%s.kext.zip", kext_url, options[1][1], root, options[1][1]);system(cmd);
            }
            // Unzip & copy
            sprintf(cmd, "unzip -oqq -d %s %s/%s.kext.zip", kext_location, root, options[1][1]); system(cmd);
        }
    }
    printf("Done.\n");

    /* === Mac OS Installation === */
    p: // Bypass Clover
    // Install Mac OS into USB only if -excludeOs is not given
    if (installation_type == USB && options[1][4][0] == '0') {
        printf("Now Installing %s...\n", _os_name);
        if (method == CREATEINSTALLMEDIA) {
            /* === createinstallmedia Method === */
            // Results in recovery partition
            // Only applies for Mavericks, Yosemite & El Capitan
            // Works in the native Mac OS too
            sprintf(cmd, "sudo \"/Applications/Install OS X %s.app/Contents/Resources/createinstallmedia\" --volume  \"/Volumes/%s\" --applicationpath \"/Applications/Install OS X %s.app\" --nointeraction", _os_name, _device_name, _os_name);
            if (system(cmd) != 0) return show_error("Installation Failed!");
            // Rename
            sprintf(cmd, "diskutil rename \"Install OS X %s\" \"%s\"", _os_name, _device_name);
            system(cmd);
        }else if (method == RAW_IMG){
            /* === RawImage Method === */
            // Doesn't create a recovery partition
            // Note: BaseBinaries Clone method in a raw file with kernel & /Extra folder
            // Supported file types: raw, iso, img
            // Need no version specification
            printf("Restoring Raw Image to the USB...\n");
            if (access(options[1][10], F_OK) == 0) {
                // Unmount disk first
                sprintf(cmd, "diskutil unmountDisk /dev/%s", _identifier); system(cmd);
                // Restore the Raw Image to the usb
                printf("Note: Press \e[1mctrl+t\e[0m to show status.\n");
                sprintf(cmd, "sudo dd if=\"%s\" of=/dev/r%s bs=1m", options[1][10], _identifier);
                if (system(cmd) != 0) return show_error("Installation Failed!");
                // Rename
                sprintf(cmd, "diskutil rename \"OS X Base System\" \"%s\"", _device_name); system(cmd);
                // Delete /Exrta folder & /kernel which are for legacy
                sprintf(cmd, "rm -R \"/Volumes/%s/Extra/\" \"Volumes/%s/kernel\"", _device_name, _device_name); system(cmd);
                // Eject the disk
                sprintf(cmd, "diskutil eject /dev/%s", _identifier);
            }else return show_error("Invalid source. Try again.\n");
        }else{ // method = BASE_BINARIES_CLONE
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
            printf("5... ");
            // Copy Packages as folder
            sprintf(cmd, "cp -a /tmp/install_esd/Packages \"/Volumes/%s/System/Installation\"", _device_name);
            system(cmd);
            printf("60... ");
            // Copy BaseSystem.dmg
            sprintf(cmd, "cp -a /tmp/install_esd/BaseSystem.dmg /tmp/install_esd/BaseSystem.chunklist \"/Volumes/%s\"", _device_name);
            system(cmd);
            printf("100\n");
            // Unmount InstallESD.dmg
            system("hdiutil detach /tmp/install_esd");
            // Remove tmp dir
            system("rmdir /tmp/install_esd");
            /*
             * Extract kernel
             * Don't waste time for copying kernel if -replaceKernel is given
             * TODO: need to be implemented later perhaps with an option -extractKernel
            if (strcmp(options[1][8], EMPTY) == 0 || (strcmp(options[1][8], EMPTY) != 0 && access(options[1][8], F_OK) != 0)) {
                // extract & copy kernel
            }
            */

        }
        if (method == BASE_BINARIES_CLONE || method == RAW_IMG) { // MBR patch and kernel replace
            /* === Special (for BaseBinaries and/or RawImage) === */
            // MBR patch
            if (strcmp(options[1][9], EMPTY) != 0 && access(options[1][9], F_OK) == 0){
                printf("Applying MBR patch... ");
                sprintf(cmd, "cp \"%s/OSInstall.mpkg\" \"/Volumes/%s/System/Installation/Packages/OSInstall.mpkg\"", options[1][9], _device_name);
                system(cmd);
                printf("...\n");
                sprintf(cmd, "cp \"%s/OSInstaller\" \"/Volumes/%s/System/Library/PrivateFrameworks/OSInstaller.framework/Versions/A/OSInstaller\"", options[1][9], _device_name);
                system(cmd);
            }
            // Replace Kernel
            if (strcmp(options[1][8], EMPTY) != 0 && access(options[1][8], F_OK) == 0) {
                printf("Replacing kernel...\n");
                // Backup kernel first
                sprintf(cmd, "/Volumes/%s/System/Library/Kernels/kernel", _device_name);
                if (access(cmd, F_OK) == 0) { // Backup kernel if exists
                    sprintf(cmd, "cp \"/Volumes/%s/System/Library/Kernels/kernel\" ~/Desktop/kernel_backup", _device_name); system(cmd);
                }else{ // Create directory
                    sprintf(cmd, "mkdir \"/Volumes/%s/System/Library/Kernels/\"", _device_name); system(cmd);
                }
                // Replace/Add kernel
                sprintf(cmd, "cp \"%s\" \"/Volumes/%s/System/Library/Kernels/kernel\"", options[1][8], _device_name);
            }
        }
        printf("Done.\n");
    }
    // Final Message
    printf("Your %s is now ready.\n"
           "NB: If you did not use a config.plist, you should configure your own config.plist "
           "based on your hardwares. Otherwise your USB/HDD may not work. "
           "The EFI partition should be mounted at /Volumes/ESP/ (Shown as EFI).\n", type);
    return EXIT_SUCCESS;
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
    if (strcmp(mount_point, EMPTY) == 0) sprintf(cmd, "diskutil mount %s", _identifier);
    else{
        sprintf(cmd, "diskutil unmount force %s", _identifier); system(cmd);
        sprintf(cmd, "diskutil mount -mountPoint %s %s", mount_point, _identifier);
    }
    // First warn user if trying to do it from single user mode
    if (system(cmd) != 0) return show_error("You cannot do this operation in single user mode!");
    return EXIT_SUCCESS;
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
        sprintf(cmd, "curl -sL https://bitbucket.org/RehabMan/os-x-maciasl-patchmatic/downloads/RehabMan-patchmatic-2015-0107.zip > %s/patchmatic.zip", root);
        system(cmd);
    }
    // Extract patchmatic.zip
    sprintf(cmd, "unzip %s/patchmatic.zip -d %s", root, destination); system(cmd);
    // Download iasl.zip
    if (disassemble) {
        sprintf(cmd, "%s/iasl.zip", root); system(cmd);
        if (access(cmd, F_OK) != 0) {
            sprintf(cmd, "curl -sL https://bitbucket.org/RehabMan/acpica/downloads/iasl.zip > %s/iasl.zip", root); system(cmd);
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
    return EXIT_SUCCESS;
}

// Kext administration
int kext_admin(unsigned int id, char src[]){
    // For id INSTALL_KEXT|UNINSTALL_KEXT: src is the *source* of the kext
    // And for id FIX_PERMISSIONS: src is the *name* of the kext (this is for internal use only)
    char cmd[300];
    switch (id) {
        case INSTALL_KEXT:
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
                kext_admin(FIX_PERMISSIONS, kext_name);
            }else return show_error("Inaccessible or invalid source.");
            break;
        case UNINSTALL_KEXT:
            // Uninstall the kext
            if (access(src, F_OK) == 0) {
                printf("Removing the kext...\n");
                // Backup & remove the kext
                sprintf(cmd, "sudo mv \"%s\" ~/Desktop/", src);
                system(cmd);
                // Rebuild Cache
                kext_admin(REBUILD_CACHE, EMPTY);
            }else return show_error("Inaccessible or invalid source.");
            break;
        case FIX_PERMISSIONS:
            // Fix permission
            printf("Fixing permissions...\n");
            if (strcmp(src, "") != 0) { // If kext_name is set, only fix permission of the kext
                sprintf(cmd, "sudo chown -R root:wheel /System/Library/Extensions/%s", src);
            }else{ // Else fix permission of the whole /S/L/E
                sprintf(cmd, "sudo chown -R root:wheel /System/Library/Extensions/*");
            }
            if (system(cmd) != 0) return show_error("Fixing permission failed!");
            // Also, rebuid cache
            kext_admin(REBUILD_CACHE, EMPTY);
            break;
        case REBUILD_CACHE:
            // Rebuild cache
            printf("Rebuilding Cache...\n");
            if (system("sudo touch /S*/L*/Extensions && sudo kextcache -Boot -U /") != 0) return show_error("Rebuilding Kernel Cache failed!");
            printf("Done.\n");
            break;
        default:
            return show_error("Invalid Argument(s).");
    }
    return EXIT_SUCCESS;
}

// Recovery tools
int recovery(unsigned int id, char src[]){
    char cmd[300];
    char _identifier[15];
    char location[100];
    char confirm[2]; // Solve weired problem related to scanf()
    switch (id) {
        case REMOVE_KEXT:
            kext_admin(UNINSTALL_KEXT, src);
            break;
        case FIX_APPLE_ID:
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
        case INIT:
            // Initialize & mount
            // This is needed when booting into single user mode
            if (system("fsck -fy && mount -uw /") != 0) return show_error("Init failed! This usually occurs because of the user being not logged in single user mode.");
        case LIST_UNSIGNED:
            // List Unsigned kext(s)
            if (system("sudo touch /S*/L*/Extensions && sudo kextcache -Boot -U /") != 0) return show_error("Operation failed!");
            break;
        case INSTALL_ON_USB:
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
        case REPLACE_KERNEL_RECOVERY:
            // Replace Kernel
            if (access(src, F_OK)) {
                // Backup current kernel
                printf("Backing up kernel...\n");
                system("cp /System/Library/Kernels/kernel ~/Desktop/kernel_backup");
                if (system(cmd) == 0) printf("Kernel is backed up at ~/Desktop/\n");
                // Replace Kext
                printf("Replacing kernel...\n");
                sprintf(cmd, "sudo cp %s /System/Library/Kernels/kernel", src);
                if (system(cmd) != 0) return show_error("Replacing kernel failed!");
            }else return show_error("Unresolved source.");
            break;
        case DISABLE_HIBERNATION:
            // Disable Hibernation
            printf("Disabling Hibernation...\n");
            if(system("sudo pmset -a hibernatemode 0 && sudo rm /var/vm/sleepimage && sudo mkdir /var/vm/sleepimage") != 0) return show_error("Disabling hibernation failed!");
            break;
        case COPY_CLOVER:
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
    return EXIT_SUCCESS;
}

// Generate error message: something failed!
int show_error(char subject[]){
    printf("machelper: %s\n", subject);
    return EXIT_FAILURE;
}
