Drupal.behaviors.blvdSearch = function(context) {
	jQuery('form.blvd-search-apartments').each(function(){
		new Drupal.blvdSearch(this, Drupal.settings.blvdSearch.apartments);
	});
	jQuery('form.blvd-search-homes').each(function(){
		new Drupal.blvdSearch(this, Drupal.settings.blvdSearch.homes);
	});
}

Drupal.blvdSearch = function(form, settings)
{
	jQuery(form).submit(function() {
		filters = new Array();
		keyword = jQuery('input[name=keywords]', form).val().replace(settings.keywordDefault, '');
		bed = jQuery('select[name=bedrooms]', form).val();
		bath = jQuery('select[name=bathrooms]', form).val();
		town = jQuery('select[name=town]', form).val();
		pets = jQuery('#edit-pets', form).val();
		parking = jQuery('#edit-parking', form).val();
		views = jQuery('select[name=views]', form).val();
		pricemin = jQuery('.price select[name=min]', form).val();
		pricemax = jQuery('.price select[name=max]', form).val();
		
		if(pricemax > 0)
			pricemin = pricemin > 0 ? parseInt(pricemin) : 0;
		else if(pricemin > 0)
			pricemax = '*';
		
		if(bed) filters.push('tid:' + bed);
		if(bath) filters.push('tid:' + bath);
		if(town) filters.push('tid:' + town);
		if(pets) filters.push('tid:' + pets);
		if(parking) filters.push('tid:' + parking);
		if(views) filters.push('tid:' + views);
		if(pricemin || pricemax) filters.push('ps_cck_field_rent:[' + pricemin + ' TO ' + pricemax + ']');
		
		window.location = form.action + (keyword ? '/' + keyword : '') + (filters.length ? '?filters=' + filters.join(' ') : '');
		return false;
	});
	
	// onfocus, onblur reset hint text.
	jQuery('input[name=keywords]', form).each(function(){
		jQuery(this).focus(function(){
			if(jQuery(this).val() == settings.keywordDefault) jQuery(this).val('');
		});
		jQuery(this).blur(function(){
			if(jQuery(this).val() == '') jQuery(this).val(settings.keywordDefault);
		});
		
	});
	
	// bldg jumplist
	jQuery('select[name=bldg]', form).each(function(){
		jQuery(this).change(function(){
			if(jQuery(this).val() != '')
				window.location = jQuery(this).val();
		});
	});
}