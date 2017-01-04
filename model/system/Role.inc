<?php
	class Role extends PersistentClass{
		/** @Collection(type=@String) */
		protected $permissions;
	}
?>