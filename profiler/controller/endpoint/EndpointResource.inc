<?php
	class EndpointResource extends EndpointController{
		protected function get(){
			$api = $this->getResourceObject("API","id",$this->viewFromArguments("api"));
			$endpoint = $this->getResourceObject("EndpointStatistic","id",$this->viewFromArguments("endpoint"));
			
			$endpointResults = DatabaseAdministrator::execute("SELECT AVG(runtime)/AVG(requests) as averageRuntime, AVG(requests) AS dailyRequests ".
								"FROM EndpointStatistic WHERE api = ? AND endpoint = '?'",$api->getId(),$endpoint);
								
			$endpointStatistic = new EndpointStatistic($api,$endpoint->getEndpoint(),"",$endpointResults['averageRuntime'],$endpointResults['dailyRequests']);
			$endpointStatistic->setId($endpoint->getId());
			
			$payload = $this->assemblePayload($endpoint);
			ResponseHandler::retrieved($payload,$endpoint->getURI(),$endpoint->getModified(),md5($endpoint->getETag()));
		}
		
		protected function isConsistent($modifiedDate,$eTag){
			$endpointStatistic = $this->getResourceObject("EndpointStatistic","id",$this->viewFromArguments("endpoint"));			
			return $modifiedDate == $endpointStatistic->getModified() && $eTag == md5($endpointStatistic->getETag());
		}
	}
?>