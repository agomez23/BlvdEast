$(document).ready(function(){
  var fieldset = $("#better_domain_blocks_fieldset");
  var check = "#edit-visibility-";
  var checkbox = $(check + "124");
  var textarea = $("#edit-pages-wrapper");
  
  // hide our settings by default
  fieldset.hide();
  
  // if better domains is enabled hide the defualt textarea and show our settings
  if (checkbox.is(":checked")) {
    textarea.hide();
      fieldset.show();
  }
  
  $("input:radio").click(function() {
    var rightID = $(this).is(check + "124");
    var isChecked = $(this).is(":checked");
    if (rightID && isChecked) {
      textarea.hide();
      fieldset.show();
    }
    if ($(this).is(check + "0") || $(this).is(check + "1") || $(this).is(check + "2")) {
      fieldset.hide();
      textarea.show();
    }
  });
});
