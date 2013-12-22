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

static struct WeatherData {
  Window window;
  TextLayer temperature_layer;
  BitmapLayer icon_layer;
  uint32_t current_icon;
  HeapBitmap icon_bitmap;
  AppSync sync;
  uint8_t sync_buffer[32];
} s_data;

enum {
  WEATHER_ICON_KEY = 0x0,         // TUPLE_INT
  WEATHER_TEMPERATURE_KEY = 0x1,  // TUPLE_CSTRING
};

enum {
	CMD_GET_WEATHER = 0
};

enum {
	KEY_MSG = 0,
	KEY_CMD
};

static uint32_t WEATHER_ICONS[] = {
  RESOURCE_ID_IMAGE_SUN,
  RESOURCE_ID_IMAGE_CLOUD,
  RESOURCE_ID_IMAGE_RAIN,
  RESOURCE_ID_IMAGE_SNOW
};


static void load_bitmap(uint32_t resource_id) {
  // If that resource is already the current icon, we don't need to reload it
  if (s_data.current_icon == resource_id) {
    return;
  }
  // Only deinit the current bitmap if a bitmap was previously loaded
  if (s_data.current_icon != 0) {
    heap_bitmap_deinit(&s_data.icon_bitmap);
  }
  // Keep track of what the current icon is
  s_data.current_icon = resource_id;
  // Load the new icon
  heap_bitmap_init(&s_data.icon_bitmap, resource_id);
}

// TODO: Error handling
static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {

  switch (key) {
  case WEATHER_ICON_KEY:
	load_bitmap(WEATHER_ICONS[new_tuple->value->uint8]);
    bitmap_layer_set_bitmap(&s_data.icon_layer, &s_data.icon_bitmap.bmp);
    break;
  case WEATHER_TEMPERATURE_KEY:
    // App Sync keeps the new_tuple around, so we may use it directly
    text_layer_set_text(&s_data.temperature_layer, new_tuple->value->cstring);
    break;
  default:
    return;
  }
}

static void send_msg(char *msg) {
	AppSync sync = s_data.sync;
	if(msg == NULL)
		return;
	Tuplet t = TupletCString(KEY_MSG, msg);
	app_sync_set(&sync, &t, 1);
}

static void send_cmd(int cmd) {
	AppSync sync = s_data.sync;
	Tuplet t = TupletInteger(KEY_CMD, cmd);
	app_sync_set(&sync, &t, 1);
}

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	send_msg("Ping");
	vibes_short_pulse();
}

void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	send_cmd(CMD_GET_WEATHER);
}

void click_config_provider(ClickConfig **config, Window *window) {
  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
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
    TupletCString(WEATHER_TEMPERATURE_KEY, "1234\u00B0C"),
  };
  app_sync_init(&s_data.sync, s_data.sync_buffer, sizeof(s_data.sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
                sync_tuple_changed_callback, sync_error_callback, NULL);

  window_set_click_config_provider(window, (ClickConfigProvider) click_config_provider);

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
