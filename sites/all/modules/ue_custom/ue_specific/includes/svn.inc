<?
	function ue_custom_status_svn()
	{
		drupal_set_title('Subversion Status');
		$status = shell_exec("cat /etc/motd") . "\n";
		$status .= shell_exec("svn st -u");
		return "<br><textarea readonly style=\"width:100%;height:300px;font:9pt Courier New;background:#000;color:#CCC;padding:3px\">$status</textarea>";
	}

?>