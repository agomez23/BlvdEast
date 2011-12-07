function checkDupe()
{
	//require title and zip to be filled before checking.
	if($("#edit-field-zipcode-0-value").val().length > 0 && $("#edit-title").val().length > 0)
		$.post('/sites/all/modules/ue_custom/ajax/duplicate.php', $(".dupeCheck").serialize(), dupeCallback);
}

function dupeCallback(data, textStatus, xhr)
{
	if($('.group-basic-tab .status.duplicate').length == 0)
		$('.group-basic-tab').prepend(data);
	else
		$('.group-basic-tab .status.duplicate').replaceWith(data);
}