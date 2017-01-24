<?php
	class EndpointRuntimesResource extends ResourceController{
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
			$runtimesData = Array();
			$startDate = date("Y-m-d");
			foreach($endpointStatistics as $endpointStatistic){
				$runtimesData[$endpointStatistic['date']] = round($endpointStatistic['totalRuntime']/$endpointStatistic['totalRequests'],4);
				
				if($endpointStatistic['date'] < $startDate){
					$startDate = $endpointStatistic['date'];
				}
			}
			
			$endpointPayload = array();
			$endDate = date("Y-m-d");
			for($i = $startDate; $i <= $endDate; $i = date('Y-m-d',strtotime($i.' +1 day'))){
				$endpointPayload[$i] = $runtimesData[$i]?$runtimesData[$i]:"0";
			}
			
			$payload = array(
				"self"=>"{$this->getBase()}/".RequestHandler::getRequestURI(),
				"runtimes"=>$endpointPayload
			);
			
			return $payload;
		}
		
		protected function get(){
			$api = $this->getResourceObject("API","id",$this->viewFromArguments("api"));
			$endpoint = $this->getResourceObject("EndpointStatistic","id",$this->viewFromArguments("endpoint"));
			$endpointParts = explode("?",$endpoint->getEndpoint());
			$where = Array();
			
			$where[] = "api = '{$api->getId()}'";
			$where[] = "SUBSTRING_INDEX(endpoint,'?',1) = '{$endpointParts[0]}'";
			
			if($from){
				$where[] = "date >= '$from'";
			}
			
			if($to){
				$where[] = "date <= '$to'";
			}
			
			$whereClause = implode(" AND ",$where);
			$order = $order?$order:"asc";
			
			$runtimeResult = DatabaseAdministrator::execute("SELECT date, SUM(runtime) as totalRuntime, SUM(requests) as totalRequests FROM EndpointStatistic WHERE $whereClause GROUP BY date ORDER BY date $order");
			$payload = $this->assemblePayload($runtimeResult);
			ResponseHandler::retrieved($payload,$endpoint->getURI(),$endpoint->getModified(),$endpoint->getETag());
		}
	}
?>