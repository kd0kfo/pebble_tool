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

// Commands to be run on the phone
enum {
	CMD_GET_WEATHER = 0 /* Get weather data*/,
	CMD_SEND_SMS
};

// Dictionary Keys. Define the type of Tuple send to Phone App.
enum {
	KEY_MSG = 0/* Message to phone */,
	KEY_CMD/* Command to be exectuted on phone */,
	KEY_CMD_ARG/* cstring argument to command */
};

enum {
  WEATHER_ICON_KEY = 0x0,         // TUPLE_INT
  WEATHER_TEMPERATURE_KEY = 0x1,  // TUPLE_CSTRING
};

extern uint32_t WEATHER_ICONS[];

void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context);
void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context);

void send_msg(char *msg);
void send_cmd(int cmd, const char *);

#endif
