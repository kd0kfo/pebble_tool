#ifndef QUICK_SMS_H
#define QUICK_SMS_H

#include "pebble_os.h"

typedef struct {
  Window window;
  TextLayer temperature_layer;
}MorseInputData_t;

extern MorseInputData_t MorseInputData;

void MorseInput_init();

void MorseInput_reset_morse();

#endif
