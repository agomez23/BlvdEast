<div class="panels-page" <?php if (!empty($css_id)) { print "id=\"$css_id\""; } ?>>
	<?php if($content['top']): ?>
		<div class="row">
			<div class="block grid12-12 grid16-16 top">
				<div class="inner">
					<?php echo $content['top']; ?>
				</div>
			</div>
			<div class="clear"></div>
		</div>
	<?php endif; ?>
	<?php if($content['left'] || $content['right'] || $content['left2']): ?>
		<div class="row">
			<?php if($content['left']): ?>
				<div class="block grid12-8 grid16-11 left">
					<div class="inner">
						<?php echo $content['left']; ?>
					</div>
				</div>
			<?php endif ?>
			<?php if($content['right']): ?>
				<div class="block grid12-4 grid16-5 right">
					<div class="inner">
						<?php echo $content['right']; ?>
					</div>
				</div>
			<?php endif ?>
			<?php if($content['left2']): ?>
				<div class="block grid12-8 grid16-11 left2" style="clear:left">
					<div class="inner">
						<?php echo $content['left2']; ?>
					</div>
				</div>
			<?php endif ?>
			<div class="clear"></div>
		</div>
	<?php endif; ?>
	<?php if($content['bottom']): ?>
		<div class="row">
			<div class="block grid12-12 grid16-16 bottom">
				<div class="inner">
					<?php echo $content['bottom']; ?>
				</div>
			</div>
			<div class="clear"></div>
		</div>
	<?php endif ?>
</div>