<?php
	class APIRequestsResource extends ResourceController{
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
			$api = $this->getResourceObject("API","id",$this->viewFromArguments("api"));			
			return $modifiedDate == $api->getModified() && $eTag == $api->getETag();
		}
		
		protected function assemblePayload($apiStatistics){
			$requestsData = Array();
			$startDate = date("Y-m-d");
			foreach($apiStatistics as $apiStatistic){
				$requestsData[$apiStatistic['date']] = $apiStatistic['totalRequests'];
				
				if($apiStatistic['date'] < $startDate){
					$startDate = $apiStatistic['date'];
				}
			}
			
			$apiPayload = array();
			$endDate = date("Y-m-d");
			for($i = $startDate; $i <= $endDate; $i = date('Y-m-d',strtotime($i.' +1 day'))){
				if(!array_key_exists($i,$apiPayload)){
					$apiPayload[$i] = $requestsData[$i]?$requestsData[$i]:"0";
				}
			}
			
			$payload = array(
				"self"=>"{$this->getBase()}/".RequestHandler::getRequestURI(),
				"requests"=>$apiPayload
			);
			
			return $payload;
		}
		
		protected function get(){
			$api = $this->getResourceObject("API","id",$this->viewFromArguments("api"));
			$from = RequestHandler::getQueryParameter("from");
			$to = RequestHandler::getQueryParameter("to");
			$order = RequestHandler::getQueryParameter("order");
			$where = Array();
			
			$where[] = "api = '{$api->getId()}'";
			
			if($from){
				$where[] = "date >= '$from'";
			}
			
			if($to){
				$where[] = "date <= '$to'";
			}
			
			$whereClause = implode(" AND ",$where);
			$order = $order?$order:"asc";
			
			$requestResult = DatabaseAdministrator::execute("SELECT date, SUM(requests) as totalRequests FROM EndpointStatistic WHERE $whereClause GROUP BY date ORDER BY date $order");
			$payload = $this->assemblePayload($requestResult);
			
			ResponseHandler::retrieved($payload,$api->getURI(),$api->getModified(),$api->getETag());
		}
	}
?>