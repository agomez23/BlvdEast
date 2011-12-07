<? 
function ue_specific_related_neighborhoods() {
	//print_r(arg());
	$rtn = '';
	$tid = arg(2);
	$tree = taxonomy_get_parents_all($tid);
	if (count($tree)) :
		$siblings = taxonomy_get_children($tree[1]->tid);
		//print_r($siblings);
		
		$rtn = '<ul>';
		foreach ($siblings as $s) {
			if ($s->tid!=arg(2) && $s->vid==1) $rtn .= '<li>'.l($s->name,'taxonomy/term/'.$s->tid).'</li>';
		}
		$rtn .= '<ul>';
	endif;	
	
	return $rtn;
}
?>