#ifndef QUICK_SMS_H
#define QUICK_SMS_H

#include "pebble_os.h"

typedef enum {MORSE_INPUT_STATUS_UNUSED = 0, MORSE_INPUT_STATUS_BUSY, MORSE_INPUT_STATUS_SUCCESS, MORSE_INPUT_STATUS_ERROR, NUM_INPUT_MORSE_INPUT_STATUSES} MorseInputStatus;

typedef void (MorseInput_Response)(MorseInputStatus, const char *buffer, size_t buffersize); // Called when the morse input returns

typedef struct {
  Window window;
  TextLayer temperature_layer;
}MorseInputData_t;

extern MorseInputData_t MorseInputData;

void MorseInput_init();

void MorseInput_setup(MorseInput_Response *response_funct);

#endif
