#!/usr/bin/php
<?php
	//Removes objects and queries which were not loaded or executed the last time each endpoint was requested
        $settings = dirname(__FILE__)."/../settings.xml";
        require_once(dirname(__FILE__)."/../../includes.inc");

	$objectIDs = Array();
	foreach(DatabaseAdministrator::execute("SELECT A.id as object FROM ObjectStatistic AS A JOIN (SELECT api,endpoint,MAX(modified) AS latest FROM ObjectStatistic ".
						"GROUP BY api,endpoint) AS B ON A.api = B.api AND A.endpoint = B.endpoint AND A.modified < B.latest") as $row){
		$objectIDs[] = $row["object"];			
	}
	
	if($objectIDs){
		DatabaseAdministrator::execute("DELETE FROM ObjectStatistic WHERE id IN (".join(",",$objectIDs).")");
	}

	$queryIDs = Array();
	foreach(DatabaseAdministrator::execute("SELECT A.id AS query FROM QueryStatistic AS A JOIN (SELECT api,endpoint,MAX(modified) AS latest FROM QueryStatistic ".
						"GROUP BY api,endpoint) AS B ON A.api = B.api AND A.endpoint = B.endpoint AND A.modified < B.latest") as $row){
		$queryIDs[] = $row["query"];
	}
	
	if($queryIDs){
		DatabaseAdministrator::execute("DELETE FROM QueryStatistic WHERE id IN (".join(",",$queryIDs).")");
	}
						
	Janitor::cleanup();
?>