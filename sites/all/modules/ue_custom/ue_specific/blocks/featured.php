<?
	function ue_specific_featured_listings()
	{
		$rtn = '';
		$colWidth = '202';
		
		$rtn .= '<div style="width:' . $colWidth*2 . 'px;float:left"><h3>Featured Rental Listings</h3>' . views_embed_view('getFeatured', 'block_3', 'Listing') . '</div>';
		//$rtn .= '<div style=\width:' . $colWidth . ';float:left;margin-left:16px"><h3>Featured Sales Listings</h3>' . views_embed_view('getFeatured', 'default', 'Listing') . '</div>';
		$rtn .= '<div style="width:' . $colWidth . 'px;float:left;margin-left:16px"><h3>Featured Buildings</h3>' . views_embed_view('getFeatured', 'default', 'Property') . '</div>';
		//$rtn .= '<div style="width:' . $colWidth . 'px;float:left;margin-left:16px"><h3>Featured Companies</h3>' . views_embed_view('companies', 'block_4', 'Company') . '</div>';
		
		$rtn .= '<div class="clear"></div>';
		
		return '<div class="featured">' . $rtn . '</div>';
	}
?>