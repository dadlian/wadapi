<?php
	class VariantResource extends VariantController{
		protected function get(){
			$api = $this->getResourceObject("API","id",$this->viewFromArguments("api"));
			$variant = $this->getResourceObject("EndpointStatistic","id",$this->viewFromArguments("variant"));
			
			$variantResult = DatabaseAdministrator::execute("SELECT MAX(id) AS id, REPLACE(endpoint,SUBSTRING_INDEX(endpoint,'?',1),'') as queryString,".
								"MIN(date) as startDate, AVG(runtime)/AVG(requests) as averageRuntime, SUM(requests) AS totalRequests ".
								"FROM EndpointStatistic WHERE api = {$api->getId()} AND endpoint = '{$variant->getEndpoint()}'");
			$variantResult = $variantResult[0];
				
			$startDate = new DateTime($variantResult['startDate']);
			$today = new DateTime(date("Y-m-d"));
			$daysActive = intval($today->diff($startDate)->format("%a")) + 1;
			
			$variantResult['queryString'] = $variantResult['queryString']?$variantResult['queryString']:"None";
			$variantResult['averageRequests'] = $variantResult['totalRequests']/$daysActive;
			
			$parameters = Array();
			foreach(DatabaseAdministrator::execute("SELECT REPLACE(endpoint,SUBSTRING_INDEX(endpoint,'?',1),'') as queryString FROM EndpointStatistic ".
								"WHERE api = {$api->getId()} AND SUBSTRING_INDEX(endpoint,'?',1) = SUBSTRING_INDEX('{$variant->getEndpoint()}','?',1) ".
								"GROUP BY 1") as $row){
				foreach(explode('&',substr($row['queryString'],1)) as $query){
					$queryParts = explode('=',$query);
					if($queryParts[0]){
						if(!in_array($queryParts[0],$parameters)){
							$parameters[] = $queryParts[0];
						}
					}
				}
			}
			sort($parameters);
			
			$availableParameters = Array();
			foreach(explode('&',substr($variantResult['queryString'],1)) as $query){
				$queryParts = explode('=',$query);
				$availableParameters[$queryParts[0]] = $queryParts[1];
			}
			
			$variantResult['parameters'] = Array();
			foreach($parameters as $parameter){
				$variantResult['parameters'][$parameter] = $availableParameters[$parameter]?$availableParameters[$parameter]:"N/A";
			}
				
			$payload = $this->assemblePayload($variantResult);
			ResponseHandler::retrieved($payload,$payload["self"],$variant->getModified(),$variant->getETag());
		}
		
		protected function isConsistent($modifiedDate,$eTag){
			return true;
		}
	}
?>