<?php
	class EndpointResource extends EndpointController{
		protected function get(){
			$api = $this->getResourceObject("API","id",$this->viewFromArguments("api"));
			$endpoint = $this->getResourceObject("EndpointStatistic","id",$this->viewFromArguments("endpoint"));
			
			$endpointResults = DatabaseAdministrator::execute("SELECT AVG(runtime)/AVG(requests) as averageRuntime, SUM(requests) AS totalRequests ".
								"FROM EndpointStatistic WHERE api = {$api->getId()} AND endpoint = '{$endpoint->getEndpoint()}'");
							
			$startDate = new DateTime($endpointResults[0]['startDate']);
			$today = new DateTime(date("Y-m-d"));
			$daysActive = intval($today->diff($startDate)->format("%a")) + 1;
			
			$endpointStatistic = new EndpointStatistic($api,$endpoint->getEndpoint(),"",$endpointResults[0]['totalRequests']/$daysActive,$endpointResults[0]['dailyRequests']);
			$endpointStatistic->setId($endpoint->getId());
			
			$payload = $this->assemblePayload($endpointStatistic);
			ResponseHandler::retrieved($payload,$endpoint->getURI(),$endpoint->getModified(),$endpoint->getETag());
		}
		
		protected function isConsistent($modifiedDate,$eTag){
			$endpointStatistic = $this->getResourceObject("EndpointStatistic","id",$this->viewFromArguments("endpoint"));			
			return $modifiedDate == $endpointStatistic->getModified() && $eTag == $endpointStatistic->getETag();
		}
	}
?>