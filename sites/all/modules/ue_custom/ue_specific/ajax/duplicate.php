<?
	chdir('../../../../../');
	include_once './includes/bootstrap.inc';
	drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
	
	//find properties with names/address similar to one being entered.
	$input = filter_input_array(INPUT_POST, FILTER_SANITIZE_SPECIAL_CHARS);
	//match only on zipcode and title.
	
	$sql .= 'SELECT p.nid FROM content_type_property p ';
	//if(!empty($input['field_address_line_1'][0]['value']))
	//	$sql .= 'INNER JOIN content_field_address_line_1 a1 ON a1.nid = p.nid AND LOWER(field_address_line_1_value) LIKE \'%' . strtolower($input['field_address_line_1'][0]['value']) . '%\' ';
	//if(!empty($input['field_address_line_2'][0]['value']))
	//	$sql .= 'INNER JOIN content_field_address_line_2 a2 ON a2.nid = p.nid AND LOWER(field_address_line_2_value) LIKE \'%' . strtolower($input['field_address_line_2'][0]['value']) . '%\' ';
	//if(!empty($input['field_city'][0]['value']))
	//	$sql .= 'INNER JOIN content_field_city c ON c.nid = p.nid AND LOWER(field_city_value) LIKE \'%' . strtolower($input['field_city'][0]['value']) . '%\' ';
	//if(!empty($input['field_state'][0]['value']))
	//	$sql .= 'INNER JOIN content_field_state s ON s.nid = p.nid AND LOWER(field_state_value) LIKE \'%' . strtolower($input['field_state'][0]['value']) . '%\' ';
	if(!empty($input['field_zipcode'][0]['value']))
		$sql .= 'INNER JOIN content_field_zipcode z ON z.nid = p.nid AND field_zipcode_value LIKE \'%' . $input['field_zipcode'][0]['value'] . '%\' ';
	if(!empty($input['title']))
		$sql .= 'INNER JOIN node t ON t.nid = p.nid AND LOWER(title) LIKE \'%' . strtolower($input['title']) . '%\' AND t.status != 0 ';
	else
		$sql .= 'INNER JOIN node t ON t.nid = p.nid AND t.status != 0 ';
	
	$sql .= 'LIMIT 0,5';
	
	$result = db_query($sql);
	
	while($nid = db_result($result)) $dupes[] = node_load($nid);
?>
<? if(count($dupes)): ?>
	<div class="status warning duplicate">We've detected that this may be a duplicate of:
    <? foreach($dupes as $dupe) { ?>
    <br />&nbsp; &nbsp; &bull; &nbsp;<b>
	<?='<a class="blue" href="/'.$dupe->path.'">'.$dupe->title.'</a>'?></b>
    <? } ?>
    </div>
<? endif; ?>