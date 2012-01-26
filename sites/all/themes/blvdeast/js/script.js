jQuery(function(){
	//jQuery('ul.tabs a').click(function(){ return false; });
	//jQuery('ul.tabs li a').click(function(){ return false; });
	jQuery('.view-town-docs .filefield-file a, field-field-b-floorplans .filefield-file a').attr('target', '_blank');
	
	//jQuery('.page-post-listing #main').prepend('<fieldset class="single_building"><legend>Building Details</legend><div></div></fieldset>');
	jQuery('.pane-post-listing legend, .pane-post-home legend').each(function(){
		jQuery(this).after('<h3>' + jQuery(this).html() + '</h3>').remove();
	});
	
	jQuery('#edit-profile-terms-wrapper input').click(function(){
		if (jQuery(this).is(':checked')) { jQuery('.page-post-listing .form-submit-wrapper').show(); } else {jQuery('.page-post-listing .form-submit-wrapper').hide();}
	});
	
	jQuery('.page-post-listing #edit-submit').addClass('form-button-green');
	
	jQuery('.more-box').each(function(){
		moreHeight = jQuery(this).height(); //alert(moreHeight);
		if (moreHeight>172) jQuery(this).css({'height':'152px'}).append(' <a href="#" class="expand">&hellip;more</a>');
		//if (moreHeight>100) jQuery(this).css({'height':'100px'}).append(' <a href="#" class="expand">&hellip;more</a>');
		//jQuery(this).css('height','200px');
	});
	
	jQuery('.more-box .expand').click(function(){
		jQuery(this).parent('.more-box').css({'height':'auto','padding-bottom':'0'}).slideDown();
		jQuery(this).remove();
		return false;
	});
	
	jQuery('.pane-post-listing #edit-field-propertyid-nid-nid').change(function(){
		jQuery('.single_building').show();
		jQuery('.building_ajax').html('');
		jQuery.ajax({
			type: "GET",
			url: '/single_building',
			data: 'nid='+$(this).val(),
			success: function(data){
				jQuery('.building_ajax').html(data);
				
				//alert( data );
				
			}
		});
		
		return false;
	});
	
	jQuery('.pane-post-listing #edit-field-propertyid-nid-nid-wrapper .description').appendTo(jQuery('.group-unit'));
	
	jQuery('#tabs-mini-panel-search-middle').prepend(jQuery('.pane-panels-mini.pane-search h3'));
	jQuery('#tabs-mini-panel-homepage-map-middle').prepend(jQuery('.pane-panels-mini.pane-homepage-map h3'));
	jQuery('#tabs-mini-panel-port-imperial-search-middle').prepend(jQuery('.pane-panels-mini.pane-port-imperial-search h3'));
	
	//jQuery('.blvd-search #edit-parking--wrapper label').html('Available');
	
	//jQuery('#beast-block-search-homes-form #edit-bathrooms-1-wrapper').after('<div class="form-item"><label>&nbsp;</label></div>');
	
	// launch external links in new window
	jQuery("a").filter(function () {
        return this.hostname && this.hostname !== location.hostname;
    }).each(function () {
        $(this).attr({
            target: "_blank",
            title: "Visit " + this.href + " (click to open in a new window)"
        });
    });
	
	// launch PDFs in new window
	$("a[href*=.pdf]").click(function(){
		window.open(this.href);
		return false;
	});
	
	jQuery('a.neighborhood-page, .pane-availabilities-mini-panel-pane-2 .pane-title, .breadcrumb a').each(function(){
		currHref = jQuery(this).html();
		rplHref = jQuery(this).html().replace(/-/g,' '); //alert(currHref+ ' | ' +rplHref);
		jQuery(this).html(rplHref);
	});
	
	jQuery('body.page-post-listing h1.title').before(jQuery('div.home-post'));
	
	jQuery('.group-fee label .form-required').after(' &nbsp; <span class="rental-fee-desc"> 1 month\'s rent broker fee due at Lease Signing.</span>');
	
	jQuery('.panels-search .right .facet-primary h3').addClass('open');
	jQuery('.panels-search .right h3').click(function(){
		jQuery(this).siblings('.pane-content').toggle();
		jQuery(this).toggleClass('open');
		
	});
	jQuery('.panel-pane.pane-apachesolr-form label').html('Search');
	jQuery('#breadcrumbs').after(jQuery('.panel-pane.pane-apachesolr-form'));
	
	jQuery('.facet-checkbox').click(function(){
		jQuery('.panels-search .pane-beast-result .view-availabilities').addClass('opacity2');	
	});
});
