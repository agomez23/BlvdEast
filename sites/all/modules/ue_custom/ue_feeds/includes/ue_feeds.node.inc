<?
	/* delete all queued files and mappings. */
	function ue_feeds_nodeapi(&$node, $op, $a3 = NULL, $a4 = NULL)
	{
		if(in_array($node->type, array('listing', 'property')))
		{
			switch($op)
			{
				case 'prepare':
				case 'load':
					// Cannot load if the node has not been created yet.
					if (!isset($node->nid))
						return;
					
					// Append the targets to the node for editing.
					$node->targets = ue_feeds_get_targets($node->nid);
				break;
				case 'update':
				case 'insert':
					// Update node targets.
					ue_feeds_update_targets($node);
				break;
				case 'delete':
					ue_feeds_delete_targets($node->nid);
					db_query('DELETE FROM {ue_feeds_files} WHERE nid = %d', $node->nid);
					db_query('DELETE FROM {ue_feeds_nodes} WHERE nid = %d', $node->nid);
				break;
			}
		}
	}
	
?>
