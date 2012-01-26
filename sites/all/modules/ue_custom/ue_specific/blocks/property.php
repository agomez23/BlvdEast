<?
	function ue_specific_property_progress()
	{
		global $globalForm, $user;
		$roletype = (array_key_exists(6,$user->roles) || array_key_exists(7,$user->roles) || $user->uid == 1) ? "ueuser":"other";
		//Find all field groups.
		if(is_array($globalForm))
		{
			$type = node_get_types('type', $globalForm['type']['#value']);
			
			if($globalForm['type']['#value'] == 'listing' && !empty($globalForm['nid']['#value']))
			{
				$parent = node_load(node_load($globalForm['nid']['#value'])->parent_node);
			}
			
			if(arg(1) == 'add')
				$rtn .= '<h1 class="title">Create ' . $type->name . '</h1>';
			else
				$rtn .= '<h1 class="title">Edit ' . ($parent ? $parent->title : $type->name) . ' : ' . $globalForm['title']['#default_value'] . '</h1>';
			
			$rtn .= '<div class="clear"></div>';
			$rtn .= '<p class="property_instructions">' . filter_xss_admin($type->help) . '</p>';
			
			foreach($globalForm as $key => $value)
			{ 
				//Filter out core field groups.
				if(is_array($value) && $value['#type'] == 'fieldset' && is_array($value['#attributes']) && $value['#attributes']['class'] != 'menu-item-form')
				{
					$groups[] = $value;
				}
			}
			//if ($roletype == 'ueuser') print_r($groups);
			
			
			if ($roletype == 'other') {//array_pop($groups);array_pop($groups); // remove "Sales" and "Agent"
				foreach ($groups as $k =>$group) {
					if (in_array($group["#title"],array('Sales','Agent'))) unset($groups[$k]);	
				}
			}
			
			if(is_array($groups) && count($groups) > 0)
			{
				$rtn .= '<ul class="form-tabs">';
				foreach($groups as $key => $value)
				{ 
					if($groups[0] == $value)
						$rtn .= '<li class="first active">';
					else if($groups[count($groups)-1] == $value)
						$rtn .= '<li class="last">';
					else
						$rtn .= '<li>';
					$rtn .= '<a rel="form" href="#' . $value['#attributes']['class'] . '">' . $value['#title'] . '</a>';
					$subgroups = '';
					foreach($value as $key2 => $value2)
					{
						if(is_array($value2) && $value2[0]['#type'] == 'anchorfield')
						{
							$subgroups .= '<li class="anchor_li"><a href="#' . $key2 . '" class="anchor_li_a">' . $value2['#title'] . '</a></li>';
						}
					}
					$rtn .= '</li>';
				}
				$rtn .= '</ul>';
			}
			
			
			$scrollJs = "$(function(){\n";
			
			if(!empty($user->roles[6]))
				$scrollJs .= '$("fieldset[class' . (!empty($user->roles[6]) ? '|=group]' : '') . '").addClass("uetab").not(":first").hide().css({left:$("#node-form").innerWidth()});';
			else
				$scrollJs .= '$("fieldset[class]").addClass("uetab").not(":first").hide();';
			
			$scrollJs .= "\n});";
			
			drupal_add_js(drupal_get_path('module', 'ue_specific') . '/script/form-progress.js', 'module', 'footer');
			drupal_add_js(drupal_get_path('module', 'ue_specific') . '/script/jquery.scrollTo-min.js', 'module', 'footer');
			
			if($globalForm['type']['#value'] == 'property' && arg(1) == 'add' && $user->uid == 1)
			{
				drupal_add_js(drupal_get_path('module', 'ue_specific') . '/script/property-duplicate.js', 'module', 'footer');
				drupal_add_js('$(function(){$("#edit-title").add("#edit-field-address-line-1-0-value").add("#edit-field-address-line-2-0-value").add("#edit-field-city-0-value").add("#edit-field-state-0-value").add("#edit-field-zipcode-0-value").addClass("dupeCheck").change(checkDupe);});', 'inline');
			}
			
			drupal_add_js($scrollJs, 'inline');
		}
		
		return $rtn;
	}
?>