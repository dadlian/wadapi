<?php
	class WadapiRebuildDatabase extends ResourceController{
		protected function isInvalid(){
			$invalidArguments = array();
			return $invalidArguments;
		}
		
		protected function getInvalidQueryParameters(){
			$invalidQueryParameters = array();
			return $invalidQueryParameters;
		}
		
		protected function isConsistent($modifiedDate,$eTag){
			return true;
		}
		
		protected function assemblePayload($message){
			$payload = array(
				"message"=>$message
			);
			
			return $payload;
		}
		
		protected function post(){
			$payload = $this->assemblePayload("Database Rebuilt Successfully");
			ResponseHandler::created($payload,$this->getBase()."/wadapi/rebuild");
		}
	}
?>