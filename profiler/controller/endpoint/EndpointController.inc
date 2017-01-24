<?php
	abstract class EndpointController extends PagedResourceController{
		protected function isInvalid(){
			$invalidArguments = array();
			return $invalidArguments;
		}
		
		protected function getInvalidQueryParameters(){
			$invalidQueryParameters = array();
			
			$sort = RequestHandler::getQueryParameter("sort");
			$order = RequestHandler::getQueryParameter("order");
			
			if($sort && !in_array($sort,array("uri","requests","runtime"))){
				$invalidQueryParameters[] = "sort";
			}
			
			if($order && !in_array($order,array("asc","desc"))){
				$invalidQueryParameters[] = "order";
			}
			
			return $invalidQueryParameters;
		}
		
		protected function getRecordCount(){
			$api = $this->getResourceObject("API","id",$this->viewFromArguments("api"));
			
			$countResult = DatabaseAdministrator::execute("SELECT COUNT(DISTINCT(SUBSTRING_INDEX(endpoint,'?',1))) as endpoints FROM EndpointStatistic WHERE api = {$api->getId()}");
			$count = $countResult[0]["endpoints"];
			
			return $count;
		}
		
		protected function buildPageEntries($start,$count){
			$api = $this->getResourceObject("API","id",$this->viewFromArguments("api"));
			
			$sort = RequestHandler::getQueryParameter("sort");
			if($sort == "uri"){
				$sort = "endpoint";
			}else if($sort == "requests"){
				$sort = "dailyRequests";
			}else if($sort == "runtimes"){
				$sort = "averageRuntime";
			}else{
				$sort = "id";
			}
			
			$order = RequestHandler::getQueryParameter("order");
			$order = $order?$order:"asc";
		
			$endpointStatisticStatistics = array();
			foreach(DatabaseAdministrator::execute("SELECT MIN(id) AS id, SUBSTRING_INDEX(endpoint,'?',1) as endpointRoot, MIN(date) as startDate, AVG(runtime)/AVG(requests) as averageRuntime, SUM(requests) AS totalRequests ".
								"FROM EndpointStatistic WHERE api = {$api->getId()} GROUP BY 2 ORDER BY $sort $order") as $row){
				
				$startDate = new DateTime($row['startDate']);
				$today = new DateTime(date("Y-m-d"));
				$daysActive = intval($today->diff($startDate)->format("%a")) + 1;
				
				$nextEndpoint = new EndpointStatistic($api,$row['endpointRoot'],"",$row['averageRuntime'],$row['totalRequests']/$daysActive);
				$nextEndpoint->setId($row['id']);
				$endpointStatisticStatistics[] = $nextEndpoint;
			}
			
			$payload = array();
			foreach($endpointStatisticStatistics as $endpointStatisticStatistic){
				$payload[] = $this->assemblePayload($endpointStatisticStatistic);
			}
			
			return $payload;
		}
		
		protected function assemblePayload($endpointStatistic){
			$payload = array(
				"self"=>$endpointStatistic->getURI(),
				"request"=>$endpointStatistic->getEndpoint(),
				"average-runtime"=>floatval(sprintf("%.4f",$endpointStatistic->getRuntime())),
				"daily-requests"=>floatval(sprintf("%.2f",$endpointStatistic->getRequests())),
				"variants"=>array(
					"self"=>"{$endpointStatistic->getURI()}/variants",
					"total"=>$endpointStatistic->getVariants()
				),
				"runtimes"=>"{$endpointStatistic->getURI()}/runtimes",
				"requests"=>"{$endpointStatistic->getURI()}/requests"
			);
			
			return $payload;
		}
	}
?>