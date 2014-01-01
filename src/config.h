#ifndef CONFIG_H
#define CONFIG_H

#define APP_COMPANY "davecoss.com"
#define APP_VERSION_MAJOR 0
#define APP_VERSION_MINOR 1

#ifndef TEST_BUILD

// Prod Build
// 4D4A4DC2-2077-4AF6-B3DA-60DB398932F3
#define MY_UUID {0x4D, 0x4A, 0x4D, 0xC2, 0x20, 0x77, 0x4A, 0xF6, 0xB3, 0xDA, 0x60, 0xDB, 0x39, 0x89, 0x32, 0xF3}
#define APP_NAME "Pebble Tool"

#else

// Test Build
// 55BF4759-22F0-47E5-B380-5438CF6208C5
#define MY_UUID {0x55, 0xBF, 0x47, 0x59, 0x22, 0xF0, 0x47, 0xE5, 0xB3, 0x80, 0x54, 0x38, 0xCF, 0x62, 0x08, 0xC5}
#define APP_NAME "Pebble Tool (Test)"

#endif


#endif // CONFIG_H
