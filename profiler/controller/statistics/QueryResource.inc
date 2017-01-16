<?php
	class ObjectResource extends ObjectController{
		protected function get(){
			$queryStatistic = $this->getResourceObject("QueryStatistic","id",$this->viewFromArguments("query"));	
			$payload = $this->assemblePayload($queryStatistic);
			ResponseHandler::retrieved($payload,$queryStatistic->getURI(),$queryStatistic->getModified(),$queryStatistic->getETag());
		}
		
		protected function isConsistent($modifiedDate,$eTag){
			$queryStatistic = $this->getResourceObject("QueryStatistic","id",$this->viewFromArguments("query"));			
			return $modifiedDate == $queryStatistic->getModified() && $eTag == $queryStatistic->getETag();
		}
	}
?>