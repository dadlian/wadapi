<?php
	//Initialise getallheaders() for NGinx
	if (!function_exists('getallheaders')) {
		function getallheaders() {
			$headers = [];
			foreach ($_SERVER as $name => $value) {
				if (substr($name, 0, 5) == 'HTTP_') {
				$headers[str_replace(' ', '-', ucwords(strtolower(str_replace('_', ' ', substr($name, 5)))))] = $value;
				}
			}
			
			return $headers;
		}
	}

	$settings = dirname(__FILE__)."/api/settings.xml";
        require_once(dirname(__FILE__)."/../../core/includes.inc");
        
	//Dispatch Request
        $activeDispatcher = SettingsManager::getSetting("install","dispatcher");
        $activeDispatcher::dispatchRequest();
?>