<?
function ue_specific_ads()
{
	$rtn = '';
	$arrAds = array();
	$arrAds[] = array('banner_ad-skyline.png','http://www.skylinedevelopers.com/?utm_source=Urbanedgeskylinehomepagebanner&utm_medium=Urbanedgeskylinehomepagebanner&utm_campaign=Urbanedgeskylinehomepagebanner');
	$arrAds[] = array('banner_ad-claridges.png','http://www.claridgesnyc.com/?utm_source=Urbanedgeclaridgeshomepagebanner&utm_medium=Urbanedgeclaridgeshomepagebanner&utm_campaign=Urbanedgeclaridgeshomepagebanner');
	$arrAds[] = array('banner_ad-200west.png','http://www.200west72.com/?utm_source=Urbanedge200westhomepagebanner&utm_medium=Urbanedge200westhomepagebanner&utm_campaign=Urbanedge200westhomepagebanner');
	$arrAds[] = array('banner_ad-manhattan-park.png','http://www.manhattanpark.com/?utm_source=Urbanedgemanparkhomepagebanner&utm_medium=Urbanedgemanparkhomepagebanner&utm_campaign=Urbanedgemanparkhomepagebanner');
	$arrAds[] = array('banner_ad-glenwood.png','http://www.glenwoodnyc.com/?utm_source=Urbanedgeglenwoodhomepagebanner&utm_medium=Urbanedgeglenwoodhomepagebanner&utm_campaign=Urbanedgeglenwoodhomepagebanner');
	$arrAds[] = array('banner_ad-silvertowers.png','http://silvertowers.com/?utm_source=Urbanedgesilvertowershomepagebanner&utm_medium=Urbanedgesilvertowershomepagebanner&utm_campaign=Urbanedgesilvertowershomepagebanner');
	$rand = rand(0,count($arrAds)-1);
	//print_r($arrAds[$rand]);
	$img = $arrAds[$rand][0];
	$href = $arrAds[$rand][1];
	$rtn = '<a target="_blank" href="'.$href.'" onclick="javascript: pageTracker._trackPageview(\'/banner_track/'.$href.'\'); "><img src="/sites/default/files/'.$img.'" border="0" /></a>';
	return $rtn;
}
?>