<?php
	class API extends Resource{
		/** @String */
		protected $name;
		
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
			$requestResult = DatabaseAdministrator::execute("SELECT AVG(totalRequests) AS dailyRequests FROM (SELECT SUM(requests) as totalRequests FROM EndpointStatistic WHERE api = ? GROUP BY date) dailySummary;",$this->getId());
			return round($requestResult[0]["dailyRequests"]?$requestResult[0]["dailyRequests"]:0,3);
		}
		
		protected function getTotalEndpoints(){
			$requestResult = DatabaseAdministrator::execute("SELECT COUNT(DISTINCT(endpoint)) as totalEndpoints FROM EndpointStatistic WHERE api = ?",$this->getId());
			return $requestResult?$requestResult[0]["totalEndpoints"]:0;
		}
	}
?>