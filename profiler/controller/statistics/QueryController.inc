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
			$variant = $this->getResourceObject("EndpointStatistic","id",$this->viewFromArguments("variant"));
			
			$countResult = DatabaseAdministrator::execute("SELECT COUNT(query) as queries FROM QueryStatistic WHERE api='{$api->getId()}' AND endpoint='{$variant->getEndpoint()}'");
			$count = $countResult[0]["queries"];
			
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
				"query"=>$queryStatistic->getQuery(),
				"request-executions"=>sprintf("%.2f",$queryStatistic->getExecutions()/$queryStatistic->getRequests()),
				"query-runtime"=>sprintf("%.4f",$queryStatistic->getRuntime()/$queryStatistic->getExecutions())
			);
			
			return $payload;
		}
	}
?>