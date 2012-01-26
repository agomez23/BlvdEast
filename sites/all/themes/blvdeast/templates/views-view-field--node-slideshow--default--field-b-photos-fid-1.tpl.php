<?php
// $Id: views-view-field.tpl.php,v 1.1 2008/05/16 22:22:32 merlinofchaos Exp $
 /**
  * This template is used to print a single field in a view. It is not
  * actually used in default Views, as this is registered as a theme
  * function which has better performance. For single overrides, the
  * template is perfectly okay.
  *
  * Variables available:
  * - $view: The view object
  * - $field: The field handler object that can process the input
  * - $row: The raw SQL result that can be used
  * - $output: The processed output that will normally be used.
  *
  * When fetching output from the $row, this construct should be used:
  * $data = $row->{$field->field_alias}
  *
  * The above will guarantee that you'll always get the correct data,
  * regardless of any changes in the aliasing that might happen if
  * the view is modified.
  */
/*
stdClass Object
(
    [nid] => 109
    [domain_access_gid] => 0
    [domain_access_domain_id] => 0
    [node_data_field_b_photos_field_b_photos_fid] => 642
    [node_data_field_b_photos_field_b_photos_list] => 1
    [node_data_field_b_photos_field_b_photos_data] => a:2:{s:5:"title";s:0:"";s:3:"alt";s:0:"";}
    [node_data_field_b_photos_delta] => 0
    [node_type] => town
    [node_vid] => 115
)
*/
?>

<?php
$domain = $_SERVER['HTTP_HOST'];
$domain_id = db_result(db_query("SELECT domain_id FROM {domain} WHERE subdomain LIKE '%s'", $domain));

$filepath = db_result(db_query("SELECT filepath FROM {files} WHERE fid = %d", $row->node_data_field_b_photos_field_b_photos_fid));
$data = unserialize($row->node_data_field_b_photos_field_b_photos_data);

$preset = ($domain_id > 0) ? 'property_detail_' . $domain_id : 'property_detail';
$preset_full = ($domain_id > 0) ? 'property_full_' . $domain_id : 'property_full';
$url_full = imagecache_create_url($preset_full, $filepath);

$slide_image = theme('imagecache', $preset, $filepath, $data['alt'], $data['title']);
?>
<a href="<?php print $url_full; ?>" title="<?php print $data['title']; ?>" class="colorbox" rel="gallery-<?php print $row->nid; ?>"><?php print $slide_image; ?></a>
