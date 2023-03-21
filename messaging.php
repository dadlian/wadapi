<?php
  require 'vendor/autoload.php';

  use Wadapi\Utility\FileUtility;
  use Wadapi\Messaging\SubscriptionManager;

	define('SCRIPT_START', microtime(true));
  define('PROJECT_PATH', dirname(__FILE__));
  define('CONFIG', PROJECT_PATH."/conf");
  define('MAPPINGS', PROJECT_PATH."/mappings");

	//Include User Created Files
	FileUtility::require_all(PROJECT_PATH."/src/domain");
  FileUtility::require_all(PROJECT_PATH."/src/services");
  FileUtility::require_all(PROJECT_PATH."/src/subscribers");

  SubscriptionManager::manage();
?>
