<?php
	class Detective extends Worker{
		private static $investigations = array();
		private static $closedCases = array();
		
		protected static function investigate($entity){
			if(!array_key_exists($entity,self::$investigations)){
				self::$investigations[$entity] = array();
			}
			
			self::$investigations[$entity][] = microtime(true);
		}
		
		protected static function closeCase($entity){
			if(!self::$investigations[$entity]){
				return;
			}
			
			if(!array_key_exists($entity,self::$closedCases)){
				self::$closedCases[$entity] = Array();
			}
			
			self::$closedCases[$entity][] = round(microtime(true)-array_pop(self::$investigations[$entity]),5);
		}
		
		protected static function report(){
			//Don't collect profiling information if the profiler is off
			if(SettingsManager::getSetting("profiler","profiling") !== "on"){
				return;
			}
			
			$api = SettingsManager::getSetting("profiler","api");
			$endpoint = "/".RequestHandler::getEndpoint()->getPath();
			
			$connection = new mysqli("localhost","wadapi_main","2#wGgy6)*3%","wadapi_main");
			if($connection->connect_error){
				fatal_error(DATABASE_CONNECT_ERROR, $connection->connect_error.".");
				return;
			}
			$connection->autocommit(FALSE);
			
			
			//Write Investigation Statistics
			$objectValues = Array();
			$queryValues = Array();
			$id = number_format(microtime(true)*10000,0,'','');
			foreach(self::$closedCases as $entity => $cases){
				$id++;
				$created = strval(time());
				
				if(class_exists($entity)){
					$class = $entity;
					$instances = count($cases);
					$loadTime = round(array_sum($cases),5);
					
					$objectValues[] = "($id,$created,$created,$api,'$endpoint','$class',$instances,$loadTime)";
				}else{
					$query = $entity;
					$executions = count($cases);
					$runtime = round(array_sum($cases),5);
					
					$queryValues[] = "($id,$created,$created,$api,'$endpoint','".$connection->escape_string($query)."',$executions,$runtime)";
				}
			}
			
			if($objectValues){
				$connection->query("INSERT INTO profiler_ObjectStatistic VALUES ".join(",",$objectValues)." ON DUPLICATE KEY UPDATE ".
							"modified=VALUES(modified),instances=VALUES(instances),loadTime=VALUES(loadTime)");
			}
			
			if($queryValues){
				$connection->query("INSERT INTO profiler_QueryStatistic VALUES ".join(",",$queryValues)." ON DUPLICATE KEY UPDATE ".
							"modified=VALUES(modified),executions=VALUES(executions),runtime=VALUES(runtime)");
			}
			
			
			//Write Endpoint Statistics
			$id = number_format(microtime(true)*10000,0,'','');
			$created = strval(time());
			$date = date("Y-m-d");
			$runtime = round(microtime(true)-$GLOBALS["scriptStartTime"],5);
			
			$connection->query("INSERT INTO profiler_EndpointStatistic VALUES($id,$created,$created,$api,'$endpoint','$date',1,$runtime) ".
						"ON DUPLICATE KEY UPDATE modified='$created',requests=requests+1,runtime=runtime+$runtime");
			
			if($connection->affected_rows == 1){
				$connection->query("INSERT INTO profiler_Resource VALUES($id,$created,$created)");
			}
			
			$connection->commit();
			$connection->close();
		}
	}
?>