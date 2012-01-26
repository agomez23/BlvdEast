<?php

/*
a
	these are all intermediary containers for translating nodes to feed data or vice versa.
	all of the fields exist in UE, but not all will exist in a feed.
	
	passing a node while instantiating any of these will fill all fields to prepare for a feed export.
*/

// base intermediary class will populate data that is present in most node types.
class ue_feeds_intermediary
{
	/*var $externalID; // external ID
	var $ueID; // node ID
	var $name; // name
	var $addr1;
	var $addr2;
	var $city;
	var $state;
	var $zip;
	var $phone;
	var $fax;
	var $email;
	var $website;
	var $facebook;
	var $twitter;
	var $desc;*/
	
	// standard text fields, etc. can be mapped. we only have to write extra code for weird fields.
	protected $mappings;
	
	// setup mappings then if node is set, run translation. classes that inherit this should setup their mappings before calling super:construct.
	function __construct($node = NULL)
	{
		$this->mappings['tagline'] = 'field_tagline';
		$this->mappings['desc'] = 'field_long_description';
		
		$this->mappings['addr1'] = 'field_address_line_1';
		$this->mappings['addr2'] = 'field_address_line_2';
		$this->mappings['city'] = 'field_city';
		$this->mappings['state'] = 'field_state'; //we need to convert abbreviations into full names - considering keys for this.
		$this->mappings['zip'] = 'field_zipcode';
		
		$this->mappings['phone'] = 'field_phone';
		$this->mappings['phone_ext'] = 'field_phone_ext';
		$this->mappings['phone2'] = 'field_phone2';
		$this->mappings['phone2_ext'] = 'field_phone2_ext';
		$this->mappings['fax'] = 'field_fax';
		
		//$this->mappings['email'] = 'field_email'; email is email instead of value
		//$this->mappings['website'] = 'field_website_link'; website is url instead of value.
		$this->mappings['facebook'] = 'field_facebook_link';
		$this->mappings['twitter'] = 'field_twitter';
		
		$this->mappings['op'] = 'field_op';
    $this->mappings['showing_instructions'] = 'field_showing_instructions';
		
		if($node) $this->fromNode($node);
	}
	
	public function fromNode($node)
	{
		$this->name = $node->title;
		$this->id = $node->nid;
		//lField is local key, dField is drupal fieldname.
		foreach($this->mappings as $lField => $dField)
		{
			if(isset($node->$dField))
			{
				$value = $node->$dField;
				if(isset($value[0]['value']) && strlen(trim($value[0]['value'])) > 0)
					$this->$lField = trim($value[0]['value']);
			}
		}
		
		if(isset($node->field_website_link) && strlen($node->field_website_link[0]['url']))
			$this->website = link_cleanup_url($node->field_website_link[0]['url']);
		
		if(isset($node->field_email) && strlen($node->field_email[0]['email']))
			$this->email = $node->field_email[0]['email'];
		
		if(in_array($node->type, array('property', 'listing'))) {
			$result = db_query('SELECT r.nid FROM {relativity} r INNER JOIN {node} n ON n.nid = r.nid AND n.type = \'openhouse\' AND n.status = 1 WHERE r.parent_nid = %d', $node->nid);
			while($nid = db_result($result)) {
				$oh = node_load($nid);
				if(count($oh->field_openhousedate)) {
					foreach($oh->field_openhousedate as $date) {
						if(strtotime($date['value']) > time())
							$this->openhouses[] = array('start' => $date['value'], 'end' => $date['value2']);
					}
				}
			}
    }
    //print_r($node);
    //$this->company_showing_instructions = $node->field_company_showing[0]['value'];
    //$this->company_op = $node->field_company_op[0]['value'];  
		$this->updated = $node->changed;
		$this->created = $node->created;
    $this->uid = $node->uid;
	}
	
	public function toNode($node = NULL)
	{
		if(!$node) $node = new StdClass();
		
		if(!$node->uid) $node->uid = 126; //default user 126
		if($node->status == null) $node->status = 0; //default unpublished.
		
		//do not overwrite title.
		if(strlen($node->title) == 0)
			$node->title = $this->name;
		
		//lField is local key, dField is drupal fieldname.
		foreach($this->mappings as $lField => $dField)
			if($this->$lField)
				$node->$dField = array(array('value' => $this->$lField));
		
		if($node->field_state[0]['value'] == 'DC')
			$node->field_state[0]['value'] = 'District of Columbia';
		
		if($this->website)
			$node->field_website_link[0]['url'] = $this->website;
		
		if($this->email)
			$node->field_email[0]['email'] = $this->email;
		
		return $node;
	}
	
	public function getFiles()
	{
		$files = array();
		
		if($this->logo)
			$files['field_logo'] = array($this->logo);
		
		return $files;
	}
	
	public function toNodeFileArray(array $urls)
	{
		foreach($urls as $i => $url)
			$files[] = $this->toFile($url);
		
		return $files;
	}
	
