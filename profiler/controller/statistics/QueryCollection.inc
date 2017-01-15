<?php
	class QueryCollection extends QueryController{
		protected function isConsistent($modifiedDate,$eTag){
			return true;
		}
	}
?>