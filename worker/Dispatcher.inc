<?php
	class Dispatcher extends Worker{
		protected static function dispatchRequest(){
			$sqlGateway = new SQLGateway();
			$searcher = new Searcher();
			
			if(RequestHandler::isTest()){
				SettingsManager::changeSource("wadadli/conf/settings.testing.xml");
			}
			
			//If there is no controller mapped to this endpoint
			if(!RequestHandler::getEndpoint() || !class_exists(RequestHandler::getEndpoint()->getController()) || !Mirror::reflectClass(RequestHandler::getEndpoint()->getController())->descendsFrom("ResourceController")){
				ResponseHandler::missing("The requested endpoint /".RequestHandler::getRequestURI()." does not exist on this server.");
			}
			
			//Answer OPTIONS requests without requiring authorisation
			if(strtoupper(RequestHandler::getMethod()) == "OPTIONS"){
				$controllerClass = RequestHandler::getEndpoint()->getController();
				$controller = new $controllerClass(RequestHandler::getURIArguments());
				$controller->options();
				exit;
			}
			
			//Ensure request is authenticated
			if(!RequestHandler::getAuthorisation()){
				ResponseHandler::unauthorised("Please use Basic Authentication to authorise this request.");
			}
			
			//Ensure request authentication is valid
			$authorisation = RequestHandler::getAuthorisation();
			$accessSecret = $authorisation["secret"];
			
			$token = RequestHandler::getAuthenticatedToken();
			$method = strtolower(RequestHandler::getMethod());
			
			$isValidKey = $token;
			$isFresh = $isValidKey && (!$token->getExpires() || $token->getExpires() > time());
			$isValidSecret = $isValidKey && $isFresh && $token->getAccessSecret() == md5($accessSecret); 
			$isValidRefresh = $isValidKey && $token->getRefreshSecret() == md5($accessSecret);
			$isRefreshRequest = preg_match("/^access\/[0-9]+\/tokens$/",RequestHandler::getRequestURI());
			$isValidRole = $isValidKey && ($token->getRole() == "root" || RequestHandler::getEndpoint()->viewFromRoles($token->getRole()));
			$isRoot = $isValidRole && $token->getRole() == "root";
			$activeRole = $isValidRole?RequestHandler::getEndpoint()->viewFromRoles($token->getRole()):null;
			
			$hasPermission = $isRoot || ($isValidRole && ((in_array($method,array("get")) && in_array("read",$activeRole->getPermissions())) ||
							in_array($method,array("put","post","delete")) && in_array("write",$activeRole->getPermissions())));
			
			if(($isValidRole && $hasPermission && $isValidSecret) || ($isRefreshRequest && $isValidRefresh)){
				if(RequestHandler::getAuthenticatedToken()->getRole() == "test" && !RequestHandler::isTest()){
					ResponseHandler::forbidden("The provided tokens do not have permission to perform this action.");
				}
			
				$controllerClass = RequestHandler::getEndpoint()->getController();
				$controller = new $controllerClass(RequestHandler::getURIArguments());
				$controller->execute();
			}else{
				ResponseHandler::forbidden("The provided tokens do not have permission to perform this action.");
			}
		}
	}
?>