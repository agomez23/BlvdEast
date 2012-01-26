// $Id: scaffold.js,v 1.1 2010/08/03 11:39:59 jide Exp $

/**
 * @file
 * Adds Farbtastic to color fields used by themes using Scaffold constants.
 */

Drupal.behaviors.scaffold = function (context) {
  // This behavior attaches by ID, so is only valid once on a page.
  if ($('#scaffold-form-wrapper.color-processed').size()) {
    return;
  }

  var form = $('#scaffold-form-wrapper', context).addClass('scaffold-colorpicker-processed');

  // Add Farbtastic
  $('body').prepend('<div id="scaffold-colorpicker"></div>');
  var colorpicker = $('#scaffold-colorpicker').hide();
  var farb = $.farbtastic('#scaffold-colorpicker');

  // Link Farbtastic to fields
  $('input.form-text', form).each(function() {
    if ($(this).val().match(/^#[a-zA-Z0-9]{6,8}$/)) {
      // First link to apply color to the field.
      farb.linkTo($(this));

      // On focus, show colorpicker.
      $(this).focus(function() {
        // Position colorpicker.
        var offset = $(this).offset();
        colorpicker.css({
          'top': offset.top + ($(this).outerHeight() / 2) - (colorpicker.outerHeight() / 2),
          'left': offset.left + $(this).outerWidth()
        }).show();

        // Link it.
        farb.linkTo($(this));
      }).
      // On blur, hide it.
      blur(function() {
        colorpicker.hide();
      });
    }
  });
};
