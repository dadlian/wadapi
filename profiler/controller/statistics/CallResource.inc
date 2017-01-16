<?php
	class CallResource extends CallController{
		protected function get(){
			$callStatistic = $this->getResourceObject("CallStatistic","id",$this->viewFromArguments("call"));	
			$payload = $this->assemblePayload($callStatistic);
			ResponseHandler::retrieved($payload,$callStatistic->getURI(),$callStatistic->getModified(),$callStatistic->getETag());
		}
		
		protected function isConsistent($modifiedDate,$eTag){
			$callStatistic = $this->getResourceObject("CallStatistic","id",$this->viewFromArguments("call"));			
			return $modifiedDate == $callStatistic->getModified() && $eTag == $callStatistic->getETag();
		}
	}
?>