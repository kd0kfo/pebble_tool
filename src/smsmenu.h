#ifndef SMSMENU_H
#define SMSMENU_H

#include "pebble_os.h"

#define DEFAULT_SMS_BUFFER_SIZE 30
typedef struct {
	Window window;
	MenuLayer menu_layer;
	char custom_sms_buffer[DEFAULT_SMS_BUFFER_SIZE];
} SMSMenu_t;

extern SMSMenu_t SMSMenu_data;

void SMSMenu_init();

#endif /* SMSMENU_H */
