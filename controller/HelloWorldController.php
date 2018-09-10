<?php
  use Wadapi\Http\ResourceController;
  use Wadapi\Http\ResponseHandler;
  
  class HelloWorldController extends ResourceController{
    public function get(){
      ResponseHandler::retrieved($this->assemblePayload("Hello World"),"");
    }

    protected function isInvalid(){
      return false;
    }

    protected function isConsistent($modified,$eTag){
      return true;
    }

    protected function assemblePayload($data){
      return [
        "message"=>$data
      ];
    }
  }
?>
