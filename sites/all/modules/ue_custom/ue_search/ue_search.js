$(function(){
	$('#search-keywords').focus(function(){if($(this).val() == 'enter building name, address or keyword') $(this).val('');});
	$('#search-webid').focus(function(){if($(this).val() == 'web ID') $(this).val('');});
	
	updateNH2 = function(val)
	{
		newopts = '<option value=\"' + val + '\" selected>select neighborhood (optional)</option>';
		if(nhdata[val])
		{
			opts = 0;
			for(nhindex in nhdata[val])
			{
				nh = nhdata[val][nhindex];
				newopts += '<option value=\"' + nh.tid + '\">' + nh.name + '</option>';
				opts++;
			}
			if(opts > 0)
			{
				$('#uniform-search-neighborhood2').removeClass('uniform-disabled').find('span').html('select neighborhood (optional)').attr('style', 'font-weight:bold; color:#EF3E48');
				$('#search-neighborhood2').removeAttr('disabled').html(newopts).change(function(){$(this).siblings('span').attr('style', '')});
			}
		}
		else
		{
			$('#uniform-search-neighborhood2').addClass('uniform-disabled').find('span').html('select neighborhood (optional)').attr('style', '');
			$('#search-neighborhood2').attr('disabled', 'disabled').html(newopts);
		}
	}
	
	$('#search-neighborhood').change(function(){updateNH2($(this).val())});
	//updateNH2($('#search-neighborhood').val());
});