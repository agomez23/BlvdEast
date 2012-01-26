<?php

	//drupal_add_js("//s7.addthis.com/js/250/addthis_widget.js#username=blvdeast");
	//drupal_add_js(drupal_get_path('theme', 'blvdeast') . '/js/curvycorners.js');
	drupal_add_css(drupal_get_path('theme', 'blvdeast') . '/css/addthis.css');

	function blvdeast_preprocess_page(&$variables)
	{
		//drupal_add_css(path_to_theme() . '/fonts/aller_rg.css', 'theme', 'all');
		$variables['styles'] = drupal_get_css();
		$host = $_SERVER['HTTP_HOST'];
		$domain = str_replace(array('www.', 'dev.', '.com'), '', $host);
		$variables['body_classes'] .= ' ' . $domain;
	}
	
	function blvdeast_login_block()
	{
		$module = 'user';
		$delta = 0;
		
		$block = module_invoke($module, 'block', 'view', $delta);

		// must be converted to an object
		$block = !empty($block) ? (object)$block : new stdclass;
		
		$block->module = $module;
		$block->delta = $delta;
		$block->region = 'login';
		$block->subject = '';
		
		return theme('block',$block);
	}
	
	/*
		Implementation of theme_apachesolr_facet_link($facet_text, $path, $options = array(), $count, $active = FALSE, $num_found = NULL)
		Wraps the result count for a facet with a span.facet-count.
	*/
	function blvdeast_apachesolr_facet_link($facet_text, $path, $options = array(), $count, $active = FALSE, $num_found = NULL)
	{
		$options['attributes']['class'][] = 'apachesolr-facet';
		if ($active) {
			$options['attributes']['class'][] = 'active';
		}
		$options['attributes']['class'] = implode(' ', $options['attributes']['class']);
		$options['html'] = true;
		return apachesolr_l(' ' . check_plain($facet_text) . " <span class=\"facet-count\">$count</span>",  $path, $options);
	}
	
	function blvdeast_item_list($items = array(), $title = NULL, $type = 'ul', $attributes = NULL) {
	  $output = '<div class="item-list">';
	  if (isset($title)) {
		$output .= '<h3>' . $title . '</h3>';
	  }
	
	  if (!empty($items)) {
		$output .= "<$type" . drupal_attributes($attributes) . '>';
		$num_items = count($items);
		foreach ($items as $i => $item) {
		  $attributes = array();
		  $children = array();
		  if (is_array($item)) {
			foreach ($item as $key => $value) {
			  if ($key == 'data') {
				$data = $value;
			  }
			  elseif ($key == 'children') {
				$children = $value;
			  }
			  else {
				$attributes[$key] = $value;
			  }
			}
		  }
		  else {
			$data = $item;
		  }
		  if (count($children) > 0) {
			$data .= theme_item_list($children, NULL, $type, $attributes); // Render nested list
		  }
		  if ($i == 0) {
			$attributes['class'] = empty($attributes['class']) ? 'first' : ($attributes['class'] . ' first');
		  }
		  if ($i == $num_items - 1) {
			$attributes['class'] = empty($attributes['class']) ? 'last' : ($attributes['class'] . ' last');
		  }
		  // The first item has $i=0, but we want its class to be 'odd' for
		  // consistency with other Drupal listings.
		  $zebra = ($i % 2) ? 'even' : 'odd';
		  $attributes['class'] = empty($attributes['class']) ? $zebra : ($attributes['class'] . " $zebra");
		  $output .= '<li' . drupal_attributes($attributes) . '>' . $data . "</li>\n";
		}
		$output .= "</$type>";
	  }
	  $output .= '</div>';
	  return $output;
	}
	
	/* implementation of theme_button. add extra html for nice themed css buttons */
	
	function blvdeast_button($element) {
		// Make sure not to overwrite classes.
		if (isset($element['#attributes']['class'])) {
			$element['#attributes']['class'] = 'form-' . $element['#button_type'] . ' ' . $element['#attributes']['class'];
		}
		else {
			$element['#attributes']['class'] = 'form-' . $element['#button_type'];
		}
		
		return '<span class="form-' . $element['#button_type'] . '-wrapper"><input type="submit" ' . (empty($element['#name']) ? '' : 'name="' . $element['#name'] . '" ') . 'id="' . $element['#id'] . '" value="' . check_plain($element['#value']) . '" ' . drupal_attributes($element['#attributes']) . " /></span>\n";
	}
	
	function blvdeast_tabset($element) {
		//print_r($element); exit;
	  $output = '<div id="tabs-'. $element['#tabset_name'] .'"'. drupal_attributes($element['#attributes']) .'>';
	  if($element['#description']) $output .= '<div class="description">'. $element['#description'] .'</div>';
	  $output .= '<ul class="clear-block">';
	  foreach (element_children($element) as $key) {
		if (isset($element[$key]['#type']) && $element[$key]['#type'] == 'tabpage') {
		  // Ensure the tab has content before rendering it.
		  if (
			(isset($element[$key]['#ajax_url']) && !empty($element[$key]['#ajax_url'])) ||
			(isset($element[$key]['#content']) && !empty($element[$key]['#content'])) ||
			(isset($element[$key]['#children']) && !empty($element[$key]['#children']))
		  ) {
			$output .= '<li'. drupal_attributes($element[$key]['#attributes']) .'><a href="' . $element[$key]['#url'] . '"><span class="tab">'. $element[$key]['#title'] .'</span></a></li>';
		  }
		}
	  }
	  $output .= '</ul>';
	  
		if($element['#tabset_name'] == 'listing-left')
		{
			//add fb like
			$output .= theme('addthis_toolbox', 'facebook_like');
		}
		
	  $output .= '<div class="tabpage-container"><div class="tabpage-container-inner">';
	  if (isset($element['#children'])) {
		$output .= $element['#children'];
	  }
	  $output .= '</div></div></div>';
	  return $output;
	}
	
	/**
	 * Return rendered content of a tab.
	 *
	 * @themable
	 */
	function blvdeast_tabpage($element) {
	  $output = '';
	  // Ensure the tab has content before rendering it.
	  if (!empty($element['#ajax_url']) || !empty($element['#content']) || !empty($element['#children'])) {
		$output .= '<div id="' . $element['#tab_name'] . '" class="tabpage tabs-' . $element['#tabset_name'] . '">';
		$output .= '<h2 class="drupal-tabs-title js-hide">'. $element['#title'] .'</h2>';
		$output .= $element['#content'] . (!empty($element['#children']) ? $element['#children'] : '');
		$output .='</div>';
	  }
	  return $output;
	}
	
	/* print sort links horizontally instead of in a list. */
	function blvdeast_apachesolr_sort_list($items) {
		$output .= 'Sort by ';
		$i = 0;
		foreach($items as $index => $link)
		{
			$output .= $link . ($i < count($items) - 1 ? ' - ' : '');
			$i++;
		}
		return $output;
	}
	
	function blvdeast_menu_item($link, $has_children, $menu = '', $in_active_trail = FALSE, $extra_class = NULL) {
		$class = ($menu ? 'expanded' : ($has_children ? 'collapsed' : 'leaf'));
	  if (!empty($extra_class)) {
		$class .= ' '. $extra_class;
	  }
	  if ($in_active_trail) {
		$class .= ' active-trail';
	  }
	  
	  if (!empty($link)) {
				// remove all HTML tags and make everything lowercase
				$css_id = strtolower(str_replace('&amp;', '', strip_tags($link)));
				// remove colons and anything past colons
				if (strpos($css_id, ':')) $css_id = substr ($css_id, 0, strpos($css_id, ':'));
				// Preserve alphanumerics, everything else goes away
				$pattern = '/[^a-z]+/ ';
				$css_id = preg_replace($pattern, '', $css_id);
				$class .= ' '. $css_id;
		}
		return '<li class="'. $class .'">'. $link . $menu ."</li>\n";
	}
	