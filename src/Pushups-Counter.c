#include <pebble.h>
#include "pushups.h"
#include "about_info.h"

#define SECTION_COUNT 4


static Window *window;
static Window *pushups_window;
static Window *info_window;
static SimpleMenuLayer *menu_layer;
static SimpleMenuSection sections[1];
static SimpleMenuItem menu_items[SECTION_COUNT];

static NumberWindow * number_window;

static int menu_select = 0;

static char goal_str[10];
static char history_str[50];


static void select_menu_handler(int index, void *context);

static void reset_menu()
{
	
  	int num_a_items = 0;
	menu_items[num_a_items++] = (SimpleMenuItem){
		.title = "Start",
		.subtitle = "Lets Push-ups",
		.callback = select_menu_handler,
	};
	persist_exists(GOAL_KEY)?1: persist_write_int(GOAL_KEY,15);

	snprintf(goal_str,sizeof(goal_str), "%ld once", 
			persist_read_int(GOAL_KEY));
	menu_items[num_a_items++] = (SimpleMenuItem){
		.title= "Goal",
		.subtitle = goal_str,
		.callback = select_menu_handler,
	};
	
	snprintf(history_str,sizeof(history_str), "Total: %ld  Top: %ld", 
			persist_exists(TOTAL_PUSHUPS_KEY)?persist_read_int(TOTAL_PUSHUPS_KEY) : 0,
			persist_exists(MAX_PUSHUPS_KEY)?persist_read_int(MAX_PUSHUPS_KEY) : 0);
	menu_items[num_a_items++] = (SimpleMenuItem){
		.title = "History",
		.subtitle = history_str,
		.callback = select_menu_handler,
	};

	menu_items[num_a_items++] = (SimpleMenuItem){
		.title= "Version",
		.subtitle = "1.0  @copyright shanfeng",
		.callback = select_menu_handler,
	};

	sections[0] = (SimpleMenuSection){
    		.num_items = SECTION_COUNT,
    		.items = menu_items,
  	};

	if(menu_layer)
	{
		layer_remove_from_parent(simple_menu_layer_get_layer(menu_layer));
		simple_menu_layer_destroy(menu_layer);
	}
	
  	Layer *window_layer = window_get_root_layer(window);
  	GRect bounds = layer_get_bounds(window_layer);
  	menu_layer = simple_menu_layer_create(bounds,window,sections, 1,NULL);
	simple_menu_layer_set_selected_index(menu_layer,menu_select,false);
  	layer_add_child(window_layer, simple_menu_layer_get_layer(menu_layer));
}

static void goal_select(struct NumberWindow *number_window, void *context)
{
	persist_write_int(GOAL_KEY,number_window_get_value(number_window));
	window_stack_pop(true);
}

static void select_menu_handler(int index, void *context)
{
	APP_LOG(APP_LOG_LEVEL_DEBUG,"%d",index);
	switch(index)
	{
		case 0:
			if(!pushups_window)
			{
				APP_LOG(APP_LOG_LEVEL_DEBUG,
					"pushups_window create");
  			 	pushups_window = create_pushups_window();
			}
  			window_stack_push(pushups_window, true);
			break;
		case 1:
			if(!number_window)
			{
				number_window = number_window_create("Goal",(NumberWindowCallbacks){
					.selected = goal_select,
				},NULL);
				number_window_set_value(number_window,
					persist_read_int(GOAL_KEY));
				number_window_set_min(number_window,5);
				number_window_set_step_size(number_window, 5);
				number_window_set_max(number_window,1000);
			}
			window_stack_push((Window*)number_window,true);
			break;
		case 3:
			{
				if(!info_window)
				{
					info_window = aboutinfo_window_create();
				}
				window_stack_push(info_window,true);	
			} 

	}
	menu_select = index;
}


static void window_load(Window *window) {

}

static void window_unload(Window *window) {
	simple_menu_layer_destroy(menu_layer);
}


static void window_appear(Window *window)
{
	reset_menu();
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
    .appear = window_appear, 
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  number_window?number_window_destroy(number_window):1;
  pushups_window?window_destroy(pushups_window):1;
  info_window?aboutinfo_window_destroy(info_window):1;
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
