#!/usr/bin/php
<?php
	//Removes objects and queries which were not loaded or executed the last time each endpoint was requested
        $settings = dirname(__FILE__)."/../settings.xml";
        require_once(dirname(__FILE__)."/../../includes.inc");

	$objectIDs = Array();
	foreach(DatabaseAdministrator::execute("SELECT B.id as object FROM EndpointStatistic AS A JOIN ObjectStatistic AS B ON A.api = B.api AND A.endpoint = B.endpoint AND A.modified > B.modified") as $row){
		$objectIDs[] = $row["object"];			
	}
	
	if($objectIDs){
		DatabaseAdministrator::execute("DELETE FROM ObjectStatistic WHERE id IN (".join(",",$objectIDs).")");
	}

	$queryIDs = Array();
	foreach(DatabaseAdministrator::execute("SELECT B.id as query FROM EndpointStatistic AS A JOIN QueryStatistic AS B ON A.api = B.api AND A.endpoint = B.endpoint AND A.modified > B.modified") as $row){
		$queryIDs[] = $row["query"];
	}
	
	if($queryIDs){
		DatabaseAdministrator::execute("DELETE FROM QueryStatistic WHERE id IN (".join(",",$queryIDs).")");
	}

	$callIDs = Array();
	foreach(DatabaseAdministrator::execute("SELECT B.id as externalCall FROM EndpointStatistic AS A JOIN CallStatistic AS B ON A.api = B.api AND A.endpoint = B.endpoint AND A.modified > B.modified") as $row){
		$callIDs[] = $row["externalCall"];
	}
	
	if($callIDs){
		DatabaseAdministrator::execute("DELETE FROM CallStatistic WHERE id IN (".join(",",$callIDs).")");
	}

	$customIDs = Array();
	foreach(DatabaseAdministrator::execute("SELECT B.id as customKey FROM EndpointStatistic AS A JOIN CustomStatistic AS B ON A.api = B.api AND A.endpoint = B.endpoint AND A.modified > B.modified") as $row){
		$customIDs[] = $row["customKey"];
	}
	
	if($customIDs){
		DatabaseAdministrator::execute("DELETE FROM CustomStatistic WHERE id IN (".join(",",$customIDs).")");
	}
						
	Janitor::cleanup();
?>