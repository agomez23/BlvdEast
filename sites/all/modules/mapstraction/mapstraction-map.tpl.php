<?php
// $Id: mapstraction-map.tpl.php,v 1.1.2.1 2010/02/11 19:15:14 loubabe Exp $
/* @file
 * mapstraction.map.tpl.php
 *
 * this file is all about getting theme preprocess functionality.
 *
 * script code
 *   $api_script
 * div attributes
 *   $width
 *   $height
 *   $map_id
 */
?>
<?php print $api_script; ?>
<div class="mapstraction" id="<?php print $map_id; ?>" style="height: <?php print $height; ?>px;width: <?php print $width; ?>px;"></div>
