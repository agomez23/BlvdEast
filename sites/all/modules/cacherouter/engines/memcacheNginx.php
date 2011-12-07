<?php
/**
 * $Id: memcache.php,v 1.1.2.13 2009/09/05 13:03:25 slantview Exp $
 *
 * @file memcache.php
 *   Engine file for memcache.
 */
require_once 'memcache.php';
class memcacheNginxCache extends memcacheCache {
  /**
   * key()
   *   Get the full key of the item
   *
   * @param string $key
   *   The key to set.
   * @return string
   *   Returns the full key of the cache item.
   */
  function key($key) {
    if($this->name == 'cache_page')
        return $this->prefix . $this->name . '-' . $key;
    else
        return urlencode($this->prefix . $this->name .'-'. $key);
  }
  
  function set($key, $value, $expire = CACHE_PERMANENT, $headers = NULL) {    
    // Create new cache object.
    $cache = new stdClass;
    $cache->cid = $key;
    $cache->created = time();
    $cache->expire = $expire;
    $cache->headers = $headers;
    $cache->data = $value;
    
    if ($expire == CACHE_TEMPORARY || $expire == CACHE_PERMENANT) {
      $set_expire = 0;  
    }
    
    if (!empty($key)) {
      if ($this->settings['shared']) {
        if ($this->lock()) {
          // Get lookup table to be able to keep track of bins
          $lookup = $this->memcache->get($this->lookup);

          // If the lookup table is empty, initialize table
          if (empty($lookup)) {
            $lookup = array();
          }

          // Set key to 1 so we can keep track of the bin
          $lookup[$this->key($key)] = $expire;

          // Attempt to store full key and value
          if ($this->name == 'cache_page' && !$this->memcache->set($this->key($key), $cache->data, $this->settings['compress'], $set_expire)) {
            unset($lookup[$this->key($key)]);
            $return = FALSE;
          }
          elseif ($this->name != 'cache_page' && !$this->memcache->set($this->key($key), $cache, $this->settings['compress'], $set_expire)) {
            unset($lookup[$this->key($key)]);
            $return = FALSE;
          }
          else {
            // Update static cache
            Cache::set($this->key($key), $cache);
            $return = TRUE;
          }

          // Resave the lookup table (even on failure)
          $this->memcache->set($this->lookup, $lookup, FALSE, 0);  

          // Remove lock.
          $this->unlock();
        }
      }
      else {
        // Update memcache
		if($this->name == 'cache_page')
		  return $this->memcache->set($this->key($key), $cache->data, $this->settings['compress'], $set_expire);
		else
		  return $this->memcache->set($this->key($key), $cache, $this->settings['compress'], $set_expire);
      }
    }
  }
}
?>
