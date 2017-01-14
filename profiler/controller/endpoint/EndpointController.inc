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
			
			if($sort && !in_array($sort,array("requests","runtime"))){
				$invalidQueryParameters[] = "sort";
			}
			
			if($order && !in_array($order,array("asc","desc"))){
				$invalidQueryParameters[] = "order";
			}
			
			return $invalidQueryParameters;
		}
		
		protected function getRecordCount(){
			$countResult = DatabaseAdministrator::execute("SELECT COUNT(id) as endpoints FROM EndpointStatistic");
			$count = $countResult[0]["endpoints"];
			
			return $count;
		}
		
		protected function buildPageEntries($start,$count){
			$api = $this->getResourceObject("API","id",$this->viewFromArguments("api"));
			
			$sort = RequestHandler::getQueryParameter("sort");
			if($sort == "requests"){
				$sort = "dailyRequests";
			}else if($sort == "runtimes"){
				$sort = "averageRuntime";
			}else{
				$sort = "id";
			}
			
			$order = RequestHandler::getQueryParameter("order");
			$order = $order?$order:"asc";
		
			$endpointStatisticStatistics = array();
			foreach(DatabaseAdministrator::execute("SELECT MIN(id) AS id, endpoint, AVG(runtime)/AVG(requests) as averageRuntime, AVG(requests) AS dailyRequests ".
								"FROM EndpointStatistic WHERE api = ? GROUP BY endpoint ORDER BY $sort $order",$api->getId()) as $row){
				$nextEndpoint = new EndpointStatistic($api,$row['endpoint'],"",$row['averageRuntime'],$row['dailyRequests']);
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
				"uri"=>$endpointStatistic->getEndpoint(),
				"average-runtime"=>$endpointStatistic->getRuntime(),
				"daily-requests"=>$endpointStatistic->getRequests(),
				"runtimes"=>"{$endpointStatistic->getURI()}/runtimes",
				"requests"=>"{$endpointStatistic->getURI()}/requests",
				"objects"=>"{$endpointStatistic->getURI()}/objects",
				"queries"=>"{$endpointStatistic->getURI()}/queries"
			);
			
			return $payload;
		}
	}
?>