	static function toNodeArray(array $values, $existingNodeArray = NULL)
	{
		if(!$values) return;
		
		if(is_array($existingNodeArray))	
			foreach($existingNodeArray as $i => $value) if($value['value'] != '') $uniqueValues[] = $value['value'];
		
		if(is_array($uniqueValues))
			$uniqueValues = array_unique(array_merge($uniqueValues, $values));
		else
			$uniqueValues = array_unique($values);
		
		foreach($uniqueValues as $i => $value)
			$newValues[] = array('value' => $value);
		
		return $newValues;
	}
	
	static function fromNodeArray(array $nodeArray)
	{
		if(!is_array($nodeArray)) return;
		
		foreach($nodeArray as $i => $value) if($value['value'] != '') $values[] = $value['value'];
		if(isset($values)) return array_unique($values);
	}
}

class ue_feeds_company extends ue_feeds_intermediary
{
	function __construct($node = NULL)
	{
		$this->mappings['isBroker'] = 'field_is_broker';
    $this->mappings['company_op'] = 'field_company_op';
    $this->mappings['company_showing_instructions'] = 'field_company_showing';
		
		if(strlen($node->field_syndication_email[0]['email']))
			$this->syndication_email = $node->field_syndication_email[0]['email'];
		
		parent::__construct($node);
	}
	
	public function toNode($node = NULL)
	{
		$node = parent::toNode($node);
		
		return $node;
	}
}

class ue_feeds_property extends ue_feeds_intermediary
{
	protected $popListings;
	
	function __construct($node = NULL, $popListings = true)
	{
		$this->popListings = $popListings;
		
		$this->mappings['lat'] = 'field_latitude';
		$this->mappings['lng'] = 'field_longitude';
		
		$this->mappings['unitCount'] = 'field_unit_count';
		$this->mappings['yearBuilt'] = 'field_year_built';
		
		$this->mappings['contact_name'] = 'field_contact_name';
		
		$this->mappings['crossStreet'] = 'field_cross_street_1';
		$this->mappings['crossStreet2'] = 'field_cross_street_2';
		
		$this->mappings['hoodDesc'] = 'field_neighborhood_info';
    $this->mappings['incentivesText'] = 'field_other_incentives';

    $this->mapping['super'] = 'field_super';
    $this->mapping['super_phone'] = 'field_super_phone';
    $this->mapping['fee_desc'] = 'field_fee_descr';
    $this->mapping['refundable_fee'] = 'field_refundable_movein_fee_adv';
    $this->mapping['non_refundable_fee'] = 'field_nonrefund_movein_fee_adv';

		
		parent::__construct($node);
	}
	
