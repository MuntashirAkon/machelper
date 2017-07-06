<?php
/**
 * Created by PhpStorm.
 * User: muntashir
 * Date: 11/26/16
 * Time: 1:21 PM
 */

namespace MacHelper;

use PListEditor\PListEditor;
use PListEditor\PListProperty;

require_once __DIR__."/Essentials.php";
require_once __DIR__."/KextManager.php";
require_once __DIR__."/../Libraries/PListEditor/PListEditor.php";

/**
 * Class HackUpdater
 *
 * A startup script for OSx86 (Hackintosh) user to
 * install kexts or run scripts on update or upgrade
 *
 * NOTE: It will be installed for all users.
 *
 * NOTICE: This version of hackupdater is different from
 *        the one that found in my other repo, and part of
 *        a different project & a different license.
 * @package MacHelper
 */

class HackUpdater
{
    const LABEL = "info.muntashir.machelper.hackupdater";
    const PLIST_FILE_NAME = self::LABEL.".plist";
    const LAUNCH_DAEMON_DIR = "/Library/LaunchDaemons";

    const WORKING_DIR = '/Users/Shared/.hackupdater';
    const INFO_PLIST = 'Info.plist';
    const KEXTS_DIR = 'kexts';
    const SCRIPTS_DIR = 'scripts';

    /**
     * Installs HackUpdater
     *
     * Creates a `LaunchAgent` at `/Library/LaunchDaemons`
     * and loads it.
     *
     * @return bool `true` on success and `false` on failure
     */
    public function install(){
        if($this->is_installed()) return true;

        $plist = new PListEditor();
        $plist->create();
        $root = $plist->root(PListProperty::PL_DICT);
        $root->addProperty(PListProperty::PL_STRING, self::LABEL, "Label");
        $args = $root->addProperty(PListProperty::PL_ARRAY, null, "ProgramArguments"); // TODO: need to fix the commands
            $args->addProperty(PListProperty::PL_STRING, "/usr/local/machelper/machelper");
            $args->addProperty(PListProperty::PL_STRING, "hackupdater");
            $args->addProperty(PListProperty::PL_STRING, "update");
        $root->addProperty(PListProperty::PL_TRUE, null, "RunAtLoad");
        $root->addProperty(PListProperty::PL_STRING, "/Users/Shared/.hackupdater/logs/in.log", "StandardInPath");
        $root->addProperty(PListProperty::PL_STRING, "/Users/Shared/.hackupdater/logs/out.log", "StandardOutPath");
        $root->addProperty(PListProperty::PL_STRING, "/Users/Shared/.hackupdater/logs/error.log", "StandardErrorPath");
        //print_r($plist->preview());
//        if(file_exists(self::TMP_DIR) === false) mkdir(self::TMP_DIR);
        Essentials::createRoot();
        $plist_dir = Essentials::TMP_DIR."/".self::PLIST_FILE_NAME;
        $plist->save($plist_dir);
        exec("sudo cp {$plist_dir} ".self::LAUNCH_DAEMON_DIR);
        exec("sudo chown root:wheel ".self::LAUNCH_DAEMON_DIR."/".self::PLIST_FILE_NAME);
        $this->set_mac_os_version();
        return $this->enable();
    }

    /**
     * Enables HackUpdater
     *
     * NOTE: HackUpdater is enabled by default upon installation
     *
     * @return bool
     */
    public function enable(){
        if($this->is_installed()){
            exec("launchctl load -w ".self::LAUNCH_DAEMON_DIR."/".self::PLIST_FILE_NAME, $out, $return);
            if($return === Essentials::EXIT_SUCCESS) return true;
        }
        return false;
    }

    /**
     * Disables HackUpdater
     *
     * @return bool
     */
    public function disable(){
        if($this->is_installed()){
            exec("launchctl unload -w ".self::LAUNCH_DAEMON_DIR."/".self::PLIST_FILE_NAME, $out, $return);
            if($return === Essentials::EXIT_SUCCESS) return true;
        }else return true; // Because the file isn't there already/not installed
        return false;
    }

    /**
     * This method should be run after an update
     *
     * It runs scripts and kexts that user have set.
     *
     * @return void
     */
    public function update(){
        if($this->is_mac_os_updated()){
            $this->run_scripts();
            $this->install_kexts();
        }
    }

    //

    /**
     * Checks whether HackUpdated is installed
     *
     * It merely checks whether the plist file is
     * existed at /Library/LaunchDaemons/
     *
     * @return bool
     */
    protected function is_installed(){
        return (file_exists(self::LAUNCH_DAEMON_DIR."/".self::PLIST_FILE_NAME) !== false) ? true : false;
    }

    /**
     * Get macOS version from sw_vers
     *
     * @return array containing product and build numbers
     */
    protected function get_mac_os_version(){
        exec('sw_vers', $version);
        $product = explode(":\t", $version[1]);
        $build = explode(":\t", $version[2]);
        return ["product" => $product[1], "build" => $build[1]];
    }

