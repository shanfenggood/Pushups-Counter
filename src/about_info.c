#include "about_info.h"

static TextLayer* info_layer;
static void window_load(Window* w)
{
	Layer* rootLayer = window_get_root_layer(w);
	GRect rect = layer_get_frame(rootLayer);
	info_layer = text_layer_create(GRect(10,10,rect.size.w-20,rect.size.h-20)); 
	text_layer_set_font(info_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text(info_layer, "Developed By:\r\nxieshanfeng\r\n\r\nshanfenggood@gmail.com \r\n\r\n2014-4");
	text_layer_set_text_alignment(info_layer, GTextAlignmentCenter);
	text_layer_set_background_color(info_layer,GColorBlack);
	text_layer_set_text_color(info_layer,GColorWhite);
	layer_add_child(rootLayer,text_layer_get_layer(info_layer));


}
static void window_unload(Window* w)
{
	text_layer_destroy(info_layer);	
}


Window* aboutinfo_window_create()
{
	Window* infoW = window_create();	
	window_set_background_color(infoW,GColorBlack);
	window_set_window_handlers(infoW, (WindowHandlers) {
    	.load = window_load,
    	.unload = window_unload,
  });
	return infoW;
}


void aboutinfo_window_destroy(Window* w)
{
	window_destroy(w);
}
