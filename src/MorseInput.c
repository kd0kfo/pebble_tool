#include "morse.h"

#include "MorseInput.h"

#include "io.h"

#include "pebble_fonts.h"

MorseInputData_t MorseInputData;

#define MorseInput_screen_buffer_size 32
size_t MorseInput_screen_buffer_idx = 0;
char MorseInput_screen_buffer[MorseInput_screen_buffer_size];

#define MorseInput_morse_buffer_size 6
size_t MorseInput_morse_buffer_idx = 0;
char MorseInput_morse_buffer[MorseInput_morse_buffer_size];

// Called when the dialog closes
static MorseInput_Response *response_function = NULL;

void MorseInput_reset_morse() {
  MorseInput_morse_buffer_idx = 0;
  MorseInput_morse_buffer[MorseInput_morse_buffer_idx] = 0;

  MorseInput_screen_buffer_idx = 0;
  MorseInput_screen_buffer[MorseInput_screen_buffer_idx] = 0;
  layer_mark_dirty(&MorseInputData.temperature_layer.layer);
}

void MorseInput_setup(MorseInput_Response *response_funct) {
    MorseInput_reset_morse();
    response_function = response_funct;
}

void MorseInput_update_screen_buffer(char ch) {
  MorseInput_screen_buffer[MorseInput_screen_buffer_size - 1] = 0;// ensure always null terminated
  if(MorseInput_screen_buffer_idx >= MorseInput_screen_buffer_size - 1)
    return;

  MorseInput_screen_buffer[MorseInput_screen_buffer_idx] = ch;
  MorseInput_screen_buffer[MorseInput_screen_buffer_idx + 1] = 0;
  
  text_layer_set_text(&MorseInputData.temperature_layer, MorseInput_screen_buffer);
  layer_mark_dirty(&MorseInputData.temperature_layer.layer);
}

void MorseInput_add_char(char ch) {
  if(MorseInput_morse_buffer_idx >= MorseInput_morse_buffer_size - 1)
    return;

  MorseInput_morse_buffer[MorseInput_morse_buffer_idx++] = ch;
  MorseInput_morse_buffer[MorseInput_morse_buffer_idx] = 0;

  MorseInput_update_screen_buffer(morse2char(MorseInput_morse_buffer));
}

void MorseInput_select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  MorseInput_screen_buffer_idx++;
  MorseInput_morse_buffer_idx = 0;
  MorseInput_morse_buffer[0] = 0;
}

void MorseInput_up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  MorseInput_add_char('.');
}

void MorseInput_down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  MorseInput_add_char('-');
}

void MorseInput_click_config_provider(ClickConfig **config, Window *window) {
  config[BUTTON_ID_UP]->click.handler = (ClickHandler) MorseInput_up_single_click_handler;
  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) MorseInput_down_single_click_handler;
  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) MorseInput_select_single_click_handler;
}

void MorseInput_load(struct Window *window) {
	text_layer_set_text(&MorseInputData.temperature_layer, "Enter Text Here");
}

void MorseInput_appear(struct Window *window) {
}

void MorseInput_disappear(struct Window *window) {
	MorseInput_screen_buffer[MorseInput_screen_buffer_idx] = 0;
	if(response_function != NULL) {
		(*response_function)(MORSE_INPUT_STATUS_SUCCESS, MorseInput_screen_buffer, MorseInput_screen_buffer_idx);
		response_function = NULL;
	}
}

void MorseInput_unload(struct Window *window) {
}

void MorseInput_init() {

    Window* window = &MorseInputData.window;
  window_init(window, "Weather");
  window_set_background_color(window, GColorBlack);
  window_set_fullscreen(window, true);

  WindowHandlers handlers = {.load = MorseInput_load,
  			     .appear = MorseInput_appear,
  			     .disappear = MorseInput_disappear,
  			     .unload = MorseInput_unload
  };
  window_set_window_handlers(window, handlers);

    text_layer_init(&MorseInputData.temperature_layer, GRect(0, 100, 144, 68));
  text_layer_set_text_color(&MorseInputData.temperature_layer, GColorWhite);
  text_layer_set_background_color(&MorseInputData.temperature_layer, GColorClear);
  text_layer_set_font(&MorseInputData.temperature_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(&MorseInputData.temperature_layer, GTextAlignmentCenter);
  layer_add_child(&window->layer, &MorseInputData.temperature_layer.layer);

  window_set_click_config_provider(window, (ClickConfigProvider) MorseInput_click_config_provider);
}


