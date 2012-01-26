<?php

	abstract class ue_custom_block
	{
		const BLOCK_NAME = 'Block';
		
		// these are the only methods that MUST be defined.
		abstract public function view();
		
		// rest of this is optional.
		
		protected $blockId;
		
		public function getId()
		{
			return $this->blockId;
		}
		
		public function ue_custom_block($blockId)
		{
			$this->blockId = $blockId;
		}
		
		public function configure()
		{
		}
		
		public function save($edit)
		{
		}
		
		protected function loadSetting($name, $default)
		{
			return variable_get(get_class($this) . '_' . $this->blockId . "_$name", $default);
		}
		
		protected function saveSetting($name, $value)
		{
			variable_set(get_class($this) . '_' . $this->blockId . "_$name", $value);
		}
		
	}