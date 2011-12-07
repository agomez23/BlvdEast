<?php
	
	class ue_feeds_feed
	{
	
		// The database ID. Null for feeds that haven't been saved to the database yet.
		var $fid = NULL;
		
		// The type this feed works with.
		var $adapterclass = NULL;
		
		// The filter this feed works with.
		var $filterclass = NULL;
		
		// The feed's "machine readable" name..
		var $name = '';
		
		// The feed's uri. (accessible by http, https, or ftp)
		var $uri = '';
		
		// Feed options.
		var $options = NULL;
		
		// Last import.
		var $lastrun = NULL;
		
		// Existing nodes that have been mapped.
		var $nodes = array();
		
		// Feed targets.
		var $targets = array();
		//var $freeTargets = array();
		
		// Feed targets.
		//var $paidTargets = array();
		
		// Premium targets.
		//var $premiumTargets = array();
		
		// Default author.
		var $authorid = NULL;
		
		// authorname used in admin.
		var $authorname = NULL;
		
		// Default company.
		var $companyid = NULL;
		
		private $feedcontent = NULL;
		
		// Creates a feed from a database row. Returns it.
		function factory_by_row($row)
		{
			$feed = new ue_feeds_feed;
			
			if (class_exists($row->adapterclass))
				$feed->adapter = new $row->adapterclass($feed);
			else
				drupal_set_message(t('Could not create adapter class: @class', array('class' => $row->adapterclass)));
			
			if($row->dir == 1 && $row->filterclass)
			{
				if (class_exists($row->filterclass))
					$feed->filter = new $row->filterclass($feed);
				else
					drupal_set_message(t('Could not create filter class: @class', array('class' => $row->filterclass)));
			}
			
			// Lump all data into the object...
			foreach ($row as $field => $value) $feed->$field = $value;
			
			// unserialize options.
			$feed->options = unserialize($feed->options);
			$feed->targets = unserialize($feed->targets);
			//$feed->freeTargets = unserialize($feed->freeTargets);
			//$feed->paidTargets = unserialize($feed->paidTargets);
			//$feed->premiumTargets = unserialize($feed->premiumTargets);
			
			$author = user_load($feed->authorid);
			$feed->authorname = $author->name;
			
			// load nodes.
			$result = db_query('SELECT * FROM {ue_feeds_nodes} WHERE fid = %d', $feed->fid);
			while($map = db_fetch_object($result)) $feed->nodes[$map->eid] = $map->nid;
			
			return $feed;
		}
		
		function factory_by_type($adapterclass)
		{
			$feed = new ue_feeds_feed;
			
			if (class_exists($adapterclass))
				$feed->adapter = new $adapterclass($feed);
			else
				$feed->adapter = new ue_feeds_adapter_broken($feed);
			
			return $feed;
		}
		
		function factory_by_array($array)
		{
			if(isset($array['adapterclass']))
			{
				$adapters = ue_feeds_get_adapters();
				$feed = ue_feeds_feed::factory_by_type($adapters[$array['adapterclass']]['class']);
				
				// Lump all data into the object...
				foreach ($array as $field => $value) $feed->$field = $value;
				return $feed;
			}
		}
		
		/**
		* Saves a feed to the database. It is a wrapper around update() and insert().
		*/
		function save()
		{
			if (isset($this->fid))
				$this->update();
			else
				$this->insert();
		}
		
		/**
		* Saves an existing feed to the database. Better use save().
		*/
		function update()
		{
			db_query("UPDATE {ue_feeds} SET name = '%s', uri = '%s', options = '%s', targets = '%s', filterclass = '%s', authorid = %d, companyid = %d WHERE fid = %d", $this->name, $this->uri, serialize($this->options), serialize($this->targets), $this->filterclass, $this->authorid, $this->companyid, $this->fid);
			
			$eids = array_keys($this->nodes);
			
			db_query("DELETE FROM {ue_feeds_nodes} WHERE fid = %d AND eid NOT IN " . db_placeholders($eids, 'varchar') . ")", array_merge((array)$this->fid, $eids));
			
			$qry = "INSERT INTO {ue_feeds_nodes} (fid, nid, eid) VALUES ";
			
			foreach($this->nodes as $eid => $nid)
			{
				$args[] = $this->fid;
				$args[] = $nid;
				$args[] = $eid;
			}
			
			$qry .= substr(str_repeat(",(%d, %d, '%s')", count($this->nodes)), 1);
			
			$qry .= ' ON DUPLICATE KEY UPDATE nid = VALUES(nid)';
			
			db_query($qry, $args);
		}
		
		/**
		* Saves a new feed to the database.
		*/
		function insert()
		{
			if (function_exists('db_last_insert_id'))
			{
				// Drupal 6. We have a 'serial' primary key.
				db_query("
					INSERT INTO {ue_feeds} (
						adapterclass,
						dir,
						name,
						uri,
						options,
						targets,
						filterclass,
						authorid,
						companyid
					) VALUES (
						'%s',
						%d,
						'%s',
						'%s',
						'%s',
						'%s',
						'%s',
						%d,
						%d
					)",$this->adapterclass, $this->dir, $this->name, $this->uri, serialize($this->options), serialize($this->targets), $this->filterclass, $this->authorid, $this->companyid);
				$this->fid = db_last_insert_id('ue_feeds', 'fid');
			}
			else
			{
				// Drupal 5. We have an 'integer' primary key.
				$this->fid = db_next_id('{ue_feeds}_fid');
				db_query("INSERT INTO {ue_feeds} (fid, adapterclass, dir, name, uri, options, targets, filterclass, authorid, companyid) VALUES (%d, '%s', %d, '%s', '%s', '%s', '%s', '%s', %d, %d)", $this->fid, $this->adapterclass, $this->dir, $this->name, $this->uri, serialize($this->options), serialize($this->targets), $this->filterclass, $this->authorid, $this->companyid);
			}
		}
		
		/**
		* Deletes a feed from the database.
		*/
		function delete()
		{
			db_query('DELETE FROM {ue_feeds} WHERE fid = %d', $this->fid);
		}
		
		/* feed logic. it's business time */
		
		//returns the feed content. maybe do this with __get or __set instead.
		function read()
		{
			if(!$this->feedcontent)
			{
				$urlregex = "^(https?|ftp)\:\/\/([a-z0-9+!*(),;?&=\$_.-]+(\:[a-z0-9+!*(),;?&=\$_.-]+)?@)?[a-z0-9+\$_-]+(\.[a-z0-9+\$_-]+)*(\:[0-9]{2,5})?(\/([a-z0-9+\$_-]\.?)+)*\/?(\?[a-z+&\$_.-][a-z0-9;:@/&%=+\$_.-]*)?(#[a-z_.-][a-z0-9+\$_.-]*)?\$";
			
				//make sure it's a true url, could possibly be hacked by loading system files.	
				if(!eregi($urlregex, $this->uri)) die("Not a valid feed URI");
				
				$this->feedcontent = trim(file_get_contents($this->uri)); //trim whitespace to prevent xml declaration errors.
			}
			
			return $this->feedcontent;
		}
	}
	
?>