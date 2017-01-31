<?php
	class Role extends PersistentClass{
		/** @Collection(type=@WadapiString) */
		protected $permissions;
	}
?>