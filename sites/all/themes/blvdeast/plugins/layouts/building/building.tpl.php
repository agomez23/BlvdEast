<div class="panels-building" <?php if (!empty($css_id)) { print "id=\"$css_id\""; } ?>>
	<div class="row row-top">
		<div class="block grid12-8 grid16-11 panel-topleft">
			<div class="inner">
				<?php echo $content['topleft']; ?>
			</div>
		</div>
		<div class="block grid12-4 grid16-5 panel-topright">
			<div class="inner">
				<?php echo $content['topright']; ?>
			</div>
		</div>
		<div class="clear"></div>
	</div>
	<div class="row">
		<div class="block grid12-12 grid16-16 panel-center">
			<div class="inner">
				<?php echo $content['center']; ?>
			</div>
		</div>
		<div class="clear"></div>
	</div>
	<div class="row">
		<div class="block grid12-3 grid16-4 panel-left">
			<div class="inner">
				<?php echo $content['left']; ?>
			</div>
		</div>
		<div class="block grid12-6 grid16-8 panel-middle">
			<div class="inner">
				<?php echo $content['middle']; ?>
			</div>
		</div>
		<div class="block grid12-3 grid16-4 panel-right">
			<div class="inner">
				<?php echo $content['right']; ?>
			</div>
		</div>
		<div class="clear"></div>
	</div>
</div>