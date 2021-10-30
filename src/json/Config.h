#ifndef FirebaseJSON_CONFIG_H
#define FirebaseJSON_CONFIG_H
#include <Arduino.h>

/** Use PSRAM for supported ESP32 module */
#if defined(ESP32)
#define FIREBASEJSON_USE_PSRAM
#endif

#endif