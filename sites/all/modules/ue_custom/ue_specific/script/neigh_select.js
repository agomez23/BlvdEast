$(function(){
	
	$('a.level0').click(function(){ 
		var thisid = $(this).attr('id');
		if ($(this).parent().hasClass('expanded')) {
			$(this).parent().removeClass('expanded');
			$('.parent_'+thisid).hide().removeClass('expanded');
			$('.grandparent_'+thisid).hide();
		} else {
			$('div.level0').removeClass('expanded');
			$('div.level1, div.level1 div').hide();
			$(this).parent().addClass('expanded');
			$('.parent_'+thisid).show();
		}
		return false;
	});
	
	$('.level1 a.expand_trigger').click(function(){ 
		var thisid = $(this).attr('id');
		$(this).parent().toggleClass('expanded');
		$('.parent_'+thisid).toggle();
		disableds = $('.level2.parent_'+thisid+'.disabled');
		$('.level2.parent_'+thisid+'.disabled').remove()
		$('.level2.parent_'+thisid).appendTo($(this).parent('div.level1'));
		$(disableds).appendTo($(this).parent('div.level1'));
		return false;
	});
	
	$('div.level1 a.select_level2').click(function(){ 
		var thisid = $(this).parent('div').attr('class');
		var currChk = $('.level2.'+thisid+' input:not(:disabled)').attr('checked')?false:true;
		$('.level2.'+thisid+' input:not(:disabled)').attr('checked', currChk);
		return false;
	});
	
	var arrNeighVals = new Array; var arrNeighLabels = new Array;
	$('#neigh_select input').change(function(){ collNeigh(); });
	$('div.level1 a.select_level2').click(function(){ collNeigh(); });

	

});
	
function collNeigh() {
	arrNeighVals = new Array; arrNeighLabels = new Array;
	$('.level2 input:checked').each(function() {
		arrNeighVals.push($(this).val());
		arrNeighLabels.push($(this).attr('title'));
	});
	$('#neigh_selected').attr('value',arrNeighVals.toString());
	$('#neigh_displayed').html(arrNeighLabels.toString());
	return false;
}