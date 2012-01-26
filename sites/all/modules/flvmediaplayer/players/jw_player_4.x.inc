<?php

  /**
   * @file this defines the forms and variables for the 4.x version of the player
   */

/**
 * defines the options for the player
 * @param array $config
 * @return array
 */
function flvmediaplayer_player_form($config) {
  // main display options
  $form['display_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Node display options')
  );
  $options = array(
    'display_body' => t('Display player in node body'),
    'body_embed' => t('Display embed code in node body'),
    'display_teaser' => t('Display player in node teaser'),
    'display_teaser_embed' => t('Display embed code in node teaser'),
  );
  $form['display_options']['flvmp_player_display'] = array(
    '#title' => t('Display options'),
    '#type' => 'checkboxes',
    '#options' => $options,
    '#description' => t('If you want to have the player automatically displayed on a node. If you are not using custom theming, this needs to be on.'),
    '#default_value' =>  $config['flvmp_player_display'] ? $config['flvmp_player_display']: array() ,
  );

  $form['profile_meta'] = array(
    '#type' => 'fieldset',
    '#title' => t('Profile settings'),
    '#collapsible' => true,
    '#collapsed' => true,
    '#description' => t('Specific settings for this player')
  );

  $form['profile_meta']['flvmp_name'] = array(
    '#title' => t('Profile name'),
    '#type' => 'textfield',
    '#default_value' => $config['flvmp_name'],
    '#description' => t('The unique name of your profile.'),
  );

  // define default player to use
  $form['profile_meta']['flvmp_player_path'] = array(
    '#title' => t('Player path'),
    '#type' => 'textfield',
    '#default_value' => $config['flvmp_player_path'] ? $config['flvmp_player_path'] : variable_get('flvmediaplayer_path', null),
     '#description' => t('URL to your flash player: <em>/path/@file</em>
      You can put the flash player into this module directory or in any other web accessible directory.
      If you use <em>http://PATH</em> the path to the player will absolute and will not change regardless of the
      current URL. You may need to adjust your crossdomain.xml file if you use an absolute URL.
      If you use <em>PATH/@file</em> flvmediaplayer will attach the current domain to the player.', array('@file' => 'mediaplayer.swf')),
  );

  // file settings
  $form['file'] = array(
    '#type' => 'fieldset',
    '#title' => t('File settings'),
    '#description' => t('If you select an external file source, these values will be overridden'),
    '#collapsible' => true,
    '#colapsed' => true,
  );

  // get all modules which implement our hook
  $options = flvmediaplayer_extend('flvmediaplayer_file_use', 'define');
  if (! empty($options)) {
    $form['file']['flvmp_file_location_hook'] = array(
      '#type' => 'select',
      '#title' => t('Media file'),
      '#options' => $options,
      '#default_value' => $config['flvmp_file_location_hook'] ? $config['flvmp_file_location_hook'] : array(),
      '#description' => t('Select a module to load the video for this player. A specified file location will override this choice.'),
    );
  }

  // file meta data
  $form['file']['meta'] = array(
    '#type' => 'fieldset',
    '#title' => t('Node meta data'),
    '#description' => t('Specific information for this node'),
    '#collapsible' => true,
    '#collapsed' => true,
  );

  // file location
  $form['file']['meta']['flashvar_file'] = array(
    '#title' => t('File location'),
    '#type' => 'textfield',
    '#default_value' => $config['flashvar_file'],
    '#description' => t('Location of the mediafile or playlist to play, leave blank to be overridden'),
  );

  $form['file']['meta']['flashvar_image'] = array(
    '#title' => t('Preview image'),
    '#type' => 'textfield',
    '#default_value' => $config['flashvar_image'],
    '#description' => t('Location of a preview image; shown in display and playlist, leave blank to be overrriden'),
  );

  $form['file']['meta']['flashvar_title'] = array(
    '#title' => t('File title'),
    '#type' => 'textfield',
    '#default_value' => $config['flashvar_title'],
    '#description' => t('Title of the video, shown in the display or playlist.'),
  );

  $form['file']['meta']['flashvar_author'] = array(
    '#title' => t('File author'),
    '#type' => 'textfield',
    '#default_value' => $config['flashvar_author'],
    '#description' => t('Author of the video, shown in the display or playlist'),
  );

  $form['file']['meta']['flashvar_description'] = array(
    '#title' => t('File description'),
    '#type' => 'textfield',
    '#default_value' => $config['flashvar_description'],
    '#description' => t('Text description of the file.'),
  );

  $form['file']['meta']['flashvar_duration'] = array(
    '#title' => t('File duration'),
    '#type' => 'textfield',
    '#default_value' => $config['flashvar_duration'],
    '#description' => t('Duration of the file in seconds'),
    '#size' => 4,
  );

  $form['file']['meta']['flashvar_file_url'] = array(
    '#title' => t('Link URL'),
    '#type' => 'textfield',
    '#default_value' => $config['flashvar_file_url'],
    '#description' => t('URL to an external page the display, controlbar and playlist can link to.'),
  );

  $form['file']['meta']['flashvar_file_start'] = array(
    '#title' => t('File start time'),
    '#type' => 'textfield',
    '#default_value' => $config['flashvar_file_start'] ? $config['file_start'] : 0 ,
    '#size' => '4',
    '#description' => t('Position in seconds where playback will start.'),
  );

  $form['file']['meta']['flashvar_file_type'] = array(
    '#title' => t('File type'),
    '#type' => 'selected',
    '#options' => array('none', 'sound', 'image', 'video', 'youtube', 'camera', 'http', 'rtmp'),
    '#default_value' => $config['flashvar_file_type'] ? $config['flashvar_file_type'] : 'none',
    '#description' => t('Type of file, can be sound, image, video, youtube, camera, http or rtmp. Use this to override auto-detection.'),
  );

  $form['file']['meta']['flashvar_file_captions'] = array(
    '#title' => t('Captions file'),
    '#type' => 'textfield',
    '#default_value' => $config['flashvar_file_captions'],
    '#description' => t('URL to a TT captions XML file.'),
  );

  // ----------------------------------------------------------
  // player formating
  $form['color'] = array(
    '#type' => 'fieldset',
    '#title' => t('Color settings'),
    '#collapsible' => true,
    '#collapsed' => true,
  );

  $form['color']['param_bgcolor'] = array(
    '#title' => t('Player background color'),
    '#type' => 'textfield',
    '#default_value' => $config['param_bgcolor'] ? $config['param_bgcolor'] : '#fffff',
    '#description' => t('Background color of the controlbar and playlist. Default is #ffffff'),
    '#size' => 7,
  );

  $form['color']['flashvar_background'] = array(
    '#title' => t('Background color'),
    '#type' => 'textfield',
    '#default_value' => $config['flashvar_background'] ? $config['flashvar_background'] : '#ffffff',
    '#description' => t('Background color of the controlbar and playlist. Default is #ffffff'),
    '#size' => 7,
  );

  $form['color']['flashvar_frontcolor'] = array(
    '#title' => t('Front color'),
    '#type' => 'textfield',
    '#default_value' => $config['flashvar_frontcolor'] ? $config['flashvar_frontcolor'] : '#000000',
    '#description' => t('Color of all icons and texts in the controlbar and playlist. Default is #000000'),
    '#size' => 7,
  );

  $form['color']['flashvar_lightcolor'] = array(
    '#title' => t('Light color'),
    '#type' => 'textfield',
    '#default_value' => $config['flashvar_lightcolor'] ? $config['flashvar_lightcolor'] : '#000000',
    '#description' => t('Color of an icon or text when you rollover it with the mouse. Default is #000000'),
    '#size' => 7,
  );

  $form['color']['flashvar_screencolor'] = array(
    '#title' => t('Screen color'),
    '#type' => 'textfield',
    '#default_value' => $config['flashvar_screencolor'] ? $config['flashvar_screencolor'] : '#000000',
    '#description' => t('Background color of the display. Default is #000000'),
    '#size' => 7,
  );

  // ----------------------------------------------------------
  // layout
  $form['layout'] = array(
    '#type' => 'fieldset',
    '#title' => t('Player Layout settings'),
    '#collapsible' => true,
    '#collapsed' => true,
  );

  $form['layout']['param_height'] = array(
    '#title' => t('Player height'),
    '#type' => 'textfield',
    '#default_value' => $config['param_height'] ? $config['param_height'] : 280,
    '#description' => t('Height of the player, supports % for flexible layout'),
    '#size' => 4,
  );

  $form['layout']['param_width'] = array(
    '#title' => t('Player width'),
    '#type' => 'textfield',
    '#default_value' => $config['param_width'] ? $config['param_width'] : 400,
    '#description' => t('Width of the player, supports % for flexible layout'),
    '#size' => 4,
  );

  $form['layout']['flashvar_height'] = array(
    '#title' => t('Display height'),
    '#type' => 'textfield',
    '#default_value' => $config['flashvar_height'] ? $config['flashvar_height'] : 280,
    '#description' => t('Height of the display (not the entire player!) in pixels. Default is 280.'),
    '#size' => 4,
  );

  $form['layout']['flashvar_width'] = array(
    '#title' => t('Display width'),
    '#type' => 'textfield',
    '#default_value' => $config['flashvar_width'] ? $config['flashvar_width'] : 400,
    '#description' => t('Width of the display (not the entire player!) in pixels. Default is 400'),
    '#size' => 4,
  );

  $form['layout']['flashvar_controlbar'] = array(
    '#title' => t('Controlbar position'),
    '#type' => 'select',
    '#options' => array('bottom', 'over', 'none'),
    '#default_value' => $config['flashvar_controlbar'],
    '#description' => t('Position of the controlbar. Can be set to bottom, over and none.'),
  );

  $form['layout']['flashvar_controlbarsize'] = array(
    '#title' => t('Controlbar size'),
    '#type' => 'textfield',
    '#default_value' => $config['flashvar_controlbarsize'],
    '#description' => t('Height of the controlbar in pixels.'),
    '#size' => 4,
  );

  $form['layout']['flashvar_logo'] = array(
    '#title' => t('Logo'),
    '#type' => 'textfield',
    '#default_value' => $config['flashvar_logo'],
    '#description' => t('Location of an external jpg,png or gif image to show in the display.'),
  );

  $form['layout']['flashvar_playlist'] = array(
    '#title' => t('Playlist position'),
    '#type' => 'select',
    '#options' => array('bottom' => 'bottom', 'over' => 'over', 'right' => 'right', 'none' => 'none'),
    '#default_value' => $config['flashvar_playlist'] ? $config['flashvar_playlist'] : 'none',
    '#description' => t('Position of the playlist. Can be set to bottom, over, right or none.'),
  );

  $form['layout']['flashvar_playlistsize'] = array(
    '#title' => t('Playlist size'),
    '#type' => 'textfield',
    '#default_value' => $config['flashvar_playlistsize'],
    '#description' => t('Size of the playlist. When below or above, this refers to the height, when right, this refers to the width of the playlist.'),
    '#size' => 4,
  );

  $form['layout']['flashvar_skin'] = array(
    '#title' => t('Skin'),
    '#type' => 'textfield',
    '#default_value' => $config['flashvar_skin'],
    '#description' => t('Location of a SWF file with the player graphics.'),
  );


  // ----------------------------------------------------------

  $form['playback'] = array(
    '#type' => 'fieldset',
    '#title' => t('Playback settings'),
    '#collapsible' => true,
    '#collapsed' => true,
  );

  $form['playback']['param_allowfullscreen'] = array(
    '#title' => t('Allow fullscreen'),
    '#type' => 'checkbox',
    '#return_value' => 'true',
    '#default_value' => $config['param_allowfullscreen'],
    '#description' => t('Enable fullscreen display'),
  );

  $form['playback']['flashvar_autostart'] = array(
    '#title' => t('Autostart'),
    '#type' => 'checkbox',
    '#default_value' => $config['flashvar_autostart'],
    '#description' => t('Automatically start the player on load.'),
  );

  $form['playback']['flvmp_bufferlength'] = array(
    '#title' => t('Buffer length'),
    '#type' => 'textfield',
    '#default_value' => $config['flashvar_bufferlength'] ? $config['flashvar_bufferlength'] : 0.1,
    '#description' => t('Number of seconds of the file that has to be loaded before starting.'),
    '#size' => 3,
  );

  $form['playback']['flvmp_displayclick'] = array(
    '#title' => t('Display click'),
    '#type' => 'select',
    '#options' => array('play', 'link', 'fullscreen', 'none', 'mute', 'next'),
    '#default_value' => $config['flashvar_displayclick'] ? $config['flashvar_displayclick'] : 'play',
    '#description' => t('What to do when one clicks the display. Can be play, link, fullscreen, none, mute, next.'),
  );

  $form['playback']['flvmp_item'] = array(
    '#title' => t('Playlist start item'),
    '#type' => 'textfield',
    '#default_value' => $config['flashvar_item'] ? $config['flashvar_item'] : 0,
    '#description' => t('Playlist item that should start to play. Use this to set a specific start-item.'),
    '#size' => 3,
  );

  $form['playback']['flvmp_mute'] = array(
    '#title' => t('Mute'),
    '#type' => 'checkbox',
    '#default_value' => $config['flashvar_mute'],
    '#description' => t('Mute all sounds on startup. Is saved as cookie.'),
  );

  // quality is a true false value
  $form['playback']['param_quality'] = array(
    '#title' => t('Quality'),
    '#type' => 'checkbox',
    '#return_value' => 'true',
    '#default_value' => $config['param_quality'] ? $config['param_quality'] : true,
    '#description' => t('Enables high-quality playback. This sets the smoothing of videos on/off, the deblocking of videos on/off and the dimensions of the camera small/large. Is saved as cookie.'),
  );

  $form['playback']['flashvar_repeat'] = array(
    '#title' => t('Repeat'),
    '#type' => 'select',
    '#options' => array('none' => t('no'), 'list' => t('Play all items on the playlist'), 'always' => t('Repeat'), 'single' => t('Repeate selected item')),
    '#default_value' => $config['flashvar_repeat'] ? $config['flashvar_repeat'] : false,
    '#description' => t('Continously play the song/video/playlist.'),
  );

  $form['playback']['flashvar_shuffle'] = array(
    '#title' => t('Shuffle'),
    '#type' => 'checkbox',
    '#default_value' => $config['flashvar_shuffle'],
    '#description' => t('Shuffle playback of playlistitems'),
  );

  $form['playback']['flashvar_stretching'] = array(
    '#title' => t('Streching'),
    '#type' => 'select',
    '#options' => array('none' => t('No stretching'), 'exactfit' => t('Disproportionate stretch'), 'uniform' => t('Stretch with black borders'), 'fill' => t('Uniform, but completely fill the display')),
    '#default_value' => $config['flashvar_stretching'] ? $config['flashvar_stretching']  : 'uniform',
    '#description' => t('Defines how to resize images in the display.'),
  );


  // build the volume array
  $volume = array(0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100);
  $volume = array_combine (array_values($volume), $volume);

  $form['playback']['flashvar_volume'] = array(
    '#title' => t('Volume'),
    '#type' => 'select',
    '#options' => $volume,
    '#default_value' => $config['flashvar_volume'] ? $config['flashvar_volume'] : 90 ,
    '#description' => t('Startup volume of the player. Is saved as cookie.'),
  );

  $form['playback' ]['param_allowscriptaccess'] = array(
    '#title' => t('Enable javascript'),
    '#type' => 'checkbox',
    '#return_value' => 'always',
    '#default_value' => $config['param_allowscriptaccess'] ? $config['param_allowscriptaccess'] : true,
    '#description' => t('You can allow javascript access to the player. Default is on. You need this for recommended content '),
  );

  // ----------------------------------------------------------

  $form['external'] = array(
    '#type' => 'fieldset',
    '#title' => t('External settings'),
    '#collapsible' => true,
    '#collapsed' => true,
  );

  // use external config file
  $form['external']['flvmp_external_config' ] = array(
    '#title' => t('External Config'),
    '#type' => 'checkbox',
    '#default_value' => $config['flvmp_external_config'],
    '#description' => t('An external configuration file allows you to adjust the paremters of a player after it has been embeded. This is good if you want to change the appearnce of off site players. This does add server overhead, however the configuration files are cached by Drupal. Default is off. Value applies to the embedded player as well. This file is dynamically generated at <em>flvmediaplayer/node/X</em>'),
  );

  // use external config file
  $form['external']['flashvars_plugins' ] = array(
    '#title' => t('Plugins'),
    '#type' => 'textarea',
    '#rows' => 2,
    '#default_value' => $config['flashvars_plugins'],
    '#description' => t('Add additional plugins to use with your player.
      Format should be <em>http://yoursite.com/plugin1,http://yoursite.com/plugin2,http://yoursite.com/plugin3</em>.
      Please note, you can cause issues mixing SSL and non-SSL content with plugin URLs.'),
  );

  // use additional flash_vars
  $form['external']['flashvars_flashvars' ] = array(
    '#title' => t('Additional flash vars'),
    '#type' => 'textarea',
    '#rows' => 2,
    '#default_value' => $config['flashvars_flashvars'],
    '#description' => t('Additional data to load into the player. Enter each variables and value on a new line: <em>file=foo.text</em>'),
  );

  $form['external']['flvmp_external_abouttext'] = array(
    '#title' => t('About text'),
    '#type' => 'textfield',
    '#default_value' => $config['flvmp_external_abouttext'],
    '#description' => t('Text to show in the rightclick menu. Please do not change this if you do not have a commercial license! When undefined it shows the player version.'),
  );

  $form['external']['flvmp_external_aboutlink'] = array(
    '#title' => t('About link'),
    '#type' => 'textfield',
    '#default_value' => $config['flvmp_external_aboutlink'],
    '#description' => t('URL to link to from the rightclick menu. Do not change this if you do not have a commercial license!'),
  );

  $form['external']['flvmp_external_client'] = array(
    '#title' => t('Client'),
    '#type' => 'textfield',
    '#default_value' => $config['flvmp_external_client'],
    '#description' => t('Version and platform of the Flash client plugin. Useful to check for e.g. MP4 playback or fullscreen capabilities.'),
  );

  $form['external']['flvmp_external_id'] = array(
    '#title' => t('Player ID'),
    '#type' => 'textfield',
    '#default_value' => $config['flvmp_external_id'],
    '#description' => t('ID of the player within the javascript DOM. Useful for javascript interaction.'),
  );

  $form['external']['flvmp_external_linktarget'] = array(
    '#title' => t('Link target'),
    '#type' => 'textfield',
    '#default_value' => $config['flvmp_external_linktarget'],
    '#description' => t("Browserframe where the links from display are opened in. Some possibilities are '_self' (same frame) , '_blank' (new browserwindow) or 'none' (links are ignored in the player, so javascript can handle it)."),
  );

  $form['external']['flvmp_external_streamer'] = array(
    '#title' => t('Streamer'),
    '#type' => 'textfield',
    '#default_value' => $config['flvmp_external_streamer'],
    '#description' => t('Location of a server to use for streaming. Can be an RTMP application or external PHP/ASP file to use for HTTP streaming. If set to lighttpd, the player presumes a Lighttpd server is used to stream videos.'),
  );

  $form['external']['flvmp_external_tracecall'] = array(
    '#title' => t('Tracecall'),
    '#type' => 'textfield',
    '#default_value' => $config['flvmp_external_tracecall'],
    '#description' => t('Name of a javascript function that can be used for tracing the player activity. All events from the view, model and controller are sent there.'),
  );

  $form['external']['flvmp_external_version'] = array(
    '#title' => t('Version'),
    '#type' => 'textfield',
    '#default_value' => $config['flvmp_external_version'],
    '#description' => t('Exact major release, minor release and revision number of the player. sent to javascript with every call.'),
  );

  // --------------------------------------------
  //  Embed Options

  $form['embed'] = array(
    '#type' => 'fieldset',
    '#title' => t('Embedded player options'),
    '#collapsible' => true,
    '#collapsed' => true,
  );

  // use a specific profile with an embed
  $form['embed']['flvmp_embed_profile' ] = array(
    '#title' => t('Embeded player profile'),
    '#type' => 'select',
    '#options' => array_merge(array($config['name'] => t('This profile')), flvmediaplayer_profile_get_configs('name')),
    '#default_value' => $config['flvmp_embed_profile'] ? $config['flvmp_embed_profile'] : $config['name'],
    '#description' => t('Use a specific profile with the embed player.'),
  );

  // -----------------------------------------------
  // No flash

  $form['no_flash'] = array(
    '#title' => t('No Flash support'),
    '#type' => 'fieldset',
    '#collapsible' => true,
    '#collapsed' => $config['flvmp_noflash_text'] ? false : true,
  );
  // get all the no_flash items
  $options = flvmediaplayer_extend('flvmediaplayer_no_flash', 'define');
  if (! empty($options)) {
    $options['none'] = t('None');
    $form['no_flash']['flvmp_flash_noflash_hook'] = array(
      '#title' => t('No flash display'),
      '#type' => 'select',
      '#options' => $options,
      '#default_value' => $config['flvmp_flash_noflash_hook'] ? $config['flvmp_flash_noflash_hook'] : 'none' ,
      '#description' => t('Select an item which display no flash content. This will be overriden by entered text.'),
    );
  }
  $form['no_flash']['flvmp_noflash_text_filter']['flvmp_noflash_text'] = array(
    '#title' => t('No flash display'),
    '#type' => 'textarea',
    '#default_value' => $config['flvmp_noflash_text'],
    '#description' => t('Enter text to be displayed if the browser does not have flash enabled, ie: <em>Sorry, you need to install flash to see this content.</em>'),
  );
  // add a filter option
  $form['no_flash']['flvmp_noflash_text_filter']['filter'] = filter_form($config['flvmp_noflash_text_filter']);

  return $form;
}