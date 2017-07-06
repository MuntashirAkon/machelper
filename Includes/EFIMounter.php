<?php

/**
 * Created by PhpStorm.
 * User: muntashir
 * Date: 1/26/17
 * Time: 12:13 AM
 */
namespace MacHelper;

require_once __DIR__."/Essentials.php";
require_once __DIR__."/../Libraries/PListEditor/PListEditor.php";

use PListEditor\PListEditor;

/**
 * Class EFIMounter
 *
 * By default, it identifies the drive where macOS is installed
 *  and mounts the EFI partition as ESP (Clover default).
 *
 * If a MountPoint or DeviceIdentifier is given, it'll find out
 *  the associated drive and mounts the EFI partition if available.
 *
 * It also has interactive mode which allows anyone to choose from
 *  a bunch of DeviceIdentifier/MountPoint list (implemented in the
 *  script file using `machelper efi list` and `machelper efi mount ...`).
 *
 * NOTE 1: This function uses `mount` to mount or eject volumes and
 * `diskutil` to get disk list and other info.
 * NOTE 2: This can be used to mount other volumes too (just a little hack!)
 *
 * @package MacHelper
 */

class EFIMounter
{
    const DISK_PLIST      = Essentials::TMP_DIR . "/info.muntashir.machelper.disks.plist";
    /**
     * Clover default, being the default path for ESP Mounter
     */
    const DEFAULT_EFI_DIR = "/Volumes/ESP";

    protected $mountPoint;

    /**
     * EFIMounter constructor.
     *
     * @param string $mountPoint EFI mount point
     */
    public function __construct($mountPoint = self::DEFAULT_EFI_DIR){
        $this->mountPoint = $mountPoint;
        Essentials::createRoot();
    }

    public function setMountPoint($mountPoint){
        $this->mountPoint = $mountPoint;
    }

    /**
     * Mounts the EFI volume automatically
     *
     * @return bool
     */
    public function mountAutomatically(){
        $disk = $this->determineEFIPartition();
        if($disk != null) return $this->mount($disk, true);
        return false;
    }

    public function listEFIPartitions(){
        if(!$this->getDevices()) return false;
        $disks = $this->listAllGUIDDrives();
        $efi_partitions = [];
        foreach ($disks['efi_partitions'] as $disk)
            foreach ($disk as $partition)
                array_push($efi_partitions, $partition);
        return $efi_partitions;
    }
    /**
     * Mounts a given EFI partition
     *
     * @param string $efi_partition Disk identifier of the EFI partition
     * @param bool   $force
     * @return bool
     */
    public function mount($efi_partition, $force = false){
        if($force){
            if($this->alreadyMounted($this->mountPoint))
                $this->unmount($this->mountPoint);

            if($this->alreadyMounted($efi_partition))
                $this->unmount($efi_partition);

//            if(file_exists($this->mountPoint)){
//                exec('sudo rmdir "' . $this->mountPoint . '"');
//            }
        }

        if($force || !($this->alreadyMounted($this->mountPoint) || $this->alreadyMounted($efi_partition))){
            if(!file_exists($this->mountPoint)) exec("sudo mkdir {$this->mountPoint}");
            exec("sudo chown \$USER:staff {$this->mountPoint}");
            passthru("sudo mount -t msdos -o async,noowners -w /dev/{$efi_partition} {$this->mountPoint}", $return);
            if($return === Essentials::EXIT_SUCCESS) return true;
            // else passthru('sudo rmdir "'.$this->mountPoint.'""', $return);
        }
        return false;
    }

    /**
     * Unmount a certain partition
     *
     * @param string $mountPoint DiskIdentifier or mount point
     * @return bool
     */
    public function unmount($mountPoint){
        $mountPoint = preg_replace('/\/$/', '', $mountPoint); // Remove Trailing slash, if any
        if(preg_match('/^(disk)([\d]+)(s)([\d]+)$/', $mountPoint)) // Given parameter is a device identifier (otherwise, mount point)
            $mountPoint = "/dev/{$mountPoint}";

        if($this->alreadyMounted($mountPoint)){
            passthru("sudo umount -f {$mountPoint}", $return); // -f flag is used to suppress any kind of error, even though originally used for forcing
            // Delete mounted folder, not implemented, neither it is recommended
            // if(file_exists($mountPoint)) exec('sudo rmdir  "' . $mountPoint . '"');
            if($return == Essentials::EXIT_SUCCESS) return true;
        }
        return false;
    }

