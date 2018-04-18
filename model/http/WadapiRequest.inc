<?php
	class WadapiRequest extends WadadliClass{
		/** @URL(required=true) */
		protected $host;

		/** @WadapiString(default="") */
		protected $endpoint;

		/** @WadapiString(values={'GET','HEAD','POST','PUT','DELETE','OPTIONS','TRACE','CONNECT'}, default='GET') */
		protected $method;

		/** @Collection(type=@WadapiString) */
		protected $arguments;

		/** @Collection(type=@WadapiString) */
		protected $headers;

		/** @WadapiString(default='text/plain') */
		protected $contentType;

		/** @Integer */
		protected $contentLength;

		/** @WadapiString */
		protected $payload;

		/** @WadapiString(default='tcp') */
		protected $transport;

		/** @Integer(default=80) */
		protected $port;

		protected function addPreferedContentType($mimeType,$priority=1.0){
			$this->addPreference("Accept",$mimeType,$priority);
		}

		protected function addPreferedCharset($mimeType,$priority=1.0){
			$this->addPreference("Accept-Charset",$mimeType,$priority);
		}

		protected function addPreferedEncoding($mimeType,$priority=1.0){
			$this->addPreference("Accept-Encoding",$mimeType,$priority);
		}

		protected function addPreferedLanguage($mimeType,$priority=1.0){
			$this->addPreference("Accept-Language",$mimeType,$priority);
		}

		protected function authorise($key,$secret){
			$this->insertToHeaders("Authorization","Basic ".base64_encode("$key:$secret"));
		}

		protected function assureCurrency($date,$eTag){
			$httpDate = gmdate("D, d M Y H:i:s",strtotime($date))." GMT";
			$this->insertToHeaders("If-Modified-Since",$httpDate);
			$this->insertToHeaders("If-None-Match",$eTag);
		}

		protected function assureConsistency($date=null,$eTag=null){
			if(!$date || !$eTag){
				$response = $this->get();
				$date = $response->viewFromHeaders("Last-Modified");
				$eTag = $response->viewFromHeaders("ETag");
			}

			$httpDate = gmdate("D, d M Y H:i:s",strtotime($date))." GMT";
			$this->insertToHeaders("If-Unmodified-Since",$httpDate);
			$this->insertToHeaders("If-Match",$eTag);
		}

		protected function suggestURI($suggestion){
			$this->insertToHeaders("Slug",$suggestion);
		}

		protected function getFullURI(){
			$protocol = ($this->getTransport()=="ssl")?"https":"http";

			$queryString = "";
			if($this->getArguments()){
				$queryString = array();
				foreach($this->getArguments() as $key=>$value){
					$queryString[] = "$key=".urlencode($value);
				}
				$queryString = "?".implode("&",$queryString);
			}

			return "$protocol://{$this->getHost()}{$this->getEndpoint()}$queryString";
		}

		protected function get(){
			$this->setMethod("GET");
			return $this->send();
		}

		protected function post(){
			$this->setMethod("POST");
			return $this->send();
		}

		protected function put(){
			$this->setMethod("PUT");
			return $this->send();
		}

		protected function delete(){
			$this->setMethod("DELETE");
			return $this->send();
		}

		protected function send(){
			Detective::investigate("{$this->getMethod()} {$this->getFullURI()}",false);

			//Build Request Headers
			$headers = array();
			foreach($this->getHeaders() as $header => $value){
				$headers[] = "$header: $value";
			}

			$ch = curl_init();
			curl_setopt($ch, CURLOPT_URL,$this->getFullURI());
			curl_setopt($ch, CURLOPT_HEADER, 1);
			curl_setopt($ch, CURLOPT_HTTPHEADER,$headers);
			curl_setopt($ch, CURLOPT_RETURNTRANSFER,TRUE);
			curl_setopt($ch, CURLOPT_FAILONERROR,false);
			curl_setopt($ch, CURLOPT_FORBID_REUSE, 1);

			if($this->getTransport() == "ssl"){
				curl_setopt($ch, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
				curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 1);
				curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 2);
			}

			if($this->getMethod() == "POST"){
				curl_setopt($ch, CURLOPT_POST,true);
				curl_setopt($ch, CURLOPT_POSTFIELDS,http_build_query($this->getPayload()));
			}else if($this->getMethod() == "PUT"){
				curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "PUT");
				curl_setopt($ch, CURLOPT_POSTFIELDS,http_build_query($this->getPayload()));
			}else if($this->getMethod() == "DELETE"){
				curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "DELETE");
			}

			$responseString = curl_exec($ch);

			$response = new WadapiResponse();
			preg_match("/^(.*)\r\n((?:.*\r\n)*)\r\n((?:.*\n?)*)/",$responseString,$matches);
			if($matches && preg_match("/^HTTP\/1\.[0-1] [0-9]{3} [A-Za-z\s]+$/",$matches[1])){
				$statusParts = preg_split("/\s/",$matches[1]);
				$response->setStatusCode(intval($statusParts[1]));
				$response->setReason(implode(" ",array_slice($statusParts,2,sizeof($statusParts))));

				foreach(preg_split("/\r\n/",trim($matches[2])) as $header){
					$headerParts = preg_split("/:\s/",$header);
					$response->insertToHeaders($headerParts[0],trim($headerParts[1]));
				}

				$response->setBody($matches[3]);
			}else{
				warning("An HTTP Response was invalid.","Invalid Response: $responseString");
			}

			//Close connection
			curl_close($ch);

			Detective::closeCase("{$this->getMethod()} {$this->getFullURI()}",false);
			return $response;
		}

		private function addPreference($header,$preference,$priority){
			$contentTypes = $this->viewFromHeaders($header);
			if(!$contentTypes){
				$contentTypes = array();
			}else{
				$contentTypes = explode(",",$contentTypes);
			}

			if(!$priority){
				$priority = 1.0;
			}

			$priority = min(max($priority,0.0),1.0);
			$contentTypes[] = sprintf("$preference;q=%.1f",$priority);
			$this->insertToHeaders($header,implode(",",$contentTypes));
		}
	}
?>
