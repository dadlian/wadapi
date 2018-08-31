<?php
	class SettingsGateway{
		private $settings;

		public function __construct(){
			//Load User Settings
			$this->settings = json_decode(file_get_contents(dirname(__FILE__)."/../../../settings.json"),true);
		}

		public function find($name,$unique=false){
			$matchingSettings = array();

			foreach($this->settings as $setting => $values){
				if($name == $setting){
					$matchingSettings[] = new Settings($setting,$values);

					if($unique){
						break;
					}
				}
			}

			if($unique){
				return(sizeof($matchingSettings) > 0)?$matchingSettings[0]:null;
			}else{
				return $matchingSettings;
			}
		}

		public function findUnique($name){
			return $this->find($name,true);
		}
	}
?>
