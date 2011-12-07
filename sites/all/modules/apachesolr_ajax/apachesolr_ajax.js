// $Id: apachesolr_ajax.js,v 1.15 2010/07/06 19:40:53 jpmckinney Exp $

Drupal.apachesolr_ajax = {};

if (Drupal.settings.apachesolr_ajax) {
  var blocks  = Drupal.settings.apachesolr_ajax.blocks;
  var content = Drupal.settings.apachesolr_ajax.content;
  var regions = Drupal.settings.apachesolr_ajax.regions;
  var spinner = Drupal.settings.apachesolr_ajax.spinner;

  Drupal.apachesolr_ajax.initialize = function () {
    if (content) {
      Drupal.apachesolr_ajax.ajax(content);
    }
    if (blocks) {
      for (var block in blocks) {
        Drupal.apachesolr_ajax.ajax(blocks[block]);
      }
    }
  };

  Drupal.apachesolr_ajax.url_to_state = function (url) {
    return url.replace(new RegExp('^.*' + Drupal.settings.basePath + 'search/apachesolr_[a-z_]+/?'), '');
  };

  Drupal.apachesolr_ajax.request_callback = function (state) {
    if (spinner) {
      if (content) {
        $(content).html($('<img/>').attr('src', spinner));
      }
    }
    $.post(Drupal.settings.basePath + 'search/apachesolr_ajax/' + state, { js: 1 }, Drupal.apachesolr_ajax.response_callback, 'json');
  };

  Drupal.apachesolr_ajax.response_callback = function (data) {
    for (var setting in data.settings) {
      Drupal.settings[setting] = data.settings[setting];
    }

    var list = [];

    // Schedule items for removal to reduce page jumpiness.
    if (blocks) {
      for (var block in blocks) {
        list.push($(blocks[block]));
      }
    }
    for (var region in data.regions) {
      if (region == 'apachesolr_ajax') {
        if (content) {
          var elements = $(data.regions[region]);
          Drupal.attachBehaviors(elements.appendTo($(content).empty()));
        }
      }
      else {
        for (var block in data.regions[region]) {
          if (regions[region] && blocks[block]) {
            var elements = $(data.regions[region][block]);
            Drupal.attachBehaviors(elements.appendTo(regions[region]));
          }
        }
      }
    }
    for (var i = 0, l = list.length; i < l; i++) {
      list[i].remove();
    }
  };

  Drupal.apachesolr_ajax.navigate = function (url) {
    var state = Drupal.apachesolr_ajax.url_to_state(url) || '?'; // state cannot be empty.
    try {
      YAHOO.util.History.navigate('q', state);
    }
    catch (e) {
      Drupal.apachesolr_ajax.request_callback(state);
    }
    return false;
  };

  Drupal.apachesolr_ajax.ajax = function (selector) {
    $(selector + ' a[href*=' + Drupal.settings.basePath + 'search/apachesolr_]').livequery('click', function () {
      return Drupal.apachesolr_ajax.navigate($(this).attr('href'));
    });
    $(selector + ' form[action*=' + Drupal.settings.basePath + 'search/apachesolr_]').livequery('submit', function () {
      return Drupal.apachesolr_ajax.navigate($('#edit-keys').val()); // @todo Add support for retain-filters.
    });
  };
}

$(function () {
  var initialState = YAHOO.util.History.getBookmarkedState('q') || Drupal.apachesolr_ajax.url_to_state(window.location.href);
  YAHOO.util.History.register('q', initialState, Drupal.apachesolr_ajax.request_callback);

  YAHOO.util.History.onReady(function () {
    Drupal.apachesolr_ajax.initialize();
    var currentState = YAHOO.util.History.getCurrentState('q');
    Drupal.apachesolr_ajax.request_callback(currentState);
  });

  try {
    YAHOO.util.History.initialize('yui-history-field', 'yui-history-iframe');
  }
  catch (e) {
    Drupal.apachesolr_ajax.request_callback(initialState);
  }
});