	public function toNode($node = NULL)
	{
		$node = parent::toNode($node);
		$node->type = 'property';
		
		//Pets checkboxes.
		if($this->pets->dogs && $this->pets->cats)
			$node->field_pets_allowed = array(array('value' => 'Pets OK'));
		elseif($this->pets->dogs)
			$node->field_pets_allowed = array(array('value' => 'Dogs OK'));
		elseif($this->pets->cats)
			$node->field_pets_allowed = array(array('value' => 'Cats OK'));
		
		//Pets freeform.
		if(count($this->pets->comments)) $node->field_pet_policy_adv = array(array('value' => implode('<br>', array_unique($this->pets->comments))));
		
		//amenities
		if(count($this->amenities)) $node->field_basic_property_amenities = $this->toNodeArray($this->amenities, $node->field_basic_property_amenities);
		
		//more amenities
		if(count($this->moreAmen)) $node->field_more_building_amenities = $this->toNodeArray($this->moreAmen, $node->field_more_building_amenities);
		
		//services
		if(count($this->services)) $node->field_building_services = $this->toNodeArray($this->services, $node->field_building_services);
		
		//type
		if(count($this->type)) $node->field_building_size_adv = $this->toNodeArray($this->type, $node->field_building_size_adv);
		
		//access
		if(count($this->building_access)) $node->field_building_access = $this->toNodeArray($this->access, $node->field_building_access);
		
		$laundryname = 'field_laundry_facilities_&_storage';
		if(count($this->laundryStorage)) $node->$laundryname = $this->toNodeArray($this->laundryStorage, $node->$laundryname);
		
		//parking
		if(count($this->parking)) $node->field_parking_opts_adv = $this->toNodeArray($this->parking, $node->field_parking_opts_adv);
		
		// outdoorSpace
		if(count($this->outdoorSpace)) $node->field_outdoor_space = $this->toNodeArray($this->outdoorSpace, $node->field_outdoor_space);


    //added by cthorn
    if(count($this->bldg_amenities)) $node->field_bldg_amenities_adv = $this->toNodeArray($this->bldg_amenities, $node->field_bldg_amenities_adv);
    if(count($this->laundry)) $node->field_laundry_adv = $this->toNodeArray($this->laundry, $node->field_laundry_adv);
    if(count($this->concierge)) $node->field_laundry_adv = $this->toNodeArray($this->concierge, $node->field_concierge_adv);
    if(count($this->healthclub)) $node->field_healthclub_adv = $this->toNodeArray($this->healthclub, $node->field_healthclub_adv);
    if(count($this->sports)) $node->field_sports_adv = $this->toNodeArray($this->sports, $node->field_sports_adv);
    if(count($this->green)) $node->field_green_adv = $this->toNodeArray($this->green, $node->field_green_adv);
    if(count($this->parking_opts)) $node->field_parking_opts_adv = $this->toNodeArray($this->parking_opts, $node->field_parking_opts_adv);
    if(count($this->security_maint)) $node->field_security_maint_adv = $this->toNodeArray($this->security_maint, $node->field_security_maint_adv);
    if(count($this->bldg_doorman)) $node->field_bldg_doorman_adv = $this->toNodeArray($this->bldg_doorman, $node->field_bldg_doorman_adv);
    if(count($this->incentives)) $node->field_incentives_adv = $this->toNodeArray($this->incentives, $node->field_incentives_adv);
    if(count($this->nbrhd_details)) $node->field_nbrhd_details_adv = $this->toNodeArray($this->nbrhd_details, $node->field_nbrhd_details_adv);
    if(count($this->walking_distance)) $node->field_walking_distance_adv = $this->toNodeArray($this->walking_distance, $node->field_walking_distance_adv);
    if(count($this->nearby_trans)) $node->field_nearby_trans_adv = $this->toNodeArray($this->nearby_trans, $node->field_nearby_trans_adv);    
    if(count($this->trans_distance)) $node->field_trans_distance_adv = $this->toNodeArray($this->trans_distance, $node->field_trans_distance_adv);    
    if(count($this->income_required_tenant)) $node->field_income_required_tenant_adv = $this->toNodeArray($this->income_required_tenant, $node->field_income_required_tenant_adv);    
    if(count($this->rental_requirements)) $node->field_rental_requirements_adv = $this->toNodeArray($this->rental_requirements, $node->field_rental_requirements_adv);    
    if(count($this->income_required_guar)) $node->field_income_required_guar_adv = $this->toNodeArray($this->income_required_guar, $node->field_income_required_guar_adv);    
    if(count($this->cosigner)) $node->field_cosigner_adv = $this->toNodeArray($this->cosigner, $node->field_cosigner_adv);    
    if(count($this->deposit)) $node->field_deposit_adv = $this->toNodeArray($this->deposit, $node->field_deposit_adv);    
    if(count($this->refundable_movein_fee)) $node->field_refundable_movein_fee_adv = $this->toNodeArray($this->refundable_movein_fee, $node->field_refundable_movein_fee_adv);    
    if(count($this->nonrefund_movein_fee)) $node->field_nonrefund_movein_fee_adv = $this->toNodeArray($this->nonrefund_movein_fee, $node->field_nonrefund_movein_fee_adv);    
    if(count($this->movein_details)) $node->field_movein_details_adv = $this->toNodeArray($this->movein_details, $node->field_movein_details_adv);
    if(count($this->furnished)) $node->field_furnished = $this->toNodeArray($this->furnished, $node->field_furnished); 
    if(count($this->unit_type)) $node->field_unit_type = $this->toNodeArray($this->unit_type, $node->field_unit_type); 
    if(count($this->unit_outdoor)) $node->field_unit_outdoor = $this->toNodeArray($this->unit_outdoor, $node->field_unit_outdoor); 
    if(count($this->unit_views)) $node->field_unit_views = $this->toNodeArray($this->unit_views, $node->field_unit_views); 
    if(count($this->building_access)) $node->field_building_access = $this->toNodeArray($this->building_access, $node->field_building_access);
    if(count($this->building_type)) $node->field_building_type = $this->toNodeArray($this->building_type, $node->field_building_type);
    if(count($this->other_incentives)) $node->field_other_incentives = $this->toNodeArray($this->other_incentives, $node->field_other_incentives);
    
		
		if(!$node->field_property_neighborhood && $this->hood) $node->field_property_neighborhood = array(array('value' => $this->hood));
		
		$node->field_authorization = array(array('value' => 'I hereby grant Urban Edge Realty, LLC permission to advertise my property(ies) on the website www.UrbanEdgeNY.com.'));
		
		return $node;
	}
	
