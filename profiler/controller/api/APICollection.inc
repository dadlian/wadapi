<?php
	class APICollection extends APIController{
		public function post(){
			$sqlGateway = new SQLGateway();
			
			//Create Access Token
			$api = new API($this->getFromContent("root"));
			$sqlGateway->save($api);
			
			$payload = $this->assemblePayload($api);
			ResponseHandler::created($payload,$api->getURI());
		}
		
		protected function isConsistent($modifiedDate,$eTag){
			return true;
		}
	}
?>