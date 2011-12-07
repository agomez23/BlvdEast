jQuery(function(){
	/*jQuery('body').add(document).scroll(function(){
		min = 0;
		if(jQuery('#block-ue_specific-1').prev().length > 0)
			min = jQuery("#block-ue_specific-1").prev().outerHeight()+jQuery('#block-ue_specific-1').prev().offset().top-jQuery('#block-ue_specific-1').parent().offset().top+16;
		max = jQuery('#main-inner').innerHeight()-jQuery('#block-ue_specific-1').outerHeight()-19;
		jQuery('#block-ue_specific-1').css({position:'absolute'}).stop().animate({
			top:Math.min(Math.max(jQuery(document).scrollTop()-jQuery('#sidebar-right-inner').offset().top+16,min), max)
		})}
	);
	
	jQuery('#block-ue_specific-1 .content>ul>li:gt(0) ul').hide();
	*/
	//jQuery('#block-ue_specific-1 li:first-child a[rel=form]').addClass('active');	
	
	jQuery('#block-ue_specific-1 a[rel=form]').click(function(){switchTab(jQuery(this).attr('href').replace('#', '')); return false});
	/*{
		newtab = jQuery(this).attr('href').replace('#', '');
		
		jQuery(this).parent('li').addClass('active').siblings('.active').removeClass('active');
		Cufon.refresh();
		var fieldset = jQuery(jQuery(this).attr('href').replace('#', 'fieldset.') + ':hidden');
		if(fieldset.length > 0)
		{
			tohide = fieldset.siblings('.uetab:visible');
			tohide.animate({left:-1*jQuery("#node-form").innerWidth()}, 'fast', null, function(){jQuery(this).slideToggle();fieldset.css({left:jQuery("#node-form").innerWidth()}).slideToggle(null, function(){jQuery(this).animate({left:0}, 'fast', 'swing')})});
			
			//jQuery(this).parents('#block-ue_specific-1').find('ul ul:visible').not(jQuery(this).siblings('ul')).slideToggle();
			//jQuery(this).siblings('ul:hidden').slideToggle();
		}
		return false;
	});*/
	
	function switchTab(newtab)
	{
		jQuery('.form-tabs a[href=#' + newtab + ']').parent('li').addClass('active').siblings('.active').removeClass('active');
		Cufon.refresh();
		var fieldset = jQuery('fieldset.' + newtab + ':hidden');
		if(fieldset.length > 0)
		{
			tohide = fieldset.siblings('.uetab:visible');
			
			tohide.animate({
				left:-tohide.innerWidth()
			}, 'fast', function(){
				jQuery(this).slideUp('fast');
				jQuery(jQuery('.form-tabs li.active a').attr('href').replace(/#/, '.')).show().css({
					left:jQuery("#node-form").innerWidth(),
					top:0
				}).animate({left:0}, 'fast', 'swing');
			});
			
			if(fieldset.next('.uetab').length == 0)
				jQuery('.nextpg-container:visible').fadeOut();
			else
				jQuery('.nextpg-container:hidden').fadeIn();
			//jQuery(this).parents('#block-ue_specific-1').find('ul ul:visible').not(jQuery(this).siblings('ul')).slideToggle();
			//jQuery(this).siblings('ul:hidden').slideToggle();
		}
	}
	
	jQuery('#edit-submit').before('<span class="nextpg-container"><input type="button" id="edit-nextpg" value="Next Page"><span class="button-separator">OR</span></span>');
	jQuery('#edit-nextpg').click(function(){switchTab(jQuery('.uetab:visible').next('.uetab').attr('class').replace(' uetab', '')); jQuery.scrollTo('#page'); return false});
	
	//jQuery('a.savework').click(function(){jQuery('#node-form').submit()});
	//enableSave = function(){
	//	jQuery('a.savework').click(function(){jQuery('#node-form').submit();jQuery('a.savework').unbind('click').addClass('greyed').find('img').attr('src','/sites/all/themes/ue/images/save-your-work-greyed.png')}).removeClass('greyed');
	//	jQuery('a.savework img').attr('src','/sites/all/themes/ue/images/save-your-work.png');
	//}

	//jQuery(':input, :checkbox, :radio', '.form-item').change(enableSave).keyup(enableSave);
})