#ifndef QUICK_SMS_H
#define QUICK_SMS_H

#include "pebble_os.h"

typedef struct {
  Window window;
  TextLayer temperature_layer;
}QuickSMSData_t;

extern QuickSMSData_t QuickSMSData;

extern void QuickSMS_init();

#endif