	public function fromNode($node)
	{
		parent::fromNode($node);
		if($node->field_b_photos)
			foreach($node->field_b_photos as $i => $photo)
				if(strlen($photo['filename']))
					$this->photos[] = 'http://www.urbanedgeny.com/' . str_replace('sites/urbanedgeny.com/files/', 'feeds/images/', $photo['filepath']);
		
		foreach($node->field_building_size_adv as $i => $val)
		{
			$value = $val['value'];
			if(stripos($value, 'low-rise') >= 0)
				$this->size = 'lowrise';
			else if(stripos($value, 'mid-rise') >= 0)
				$this->size = 'midrise';
			else if(stripos($value, 'high-rise') >= 0)
				$this->size = 'highrise';
			else if(stripos($value, 'townhouse') >= 0)
				$this->size = 'townhouse';
			
		}
		
		foreach($node->field_pets_allowed as $i => $pets)
		{
			if($pets['value'] == 'Pets OK')
			{
				$this->pets->dogs = true;
				$this->pets->cats = true;
			}
			elseif($pets['value'] == 'Dogs OK')
				$this->pets->dogs = true;
			elseif($pets['value'] == 'Cats OK')
				$this->pets->cats = true;
		}
		
		//Pets freeform.
		if(strlen($node->field_pet_policy_adv[0]['value']))
			$this->pets->comments = $node->field_pet_policy_adv[0]['value'];
		
		//incentives
		if($node->field_incentives_adv) $this->incentives = $this->fromNodeArray($node->field_incentives_adv);
		
		//amenities
		if($node->field_basic_property_amenities) $this->amenities = $this->fromNodeArray($node->field_basic_property_amenities);
		
		//services
		if($node->field_building_services) $this->services = $this->fromNodeArray($node->field_building_services);
		
		//access
		if($node->field_building_access) $this->access = $this->fromNodeArray($node->field_building_access);
		
		//parking
		if($node->field_parking_opts_adv) $this->parking = $this->fromNodeArray($node->field_parking_opts_adv);
		
		//building type
		if($node->field_building_size_adv) $this->type = $this->fromNodeArray($node->field_building_size_adv);
		
		//security deposit
		if($node->field_deposit_adv) $this->deposit = $this->fromNodeArray($node->field_deposit_adv);
    
    //added by cthorn
    if($node->field_bldg_amenities_adv) $this->bldg_amenities = $this->fromNodeArray($node->field_bldg_amenities_adv);
    if($node->field_laundry_adv) $this->laundry = $this->fromNodeArray($node->field_laundry_adv);
    if($node->field_concierge_adv) $this->concierge = $this->fromNodeArray($node->field_concierge_adv);
    if($node->field_healthclub_adv) $this->healthclub = $this->fromNodeArray($node->field_healthclub_adv);
    if($node->field_sports_adv) $this->sports = $this->fromNodeArray($node->field_sports_adv);
    if($node->field_green_adv) $this->green = $this->fromNodeArray($node->field_green_adv);
    if($node->field_outdoor_adv) $this->outdoor = $this->fromNodeArray($node->field_outdoor_adv);
    if($node->field_parking_opts_adv) $this->parking_opts = $this->fromNodeArray($node->field_parking_opts_adv);
    if($node->field_security_maint_adv) $this->security_maint = $this->fromNodeArray($node->field_security_maint_adv);
    if($node->field_bldg_doorman_adv) $this->bldg_doorman = $this->fromNodeArray($node->field_bldg_doorman_adv);
    if($node->field_incentives_adv) $this->incentives = $this->fromNodeArray($node->field_incentives_adv);
    if($node->field_nbrhd_details_adv) $this->nbrhd_details = $this->fromNodeArray($node->field_nbrhd_details_adv);
    if($node->field_walking_distance_adv) $this->walking_distance = $this->fromNodeArray($node->field_walking_distance_adv);
    if($node->field_nearby_trans_adv) $this->nearby_trans = $this->fromNodeArray($node->field_nearby_trans_adv);
    if($node->field_trans_distance_adv) $this->trans_distance = $this->fromNodeArray($node->field_trans_distance_adv);
    if($node->field_income_required_tenant_adv) $this->income_required_tenant = $this->fromNodeArray($node->field_income_required_tenant_adv);
    if($node->field_rental_requirements_adv) $this->rental_requirements = $this->fromNodeArray($node->field_rental_requirements_adv);
    if($node->field_income_required_guar_adv) $this->income_required_guar = $this->fromNodeArray($node->field_income_required_guar_adv);
    if($node->field_cosigner_adv) $this->cosigner = $this->fromNodeArray($node->field_cosigner_adv);
    if($node->field_deposit_adv) $this->deposit = $this->fromNodeArray($node->field_deposit_adv);
    if($node->field_refundable_movein_fee_adv) $this->refundable_movein_fee = $this->fromNodeArray($node->field_refundable_movein_fee_adv);
    if($node->field_nonrefund_movein_fee_adv) $this->nonrefund_movein_fee = $this->fromNodeArray($node->field_nonrefund_movein_fee_adv);
    if($node->field_movein_details_adv) $this->movein_details = $this->fromNodeArray($node->field_movein_details_adv);

    if($node->field_furnished) $this->furnished = $this->fromNodeArray($node->field_furnished);
    if($node->field_unit_outdoor) $this->unit_outdoor = $this->fromNodeArray($node->field_unit_outdoor);
    if($node->field_unit_views) $this->unit_views = $this->fromNodeArray($node->field_unit_views);
    if($node->field_building_access) $this->building_access = $this->fromNodeArray($node->field_building_access);
    if($node->field_building_type) $this->building_type = $this->fromNodeArray($node->field_building_type);
    if($node->field_other_incentivies_adv) $this->other_incentives = $this->fromNodeArray($node->field_incentives_adv);

		
		if($node->nid && $this->popListings)
		{
			$result = db_query('SELECT DISTINCT r.nid FROM relativity r INNER JOIN node n ON n.nid = r.nid AND n.status = 1 INNER JOIN content_type_listing l ON l.nid = r.nid AND LOWER(l.field_unit_availability_value) = \'on the market\' WHERE r.parent_nid = %d', $node->nid);
			while($nid = db_result($result))
			{
				$listing = node_load($nid, NULL, true);
				$listings[] = new ue_feeds_listing($listing);
			}
			if(count($listings))
				$this->listings = $listings;
		}
		
		//bldg manager
    if($node->field_contact_build_mgr[0]['nid'] > 0) {
      $obj1 = new ue_feeds_company(node_load($node->field_contact_build_mgr[0]['nid']));
      $obj2 = (object) array('company_type' => 'building_mgr');
      $this->building_mgr = (object) array_merge((array)$obj1, (array) $obj2);
      //$this->building_mgr = new ue_feeds_company(node_load($node->field_contact_build_mgr[0]['nid']));
      //$this->building_mgr->company_type = 'building_mgr';
    }
		
		//bldg owner
		if($node->field_contact_build_owner[0]['nid'] > 0) {
      $obj1 = new ue_feeds_company(node_load($node->field_contact_build_owner[0]['nid']));
      $obj2 = (object) array('company_type' => 'building_own');
      $this->building_own = (object) array_merge((array)$obj1, (array) $obj2);
      //$this->building_own = new ue_feeds_company(node_load($node->field_contact_build_owner[0]['nid']));
      //$this->building_own->company_type = 'building_own';
    }
		
		//bldg owner
		if($node->field_contact_lease_mgr[0]['nid'] > 0) {
      $obj1 = new ue_feeds_company(node_load($node->field_contact_lease_mgr[0]['nid']));
      $obj2 = (object) array('company_type' => 'lease_mgr');
      $this->leasing_mgr = (object) array_merge((array)$obj1, (array) $obj2);
      //$this->leasing_mgr = new ue_feeds_company(node_load($node->field_contact_lease_mgr[0]['nid']));
      //$this->leasing_mgr->company_type = 'leasing_mgr';
    }
		
		$hoodinfo = taxonomy_get_term($node->field_property_neighborhood[0]['value']);
		$this->hoodName = $hoodinfo->name;
    $this->hoodId = $hoodinfo->tid;
    $this->super = $node->field_super[0]['value'];
    $this->super_phone = $node->field_super_phone[0]['value'];
	}
	
