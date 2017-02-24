<?php
	class Curator extends Worker{
		//Keep track of classes processed this session so we don't revisit them
		private static $classesSeen = array();
				
		//An array of all list properties seen for table creation
		private static $listProperties = array();
		
		/*
		 * Rebuilds the database based on the current PersistentClass definitions in the system
		 */
		protected static function rebuildSQLDatabase(){
			foreach($persistentClasses as $class){
				//Manage table hierarchy for object class
				self::manageTableForClass($class);
				
				while(sizeof(self::$listProperties) > 0){
					$listProperty = array_shift(self::$listProperties);
					self::manageTableForList($listProperty);
				}
				
				//Re-initialise seen class hierarchy for this object
				self::$classesSeen = array();
				self::$listProperties = array();
			}
		}
		 
		/*
		 * Ensures that a table representation of an object already exists, and decides how to proceed.
		 */
		private static function manageTableForClass($class){
			if(!in_array($class->getName(), self::$classesSeen)){
				self::$classesSeen[] = $class->getName();
				if(DatabaseAdministrator::tableExists($class->getName())){
					//Add any new class fields, or change field types for existing table
					self::alterTableForClass($class);
				}else{
					self::createTableForClass($class);
				}
			}
		}
		
		/*
		 * Checks whether a table representation of the list already exists, and decides how to proceed based on the result.
		 */
		private static function manageTableForList($property){
			$class = $property->getDeclaringClass();
			
			if(!in_array($class->getName().capitalise($property->getName()), self::$classesSeen)){
				self::$classesSeen[] = $class->getName().capitalise($property->getName());
				if(DatabaseAdministrator::tableExists($class->getName().capitalise($property->getName()))){
					self::alterTableForList($property);
				}else{
					self::createTableForList($property);
				}
			}
		}
			 
		/*
		 * Creates a database table corresponding to a certain PersistentClass if it does not already exist
		 */
		private static function createTableForClass($class){
			//Every class will need to include an ID field to associate with its parent
			$classProperties = array(Mirror::reflectProperty($class->getName(), "id"),Mirror::reflectProperty($class->getName(), "created"),Mirror::reflectProperty($class->getName(), "modified"));
			$classProperties = array_merge($classProperties, $class->getProperties(false));
			
			//Check that necessary parent table exist. Create them if not
			$classHierarchy = $class->getParentClass()->getClassHierarchy();
			$persistentClass = Mirror::reflectClass("PersistentClass");
			$classHierarchy = array_diff($classHierarchy, $persistentClass->getClassHierarchy());
					
			foreach($classHierarchy as $nextClass){
				self::manageTableForClass($nextClass);
			}
						
			$createStatement = "CREATE TABLE {$class->getName()}(";
			$idType = "";
			
			foreach($classProperties as $property){
				if($property->getAnnotation()->isCollection()){
					continue;
				}
				
				$columnType = "";
				$propertyName = $property->getName();
				$columnType = self::getColumnType($property->getAnnotation());
				
				//Create object field table
				if($property->getAnnotation()->isObject()){
					self::manageTableForClass(Mirror::reflectClass($property->getAnnotation()->getObjectClass()));
				}
				
				if($property->getName() == "id"){
					$idType = $columnType;
					$columnType .= " PRIMARY KEY";
				}				
				
				$createStatement .= "{$property->getName()} $columnType,";
			}
			
			//If class is not hierarchy root, add foreign key to parent object
			if($class->getParentClass()->getName() != "PersistentClass"){
				$parentClassName = $class->getParentClass()->getName();
				$constraintName = "fk_".SettingsManager::getSetting("database","tableprefix")."_".substr(strtolower($class->getName()), 0, 25)."_id";
				$createStatement .= "CONSTRAINT $constraintName FOREIGN KEY (id) 
							REFERENCES $parentClassName (id) ON DELETE CASCADE ON UPDATE CASCADE,";
			}
			
			//Add foreign keys for object references
			foreach($classProperties as $property){
				if($property->getAnnotation()->isObject()){
					$propertyName = $property->getName();
					$objectTable = $property->getAnnotation()->getObjectClass();
					$constraintName = "fk_".SettingsManager::getSetting("database","tableprefix")."_".substr(strtolower($class->getName()),0,25).substr("_$propertyName",0,20);
					$createStatement .= "CONSTRAINT $constraintName FOREIGN KEY ($propertyName) REFERENCES $objectTable (id) 
								ON DELETE SET NULL ON UPDATE CASCADE,";
				}
			}
			
			//Remove trailing comma from $createStatement
			$createStatement = substr($createStatement, 0, strlen($createStatement) - 1);
			$createStatement .= ")ENGINE=INNODB";
			DatabaseAdministrator::execute($createStatement);
			
			//Mark list property for later table creation
			foreach($classProperties as $property){
				if($property->getAnnotation()->isCollection()){
					self::$listProperties[] = $property;
				}
			}
		}
			 
		/*
		 * Creates a database table corresponding to stored object lists when a class has a one-to-many or many-to-many relationship
		 */
		private static function createTableForList($property){
			$propertyName = $property->getName();
			$className = $property->getDeclaringClass()->getName();
			$annotation = $property->getAnnotation();
			$listAnnotations = array();
			
			//Get a listing of all the nested types of the Collection property
			while($annotation->isCollection()){
				$annotation = $annotation->getContainedType();
				$listAnnotations[] = $annotation;
			}
			
			//Traverse type list to create tables for each nested list
			for($i = 0; $i < sizeof($listAnnotations); $i++){
				$nextAnnotation = $listAnnotations[$i];
				$tableName = $className.capitalise($propertyName).str_repeat("List", $i);
				if(DatabaseAdministrator::tableExists($tableName)){
					continue;
				}
				
				$createStatement = "CREATE TABLE $tableName(".strtolower($className)." VARCHAR(20) NOT NULL,name VARCHAR(128)";
				
				if($i > 0){
					$createStatement .= ",parentKey VARCHAR(128) NOT NULL";
				}
				
				if($nextAnnotation->isObject()){
					$objectClass = Mirror::reflectClass($nextAnnotation->getObjectClass());
					self::manageTableForClass($objectClass);
					
					$constraintName = "fk_".SettingsManager::getSetting("database","tableprefix")."_".substr(strtolower($tableName),0,25)."_value";
					$createStatement .= ",value VARCHAR(20),CONSTRAINT $constraintName
								FOREIGN KEY (value) REFERENCES {$objectClass->getName()} (id) 
								ON UPDATE CASCADE ON DELETE CASCADE,";
				}else if(!$nextAnnotation->isCollection()){
					$columnType = self::getColumnType($nextAnnotation);
					$createStatement .= "value $columnType,";
				}
				
				//Specify Primary Key Constraint
				$createStatement .= ",PRIMARY KEY(".strtolower($className).",name)";
				
				//Specify owning object column constraint
				$tableAbbr = substr(strtolower($tableName), 0, 15).substr(strtolower($tableName), strlen($tableName)-10,strlen($tableName));
				$constraintName = "fk_".SettingsManager::getSetting("database","tableprefix")."_".$tableAbbr."_".substr(strtolower($className),0,20);
				$createStatement .= ",CONSTRAINT $constraintName FOREIGN KEY (".strtolower($className).") REFERENCES $className (id)".
							" ON UPDATE CASCADE ON DELETE CASCADE)ENGINE=INNODB";
				
				if($i > 0){
					$constraintName = "fk_".SettingsManager::getSetting("database","tableprefix")."_".$tableAbbr."_parentKey";
					$createStatement .= ",CONSTRAINT $constraintName FOREIGN KEY (parentKey) REFERENCES ".
							     substr($tableName, 0, strlen($tableName) - 4).
							     " (name) ON UPDATE CASCADE ON DELETE CASCADE)ENGINE=INNODB";
				}
				
				DatabaseAdministrator::execute($createStatement);
			}
		}
		
		/*
		 * Change a database table to reflect new class definitions.
		 */
		private static function alterTableForClass($class){
			//Get table description
			$tableName = $class->getName();
			$tableDescription = DatabaseAdministrator::execute("DESC $tableName");
			$classProperties = $class->getProperties(false);
			$tableFieldTypeMap = array();
			$tableForeignKeyMap = array();
			
			//Check that necessary parent tables exist. Create them if not
			$classHierarchy = $class->getParentClass()->getClassHierarchy();
			$persistentClass = Mirror::reflectClass("PersistentClass");
			$classHierarchy = array_diff($classHierarchy, $persistentClass->getClassHierarchy());
					
			foreach($classHierarchy as $nextClass){
				self::manageTableForClass($nextClass);	
			}
			
			foreach($tableDescription as $column){
				$tableFieldTypeMap[$column['Field']] = $column['Type'];
				$tableForeignKeyMap[$column['Field']] = $column['Key'];
			}
			
			foreach($classProperties as $property){
				$changed = true;
				$propertyName = $property->getName();
				
				if(in_array($property->getName(), array_keys($tableFieldTypeMap))){
					$oldColumnType = $tableFieldTypeMap[$property->getName()];
					$oldColumnKey = $tableForeignKeyMap[$property->getName()];
				}else{
					$oldColumnType = "";
					$oldColumnKey = "";
				}
				
				$newColumnType = self::getColumnType($property->getAnnotation());
				
				//Create object field table
				if($property->getAnnotation()->isObject()){
					self::manageTableForClass(Mirror::reflectClass($property->getAnnotation()->getObjectClass()));
				}
				
				//Add any missing columns to table
				if(!$oldColumnType && !$property->getAnnotation()->isCollection()){
					$oldColumnType = $newColumnType;
					DatabaseAdministrator::execute("ALTER TABLE $tableName ADD $propertyName $newColumnType");
				
				//Alter table if a field type has changed
				}else if(($property->getAnnotation()->isObject() && (!preg_match("/^varchar/", $oldColumnType) 
						|| $oldColumnKey != "MUL")) ||
					($property->getAnnotation()->isInteger() && (!preg_match("/^int/", $oldColumnType) 
						|| $oldColumnKey == "MUL")) || 
					(($property->getAnnotation()->isFloat() || $property->getAnnotation()->isMonetary()) 
						&& $oldColumnType != "float") || 
					($property->getAnnotation()->isBoolean() && !preg_match("/tinyint/", $oldColumnType)) || 
					($property->getAnnotation()->isText() && $oldColumnType != "text") ||
					($property->getAnnotation()->isString() && !$property->getAnnotation()->isText() && 
						$property->getAnnotation()->getMax() &&
						!preg_match("/^varchar\({$property->getAnnotation()->getMax()}/", $oldColumnType)) ||
					($property->getAnnotation()->isString() && !$property->getAnnotation()->isText() && 
						!$property->getAnnotation()->getMax() && !preg_match("/^varchar\(256/", $oldColumnType))){
					
					//Set all values in changed column to NULL to ensure compatability with new type
					DatabaseAdministrator::execute("UPDATE $tableName SET $propertyName=NULL");
					
					//If column was previously object and no longer is, drop foreign key
					if($oldColumnKey == "MUL" && !$property->getAnnotation()->isObject()){
						$columnKey = "fk_".SettingsManager::getSetting("database","tableprefix")."_".substr(strtolower($tableName),0,25).substr("_$propertyName",0,20);
						DatabaseAdministrator::execute("ALTER TABLE $tableName DROP FOREIGN KEY $columnKey");
						DatabaseAdministrator::execute("ALTER TABLE $tableName DROP KEY $columnKey");
					}
					
					DatabaseAdministrator::execute("ALTER TABLE $tableName CHANGE $propertyName $propertyName $newColumnType");
					
				//Otherwise table has not changed				
				}else{
					$changed = false;
				}
					
				//If column has changed to object add foreign key
				if($changed && $property->getAnnotation()->isObject()){
					$columnKey = "fk_".SettingsManager::getSetting("database","tableprefix")."_".substr(strtolower($tableName),0,25).substr("_$propertyName",0,20);
					$objectTable = $property->getAnnotation()->getObjectClass();
					DatabaseAdministrator::execute("ALTER TABLE $tableName ADD CONSTRAINT $columnKey 
									FOREIGN KEY ($propertyName) REFERENCES $objectTable (id) 
									ON DELETE SET NULL ON UPDATE CASCADE");
				}
			}
			
			//Mark list property for later table creation
			foreach($classProperties as $property){
				if($property->getAnnotation()->isCollection()){
					self::$listProperties[] = $property;
				}
			}
		}
		
		/*
		 * Change a list database table to reflect new class definitions.
		 */
		private static function alterTableForList($property){
			$propertyName = $property->getName();
			$className = $property->getDeclaringClass()->getName();
			$annotation = $property->getAnnotation();
			$listAnnotations = array();
			
			//Get a listing of all the nested types of the Collection property
			while($annotation->isCollection()){
				$annotation = $annotation->getContainedType();
				$listAnnotations[] = $annotation;
			}
			
			//Traverse type list in order to alter tables for each nested list
			for($i = 0; $i < sizeof($listAnnotations); $i++){
				$nextAnnotation = $listAnnotations[$i];
				$tableName = $className.capitalise($propertyName).str_repeat("List", $i);
				$columnName = $propertyName;
				
				if(!DatabaseAdministrator::tableExists($tableName)){
					self::createTableForList($property);
				}else{
					$tableDescription = DatabaseAdministrator::execute("DESC $tableName");
					$dataIndex = sizeof($tableDescription) - 1;
					
					$storedPropertyName = $tableDescription[$dataIndex]['Field'];
					$storedAnnotationType = $tableDescription[$dataIndex]['Type'];
					$storedKey = $tableDescription[$dataIndex]['Key'];
					
					$newColumnType = self::getColumnType($nextAnnotation);
					if($nextAnnotation->isObject()){
						self::manageTableForClass(Mirror::reflectClass($nextAnnotation->getObjectClass()));
					}
					
					$typeChange = false;
					if(($nextAnnotation->isObject() && (!preg_match("/^varchar/", $storedAnnotationType) || $storedKey != "MUL"))||
					   ($nextAnnotation->isInteger() && (!preg_match("/^int/", $storedAnnotationType) || $storedKey == "MUL"))||
					   (($nextAnnotation->isFloat() || $nextAnnotation->isMonetary()) && $storedAnnotationType != "float") || 
					   ($nextAnnotation->isBoolean() && !preg_match("/tinyint/", $storedAnnotationType)) || 
					   ($nextAnnotation->isText() && $storedAnnotationType != "text") ||
					   ($nextAnnotation->isString() && !$nextAnnotation->isText() &&  $nextAnnotation->getMax() 
						&& !preg_match("/^varchar\({$nextAnnotation->getMax()}/", $storedAnnotationType)) ||
					   ($nextAnnotation->isString() && !$nextAnnotation->isText() && !$nextAnnotation->getMax() 
						&& !preg_match("/^varchar\(256/", $storedAnnotationType))){
						$typeChange = true;
					}
					
					//If property recently changed from collection add element column
					if(sizeof($tableDescription) == 3 && !$nextAnnotation->isCollection()){
						DatabaseAdministrator::execute("ALTER TABLE $tableName ADD COLUMN value $newColumnType DEFAULT NULL");						
					//If list type has become collection, drop element column
					}else if($nextAnnotation->isCollection() && sizeof($tableDescription) > 3){
						DatabaseAdministrator::execute("ALTER TABLE $tableName DROP COLUMN ".$tableDescription[3]['Field']);
					}
					
					
					$tableAbbr = substr(strtolower($tableName),0,15).substr(strtolower($tableName),strlen($tableName)-10,strlen($tableName));
					if($typeChange){
						//Set all values of old column to NULL to ensure compatability with new type
						DatabaseAdministrator::execute("UPDATE $tableName SET value=NULL");
						
						//If there is a key on the changed column drop it
						if($storedKey == "MUL"){
							$columnKey = "fk_".SettingsManager::getSetting("database","tableprefix")."_".$tableAbbr."_value";
							DatabaseAdministrator::execute("ALTER TABLE $tableName DROP FOREIGN KEY $columnKey");
							DatabaseAdministrator::execute("ALTER TABLE $tableName DROP KEY $columnKey");	
						}
							
						if(sizeof($tableDescription) > 3){
							DatabaseAdministrator::execute("ALTER TABLE $tableName CHANGE value value $newColumnType");
						}
						
						//If new column is object or collection add Foreign Key to class table
						if($nextAnnotation->isObject()){
							$columnKey = "fk_".SettingsManager::getSetting("database","tableprefix")."_{$tableAbbr}_value";
							if($nextAnnotation->isObject()){;
								self::manageTableForClass(Mirror::reflectClass($nextAnnotation->getObjectClass()));
								$foreignKeyTable = $nextAnnotation->getObjectClass();
							}
							
							DatabaseAdministrator::execute("ALTER TABLE $tableName ADD CONSTRAINT $columnKey 
												FOREIGN KEY(value)
												REFERENCES $foreignKeyTable(id)
												ON DELETE CASCADE ON UPDATE CASCADE");
						}
					}
				}
			}
		}
		
		/*
		 * Determine the MySQL column type for each possible WadadliClass property type
		 */
		private static function getColumnType($annotation){
			if($annotation->isInteger()){
				$columnType = "INT";
			}else if($annotation->isFloat() || $annotation->isMonetary()){
				$columnType = "FLOAT";
			}else if($annotation->isBoolean()){
				$columnType = "BOOLEAN";
			}else if($annotation->isObject()){
				$columnType = "VARCHAR(20)";
			}else if($annotation->isText()){
				$columnType = "TEXT";
			}else if($annotation->isString()){
				$limit = 256;
				if($annotation->getMax()){
					$limit = $annotation->getMax();
				}
				
				$columnType = "VARCHAR($limit)";
			}else if($annotation->isCollection()){
				$columnType = "";
			}
			
			return $columnType;
		}
	}
?>