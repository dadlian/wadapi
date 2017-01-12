<?php
	class AccessResource extends AccessController{
		protected function get(){
			$token = $this->getResourceObject("APIToken","id",$this->viewFromArguments("access"));	
			$payload = $this->assemblePayload(array($token));
			$eTag = md5($token->getETag());
			ResponseHandler::retrieved($payload,$token->getURI(),$token->getModified(),$eTag);
		}
		
		protected function put(){
			$sqlGateway = new SQLGateway();
			
			$token = $this->getResourceObject("APIToken","id",$this->viewFromArguments("access"));
			$token->setRole($this->getFromContent("role"));
			$sqlGateway->save($token);
			
			$payload = $this->assemblePayload(array($token));
			$eTag = md5($token->getETag());
			ResponseHandler::modified($payload,$token->getURI());
		}
		
		protected function delete(){
			$token = $this->getResourceObject("APIToken","id",$this->viewFromArguments("access"));
			CryptKeeper::bury($token);
			
			ResponseHandler::deleted("Client access: /".RequestHandler::getRequestURI().", has been revoked.");
		}
		
		protected function isConsistent($modifiedDate,$eTag){
			$token = $this->getResourceObject("APIToken","id",$this->viewFromArguments("access"));			
			return $modifiedDate == $token->getModified() && $eTag == md5($token->getETag());
		}
		
		protected function assemblePayload($data){
			$token = $data[0];
			$payload = array(
				"self"=>$token->getURI(),
				"tokens"=>"{$token->getURI()}/tokens",
				"active-token"=>"{$token->getURI()}/tokens/active",
				"role"=>$token->getRole()
			);
			
			return $payload;
		}
	}
?>