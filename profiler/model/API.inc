<?php
	class API extends Resource{
		/** @String */
		protected $root;
		
		protected function getURI(){
			return $this->getBaseUri()."/apis/{$this->getId()}";
		}
		
		protected function getURITemplate(){
			return $this->getBaseUri()."/apis/{profile_id}";
		}
		
		protected function getETag(){
			$eTag = $this->getRoot();
			return md5($this->getModified().$eTag);
		}
		
		protected function getDailyRequests(){
			$requestResult = DatabaseAdministrator::execute("SELECT AVG(requests) as dailyRequests FROM EndpointStatistic WHERE api = ?",$this->getId());
			return round($requestResult[0]["dailyRequests"]?$requestResult[0]["dailyRequests"]:0,3);
		}
		
		protected function getTotalEndpoints(){
			$requestResult = DatabaseAdministrator::execute("SELECT DISTINCT(endpoint) as totalEndpoints FROM EndpointStatistic WHERE api = ?",$this->getId());
			return round($requestResult?$requestResult[0]["totalEndpoints"]:0,3);
		}
	}
?>