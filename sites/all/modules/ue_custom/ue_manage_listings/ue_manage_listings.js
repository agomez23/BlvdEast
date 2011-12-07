function quickEdit(nodeID)
{
	$("#editrow" + nodeID + " form").show().find("input").removeAttr("disabled");
	return false;
}
function saveEdit(nodeID)
{
	dateRe = /^(0[1-9]|1[012])[- /.](0[1-9]|[12][0-9]|3[01])[- /.]\d\d$/;
	dateVal = $("input[name=field_date]", "#editrow" + nodeID).val();
	
	rentRe = /^[\d]{3,}$/;
	rentVal = $("input[name=field_price]", "#editrow" + nodeID).val();
	
	if(dateVal != "" && !dateRe.exec(dateVal))
	{
		alert("Please enter a valid date.");
		return false;
	}
	
	if(!rentRe.exec(rentVal))
	{
		alert("Please enter a valid rent amount.");
		return false;
	}
	
	$.ajax({
			url: "manage-listings/quickedit/" + nodeID,
			data: $("#editrow" + nodeID + " form").serialize(),
			type: "post",
			dataType: "json",
			success: function(data) {
				updateRow(nodeID, data);
				$("#editrow" + nodeID + " form").hide();
				//$("#editrow" + nodeID + " form").slideUp("normal", function() {
					$("#editrow" + nodeID + " .loading").hide();
					$("#editrow" + nodeID + " .buttons").show();
					$("#editrow" + nodeID + " input").attr("disabled", "disabled");
				//});
			},
			error: function() { $("#editrow" + nodeID + " .loading").hide(); $("#editrow" + nodeID + " .buttons").show(); alert("There was an error saving the listing."); }
			});
	
	$("#editrow" + nodeID + " .buttons").hide();
	$("#editrow" + nodeID + " .loading").show();
}

function updateRow(nodeID, data)
{
	row = $("#listingrow" + nodeID);
	$(".price", row).html(data.field_price + (data.field_neteffective == "Net Effective" ? " Net Effective" : ""));
	$(".date", row).html(data.field_date);
	$(".offmkt", row).html(data.field_unit_availability == "On the Market" ? "" : "&nbsp;(Off the Market)");
	$(".updated", row).html(data.changed);
}

function cancelEdit(nodeID)
{
	$("#editrow" + nodeID + " form").hide()
	$("#editrow" + nodeID + " form").find("input").attr("disabled", "disabled");
}

function bulkSubmit()
{
	if($(this.op).val() == "del" && !confirm("Are you sure you want to delete these listings? This cannot be undone!")) return false;
	
	$(this).find("input.bulk").remove();
	$(".view-getMyProperties .bulk:checked").clone().attr("type", "hidden").appendTo(this);
}

function checkAll(nid)
{
	$("#prop" + nid + " tr:visible .bulk:checkbox").attr("checked", "checked");
	return false;
}

function checkActive(nid)
{
	$("#prop" + nid + " .active .bulk:checkbox").attr("checked", "checked");
	return false;
}

function checkNone(nid)
{
	$("#prop" + nid + " .bulk:checkbox").removeAttr("checked");
	return false;
}

function showCollapsed(propnid)
{
	$("#prop" + propnid + " .inactive").css("display", "");
	return false;
}

$(function() { $("#bulk").submit(bulkSubmit); });