	public function getFiles()
	{
		$files = parent::getFiles();
		
		if($this->photos)
			$files['field_b_photos'] = $this->photos;
		
		return $files;
	}
}

class ue_feeds_listing extends ue_feeds_intermediary
{
	function __construct($node = NULL)
	{
		$this->mappings['sales'] = 'field_sales';
		
		$this->mappings['lease_term'] = 'field_lease_term';
		$this->mappings['lease_type'] = 'field_lease_type';
		$this->mappings['furnished'] = 'field_furnished';
    $this->mappings['starting_price'] = 'field_starting_price';

    //adding additional fields to xml
    $this->mappings['more_features'] = 'field_u_features_adv';
    //$this->mappings['apt_type_style'] = 'field_u_type_adv';
    //$this->mappings['floor_ceilings'] = 'field_u_floorceilings_adv';
    //$this->mappings['windows'] = 'field_u_windows_adv';
    $this->mappings['closets_storage_laundry'] = 'field_u_windows_adv';
    $this->mappings['num_closets'] = 'field_closets';
    $this->mappings['walkin_closets'] = 'field_walkin_closets';
    $this->mappings['incomerange_min'] = 'field_u_incomerange_min_adv';
    $this->mappings['incomerange_max'] = 'field_u_incomerange_max_adv';

		
		parent::__construct($node);
	}
	
