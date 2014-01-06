#ifndef PEBBLE_LIB_H
#define PEBBLE_LIB_H

#include "resource_ids.auto.h"

#include "pebble_os.h"

typedef struct  {
  Window window;
  TextLayer temperature_layer;
  BitmapLayer icon_layer;
  uint32_t current_icon;
  HeapBitmap icon_bitmap;
  AppSync sync;
  uint8_t sync_buffer[32];
}WeatherData_t;
extern WeatherData_t s_data;

// Dictionary Keys. Define the type of Tuple send to Phone App.
enum {
	KEY_CMD = 0/* Command to be exectuted on phone */,
	KEY_CMD_ARG/* cstring argument to command */
};

// TUPLE_INT
#define WEATHER_ICON_KEY KEY_CMD

// TUPLE_CSTRING
#define WEATHER_TEMPERATURE_KEY KEY_CMD_ARG

extern uint32_t WEATHER_ICONS[];

void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context);
void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context);

void send_cmd(int cmd, const char *);

#endif
