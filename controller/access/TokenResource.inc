<?php
	class TokenResource extends ResourceController{
		public function delete(){
			$token = $this->getResourceObject("APIToken","id",$this->viewFromArguments("access"));

			if($token->getRole() == "root"){
				ResponseHandler::conflict("The root access tokens cannot be invalidated.");
			}else{
				CryptKeeper::bury($token);
				ResponseHandler::deleted("Token: /".RequestHandler::getRequestURI().", has been invalidated.");
			}
		}

		protected function isInvalid(){
			return array();
		}

		protected function isConsistent($modified,$eTag){
			return true;
		}

		protected function assemblePayload($object){
			return "";
		}
	}
?>
