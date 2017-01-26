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
			$variant = $this->getResourceObject("EndpointStatistic","id",$this->viewFromArguments("variant"));
			
			$countResult = DatabaseAdministrator::execute("SELECT COUNT(customKey) as customs FROM CustomStatistic WHERE api='{$api->getId()}' AND endpoint='{$variant->getEndpoint()}'");
			$count = $countResult[0]["customs"];
			
			return $count;
		}
		
		protected function buildPageEntries($start,$count){
			$api = $this->getResourceObject("API","id",$this->viewFromArguments("api"));
			$variant = $this->getResourceObject("EndpointStatistic","id",$this->viewFromArguments("variant"));
			
			$payload = array();
			$sqlGateway = new SQLGateway();
			$searcher = new Searcher();
			$sorter = new Sorter();
			
			$sort = RequestHandler::getQueryParameter("sort");
			$sort = $sort?$sort:"id";
			
			$order = RequestHandler::getQueryParameter("order");
			$order = strtoupper($order?$order:"asc");
			
			$searcher->addCriterion("api",Criterion::EQUAL,$api->getId());
			$searcher->addCriterion("endpoint",Criterion::EQUAL,$variant->getEndpoint());
			
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