    /**
     * Lists all the GUID partitions based on their drives
     *
     * Returns an array like this:
     * <code>
     * [
     *   disks => [disk0, disk1],
     *   efi_partitions => [
     *      disk0 => [disk0s1],
     *      disk1 => [disk1s1]
     *   ],
     *   mounted_volumes => [
     *      disk0 => [macOS HD, Albatross, Tectonic Plate],
     *      disk1 => [macOS, Heart & Soul Nebula]
     *   ],
     *   already_mounted => [disk0],
     *   root_disk => disk0
     * ]
     * </code>
     * @return null|array
     */
    public function listAllGUIDDrives(){
        $disks = [
            'disks' => [],
            'efi_partitions' => [],
            'mounted_volumes' => [],
            'already_mounted' => [],
            'root_disk' => null
        ];

        Essentials::createRoot();
        //if($return !== self::EXIT_SUCCESS) return null;
        $plist = new PListEditor();
        $plist->readFile(self::DISK_PLIST);
        $partitions = $plist->root()->getProperty("AllDisksAndPartitions");
        if($partitions->hasProperties()){
            /** @var \PListEditor\PListProperty $disk */
            foreach($partitions->innerProperties() as $disk){
                if($disk->getProperty("Content")->value() == "GUID_partition_scheme"){
                    $diskID = $disk->getProperty("DeviceIdentifier")->value();
                    array_push($disks['disks'], $diskID);
                    $partitions = $disk->getProperty("Partitions");
                    if($partitions->hasProperties()){
                        $disks['efi_partitions'][$diskID]  = [];
                        $disks['mounted_volumes'][$diskID] = [];
                        foreach($partitions->innerProperties() as $partition){
                            if($partition->getProperty("Content")->value() == "EFI"){
                                if($partition->getProperty("MountPoint") != null){
                                    array_push($disks['already_mounted'], $diskID);
                                }
                                array_push($disks['efi_partitions'][$diskID], $partition->getProperty("DeviceIdentifier")->value());
                            }else{
                                if($partition->getProperty("MountPoint") != null){
                                    $volume_name = $partition->getProperty("VolumeName");
                                    // Volume name isn't always set
                                    array_push($disks['mounted_volumes'][$diskID], $volume_name != null ? $volume_name->value() : '');
                                    if($partition->getProperty("MountPoint")->value() == "/") $disks['root_disk'] = $diskID;
                                }
                            }
                        }
                    }
                }
            }
        }
        return $disks;
    }

    /**
     * Determines the EFI partitions automatically
     *
     * @return null|string
     */
    public function determineEFIPartition(){
        if(!$this->getDevices()) return null;
        $disks = $this->listAllGUIDDrives();
        $disk = $disks['root_disk'];
        if($disk != null){
            $disks = $disks['efi_partitions'][$disk];
            if(count($disks) == 1){         // Can't be determined if there are more than one EFI partition
                return $disks[0];
            }
        }
        return null;
    }

    /**
     * Generates a plist file in the root directory
     *
     * @return bool
     */
    public function getDevices(){
        passthru('diskutil list -plist physical > '.self::DISK_PLIST, $return);
        return ($return === Essentials::EXIT_SUCCESS) ? true : false;
    }

    /**
     * Checks if the partition is already mounted
     *
     * @param string $device can be a mount point or a device identifier
     * @return bool true if mounted already, otherwise false
     */
    public function alreadyMounted($device){
        if(preg_match('/^(disk)([\d]+)(s)([\d]+)$/', $device))      // Given parameter is a device identifier (otherwise, mount point)
            $device = "/dev/{$device}";
        if(file_exists($device) === false) return false;

        exec("mount | grep \"{$device} \"", $out);
        if(count($out) < 1) return false;
        return true;
    }
}