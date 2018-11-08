# Firebase-ESP32

Google's Firebase real time database - ESP32 library for Arduino

This library provides ESP32 to post, get, put and stream data to Google's Firebase database through REST API. 

The library was test and work well with ESP32s based module with no certificate or fingerprint verification.

Copyright (c) 2018 K. Suwatchai (Mobizt).

## Tested Devices

This library works well in the following tested devices.

 * Sparkfun ESP32 Thing
 * NodeMCU-32
 * WEMOS LOLIN32
 
## Features

Get, set, push and stream data in int, float, boolean, String and JSON object from/to defined firebase database.
Cloud messaging was not implemented yet.

**Supported functions**

```c++
int getInt(String path);
float getFloat(String path);
String getString(String path);
bool getBool(String path);
JsonVariant get(String path);

void setInt(String path, int value);
void setFloat(String path, float value);
void setString(String path, String value);
void setBool(String path, bool value);
void set(String path, JsonVariant value);

String pushInt(String path, int value);
String pushFloat(String path, float value);
String pushBool(String path, bool value);
String pushString(String path, String value);
String push(String path, JsonVariant value);

void stream(String path, StreamCallback callback);
void stopStream() ;

```


## Usages

```c++
Firebase.stream("/temp/ch01",streamCallback);

void streamCallback(streamResult event){
  String eventType = event.eventType();
  eventType.toLowerCase();
  if (eventType == "put" && event.path() == "/sensorValue") {
    Serial.println(event.getFloat());
  }
}
```


