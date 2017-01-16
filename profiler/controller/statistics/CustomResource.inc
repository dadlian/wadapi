<?php
	class CustomResource extends CustomController{
		protected function get(){
			$customStatistic = $this->getResourceObject("CustomStatistic","id",$this->viewFromArguments("custom"));	
			$payload = $this->assemblePayload($customStatistic);
			ResponseHandler::retrieved($payload,$customStatistic->getURI(),$customStatistic->getModified(),$customStatistic->getETag());
		}
		
		protected function isConsistent($modifiedDate,$eTag){
			$customStatistic = $this->getResourceObject("CustomStatistic","id",$this->viewFromArguments("custom"));			
			return $modifiedDate == $customStatistic->getModified() && $eTag == $customStatistic->getETag();
		}
	}
?>