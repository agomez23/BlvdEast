<?php 
// $Id: mimemail-message.tpl.php,v 1.1 2010/04/21 01:07:18 jerdavis Exp $

/**
 * @file mimemail-message.tpl.php
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
    <div id="center">
      <div id="main">
        <?php print $body ?>
      </div>
    </div>
  </body>
</html>
