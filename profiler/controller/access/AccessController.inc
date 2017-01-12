<?php
	abstract class AccessController extends ResourceController{
		protected function isInvalid(){
			$invalidArguments = array();
			
			if(!is_string($this->getFromContent("role")) || !in_array($this->getFromContent("role"),array("login","master","consumer"))){
				$invalidArguments[] = "role";
			}
			
			return $invalidArguments;
		}
	}
?>