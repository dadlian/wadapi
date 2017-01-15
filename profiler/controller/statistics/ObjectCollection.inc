<?php
	class ObjectCollection extends ObjectController{
		protected function isConsistent($modifiedDate,$eTag){
			return true;
		}
	}
?>