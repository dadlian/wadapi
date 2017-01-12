<?php
	class AccessCollection extends AccessController{
		public function post(){
			$sqlGateway = new SQLGateway();
			
			//Create Access Token
			$token = new APIToken($this->getFromContent("role"));
			$accessTokens = $token->refresh(1);
			$sqlGateway->save($token);
			
			$lifetime = $token->getExpires()?($token->getExpires()-$token->getModified()):0;
			$payload = array(
				"self"=>$token->getURI(),
				"tokens"=>"{$token->getURI()}/tokens",
				"active-token"=>array(
					"self"=>"{$token->getURI()}/tokens/active",
					"key"=>$accessTokens["key"],
					"secret"=>$accessTokens["secret"],
					"refresh"=>$accessTokens["refresh"],
					"lifetime"=>$lifetime
				),
				"role"=>$this->getFromContent("role")
			);
			
			ResponseHandler::created($payload,$token->getURI());
		}
		
		protected function isConsistent($modifiedDate,$eTag){
			return true;
		}
		
		protected function assemblePayload($object){
			return true;
		}
	}
?>