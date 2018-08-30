<?php
	$GLOBALS['scriptStartTime'] = microtime(true);

	//Initialise getallheaders() for NGinx
	if (!function_exists('getallheaders')) {
		function getallheaders() {
			$headers = [];
			foreach ($_SERVER as $name => $value) {
				if (substr($name, 0, 5) == 'HTTP_') {
				$headers[str_replace(' ', '-', ucwords(strtolower(str_replace('_', ' ', substr($name, 5)))))] = $value;
				}
			}

			return $headers;
		}
	}

	require_once(dirname(__FILE__)."/toolbox/files.php");

	//Include Third-Parth Libraries
	require_once(dirname(__FILE__)."/third_party/addendum/annotations.php");

	//Include Wadadli Libraries
	require_once(dirname(__FILE__)."/toolbox/arrays.php");
	require_once(dirname(__FILE__)."/toolbox/files.php");
	require_once(dirname(__FILE__)."/toolbox/log.php");
	require_once(dirname(__FILE__)."/toolbox/messages.php");
	require_once(dirname(__FILE__)."/toolbox/methods.php");
	require_once(dirname(__FILE__)."/toolbox/strings.php");
	require_once(dirname(__FILE__)."/toolbox/test.php");
	require_once(dirname(__FILE__)."/toolbox/Timer.inc");
	require_once(dirname(__FILE__)."/toolbox/wadadli_annotations.php");

	require_once(dirname(__FILE__)."/model/reflection/WadadliReflectionClass.inc");
	require_once(dirname(__FILE__)."/model/reflection/WadadliReflectionProperty.inc");
	require_once(dirname(__FILE__)."/model/reflection/WadadliReflectionAnnotation.inc");
	require_once(dirname(__FILE__)."/model/reflection/Mirror.inc");

	require_once(dirname(__FILE__)."/worker/Worker.inc");
	require_once(dirname(__FILE__)."/worker/Curator.inc");
	require_once(dirname(__FILE__)."/worker/CryptKeeper.inc");
	require_once(dirname(__FILE__)."/worker/DatabaseAdministrator.inc");
	require_once(dirname(__FILE__)."/worker/Detective.inc");
	require_once(dirname(__FILE__)."/worker/Dispatcher.inc");
	require_once(dirname(__FILE__)."/worker/Janitor.inc");
	require_once(dirname(__FILE__)."/worker/Magistrate.inc");
	require_once(dirname(__FILE__)."/worker/Photographer.inc");
	require_once(dirname(__FILE__)."/worker/Postman.inc");
	require_once(dirname(__FILE__)."/worker/QuarterMaster.inc");
	require_once(dirname(__FILE__)."/worker/Registrar.inc");
	require_once(dirname(__FILE__)."/worker/RequestHandler.inc");
	require_once(dirname(__FILE__)."/worker/ResponseHandler.inc");
	require_once(dirname(__FILE__)."/worker/SettingsManager.inc");

	require_once(dirname(__FILE__)."/model/WadadliClass.inc");
	require_once(dirname(__FILE__)."/model/PersistentClass.inc");

	require_once(dirname(__FILE__)."/model/api/Endpoint.inc");
	require_once(dirname(__FILE__)."/model/api/Resource.inc");
	require_once(dirname(__FILE__)."/model/api/Role.inc");
	require_once(dirname(__FILE__)."/model/api/SlugResource.inc");
	require_once(dirname(__FILE__)."/model/api/TokenProfile.inc");
	require_once(dirname(__FILE__)."/model/api/APIToken.inc");

	require_once(dirname(__FILE__)."/controller/Controller.inc");
	require_once(dirname(__FILE__)."/controller/ResourceController.inc");
	require_once(dirname(__FILE__)."/controller/PagedResourceController.inc");
	require_once(dirname(__FILE__)."/controller/APIError.inc");

	require_once(dirname(__FILE__)."/controller/access/AccessController.inc");
	require_once(dirname(__FILE__)."/controller/access/AccessCollection.inc");
	require_once(dirname(__FILE__)."/controller/access/AccessResource.inc");
	require_once(dirname(__FILE__)."/controller/access/TokenCollection.inc");
	require_once(dirname(__FILE__)."/controller/access/TokenResource.inc");

	require_once(dirname(__FILE__)."/model/database/DatabaseConnection.inc");
	require_once(dirname(__FILE__)."/model/database/MySQLConnection.inc");
	require_once(dirname(__FILE__)."/model/database/SQLString.inc");

	require_once(dirname(__FILE__)."/model/gateway/Gateway.inc");
	require_once(dirname(__FILE__)."/model/gateway/XMLGateway.inc");
	require_once(dirname(__FILE__)."/model/gateway/SQLGateway.inc");
	require_once(dirname(__FILE__)."/model/gateway/EndpointsGateway.inc");
	require_once(dirname(__FILE__)."/model/gateway/SettingsGateway.inc");
	require_once(dirname(__FILE__)."/model/gateway/Criterion.inc");
	require_once(dirname(__FILE__)."/model/gateway/Searcher.inc");
	require_once(dirname(__FILE__)."/model/gateway/Sorter.inc");

	require_once(dirname(__FILE__)."/model/http/WadapiRequest.inc");
	require_once(dirname(__FILE__)."/model/http/WadapiResponse.inc");

	require_once(dirname(__FILE__)."/model/system/Grave.inc");
	require_once(dirname(__FILE__)."/model/system/Settings.inc");
	require_once(dirname(__FILE__)."/model/system/URLPattern.inc");
	require_once(dirname(__FILE__)."/model/system/WadadliMessage.inc");
	require_once(dirname(__FILE__)."/model/system/Warrant.inc");

	//Include Utility Endpoints
	require_once(dirname(__FILE__)."/controller/utilities/WadapiRebuildDatabase.inc");

	//Initialise Custom Settings Files
	if(isset($settings)){
		SettingsManager::changeSource($settings);
	}

	//Include Wadadli Project Files
	$projectPath = SettingsManager::getSetting("install","projectPath");
	require_all("$projectPath/controller");
	require_all("$projectPath/model");
?>
