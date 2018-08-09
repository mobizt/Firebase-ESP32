# Firebase-ESP32
Arduino firebase database client libraly for ESP32 mcu (cloud messaging is not yet implemented)
## Device Support

 * Sparkfun ESP32 Thing
 * NodeMCU-32
 * WEMOS LOLIN32
 
## Features

Get, set, push and stream data in int, float, boolean, String and JSON object from/to defined firebase database.
Supported functions

```c++
int getInt(String path) ;
float getFloat(String path) ;
String getString(String path) ;
bool getBool(String path) ;
JsonVariant get(String path) ;

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

void stream(String path, StreamCallback callback) ;
void stopStream() ;

```


## Usage

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


