<?php
	class CallStatistic extends PersistentClass{
		/** @Object(class='API') */
		protected $api;
		
		/** @String */
		protected $endpoint;
		
		/** @String */
		protected $externalCall;
		
		/** @Integer */
		protected $calls;
		
		/** @Float */
		protected $roundtrip;
		
		/** @Integer */
		protected $requests;
		
		protected function getBaseUri(){
			return ((array_key_exists('HTTPS',$_SERVER) && $_SERVER['HTTPS'])?"https://":"http://").SettingsManager::getSetting("install","urlroot");
		}
		
		protected function getURI(){
			$endpointResults = DatabaseAdministrator::execute("SELECT id FROM EndpointStatistic WHERE api = '{$this->getApi()->getId()}' AND endpoint = '{$this->getEndpoint()}'");
			$endpointId = $endpointResults?$endpointResults[0]["id"]:0;
			
			return $this->getBaseUri()."/apis/{$this->getApi()->getId()}/endpoints/$endpointId/calls/{$this->getId()}";
		}
		
		protected function getURITemplate(){
			return $this->getBaseUri()."/apis/{api_id}/endpoints/{endpoint_id}/calls/{call_id}";
		}
		
		protected function getETag(){
			$eTag = $this->getApi()->getETag();
			$eTag .= $this->getEndpoint();
			$eTag .= $this->getExternalCall();
			$eTag .= $this->getCalls();
			$eTag .= $this->getRoundtrip();
			$eTag .= $this->getRequests();
			
			return md5($this->getModified().$eTag);
		}
	}
?>