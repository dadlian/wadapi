<?php
	abstract class VariantController extends PagedResourceController{
		protected function isInvalid(){
			$invalidArguments = array();
			return $invalidArguments;
		}
		
		protected function getInvalidQueryParameters(){
			$invalidQueryParameters = array();
			
			$sort = RequestHandler::getQueryParameter("sort");
			$order = RequestHandler::getQueryParameter("order");
			
			if($sort && !in_array($sort,array("query","requests","runtime"))){
				$invalidQueryParameters[] = "sort";
			}
			
			if($order && !in_array($order,array("asc","desc"))){
				$invalidQueryParameters[] = "order";
			}
			
			return $invalidQueryParameters;
		}
		
		protected function getRecordCount(){
			$api = $this->getResourceObject("API","id",$this->viewFromArguments("api"));
			$endpoint = $this->getResourceObject("EndpointStatistic","id",$this->viewFromArguments("endpoint"))->getEndpoint();
			
			$countResult = DatabaseAdministrator::execute("SELECT COUNT(DISTINCT(endpoint)) as endpoints FROM EndpointStatistic ".
									"WHERE api = {$api->getId()} AND ".
									"SUBSTRING_INDEX(endpoint,'?',1) = SUBSTRING_INDEX('$endpoint','?',1)");
			$count = $countResult[0]["endpoints"];
			
			return $count;
		}
		
		protected function buildPageEntries($start,$count){
			$api = $this->getResourceObject("API","id",$this->viewFromArguments("api"));
			$endpoint = $this->getResourceObject("EndpointStatistic","id",$this->viewFromArguments("endpoint"))->getEndpoint();
			
			$sort = RequestHandler::getQueryParameter("sort");
			if($sort == "requests"){
				$sort = "dailyRequests";
			}else if($sort == "runtimes"){
				$sort = "averageRuntime";
			}else{
				$sort = "REPLACE(endpoint,SUBSTRING_INDEX(endpoint,'?',1),'')";
			}
			
			$order = RequestHandler::getQueryParameter("order");
			$order = $order?$order:"asc";
		
			$variantStatistics = array();
			$parameters = array();
			
			foreach(DatabaseAdministrator::execute("SELECT MAX(id) AS id, REPLACE(endpoint,SUBSTRING_INDEX(endpoint,'?',1),'') as queryString,".
								"MIN(date) as startDate, AVG(runtime)/AVG(requests) as averageRuntime, SUM(requests) AS totalRequests ".
								"FROM EndpointStatistic WHERE api = {$api->getId()} AND SUBSTRING_INDEX(endpoint,'?',1) = SUBSTRING_INDEX('$endpoint','?',1) ".
								"GROUP BY 2 ORDER BY $sort $order") as $row){
				
				$startDate = new DateTime($row['startDate']);
				$today = new DateTime(date("Y-m-d"));
				$daysActive = intval($today->diff($startDate)->format("%a")) + 1;
				
				foreach(explode('&',substr($row['queryString'],1)) as $query){
					$queryParts = explode('=',$query);
					if($queryParts[0]){
						if(!in_array($queryParts[0],$parameters)){
							$parameters[] = $queryParts[0];
						}
					}
				}
				
				$row['queryString'] = $row['queryString']?$row['queryString']:"No Query";
				$row['averageRequests'] = $row['totalRequests']/$daysActive;
				
				$objectsResult = DatabaseAdministrator::execute("SELECT COUNT(id) AS averageObjects FROM ObjectStatistic WHERE endpoint = '$endpoint'");
				$row['requestObjects'] = $objectsResult[0]['averageObjects']?$objectsResult[0]['averageObjects']:0;
				
				$queriesResult = DatabaseAdministrator::execute("SELECT COUNT(id) AS averageQueries FROM QueryStatistic WHERE endpoint = '$endpoint'");
				$row['requestQueries'] = $queriesResult[0]['averageQueries']?$queriesResult[0]['averageQueries']:0;
				
				$callsResult = DatabaseAdministrator::execute("SELECT COUNT(id) AS averageCalls FROM CallStatistic WHERE endpoint = '$endpoint'");
				$row['requestCalls'] = $callsResult[0]['averageCalls']?$callsResult[0]['averageCalls']:0;
				
				$variantStatistics[] = $row;
			}
			sort($parameters);
			
			$payload = array();
			foreach($variantStatistics as $variantStatistic){
				$availableParameters = Array();
				foreach(explode('&',substr($variantStatistic['queryString'],1)) as $query){
					$queryParts = explode('=',$query);
					if(sizeof($queryParts) > 1){
						$availableParameters[$queryParts[0]] = $queryParts[1];
					}
				}
				
				$variantStatistic['parameters'] = Array();
				foreach($parameters as $parameter){
					$variantStatistic['parameters'][$parameter] = array_key_exists($parameter,$availableParameters)?$availableParameters[$parameter]:"N/A";
				}
				
				$payload[] = $this->assemblePayload($variantStatistic);
			}
			
			return $payload;
		}
		
		protected function assemblePayload($variantStatistic){
			$endpoint = $this->getResourceObject("EndpointStatistic","id",$this->viewFromArguments("endpoint"));
			$uri = "{$endpoint->getURI()}/variants/{$variantStatistic['id']}";
			
			$payload = array(
				"self"=>$uri,
				"query-string"=>$variantStatistic['queryString'],
				"parameters"=>$variantStatistic['parameters'],
				"average-runtime"=>floatval(sprintf("%.4f",$variantStatistic['averageRuntime'])),
				"daily-requests"=>floatval(sprintf("%.2f",$variantStatistic['averageRequests'])),
				"request-objects"=>floatval(sprintf("%.2f",$variantStatistic['requestObjects'])),
				"request-queries"=>floatval(sprintf("%.2f",$variantStatistic['requestQueries'])),
				"request-calls"=>floatval(sprintf("%.2f",$variantStatistic['requestCalls'])),
				"runtimes"=>"$uri/runtimes",
				"requests"=>"$uri/requests",
				"objects"=>"$uri/objects",
				"queries"=>"$uri/queries",
				"calls"=>"$uri/calls",
				"custom"=>"$uri/custom"
			);
			
			return $payload;
		}
	}
?>