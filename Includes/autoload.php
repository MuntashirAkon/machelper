<?php
/**
 * Created by PhpStorm.
 * User: muntashir
 * Date: 6/24/17
 * Time: 8:23 PM
 */

spl_autoload_register(function($class){
    $file = __DIR__ . '/' . basename(str_replace('\\', '/', $class)) . '.php';
    if(file_exists($file)) /** @noinspection PhpIncludeInspection */
        require_once $file;
});