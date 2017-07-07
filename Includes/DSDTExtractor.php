<?php
/**
 * Created by PhpStorm.
 * User: muntashir
 * Date: 1/25/17
 * Time: 7:38 PM
 */

namespace MacHelper;

require_once __DIR__."/Essentials.php";
require_once __DIR__."/EFIMounter.php";
require_once __DIR__."/../Libraries/BitBucketDownloader/BitBucketDownloader.php";
require_once __DIR__."/../Libraries/PListEditor/PListEditor.php";

use \BitBucketDownloader;
use \PListEditor\PListEditor;

/**
 * Class DSDTExtractor
 *
 * NOTE: This uses 'patchmatic' if not available at EFI/CLOVER/ACPI/Origin to extract ACPI tables,
 *  which isn't recommended for day to day use
 *  but only for debugging purpose in Hackintosh machines
 *  because nowadays many defaults or built-in patches are injected
 *  through Clover (whether you want it or not). For the same reason,
 *  this method should work fine on real Macs.
 *
 *
 * It uses iasl tool to disassemble, however as this tool is often error-prone,
 *  RehabMan's recommendation to use refs.txt is also implemented by default.
 *
 * TODO: Some common patches may also be applied in near future.
 *
 * FIXME: add support for Legacy Clover
 *
 * @package MacHelper
 */

class DSDTExtractor
{
    /**
     * Predefined (Possibility) Constants
     */
    const PATCHMATIC_IMPOSSIBLE = 1;
    const PATCHMATIC_POSSIBLE   = 2;
    const CLOVER_ORIGIN_METHOD  = 3;
    const EXTRACTION_IMPOSSIBLE = 4;
    const FORCE_EXTRACTION      = 5;
    const PATCHMATIC            = 6;

    /**
     * @var string $target
     */
    protected $target;
    /**
     * @var bool $force true = extract with a warning, false = stop with a warning
     */
    protected $force;

    /**
     * @var int $possibility One of the possibility constants
     */
    protected $possibility;

    /**
     * DSDTExtractor constructor.
     *
     * @param string $target The output directory, default ~/Desktop/ACPI (Destination must exists, otherwise default is used)
     * @param bool   $force  Use force extraction using Patchmatic tool, good for debugging
     */
    public function __construct($target = null, $force = false){
        $this->target = ($target == null OR !file_exists($target)) ? exec('echo /Users/$USER/Desktop/ACPI') : $target;
        $this->force  = $force;
        Essentials::createRoot();
    }

    /**
     * Checks whether this method will work or not
     *
     * - If available at EFI/CLOVER/ACPI/origin
     * - Whether already patched at EFI/CLOVER/ACPI/patched
     * - Whether DSDT related options are used at config.plist
     *
     *
     * @return int Possibility constants based on the criteria
     */
    public function check(){
        // Check if user wants to force extraction, handy for debugging purpose
        if($this->force) return self::FORCE_EXTRACTION;

        // Mount the EFI partition of the root device
        (new EFIMounter())->mountAutomatically();
        $root = EFIMounter::DEFAULT_EFI_DIR . '/EFI/CLOVER';
        // Check for existence of the EFI/CLOVER (which doesn't exist in the real Macs or virtual machines or Legacy)
        if(!is_dir($root)) return self::PATCHMATIC_POSSIBLE;

        $config  = $root . '/config.plist';
        $origin  = $root . '/ACPI/origin';
        $patched = $root . '/ACPI/patched/';
        // Check for existence of ACPI tables at ACPI/origin
        if(is_dir($origin) AND count(scandir($origin)) > 2) // Count is 2, because scandir() also returns . and ..
            return self::CLOVER_ORIGIN_METHOD;
        // Check for the existence of DSDT or SSDTs
        if(is_dir($patched) AND count(scandir($patched)) > 2) // Count is 2, because scandir() also returns . and ..
            return self::PATCHMATIC_IMPOSSIBLE;
        // Check for DSDT patches in the config.plist
        if(file_exists($config)){
            $plist = new PListEditor();
            $plist->readFile($config);
            $root = $plist->root();
            if($root != null AND $root->hasProperties(true)){
                $acpi = $root->getProperty('ACPI');
                if($acpi != null AND $acpi->hasProperties()){
                    $dsdt = $acpi->getProperty('DSDT');
                    if($dsdt != null AND $dsdt->hasProperties()){
                        return self::PATCHMATIC_IMPOSSIBLE;
                    }
                }
            }
        }
        return self::PATCHMATIC_POSSIBLE;
    }

