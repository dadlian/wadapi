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
		
		protected function getAverageObjects(){
			$endpointParts = explode("?",$this->getEndpoint());
			$objectsResult = DatabaseAdministrator::execute("SELECT AVG(objects) AS averageObjects FROM ".
									"(SELECT COUNT(id) AS objects FROM ObjectStatistic WHERE SUBSTRING_INDEX(endpoint,'?',1) = '{$endpointParts[0]}' GROUP BY endpoint) as objectResult");
			return $objectsResult[0]['averageObjects']?$objectsResult[0]['averageObjects']:0;
		}
		
		protected function getAverageQueries(){
			$endpointParts = explode("?",$this->getEndpoint());
			$queriesResult = DatabaseAdministrator::execute("SELECT AVG(queries) AS averageQueries FROM ".
									"(SELECT COUNT(id) AS queries FROM QueryStatistic WHERE SUBSTRING_INDEX(endpoint,'?',1) = '{$endpointParts[0]}' GROUP BY endpoint) as queryResult");
			return $queriesResult[0]['averageQueries']?$queriesResult[0]['averageQueries']:0;
		}
		
		protected function getAverageCalls(){
			$endpointParts = explode("?",$this->getEndpoint());
			$callsResult = DatabaseAdministrator::execute("SELECT AVG(calls) AS averageObjects FROM ".
									"(SELECT COUNT(id) AS calls FROM CallStatistic WHERE SUBSTRING_INDEX(endpoint,'?',1) = '{$endpointParts[0]}' GROUP BY endpoint) as callResult");
			return $callsResult[0]['averageObjects']?$callsResult[0]['averageObjects']:0;
		}
	}
?>