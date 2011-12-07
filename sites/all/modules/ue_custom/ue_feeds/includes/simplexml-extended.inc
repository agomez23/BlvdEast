<?php
	class SimpleXMLExtended extends SimpleXMLElement
	{
		public function addCData($nodename,$cdata_text)
		{
			$node = $this->addChild($nodename); //Added a nodename to create inside the function
			$node = dom_import_simplexml($node);
			$no = $node->ownerDocument;
			$node->appendChild($no->createCDATASection($cdata_text));
		}
		public function addText($nodename,$text)
		{
			$node = $this->addChild($nodename); //Added a nodename to create inside the function
			$node = dom_import_simplexml($node);
			$no = $node->ownerDocument;
			$node->appendChild($no->createTextNode($text));
		}
	}
?>