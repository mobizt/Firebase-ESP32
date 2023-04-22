#include "Firebase_Client_Version.h"
#if !FIREBASE_CLIENT_VERSION_CHECK(40310)
#error "Mixed versions compilation."
#endif

#ifndef FirebaseFS_H
#define FirebaseFS_H

#pragma once

#include <Arduino.h>
#include "mbfs/MB_MCU.h"

#define FIREBASE_ESP32_CLIENT 1

/**
 * To use other flash file systems
 *
 * LittleFS File system
 *
 * #include <LittleFS.h>
 * #define DEFAULT_FLASH_FS LittleFS //For ESP8266 LitteFS
 *
 *
 * FAT File system
 *
 * #include <FFat.h>
 * #define DEFAULT_FLASH_FS FFat  //For ESP32 FAT
 *
 */
#if defined(ESP32)
#include <SPIFFS.h>
#define DEFAULT_FLASH_FS SPIFFS
#endif

/**
 * To use SD card file systems with different hardware interface
 * e.g. SDMMC hardware bus on the ESP32
 * https://github.com/espressif/arduino-esp32/tree/master/libraries/SD#faq
 *
 #include <SD_MMC.h>
 #define DEFAULT_SD_FS SD_MMC //For ESP32 SDMMC
 #define CARD_TYPE_SD_MMC 1 //For ESP32 SDMMC

 *
*/

/**
 * To use SdFat on ESP32

#if defined(ESP32)
#include <SdFat.h> //https://github.com/greiman/SdFat
static SdFat sd_fat_fs;   //should declare as static here
#define DEFAULT_SD_FS sd_fat_fs
#define CARD_TYPE_SD 1
#define SD_FS_FILE SdFile
#endif

* The SdFat (https://github.com/greiman/SdFat) is already implemented as wrapper class in ESP8266 core library.
* Do not include SdFat.h library in ESP8266 target code which it conflicts with the wrapper one.

*/

#if defined(ESP32) || defined(ESP8266)
#include <SD.h>
#define DEFAULT_SD_FS SD
#define CARD_TYPE_SD 1
#endif

// For RTDB legacy token usage only
// #define USE_LEGACY_TOKEN_ONLY

// Enable the error string from fbdo.errorReason */
// You can get the error code from fbdo.errorCode() when disable this option
#define ENABLE_ERROR_STRING

// For ESP32, format SPIFFS or FFat if mounting failed
#define FORMAT_FLASH_IF_MOUNT_FAILED 1

// Comment to exclude the Firebase Realtime Database
#define ENABLE_RTDB

#define ENABLE_ERROR_QUEUE

// Comment to exclude Firebase Cloud Messaging
#define ENABLE_FCM

/** Use PSRAM for supported ESP32 module */
#define FIREBASE_USE_PSRAM

// To enable OTA updates
#define ENABLE_OTA_FIRMWARE_UPDATE

// Use Keep Alive connection mode
#define USE_CONNECTION_KEEP_ALIVE_MODE

// To enable external Client for ESP8266.
// This will enable automatically for other devices.
// #define FB_ENABLE_EXTERNAL_CLIENT

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
// You can create your own header file "CustomFirebaseFS.h" in the same diectory of 
// "FirebaseFS.h" and put your own custom config to overwrite or 
// change the default config in "FirebaseFS.h".
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://

/** This is an example of "CustomFirebaseFS.h" 

#pragma once

#ifndef CustomFirebaseFS_H
#define CustomFirebaseFS_H

// Use external client instead of internal client
#define FB_ENABLE_EXTERNAL_CLIENT // define to use external client

// Use LittleFS instead of SPIFFS
#include "LittleFS.h"
#undef DEFAULT_FLASH_FS // remove Flash FS defined macro
#define DEFAULT_FLASH_FS LittleFS

// Use SD_MMC instead of SD
#include <SD_MMC.h>
#undef DEFAULT_SD_FS // remove SD defined macro
#undef CARD_TYPE_SD // remove SD defined macro
#define DEFAULT_SD_FS SD_MMC
#define CARD_TYPE_SD_MMC 1


// Disable Error Queue and FCM.
#undef ENABLE_ERROR_QUEUE
#undef ENABLE_FCM

#endif

*/
#if __has_include("CustomFirebaseFS.h")
#include "CustomFirebaseFS.h"
#endif

#endif