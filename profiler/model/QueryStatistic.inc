<?php
	class QueryStatistic extends Resource{
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
		
		protected function getURI(){
			$endpointResults = DatabaseAdmininstrator::execute("SELECT id FROM EndpointStatistic WHERE api = '?' AND endpoint = '?'",
										$this->getAPI()->getId(),$this->getEndpoint());
			$endpointId = $endpointResults?$endpointResults[0]["id"]:0;
			
			return $this->getBaseUri()."/apis/{$this->getAPI()->getId()}/endpoints/$endpointId/queries/{$this->getId()}";
		}
		
		protected function getURITemplate(){
			return $this->getBaseUri()."/apis/{api_id}/endpoints/{endpoint_id}/queries/{query_id}";
		}
		
		protected function getETag(){
			$eTag = $this->getAPI()->getETag();
			$eTag .= $this->getEndpoint();
			$eTag .= $this->getQuery();
			$eTag .= $this->getExecutions();
			$eTag .= $this->getRuntime();
			
			return md5($this->getModified().$eTag);
		}
	}
?>