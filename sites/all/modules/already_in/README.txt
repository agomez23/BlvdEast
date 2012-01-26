DESCRIPTION
-----------
This module redirects authenticated users who visit user/(register|password|login) to /user page instead. This avoids authenticated users getting an Access Denied error message.


REQUIREMENTS
------------
Drupal 6.x


INSTALLING
----------
1. Copy the 'already_in' folder to your sites/all/modules directory.
2. Go to Administer > Site building > Modules. Enable the module.


CONFIGURING AND USING
---------------------
1. Go to Administer > User management > Permissions. Under the 'already_in module' section, set the appropriate permissions.
2. Go to Administer >  Site configuration > Already in. Confirgure the module options, as needed. 


REPORTING ISSUE. REQUESTING SUPPORT. REQUESTING NEW FEATURE
-----------------------------------------------------------
1. Go to the module issue queue at http://drupal.org/project/issues/already_in?status=All&categories=All
2. Click on CREATE A NEW ISSUE link.
3. Fill the form.
4. To get a status report on your request go to http://drupal.org/project/issues/user


UPGRADING
---------
1. One of the most IMPORTANT things to do BEFORE you upgrade, is to backup your site's files and database. More info: http://drupal.org/node/22281
2. Disable actual module. To do so go to Administer > Site building > Modules. Disable the module.
3. Just overwrite (or replace) the older module folder with the newer version.
4. Enable the new module. To do so go to Administer > Site building > Modules. Enable the module.
5. Run the update script. To do so go to the following address: www.yourwebsite.com/update.php
Follow instructions on screen. You must be log in as an administrator (user #1) to do this step.

Read more about upgrading modules: http://drupal.org/node/250790


CONTRIBUTIONS
-------------
* This module is based on code posted on drupal.org by loze and asimov:
    http://drupal.org/node/118498#comment-894619
    http://drupal.org/node/118498#comment-1891590

