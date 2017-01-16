<?php
	abstract class CustomController extends PagedResourceController{
		protected function isInvalid(){
			$invalidArguments = array();
			return $invalidArguments;
		}
		
		protected function getInvalidQueryParameters(){
			$invalidQueryParameters = array();
			
			$sort = RequestHandler::getQueryParameter("sort");
			$order = RequestHandler::getQueryParameter("order");
			
			if($sort && !in_array($sort,array("runs","duration"))){
				$invalidQueryParameters[] = "sort";
			}
			
			if($order && !in_array($order,array("asc","desc"))){
				$invalidQueryParameters[] = "order";
			}
			
			return $invalidQueryParameters;
		}
		
		protected function getRecordCount(){
			$api = $this->getResourceObject("API","id",$this->viewFromArguments("api"));
			$endpoint = $this->getResourceObject("EndpointStatistic","id",$this->viewFromArguments("endpoint"));
			
			$countResult = DatabaseAdministrator::execute("SELECT COUNT(customKey) as customs FROM CustomStatistic WHERE api='{$api->getId()}' AND endpoint='{$endpoint->getEndpoint()}'");
			$count = $countResult[0]["customs"];
			
			return $count;
		}
		
		protected function buildPageEntries($start,$count){
			$payload = array();
			$sqlGateway = new SQLGateway();
			$searcher = new Searcher();
			$sorter = new Sorter();
			
			$sort = RequestHandler::getQueryParameter("sort");
			$sort = $sort?$sort:"id";
			
			$order = RequestHandler::getQueryParameter("order");
			$order = strtoupper($order?$order:"asc");
			
			$searcher->addCriterion("api",Criterion::EQUAL,$this->viewFromArguments("api"));
			
			$endpoint = $this->getResourceObject("EndpointStatistic","id",$this->viewFromArguments("endpoint"));
			$searcher->addCriterion("endpoint",Criterion::EQUAL,$endpoint->getEndpoint());
			
			$sorter->addCriterion($sort,$order);
		
			$customStatistics = Array();
			foreach($sqlGateway->find("CustomStatistic",$searcher,$sorter,$count,$start) as $index => $customStatistic){
				$customStatistics[strval($customStatistic->getDuration()/$customStatistic->getRuns()).$index] = $customStatistic;
			}
			
			if($sort == "runs" && $order == "ASC"){
				ksort($customStatistics);
			}else if($sort == "runs" && $order == "DESC"){
				krsort($customStatistics);
			}
			
			foreach($customStatistics as $customStatistic){
				$payload[] = $this->assemblePayload($customStatistic);
			}
			
			return $payload;
		}
		
		protected function assemblePayload($customStatistic){
			$payload = array(
				"self"=>$customStatistic->getURI(),
				"key"=>$customStatistic->getCustomKey(),
				"runs"=>sprintf("%.2f",$customStatistic->getRuns()/$customStatistic->getRequests()),
				"duration"=>sprintf("%.4f",$customStatistic->getDuration()/$customStatistic->getRuns())
			);
			
			return $payload;
		}
	}
?>