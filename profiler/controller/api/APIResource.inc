<?php
	class APIResource extends APIController{
		protected function get(){
			$api = $this->getResourceObject("API","id",$this->viewFromArguments("api"));
			$payload = $this->assemblePayload($api);
			ResponseHandler::retrieved($payload,$api->getURI(),$api->getModified(),$api->getETag());
		}
		
		protected function delete(){
			$api = $this->getResourceObject("API","id",$this->viewFromArguments("api"));
			CryptKeeper::bury($api);
			
			ResponseHandler::deleted("API: {$api->getName()} has been removed.");
		}
		
		protected function isConsistent($modifiedDate,$eTag){
			$api = $this->getResourceObject("API","id",$this->viewFromArguments("api"));			
			return $modifiedDate == $api->getModified() && $eTag == $api->getETag();
		}
	}
?>