<?
	function ue_specific_block_manage()
	{
		global $user;
		$node = node_load(arg(1));
		/*
		if(array_key_exists(6, $user->roles) || array_key_exists(7, $user->roles))
		{
			$rtn = '<div class="manage-heading">Photos</div>';
			$rtn .= views_embed_view('manageSub', 'default');
			//$rtn .= '<a class="addPhoto thickbox" href="' . url('node/add/photo/parent/' . arg(1)) . '?destination=' . substr($_SERVER['REQUEST_URI'], 1) . '&keepThis=true&TB_iframe=true&height=480&width=640&modal=true#TB_inline">New Image</a><br><br>';
		}
		*/
		
		
		if($node->type != 'property')
		{
			/*
			if(array_key_exists(6, $user->roles) || array_key_exists(7, $user->roles))
			{
				$rtn .= '<div class="manage-heading">Floorplans</div>';
				$rtn .= views_embed_view('manageSub', 'default', 'floorplan');
				//$rtn .= '<a class="addFloorPlan thickbox" href="' . url('node/add/floorplan/parent/' . arg(1)) . '?destination=' . substr($_SERVER['REQUEST_URI'], 1) . '&keepThis=true&TB_iframe=true&height=480&width=640&modal=true#TB_inline">New Floorplan</a><br><br>';
			}
			*/
			/*
			if($user->uid == 1)
			{
			*/
				//$rtn .= '<div class="manage-heading">Metro Ads</div>';
				//$rtn .= views_embed_view('manageSub', 'default', 'metroad');
				//$rtn .= '<a class="addAd" href="' . url('node/add/metroad/parent/' . arg(1)) . '?destination=' . substr($_SERVER['REQUEST_URI'], 1) . '">Create Metro Ad</a> (<a class="blue" target="_blank" href="/owner-services/metro-new-york">What\'s this</a>)<br>';
			//}
		}
		else if($node->type == 'property' && (array_key_exists(6, $user->roles) || array_key_exists(7, $user->roles)))
		{
			//$rtn .= '<div class="manage-heading">Listings</div>';
			
			$rtn .= '<a class="editProperty" href="/manage-listings?companyID[]=all&propertyID[]='.arg(1).'" style="font-weight:bold;">Manage listings</a><br>';
			
			$rtn .= '<a class="addListing" href="' . url('node/add/listing/parent/' . arg(1)) . '?destination=' . substr($_SERVER['REQUEST_URI'], 1) . '">New Listing</a><br>';
			
			
			
			//$rtn .= views_embed_view('manageSub', 'default', 'listing');
			
			
			
			if($user->uid == 1)
			{
				
				//$rtn .= '<div class="manage-heading">Company</div>';
				//$rtn .= views_embed_view('manageSub', 'default', 'company');
				$rtn .= '<a class="addCompany thickbox" href="' . url('relativity/listnodes/company/parent/' . arg(1)) . '?destination=' . substr($_SERVER['REQUEST_URI'], 1) . '&keepThis=true&TB_iframe=true&height=480&width=640&modal=true#TB_inline">Attach Company</a><br>';
			}
		}
		
		
		//$rtn .= '<div class="manage-heading">Open Houses</div>';
		
		$rtn .= '<a class="addOpenHouse thickbox" href="' . url('node/add/openhouse/parent/' . arg(1)) . '?destination=' . substr($_SERVER['REQUEST_URI'], 1) . '&keepThis=true&TB_iframe=true&height=480&width=640&modal=true#TB_inline">Set Open House</a><br>';
		//$rtn .= '<a class="addOpenHouse" href="' . url('node/add/openhouse/parent/' . arg(1)) . '">New Open House</a><br><br>';
		$rtn .= views_embed_view('manageSub', 'default', 'openhouse');
		
		return $rtn;
	}
?>