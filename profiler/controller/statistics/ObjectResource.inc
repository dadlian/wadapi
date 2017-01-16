<?php
	class ObjectResource extends ObjectController{
		protected function get(){
			$objectStatistic = $this->getResourceObject("ObjectStatistic","id",$this->viewFromArguments("object"));	
			$payload = $this->assemblePayload($objectStatistic);
			ResponseHandler::retrieved($payload,$objectStatistic->getURI(),$objectStatistic->getModified(),$objectStatistic->getETag());
		}
		
		protected function isConsistent($modifiedDate,$eTag){
			$objectStatistic = $this->getResourceObject("ObjectStatistic","id",$this->viewFromArguments("object"));			
			return $modifiedDate == $objectStatistic->getModified() && $eTag == $objectStatistic->getETag();
		}
	}
?>