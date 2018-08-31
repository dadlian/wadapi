<?php
	class WadapiSetup extends UtilityController{
		protected function isInvalid(){
			$invalidArguments = array();
			return $invalidArguments;
		}

		protected function getInvalidQueryParameters(){
			$invalidQueryParameters = array();
			return $invalidQueryParameters;
		}

		protected function isConsistent($modifiedDate,$eTag){
			return true;
		}

		protected function assemblePayload($tokens){
			$payload = array(
				"root"=>[
          "key"=>$tokens['root-key'],
          "secret"=>$tokens['root-secret']
        ],
        "login"=>[
          "key"=>$tokens['login-key'],
          "secret"=>$tokens['login-secret']
        ]
			);

			return $payload;
		}

		protected function post(){
      $sqlGateway = new SQLGateway();
      $searcher = new Searcher();

			//Rebuild Database
			Curator::rebuildDatabase();

			//Initialise Root API Token if necessary
			$rootKey = md5(md5(time()*rand()*rand()));
			$rootSecret = md5(md5(time()*rand()*rand()));
			$searcher->addCriterion("role",Criterion::EQUAL,"root");
			if(!$sqlGateway->findUnique("APIToken",$searcher)){
				$sqlGateway->save(new APIToken("root",0,md5($rootKey),md5($rootSecret),md5($rootKey.$rootSecret)));
			}

			//Initialise Login API Token if necessary
			$loginKey = md5(md5(time()*rand()*rand()));
			$loginSecret = md5(md5(time()*rand()*rand()));
      $searcher->clearCriteria();
			$searcher->addCriterion("role",Criterion::EQUAL,"login");
			if(!$sqlGateway->findUnique("APIToken",$searcher)){
				$sqlGateway->save(new APIToken("login",0,md5($loginKey),md5($loginSecret),md5($loginKey.$loginSecret)));
			}

      $tokens = [
        "root-key"=>$rootKey,
        "root-secret"=>$rootSecret,
        "login-key"=>$loginKey,
        "login-secret"=>$loginSecret
      ];

			ResponseHandler::created($this->assemblePayload($tokens),$this->getBase()."/wadapi/setup");
		}
	}
?>
