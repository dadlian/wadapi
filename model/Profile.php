<?php
  use Wadapi\Authentication\TokenProfile;

  class Profile extends TokenProfile{
    public function getInvalidArguments($arguments,$token){
      $invalidArguments = array();
      return $invalidArguments;
    }

    public function build($arguments,$token){
    }

    public function deliverPayload(){
      return [];
    }

    protected function getURI(){
      return "";
    }

    protected function getURITemplate(){
      return "";
    }

    protected function getETag(){
      $eTag = "";
      return $eTag;
    }
  }
?>
