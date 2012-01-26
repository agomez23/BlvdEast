/**
 * Automatic ajax validation
 *
 * @see http://drupal.org/project/ajax
 * @see irc://freenode.net/#drupy
 * @depends Drupal 6
 * @author brendoncrawford
 * @note This file uses a 79 character width limit.
 * 
 * @see http://drupal.org/node/114774#javascript-behaviors
 *
 */

/**
 * Disables redirection for Ajax forms
 * 
 * @param {String} hook
 * @param {Object} args
 * @return {Bool}
 */
Drupal.Ajax.plugins.disable_redirect = function(hook, args) {
  if (hook === 'complete') {
    if (args.options.disable_redirect === true) {
      args.local.form[0].reset();
      if (args.options.remove_form === true) {
         args.local.form.empty();
         var get_response = function (data) {
 	        var result = Drupal.parseJson(data);
			args.local.form.html(result['content']);
       	}	
       	$.get(Drupal.settings.basePath + "ajax_webform", {url:args['redirect']}, get_response);
      }
      else {
        $('.form-item :input', args.local.form[0])[0].focus();
      }
      return false;
    }
  }
}


