<?php
	class CallCollection extends CallController{
		protected function isConsistent($modifiedDate,$eTag){
			return true;
		}
	}
?>