	public function toNode($node = NULL)
	{
		$node = parent::toNode($node);
		$node->type = 'listing';
		$node->status = 1;
		
		// bedroom to taxonomy.
		if($this->bed == 0) $bed = 12;
		if($this->bed == 1) $bed = 14;
		if($this->bed == 2) $bed = 15;
		if($this->bed == 3) $bed = 16;
		if($this->bed == 4) $bed = 17;
		if($this->bed >= 5) $bed = 820;
		if($bed) $node->field_bedroom = array(array('value' => $bed));
		
		// bathroom to taxonomy.
		if($this->bath == 0.5) $bath = 19;
		if($this->bath == 1) $bath = 20;
		if($this->bath == 1.5) $bath = 21;
		if($this->bath == 2) $bath = 22;
		if($this->bath == 2.5) $bath = 23;
		if($this->bath == 3) $bath = 829;
		if($this->bath == 3.5) $bath = 832;
		if($this->bath >= 4) $bath = 831;
		if($bath) $node->field_bathroom = array(array('value' => $bath));
		
		//sqft
		if(is_array($this->sqft))
		{
			$node->field_min_sqft = array(array('value' => $this->sqft['min']));
			$node->field_max_sqft = array(array('value' => $this->sqft['max']));
		}
		elseif(is_numeric($this->sqft))
			$node->field_sqft = array(array('value' => $this->sqft));
		
		//price
		if(is_array($this->price))
			$node->field_price = array(array('value' => $this->price['min']));
		elseif(is_numeric($this->price))
			$node->field_price = array(array('value' => $this->price));
		
		if($this->dateAvail)
			$node->field_date = array(array('value' => date('Y-m-d H:i:s', $this->dateAvail)));
		
		$node->field_unit_availability = array(array('value' => 'On the Market'));
		
		//utilities
		if(count($this->utilities)) $node->field_u_utilities_adv = $this->toNodeArray($this->utilities, $node->field_u_utilities_adv);
		
		//hvac
		if(count($this->hvac)) $node->field_u_hvac_adv = $this->toNodeArray($this->hvac, $node->field_u_hvac_adv);
		
		//condition
		if(count($this->condition)) $node->field_u_condition_adv = $this->toNodeArray($this->condition, $node->field_u_condition_adv);
		
		//features
    if(count($this->features)) $node->field_unit_features = $this->toNodeArray($this->features, $node->field_unit_features);

    //more features
    if(count($this->features_adv)) $node->field_u_features_adv = $this->toNodeArray($this->features_adv, $node->field_u_features_adv);

    //floorsceiling
    if(count($this->floorceiling)) $node->field_u_floorsceilings_adv = $this->toNodeArray($this->floorceiling, $node->field_u_floorsceilings_adv);

    if(count($this->apt_type)) $node->field_u_type_adv = $this->toNodeArray($this->apt_type, $node->field_u_type_adv);

    //added by cthorn
    if(count($this->windows)) $node->field_u_windows_adv = $this->toNodeArray($this->windows, $node->field_u_windows_adv);
    if(count($this->closets)) $node->field_u_closets_adv = $this->toNodeArray($this->closets, $node->field_u_closets_adv);
    if(count($this->archit_det)) $node->field_u_archit_det_adv = $this->toNodeArray($this->archit_det, $node->field_u_archit_det_adv);
    if(count($this->addl_rooms)) $node->field_u_addl_rooms_adv = $this->toNodeArray($this->addl_rooms, $node->field_u_addl_rooms_adv);
    if(count($this->lease_type)) $node->field_u_lease_type_adv = $this->toNodeArray($this->lease_type, $node->field_u_lease_type_adv);
    if(count($this->accessible)) $node->field_u_accessible_adv = $this->toNodeArray($this->accessible, $node->field_u_accessible_adv);
    if(count($this->kitchen_type)) $node->field_u_kitchen_type_adv = $this->toNodeArray($this->kitchen_type, $node->field_u_kitchen_type_adv);
    if(count($this->kitchen_condition)) $node->field_kitchen_condition = $this->toNodeArray($this->kitchen_condition, $node->field_kitchen_condition); 
    if(count($this->kitchen_amenities)) $node->field_kitchen_amenities = $this->toNodeArray($this->kitchen_amenities, $node->field_kitchen_amenities); 
    if(count($this->kitchen_appliances)) $node->field_kitchen_appliances = $this->toNodeArray($this->kitchen_appliances, $node->field_kitchen_appliances); 
    if(count($this->bathroom_condition)) $node->field_bathroom_condition = $this->toNodeArray($this->bathroom_condition, $node->field_bathroom_condition); 
    if(count($this->bathroom_features)) $node->field_bathroom_features = $this->toNodeArray($this->bathroom_features, $node->field_bathroom_features); 
    if(count($this->kitchen_features)) $node->field_u_kitchen_features_adv = $this->toNodeArray($this->kitchen_features, $node->field_u_kitchen_features_adv);
    if(count($this->cabinet)) $node->field_u_cabinet_adv = $this->toNodeArray($this->cabinet, $node->field_u_cabinet_adv);
    if(count($this->countertops)) $node->field_u_countertops_adv = $this->toNodeArray($this->countertops, $node->field_u_countertops_adv);
    if(count($this->backsplash)) $node->field_u_backsplash_adv = $this->toNodeArray($this->backsplash, $node->field_u_backsplash_adv);
    if(count($this->refridge)) $node->field_u_refridge_adv = $this->toNodeArray($this->refridge, $node->field_u_refridge_adv);
    if(count($this->oven)) $node->field_u_oven_adv = $this->toNodeArray($this->oven, $node->field_u_oven_adv);
    if(count($this->sink)) $node->field_u_sink_adv = $this->toNodeArray($this->sink, $node->field_u_sink_adv);
    if(count($this->kitchen_floors)) $node->field_u_kitchen_floors_adv = $this->toNodeArray($this->kitchen_floors, $node->field_u_kitchen_floors_adv);
    if(count($this->spec_kitchen)) $node->field_u_spec_kitchen_adv = $this->toNodeArray($this->spec_kitchen, $node->field_u_spec_kitchen_adv);
    if(count($this->bath_features)) $node->field_u_bath_features_adv = $this->toNodeArray($this->bath_features, $node->field_u_bath_features_adv);
    if(count($this->bath_floor)) $node->field_u_bath_features_adv = $this->toNodeArray($this->bath_floor, $node->field_u_bath_features_adv);
    if(count($this->bath_sink)) $node->field_u_bath_sink_adv = $this->toNodeArray($this->bath_sink, $node->field_u_bath_sink_adv);
    if(count($this->luxe_features)) $node->field_u_luxe_features_adv = $this->toNodeArray($this->luxe_features, $node->field_u_luxe_features_adv);    
    if(count($this->outdoor)) $node->field_u_outdoor_adv = $this->toNodeArray($this->outdoor, $node->field_u_outdoor_adv);    
    if(count($this->views_adv)) $node->field_u_views_adv = $this->toNodeArray($this->views_adv, $node->field_u_views_adv);    

		return $node;
	}
	
