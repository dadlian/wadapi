<?php
	class EndpointsGateway extends XMLGateway{
		public function __construct(){
			//Copy User Endpoints to Core
			$projectPath = SettingsManager::getSetting("install","projectPath");
			copy("$projectPath/endpoints.xml","$projectPath/core/endpoints.xml");

			parent::__construct("core/endpoints.xml");

			//Add System Endpoints
			$loginWrite = new Role(["write"]);
			$loginReadWrite = new Role(["read","write"]);

			$systemEndpoints = array();
			$systemEndpoints[] = new Endpoint("Access Collection","access","AccessCollection",["login"=>$loginWrite],[],["role"]);
			$systemEndpoints[] = new Endpoint("Access Resource","access/@","AccessResource",["login"=>$loginReadWrite],["access"],["role"]);
			$systemEndpoints[] = new Endpoint("Token Collection","access/@/tokens","TokenCollection",["login"=>$loginWrite],["access"],[]);
			$systemEndpoints[] = new Endpoint("Token Resource","access/@/tokens/active","TokenResource",["login"=>$loginWrite],["access"],[]);

			$this->save($systemEndpoints);
		}

		public function find(){
			$arguments = array_merge(array('Endpoint'),func_get_args());
			return call_user_func_array(array('parent','find'), $arguments);
		}
	}
?>
