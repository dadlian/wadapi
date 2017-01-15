<?php
	abstract class QueryController extends PagedResourceController{
		protected function isInvalid(){
			$invalidArguments = array();
			return $invalidArguments;
		}
		
		protected function getInvalidQueryParameters(){
			$invalidQueryParameters = array();
			
			$sort = RequestHandler::getQueryParameter("sort");
			$order = RequestHandler::getQueryParameter("order");
			
			if($sort && !in_array($sort,array("executions","runtime"))){
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
			
			$countResult = DatabaseAdministrator::execute("SELECT COUNT(query) as queries FROM QueryStatistic WHERE api='{$api->getId()}' AND endpoint='{$endpoint->getEndpoint()}'");
			$count = $countResult[0]["queries"];
			
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
		
			$queryStatistics = Array();
			foreach($sqlGateway->find("QueryStatistic",$searcher,$sorter,$count,$start) as $index => $queryStatistic){
				$queryStatistics[strval($queryStatistic->getRuntime()/$queryStatistic->getExecutions()).$index] = $queryStatistic;
			}
			
			if($sort == "runtime" && $order == "ASC"){
				ksort($queryStatistics);
			}else if($sort == "runtime" && $order == "DESC"){
				krsort($queryStatistics);
			}
			
			foreach($queryStatistics as $queryStatistic){
				$payload[] = $this->assemblePayload($queryStatistic);
			}
			
			return $payload;
		}
		
		protected function assemblePayload($queryStatistic){
			$payload = array(
				"self"=>$queryStatistic->getURI(),
				"class"=>$queryStatistic->getQuery(),
				"request-executions"=>$queryStatistic->getExecutions(),
				"average-runtime"=>floatval(sprintf("%.4f",$queryStatistic->getRuntime()/$queryStatistic->getExecutions()))
			);
			
			return $payload;
		}
	}
?>