    /**
     * Sets MacOS version to the Info.plist file
     *
     * @return void
     */
    protected function set_mac_os_version(){
        $plist_file = self::WORKING_DIR."/".self::INFO_PLIST;
        $plist      = new PListEditor();
        if(file_exists($plist_file) === false){
            $plist->create();
            $root = $plist->root(PListProperty::PL_DICT);
        }else{
            $plist->readFile($plist_file);
            $root = $plist->root();
        }
        $version         = $this->get_mac_os_version();
        $versionProperty = $root->addProperty(PListProperty::PL_DICT, null, "Version");
            $versionProperty->addProperty(PListProperty::PL_STRING, $version["product"], "product");
            $versionProperty->addProperty(PListProperty::PL_STRING, $version["build"], "build");
        $plist->save($plist_file);
    }

    /**
     * Checks if the macOS is updated
     *
     * @return bool true on updated and false if otherwise
     */
    protected function is_mac_os_updated(){
        $plist_file = self::WORKING_DIR."/".self::INFO_PLIST;
        $version    = [];
        $plist      = new PListEditor();
        $plist->readFile($plist_file);
        $versionProperty = $plist->root()->getProperty("Version");
            $version["product"] = $versionProperty->getProperty("product")->value();
            $version["build"]   = $versionProperty->getProperty("build")->value();
        return $version !== $this->get_mac_os_version();
    }

    /**
     * Run scripts
     *
     * @return int Total number of scripts to be run
     */
    protected function run_scripts(){
        $pwd = self::WORKING_DIR."/".self::SCRIPTS_DIR;
        if(file_exists($pwd) === false) mkdir($pwd);
//        exec("ls {$pwd}", $scripts);
        $scripts = Essentials::listDir($pwd, true);
        foreach ($scripts as $script){
            // run scripts
            exec($script);
        }
        return count($scripts);
    }

    /**
     * Installs kexts, backup if existed already
     *
     * @return int total kext
     */
    protected function install_kexts(){
        $pwd = self::WORKING_DIR."/".self::KEXTS_DIR;
        $backup_dir = self::WORKING_DIR."/../Backups";
        $kext_installer = new KextManager($backup_dir);
        if(file_exists($pwd) === false) mkdir($pwd);
//        exec("ls {$pwd}", $kexts);
        $kexts = Essentials::listDir($pwd, true);
        $total_kexts = count($kexts);
        if($total_kexts > 0){
            foreach ($kexts as $kext) $kext_installer->install_kext($kext, false);
            $kext_installer->rebuild_cache();
        }
        return $total_kexts;
    }
}

//date_default_timezone_set('UTC');
//
//define('SUCCESS', 0);
//define('FAILED', 1);
//
//$verb = $argv[1]; // add|remove|list|help|update
//$option = $argv[2]; // kext|script
//$dir = $argv[3]; // [dir]
//
//
///** @var string $name command name */
//$name = 'machelper hackupdater';
///** @var string $wd working directory */
//$wd = '/Users/Shared/.hackupdater';
//switch ($verb){
//    case "add":
//        if(in_array($option, ["kext", "script"]) && $dir != "") {
//            if (!file_exists("{$wd}/{$option}s")) exec("sudo mkdir {$wd}/{$option}s");
//            exec("sudo cp -R {$dir} {$wd}/{$option}s");
//            exit("{$option} added!\n");
//        }else exit("Wrong command. type: {$name} help for help\n");
//        break;
//    case "remove":
//        if(in_array($option, ["kext", "script"]) && $dir != "") {
//            if (file_exists("{$wd}/{$option}s/{$dir}")) exec("sudo rm -R {$wd}/{$option}s/{$dir}");
//            exit("{$dir} removed\n");
//        }else exit("Wrong command. type: {$name} help for help\n");
//        break;
//    case "list":
//        $dir = "{$wd}/{$option}s";
//        if(file_exists($dir)) exit(exec("ls {$dir}")."\n");
//        else exit("\n");
//        break;
//    case "update": // do update
//        // print execution time
//        echo "{$name}: running at ".((new DateTime('NOW'))->format('Y/d/m H:i:s'))."\n";
//
//        if(isUpdated()){
//            // do tasks
//            echo "{$name}: macOS is updated or upgraded.\n";
//            // install kexts
//            /** @var bool $needReboot */
//            $needReboot = (installKexts() > 0) ? true : false;
//            // run scripts
//            runScripts();
//            // keep the updated data
//            file_put_contents($version, getVersion());
//            // now exit or shutdown
//            echo "{$name}: ";
//            if($needReboot){
//                fputs(STDOUT, "Rebooting...");
//                exec('sudo shutdown -r now ');
//            }else{
//                fputs(STDOUT, "Done.");
//                exit(SUCCESS);
//            }
//        }else{
//            exit("{$name}: macOS is not updated or upgraded. Exiting...\n");
//        }
//        break;
//    case "help":
//    default:
//        exit("Usage: ${name} [update|add|remove|list|help] [kext|script] [dir|filename]\n");
//}