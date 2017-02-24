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
			Curator::rebuildDatabase();
			
			$message = "Database Rebuilt Successfully";
			ResponseHandler::created($this->assemblePayload($message),$this->getBase()."/wadapi/rebuild");
		}
	}
?>