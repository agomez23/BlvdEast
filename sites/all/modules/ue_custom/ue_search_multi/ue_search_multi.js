$(initSearch);

function initSearch(){
	
	$('.search-col1, .search-col2, #search-keywords, .search-checkboxes, #search-submit').css({visibility: "visible"}); $('.loading').remove();
	$('#search-keywords').focus(function(){if($(this).val() == 'enter building name or keyword') $(this).val('');});
	$('#search-webid').focus(function(){if($(this).val() == 'web ID') $(this).val('');});
	$('form.search').submit(function(){
		if (!isNaN($('#search-keywords').val()) && $('#search-keywords').val()!='') { window.location = '/node/'+$('#search-keywords').val(); return false; } else { return true; }
	});
	
	$('#search-neighborhood2').hide().filter(':enabled').parent().click(openNH);
	$('#search-neighborhood').change(clearNH);
	$('.nh-accordion label').each(function(i, label){
		toggleFunc = function(label){
			if($(':checked', label).length > 0)
				$('.uniform-checker', label).animate({left:-20}, 300, 'easeOutBack').find(':checkbox').removeAttr('checked').parent().removeClass('uniform-checked');
			else
				$('.uniform-checker', label).animate({left:0}, 300, 'easeOutBack').find(':checkbox').attr('checked', 'checked').parent().addClass('uniform-checked');
			refreshCount($(label).parent().prev('.regionTitle'));
		}
		$(label).hover(
			function(){ $(this).addClass('ui-corner-all ui-state-hover') },
			function(){ $(this).removeClass('ui-corner-all ui-state-hover') })
		.toggle(function(){ toggleFunc(this) }, function(){ toggleFunc(this) })
		.find('.uniform-checker').unbind('click')
		.find(':checkbox').mouseup(function(){ toggleFunc($(this).parents('label')); return false })
		.not(':checked').parents('.uniform-checker').css({left:-20});
	});
	/*
	$('.nh-accordion .regionSection a').hover(function(){ $(this).addClass('ui-corner-all ui-state-hover') }, function(){ $(this).removeClass('ui-corner-all ui-state-hover') })
	.click(function(){ $('span', this).animate({left:$(this).width()-$('span', this).width()}); return false })
	.contents().wrap('<span>')*/
}

function hideNH()
{
	$('.nh-popup').fadeOut().find('.nh-accordion').filter('#nh-accordion-' + $('#search-neighborhood').val()).hide().accordion('destroy');
	setTimeout(function() { $('body, #uniform-search-neighborhood2').unbind('click', hideNH); }, 500);
}
function openNH()
{
	$('.nh-popup').css({position:'absolute', top:$(this).position().top + $(this).height() - 5, left: $(this).position().left + 2}).fadeIn().find('.nh-accordion').hide().filter('#nh-accordion-' + $('#search-neighborhood').val()).show().accordion({fillSpace:true, collapsible:true, active:false});
	setTimeout(function() { $('body, #uniform-search-neighborhood2').one('click', hideNH) }, 500);
	return false;
}
function clearNH()
{
	$('.nh-popup .uniform-checker').css({left:-20}).find(':checkbox').removeAttr('checked').parent().removeClass('uniform-checked');
	$('#uniform-search-neighborhood2').removeClass('uniform-disabled').find('span').text('select neighborhood (optional)').siblings('select').removeAttr('disabled').parent().click(openNH);
}
function refreshCount(title)
{
	//refresh neighborhood names
	arr = jQuery.map($('.nh-popup :checked').parents('label').find('.label'), function(label){ return $(label).text() });
	$('#uniform-search-neighborhood2 span').text(arr.join(", "));
	
	checked = $(title).next().find(':checked').length;
	count = $('.count', title);
	if(checked == 0)
		count.remove();
	else
	{
		if(count.length > 0)
			count.text(checked);
		else
			$('a', title).append('<span class="count">' + checked + '</span>');
	}
}

/*

	toggleIn = toggleOut = function(){$('.ui-icon', this).addClass('ui-icon-plusthick').stop(true, true).toggle()};
	$('.regionSection a').draggable({opacity:.7, helper:'clone', appendTo:'body'}).hover(toggleIn, toggleOut).append('<span class="ui-icon"></span>');
	$('#nh-selected .ui-accordion-content').droppable({
		drop: function(event, ui) { selectNeighborhood(ui.draggable) }
	});
	$('.nh-accordion .regionSection a').click(function(){ selectNeighborhood($(this)); return false });
*/