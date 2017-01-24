<?php
	class VariantCollection extends VariantController{
		protected function isConsistent($modifiedDate,$eTag){
			return true;
		}
	}
?>