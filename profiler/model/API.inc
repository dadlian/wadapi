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
			$requestResult = DatabaseAdministrator::execute("SELECT MIN(date) as startDate, SUM(requests) as totalRequests FROM EndpointStatistic WHERE api = ?",$this->getId());
			
			$startDate = new DateTime($requestResult[0]['startDate']);
			$today = new DateTime(date("Y-m-d"));
			$daysActive = intval($today->diff($startDate)->format("%a")) + 1;
			return round($requestResult[0]["startDate"]?$requestResult[0]["totalRequests"]/$daysActive:0,3);
		}
		
		protected function getTotalEndpoints(){
			$requestResult = DatabaseAdministrator::execute("SELECT COUNT(DISTINCT(SUBSTRING_INDEX(endpoint,'?',1))) as endpoints FROM EndpointStatistic WHERE api = {$this->getId()}");
			return $requestResult?$requestResult[0]["endpoints"]:0;
		}
	}
?>