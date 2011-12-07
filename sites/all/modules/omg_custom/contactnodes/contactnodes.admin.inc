<?php
	
	function contactnodes_admin()
	{
		global $base_url;
		$form = array();
		
		//get a list of defined node types and put into a list suitable for formapi.
		$nodetypes = node_get_types();
		foreach($nodetypes as $type => $info) $displaytypes[$type] = $info->name;
		
		$form['contactnodes_general'] = array(
			'#type' => 'fieldset',
			'#title' => t('General settings'),
			'#collapsible' => TRUE,
		);
		
		$form['contactnodes_general']['contactnodes_nodetype'] = array(
			'#type' => 'select',
			'#title' => t('Contact node type'),
			'#default_value' => variable_get('contactnodes_nodetype', ''),
			'#description' => t('This is the node type that is created when users submit the contact form.'),
			'#options' => $displaytypes,
			'#required' => TRUE
		);
		
		$form['contactnodes_general']['contactnodes_formurl'] = array(
			'#type' => 'textfield',
			'#title' => t('Contact node URL'),
			'#default_value' => variable_get('contactnodes_formurl', CONTACTNODES_DEFAULT_FORMURL),
			'#field_prefix' => "$base_url/",
			'#size' => 25,
			'#description' => t('This is the URL you would like to point users to for your contact form.'),
			'#required' => TRUE
		);
		
		$form['contactnodes_form'] = array(
			'#type' => 'fieldset',
			'#title' => t('Form settings'),
			'#collapsible' => TRUE,
		);
		
		$form['contactnodes_form']['contactnodes_title'] = array(
			'#type' => 'textfield',
			'#title' => t('Contact Form Title'),
			'#default_value' => variable_get('contactnodes_title', CONTACTNODES_DEFAULT_FORMTITLE),
			'#description' => t('Title of the contact form page.'),
		);
		
		$form['contactnodes_form']['contactnodes_header'] = array(
			'#type' => 'textarea',
			'#title' => t('Contact Form Header'),
			'#default_value' => variable_get('contactnodes_header', ''),
			'#description' => t('Place any HTML you would like to appear in the header of the form here.'),
		);
		
		$form['contactnodes_form']['contactnodes_footer'] = array(
			'#type' => 'textarea',
			'#title' => t('Contact Form Footer'),
			'#default_value' => variable_get('contactnodes_footer', ''),
			'#description' => t('Place any HTML you would like to appear in the footer of the form here.'),
		);
		
		$form['contactnodes_form']['contactnodes_confirm'] = array(
			'#type' => 'textfield',
			'#title' => t('Confirmation Page'),
			'#default_value' => variable_get('contactnodes_confirm', ''),
			'#field_prefix' => "$base_url/",
			'#size' => 25,
			'#description' => t('Path you would like to redirect to after form submission.'),
		);
		
		$form['contactnodes_form']['contactnodes_submitcaption'] = array(
			'#type' => 'textfield',
			'#title' => t('Submit Button Label'),
			'#default_value' => variable_get('contactnodes_submitcaption', CONTACTNODES_DEFAULT_SUBMITCAPTION),
			'#size' => 25,
			'#description' => t('Label for the submit button.'),
		);
		
		$form['contactnodes_menu'] = array(
			'#type' => 'fieldset',
			'#title' => t('Menu settings'),
			'#collapsible' => TRUE,
			'#collapsed' => TRUE,
		);
		
		$form['contactnodes_menu']['contactnodes_menutype'] = array(
			'#type' => 'select',
			'#title' => t('Enable menu link'),
			'#default_value' => variable_get('contactnodes_menutype', MENU_CALLBACK),
			'#description' => t('Enable this option to show a menu link.'),
			'#options' => array(MENU_NORMAL_ITEM => t('Yes'), MENU_CALLBACK => t('No')),
		);
		
		//Get list of menu names.
		$menunames = menu_get_names();
		foreach($menunames as $name) $displaymenus[$name] = $name;
		
		$form['contactnodes_menu']['contactnodes_menuname'] = array(
			'#type' => 'select',
			'#title' => t('Menu'),
			'#default_value' => variable_get('contactnodes_menuname', CONTACTNODES_DEFAULT_MENUNAME),
			'#description' => t('This is the menu you\'d like the form link to be added to.'),
			'#options' => $displaymenus,
		);
		
		return system_settings_form($form);
	}
	