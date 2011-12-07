<?php

	function ue_feeds_import($fid)
	{
		$feed = ue_feeds_load($fid);
		
		$parsed = $feed->adapter->parse();
		
		//Test mode for dev.
		$TEST_MODE = $_SERVER['SERVER_NAME'] == 'carlos.urbanedgeny.com';
		
		$TEST_MODE = false;
		
		//Set all listings to 'off the market', only ones that are active in the feed will become 'on the market'.
		
		if(count($feed->nodes))
		{
			$nids = array_values($feed->nodes);
			if(!$TEST_MODE) db_query('UPDATE {content_type_listing} SET field_unit_availability_value = 0 WHERE nid IN (SELECT r.nid FROM {relativity} r WHERE parent_nid IN (' . implode(',', $nids) . '))');
		}
		
		foreach($parsed['properties'] as $i => $prop)
		{
			if(!$TEST_MODE) $output .= 'Saving property ' . $i . ': ' . $prop->name . '.<br>';
			
			if($feed->nodes[$prop->externalID])
			{
				$origNode = node_load($feed->nodes[$prop->externalID], NULL, true);
				$propNode = $prop->toNode($origNode);
				if($TEST_MODE)
				{
					$output .= $prop->name . ' - ' . $prop->externalID . '<br>';
					$output .= '<textarea style="width:32%;height:600px">' . print_r($prop, true) . '</textarea>';
					$output .= '<textarea style="width:32%;height:600px">' . print_r($origNode, true) . '</textarea>';
					$output .= '<textarea style="width:32%;height:600px">' . print_r($propNode, true) . '</textarea><br>';
				}
				else
				{
					$output .= 'saving property: ' . $prop->name . "\n";
					node_save($propNode); // save property.
					db_query('UPDATE ue_feeds_nodes SET data = \'%s\', updated = UNIX_TIMESTAMP(NOW()) WHERE fid = %d AND eid = \'%s\'', serialize($prop), $fid, $prop->externalID);
				}
			}
			else
			{
				$propNode = $prop->toNode();
				
				//specify default company.
				if(!$propNode->field_contact_build_mgr[0]['nid'] && $feed->companyid)
					$propNode->field_contact_build_mgr[0]['nid'] = $feed->companyid;
				
				//specify default author.
				if($feed->authorid > 0) $propNode->uid = $feed->authorid;
				
				if($TEST_MODE)
				{
					$output .= $prop->name . ' - ' . $prop->externalID . '<br>';
					$output .= '<textarea style="width:32%;height:600px">' . print_r($prop, true) . '</textarea>';
					$output .= '<textarea style="width:32%;height:600px"></textarea>';
					$output .= '<textarea style="width:32%;height:600px">' . print_r($propNode, true) . '</textarea><br>';
				}
				else
				{
					$output .= 'saving new property: ' . $prop->name . "\n";
					node_save($propNode); // save property.
					db_query('INSERT INTO ue_feeds_nodes (nid, fid, eid, data) VALUES (%d, %d, \'%s\', \'%s\')', $propNode->nid, $fid, $prop->externalID, serialize($prop));
				}
			}
			
			if(!$TEST_MODE)
				ue_feeds_queue_files($prop->getFiles(), $propNode->nid); //queue property files.
			
			if(!$TEST_MODE && count($prop->listings))
			{
				foreach($prop->listings as $j => $listing)
				{
					//find existing listing
					$result = db_result(db_query('SELECT n.nid FROM {relativity} r INNER JOIN {node} n ON n.nid = r.nid AND n.title = \'%s\' WHERE r.parent_nid = %d', $listing->name, $propNode->nid));
					
					if($result)
					{
						$listingNode = $listing->toNode(node_load($result, NULL, true));
						
						$output .= 'saving listing: ' . $prop->name . ' : ' . $listing->name . "\n";
					}
					else
					{
						$listingNode = $listing->toNode();
						$listingNode->uid = $propNode->uid;
						$listingNode->parent_node = $propNode->nid;
						$listingNode->field_propertyid[0]['nid'] = $propNode->nid;
						
						$output .= 'saving new listing: ' . $prop->name . ' : ' . $listing->name . "\n";
					}
						
					//Set premium/paid syndication based on options selected in feed admin.
					$listingNode->targets = $feed->targets;
					
					//Set hide unit.
					if($feed->options->hideunit)
						$listingNode->field_unit_name_show[0]['value'] = 'Hide Unit Number in Listing';
					
					$listingNode->changed = time();
					node_save($listingNode); // save new nodes twice
					
					ue_feeds_queue_files($listing->getFiles(), $listingNode->nid); // queue listing files.
					content_update($listingNode); // save node again (this time to update url).
				}
			}
		}
		
		db_query('UPDATE ue_feeds SET lastrun = UNIX_TIMESTAMP(NOW()) WHERE fid = %d', $fid);
		
		return $output;
	}
	
	// queue files for download.
	function ue_feeds_queue_files(array &$fields, $nid)
	{
		$uid = 126;
		
		if(intval($nid) == 0 || !count($fields)) return;
		
		foreach($fields as $field => $urls)
		{
			//get existing files.
			$files = array();
			$result = db_query('SELECT * FROM {content_' . $field . '} p INNER JOIN {files} f ON f.fid = p.' . $field . '_fid WHERE p.nid = %d', $nid);
			while($file = db_fetch_object($result)) $files[$file->filename] = $file;
			
			foreach($urls as $i => $url)
			{
				//$file_temp = file_get_contents($url);
				//if file already exists just add the record to ue_feeds_files again to check if newer version
				if($files[basename($url)])
					$file = $files[basename($url)];
				elseif(strtolower(basename($url)) != 'thumbs.db') // some feeds generate photos off directory listings and end up throwing this in
				{
					//if else queue up new file.
					$file_temp = file_save_data('TEMPDATA', file_directory_path() . '/' . basename($url));
					
					$file = new stdClass();
					$file->filename = basename($url);
					$file->filepath = $file_temp;
					$file->filemime = mime_content_type($file_temp);
					$file->filesize = 0;
					$file->uid = $uid;
					$file->status = FILE_STATUS_PERMANENT;
					$file->timestamp = 0;
					
					drupal_write_record('files', $file);
					$file->fid = db_result(db_query('SELECT fid FROM {files} WHERE filepath = \'%s\'', $file->filepath));
				}
				db_query('INSERT IGNORE INTO {ue_feeds_files} (nid, fid, field, url) VALUES (%d, %d, \'%s\', \'%s\')', $nid, $file->fid, $field, $url);
			}
		}
	}
	
	//downloads 50 files.
	function ue_feeds_download_files()
	{
		$result = db_query('SELECT ff.nid, ff.field, ff.url, f.* FROM {ue_feeds_files} ff INNER JOIN {files} f ON f.fid = ff.fid ORDER BY f.timestamp LIMIT 0,70');
		while ($file = db_fetch_object($result))
		{
			//print "Downloading: $file->url<br>";
			$res = @fopen($file->url, 'r');
			
			if(!$res)
			{
				$node = node_load($file->nid);
				$vars['@node-title'] = $node->title;
				$vars['@node-path'] = url($node->path);
				$vars['@error-info'] = print_r(error_get_last(), true);
				watchdog('ue_feeds', 'Image download failed for <a href="@node-path">@node-title</a>: @error-info', $vars, 'error', l('View image', $file->url));
				//print 'FILE NOT FOUND<br><br>';
				db_query('DELETE FROM {files} WHERE fid = %d', $file->fid);
				db_query('DELETE FROM {ue_feeds_files} WHERE fid = %d AND nid = %d', $file->fid, $file->nid);
			}
			else
			{
				$meta = stream_get_meta_data($res);
				
				//print 'Meta: <br><pre>' . print_r($meta, true) . '</pre>';
				foreach($meta['wrapper_data'] as $i => $data) if(stripos($data, 'last-modified') === 0) $lastmod = strtotime(trim(substr($data, 14)));
				
				//print 'Local Date: ' . $file->timestamp . ' (' . date('r', $file->timestamp) . ') Modified: ' . $lastmod . ' (' . date('r', $lastmod) . ')<br><br>';
				
				//only download if updated or new
				if($lastmod > $file->timestamp || $file->filesize == 0)
				{
					$file_temp = stream_get_contents($res);
					$file_temp = file_save_data($file_temp, $file->filepath, FILE_EXISTS_REPLACE);
					$file->timestamp = time();
					$file->filesize = filesize($file_temp);
					$file->filemime = mime_content_type($file_temp);
					drupal_write_record('files', $file, array('fid'));
					
					$field = $file->field;
				
					$arr['fid'] = $file->fid;
					$arr['title'] = $arr['filename'] = $arr['description'] = basename($file_temp);
					$arr['filepath'] = $file_temp;
					$arr['filesize'] = filesize($file_temp);
					$arr['list'] = 1;
					$arr['mimetype'] = $file->filemime;
					$arr['data'] = array('description' => '', 'alt' => '', 'title' => '');
					
					if($nodes[$file->nid])
						$node = $nodes[$file->nid];
					else
						$node = node_load($file->nid);
					
					$files = $node->$field;
					
					if($files[0] == NULL)
						$node->$field = array($arr);
					else
					{
						$fileExists = false;
						foreach($node->$field as $i => $existingFile)
						{
							if($existingFile['fid'] == $arr['fid'])
							{
								$fileExists = $i;
								break;
							}
						}
						
						if($fileExists === false)
							array_push($node->$field, $arr); //if doesnt exist then push
						else
							array_splice($node->field, $fileExists, 1, $arr); //else replace.
					}
					
					$nodes[$node->nid] = $node;
				}
				db_query('DELETE FROM {ue_feeds_files} WHERE fid = %d AND nid = %d', $file->fid, $file->nid);
				fclose($res);
			}
		}
		
		if(count($nodes)) foreach($nodes as $id => $node) content_update($node);
	}
?>
