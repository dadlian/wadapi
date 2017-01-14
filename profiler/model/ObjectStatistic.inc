<?php
	class ObjectStatistic extends Resource{
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
		
		protected function getURI(){
			$endpointResults = DatabaseAdmininstrator::execute("SELECT id FROM EndpointStatistic WHERE api = '?' AND endpoint = '?'",
										$this->getAPI()->getId(),$this->getEndpoint());
			$endpointId = $endpointResults?$endpointResults[0]["id"]:0;
			
			return $this->getBaseUri()."/apis/{$this->getAPI()->getId()}/endpoints/$endpointId/objects/{$this->getId()}";
		}
		
		protected function getURITemplate(){
			return $this->getBaseUri()."/apis/{api_id}/endpoints/{endpoint_id}/objects/{object_id}";
		}
		
		protected function getETag(){
			$eTag = $this->getAPI()->getETag();
			$eTag .= $this->getEndpoint();
			$eTag .= $this->getClass();
			$eTag .= $this->getInstances();
			$eTag .= $this->getLoadTime();
			
			return md5($this->getModified().$eTag);
		}
	}
?>