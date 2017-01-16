<?php
	class CustomCollection extends CustomController{
		protected function isConsistent($modifiedDate,$eTag){
			return true;
		}
	}
?>