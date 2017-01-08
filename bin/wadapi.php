<?php
	$settings = dirname(__FILE__)."/api/settings.xml";
        require_once(dirname(__FILE__)."/../../core/includes.inc");
        
	//Dispatch Request
        $activeDispatcher = SettingsManager::getSetting("install","dispatcher");
        $activeDispatcher::dispatchRequest();
?>