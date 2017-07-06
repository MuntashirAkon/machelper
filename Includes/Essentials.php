<?php
/**
 * Created by PhpStorm.
 * User: muntashir
 * Date: 12/28/16
 * Time: 3:46 PM
 */

namespace MacHelper;

/**
 * Class Essentials
 *
 * This class consists of some essential methods and constants.php
 * that are used throughout the project. These methods include:
 * - createRoot
 * - unzip
 * - list_dir
 * - download
 *
 * @package MacHelper
 */
class Essentials
{
    /**
     * Temporary as well as working directory for the entire program
     */
    const TMP_DIR = "/tmp/machelper";

    /*
     * Terminal exit constants.php (they are different from the regular success and failure)
     */
    /**
     * In terminal, exit status is zero on success
     */
    const EXIT_SUCCESS = 0;
    /**
     * In terminal, exit status in non-zero on failure, 1 is used here as default
     */
    const EXIT_FAILURE = 1;

    /**
     * Creates root if not existed already
     *
     * @return void
     */
    static function createRoot(){
        if(file_exists(self::TMP_DIR) === false) mkdir(self::TMP_DIR);
    }

    /**
     *
     * Unzips a zipped file
     *
     * @param string $file   The zip file
     * @param string $target Target directory
     * @return bool true on success and false on failure
     */
    static function unzip($file, $target){
        $return_value = false;
        $zip = new \ZipArchive();
        if($zip->open($file)){
            $return_value  = $zip->extractTo($target);
            $zip->close();
        }
        return $return_value;
    }

    /**
     * Lists all file names and directory names under the target directory
     *
     * NOTE: This listing excludes . and ..
     *
     * @param string $directory The target directory
     * @param bool   $full_path Whether to return short path (ie. only file names or last directory names) or full path
     * @return array a list of file names and directory names
     */
    static function listDir($directory, $full_path = false){
        if(!$full_path) return array_diff(scandir($directory), ['.', '..']);
        else{
            // remove trailing slash if any
            $directory = preg_replace('/\/$/', '', $directory);
            $files = array_diff(scandir($directory), ['.', '..']);
            foreach ($files as &$file){
                $file = $directory . '/' . $file;
            }
            return $files;
        }
    }

    /**
     * Downloads file using cURL
     *
     * NOTE: this is done to support PHP >= 5.0.0
     *
     * @param string $url   File url
     * @param string $target File saving location INCLUDING the filename
     * @return bool true on success and false on failure
     */
    static function download($url, $target){
        set_time_limit(0);
        $curl = curl_init();
        $target = fopen($target, "w+");
        $options = [
            CURLOPT_FILE        => $target,
            CURLOPT_URL         => $url,
            CURLOPT_FOLLOWLOCATION => true
        ];
        curl_setopt_array($curl, $options);
        $result = curl_exec($curl);
        curl_close($curl);
        fclose($target);
        return ($result !== false) ? true : false;
    }
}