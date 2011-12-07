; $Id: README.txt,v 1.1.2.1 2010/04/30 10:59:12 alliprice Exp $

Module description
------------------

The Postmark module allows the administrator to switch the standard SMTP library over to use the third party Postmark library. An account with Postmark is required to use this module.

Postmark was developed by Allister Price (alli.price) and Luke Simmons (luketsimmons) from Deeson Online (http://www.deeson.co.uk/online).


Installation
------------

Installing the Postmark module requires a few steps :

1) Copy the postmark folder to the modules folder in your installation (usually sites/all/modules).

2) Obtain the Postmark.php library from http://github.com/Znarkus/postmark-php.

3) Copy the file to the includes directory in the module folder (modules/postmark/includes).

4) Enable the module using Administer > Modules (/admin/build/modules).

5) Go to Site configuration > Postmark and enable both the Postmark functionality but also add your API key from your Postmark account.

6) The test functionality enables you to test the integration is working, this will use a credit by default each time you submit an email address.

7) The email address that emails are sent from on your site must have a valid Sender Signature set up in your Postmark account.


Support
-------

If you have any problems using the module, please submit an issue in the Postmark queue (http://drupal.org/project/issues/postmark).


Credit
------

Credit to the phpmailer (http://drupal.org/project/phpmailer) module on which this is heavily based.


Todo
----

This is a very early release of the module, work on this is ongoing, the project page will be updated with todos for this module.