<div class="panels-search" <?php if (!empty($css_id)) { print "id=\"$css_id\""; } ?>>
	<div class="row">
		<div class="block grid12-12 grid16-16 top">
			<div class="inner">
				<?php echo $content['top']; ?>
			</div>
		</div>
		<div class="clear"></div>
	</div>
	<div class="row">
		<div class="block grid12-9 grid16-12 left">
			<div class="inner">
				<?php echo $content['left']; ?>
			</div>
		</div>
		<div class="block grid12-3 grid16-4 right">
			<div class="inner">
				<?php echo $content['right']; ?>
			</div>
		</div>
		<div class="block grid12-9 grid16-12 left2" style="clear:left">
			<div class="inner">
				<?php echo $content['left2']; ?>
			</div>
		</div>
		<div class="clear"></div>
	</div>
	<div class="row">
		<div class="block grid12-12 grid16-16 bottom">
			<div class="inner">
				<?php echo $content['bottom']; ?>
			</div>
		</div>
		<div class="clear"></div>
	</div>
</div>