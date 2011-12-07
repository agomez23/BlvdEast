<!DOCTYPE html>
<html lang="<?php print $language->language; ?>">
	<head>
        <title><?php print $head_title; ?></title>
		<meta property="og:image" content="<?php print file_create_url(file_directory_path() . '/imagecache/homepage_feature_sm/NKN_0594_edited.jpg'); ?>" />
		<?php print $head; ?>
		
		<?php print $styles; ?>
		<?php print $setting_styles; ?>
		<!--[if lte IE 8]>
			<link type="text/css" rel="stylesheet" media="all" href="<?php print file_create_url(drupal_get_path('theme', 'blvdeast') . '/css/ie.css') ?>" />
		<![endif]-->
        <!--[if IE 8]>
		<?php print $ie8_styles; ?>
		<![endif]-->
		<!--[if IE 7]>
		<?php print $ie7_styles; ?>
		<![endif]-->
		<!--[if lte IE 6]>
		<?php print $ie6_styles; ?>
		<![endif]-->
        
		<?php print $local_styles; ?>
		
		<?php print $scripts; ?>
	</head>
	<body id="<?php print $body_id; ?>" class="<?php print $body_classes; ?>">
        <header id="header-bar-group" class="header-bar-group clearfix">
			<div class="row <?php print $grid_width; ?>">
				<div class="header-left">
					<?php print theme('grid_block', $search_box, 'search-box'); ?>
					
					<?php if ($logo || $site_name || $site_slogan): ?>
						<div id="header-site-info" class="header-site-info block">
							<div id="header-site-info-inner" class="header-site-info-inner inner">
							<?php if ($logo): ?>
								<div id="logo">
									<a href="<?php print check_url($front_page); ?>" title="<?php print t('Home'); ?>"><img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>" /></a>
								</div>
							<?php endif; ?>
							<?php if ($site_name || $site_slogan): ?>
								<div id="site-name-wrapper" class="clearfix">
								<?php if ($site_name): ?>
									<span id="site-name"><a href="<?php print check_url($front_page); ?>" title="<?php print t('Home'); ?>"><?php print $site_name; ?></a></span>
								<?php endif; ?>
								</div><!-- /site-name-wrapper -->
							<?php endif; ?>
							</div><!-- /header-site-info-inner -->
						</div><!-- /header-site-info -->
					<?php endif; ?>
				</div>
				<?php if($header_bar_middle):// || $site_slogan): ?>
					<div class="header-middle">
						<?php //if ($site_slogan): print '<span id="slogan">&ldquo;Renting on Blvd East made easy&rdquo;</span>'; endif; ?>
						<?php if ($header_bar_middle): ?>
							<?php print $header_bar_middle; ?>
						<?php endif; ?>
					</div>
				<?php endif; ?>
				<div class="header-right <?=($user->uid)?'auth':'anon'?>">
					
					<?php //print theme('grid_block', $secondary_links_tree, 'secondary-menu'); ?>
					<?php //print theme('grid_block', theme('links', $secondary_links), 'secondary-menu'); ?>
					<?php print $header_bar; ?>
					<?php /*
					<div class="header-share" style="display:none">
						<div class="addthis_toolbox at_medium addthis_share_btn at_orange">
                            <a href="http://addthis.com/bookmark.php" class="addthis_button_compact at300m"><span>Share</span></a>
                        </div>  
					</div>
					*/ ?>
				</div>
				<div><?php print theme('grid_block', $primary_links_tree, 'primary-menu'); ?></div>
				<div class="clear"></div>
			</div>
		</header>

    <div id="main" class="main row clearfix <?php print $grid_width; ?>">
      <?php if ($sidebar_first): ?>
      <div id="sidebar-first" class="sidebar-first row nested <?php print $sidebar_first_width; ?>">
        <?php print $sidebar_first; ?>
      </div><!-- /sidebar-first -->
      <?php endif; ?>

      <div id="main-group" class="main-group row nested <?php print $main_group_width; ?>">
        <div id="content-group" class="content-group row nested <?php print $content_group_width; ?>">
          <?php print theme('grid_block', $breadcrumb, 'breadcrumbs'); ?>
          <?php print theme('grid_block', $tabs, 'content-tabs'); ?>
			<?php if ($tools): ?>
				<div id="tools" class="row grid16-5 right clear_right"><?php print $tools; ?></div>
			<?php endif; ?>
          <?php print theme('grid_block', $help, 'content-help'); ?>
          <?php print theme('grid_block', $messages, 'content-messages'); ?>
          <a name="main-content-area" id="main-content-area"></a>

          <div id="content-inner" class="content-inner block">
              <?php if ($title): ?>
              <h1 class="title"><?php print $title; ?></h1>
              <?php endif; ?>
    
              <?php if ($content): ?>
                <?php print $content; ?>
                <?php print $feed_icons; ?>
              <?php endif; ?>
          </div><!-- /content-inner -->
        </div><!-- /content-group -->

        <?php if ($sidebar_last): ?>
        <div id="sidebar-last" class="sidebar-last row nested <?php print $sidebar_last_width; ?>">
          <?php print $sidebar_last; ?>
        </div><!-- /sidebar-last -->
        <?php endif; ?>
      </div><!-- /main-group -->
    </div><!-- /main -->

    <?php if ($footer): ?>
    <div id="footer" class="footer row <?php print $grid_width; ?>">
      <?php print $footer; ?>
    </div><!-- /footer -->
    <?php endif; ?>

    <?php if ($footer_message): ?>
    <div id="footer-message" class="footer-message row <?php print $grid_width; ?>">
      <?php print theme('grid_block', $footer_message, 'footer-message-text'); ?>
    </div><!-- /footer-message -->
    <?php endif; ?>
  <?php print $closure; ?>
</body>
</html>
