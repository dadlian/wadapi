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
			//Confirm that the user has utilities access
			$password = $this->getFromContent("authentication");
			if($password !== SettingsManager::getSetting("install","adminpass")){
				ResponseHandler::forbidden("The provided tokens do not have permission to perform this action.");
			}
			
			//Rebuild Database
			Curator::rebuildDatabase();
			
			//Initialise Root API Token if necessary
			$rootKey = md5(SettingsManager::getSetting("api","key"));
			$rootSecret = md5(SettingsManager::getSetting("api","secret"));
			
			$sqlGateway = new SQLGateway();
			$searcher = new Searcher();
			$searcher->addCriterion("accessKey",Criterion::EQUAL,$rootKey);
			$searcher->addCriterion("accessSecret",Criterion::EQUAL,$rootSecret);
			if(!$sqlGateway->findUnique("APIToken",$searcher)){
				$sqlGateway->save(new APIToken("root",0,$rootKey,$rootSecret,md5($rootKey.$rootSecret)));
			}
			
			$message = "Database Rebuilt Successfully";
			ResponseHandler::created($this->assemblePayload($message),$this->getBase()."/wadapi/rebuild");
		}
	}
?>