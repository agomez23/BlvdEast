<?
	function ue_specific_related_listings()
	{
		$node = node_load(arg(1));
		$propNode = node_load(array('nid'=>$node->field_propertyid[0][nid]));
		
		$size = array_keys(taxonomy_get_parents($node->field_bedroom[0]['value'], 'tid'));
		//print_r(taxonomy_get_parents($node->field_bedroom[0]['value']));
		if (count(taxonomy_get_parents($node->field_bedroom[0]['value']))==0) {
			$is_parent = true;
			$size = $node->field_bedroom[0]['value'];
		} else {
			$is_parent = false;
			$size = implode(',',array_keys(taxonomy_get_parents($node->field_bedroom[0]['value'], 'tid'))); 
		}
		//print $size;
														 
														 
		$bedrooms = implode(',',array_keys(taxonomy_get_children($size, 6, 'tid'))).','.$size; //echo $bedrooms;
		
		$neighborhood = $propNode->field_property_neighborhood[0]['value'];
		if (arg(1)=="term") $neighborhood = arg(2);
		$topNeighborhood = array_keys(taxonomy_get_parents($neighborhood, 'tid'));
		$subNeighborhoods = taxonomy_get_children($topNeighborhood[0], 1, 'tid');
		$neigh = implode(',',array_keys($subNeighborhoods)); //echo $neigh;
		
		$vwRelated = views_get_view('related_listings');
		$vwRelated->set_arguments(array($bedrooms,$neigh));
		$vwRelated->pager['items_per_page']=5;
		$vwRelated->pager['use_pager']=NULL;
		$vwRelated->use_ajax=TRUE;
		$vwRelated->execute();
		
		//print_r($vwRelated);
		
		if (count($vwRelated->result)>1) return ''.$vwRelated->render();
		
	}
?>