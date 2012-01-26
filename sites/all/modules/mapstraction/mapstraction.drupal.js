// $Id: mapstraction.drupal.js,v 1.1.2.5 2009/02/23 23:08:51 diggersf Exp $
$(document).ready(function() {
  Drupal.mapstraction = [];
  $(Drupal.settings.mapstraction).each(function(index) {
    var id = 'mapstraction-' + this.viewName + '-' + this.currentDisplay;
    Drupal.mapstraction[id] = new Mapstraction(id, this.apiName);
    
    // Set start point
    var startPoint = new LatLonPoint(Number(this.initialPoint.latitude), Number(this.initialPoint.longitude));
    Drupal.mapstraction[id].setCenterAndZoom(startPoint, Number(this.initialPoint.zoom));
    
    // Set up controls
	if(this.apiName == "google" && this.controls.zoom == "large" && this.controls.pan)
	{
		this.controls.zoom = this.controls.pan = false;
		gmap = Drupal.mapstraction[id].getMap();
		gmap.addControl(new GLargeMapControl3D());
	}
	Drupal.mapstraction[id].addControls(this.controls);
	
	if(this.scroll_zoom)
		Drupal.mapstraction[id].enableScrollWheelZoom();
    
    // Set up markers & info bubbles
    $(this.markers).each(function(index) {
      marker = new Marker(new LatLonPoint(Number(this.lat), Number(this.lon)));
      marker.setInfoBubble(this.title);
      Drupal.mapstraction[id].addMarker(marker);
	  Drupal.mapstraction[id].autoCenterAndZoom();
    });
  });
});
