<?php
	class VariantRuntimesResource extends ResourceController{
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
			$runtimesData = Array();
			$startDate = date("Y-m-d");
			foreach($variantStatistics as $variantStatistic){
				$runtimesData[$variantStatistic['date']] = round($variantStatistic['totalRuntime']/$variantStatistic['totalRequests'],4);
				
				if($variantStatistic['date'] < $startDate){
					$startDate = $variantStatistic['date'];
				}
			}
			
			$variantPayload = array();
			$endDate = date("Y-m-d");
			for($i = $startDate; $i <= $endDate; $i = date('Y-m-d',strtotime($i.' +1 day'))){
				$variantPayload[$i] = array_key_exists($i,$runtimesData)?$runtimesData[$i]:"0";
			}
			
			$payload = array(
				"self"=>"{$this->getBase()}/".RequestHandler::getRequestURI(),
				"runtimes"=>$variantPayload
			);
			
			return $payload;
		}
		
		protected function get(){
			$api = $this->getResourceObject("API","id",$this->viewFromArguments("api"));
			$variant = $this->getResourceObject("EndpointStatistic","id",$this->viewFromArguments("variant"));
			$where = Array();
			
			$from = RequestHandler::getQueryParameter("from");
			$to = RequestHandler::getQueryParameter("to");
			$order = RequestHandler::getQueryParameter("order");
			
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
			
			$runtimeResult = DatabaseAdministrator::execute("SELECT date, SUM(runtime) as totalRuntime, SUM(requests) as totalRequests FROM EndpointStatistic WHERE $whereClause GROUP BY date ORDER BY date $order");
			$payload = $this->assemblePayload($runtimeResult);
			ResponseHandler::retrieved($payload,$variant->getURI(),$variant->getModified(),$variant->getETag());
		}
	}
?>