<?php
	$settings = dirname(__FILE__)."/settings.xml";
        require_once(dirname(__FILE__)."/../includes.inc");
	
	//Dispatch Request
        $activeDispatcher = SettingsManager::getSetting("install","dispatcher");
        $activeDispatcher::dispatchRequest();
?>