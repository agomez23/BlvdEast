<?
	function ue_specific_select_neigh($reset = FALSE)
	{
		drupal_add_js(drupal_get_path('module', 'ue_specific') . '/script/neigh_select.js', 'module', 'footer');
		
		static $rtn;
		$cache = cache_get('appMultiNeigh');
		//print strlen($cache->data);
		//print_r($cache);
		
		
		
			if (!$reset && strlen($cache->data)) : $rtn = $cache->data;
			else :

				$tree = taxonomy_get_tree(1);
				//$tree2 = ue_specific_nh_listings();
				$rtn = '<div id="neigh_select"><div style="display:none;">';
				
				foreach ($tree as $term) {
					switch ($term->depth){
						case 0: 
							$rtn .= "\n".'<!-- close '.$grandparent_class.' --></div>'."\n\n".'<div class="expandable level0">';
							$rtn .= "\n ".'<a href="#" class="level0" id="'.$term->tid.'">'.$term->name.'</a> ';
							$grandparent_class = 'grandparent_'.$term->tid;
							break;
						case 1: 
							if (cntListings($term->tid,$term->depth)>=1) :
								//if ($currParent!=$term->tid && $currParent!=0) $rtn .= '</div>'; // close level1
								$rtn .= "\n	".'<div class="expandable level1 parent_'.$term->parents[0].' '.$grandparent_class.'">';
								$rtn .= ' <a class="expand_trigger" href="#" id="'.$term->tid.'">'.$term->name.' <span class="greycount">('.cntListings($term->tid,$term->depth).')</span></a> ';
								$rtn .= "\n	".' <div class="parent_'.$term->tid.'" style=" display:none;"><a href="#" class="select_level2 blue ">Select all</a></div>';
								//$rtn .= '<label><input type="checkbox" name="nh1" value="'.$term->tid.'"> '.$term->name.' <span class="greycount">('.cntListings($term->tid,$term->depth).')</span></label>';
								$rtn .= ' </div>'; 
								$currParent = $term->tid;
							endif;
							break;
						case 2: 
								$selectable = (cntListings($term->tid,$term->depth)>=1)?'' :'disabled';
								$rtn .= "\n		".'<div class="level2 parent_'.$term->parents[0].' '.$grandparent_class.' '.$selectable.'">';
								$rtn .= '<label><input title="'.trim($term->name).'" type="checkbox" name="nh2" value="'.$term->tid.'" '.$selectable.'> ';
								$rtn .= ''.$term->name.' <span class="greycount">('.cntListings($term->tid,$term->depth).')</span></label>';
								$rtn .= '</div>'; 
								
							break;
					}
				}
				$rtn .= '</div>'."\n".'</div>'."\n".'<input id="neigh_selected"/>'."\n".'<div id="neigh_displayed"></div>';
				
				//variable_set('appMultiNeigh',$rtn);
				cache_set('appMultiNeigh', $rtn);
				
			//else: $rtn = variable_get('appMultiNeigh','');
			endif;

		return $rtn;
	}
	
	function cntListings($tid,$depth){
		// get property id by neighborhood
		if ($depth==1) :
			$tids = array();
			$tree = taxonomy_get_tree(1,$tid,1);
			foreach ($tree as $term) $tids[] = $term->tid;
		else:
			$tids[] = $tid;
		endif;
		$strProperties = "SELECT nid FROM content_field_property_neighborhood WHERE field_property_neighborhood_value IN (".implode(',',$tids).")";
		$strCntListings = "SELECT nid FROM relativity WHERE parent_nid IN (".$strProperties.")";
		$cntActive = "SELECT count(n.nid) AS cntListing FROM node n INNER JOIN content_type_listing l ON n.nid=l.nid WHERE l.field_unit_availability_value = 'On the Market' AND n.status = '1' AND n.type = 'listing' AND n.nid IN (".$strCntListings.")";
		//print $cntActive;
		$cntListings = db_query($cntActive);
		while($obj = db_fetch_object($cntListings)) {
			return $obj->cntListing;
		}
	}
	
	function ue_specific_nh_listings($vid = 1, $parent = 0, $depth = -1, $max_depth = NULL)
	{
		static $children, $parents, $terms;
		
		$depth++;
		
		// We cache trees, so it's not CPU-intensive to call get_tree() on a term
		// and its children, too.
		if (!isset($children[$vid])) {
			$children[$vid] = array();
			
			$sql = '
			SELECT t.tid, t.*, parent, COUNT(l.nid) AS listings
			FROM {term_data} t
			INNER JOIN {term_hierarchy} h ON t.tid = h.tid
			LEFT OUTER JOIN {content_field_property_neighborhood} nh ON	t.tid = nh.field_property_neighborhood_value
			LEFT OUTER JOIN {relativity} r ON r.parent_nid = nh.nid
			LEFT OUTER JOIN {content_type_listing} l ON l.nid = r.nid AND l.field_unit_availability_value = \'On the Market\'
			LEFT OUTER JOIN {node} n ON n.nid = l.nid AND n.status = 1
			WHERE t.vid = %d
			GROUP BY t.tid
			ORDER BY weight, name';
			$result = db_query(db_rewrite_sql($sql, 't', 'tid'), $vid);
			while ($term = db_fetch_object($result)) {
				$children[$vid][$term->parent][] = $term->tid;
				$parents[$vid][$term->tid][] = $term->parent;
				$terms[$vid][$term->tid] = $term;
			}
		}
		
		$max_depth = (is_null($max_depth)) ? count($children[$vid]) : $max_depth;
		$tree = array();
		if ($max_depth > $depth && !empty($children[$vid][$parent])) {
			foreach ($children[$vid][$parent] as $child) {
				$term = drupal_clone($terms[$vid][$child]);
				$term->depth = $depth;
				// The "parent" attribute is not useful, as it would show one parent only.
				unset($term->parent);
				$term->parents = $parents[$vid][$child];
				$tree[] = $term;
				if (!empty($children[$vid][$child])) {
					$tree = array_merge($tree, ue_specific_nh_listings($vid, $child, $depth, $max_depth));
				}
			}
		}
		
		return $tree;
	}
?>
