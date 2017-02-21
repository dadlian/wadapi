<?php	
	abstract class WadadliAnnotation extends Annotation{
		public $required;
	}
	
	class Object extends WadadliAnnotation{
		public $class;
	}
	
	class Date extends WadadliAnnotation {}
	
	abstract class DefaultedAnnotation extends WadadliAnnotation{
		public $default;
	}
	
	class Boolean extends DefaultedAnnotation {}
	
	abstract class ValuedAnnotation extends DefaultedAnnotation{
		public $values;
	}
	
	abstract class RangedAnnotation extends ValuedAnnotation{
		public $min;
		public $max;
	}
	
	class WadapiString extends RangedAnnotation {}
	class URL extends WadapiString {}
	class Email extends WadapiString {}
	class Phone extends WadapiString {}
	class Password extends WadapiString {}
	class File extends WadapiString {}
	class Image extends File {
		public $height;
		public $width;
	}
	class Text extends WadapiString {}
	
	class Integer extends RangedAnnotation {}
	class WadapiFloat extends RangedAnnotation {}
	class Monetary extends RangedAnnotation {}
	
	class Collection extends RangedAnnotation{
		public $type;
	}
?>