	public function fromNode($node)
	{
		parent::fromNode($node);
		
		if($node->field_u_photos)
			foreach($node->field_u_photos as $i => $photo)
				if(strlen($photo['filename']))
					$this->photos[] = 'http://www.urbanedgeny.com/' . str_replace('sites/urbanedgeny.com/files/', 'feeds/images/', $photo['filepath']);
		
		if($node->field_u_floorplans)
			foreach($node->field_u_floorplans as $i => $floorplan)
				if(strlen($floorplan['filename']))
					$this->floorplans[] = 'http://www.urbanedgeny.com/' . str_replace('sites/urbanedgeny.com/files/', 'feeds/images/', $floorplan['filepath']);
		
		if(strlen($node->field_bedroom[0]['value']))
		{
			$bed = $node->field_bedroom[0]['value'];
      if(in_array($bed, array(12, 13, 814, 815))) {
        $this->bed = 0;
        $bed_term = taxonomy_get_term($bed);
        $this->bed_desc = $bed_term->name;
      }
      if(in_array($bed, array(14, 823, 822))) {
        $this->bed = 1;
        $bed_term = taxonomy_get_term($bed);
        $this->bed_desc = $bed_term->name;
      }
      if(in_array($bed, array(15, 824, 833))) {
        $this->bed = 2;
        $bed_term = taxonomy_get_term($bed);
        $this->bed_desc = $bed_term->name;
      }
      if(in_array($bed, array(16, 825, 834))) {
        $this->bed = 3;
        $bed_term = taxonomy_get_term($bed);
        $this->bed_desc = $bed_term->name;
      }
      if(in_array($bed, array(17, 826, 835))) {
        $this->bed = 4;
        $bed_term = taxonomy_get_term($bed);
        $this->bed_desc = $bed_term->name;
      }
      if($bed == 820) {
        $this->bed = 5;
        $bed_term = taxonomy_get_term($bed);
        $this->bed_desc = $bed_term->name;
      }
		}
		
		if(strlen($node->field_bathroom[0]['value']))
		{
			$bath = $node->field_bathroom[0]['value'];
			if($bath == 19) $this->bath = 0.5;
			if($bath == 20) $this->bath = 1;
			if($bath == 21) $this->bath = 1.5;
			if($bath == 22) $this->bath = 2;
			if($bath == 23) $this->bath = 2.5;
			if($bath == 829) $this->bath = 3;
			if($bath == 832) $this->bath = 3.5;
			if($bath == 831) $this->bath = 4;
			
			//calculate full/half baths.
			$this->fullBath = floor($this->bath);
			if($this->bath - floor($this->bath) > 0) $this->halfBath = 1;
			
			if($bath == 827)
			{
				$this->bath = 1;
				$this->fullBath = 1;
				$this->halfBath = 2;
			}
			if($bath == 828)
			{
				$this->bath = 2;
				$this->fullBath = 2;
				$this->halfBath = 2;
			}
			if($bath == 830)
			{
				$this->bath = 3;
				$this->fullBath = 3;
				$this->halfBath = 2;
			}
		}
		
		//sqft
		if(floatval($node->field_min_sqft[0]['value']) > 0 || floatval($node->field_max_sqft[0]['value']))
		{
			$this->sqft['min'] = floatval($node->field_min_sqft[0]['value']);
			$this->sqft['max'] = floatval($node->field_max_sqft[0]['value']);
		}
		elseif(floatval($node->field_sqft[0]['value']) > 0)
			$this->sqft = floatval($node->field_sqft[0]['value']);
			
		//price
		if(floatval($node->field_price[0]['value']) > 0)
			$this->price = floatval($node->field_price[0]['value']);
		
		if(strtotime($node->field_date[0]['value']) > time())
			$this->dateAvail = strtotime($node->field_date[0]['value']);
		
		if($node->field_unit_features) $this->features = $this->fromNodeArray($node->field_unit_features);
		if($node->field_u_utilities_adv) $this->utilities = $this->fromNodeArray($node->field_u_utilities_adv);
		if($node->field_u_hvac_adv) $this->hvac = $this->fromNodeArray($node->field_u_hvac_adv);
    if($node->field_u_condition_adv) $this->condition = $this->fromNodeArray($node->field_u_condition_adv);

    //added by cthorn
    //adding more amenties to the xml
    if($node->field_u_features_adv) $this->features_adv = $this->fromNodeArray($node->field_u_features_adv);
    //if($node->field_u_floorsceilings_adv) $this->floorsceiling = $this->fromNodeArray($node->field_u_floorsceilings_adv);
    //if($node->field_u_windows_adv) $this->windows = $this->fromNodeArray($node->field_u_windows_adv);
    //if($node->field_u_closets_adv) $this->closets = $this->fromNodeArray($node->field_u_closets_adv);
    if($node->field_u_windows_adv) $this->windows = $this->fromNodeArray($node->field_u_windows_adv);
    if($node->field_u_closets_adv) $this->closets = $this->fromNodeArray($node->field_u_closets_adv);
    if($node->field_u_archit_det_adv) $this->archit_det = $this->fromNodeArray($node->field_u_archit_det_adv);
    if($node->field_u_addl_rooms_adv) $this->addl_rooms = $this->fromNodeArray($node->field_u_addl_rooms_adv);
    if($node->field_u_views_adv) $this->views_adv = $this->fromNodeArray($node->field_u_views_adv);
    if($node->field_u_lease_type_adv) $this->lease_typeadv = $this->fromNodeArray($node->field_u_lease_type_adv);
    if($node->field_u_accessible_adv) $this->accessible = $this->fromNodeArray($node->field_u_accessible_adv);
    if($node->field_u_kitchen_type_adv) $this->kitchen_type = $this->fromNodeArray($node->field_u_kitchen_type_adv);
    if($node->field_u_kitchen_features_adv) $this->kitchen_features = $this->fromNodeArray($node->field_u_kitchen_features_adv);
    if($node->field_u_cabinet_adv) $this->cabinet = $this->fromNodeArray($node->field_u_cabinet_adv);
    if($node->field_u_countertops_adv) $this->countertops = $this->fromNodeArray($node->field_u_countertops_adv);
    if($node->field_u_backsplash_adv) $this->backsplash = $this->fromNodeArray($node->field_u_backsplash_adv);
    if($node->field_u_refridge_adv) $this->refridge = $this->fromNodeArray($node->field_u_refridge_adv);
    if($node->field_u_oven_adv) $this->oven = $this->fromNodeArray($node->field_u_oven_adv);
    if($node->field_u_sink_adv) $this->sink = $this->fromNodeArray($node->field_u_sink_adv);
    if($node->field_u_kitchen_floors_adv) $this->kitchen_floors = $this->fromNodeArray($node->field_u_kitchen_floors_adv);
    if($node->field_u_spec_kitchen_adv) $this->spec_kitchen = $this->fromNodeArray($node->field_u_spec_kitchen_adv);
    if($node->field_u_bath_features_adv) $this->bath_features = $this->fromNodeArray($node->field_u_bath_features_adv);
    if($node->field_u_bath_floor_adv) $this->bath_floor = $this->fromNodeArray($node->field_u_bath_floor_adv);
    if($node->field_u_bath_sink_adv) $this->bath_sink = $this->fromNodeArray($node->field_u_bath_sink_adv);
    if($node->field_u_luxe_features_adv) $this->luxe_features = $this->fromNodeArray($node->field_u_luxe_features_adv);
    if($node->field_u_type_adv) $this->apt_type = $this->fromNodeArray($node->field_u_type_adv);
    if($node->field_u_outdoor_adv) $this->outdoor = $this->fromNodeArray($node->field_u_outdoor_adv);
    if($node->field_kitchen_condition) $this->kitchen_condition = $this->fromNodeArray($node->field_kitchen_condition);
    if($node->field_kitchen_amenities) $this->kitchen_amenities = $this->fromNodeArray($node->field_kitchen_amenities);
    if($node->field_kitchen_appliances) $this->kitchen_appliances = $this->fromNodeArray($node->field_kitchen_appliances);
    if($node->field_bathroom_condition) $this->bathroom_condition = $this->fromNodeArray($node->field_bathroom_condition);
    if($node->field_bathroom_features) $this->bathroom_features = $this->fromNodeArray($node->field_bathroom_features);
    if($node->field_unit_type) $this->unit_type = $this->fromNodeArray($node->field_unit_type);
    if($node->field_u_floorsceilings_adv) $this->floorceiling = $this->fromNodeArray($node->field_u_floorsceilings_adv);

		
		//hide unit number if checked.
		if($node->field_unit_name_show[0]['value'] != '')
		{
			if($this->bed == 0)
				$this->name = 'Studio ';
			else
				$this->name = $this->bed . ' bd ';
			$this->name .= $this->bath . ' ba';
		}
		
		//but leave unit number in another field - incase feed overrides
		$this->unit_num = $node->title;
	}
	
	public function getFiles()
	{
		$files = parent::getFiles();
		
		if($this->photos)
			$files['field_u_photos'] = $this->photos;
		
		if($this->floorplans)
			$files['field_u_floorplans'] = $this->floorplans;
		
		return $files;
	}
}

?>
