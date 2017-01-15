<?php
	class ObjectStatistic extends PersistentClass{
		/** @Object(class='API') */
		protected $api;
		
		/** @String */
		protected $endpoint;
		
		/** @String */
		protected $class;
		
		/** @Integer */
		protected $instances;
		
		/** @Float */
		protected $loadTime;
		
		protected function getBaseUri(){
			return ((array_key_exists('HTTPS',$_SERVER) && $_SERVER['HTTPS'])?"https://":"http://").SettingsManager::getSetting("install","urlroot");
		}
		
		protected function getURI(){
			$endpointResults = DatabaseAdministrator::execute("SELECT id FROM EndpointStatistic WHERE api = '?' AND endpoint = '?'",
										$this->getApi()->getId(),$this->getEndpoint());
			$endpointId = $endpointResults?$endpointResults[0]["id"]:0;
			
			return $this->getBaseUri()."/apis/{$this->getApi()->getId()}/endpoints/$endpointId/objects/{$this->getId()}";
		}
		
		protected function getURITemplate(){
			return $this->getBaseUri()."/apis/{api_id}/endpoints/{endpoint_id}/objects/{object_id}";
		}
		
		protected function getETag(){
			$eTag = $this->getApi()->getETag();
			$eTag .= $this->getEndpoint();
			$eTag .= $this->getClass();
			$eTag .= $this->getInstances();
			$eTag .= $this->getLoadTime();
			
			return md5($this->getModified().$eTag);
		}
	}
?>