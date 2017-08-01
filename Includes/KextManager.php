<?php
/**
 * Created by PhpStorm.
 * User: muntashir
 * Date: 12/29/16
 * Time: 8:39 PM
 */

namespace MacHelper;

require_once __DIR__."/Essentials.php";

/**
 * Class KextManager
 *
 * @package MacHelper
 */
class KextManager
{
    const SLE = "/System/Library/Extensions";
    const LE  = "/Library/Extensions";
    /**
     * @var string $backup_dir
     */
    protected $backup_dir;
    protected $kext_dir;

    /**
     * KextManager constructor.
     * @param string $installation_path SLE or LE
     * @param string $backup_dir        Where to backup uninstalled kext(s)
     */
    public function __construct($installation_path = self::LE, $backup_dir = null){
        $this->kext_dir   = $installation_path;
        $this->backup_dir = $backup_dir == null ? exec('echo /Users/$USER/Desktop') : $backup_dir;
    }

    /**
     * Installs Kext(s)
     *
     * Also Backup and Fixes permissions.
     *
     * @param string|array  $src
     * @param bool          $rebuild_cache
     */
    public function install_kext($src, $rebuild_cache = true){
        if(is_array($src)) foreach ($src as $source) $this->kext_installer($source);
        else $this->kext_installer($src);

        if($rebuild_cache) $this->rebuild_cache();
    }

    /**
     * Uninstalls Kext(s)
     *
     * @param string|array  $src
     * @param bool          $rebuild_cache
     */
    public function uninstall_kext($src, $rebuild_cache = true){
        if(is_array($src)) foreach ($src as $source) $this->kext_uninstaller($source);
        else $this->kext_uninstaller($src);

        if($rebuild_cache) $this->rebuild_cache();
    }

    /**
     * Fixes permission(s)
     *
     * If parameter is set, only fix the permission of the kext
     * Otherwise fix permissions of the entire S/L/E or L/E
     *
     * @param string $kext
     * @return void
     */
    public function fix_permission($kext = null){
        if($kext !== null){
            exec("sudo chown -R root:wheel {$this->kext_dir}/{$kext}");
        }else{
            exec("sudo chown -R root:wheel {$this->kext_dir}/*");
        }
    }

    /**
     * Rebuilds Kernel Cache
     *
     * @return bool
     */
    public function rebuild_cache(){
//        exec("sudo touch /S*/L*/Extensions");
        passthru("sudo kextcache -i /", $return);
        return ($return == Essentials::EXIT_SUCCESS) ? true : false;
    }

    /**
     * @param string $src location to the kext to be installed
     * @return bool
     */
    protected function kext_installer($src){
        if(file_exists($src) AND preg_match('/.*\.kext$/', $src)){
            $kext = basename($src);
            $this->backup_kext($kext);

            passthru("sudo cp -R \"{$src}\" {$this->kext_dir}/{$kext}", $return);
            if($return != Essentials::EXIT_SUCCESS) return false;
            $this->fix_permission($kext);
        }else{
            return false;
        }
        return true;
    }

    /**
     *
     * @param string $src Either Source or Kext name
     * @return bool
     */
    protected function kext_uninstaller($src){
        return ($this->backup_kext(basename($src)) ? true : false);
    }

    /**
     * Backs up as well as moves the requested kext if exists
     *
     * @param string $kext
     * @return bool
     */
    protected function backup_kext($kext){
        if(file_exists("{$this->kext_dir}/{$kext}")){
            // Backup kext if already exists at /S/L/E
            date_default_timezone_set('UTC');
            $backup_dir_name = "Kext_Backup_".date("Y-m-d");
            $backup_dir = "{$this->backup_dir}/{$backup_dir_name}";
            if(!file_exists($backup_dir)) mkdir($backup_dir);
            passthru("sudo mv {$this->kext_dir}/{$kext} {$backup_dir}/", $return);
            if($return === Essentials::EXIT_SUCCESS){
                return true;
            }
        }
        return false;
    }
}
