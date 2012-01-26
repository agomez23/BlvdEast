// $Id: autoresize.js,v 1.1.2.1.2.1 2009/03/15 12:39:07 george2 Exp $

/**
 * @file
 * js file for the vertical auto resizing of the textarea field.
 *
 * based (very heavily) upon elastic jquery plugin by Unwrongest(http://plugins.jquery.com/user/8094)
 * http://plugins.jquery.com/project/elastic
 * The only difference being that this elastic function accepts an animate param, if and when the arg is added to the original jquery function, this file will be replaced
 *
*/

(function($){ 
  $.fn.extend({  
    elastic: function(animate) { 
      var mimics = new Array('paddingTop','paddingRight','paddingBottom','paddingLeft','fontSize','lineHeight','fontFamily','width','fontWeight');	
      return this.each(function() { 
        if(this.type == 'textarea') {
          
          var textarea = $(this);
          var marginbottom = parseInt(textarea.css('lineHeight'))*2 || parseInt(textarea.css('fontSize'))*2;
          var minheight = parseInt(textarea.css('height')) || marginbottom;
          var goalheight = 0;
          var twin = null;
          
          function update() {
            if (!twin)
            {
              twin = $('<div />').css({'display': 'none','position': 'absolute'}).appendTo('body');
              $.each(mimics, function(){
                twin.css(this,textarea.css(this));
              });
            }
            
            var content = textarea.val().replace(/<|>/g, ' ').replace(/\n/g, '<br />').replace(/&/g,"&amp;").replace(/  /g,' &nbsp;')
            if (twin.text() != content)
            {			
              twin.html(content);
              goalheight = (twin.height()+marginbottom > minheight)?twin.height()+marginbottom:minheight;
              if(Math.abs(goalheight - textarea.height()) > 10)
                if (animate) {
                  textarea.animate({'height': goalheight},500);		
                }
                else {
                  textarea.css('height', goalheight);
                }
              }
            }
          textarea.css({overflow: 'hidden',display: 'block'}).bind('focus',function() { self.periodicalUpdater = window.setInterval(function() {update();}, 400); }).bind('blur', function() { clearInterval(self.periodicalUpdater); });
          update();
          
        }
      }); 
    } 
  }); 
})(jQuery);