<?php

	class ue_facebook_blocks_connect extends ue_custom_block
	{
		const BLOCK_NAME = 'Connect';
		
		public function view()
		{
			global $UE_FACEBOOK_FBSDK;
			
			$UE_FACEBOOK_FBSDK = true;
			
			if(variable_get('ue_facebook_likebox_css', '') != '')
				$css = 'css="' . variable_get('ue_facebook_likebox_css', '') . '"';
			
			return '
			<fb:like-box
				profile_id="' . UE_FACEBOOK_APPID . '"
				connections="' . $this->loadSetting('connections', '6') . '"
				stream="' . $this->loadSetting('stream', 'true') . '"
				header="' . $this->loadSetting('header', 'true') . '"
				width="' . $this->loadSetting('width', '225') . '"
				' . $css . '
			/>';
		}
		
		public function configure()
		{
			global $base_url;
			return array(
				'connections' => array(
					'#type' => 'textfield',
					'#title' => t('Connections'),
					'#size' => 10,
					'#default_value' => $this->loadSetting('connections', '6'),
				),
				'stream' => array(
					'#type' => 'checkbox',
					'#title' => t('Show stream'),
					'#default_value' => $this->loadSetting('stream', true),
				),
				'header' => array(
					'#type' => 'checkbox',
					'#title' => t('Show header'),
					'#default_value' => $this->loadSetting('header', true),
				),
				'width' => array(
					'#type' => 'textfield',
					'#title' => t('Width'),
					'#size' => 10,
					'#default_value' => $this->loadSetting('width', '225'),
				),
				'css' => array(
					'#type' => 'textfield',
					'#title' => t('CSS'),
					'#size' => 20,
					'#field_prefix' => "$base_url/",
					'#default_value' => $this->loadSetting('css', ''),
				),
			);
		}
		
		public function save($edit)
		{
			$this->saveSetting('connections', $edit['connections']);
			$this->saveSetting('stream', $edit['stream']);
			$this->saveSetting('header', $edit['header']);
			$this->saveSetting('width', $edit['width']);
			$this->saveSetting('css', $edit['css']);
		}
	
	}