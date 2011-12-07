<?php
	/* MANAGE LISTINGS 2.0 */
	function ue_custom_manage()
	{
		drupal_set_title('Manage Listings');
		drupal_set_breadcrumb(array(l(t("Home"), $base_url),l(t("My Account"),"user")));
		global $user;
		
		$superuser = array_key_exists(6, $user->roles) || array_key_exists(7, $user->roles) || $user->uid == 1;
		
		$paidSites = array('CZ', 'EN', 'HP', 'NC', 'OX', 'OO', 'RE', 'SE', 'TR', 'TA', 'VA', 'YA', 'ZI');
		foreach($paidSites as $i => $key) $paid[] = array('value' => $key);
		
		$premSites = array();
		foreach($premSites as $i => $key) $prem[] = array('value' => $key);
		
		if(strlen($_POST['op']))
		{
			foreach($_POST['nid'] as $nid)
			{
				if(intval($nid) > 0)
				{
					$node = node_load($nid);
					if(node_access('update', $node))
					{
						switch($_POST['op'])
						{
							case 'onmkt':
							case 'offmkt':
								$node->field_unit_availability = array(array('value' => ($_POST['op'] == 'onmkt' ? 'On the Market' : '')));
								//if ($_POST['op']=="offmkt") $node->promote = 0;
								content_update($node);
								node_save($node);
								break;
							case 'pub':
							case 'unpub':
								if($superuser)
								{
									$node->status = $_POST['op'] == 'pub' ? 1 : 0;
									node_save($node);
								}
								break;
							case 'syndication':
								if($superuser)
								{
									$node->field_syndication_free = array(array('value' => 1));
									$node->field_syndication_paid = $paid;
									$node->field_syndication_premium = $prem;
									content_update($node);
									node_save($node);
								}
								break;
							case 'syndicationoff':
								if($superuser)
								{
									$node->field_syndication_free = array(array('value' => 0));
									$node->field_syndication_paid = array(array('value' => ''));
									$node->field_syndication_premium = array(array('value' => ''));
									content_update($node);
									node_save($node);
								}
								break;
							case 'del':
								if($superuser)
									if(node_access('delete', $node)) node_delete($nid);
								break;
						}
					}
				}
			}
			header('Location: ' . $_POST['destination']);
			exit();
		}
		
		if(node_access('create', 'property')) $output .= '<div style="float:right;font-size:1.2em;font-weight:bold"><a class="addProperty" href="' . url('node/add/property') . '?destination=' . substr($_SERVER['REQUEST_URI'], 1) . '">Add New Building</a></div>';
		
		$output .= '<div style="margin:1em 0">Here is where you manage your listings.<br><br>';
		$output .= 'Before adding a listing, you\'ll need to <a href="' . url('node/add/property') . '?destination=' . substr($_SERVER['REQUEST_URI'], 1) . '">Add a Building</a>.  Each listing must &quot;belong&quot; to a building.</div>';
		
		$output .= '<div class="view-getMyProperties">';
		
		$superuser = array_key_exists(6, $user->roles) || array_key_exists(7, $user->roles);
		
		$companyID = is_array($_GET['companyID']) ? $_GET['companyID'] : array();
		$propID = is_array($_GET['propertyID']) ? $_GET['propertyID'] : array();
	
		// get list of properties for filter.
		$properties = ue_custom_get_properties('n.title', $companyID);
		
		// remove properties from filter params that were just filtered out by companies.
		if(count($propID))
			for($i = count($propID); $i--; $i >= 0)
				if(!array_key_exists($propID[$i], $properties)) array_splice($propID, $i, 1);
		
		// get list of properties for page.
		$filteredProperties = ue_custom_get_properties('n.title', $companyID, $propID, true);
		
		// render filters.
		$output .= ue_custom_manage_filters($superuser, $companyID, $propID, $properties);
		
		// render property list.
		$output .= ue_custom_manage_properties($filteredProperties, $superuser);
		
		$output .= '</div>';
		
		$script = '
		function quickEdit(nodeID)
		{
			$("#editrow" + nodeID + " form").slideDown().find("input").removeAttr("disabled");
			return false;
		}
		function saveEdit(nodeID)
		{
			dateRe = /^(0[1-9]|1[012])[- /.](0[1-9]|[12][0-9]|3[01])[- /.]\d\d$/;
			dateVal = $("input[name=field_date]", "#editrow" + nodeID).val();
			
			rentRe = /^[\d]{3,}$/;
			rentVal = $("input[name=field_price]", "#editrow" + nodeID).val();
			
			if(dateVal != "" && !dateRe.exec(dateVal))
			{
				alert("Please enter a valid date.");
				return false;
			}
			
			if(!rentRe.exec(rentVal))
			{
				alert("Please enter a valid rent.");
				return false;
			}
			
			$.ajax({
				   url: "manage-listings/quickedit/" + nodeID,
				   data: $("#editrow" + nodeID + " form").serialize(),
				   type: "post",
				   dataType: "json",
				   success: function(data) { updateRow(nodeID, data); $("#editrow" + nodeID + " form").slideUp("normal", function() {$(this).find(".loading").hide(); $("#editrow" + nodeID + " .buttons").show(); $(this).find("input").attr("disabled", "disabled"); }); } ,
				   error: function() { $("#editrow" + nodeID + " .loading").hide(); $("#editrow" + nodeID + " .buttons").show(); alert("There was an error saving the listing."); }
				   });
			
			$("#editrow" + nodeID + " .buttons").hide();
			$("#editrow" + nodeID + " .loading").show();
		}
		
		function updateRow(nodeID, data)
		{
			row = $("#listingrow" + nodeID);
			$(".price", row).html(data.field_price + (data.field_neteffective == "Net Effective" ? " Net Effective" : ""));
			$(".date", row).html(data.field_date);
			$(".offmkt", row).html(data.field_unit_availability == "On the Market" ? "" : "&nbsp;(Off the Market)");
			$(".updated", row).html(data.changed);
		}
		
		function cancelEdit(nodeID)
		{
			$("#editrow" + nodeID + " form").slideUp("normal", function() { $(this).find("input").attr("disabled", "disabled"); })
		}
		
		function bulkSubmit()
		{
			if($(this.op).val() == "del" && !confirm("Are you sure you want to delete these listings? This cannot be undone!")) return false;
			
			$(this).find("input.bulk").remove();
			$(".view-getMyProperties .bulk:checked").clone().attr("type", "hidden").appendTo(this);
		}
		
		function checkAll(nid)
		{
			$("#prop" + nid + " tr:visible .bulk:checkbox").attr("checked", "checked");
			return false;
		}
		
		function checkActive(nid)
		{
			$("#prop" + nid + " .active .bulk:checkbox").attr("checked", "checked");
			return false;
		}
		
		function checkNone(nid)
		{
			$("#prop" + nid + " .bulk:checkbox").removeAttr("checked");
			return false;
		}
		
		function showCollapsed(propnid)
		{
			$("#prop" + propnid + " .inactive").css("display", "");
			return false;
		}
		
		$(function() { $("#bulk").submit(bulkSubmit); });
		
		';
		
		drupal_add_js($script, 'inline');
		
		return $output;
	}
	
	function ue_custom_manage_filters($superuser, $companyID, $propID, $properties)
	{
		
		$output .= '<form action="' . $_SERVER['REQUEST_URI'] . '" method="GET">';
		
		if($superuser) $output .= ue_custom_manage_company_filter($companyID);
		
		// output property filter if more than 3 properties.
		if(count($properties) > 3) $output .= ue_custom_manage_property_filter($propID, $properties);
		
		if($superuser || count($properties) > 3) $output .= '<div class="clear"><input type="submit" value="Update filter"></div><br>';
		
		$output .= '</form>';
		
		return $output;
	}
	
	function ue_custom_manage_company_filter($companyID)
	{
		$companies = ue_custom_get_companies();
		$output .= '<div style="width:47%;float:left;padding-right:20px;">';
		$output .= '<h3>Filter Companies:</h3>';
		//$output .= '<a class="blue" href="#" onclick="$(\'option\', $(this).next(\'select\')).attr(\'selected\',\'selected\'); return false" style="font-size:.9em; border:0;">select all</a>';
		$output .= '<select name="companyID[]" size="6" multiple="multiple" style="width:100%;">';
		
		$output .= '<option value="all"' . ($companyID[0]=="all" ? ' selected>' : '>').'all properties</option>';
		$output .= '<option value="0"' . ($companyID[0]=="0" ? ' selected>' : '>') . 'unattached properties</option>';
		$output .= '<optgroup label="Select company...">';
		
		foreach ($companies as $company)
			$output .= '<option value="' . $company->nid . '"' . (in_array($company->nid, $companyID) ? ' selected>' : '>') . $company->title . '</option>';
		
		
		$output .= '</optgroup></select>';
		$output .= '</div>';
		return $output;
	}
	
	function ue_custom_manage_property_filter($propID, $properties)
	{
		$output .= '<div style="width:47%;float:left">';
		$output .= '<h3>Filter Properties:</h3>';
		$output .= '<select name="propertyID[]" size="6" multiple="multiple" style="width:100%;">';
		
		foreach ($properties as $property)
			$output .= '<option value="' . $property->nid . '"' . (in_array($property->nid, $propID) ? ' selected>' : '>') . $property->title . '</option>';
		
		$output .= '</select>';
		
		$output .= '</div>';
		return $output;
	}
	
	function ue_custom_manage_properties($filteredProperties, $superuser)
	{
		
		if(!count($filteredProperties))
			return '<div style="text-align:center">You haven\'t added any properties yet. <a href="' . url('node/add/property') . '?destination=' . substr($_SERVER['REQUEST_URI'], 1) . '">Add a property</a> to get started!</div>';
		
		
		// render pager.
		$output .= theme('pager', NULL, 30);
		
		foreach($filteredProperties as $obj)
		{
			global $theme_path;
			$collapsed = false;
			$collapsedRows = 0;
			$prop = node_load($obj->nid);
			$editProperty = node_access('update', $prop); // access to update
			
			$output .= '<div style=" position:relative;border-top:solid 3px #999;clear:both;padding:.5em 0 1em" id="prop' . $prop->nid . '">';
			
			$check = '<div><span style="font-size:.9em"><a href="#" onclick="return checkAll(' . $prop->nid . ');">Check All</a> | <a href="#" onclick="return checkActive(' . $prop->nid . ');">Check Active</a> | <a href="#" onclick="return checkNone(' . $prop->nid . ');">Check None</a></span><br /><a class="btip" href="#" onclick="return false;" title="Select the listings to update, then choose bulk operations at the bottom of the page.">'.t('Bulk Update').'*</a></div>';
			
			$header = array(array('colspan' => 2, data => $check), t('Size'), t('Rent'), t('Available'), t('Updated'));
			
			if($editProperty)
			{
				$output .= '<div class="property-edit" style="font-weight:bold"><a href="' . url('node/' . $prop->nid . '/edit') . '?destination=' . substr($_SERVER['REQUEST_URI'], 1) . '">Update Building Information</a></div>';
				$output .= '<div class="property-addListing" style="font-weight:bold"><a href="' . url('node/add/listing/parent/' . $prop->nid) . '?destination=' . substr($_SERVER['REQUEST_URI'], 1) . '">Add a New Listing</a></div>';
				// added 20100518 - ZE //
				$vwBFlags = views_get_view('building_flags'); $vwBFlags->set_arguments(array($prop->nid)); $vwBFlags->execute();
				$output .= '<br class="clear"/><div class="bFlags" style=" clear:both; float:right;">'.$vwBFlags->render().'</div>';
				// *** //
			}
			
			$output .= '<div>';
			$filepath = (strlen($prop->field_b_photos[0]['filepath'])) ? $prop->field_b_photos[0]['filepath'] : $theme_path . '/images/no-photo.jpg' ; 
			$output .= theme('imagecache', 'mini', $filepath, null, null, array('align'=>'left','style' => 'margin:0 5px 0 0;'));
			
			$output .= '<h3 class="property-title" style="border:none;margin:0;padding:0;">' . l($prop->title, 'node/' . $prop->nid) . '</h3>';
			
			$output .= '<span class="property-address">' . $prop->field_address_line_1[0]['value'] . '</span>';
			
			if($prop->status != 1) $output .= '&nbsp;(Inactive)';
			$output .= '</div>';
			$output .= '<div class="clear"></div>';
			
			$result = db_query('SELECT DISTINCT r.nid FROM relativity r INNER JOIN content_type_listing l ON l.nid = r.nid INNER JOIN node n ON n.nid = l.nid AND (n.status = 1 OR (n.status = 0 AND (n.uid = %d OR %d = 1))) WHERE r.parent_nid = %d ORDER BY l.field_unit_availability_value = \'On the Market\' OR n.status = 0 DESC, n.title', $user->uid, $superuser, $prop->nid);
			
			$rows = array();
			
			while($listing = node_load(db_result($result)))
			{
				
				$size = '';
				if($listing->field_bedroom[0]['value'])
				{
					$name = taxonomy_get_term($listing->field_bedroom[0]['value'])->name;
					
					if(strpos($name, '(') > 0) $name = substr($name, 0, strpos($name, '(') - 1);
					$size .= '<div style="font-size:.9em">' . $name . '</div>';
				}
				
				if($listing->field_bathroom[0]['value'] > 0)
				{
					$name = taxonomy_get_term($listing->field_bathroom[0]['value'])->name;
					if(strpos($name, '(') > 0) $name = substr($name, 0, strpos($name, '(') - 1);
					$size .= '<div style="font-size:.9em">' . $name . '</div>';
				}
				
				$ops = l($listing->title, 'node/' . $listing->nid, array('attributes' => array('class' => 'blue')));
				
				if($listing->status != 1) $ops .= ' (Inactive)';
				
				$ops .= ' <span class="offmkt">' . ($listing->field_unit_availability[0]['value'] == 'On the Market' ? '' : '(Off the Market)') . '</span>';
				
				$ops .= '<div style="font-size:.9em">';
				
				if(node_access('update', $listing))
				{
					$ops .= '<a href="' . url('node/' . $listing->nid . '/edit') . '?destination=' . substr($_SERVER['REQUEST_URI'], 1) . '">Edit</a>&nbsp;|&nbsp;';
					$ops .= '<a href="#" onclick="return quickEdit(' . $listing->nid . ');">Quick Edit</a>&nbsp;|&nbsp;';
				}
				if(node_access('delete', $listing))
					$ops .= '<a href="' . url('node/' . $listing->nid . '/delete') . '?destination=' . substr($_SERVER['REQUEST_URI'], 1) . '">Delete</a>&nbsp;|&nbsp;';
				
				if(node_access('update', $listing))
				{
					$ops .= '<a href="' . url('node/' . $listing->nid . '/clone') . '?destination=' . substr($_SERVER['REQUEST_URI'], 1) . '">Clone</a>&nbsp;|&nbsp;';
					$ops .= '<a href="' . url('node/add/openhouse/parent/' . $listing->nid) . '?destination=' . substr($_SERVER['REQUEST_URI'], 1) . '&keepThis=true&TB_iframe=true&height=480&width=640&modal=true#TB_inline" class="thickbox">Add Open House</a>';
				}
					
				$ops .= '</div>';
				
				$collapsed = $listing->field_unit_availability[0]['value'] != 'On the Market';
				
				$rows[] = array(
					'data' => array(
						node_access('update', $listing) ? '<div style="padding-left:10px"><input type="checkbox" value="' . $listing->nid . '" name="nid[]" class="bulk"></div>' : '',
						$ops,
						'<span class="size">' . $size . '</span>',
						'<span class="price">$' . number_format($listing->field_price[0]['value']) . ($listing->field_neteffective[0]['value'] == 'Net Effective' ? ' Net Effective' : '') . '</span>',
						'<span class="date">' . (strtotime($listing->field_date[0]['value']) > 0 ? format_date(strtotime($listing->field_date[0]['value']), 'custom', 'm/d/y') : 'Immediately') . '</span>',
						'<span class="updated">' . format_date($listing->changed, 'custom', 'm/d/y') . '</span>'
					),
					'style' => 'border-bottom:none;padding-top:3px' . ($collapsed ? ';display:none' : ''),
					'id' => 'listingrow' . $listing->nid,
					'class' => $collapsed ? 'inactive' : 'active'
				);
				
				$editrow = '<form style="display:none">';
				$editrow .= '<div style="font-size:.9em;float:left">';
				
				$editrow .= '<div style="width:80px;float:left">On the Market</div>';
				$editrow .= '<div style="width:100px;float:left">';
				$editrow .= '<input type="radio" value="On the Market" disabled name="field_unit_availability"' . ($listing->field_unit_availability[0]['value'] == 'On the Market' ? ' checked>' : '>') . ' Yes&nbsp;&nbsp;';
				$editrow .= '<input type="radio" value="" disabled name="field_unit_availability"' . ($listing->field_unit_availability[0]['value'] != 'On the Market' ? ' checked>' : '>') . ' No';
				$editrow .= '</div>';
				$editrow .= '<div style="width:70px;float:left">Monthly Rent</div>';
				$editrow .= '<div style="width:80px;float:left"><input type="text" name="field_price" style="width:65px" value="' . $listing->field_price[0]['value'] . '" disabled></div>';
				
				$editrow .= '<div style="width:300px;clear:both;height:5px"></div>';
				
				$editrow .= '<div style="width:80px;float:left">Date Available</div>';
				$editrow .= '<div style="width:100px;float:left"><input type="text" name="field_date" style="width:65px" value="' . (strtotime($listing->field_date[0]['value']) > 0 ? format_date(strtotime($listing->field_date[0]['value']), 'custom', 'm/d/y') : '') . '" disabled></div>';
				
				$editrow .= '<div style="width:70px;float:left">Net Effective</div>';
				$editrow .= '<div style="width:100px;float:left">';
				$editrow .= '<input type="radio" value="Net Effective" disabled name="field_neteffective"' . ($listing->field_neteffective[0]['value'] == 'Net Effective' ? ' checked>' : '>') . ' Yes&nbsp;&nbsp;';
				$editrow .= '<input type="radio" value="" disabled name="field_neteffective"' . ($listing->field_neteffective[0]['value'] != 'Net Effective' ? ' checked>' : '>') . ' No';
				$editrow .= '</div>';
				
				//$editrow .= '<div style="width:70px;float:left">Sq Ft.</div>';
				//$editrow .= '<div style="width:80px;float:left"><input type="text" name="field_sqft" style="width:65px" value="' . $listing->field_sqft[0]['value'] . '" disabled></div>';
				
				$editrow .= '</div>';
				
				$editrow .= '<div style="float:left;padding-top:9px" class="buttons"><input type="button" value="Save" onclick="saveEdit(' . $listing->nid . ');" disabled>&nbsp;&nbsp;<input type="button" value="Cancel" onclick="cancelEdit(' . $listing->nid . ');" disabled></div>';
				
				$editrow .= '<div style="float:left;padding-top:13px;display:none" class="loading"><img src="/sites/urbanedgeny.com/themes/ue/images/ajax-loader.eee.gif"></div>';
				
				$editrow .= '</form>';
				
				$rows[] = array('data' => array('', array('data' => $editrow, 'colspan' => 5)), 'id' => 'editrow' . $listing->nid, 'class' => 'editrow' . ($collapsed ? ' inactive' : ' active'), 'style' => $collapsed ? 'display:none' : '');
				
				if($collapsed) $collapsedRows++;
			}
			
			if(count($rows) == 0)
				$rows[] = array(array('data' => '<div style="text-align:center;margin:1em 0">' . t('No listings in this property.') . '</div>', 'colspan' => count($header) + 1));
			
			if($collapsedRows > 0)
				$rows[] = array(array('data' => '<div class="off_the_market" style="text-align:center;margin:1em 0"><a href="#" onclick="$(this).parents(\'tr\').hide(); return showCollapsed(' . $prop->nid . ');">' . $collapsedRows . t(' off-the-market listings.') . '</a></div>', 'class' => 'offmarketrow', 'colspan' => count($header) + 1));
			
			$output .= '<div class="view-getMyListings">' . theme('table', $header, $rows, array('width' => '100%', 'class' => 'clear')) . '</div>';
			
			$output .= '</div>';
		}
		
		// render bulk update.
		$output .= ue_custom_manage_bulk_update($superuser);
		
		// render pager.
		$output .= theme('pager', NULL, 30);
		
		return $output;
	}
	
	function timeago($date)
	{
		global $user;
		$interval = date('U') - $date;
		
		$min = 60;
		$hour = $min*60;
		$day = $hour*24;
		$week = $day*7;
		$month = $day*31;
		$year = $month*12;
		
		if($interval < $day)
		{
			if($user->uid != 1)
				$time_ago = t('today');
			else
				$time_ago = format_interval($interval, 1) . t(' ago');
		}
		else if($interval > $week && $interval < $week * 2)
			$time_ago = format_interval($interval, 2) . t(' ago');
		else
			$time_ago = format_interval($interval, 1) . t(' ago');
			
		return $time_ago;
	}

	function ue_custom_manage_bulk_update($superuser)
	{
		$output .= '<div style="padding:1em 0;">';
		$output .= '<form action="' . $_SERVER['REQUEST_URI'] . '" method="POST" id="bulk">';
		$output .= '<input type="hidden" name="destination" value="' . substr($_SERVER['REQUEST_URI'], 1) . '">';
		
		$output .= '<div>Bulk update: ';
		$output .= '<select name="op">';
		$output .= '<option value="onmkt">On the Market</option>';
		$output .= '<option value="offmkt">Off the Market</option>';
		if($superuser)
		{
			$output .= '<option value="del">Delete</option>';
			$output .= '<option value="pub">Publish</option>';
			$output .= '<option value="unpub">Unpublish</option>';
			$output .= '<option value="syndication">Enable All Syndication</option>';
			$output .= '<option value="syndicationoff">Disable All Syndication</option>';
		}
		$output .= '</select>';
		$output .= '<input type="submit" value="Update">';
		$output .= '</div>';
		
		$output .= '<div class="hidden" style="display:none"></div>';
		
		$output .= '</form>';
		$output .= '</div>';
		
		return $output;
	}
	
	// ajax callback for quick edit
	function ue_custom_manage_edit($nid)
	{
		global $user;
		$args = array('field_unit_availability' => array('filter' => FILTER_SANITIZE_SPECIAL_CHARS),
					  'field_neteffective' => array('filter' => FILTER_SANITIZE_SPECIAL_CHARS),
					  'field_date' => array('filter' => FILTER_VALIDATE_REGEX, 'options' => array('regexp' => '(0[1-9]|1[012])[- /.](0[1-9]|[12][0-9]|3[01])[- /.](19|20)[0-9]{2}')), 
					  'field_price' => array('filter' => FILTER_VALIDATE_INT),
					  );
		
		$input = filter_input_array(INPUT_POST, $args);
		
		if(strlen($input['field_date']))
			$input['field_date'] = date('Y-m-d H:i:s', strtotime($input['field_date']));
		
		$node = node_load(intval($nid));
		
		if(node_access('update', $node))
		{
			foreach($input as $field => $value)
				if($value !== false) $node->$field = array(array('value' => $value));
			
			content_update($node);
			node_save($node);
			
			$rtn = new StdClass();
			
			foreach($input as $field => $value)
			{
				$val = $node->$field;
				$rtn->$field = $val[0]['value'];
			}
			
			$rtn->field_date = strtotime($rtn->field_date) > 0 ? format_date(strtotime($rtn->field_date), 'custom', 'm/d/y') : 'Immediately';
			$rtn->field_price = '$' . number_format($rtn->field_price);
			$rtn->changed = format_date($node->changed, 'custom', 'm/d/y');
			
			print json_encode($rtn);
		}
		else
		{
			header("HTTP/1.0 403 Access Denied");
			die('Access denied for user ' . $user->uid . '.');
		}
	}

?>