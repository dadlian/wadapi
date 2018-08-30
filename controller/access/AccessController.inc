<?php
	abstract class AccessController extends ResourceController{
		protected function isInvalid(){
			$invalidArguments = array();

			$validRoles = preg_split("/,/",SettingsManager::getSetting("api","roles"));

			$role = $this->getFromContent("role");
			if(!is_string($role) || !in_array($role,$validRoles)){
				$invalidArguments[] = "role";
			}

			return $invalidArguments;
		}

		protected function getTokenProfileClass(){
			foreach(get_declared_classes() as $declaredClass){
				$reflectedClass = Mirror::reflectClass($declaredClass);
				if($reflectedClass->descendsFrom("TokenProfile")){
					$profileClass = $declaredClass;
				}
			}

			return $profileClass;
		}
	}
?>
