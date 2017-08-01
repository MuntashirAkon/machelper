<?php
/**
 * Created by PhpStorm.
 * User: muntashir
 * Date: 6/21/17
 * Time: 10:08 PM
 */

namespace MacHelper;

require_once __DIR__ . '/Essentials.php';
require_once __DIR__ . '/../Libraries/PListEditor/PListEditor.php';

use \PListEditor\PListEditor;

/**
 * Class CloverInstaller
 *
 * CLOVER INSTALLER ONLY SUPPORTS EFI.
 * No legacy support added, neither I've planned to support legacy.
 * But anyone can easily implement this feature if s/he wants.
 *
 * @package MacHelper
 */
class CloverInstaller
{
    /*
     * Installation modes, installation choices change according to these modes
     */
    const INSTALLATION_MEDIA   = 1; // Bootable USB
    const POST_INSTALLATION    = 2; // To the newly created USB
    const REGULAR_INSTALLATION = 3; // Do not apply any choice file
    /*
     * PC type
     */
    const DESKTOP = 4;
    const LAPTOP  = 5;
    /*
     * array macOSNames names and version
     */
    const macOSNames = [
        'Tiger'         => [10.4 , 'Mac OS X'],
        'Leopard'       => [10.5 , 'Mac OS X'],
        'Snow Leopard'  => [10.6 , 'Mac OS X'],
        'Lion'          => [10.7 , 'Mac OS X'],
        'Mountain Lion' => [10.8 , 'OS X'],
        'Mavericks'     => [10.9 , 'OS X'],
        'Yosemite'      => [10.10, 'OS X'],
        'El Capitan'    => [10.11, 'OS X'],
        'Sierra'        => [10.12, 'macOS'],
        'High Sierra'   => [10.13, 'macOS']
    ];
    /*
     * Download and file related constants.php
     */
    const CLOVER_FILE = Essentials::TMP_DIR . '/Clover.zip';
    const CLOVER_URL  = 'https://sourceforge.net/projects/cloverefiboot/files/latest/download';
    const CLOVER_CHOICE_FILE = Essentials::TMP_DIR . '/Choices.plist';
    const CLOVER_EFI_DIR = '/Volumes/ESP/EFI/CLOVER';
    /**
     * @var int
     */
    protected $mode;
    protected $pc_type;

    /*
     * Clover related variables
     */
    protected $clover_package = null;
    protected $clover_version   = 0;
    protected $target_volumes   = [];
    protected $target_volume;


    /**
     * CloverInstaller constructor.
     *
     * @param int $mode Installation mode constant
     * @param int $pc_type  PC type: laptop or desktop (because of some major config changes based on spec)
     */
    function __construct($mode, $target, $pc_type){
        $this->mode = $mode;
        $this->target_volume = $target;
        $this->pc_type = $pc_type;
        Essentials::createRoot();
    }

    /**
     * This method downloads essential files and prepare them to be installed
     * The downloaded files include:
     * - Clover bootloader
     * - Choice file
     */
    function prepare(){
        // Download the latest stable Clover release
        Essentials::download(self::CLOVER_URL, self::CLOVER_FILE);
        // Unzip the file
        Essentials::unzip(self::CLOVER_FILE, Essentials::TMP_DIR);
        // Extract latest version and filename: in case of trying multiple times
        exec('ls  ~/Downloads/Clover_*.pkg', $output, $return);
        foreach($output as $file){
            preg_match('/r(?<version>\d+).pkg$/', $file, $match);
            if((int)$match['version'] > $this->clover_version){
                $this->clover_version   = (int)$match['version'];
                $this->clover_package = $file;
            }
        }
        // Extract target volumes
        exec('installer -volinfo -pkg ' . $this->clover_package, $this->target_volumes);
        // Extract choice file
        exec('installer -showChoiceChangesXML -pkg ' . $this->clover_package, $choices, $return);
        file_put_contents(self::CLOVER_CHOICE_FILE, implode("\n", $choices));
        $this->prepare_choices();
    }

    /**
     * Installs Clover bootloader
     *
     * NOTE: By Clover default, the EFI volume is mounted at /Volumes/ESP
     *
     * @return bool true on success and false on failure
     */
    function install(){
        if(!in_array($this->target_volume, $this->target_volumes)) return false;
        passthru("sudo installer -pkg \"{$this->clover_package}\" -target \"{$this->target_volume}\" -applyChoiceChangesXML " . self::CLOVER_CHOICE_FILE, $return);
        return ($return == Essentials::EXIT_SUCCESS) ? true : false;
    }

    function configure(){
        $kext_location = "/Volumes/ESP/EFI/CLOVER/kexts/Other";

        // Remove all the folder except Other at CLOVER/kexts
        exec('rm -R ' . self::CLOVER_EFI_DIR . '/kexts/10*');
        // TODO: Remove drivers32EFI, if architecture is set to X64
        // Install HFSPlus.efi
        Essentials::download('https://github.com/JrCs/CloverGrowerPro/raw/master/Files/HFSPlus/X64/HFSPlus.efi', self::CLOVER_EFI_DIR . '/drivers64UEFI');
        // TODO: Install apfs.efi
        // Install the latest FakeSMC.kext
        $kext = (new \BitBucketDownloader('RehabMan', 'os-x-fakesmc-kozlek'))->getFile('FakeSMC', Essentials::TMP_DIR);
        Essentials::unzip($kext['file'], Essentials::TMP_DIR);
        exec("cp -R ".Essentials::TMP_DIR."/FakeSMC.kext {$kext_location}");

        // PC specific business
        if($this->pc_type == self::LAPTOP){

        }
    }

    /**
     * Get macOS installer path
     *
     * If only name is provided in options, find the macOS installer in the default path.
     *
     * @param int|string $os
     * @return string|null Returns app path on success, null otherwise
     */
    protected function get_app_path($os){
        $os_path = null;
        if(file_exists($os)){
            $os_path = $os;
        }elseif($this->valid_os_name($os)){
            $possible_os_paths = [];
            array_push($possible_os_paths, '/Applications/Install ' . self::macOSNames[$os][1] . ' ' . $os . '.app');
            array_push($possible_os_paths, '/Applications/Install ' . self::macOSNames[$os][1] . ' ' . $os . ' Beta.app');
            array_push($possible_os_paths, '/Applications/Install ' . self::macOSNames[$os][1] . ' '. self::macOSNames[$os][0] . '.app');
            array_push($possible_os_paths, '/Applications/Install ' . self::macOSNames[$os][1] . ' '. self::macOSNames[$os][0] . ' Beta.app');
            foreach($possible_os_paths as $possible_os_path){
                if(file_exists($possible_os_path)){
                    $os_path = $possible_os_path;
                    break;
                }
            }
        }
        return $os_path;
    }

    /**
     * Verifies the provided macOS installer app
     * Also returns os name and type
     *
     * @param string $os_path
     * @return string|false A string of macOS app name
     */
    protected function verify_os_app($os_path){
        $plist = new PListEditor();
        $plist->readFile($os_path . '/Contents/Info.plist');
        $os_info = $plist->root()->getProperty("CFBundleDisplayName");
        if($os_info !== null AND preg_match('/^Install (macOS|OS X) ([A-Za-z\d\. ]+)$/', $os_info->value())) return $os_info->value();
        return false;
    }

    /**
     * Is the provided os name a valid one?
     *
     * @param string $os_name macOS name
     * @return bool
     */
    private function valid_os_name($os_name){
        foreach (self::macOSNames as $macOSName => $value){
            if($macOSName == $os_name) return true;
        }
        return false;
    }
}
