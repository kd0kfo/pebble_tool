/*
 * text_menu.c
 *
 *  Created on: Jan 5, 2014
 *      Author: David Coss, PhD
 */

#include "smsmenu.h"
#include "MorseInput.h"
#include "io.h"

#include "pebble_fonts.h"

SMSMenu_t SMSMenu_data;

#define NUM_MENU_SECTIONS 2
#define NUM_MENU_ICONS 3
#define NUM_FIRST_MENU_ITEMS 2
#define NUM_SECOND_MENU_ITEMS 2

char* SMS_MSGS[] = {"Leaving",
		"Driving"
};
#define NUM_SMS_MSGS 2


// A callback is used to specify the amount of sections of menu items
// With this, you can dynamically add and remove sections
uint16_t menu_get_num_sections_callback(MenuLayer *me, void *data) {
  return NUM_MENU_SECTIONS;
}


// Each section has a number of items;  we use a callback to specify this
// You can also dynamically add and remove items using this
uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return NUM_FIRST_MENU_ITEMS;

    case 1:
      return NUM_SECOND_MENU_ITEMS;

    default:
      return 0;
  }
}


// A callback is used to specify the height of the section header
int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data) {
  // This is a define provided in pebble_os.h that you may use for the default height
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}


// Here we draw what each header is
void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  switch (section_index) {
    case 0:
      // Draw title text in the section header
      menu_cell_basic_header_draw(ctx, cell_layer, "Preset Messages");
      break;

    case 1:
      menu_cell_basic_header_draw(ctx, cell_layer, "Custom Text");
      break;
  }
}


// This is the menu item draw callback where you specify what each item should look like
void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {

	char SEND_TITLE[] = "Send";
	switch (cell_index->section) {
    case 0:
    	if(cell_index->row >= NUM_SMS_MSGS)
    		break;
		  menu_cell_basic_draw(ctx, cell_layer, SEND_TITLE, SMS_MSGS[cell_index->row], NULL);
		  break;

    case 1:
      switch (cell_index->row) {
        case 0:
          // There is title draw for something more simple than a basic menu item
        	menu_cell_basic_draw(ctx, cell_layer, "Create Text", "Click Here for custom message.", NULL);
        	break;
        case 1:
        	menu_cell_basic_draw(ctx, cell_layer, "Send Text", SMSMenu_data.custom_sms_buffer, NULL);
        	break;
      }
  }
}

void SMSMenu_morse_response(MorseInputStatus status, const char *buffer, size_t buffersize) {
	if(status == MORSE_INPUT_STATUS_SUCCESS) {
		strncpy(SMSMenu_data.custom_sms_buffer, buffer, buffersize);
	}
}


// Here we capture when a user selects a menu item
void SMSMenu_select_callback(MenuLayer *me, MenuIndex *cell_index, void *data) {
	if(cell_index->section == 0) {
		if(cell_index->row >= NUM_SMS_MSGS)
			return;
		send_cmd(CMD_SEND_SMS, SMS_MSGS[cell_index->row]);
	} else if(cell_index->section == 1) {
		if(cell_index->row == 0) {
			MorseInput_setup(SMSMenu_morse_response);
			window_stack_push(&MorseInputData.window, true);
		} else if(cell_index->row == 1) {
			send_cmd(CMD_SEND_SMS, SMSMenu_data.custom_sms_buffer);
		}
	}
}


// This initializes the menu upon window load
void SMSMenu_load(Window *me) {
	MenuLayer *menu_layer = &SMSMenu_data.menu_layer;
  // Now we prepare to initialize the menu layer
  // We need the bounds to specify the menu layer's viewport size
  // In this case, it'll be the same as the window's
  GRect bounds = me->layer.bounds;

  memset(SMSMenu_data.custom_sms_buffer, 0, DEFAULT_SMS_BUFFER_SIZE);// Initially empty

  // Initialize the menu layer
  menu_layer_init(menu_layer, bounds);

  // Set all the callbacks for the menu layer
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = SMSMenu_select_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(menu_layer, me);

  // Add it to the window for display
  layer_add_child(&me->layer, menu_layer_get_layer(menu_layer));
}


void SMSMenu_unload(Window *me) {
}


void SMSMenu_init(AppContextRef ctx) {
  window_init(&SMSMenu_data.window, "SMS Messages");
  window_set_window_handlers(&SMSMenu_data.window, (WindowHandlers){
    .load = SMSMenu_load,
    .unload = SMSMenu_unload,
  });
}
