<?php
	class Endpoint extends PersistentClass{
		/*
		 * The unique name that identifies this endpoint
		 */
		/** @String(required=true) */
		protected $name;
		
		/*
		 * The URL that invokes this particular endpoint
		 */
		/** @URL */
		protected $path;
		
		/*
		 * The name that identifies this endpoint and the controller that handles it
		 */
		/** @String(required=true) */
		protected $controller;
		
		/*
		 * A list of user types that have permission to execute this endpoint
		 */
		/** @Collection(type=@Object(class='Role')) */
		protected $roles;
		
		/*
		 * A list of the expected parameters to this endpoint
		 */
		/** @Collection(type=@String) */
		protected $parameters;
		
		/*
		 * A list of the required arguments to this endpoint
		 */
		/** @Collection(type=@String) */
		protected $requirements;
		
		/*
		 * A list of MIME types which this endpoint can respond with (overides the global settings)
		 */
		/** @Collection(type=@String) */
		protected $formats;
		
		/*
		 * A list of character sets in which this endpoint can respond (overides the global settings)
		 */
		/** @Collection(type=@String) */
		protected $charsets;
		
		/*
		 * A list of languages in which this endpoint can respond (overides the global settings)
		 */
		/** @Collection(type=@String) */
		protected $languages;
		
		public function getAllowedRoles(){
			$allowedRoles = array();
			foreach($this->getRoles() as $role){
				$allowedRoles[] = $role->getTitle();
			}
			return $allowedRoles;
		}
		
		public function __toString(){
			return $this->getPath();
		}
	}
?>