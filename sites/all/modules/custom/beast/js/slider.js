if (Drupal.jsEnabled) {
  Drupal.behaviors.sliderFilter = function(context) {
	$('input.form-slider-min', context).each(function(index, el)
	{
		var widget = $(el).parents('.views-widget');
		var min = $('input[id$="-min"]', widget);
		var max = $('input[id$="-max"]', widget);
		
		if (!min.length || !max.length) {
		  // No min/max elements on this page
		  return;
		}
		
		// Set default values or use those passed into the form
		var init_min = ('' == min.val()) ? 0 : min.val();
		var init_max = ('' == max.val()) ? 10 : max.val();
	
		 // Set initial values of the slider 
		min.val(init_min);
		max.val(init_max);
		
		var regex = /edit-([^-]*)/;
		var sliderId = 'slider-' + regex.exec(min.attr('id'))[1];
		
		// Insert the slider before the min/max input elements 
		widget.prepend($('<div id="' + sliderId + '"><div class="ui-slider-handle"></div><div class="ui-slider-handle"></div></div>'));
		
		/*
		$('#' + sliderId).width(100).slider()
		.slider("option", "min", 0)
		.slider("option", "max", 10000)
		.slider("option", "values", [20,60,80])
		.slider("option", "range", true);
		*/
		
		//$('#' + sliderId).slider({ steps: 10, range: true, change: function(e,ui) { console.log(ui.range); } });
		
		
			$('#' + sliderId).width(100).slider({
			range: true,
			min: 0,     // Adjust slider min and max to the range 
			max: 5000,    // of the exposed filter.
			steps:100,
			values: [init_min, init_max],
			slide: function(event, ui){
				// Update the form input elements with the new values when the slider moves
				console.dir(ui);
				min.val(ui.value);
				max.val(ui.value);
			}
		  });
		  
		
		//min.parent().hide();
		//max.parent().hide();
	});
  };
}