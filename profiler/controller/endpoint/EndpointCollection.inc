<?php
	class EndpointCollection extends EndpointController{
		protected function isConsistent($modifiedDate,$eTag){
			return true;
		}
	}
?>