#include <pebble.h>
#include "pushups.h"

#define SETUP_TIMER_INTERVAL  1000
#define PUSHUPS_TIMER_INTERVAL  180
#define PRECISION  70

static AppTimer *setup_timer;
static AppTimer *pushups_timer;
static AccelData preAccel;

static TextLayer* count_layer;  
static TextLayer* title_layer;  
static BitmapLayer *image_layer;
static GBitmap *image;


static int pushups_counter;
static int setup_time=3;

static int x_pre_flag;
static int y_pre_flag;
static int z_pre_flag;

static char count_str[20] = "0";

static void init_ready()
{
	snprintf(count_str,sizeof(count_str), "%d", setup_time);
	text_layer_set_text(count_layer,count_str);
	text_layer_set_text(title_layer,"Ready!");
	if(image)
	{
		gbitmap_destroy(image);	
	}
	image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PUSHUPS_READY);
	bitmap_layer_set_bitmap(image_layer, image);
}


static void init_pushup()
{
	text_layer_set_text(title_layer,"Fighting!");
	text_layer_set_text(count_layer,"0");
	if(image)
	{
		gbitmap_destroy(image);	
	}
 	image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PUSHUPS_ING);
	bitmap_layer_set_bitmap(image_layer, image);
}

static void init_end()
{
	
	text_layer_set_text(title_layer,"congratulations!");
	text_layer_set_text(count_layer,"Done!");
	if(image)
	{
		gbitmap_destroy(image);	
	}
	image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PUSHUPS_DONE);
	bitmap_layer_set_bitmap(image_layer, image);
}

static void pushups_timer_callback(void *data) 
{
	AccelData accel = (AccelData) { .x = 0, .y = 0, .z = 0 };
  	accel_service_peek(&accel);
	
	
	int dfx =(accel.x - preAccel.x)/PRECISION;
	int dfy =(accel.y - preAccel.y)/PRECISION;
	int dfz =(accel.z - preAccel.z)/PRECISION;
		

	
	int new_x_flag = dfx==0?x_pre_flag:(dfx>0?1:-1); 	
	int new_z_flag = dfz==0?z_pre_flag:(dfz>0?1:-1); 	
	int new_y_flag = dfy==0?y_pre_flag:(dfy>0?1:-1); 	

	//if((new_x_flag!=x_pre_flag)&&
	//	if((new_z_flag!=z_pre_flag)&&
	if((new_y_flag != y_pre_flag))
	{
	
		APP_LOG(APP_LOG_LEVEL_DEBUG,"%d,%d,%d,%d,%d,%d", preAccel.x,preAccel.y,preAccel.z,accel.x,accel.y,accel.z);
		APP_LOG(APP_LOG_LEVEL_DEBUG,"%d,%d,%d,%d,%d,%d,%d",pushups_counter
			,x_pre_flag,y_pre_flag,z_pre_flag
			,new_x_flag,new_y_flag,new_z_flag);

		++pushups_counter;
		if(pushups_counter/2 == persist_read_int(GOAL_KEY))
		{
			vibes_long_pulse();
			//save tatal
			persist_write_int(TOTAL_PUSHUPS_KEY,
				persist_read_int(TOTAL_PUSHUPS_KEY) + pushups_counter/2); 
			//save max	
			if(pushups_counter > persist_read_int(MAX_PUSHUPS_KEY))
			{
				persist_write_int(MAX_PUSHUPS_KEY,pushups_counter/2);
			}
			//show congratulation
			init_end();
			return;
		
						
		}else{
			snprintf(count_str,sizeof(count_str), "%d", pushups_counter/2);
			text_layer_set_text(count_layer,count_str);
		}
	
	}

	x_pre_flag = new_x_flag;
	y_pre_flag = new_y_flag;
	z_pre_flag = new_z_flag;

	preAccel = accel;
	pushups_timer = app_timer_register(PUSHUPS_TIMER_INTERVAL,pushups_timer_callback,NULL);
}


static void setup_timer_callback(void *data) 
{
	setup_time--;
	if(setup_time==0)
	{
		//begin
		init_pushup();
		vibes_short_pulse();
		pushups_counter = 0;
		//becase of vibes so  timer set 1000
		pushups_timer = app_timer_register(1000,pushups_timer_callback,NULL);

	}else{
		snprintf(count_str,sizeof(count_str), "%d", setup_time);
		text_layer_set_text(count_layer,count_str);
		setup_timer = app_timer_register(SETUP_TIMER_INTERVAL,setup_timer_callback,NULL);
	}

}


static void window_load(Window *window)
{	
	Layer* rootLayer = window_get_root_layer(window); 
	GRect  rect = layer_get_frame(rootLayer);
	accel_data_service_subscribe(0,NULL);
	setup_timer = app_timer_register(SETUP_TIMER_INTERVAL,setup_timer_callback,NULL);
	setup_time = 3;
	title_layer = text_layer_create(GRect(0,0,rect.size.w,30)); 
	text_layer_set_font(title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));


	count_layer = text_layer_create(GRect(0,110,rect.size.w,30)); 
	image_layer = bitmap_layer_create(GRect(0, 30,rect.size.w,80));

	APP_LOG(APP_LOG_LEVEL_DEBUG,"push window load:1");
	text_layer_set_font(count_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));

	text_layer_set_text_alignment(title_layer, GTextAlignmentCenter);
	text_layer_set_text_alignment(count_layer, GTextAlignmentCenter);
  	bitmap_layer_set_alignment(image_layer, GAlignCenter);

  	layer_add_child(rootLayer, bitmap_layer_get_layer(image_layer));
	layer_add_child(rootLayer, text_layer_get_layer(count_layer));
	layer_add_child(rootLayer, text_layer_get_layer(title_layer));
	APP_LOG(APP_LOG_LEVEL_DEBUG,"push window load:2");
	init_ready();

	APP_LOG(APP_LOG_LEVEL_DEBUG,"push window load: %d", rect.size.h);
}

static void  window_unload(Window *window)
{
	accel_data_service_unsubscribe();
	text_layer_destroy(count_layer);
	text_layer_destroy(title_layer);
	bitmap_layer_destroy(image_layer);
	if(image)
	{	
		gbitmap_destroy(image);	
	}
	
	setup_timer?app_timer_cancel(setup_timer):1;
	if(pushups_timer)
	{
		app_timer_cancel(pushups_timer);
	}
	APP_LOG(APP_LOG_LEVEL_DEBUG,"push window unload");
}


Window *create_pushups_window()
{
	Window * win = window_create();
	window_set_window_handlers(win, (WindowHandlers) {
    		.load = window_load,
    		.unload = window_unload,
  	});
	return win;

}

