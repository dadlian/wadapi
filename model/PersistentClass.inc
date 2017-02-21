<?php
	abstract class PersistentClass extends WadadliClass{
		/*
		 * Keeps track of IDs dispensed to new objects to avoid duplicates
		 */
		private static $dispensedIDs = array();
		
		/*
		 * A unique integer identifying this object from other instances of the same class.
		 */
		/** @WadapiString(required=true, min=0, max=20)*/
		protected $id;
		
		/*
		 * A unix timestamp representing when this element was created.
		 */
		/** @WadapiString */
		protected $created;
		
		/*
		 * A unix timestamp representing when this element last had one of its values changed.
		 */
		/** @WadapiString */
		protected $modified;
		
		/*
		 * An instance of the Gateway type used to load this object
		 */
		/** @Object(class='Gateway') */
		protected $gateway;

		/*
		 * Boolean flag indicating whether or not this object's data has been completely loaded.
		 */
		/** @Boolean(required=true, default=true) */
		protected $loaded;

		/*
		 * Constructer returns unbound instance of the object. An ID is automatically generated and will be
		 * used to refer to the object going forward.
		 */		
		public function __construct(){
			$arguments = func_get_args();
			
			//Add default gateway and loaded values to constructor arguments
			$arguments = array_merge(array(self::generateID(), strval(time()), strval(time()), null, true), $arguments);
			call_user_func_array(array('parent','__construct'), $arguments);
			
			QuarterMaster::store($this);
		}
		
		/*
		 * Pseudo-constructor that returns a bound version of the object with all its fields null. This
		 * class will be loaded when one of its fields is accessed.
		 */
		public static function bindInstance($id, $gateway){
			$currentClass = get_called_class();
			$instance = new $currentClass();
			
			$instance->setId($id);
			$instance->setGateway($gateway);
			$instance->setLoaded(false);
			
			return $instance;
		}
		  
		  /*
		   * Default persistent level behaviour that returns a PersistentCLass ID as its string representation
		   */
		  public function __toString(){
			return $this->getId();
		  }
		
		/*
		 * Overridden getX method to account for lazy loading
		 */
		 protected function getX($searchProperty){
			if(!in_array($searchProperty,array('id','gateway','loaded'))){
				$this->loadData();
			}
			
			if(Mirror::reflectProperty($this,$searchProperty)->getAnnotation()->isCollection() && is_null($this->$searchProperty)){
				$this->$searchProperty = $this->loadCollections(
					Mirror::reflectProperty($this,$searchProperty)->getAnnotation()->getContainedType(), //ListAnnotation
					get_class($this).capitalise($searchProperty), //ListTable
					array($this->getId()), //Current Object ID
					strtolower(get_class($this)) //ParentTable
				)[$this->getId()];
			}
			
		 	return parent::getX($searchProperty);
		 }
		
		/*
		 * Overridden setX method to update last modified time
		 */
		 protected function setX($propertyName, $newValue, $calledMethod){
			$oldValue = $this->$propertyName;
			if($oldValue && $newValue != $oldValue){
				$this->modified = strval(time());
			}
			
			if(!$this->modified){
				$this->modified = strval(time());
			}
			
			return parent::setX($propertyName, $newValue, $calledMethod);
			
		 }
		 
		 /*
		  * Returns an object's creation time object based on its ID
		  */
		 protected function getCreationTime(){
			$createTime = $this->getCreated();
			return new WadadliDate(intval(date('Y',$createTime)),intval(date('m',$createTime)),intval(date('d',$createTime)),
						intval(date('G',$createTime)),intval(date('i',$createTime)),intval(date('s',$createTime)));
		 }
		 
		 /*
		  * Generates a unique ID number for the current object based on the current Unix timestamp
		  */
		  private static function generateID(){
			$id = number_format(microtime(true)*10000,0,'','');
			
			do{
				$id++;
			}while(in_array($id, self::$dispensedIDs));
			
			self::$dispensedIDs[] = $id;
			return strval($id);
		  }

		/*
		 * If an instance of the object has been bound but not loaded.
		 */
		 private function loadData(){
			if($this->loaded || !$this->getGateway()){
				return;
			}
			
			//Load object data
			$searcher = new Searcher();
			$searcher->addCriterion("id",Criterion::EQUAL,$this->getId());
			
			$className = get_class($this);
			$object = $this->gateway->findUnique($className,$searcher);
			
			if(!$object){
				fatal_error(DATA_ACCESS_ERROR, "Instance of class ".get_class($this)." has an invalid binding ".
								"(Gateway: ".get_class($this->getGateway()).", ID: {$this->getId()}).");
				return;
			}
			
			$class = Mirror::reflectClass($className);
			foreach($class->getProperties(false) as $property){
				$propertyName = $property->getName();
				$this->$propertyName = $object->$propertyName;	
			}
			
			$this->loaded = true;
		}
		
		private function loadCollections($listAnnotation, $listTable, $parentIds, $parentColumn = "parentList"){
			$collections = array();
			
			$fields = "id,$parentColumn,name";
			if(!$listAnnotation->isCollection()){
				$fields .= ",value";
			}
			
			$results = array();
			if(DatabaseAdministrator::tableExists($listTable) && $parentIds){
				$results = DatabaseAdministrator::execute("SELECT $fields FROM $listTable WHERE $parentColumn IN (".implode(",",$parentIds).")");
			}
			
			if($listAnnotation->isCollection()){
				$ids = array();
				
				foreach($results as $result){
					$ids[] = $result['id'];
				}
				
				$nestedCollections = $this->loadCollections($listTable."List", $listAnnotation->getContainedType(), $ids);
			}
			
			foreach($results as $result){
				if(!array_key_exists($result[$parentColumn], $collections)){
					$collections[$result[$parentColumn]] = array();
				}
				
				if($listAnnotation->isDate()){
					$datePieces = preg_split("/[-: ]/", $result['value']);
					$value = new WadadliDate(intval($datePieces[0]),intval($datePieces[1]),intval($datePieces[2]),
										intval($datePieces[3]),intval($datePieces[4]),intval($datePieces[5]));
					$collections[$result[$parentColumn]][$result['name']] = $value;
				}else if($listAnnotation->isObject()){
					if($result['value']){
						//Set object stub for lazy loading
						$containedClass = $listAnnotation->getObjectClass();
						$collections[$result[$parentColumn]][$result['name']] = call_user_func_array(array($containedClass,"bindInstance"),array($result['value'],new SQLGateway()));
					}else{
						$collections[$result[$parentColumn]][$result['name']] = null;
					}
				}else if($listAnnotation->isCollection()){
					if(array_key_exists($result['id'], $nestedCollections)){
						$collections[$result[$parentColumn]][$result['name']] = $nestedCollections[$result['id']];
					}else{
						$collections[$result[$parentColumn]][$result['name']] = array();
					}
				}else{
					$collections[$result[$parentColumn]][$result['name']] = $listAnnotation->isBoolean()?boolval($result['value']):$result['value'];
				}
			}
			
			return $collections;
		 }
	}
?>