    /**
     * Extract using patchmatic (ie. IOReg) or just copy DSDT and SSDTs to the destination folder
     *
     * @param null|string $target path to save the ACPI files, if null default path will be used (only for debugging purpose)
     * @return int Extraction constants
     */
    public function extract($target = null){
        // Check for possibility at first
        $this->possibility = $this->check();
        if($target === null) $target = $this->target;
        // Check for the existence of the destination
        if(!file_exists($target)) exec("mkdir \"{$this->target}\"");
        if(!file_exists($target)) return self::EXTRACTION_IMPOSSIBLE;
        // Copy DSDT/SSDTs to the destination folder
        if($this->possibility == self::CLOVER_ORIGIN_METHOD){
            // Mount the EFI partition of the root device
            (new EFIMounter())->mountAutomatically();
            $patched = EFIMounter::DEFAULT_EFI_DIR . '/EFI/CLOVER/ACPI/origin';
            // Get DSDT/SSDTs
            $files   = file_exists($patched) ? array_diff(scandir($patched), ['.', '..']) : [];
            $files   = preg_grep('/^(DSDT|SSDT)\-?(\d+)?x?\.aml$/', $files);
            // Copy them all!
            if(count($files) > 0){
                foreach ($files as $file){
                    copy(($patched . '/' . $file), ($target . '/' . $file));
                }
                return self::CLOVER_ORIGIN_METHOD;
            }
            return self::EXTRACTION_IMPOSSIBLE;
        }elseif($this->possibility === self::PATCHMATIC_IMPOSSIBLE OR $this->possibility === self::EXTRACTION_IMPOSSIBLE) return self::EXTRACTION_IMPOSSIBLE;
        // Skip downloading patchmatic if it's there (ie. in the destination folder) already
        if(file_exists(Essentials::TMP_DIR . '/patchmatic')) goto extraction;
        // Download patchmatic
        /**
         * NOTE: This is a modified version of patchmatic. The original version can be downloaded like this:
         * <code>
         * $patchmatic = (new BitBucketDownloader('RehabMan', 'os-x-maciasl-patchmatic'))->getFile('patchmatic', Essentials::TMP_DIR);
         * </code>
         * But this isn't recommended as it's not supported. You need to implement it yourself.
         * Basically I modified the patchmatic myself to include a target with -extract.
         * (I've made a pull request which if RehabMan accepts, the above
         */
        $patchmatic = Essentials::download('https://github.com/MuntashirAkon/OS-X-MaciASL-patchmatic/releases/download/patchmatic_modified/patchmatic.zip', Essentials::TMP_DIR . '/patchmatic.zip');
        // Check if the download was a success
        if($patchmatic === false) return self::EXTRACTION_IMPOSSIBLE;
        // Extract patchmatic.zip
        Essentials::unzip(Essentials::TMP_DIR . '/patchmatic.zip', Essentials::TMP_DIR);
        //Essentials::unzip($patchmatic['file'], Essentials::TMP_DIR); // When using BitBucketDownloader
        // Extract DSDT/SSDTs
        extraction:
        // Make patchmatic executable
        passthru('chmod +x '. Essentials::TMP_DIR . '/patchmatic');
        passthru(Essentials::TMP_DIR . '/patchmatic -extract "'.$target.'"', $return);
        // Success or failure?
        if($return != Essentials::EXIT_SUCCESS) return self::EXTRACTION_IMPOSSIBLE;
        return self::PATCHMATIC;
    }

    /**
     * Disassembles the DSDT/SSDTs (ie. make dsl file from aml files)
     *
     * NOTE: the destination must contain the extracted DSDT/SSDTs to get this to work
     *
     * @param string|null $source path to the DSDT/SSDTs, if null is given $target will be used
     * @param string|null $target path to disassemble the ACPI files, if null default path will be used
     * @return bool
     */
    public function disassemble($source = null, $target = null){
        if($target === null) $target = $this->target;
        if($source === null) $source = $this->target;
        // Check for the existence of the destination
        if(!file_exists($target)) passthru("mkdir \"{$this->target}\"");
        if(!file_exists($target) OR !file_exists($source)) return false;
        // Check if the destination folder contains the DSDT/SSDTs
        if(count(preg_grep('/^(DSDT|SSDT)\-?(\d+)?x?\.aml$/', scandir($source))) <= 0) return false;
        // Skip downloading iasl if it's there (ie. in the destination folder) already
        if(file_exists(Essentials::TMP_DIR . '/iasl')) goto disassemble;
        // Download iasl
        $iasl = (new BitBucketDownloader('RehabMan', 'acpica'))->getFile('iasl', Essentials::TMP_DIR);
        // Check if the download was a success
        if($iasl === false) return false;
        /**
         * TODO AND NOTE: This way is more messy and weird than simply including an output directory at iasl
         *  which is sadly not implemented by default in iasl by RehabMan, maybe in future I'll implement this.
         * Also, an additional mechanism is needed to be added to prevent stdout, but writing to the debug log.
         */
        // Extract iasl.zip
        Essentials::unzip($iasl['file'], Essentials::TMP_DIR);
        // Disassemble DSDT/SSDTs
        disassemble:
        // Make iasl executable
        passthru('chmod +x '. Essentials::TMP_DIR . '/iasl');
        // create refs.txt to get the most of iasl
        file_put_contents(Essentials::TMP_DIR . "/refs.txt", "External(_SB_.PCI0.PEG0.PEGP.SGPO, MethodObj, 2)\nExternal(_SB_.PCI0.LPCB.H_EC.ECWT, MethodObj, 2)\nExternal(_SB_.PCI0.LPCB.H_EC.ECRD, MethodObj, 1)\nExternal(_GPE.MMTB, MethodObj, 0)");
        // Disassemble
        exec(Essentials::TMP_DIR . '/iasl -da -dl -bf -fe ' . Essentials::TMP_DIR . "/refs.txt \"{$source}\"/*.aml", $return);
        // Move if target != source
        if($target != $source AND $return == Essentials::EXIT_SUCCESS){
            exec("mv \"{$source}\"/*.dsl \"{$target}\"");
        }
        // Success or failure?
        if($return != Essentials::EXIT_SUCCESS) return false;
        return true;
    }
}