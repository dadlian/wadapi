<?php
	abstract class CallController extends PagedResourceController{
		protected function isInvalid(){
			$invalidArguments = array();
			return $invalidArguments;
		}
		
		protected function getInvalidQueryParameters(){
			$invalidQueryParameters = array();
			
			$sort = RequestHandler::getQueryParameter("sort");
			$order = RequestHandler::getQueryParameter("order");
			
			if($sort && !in_array($sort,array("calls","roundtrip"))){
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
			
			$countResult = DatabaseAdministrator::execute("SELECT COUNT(externalCall) as calls FROM CallStatistic WHERE api='{$api->getId()}' AND endpoint='{$endpoint->getEndpoint()}'");
			$count = $countResult[0]["calls"];
			
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
		
			$callStatistics = Array();
			foreach($sqlGateway->find("CallStatistic",$searcher,$sorter,$count,$start) as $index => $callStatistic){
				$callStatistics[strval($callStatistic->getRoundtrip()/$callStatistic->getCalls()).$index] = $callStatistic;
			}
			
			if($sort == "runtime" && $order == "ASC"){
				ksort($callStatistics);
			}else if($sort == "runtime" && $order == "DESC"){
				krsort($callStatistics);
			}
			
			foreach($callStatistics as $callStatistic){
				$payload[] = $this->assemblePayload($callStatistic);
			}
			
			return $payload;
		}
		
		protected function assemblePayload($callStatistic){
			$payload = array(
				"self"=>$callStatistic->getURI(),
				"call"=>$callStatistic->getExternalCall(),
				"request-calls"=>sprintf("%.2f",$callStatistic->getCalls()/$callStatistic->getRequests()),
				"call-roundtrip"=>sprintf("%.4f",$callStatistic->getRoundtrip()/$callStatistic->getCalls())
			);
			
			return $payload;
		}
	}
?>