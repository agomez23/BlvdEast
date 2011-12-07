<?
/*
	function ue_specific_parent_neigh()
	{
		if ($_GET['nh2']!="") :
		$empty = variable_get('empty',0);
		
		$viewPN = views_get_view('results');
		$viewPN->set_display('block_1');
		$viewPN->pager['items_per_page']=10;
		$viewPN->pager['use_pager']='mini';
		$viewPN->use_ajax=TRUE;
		
		//print_r($viewPN);
		//print_r($_GET['nh1']);
		$neighborhoods = taxonomy_get_tree(1, $_GET['nh1'], 1);
		foreach ($neighborhoods as $tid=>$term) $nh[] = $term->tid; // print_r($nh);
		$nhList = implode(',',$nh);
		$viewPN->set_arguments(array($nhList));
		$viewPN->execute();
		
		if ($empty && $viewPN->total_rows) return $viewPN->render();
		endif;
	}
*/
?>