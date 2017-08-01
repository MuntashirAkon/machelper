<?php
/**
 * Created by PhpStorm.
 * User: muntashir
 * Date: 6/22/17
 * Time: 4:50 AM
 */

namespace MacHelper;

require_once __DIR__ . '/Essentials.php';
require_once __DIR__ . '/../Libraries/PListEditor/PListEditor.php';

/**
 * Class DiskManager
 *
 * Disk manager is a disk management tool for macOS X.
 * Although named as `DiskManager`, this class only reads
 * disk info from `diskutil`, a commandline tool comes natively
 * with macOS.
 *
 * @package MacHelper
 */
class DiskManager
{
    /*
     * Disk Types
     */
    const GUID     = 'GUID_partition_scheme';
    const FDisk    = 'FDisk_partition_scheme';
    const APS      = 'Apple_partition_scheme';
    const INTERNAL = 'internal';
    const EXTERNAL = 'external';
    const PHYSICAL = 'physical';
    const VIRTUAL  = 'virtual';

    private $disk_plist = Essentials::TMP_DIR . '/info.muntashir.machelper.disks.plist';
    public $disks = [];

    function __construct(){
        Essentials::createRoot();
    }

    /**
     * Get the requested disks info plist using the `diskutil` tool
     *
     * NOTE: only physical disks will be returned in next versions
     *
     * @param string $scheme Disk partition scheme, returns all partitions if not set
     * @param string $verb1  physical or virtual, returns all if not set
     * @param string $verb2  internal or external, returns all if not set
     * @return bool true on success, false on failure.
     */
    function getDisksList($scheme = '', $verb1 = '', $verb2 = ''){
        passthru('diskutil list -plist ' . $verb1 . ' ' .  $verb2 . ' > '.$this->disk_plist, $return);
        if($return === Essentials::EXIT_SUCCESS){
            $all_disks = \PListEditor\plist_decode_file($this->disk_plist);
            switch($scheme){
                case self::GUID:
                    foreach($all_disks['AllDisksAndPartitions'] as $disk)
                        if($disk['Content'] == self::GUID) array_push($this->disks, $disk);
                    break;
                case self::FDisk:
                    foreach($all_disks['AllDisksAndPartitions'] as $disk)
                        if($disk['Content'] == self::FDisk) array_push($this->disks, $disk);
                    break;
                case self::APS:
                    foreach($all_disks['AllDisksAndPartitions'] as $disk)
                        if($disk['Content'] == self::APS) array_push($this->disks, $disk);
                    break;
                default:
                    $this->disks = $all_disks['AllDisksAndPartitions'];
            }
            return true;
        }
        return false;
    }

    /**
     *
     */
}