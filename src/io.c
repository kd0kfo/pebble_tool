#include "io.h"

void load_bitmap(uint32_t resource_id) {
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
void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
}

void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {

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

void send_cmd(int cmd, const char *arg) {
	static const char empty[] = "";
	if(arg == NULL)
		arg = empty;
	AppSync *sync = &s_data.sync;
	Tuplet cmd_arg_pair = TupletCString(cmd, arg);
	app_sync_set(sync, &cmd_arg_pair, 1);
}



