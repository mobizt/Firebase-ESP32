# Firebase-ESP32
Arduino firebase database client libralies for ESP32 mcu (cloud messaging is not yet implemented)
## Device Support

 * SparkfunESP32 Thing
 * NodeMCU-32
 * WEMOS LOLIN32
 
## Features

### Get

Get int, float, boolean, String, Json object from defined firebase database.
```c++
int getInt(String path) ;
float getFloat(String path) ;
String getString(String path) ;
bool getBool(String path) ;
JsonVariant get(String path) ;
```


### Set

Set int, float, boolean, String, Json object to defined firebase database.

```c++
void setInt(String path, int value);
void setFloat(String path, float value);
void setString(String path, String value);
void setBool(String path, bool value);
void set(String path, JsonVariant value);
```

### Push

Put int, float, boolean, String, Json object to defined firebase database.

```c++
String pushInt(String path, int value);
String pushFloat(String path, float value);
String pushBool(String path, bool value);
String pushString(String path, String value);
String push(String path, JsonVariant value);
```


### Stream

Get the stream from firebase.

```c++
void stream(String path, StreamCallback callback) ;
void stopStream() ;
```

**Stream class**

```c++
String eventType() ;
String path() ;
int getInt() ;
float getFloat() ;
String getString() ;
JsonVariant getPayloadData() ;
```

**Example**

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


