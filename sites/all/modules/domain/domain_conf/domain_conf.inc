<?php
	/* form hook to place robotstxt config box in domain settings. */
	function robotstxt_domainconf() {
	  $form['robotstxt'] = array(
		'#type' => 'fieldset',
		'#title' => t('Robots.txt contents'),
		'#collapsible' => TRUE,
		'#collapsed' => FALSE,
	  );
	  $form['robotstxt']['robotstxt'] = array(
		'#type' => 'textarea',
		'#title' => t('Contents of robots.txt'),
		'#default_value' => _robotstxt_get_content(),
		'#cols' => 60,
		'#rows' => 20,
		'#wysiwyg' => FALSE,
	  );
	  return $form;
	}