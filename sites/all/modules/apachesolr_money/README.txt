
APACHE SOLR MONEY SLIDER
========================

An integration modules for the Apache Solr and the Money CCK projects.
http://drupal.org/project/apachesolr_money

Features
========

- Provides a price range slider based on a Money CCK field

- Dual value indexed by Apache Solr for both price and amount

Requirements
============

This module has a complex set of requirements. Be careful 

First off, you need jQuery 1.3.x and jQuery UI 1.7.x. 
Update them with jQuery Update and jQUery UI. Follow their README.txt
The Money CCK has some dependencies of its, like the Currency API and Formatted 
number API. Install them first.
Apache Solr needs the 6.x-2.x branch, but currently only 6.x.-2.x-beta2 works.

- Apache Solr - 6.x-2.x-beta2 (WARNING: beta3 and dev currently fail) 
  http://drupal.org/project/apachesolr
     
- Money CCK fields - 6.x-1.x
  http://drupal.org/project/money 
    
- jQuery Update - 6.x-2.x (for upgrading to jQuery 1.3.x)
  http://drupal.org/project/jquery_update

- jQuery UI - 6.x-1.x
  http://drupal.org/project/jquery_update

WARNING: For jQuery UI, be sure to download and install jQuery UI 1.7.x (follow 
their README.txt), http://code.google.com/p/jquery-ui/downloads/list?q=1.7

Also make sure the jQuery base theme css exists at:
/jquery_ui/jquery.ui/themes/base/jquery-ui.css (included in the download)

You can override the CSS in your theme style.css

Installation
============

- Install the requirements and enable this module as usual

- Setup a working Apache Solr connection

- Setup a content type with a Money CCK field using CCK

- Enable the Money CCK field to be indexed at 'Enabled Filters' in Apache Solr 
  admin settings
  
- Re-index Solr if necessary

Configuration
============

- select the enabled CCK filters at /admin/settings/apachesolr/enabled-filters 
  (specifically the amount filter)
  
- see the fieldgroup "Apache Solr Money" - do NOT enable the CCK money fields 
  under "Apache Solr search"
  
- place the Solr block in a left or right column on the search page at 
  /search/apachesolr_search
  
- perform a search and the price slider should become visible

- !IMPORTANT: make sure your Money CCK field has a default value, like 0 USD.
  Empty values currently cannot be indexed by Apache Solr.
