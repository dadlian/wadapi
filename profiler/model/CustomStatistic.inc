<?php
	class CustomStatistic extends PersistentClass{
		/** @Object(class='API') */
		protected $api;
		
		/** @String */
		protected $endpoint;
		
		/** @String */
		protected $customKey;
		
		/** @Integer */
		protected $runs;
		
		/** @Float */
		protected $duration;
		
		/** @Integer */
		protected $requests;
		
		protected function getBaseUri(){
			return ((array_key_exists('HTTPS',$_SERVER) && $_SERVER['HTTPS'])?"https://":"http://").SettingsManager::getSetting("install","urlroot");
		}
		
		protected function getURI(){
			$endpointResults = DatabaseAdministrator::execute("SELECT id FROM EndpointStatistic WHERE api = '{$this->getApi()->getId()}' AND endpoint = '{$this->getEndpoint()}'");
			$endpointId = $endpointResults?$endpointResults[0]["id"]:0;
			
			return $this->getBaseUri()."/apis/{$this->getApi()->getId()}/endpoints/$endpointId/custom/{$this->getId()}";
		}
		
		protected function getURITemplate(){
			return $this->getBaseUri()."/apis/{api_id}/endpoints/{endpoint_id}/custom/{custom_id}";
		}
		
		protected function getETag(){
			$eTag = $this->getApi()->getETag();
			$eTag .= $this->getEndpoint();
			$eTag .= $this->getCustomKey();
			$eTag .= $this->getRuns();
			$eTag .= $this->getDuration();
			$eTag .= $this->getRequests();
			
			return md5($this->getModified().$eTag);
		}
	}
?>