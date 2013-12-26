#include "quick_sms.h"
#include "io.h"

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "resource_ids.auto.h"

#include <stdint.h>
#include <string.h>

#define BITMAP_BUFFER_BYTES 1024

// 4D4A4DC2-2077-4AF6-B3DA-60DB398932F3
#define MY_UUID {0x4D, 0x4A, 0x4D, 0xC2, 0x20, 0x77, 0x4A, 0xF6, 0xB3, 0xDA, 0x60, 0xDB, 0x39, 0x89, 0x32, 0xF3}
PBL_APP_INFO(MY_UUID,
		"Pebble Tool", "davecoss.com",
		0x0, 0x1, /** version **/
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
	send_cmd(CMD_SEND_ALERT, "Ping");
	vibes_short_pulse();
}

void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	send_cmd(CMD_GET_WEATHER, NULL);
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	window_stack_push(&QuickSMSData.window, true);
}

void click_config_provider(ClickConfig **config, Window *window) {
  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;
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
  app_sync_init(&s_data.sync, s_data.sync_buffer, sizeof(s_data.sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
                sync_tuple_changed_callback, sync_error_callback, NULL);
  window_set_click_config_provider(window, (ClickConfigProvider) click_config_provider);

  QuickSMS_init();

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
