<?php
	class EndpointStatistic extends Resource{
		/** @Object(class='API') */
		protected $api;
		
		/** @WadapiString */
		protected $endpoint;
		
		/** @WadapiString */
		protected $date;
		
		/** @WadapiFloat */
		protected $runtime;
		
		/** @WadapiFloat */
		protected $requests;
		
		protected function getVariants(){
			$variantsResult = DatabaseAdministrator::execute("SELECT COUNT(DISTINCT(endpoint)) as endpoints FROM EndpointStatistic WHERE api = '{$this->getApi()->getId()}' AND SUBSTRING_INDEX(endpoint,'?',1) = SUBSTRING_INDEX('{$this->getEndpoint()}','?',1)");
			return $variantsResult[0]['endpoints']?$variantsResult[0]['endpoints']:1;
		}
		
		protected function getURI(){
			return $this->getBaseUri()."/apis/{$this->getApi()->getId()}/endpoints/{$this->getId()}";
		}
		
		protected function getURITemplate(){
			return $this->getBaseUri()."/apis/{api_id}/endpoints/{endpoint_id}";
		}
		
		protected function getETag(){
			$eTag = $this->getApi()->getETag();
			$eTag .= $this->getEndpoint();
			$eTag .= $this->getDate();
			$eTag .= $this->getRequests();
			$eTag .= $this->getRuntime();
			
			return md5($this->getModified().$eTag);
		}
	}
?>