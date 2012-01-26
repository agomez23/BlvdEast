
/**
 * @file
 * Provides the ApacheSolr Money jQuery slider settings and actions.
 */

Drupal.theme.prototype.delayPriceSubmit = function(){
  window.setTimeout(function(){
    $("#apachesolr-money-price-range-form").submit();
  }, 500);
};

Drupal.theme.prototype.loadPriceSlider = function(){
  $("#range-slider").slider({
    range: true,
    min: parseInt(Drupal.settings.apacheSolrMoneyMin),
    max: parseInt(Drupal.settings.apacheSolrMoneyMax),
    values: [parseInt(Drupal.settings.apacheSolrMoneyFrom), parseInt(Drupal.settings.apacheSolrMoneyTo)],
    slide: function(event, ui){
      $("#edit-range-from").val(ui.values[0]);
      $("#edit-range-to").val(ui.values[1]);
    }
  });
  $(".block-apachesolr_money .form-submit").hide();
  $("#range-slider").slider("values", 0, $("#edit-range-from").val());
  $("#range-slider").slider("values", 1, $("#edit-range-to").val());
  $("#edit-range-from").keyup(function(){
    if (this.value <= $("#range-slider").slider("values", 1)) {
      $("#range-slider").slider("values", 0, this.value);
    }
    else {
      $("#range-slider").slider("values", 0, $("#range-slider").slider("values", 1));
    }
  });
  $("#edit-range-to").keyup(function(){
    if (this.value >= $("#range-slider").slider("values", 0)) {
      $("#range-slider").slider("values", 1, this.value);
    }
    else {
      $("#range-slider").slider("values", 1, $("#range-slider").slider("values", 0));
    }
  });
  $("#edit-range-from").keyup(function(){
    Drupal.theme('delayPriceSubmit');
  });
  $("#edit-range-to").keyup(function(){
    Drupal.theme('delayPriceSubmit');
  });
  // we add a .mouseover check to prevent unwanted submitting (e.g. caused by the the ajax "I like" flags)
  // @todo find a more stable solution for the following
  $("#range-slider").mouseenter(function(){
    $("#range-slider").bind("slidechange", function(event, ui){
      Drupal.theme('delayPriceSubmit');
    });
  });
  $("#range-slider").mouseleave(function(){
    $("#range-slider").unbind("slidechange");
  });
}

Drupal.behaviors.apachesolr_money = function(context){
  Drupal.settings.apacheSolrMoneyMin = $("#edit-range-min").val();
  Drupal.settings.apacheSolrMoneyMax = $("#edit-range-max").val();
  Drupal.settings.apacheSolrMoneyFilters = $('#edit-ajax-filters').val();
  
  Drupal.theme('loadPriceSlider');
}
