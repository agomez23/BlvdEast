<?php // $Id: mimemail.tpl.php,v 1.2 2009/08/10 17:53:39 vauxia Exp $

/**
 * @file mimemail.tpl.php
 */
?>
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
		<?php if ($css): ?>
		<style type="text/css">
			<!--
			<?php print $css ?>
			-->
		</style>
		<?php endif; ?>
	</head>
	<body id="mimemail-body">
		<?php print $body ?>
		<br>
		<br>
		<div class="mail-footer">
			<?php echo l(theme('image', drupal_get_path('theme', 'blvdeast') . '/images/blvdeast_footer-logo-white.gif'), '<front>', array('html' => true)); ?><br>
			<b>Blvd East Rental Group</b><br>
			REMAX Villa Realtors<br>
			201-868-BLVD (2583)<br>
			<?php print l('www.blvdeastrentals.com', '<front>') ?>
		</div>
	</body>
</html>
