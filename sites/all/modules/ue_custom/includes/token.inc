<?
	/**
	* Implementation of hook_token_list().
	*/
	function ue_custom_token_list($type = 'all')
	{
		if ($type == 'node' || $type == 'all')
		{
			$tokens['relativity']['parent-path'] = t('Parent node path');
			return $tokens;
		}
	}
	
	/**
	* Implementation of hook_token_values().
	*/
	function ue_custom_token_values($type, $object = NULL, $options = array())
	{
		$tokens['parent-path'] = '';
		
		if ($type == 'node')
		{
			// check if the node has parents
			if (isset($object->parent_node))
			{
				$parent = node_load($object->parent_node);
				$tokens['parent-path'] = $parent->path;
			}
			else
			{
				$result = db_query('SELECT parent_nid as pid FROM {relativity} WHERE nid = %d', $object->nid);
				if($parent = db_fetch_object($result))
				{
					$parent = node_load($parent->pid);
					$tokens['parent-path'] = $parent->path;
				}
			}
		}
		return $tokens;
	}
?>