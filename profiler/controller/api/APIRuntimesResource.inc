<?php
	class APIRuntimesResource extends ResourceController{
		protected function isInvalid(){
			$invalidArguments = array();
			return $invalidArguments;
		}
		
		protected function getInvalidQueryParameters(){
			$invalidQueryParameters = array();
			
			$from = RequestHandler::getQueryParameter("from");
			$to = RequestHandler::getQueryParameter("to");
			$aggregator = RequestHandler::getQueryParameter("aggregator");
			$order = RequestHandler::getQueryParameter("order");
			
			if($from && !preg_match("/^[0-9]{4}-[0-1][0-9]-[0-3][0-9]$/",$from)){
				$invalidQueryParameters[] = "from";
			}
			
			if($to && !preg_match("/^[0-9]{4}-[0-1][0-9]-[0-3][0-9]$/",$to)){
				$invalidQueryParameters[] = "to";
			}
			
			if($aggregator && !in_array($aggregator,array("date","endpoint"))){
				$invalidQueryParameters[] = "aggregator";
			}
			
			if($order && !in_array($order,array("asc","desc"))){
				$invalidQueryParameters[] = "order";
			}
			
			return $invalidQueryParameters;
		}
		
		protected function isConsistent($modifiedDate,$eTag){
			$api = $this->getResourceObject("API","id",$this->viewFromArguments("api"));			
			return $modifiedDate == $api->getModified() && $eTag == $api->getETag();
		}
		
		protected function assemblePayload($apiStatistics){
			$aggregator = RequestHandler::getQueryParameter("aggregator");
			$aggregator = $aggregator?$aggregator:"date";
			
			$runtimesData = Array();
			$startDate = date("Y-m-d");
			foreach($apiStatistics as $apiStatistic){
				$runtimesData[$apiStatistic['aggregator']] = round($apiStatistic['totalRuntime']/$apiStatistic['totalRequests'],4);
				
				if($aggregator == "date"){
					if($apiStatistic['aggregator'] < $startDate){
						$startDate = $apiStatistic['aggregator'];
					}
				}
			}
			
			$apiPayload = array();
			if($aggregator == "date"){
				$endDate = date("Y-m-d");
				for($i = $startDate; $i <= $endDate; $i = date('Y-m-d',strtotime($i.' +1 day'))){
					if(!array_key_exists($i,$apiPayload)){
						$apiPayload[$i] = $runtimesData[$i]?$runtimesData[$i]:"0";
					}
				}
			}else{
				$apiPayload = $runtimesData;
			}
			
			$payload = array(
				"self"=>"{$this->getBase()}/".RequestHandler::getRequestURI(),
				"runtimes"=>$apiPayload
			);
			
			return $payload;
		}
		
		protected function get(){
			$api = $this->getResourceObject("API","id",$this->viewFromArguments("api"));
			$from = RequestHandler::getQueryParameter("from");
			$to = RequestHandler::getQueryParameter("to");
			$aggregator = RequestHandler::getQueryParameter("aggregator");
			$order = RequestHandler::getQueryParameter("order");
			$where = Array();
			
			$where[] = "api = '{$api->getId()}'";
			
			if($from){
				$where[] = "date >= '$from'";
			}
			
			if($to){
				$where[] = "date <= '$to'";
			}
			
			if($aggregator == "endpoint"){
				$aggregator = "SUBSTRING_INDEX(endpoint,'?',1)";
			}
			
			$aggregator = $aggregator?$aggregator:"date";
			
			$whereClause = implode(" AND ",$where);
			$order = $order?$order:"asc";
			
			$runtimeResult = DatabaseAdministrator::execute("SELECT $aggregator as aggregator, SUM(runtime) as totalRuntime, SUM(requests) as totalRequests FROM EndpointStatistic WHERE $whereClause GROUP BY aggregator ORDER BY aggregator $order");
			$payload = $this->assemblePayload($runtimeResult);
			
			ResponseHandler::retrieved($payload,$api->getURI(),$api->getModified(),$api->getETag());
		}
	}
?>