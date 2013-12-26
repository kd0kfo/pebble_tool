#include "quick_sms.h"

#include "io.h"

#include "pebble_fonts.h"

QuickSMSData_t QuickSMSData;

void QuickSMS_up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	send_cmd(CMD_SEND_SMS, "FOO?");
}

void QuickSMS_click_config_provider(ClickConfig **config, Window *window) {
	  config[BUTTON_ID_UP]->click.handler = (ClickHandler) QuickSMS_up_single_click_handler;
}

void QuickSMS_init() {

    Window* window = &QuickSMSData.window;
  window_init(window, "Weather");
  window_set_background_color(window, GColorBlack);
  window_set_fullscreen(window, true);

    text_layer_init(&QuickSMSData.temperature_layer, GRect(0, 100, 144, 68));
  text_layer_set_text_color(&QuickSMSData.temperature_layer, GColorWhite);
  text_layer_set_background_color(&QuickSMSData.temperature_layer, GColorClear);
  text_layer_set_font(&QuickSMSData.temperature_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(&QuickSMSData.temperature_layer, GTextAlignmentCenter);
  text_layer_set_text(&QuickSMSData.temperature_layer, "FOO!");
  layer_add_child(&window->layer, &QuickSMSData.temperature_layer.layer);

  window_set_click_config_provider(window, (ClickConfigProvider) QuickSMS_click_config_provider);
}


