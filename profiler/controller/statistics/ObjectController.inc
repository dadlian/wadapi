<?php
	abstract class ObjectController extends PagedResourceController{
		protected function isInvalid(){
			$invalidArguments = array();
			return $invalidArguments;
		}
		
		protected function getInvalidQueryParameters(){
			$invalidQueryParameters = array();
			
			$sort = RequestHandler::getQueryParameter("sort");
			$order = RequestHandler::getQueryParameter("order");
			
			if($sort && !in_array($sort,array("instances","loadtime"))){
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
			
			$countResult = DatabaseAdministrator::execute("SELECT COUNT(class) as objects FROM ObjectStatistic WHERE api='{$api->getId()}' AND endpoint='{$variant->getEndpoint()}'");
			$count = $countResult[0]["objects"];
			
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
		
			$objectStatistics = Array();
			foreach($sqlGateway->find("ObjectStatistic",$searcher,$sorter,$count,$start) as $index => $objectStatistic){
				$objectStatistics[strval($objectStatistic->getLoadTime()/$objectStatistic->getInstances()).$index] = $objectStatistic;
			}
			
			if($sort == "loadtime" && $order == "ASC"){
				ksort($objectStatistics);
			}else if($sort == "loadtime" && $order == "DESC"){
				krsort($objectStatistics);
			}
			
			foreach($objectStatistics as $objectStatistic){
				$payload[] = $this->assemblePayload($objectStatistic);
			}
			
			return $payload;
		}
		
		protected function assemblePayload($objectStatistic){
			$payload = array(
				"self"=>$objectStatistic->getURI(),
				"class"=>$objectStatistic->getClass(),
				"request-instances"=>sprintf("%.2f",$objectStatistic->getInstances()/$objectStatistic->getRequests()),
				"instance-loadtime"=>sprintf("%.4f",$objectStatistic->getLoadTime()/$objectStatistic->getInstances())
			);
			
			return $payload;
		}
	}
?>