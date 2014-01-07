#include "MorseInput.h"
#include "smsmenu.h"
#include "io.h"
#include "commands.h"

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "resource_ids.auto.h"

#include "config.h"

#include <stdint.h>
#include <string.h>

PBL_APP_INFO(MY_UUID,
	     APP_NAME, APP_COMPANY,
	     APP_VERSION_MAJOR, APP_VERSION_MINOR, /** version **/
	     DEFAULT_MENU_ICON,
	     APP_INFO_STANDARD_APP);


WeatherData_t s_data;

uint32_t WEATHER_ICONS[] = {
  RESOURCE_ID_IMAGE_SUN,
  RESOURCE_ID_IMAGE_CLOUD,
  RESOURCE_ID_IMAGE_RAIN,
  RESOURCE_ID_IMAGE_SNOW
};

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	send_cmd(CMD_SEND_ALERT, "ping");
}

void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	send_cmd(CMD_GET_WEATHER, NULL);
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	window_stack_push(&SMSMenu_data.window, true);
}

void click_config_provider(ClickConfig **config, Window *window) {
  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;
}

void main_window_load(struct Window *window) {
}

void main_window_appear(struct Window *window) {
}

void main_window_disappear(struct Window *window) {
}

void main_window_unload(struct Window *window) {
}

void init_other_windows() {
	MorseInput_init();
	SMSMenu_init();
}

static void tool_app_init(AppContextRef c) {

  s_data.current_icon = 0;

  resource_init_current_app(&WEATHER_APP_RESOURCES);

  Window* window = &s_data.window;
  window_init(window, "Weather");
  window_set_background_color(window, GColorBlack);
  window_set_fullscreen(window, true);

  GRect icon_rect = (GRect) {(GPoint) {32, 10}, (GSize) { 80, 80 }};
  bitmap_layer_init(&s_data.icon_layer, icon_rect);
  layer_add_child(&window->layer, &s_data.icon_layer.layer);

  text_layer_init(&s_data.temperature_layer, GRect(0, 100, 144, 68));
  text_layer_set_text_color(&s_data.temperature_layer, GColorWhite);
  text_layer_set_background_color(&s_data.temperature_layer, GColorClear);
  text_layer_set_font(&s_data.temperature_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(&s_data.temperature_layer, GTextAlignmentCenter);
  layer_add_child(&window->layer, &s_data.temperature_layer.layer);

  Tuplet initial_values[] = {
    TupletInteger(WEATHER_ICON_KEY, (uint8_t) 1),
    TupletCString(WEATHER_TEMPERATURE_KEY, "???\u00B0F"),
  };
  app_sync_init(&s_data.sync, s_data.sync_buffer, sizeof(s_data.sync_buffer),
		initial_values, ARRAY_LENGTH(initial_values),
                sync_tuple_changed_callback, sync_error_callback, NULL);
  window_set_click_config_provider(window, (ClickConfigProvider) click_config_provider);

  WindowHandlers handlers = {.load = main_window_load,
			     .appear = main_window_appear,
			     .disappear = main_window_disappear,
			     .unload = main_window_unload
  };
  window_set_window_handlers(window, handlers);

  init_other_windows();

  window_stack_push(window, true);

}

static void tool_app_deinit(AppContextRef c) {
  app_sync_deinit(&s_data.sync);
  if (s_data.current_icon != 0) {
    heap_bitmap_deinit(&s_data.icon_bitmap);
  }
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &tool_app_init,
    .deinit_handler = &tool_app_deinit,
    .messaging_info = {
      .buffer_sizes = {
        .inbound = 64,
        .outbound = 16,
      }
    }
  };
  app_event_loop(params, &handlers);
}
