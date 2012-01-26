// $Id: README.txt,v 1.6 2010/08/05 08:54:08 jide Exp $

Installation
============
1. Download and extract Scaffold from http://github.com/sunny/csscaffold. If you
   have the libraries module installed, put the "scaffold" subfolder in 
   sites/all/libraries, otherwise put it directly in the scaffold module folder.
   Be aware that the Scaffold repository location is likely to change.

2. Install & enable the module.

Configuration
=============
1. Go to Administer > Site configuration > Scaffold (admin/settings/scaffold).
   You need the "administer site configuration" permission to be able to change
   these settings.

2. If the theme supports it, you can configure additionnal settings for the
   theme by going to Administer > Site building > Themes > Configure
   (admin/build/themes/settings) and go to the tab of your theme.

3. Drupal aggregation will work seamlessly

Developers
==========
1. To add a Scaffold stylesheet to your theme, add this to your .info file :

   scaffold[all][] = style.css
   scaffold[print][] = print.css

2. Scaffold module also provides an API function similar to drupal_add_css() to
   add stylesheets from your modules :

   scaffold_add_css($path, $type, $media, $preprocess)

3. You may want to call a stylesheet directly through an url, e.g. to use
   conditional stylesheets for IE. To do so, use the scaffold url :

   scaffold/'. path_to_theme() .'/style.css';

   For a constional stylesheet, you could use hook_preprocess_page() and alter
   the 'styles' variables :

   $variables['styles'] .= '<!--[if lte IE 7]>
     <link type="text/css" rel="stylesheet" media="all" href="'. base_path() . 'scaffold/' . path_to_theme() .'/ie-fix.css" />
   <![endif]-->

4. You can use Scaffold with a base theme, but be aware that when you override
   mixins set in the base theme form the subtheme, rules that use the mixin in
   the base theme will keep using the original mixin. E.g :
   
   In your base theme stylesheet :
   
   =my-mixin {
     color: #FF0000;
   }

   .my-rule {
     +my-mixin;
   }

   In your subtheme stylesheet :

   =my-mixin {
     color: #00FF00;
   }

   The overriden mixin won't affect .my-rule. If you need such feature, you can
   work around this limitation by using a @include statement :
   include the needed stylesheets of your base theme using @include :

   @include '../my_base_theme/style.css';

5. You can also add configurable constants to the theme settings by using the
   provided hooks :

   hook_scaffold_constants_defaults()

   which should return a structured array :

   return array(
     'font_size' => '13px',
     'base_color' => '#666666',
   );
   
   hook_scaffold_constants_form()
   
   which should return a FAPI form using constant names as the keys of the form :
   
   return array(
     'font_size' => array(
       '#title' => t('Font size'),
       '#type' => 'textfield',
     ),
     'base_color' => array(
       '#title' => t('Base color'),
       '#type' => 'textfield',
     ),
   );
   
   A new form will be added to admin/build/themes/settings/[theme]. These values
   will then be used as constants by Scaffold. Be aware that already defined
   constants in your stylesheet will override those set in the hook.

6. Currently, the Scaffold classes are used directly in the Drupal environment,
   which can potentially clash with other existing classes when calling the
   Scaffold class. No problems have been found or reported so far, but the
   library may be sandboxed in a minimal bootstrap in future versions.
