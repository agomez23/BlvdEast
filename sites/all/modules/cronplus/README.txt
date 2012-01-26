$Id: README.txt,v 1.5 2007/03/22 23:41:18 syscrusher Exp $

cronplus.module		CronPlus adds more "cron" hooks to Drupal beyond the
			basic one, with each of the new hooks being called
			at or near specified time intervals (hourly, daily,
			weekly, monthly, and yearly).

Homepage:               http://drupal.org/node/24208

Author:			Scott Courtney (drupal.org name "syscrusher")
			email: syscrusher (at) 4th (dot) com

License:		GNU General Public License (GPL) http://gnu.org/

Status:			BETA -- Believed stable, but use with caution.

Prerequisites:		You must have cron.php being invoked at intervals on
			your system. CronPlus is believed to be compatible
			with Poor Man's Cron (a contributed module), but this
			has not been specifically tested.

			This version works with Drupal 5.x. Older versions
			are available for Drupal 4.5, 4.6, and 4.7.

Features:

* Adds hourly, daily, weekly, monthly, and yearly cron hooks to any Drupal
  module.

* Implementation of the hooks is totally optional; existing modules that
  do not use CronPlus will simply be ignored by this module.

* Administrative preferences allow advisory settings for the preferred
  time of day and day of week on which the longer-period jobs should
  be invoked.

* Modules receive parameters indicating how long it has been since the
  last time each given hook was invoked, and may use these parameters as
  they wish.

* The administrative settings menu also offers a "status report" page that
  shows what modules are using each of the API hooks, and when each hook
  was last invoked.

Related Files:

* INSTALL.txt       Installation instructions -- PLEASE READ!

* API.txt           Information for developers implementing CronPlus hooks.

* CHANGELOG.txt     Change log for the module.
