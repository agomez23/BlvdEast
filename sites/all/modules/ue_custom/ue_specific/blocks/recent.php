<?
	function ue_specific_recent_listings()
	{
		$rtn = '';
		$colWidth = '202';
		
		$rtn .= '<div class="custom_view_col" style="width:' . $colWidth*2 . 'px ;float:left;"><h3>Fresh on the Market</h3>' . views_embed_view('getFeatured', 'block_2', 'Listing') . '</div>';
		//$rtn .= '<div style=\width:' . $colWidth . ';float:left;margin-left:16px"><h3>Featured Sales Listings</h3>' . views_embed_view('getFeatured', 'default', 'Listing') . '</div>';
		$rtn .= '<div class="custom_view_col" style="width:' . $colWidth . 'px ;float:left;margin-left:16px"><h3>Featured Buildings</h3>' . views_embed_view('getFeatured', 'default', 'Property') . '</div>';
		//$rtn .= '<div style="width:' . $colWidth . ';float:left;margin-left:16px"><h3>Featured Companies</h3>' . views_embed_view('getFeatured', 'block_1', 'Company') . '</div>';
		
		$rtn .= '<div class="clear"></div>';
		
		return '<div class="featured">' . $rtn . '</div>';
	}
?>