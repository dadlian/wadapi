<?php
	class QueryStatistic extends PersistentClass{
		/** @Object(class='API') */
		protected $api;
		
		/** @String */
		protected $endpoint;
		
		/** @String */
		protected $query;
		
		/** @Integer */
		protected $executions;
		
		/** @Float */
		protected $runtime;
		
		/** @Integer */
		protected $requests;
		
		protected function getBaseUri(){
			return ((array_key_exists('HTTPS',$_SERVER) && $_SERVER['HTTPS'])?"https://":"http://").SettingsManager::getSetting("install","urlroot");
		}
		
		protected function getURI(){
			$endpointParts = explode("?",$this->getEndpoint());
			$endpointResults = DatabaseAdministrator::execute("SELECT MIN(id) as endpointId FROM EndpointStatistic WHERE api = '{$this->getApi()->getId()}' AND SUBSTRING_INDEX(endpoint,'?',1) = '{$endpointParts[0]}'");
			$endpointId = $endpointResults?$endpointResults[0]["endpointId"]:0;
			
			$endpointResults = DatabaseAdministrator::execute("SELECT MAX(id) as variantId FROM EndpointStatistic WHERE api = '{$this->getApi()->getId()}' AND endpoint = '{$this->getEndpoint()}'");
			$variantId = $endpointResults?$endpointResults[0]["variantId"]:0;
			
			return $this->getBaseUri()."/apis/{$this->getApi()->getId()}/endpoints/$endpointId/variants/$variantId/queries/{$this->getId()}";
		}
		
		protected function getURITemplate(){
			return $this->getBaseUri()."/apis/{api_id}/endpoints/{endpoint_id}/variants/{variant_id}/queries/{query_id}";
		}
		
		protected function getETag(){
			$eTag = $this->getApi()->getETag();
			$eTag .= $this->getEndpoint();
			$eTag .= $this->getQuery();
			$eTag .= $this->getExecutions();
			$eTag .= $this->getRuntime();
			$eTag .= $this->getRequests();
			
			return md5($this->getModified().$eTag);
		}
	}
?>