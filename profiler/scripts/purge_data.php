#!/usr/bin/php
<?php
	//Removes all data older than 28 days
        $settings = dirname(__FILE__)."/../settings.xml";
        require_once(dirname(__FILE__)."/../../includes.inc");

	$cutoff = date("Y-m-d h:i:s",strtotime("-28days"));
	DatabaseAdministrator::execute("DELETE FROM Resource WHERE id IN (SELECT id FROM EndpointStatistic) AND FROM_UNIXTIME(modified) < '$cutoff'");
	DatabaseAdministrator::execute("DELETE FROM EndpointStatistic WHERE FROM_UNIXTIME(modified) < '$cutoff'");
	DatabaseAdministrator::execute("DELETE FROM CallStatistic WHERE FROM_UNIXTIME(modified) < '$cutoff'");
	DatabaseAdministrator::execute("DELETE FROM CustomStatistic WHERE FROM_UNIXTIME(modified) < '$cutoff'");
	DatabaseAdministrator::execute("DELETE FROM ObjectStatistic WHERE FROM_UNIXTIME(modified) < '$cutoff'");
	DatabaseAdministrator::execute("DELETE FROM QueryStatistic WHERE FROM_UNIXTIME(modified) < '$cutoff'");
	
	Janitor::cleanup();
?>
