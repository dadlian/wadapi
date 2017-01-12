<?php
	class TokenResource extends ResourceController{
		public function execute(){
			$token = $this->getResourceObject("APIToken","id",$this->viewFromArguments("access"));
			$authorisation = RequestHandler::getAuthorisation();
			
			if(md5($authorisation["key"]) != $token->getAccessKey() || md5($authorisation["secret"]) != $token->getAccessSecret()){
				ResponseHandler::forbidden("The provided tokens do not have permission to perform this action.");
			}
			
			parent::execute();
		}
	
		public function get(){
			$token = $this->getResourceObject("APIToken","id",$this->viewFromArguments("access"));
			$authorisation = RequestHandler::getAuthorisation();
			$lifetime = $token->getExpires()?($token->getExpires()-$token->getModified()):0;
			
			$payload = array(
				"self"=>"{$this->getBase()}/".RequestHandler::getRequestURI(),
				"key"=>$token->getAccessKey(),
				"secret"=>$token->getAccessSecret(),
				"lifetime"=>$lifetime
			);
			
			ResponseHandler::addExpiry($token->getExpires());
			ResponseHandler::retrieved($payload,$payload["self"]);
		}
		
		public function delete(){
			$token = $this->getResourceObject("APIToken","id",$this->viewFromArguments("access"));
			CryptKeeper::bury($token);
			ResponseHandler::deleted("Token: /".RequestHandler::getRequestURI().", has been invalidated.");
		}
		
		protected function isInvalid(){
			return array();
		}
		
		protected function isConsistent($modified,$eTag){
			return false;
		}
		
		protected function assemblePayload($object){
			return "";
		}
		
		protected function mustPrevalidate(){
			return false;
		}
	}
?>