<?php
	abstract class APIController extends PagedResourceController{
		protected function isInvalid(){
			$invalidArguments = array();
			
			if(!is_string($this->getFromContent("root")) || !filter_var($this->getFromContent("root"), FILTER_VALIDATE_URL)){
				$invalidArguments[] = "root";
			}
			
			return $invalidArguments;
		}
		
		protected function getInvalidQueryParameters(){
			$invalidQueryParameters = array();
			return $invalidQueryParameters;
		}
		
		protected function getRecordCount(){
			$countResult = DatabaseAdministrator::execute("SELECT COUNT(id) as apis FROM API");
			$count = $countResult[0]["apis"];
			
			return $count;
		}
		
		protected function buildPageEntries($start,$count){
			$payload = array();
			$sqlGateway = new SQLGateway();
		
			foreach($sqlGateway->find("API",new Searcher(),new Sorter,$count,$start) as $api){
				$payload[] = $this->assemblePayload($api);
			}
			
			return $payload;
		}
		
		protected function assemblePayload($api){
			$payload = array(
				"self"=>$api->getURI(),
				"root"=>$api->getRoot(),
				"daily-requests"=>$api->getDailyRequests(),
				"endpoints"=>array(
					"self"=>"{$api->getURI()}/endpoints",
					"total"=>$api->getTotalEndpoints()
				)
			);
			
			return $payload;
		}
	}
?>