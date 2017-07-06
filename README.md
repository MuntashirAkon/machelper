# machelper
A command line tool for Macintosh as well as Hackintosh to help OSx86 users.

## Features

(Run `machelper help` or just `machelper` for a list of options)
- Create bootable USB for Hackintosh as well as Macintosh
  - Apply MBR patch (From [OSx86.org](OSx86.org))
  - Replace Kernel
  - Special `config.plist` as well kext for Intel Laptops (*credit: RehabMan*)
  - Ethernet kext (or just null ethernet)
  - Exclude macOS installation, install only Clover
  - Exclude Clover installation (for native Macs)
- Post installation
  - Install Clover bootloader
  - Install necessary kexts at LE (/Library/Extensions) instead of CLOVER/kexts
  - Copy the bootloader from the USB to HDD/SSD
  - Extract and/or disassemble DSDT/SSDTs
- Daily usage
  - Mount EFI partition
  - Install kext as well as fix permission and rebuild cache.
  - Disable hibernation
  
The `v1.0.0-php` and later (the project was previously written in C, now in PHP)
also come with the [`hackupdater`](https://github.com/MuntashirAkon/hackupdater) tool
which run script(s) or re-install kext(s) after an update or an upgrade.
Run `machelper hackupdater` for help or visit the [hackupdater repo](https://github.com/MuntashirAkon/hackupdater).

## Binaries
Latest Binary can be found [here](https://github.com/MuntashirAkon/machelper/releases/latest).

*Always use the latest binaries to get a better user experience.*

## Installation
Unzip the binary and run `machelper_installer` to install.

To make sure that all is working fine, run `machelper version` from Terminal.

## Contribution & Feature Request
You are absolutely welcome to contribute and/or request for features.

## Credits
RehabMan, Netkas, Slice, Allan, OSx86.org, InsanelyMac, CloverEFIBoot and Apple.

## License
MIT License
