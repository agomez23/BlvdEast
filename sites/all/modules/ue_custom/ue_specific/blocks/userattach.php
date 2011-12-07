<?
function ue_specific_userattach()
{
	$rtn = '';
	$opts='';
	$rtn.='<form onsubmit="return confirm(\'Merge these nodes with selected user?\')" method="get"><h2 class="title">User Attach</h2><div class="form-item"><div class="description">Attach all properties and listings associated with a company to a user.</div></div>';
		//$rtn.= ue_specific_userattach_form();
		
	$arrUsers = db_query("SELECT uid, name FROM users order by name");
	while($obj = db_fetch_object($arrUsers)) {
		$opts .= '<option value="'.$obj->uid.'">'.$obj->name.'</option>';
	}
	//$rtn.= '<div><label>User (uid): </label><input name="username"></div>';
	$rtn.= '<div><label>User (uid): </label><select style=" max-width:100%;" name="uid">'.$opts.'</select></div>';
	$opts='';
	$arrCompany = db_query("SELECT nid, title FROM node WHERE type = 'company' order BY title");
	while($obj = db_fetch_object($arrCompany)) {
		$opts .= '<option value="'.$obj->nid.'">'.$obj->title.'</option>';
	}
	//$rtn.= '<div><label>Company (nid): </label><input name="company"></div>';
	$rtn.= '<div><label>Company (nid): </label><select style=" max-width:100%;" name="nid"><option value=""></option>'.$opts.'</select></div>';
	
	$rtn.= '<div><label>&nbsp;</label><input type="submit"></div></form>';
	
	if ($_GET['uid']) :
		$vals = $_GET['uid'].'->'.$_GET['nid'];
		$uid = $_GET['uid']; $nid = $_GET['nid'];
		$arrProcNodes = array();$arrListingNodes = array();
		$nids = db_query('SELECT nid FROM content_type_property WHERE (field_contact_build_mgr_nid = '.$nid.' OR field_contact_build_owner_nid = '.$nid.' OR field_contact_lease_mgr_nid = '.$nid.')');
		while($obj = db_fetch_object($nids)) { array_push($arrProcNodes,$obj->nid); }
		$propertyNodes = implode(',',$arrProcNodes);
		$nids = db_query('SELECT nid FROM content_type_listing WHERE field_propertyid_nid IN ('.$propertyNodes.')');
		while($obj = db_fetch_object($nids)) { array_push($arrProcNodes,$obj->nid); }
		$listingNodes = implode(',',$arrProcNodes);
		$allNodes = $propertyNodes.','.$listingNodes;
		$qry = 'UPDATE node SET uid ='.$uid.' WHERE nid IN ('.$allNodes.')'; 
		//print $qry;
		$rtn .= (db_query($qry)) ? ' Updated':' Not Updated';
		
	endif;
	
	return '<div class="userattach">' . $rtn . '</div><div class="attachResults">'.$vals.'</div><div class="qry" style=" display:none">'.$qry.'</div>';
}
?>