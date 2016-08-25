<?php
	abstract class Resource extends PersistentClass{
		abstract protected function getURI();
		abstract protected function getURITemplate();
		abstract protected function getETag();
		
		protected function getBaseUri(){
			return ((array_key_exists('HTTPS',$_SERVER) && $_SERVER['HTTPS'])?"https://":"http://").SettingsManager::getSetting("install","urlroot");
		}
	}
?>