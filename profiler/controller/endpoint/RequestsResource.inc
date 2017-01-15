<?php
	class RequestsResource extends ResourceController{
		protected function isInvalid(){
			$invalidArguments = array();
			return $invalidArguments;
		}
		
		protected function getInvalidQueryParameters(){
			$invalidQueryParameters = array();
			
			$from = RequestHandler::getQueryParameter("from");
			$to = RequestHandler::getQueryParameter("to");
			$order = RequestHandler::getQueryParameter("order");
			
			if($from && !preg_match("/^[0-9]{4}-[0-1][0-9]-[0-3][0-9]$/",$from)){
				$invalidQueryParameters[] = "from";
			}
			
			if($to && !preg_match("/^[0-9]{4}-[0-1][0-9]-[0-3][0-9]$/",$to)){
				$invalidQueryParameters[] = "to";
			}
			
			if($order && !in_array($order,array("asc","desc"))){
				$invalidQueryParameters[] = "order";
			}
			
			return $invalidQueryParameters;
		}
		
		protected function isConsistent($modifiedDate,$eTag){
			$endpoint = $this->getResourceObject("Endpoint","id",$this->viewFromArguments("endpoint"));			
			return $modifiedDate == $endpoint->getModified() && $eTag == $endpoint->getETag();
		}
		
		protected function assemblePayload($endpointStatistics){
			$endpointPayload = array();
			foreach($endpointStatistics as $endpointStatistic){
				$endpointPayload[$endpointStatistic->getDate()] = $endpointStatistic->getRequests();
			}
			
			$payload = array(
				"self"=>"{$this->getBase()}/".RequestHandler::getRequestURI(),
				"requests"=>$endpointPayload
			);
			
			return $payload;
		}
		
		protected function get(){
			$api = $this->getResourceObject("API","id",$this->viewFromArguments("api"));
			$endpoint = $this->getResourceObject("EndpointStatistic","id",$this->viewFromArguments("endpoint"));
			
			$sqlGateway = new SQLGateway();
			$searcher = new Searcher();
			$sorter = new Sorter();
			
			$from = RequestHandler::getQueryParameter("from");
			$to = RequestHandler::getQueryParameter("to");
			$order = RequestHandler::getQueryParameter("order");
			
			$searcher->addCriterion("api",Criterion::EQUAL,$api);
			$searcher->addCriterion("endpoint",Criterion::EQUAL,$endpoint->getEndpoint());
			
			if($from){
				$searcher->addCriterion("date",Criterion::GREATER_THAN_EQUAL,"$from");
			}
			
			if($to){
				$searcher->addCriterion("date",Criterion::LESS_THAN_EQUAL,"$to");
			}
			
			$order = $order?$order:"asc";
			$sorter->addCriterion("date",$order);
			
			$payload = $this->assemblePayload($sqlGateway->find("EndpointStatistic",$searcher,$sorter));
			ResponseHandler::retrieved($payload,$endpoint->getURI(),$endpoint->getModified(),$endpoint->getETag());
		}
	}
?>