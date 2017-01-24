<?php
	class VariantRequestsResource extends ResourceController{
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
			$variant = $this->getResourceObject("Endpoint","id",$this->viewFromArguments("variant"));			
			return $modifiedDate == $variant->getModified() && $eTag == $variant->getETag();
		}
		
		protected function assemblePayload($variantStatistics){
			$requestsData = Array();
			$startDate = date("Y-m-d");
			foreach($variantStatistics as $variantStatistic){
				$requestsData[$variantStatistic['date']] = $variantStatistic['totalRequests'];
				
				if($variantStatistic['date'] < $startDate){
					$startDate = $variantStatistic['date'];
				}
			}
			
			$variantPayload = array();
			$endDate = date("Y-m-d");
			for($i = $startDate; $i <= $endDate; $i = date('Y-m-d',strtotime($i.' +1 day'))){
				$variantPayload[$i] = $requestsData[$i]?$requestsData[$i]:"0";
			}
			
			$payload = array(
				"self"=>"{$this->getBase()}/".RequestHandler::getRequestURI(),
				"requests"=>$variantPayload
			);
			
			return $payload;
		}
		
		protected function get(){
			$api = $this->getResourceObject("API","id",$this->viewFromArguments("api"));
			$variant = $this->getResourceObject("EndpointStatistic","id",$this->viewFromArguments("variant"));
			$where = Array();
			
			$where[] = "api = '{$api->getId()}'";
			$where[] = "endpoint = '{$variant->getEndpoint()}'";
			
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
			ResponseHandler::retrieved($payload,$variant->getURI(),$variant->getModified(),$variant->getETag());
		}
	}
?>