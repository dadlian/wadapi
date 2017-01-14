<?php
	class EndpointStatistic extends Resource{
		/** @Object(class='API') */
		protected $api;
		
		/** @String */
		protected $endpoint;
		
		/** @String */
		protected $date;
		
		/** @Float */
		protected $runtime;
		
		/** @Integer */
		protected $requests;
		
		protected function getURI(){
			return $this->getBaseUri()."/apis/{$this->getAPI()->getId()}/endpoints/{$this->getId()}";
		}
		
		protected function getURITemplate(){
			return $this->getBaseUri()."/apis/{api_id}/endpoints/{endpoint_id}";
		}
		
		protected function getETag(){
			$eTag = $this->getAPI()->getETag();
			$eTag .= $this->getEndpoint();
			$eTag .= $this->getDate();
			$eTag .= $this->getRequests();
			$eTag .= $this->getRuntime();
			
			return md5($this->getModified().$eTag);
		}
	}
?>