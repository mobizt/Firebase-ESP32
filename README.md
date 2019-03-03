# Firebase Real Time Database Arduino Client Library for ESP32

Google's Firebase Real Time Database Arduino Library for ESP32 v 2.3.0

This client library provides the most reliable operations for read, store, update, delete, backup and restore the database data.

In addition, the library allows you to read and store binary data from/to device's memory or external SD memory.

This library comunicated with Google Firebase Real Time Database through the REST API. 

The library supports all ESP32 MCU based modules. For ESP8266 please try [ESP8266 Firebase Arduino library]( https://github.com/mobizt/Firebase-ESP8266)

Copyright (c) 2019 K. Suwatchai (Mobizt).

## Tested Devices

This following devices were tested and work well.

 * Sparkfun ESP32 Thing
 * NodeMCU-32
 * WEMOS LOLIN32
 
## Features

* **Read data** at the defined database path through get functions e.g. **getInt**, **getFloat**, **getString**, **getJSON**, **getBlob** and **getFile**.

* **Store data** at the defined database path through set functions e.g. **setInt**, **setFloat**, **setString**, **setJSON**, **setBlob** and **setFile**.

* **Append data** to the defined database path through push functions e.g. **pushInt**, **pushFloat**, **pushString**, **pushJSON**, **pushBlob** and **pushFile**.

* **Update data** at the defined database path through **updateNode** and **updateNodeSilent** functions.

* **Delete data** at the defined database path (include all child nodes) through **deleteNode** function.

* Using **Firebase Data object** that holds all data and instances.

* Supports integer, float, string and JSON string data types. Boolean data is actually not supported by Firebase, unless using integer or float to determine its non-zero and zero values for boolean.  For JSON string data type, parsing as an object required external JSON parser library e.g. [**ArduinoJson**](https://github.com/bblanchon/ArduinoJson).

* Supports **automatic stream resuming** when the stream connection was timeout. 

* Supports **multiple streaming paths** through multiple Firebase Data objects. 

* Supports set and remove **stream event callbacks**.

* Supports **Firebase pause**.

* Supports data **backup and restore** that working with SD card. 

* Supports **BLOB** or binary data from memory and **File stream** from SD card.


## Dependencies

Required [HTTPClientESP32Ex library](https://github.com/mobizt/HTTPClientESP32Ex) to be installed. The HTTPClientESP32Ex library was the customized HTTPClient wrapper used for making SSL connection.

## Usages

__Declaration and Initialization__


```C++

//1. Include Firebase ESP32 library

#include "FirebaseESP32.h"

//2. Declare the Firebase Data object in global scope

FirebaseData firebaseData;

//3. Setup Firebase credential in setup()

Firebase.begin("yout_project_id.firebaseio.com", "your_Firebase_database_secret");

//4. Optional, setup AP reconnection in setup()

Firebase.reconnectWiFi(true);

```

__Read, Store, Update, Delete, Backup and Restore Data__

To read the data, use get<Data Type> functions i.e. getInt, getFlot, getString, getJSON, getBlob and getFile.

get<Data Type> function returned boolean value for success of operation. The success of operation determined from

payload that Firebase server returned back to client. The http status and matching between data type request and response.

To read the payload data, one of theses functions can be called i.e. intData, floatData, stringData, jsonData and blobData.

fileData function was not implemented becase data was handled as file stream.

getBlob and getFile will read the binary data that stored in database. Actually BLOB or binary data type is not supported by Firebase, this library will encode all bytes of data into appopiated format string before send to Firebase's server.

Any binary data that store to database and read from database called BlOB which is binary data in RAM, while binary data from file that being read from or write to SD card, called File stream.

getBlob will read data from server and decoded it to byte array in memory. getFile will read data from server, decoded and save it to SD card.

Here is the example usage to read integer from defined database path "/test/int".


```C++

//Read integer from "/test/int"

  int val = 0;

  if (Firebase.getInt(firebaseData, "/test/int")) {

    //Success, then read the payload value

    //Make sure payload value returned from server is integer
    //This prevent you to get garbage data
    if (firebaseData.dataType() == "int")) {
      val = firebaseData.intData();
      Serial.println(val);
    }

  } else {
    //Failed, then print out the error detail
    Serial.println(firebaseData.errorReason()));
  }

```

To store the data, use set<Data Type> functions i.e. setInt, setFlot, setString, setJSON, setBlob and setFile.

set<Data Type> function returned boolean value for success of operation. The success of operation determined from

payload that Firebase server returned back to client. The http status and matching between data type request and response.

Only setBlob and setFile functions that make a silent request to Firebase server, thus no payload response is returned. 

The success operation for setBlob and setFile determined only from http status "No Content".


Below is the example usage to store or set file data from SD card, "/test.txt" to defined database path "/test/file_data".


```C++

//Store file content "/test.txt" on SD card to database at "/test/file_data".


if (Firebase.setFile(firebaseData, "/test/file_data", "/test.txt")){

  //Success, data was downloaded and saved then open the file.
  //Print out all file content.

  File file = SD.open("/test.txt", FILE_READ);

  while (file.available())
  {     
    Serial.print(file.read(), HEX);     
  }    
  file.close();
  Serial.println();

} else {
  //Failed, then print out the error detail for file transfer error.
  //Use firebaseData.fileTransferError instead of firebaseData.errorReason() in case of working with file.
  Serial.println(firebaseData.fileTransferError()));
}

```

__Data changes monitoring__

The Firebase Real Time Database update or change event was sent to client through the HTTP stream connection. The connection was keep alive as long as no network communication interruption.

The function to handle the update and change event monitoring are beginStream, setStreamCallback and readStream.

To subscribe, monitor or follow the data change on defined database path, the function beginStream must be called first.

To handle the data that will be received when database at that path updates or changes, the function readStream should be called in loop,
otherwise callback function is need to assigned using setStreamCallback.

When no callback is assigned, the data that return from stream should be routinely veriy inside the loop function. To determine the stream data is available, function streamAvailable should be called. The function streamAvailable returned true when stream data was received in buffer. 

Then stream data can be accessed directly by calling intData, floatData, stringData, jsonData and blobData.

If stream callback function was assigned, it should accept StreamData as parameter which is a class that contains all data from stream, included streamPath, dataPath, dataType and value changes at defined streaming path.

Here is the example use of stream to handle the changes or updates at "/test/data".

```C++

//In setup(), set the stream callback function to handle data
//streamCallback is the function that called when database data changes or updates occurred
//streamTimeoutCallback is the function that called when connection between server and client was timeout during HTTP stream

Firebase.setStreamCallback(firebaseData, streamCallback, streamTimeoutCallback);

//In setup(), set the streaming path to "/test/data" and begin stream connection

if (!Firebase.beginStream(firebaseData, "/test/data"))
{
  //Could not begin stream connection, then print out the error detail
  Serial.println(firebaseData.errorReason());
}

  
  //Global function that handle stream data
void streamCallback(StreamData data)
{

  //Print out all information

  Serial.println("Stream Data...");
  Serial.println(data.streamPath());
  Serial.println(data.dataPath());
  Serial.println(data.dataType());

  //Print out value
  //Stream data can be many types which can be determined from function dataType

  if (data.dataType() == "int")
    Serial.println(data.intData());
  else if (data.dataType() == "float")
    Serial.println(data.floatData());
  else if (data.dataType() == "string")
    Serial.println(data.stringData());
  else if (data.dataType() == "json")
    Serial.println(data.jsonData());

}

//Global function that notify when stream connection lost
//The library will resume the stream connection automatically
void streamTimeoutCallback()
{
  Serial.println("Stream timeout, resume streaming...");
}

```

__Database Backup and Restore__

The backup and restore are very easy, no need to specify the filename, just defined the database path to backup or restore and
the folder or path on SD card to save the backup data.

The downloaded backup file is actually JSON text file with .json extension. The file name is constructed from the defined database path,
e.g. "/root.json" for "/" or "/mydb.backup.data.json" for "/mydb/backup/data".

The database restoration returned completed status only when Firebase server completely update the data. Any failed operation will not affected the database (no updates or changes).

Here is the usage example to back up all database at root path "/" and restore it back to database.

```C++

 String backupFilePath = "";

 //Begin backup whole database and download to "/myBackup" on SD card
 if (!Firebase.backup(firebaseData, "/", "/myBackup"))
 {
   //Error when backup database, then print out file transfer error detail
   Serial.println(firebaseData.fileTransferError());
 }
 else
 {
   //Successfull backup, print out the file path and size
   Serial.println(firebaseData.getBackupFilename());
   Serial.println(String(firebaseData.getBackupFileSize()));

   backupFilePath = firebaseData.getBackupFilename();
  }


  //Begin restore backed dup data back to database

  if (!Firebase.restore(firebaseData, "/", backupFilePath))
  {
    //Database restoration failed, printout the error detail
    Serial.println(firebaseData.fileTransferError());
  }
  else
  {
    //Database restoration success, printout the filename that used for restore
    Serial.println(firebaseData.getBackupFilename());
  }

```



See [full examples](https://github.com/mobizt/Firebase-ESP32/tree/master/examples) for all features usages.



## Supported functions

__Global functions__


**Store Firebase's authentication credentials.**

param *host* - Your Firebase database project host without http:// or https:// protocol e.g. Your_ProjectID.firebaseio.com.

param *auth* - Your database secret.

```C++
void begin(const String &host, const String &auth);
```



**Store Firebase's authentication credentials.**

param *host* - Your Firebase database project host without http:// or https:// protocol e.g. Your_ProjectID.firebaseio.com.

param *auth* - Your database secret.

param *rootCA* - Base64 encoded root certificate string.

```C++
void begin(const String &host, const String &auth, const char *rootCA);
```



**Reconnect WiFi if lost connection.**

param *reconnect* - The boolean to set/unset WiFi AP reconnection.

```C++
void reconnectWiFi(bool reconnect);
```



**Determine whether defined database path is existed or not.**

param *dataObj* - Firebase Data Object to hold data and instances.

param *path* - Database path to be checked.

return - *Boolean* type result indicates whether the defined database
path was existed or not.

```C++
bool pathExist(FirebaseData &dataObj, const String &path);
```



**Append new integer value to the defined database path.**

param *dataObj* - Firebase Data Object to hold data and instances.

param *path* - Target database path which integer value will be appended.

param *intValue* - The appended value.

return *Boolean* type status indicates the success of operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushInt(FirebaseData &dataObj, const String &path, int intValue);
```



**Append new float value to the defined database path.**

param *dataObj* - Firebase Data Object to hold data and instances.

param *path* - Target database path which float value will be appended.

param *floatValue* - The appended value.

return *Boolean* type status indicates the success of operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushFloat(FirebaseData &dataObj, const String &path, float floatValue);
```



**Append new string (text) to the defined database path.**

param *dataObj* - Firebase Data Object to hold data and instances.

param *path* - Target database path which string will be appended.

param *StringValue* - The appended value.

return *Boolean* type status indicates the success of operation.

The new appended node's key stored in Firebase Data object, 
which can be accessed via function [FirebaseData object].pushName().

```C++
bool pushString(FirebaseData &dataObj, const String &path, const String &stringValue);
```



**Append new child nodes's key and value (using JSON data) to the defined database path.**

param *dataObj* - Firebase Data Object to hold data and instances.

param *path* - Target database path which key and value in JSON data will be appended.

param *jsonString* - The appended JSON string (should be valid JSON data).

return *Boolean* type status indicates the success of operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushJSON(FirebaseData &dataObj, const String &path, const String &jsonString);
```



**Append new blob (binary data) to the defined database path.**

param *dataObj* - Firebase Data Object to hold data and instances.

param *path* - Target database path which binary data will be appended.

param *blob* - Byte array of data.

param *size* - Size of byte array.

return *Boolean* type status indicates the success of operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushBlob(FirebaseData &dataObj, const String &path, uint8_t *blob, size_t size);
```




**Append new binary data from file stores on SD card to the defined database path.**

param *dataObj* - Firebase Data Object to hold data and instances.

param *path* - Target database path which binary data from file will be appended.

param *fileName* - File name (full file path) in SD card.

return *Boolean* type status indicates the success of operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushFile(FirebaseData &dataObj, const String &path, const String &fileName);
```




**Set integer data at the defined database path.**

param *dataObj* - Firebase Data Object to hold data and instances.

param *path* - Target database path which integer data will be set.

param *intValue* - Integer value to set.

return *Boolean* type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 
    
Call [FirebaseData object].intData will return the integer value of
payload returned from server.

```C++
bool setInt(FirebaseData &dataObj, const String &path, int intValue);
```



**Set float data at the defined database path.**

param *dataObj* - Firebase Data Object to hold data and instances.

param *path* - Target database path which float data will be set.

param *floatValue* - Float value to set.

return *Boolean* type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].floatData will return the float value of
payload returned from server.

```C++
bool setFloat(FirebaseData &dataObj, const String &path, float floatValue);
```



**Set string (text) at the defined database path.**

param *dataObj* - Firebase Data Object to hold data and instances.

param *path* - Target database path which string data will be set.

param *stringValue* - String or text to set.

return *Boolean* type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].stringData will return the string value of
payload returned from server.

```C++
bool setString(FirebaseData &dataObj, const String &path, const String &stringValue);
```



**Set child nodes's key and value (using JSON data) to the defined database path.**

This will replace any child nodes inside the defined path with node' s key
and value defined in JSON data.

param *dataObj* - Firebase Data Object to hold data and instances.

param *path* - Target database path which key and value in JSON data will be replaced or set.

param *jsonString* - The JSON string to set (should be valid JSON data).

return *Boolean* type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 
 
Call [FirebaseData object].jsonData will return the JSON string value of
payload returned from server.

```C++
bool setJSON(FirebaseData &dataObj, const String &path, const String &jsonString);
```




**Set blob (binary data) at the defined database path.**

This will replace any child nodes inside the defined path with blob or binary data.

param *dataObj* - Firebase Data Object to hold data and instances.

param *path* - Target database path which binary data will be set.

param *blob* - Byte array of data.

param *size* - Size of byte array.

return *Boolean* type status indicates the success of operation.

No payload returned from server.

```C++
bool setBlob(FirebaseData &dataObj, const String &path, uint8_t *blob, size_t size);
```



**Set binary data from file stores on SD card to the defined database path.**

param *dataObj* - Firebase Data Object to hold data and instances.

param *path* - Target database path which binary data from file will be set.

param *filename* - File name (full file path) in SD card.

return *Boolean* type status indicates the success of operation.

No payload returned from server.

```C++
bool setFile(FirebaseData &dataObj, const String &path, const String &fileName);
```




**Update child nodes's key or exising key's value (using JSON data) under the defined database path.**

param *dataObj* - Firebase Data Object to hold data and instances.

param *path* - Target database path which key and value in JSON data will be update.

param *jsonString* - The JSON string use for update.

return *Boolean* type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 
 
Call [FirebaseData object].jsonData will return the json string value of
payload returned from server.

To reduce the network data usage, use updateNodeSilent instead.

```C++
bool updateNode(FirebaseData &dataObj, const String &path, const String &jsonString);
```



**Update child nodes's key or exising key's value (using JSON data) under the defined database path.**

param *dataObj* - Firebase Data Object to hold data and instances.

param *path* - Target database path which key and value in JSON data will be update.

param *jsonString* - The JSON string use for update.

return *Boolean* type status indicates the success of operation.

Owing to the objective of this function to reduce the netwok data usage, 
no payload will be returned from server.

```C++
bool updateNodeSilent(FirebaseData &dataObj, const String &path, const String &jsonString);
```




**Read the integer value at the defined database path.**

param *dataObj* - Firebase Data Object to hold data and instances.

param *path* - Database path which the integer value is being read.

return *Boolean* type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].intData will return the integer value of
payload returned from server.

If the payload returned from server is not integer or float type, 
the function [FirebaseData object].intData will return zero (0).

If the payload returned from server is float type, 
the function [FirebaseData object].intData will return rounded integer value.

```C++
bool getInt(FirebaseData &dataObj, String &path);
```



**Read the float value at the defined database path.**

param *dataObj* - Firebase Data Object to hold data and instances.

param *path* - Database path which the float value is being read.

return *Boolean* type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].floatData will return the float value of
payload returned from server.

If the payload returned from server is not integer or float type, 
the function [FirebaseData object].intData will return zero (0).

```C++
bool getFloat(FirebaseData &dataObj, String &path);
```



**Read the string or text at the defined database path.**

param *dataObj* - Firebase Data Object to hold data and instances.

param *path* - Database path which the string value is being read.

return *Boolean* type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].stringData will return the string value of
payload returned from server.

If the payload returned from server is not string type, 
the function [FirebaseData object].stringData will return empty string (String object).

```C++
bool getString(FirebaseData &dataObj, String &path);
```



**Read the JSON string at the defined database path.**

The returned payload JSON string represents the child nodes and their value.

param *dataObj* - Firebase Data Object to hold data and instances.

param *path* - Database path which the string value is being read.

return *Boolean* type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].jsonData will return the JSON string value of
payload returned from server.

If the payload returned from server is not json type, 
the function [FirebaseData object].jsonData will return empty string (String object).

```C++
bool getJSON(FirebaseData &dataObj, String &path);
```




**Read the blob (binary data) at the defined database path.**

param *dataObj* - Firebase Data Object to hold data and instances.

param *path* - Database path which the binary data is being read.

return *Boolean* type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 
    
Call [FirebaseData object].blobData will return the dynamic array of unsigned 8-bit data (i.e. std::vector<uint8_t>) of
payload returned from server.

If the payload returned from server is not blob type, 
the function [FirebaseData object].blobData will return empty array.

```C++
bool getBlob(FirebaseData &dataObj, String &path);
```



**Download file data in database at defined database path and save to SD card.**

The downloaded data will be decoded to binary and save to SD card, then

please make sure that data at the defined database path is file type.

param *dataObj* - Firebase Data Object to hold data and instances.

param *nodePath* - Database path that file data will be downloaded.

param *fileName* - File name (full path) to save in SD card.

return *Boolean* type status indicates the success of operation.

```C++
bool getFile(FirebaseData &dataObj, const String &nodePath, const String &fileName);
```




**Delete all child nodes at the defined database path.**

param *dataObj* - Firebase Data Object to hold data and instances.

param *path* - Database path to be deleted.

return *Boolean* type status indicates the success of operation.*

```C++
bool deleteNode(FirebaseData &dataObj, const String &path);
```



**Start monitoring the value changes at the defined path and its children.**

param *dataObj* - Firebase Data Object to hold data and instances.

param *path* - Database path being monitor.

return *Boolean* type status indicates the success of operation.*

```C++
bool beginStream(FirebaseData &dataObj, const String &path);
```



**Read the stream event data at defined database path.**

Once beginStream was called e.g. in setup(), the readStream function
should call inside the loop function.

param *dataObj* - Firebase Data Object to hold data and instances.

return *Boolean* type status indicates the success of operation.

Using the same Firebase Data object for stream read/monitoring associated 
with read, store, update, delete will break or quit the current stream connection. 
    
The stream will be resumed or reconnected automatically when calling readStream.

```C++
bool readStream(FirebaseData &dataObj);
```



**End the stream connection at defined path.**

Can be restart again by calling beginStream.

param *dataObj* - Firebase Data Object to hold data and instances.

return *Boolean* type status indicates the success of operation.
 
```C++
bool endStream(FirebaseData &dataObj);
```



**Set the stream callback functions.**

setStreamCallback should be called before Firebase.beginStream.

param *dataObj* - Firebase Data Object to hold data and instances.

param *dataAvailablecallback* - Callback function that accepts streamData parameter.

param *timeoutCallback* - Callback function will be called when stream connection was timeout (optional).

dataAvailablecallback will be called When data in the defined path changed or the stream path changed or stream connection
was resumed from read, store, update and deleteNode.

The payload returned from server will be one of these integer, float, string, json and blob types.

Call [streamData object].dataType to determine what type of data that successfully
stores in database. 
    
Call [streamData object].xxxData will return the appropiated data type of
payload returned from server.

```C++
void setStreamCallback(FirebaseData &dataObj, StreamEventCallback dataAvailablecallback, StreamTimeoutCallback timeoutCallback = NULL);
```



**Remove stream callback functions.**

param *dataObj* - Firebase Data Object to hold data and instances.

```C++
void removeStreamCallback(FirebaseData &dataObj);
```



**Backup (download) database at defined database path to SD card.**

param *dataObj* - Firebase Data Object to hold data and instances.

param *nodePath* - Database path to be backuped.

param *dirPath* - Folder in SD card to save the downloaed file.

return *Boolean* type status indicates the success of operation.

The backup .json filename is constructed from the database path by replace slash (/) with dot (.).

```C++
bool backup(FirebaseData &dataObj, const String &nodePath, const String &dirPath);
```



**Restore database at defined path usin backup file saved on SD card.**

param *dataObj* - Firebase Data Object to hold data and instances.

param *nodePath* - Database path to  be restored.

param *dirPath* - Path/Folder in SD card that the backup file was saved.

return *Boolean* type status indicates the success of operation.

```C++
bool restore(FirebaseData &dataObj, const String &nodePath, const String &dirPath);
```


__Firebase Data object functions__

**Pause/Unpause WiFiClient from all Firebase operations.**

param *pause* True for pause and False for unpause.

return *Boolean* type status indicates the success of operation.

```C++
bool pauseFirebase(bool pause);
```



**Determine the data type of payload returned from server.**

return *The one of these data type e.g. integer, float, string, json and blob.*

```C++
String dataType();
```



**Determine the current stream path.**

return *The database streaming path.*

```C++
String streamPath();
```



**Determine the current data path.**

return *The database path which belong to server's returned payload.*

The database path returned from this function in case of stream, also changed up on the child or parent's stream
value changes.

```C++
String dataPath();
```



**Determine the error reason String from process.**

return *The error description string (String object).*

```C++
String errorReason();
```



**Return the ineger data of server returned payload.**

return *Integer value.*

```C++
int intData();
```



**Return the float data of server returned payload.**

return *Float value.*

```C++
float floatData();
```



**Return the String data of server returned payload.**

return *String (String object).*

```C++
String stringData();
```



**Return the JSON String data of server returned payload.**

return *String (String object).*

```C++
String jsonData();
```




**Return the blob data (uint8_t) array of server returned payload.**

return *Dynamic array* of 8-bit unsigned integer i.e. `std::vector<uint8_t>`.

```C++
std::vector<uint8_t> blobData();
```




**Return the new appended node's name or key of server returned payload when calling pushXXX function.**

return *String (String object).*

```C++
String pushName();
```



**Determine the stream connection status.**

return *Boolean* type status indicates whether the Firebase Data object is working with stream or not.

```C++
bool isStream();
```



**Determine the server connection status.**

return *Boolean* type status indicates whether the Firebase Data object is connected to server or not.

```C++
bool httpConnected();
```



**Determine the timeout event of server's stream (30 sec is default).**

Nothing to do when stream connection timeout, the stream connection will be automatic resumed.

return *Boolean* type status indicates whether the stream was timeout or not.

```C++
bool streamTimeout();
```



**Determine the availability of data or paylaod returned from server.**

return *Boolean* type status indicates whether the server return back the new payload or not.

```C++
bool dataAvailable();
```



**Determine the availability of stream event-data paylaod returned from server.**

return *Boolean* type status indicates whether the server return back the stream event-data 
payload or not.

```C++
bool streamAvailable();
```



**Determine the matching between data type that intend to get from/store to database and the server's return payload data type.**

return *Boolean* type status indicates whether the type of data being get from/store to database 
and server's returned payload are matched or not.

```C++
bool mismatchDataType();
```



**Determine the http status code return from server.**

return *integer* number of HTTP status.

```C++
int httpCode();
```



**Determine the name (full path) of backup file in SD card.**

return *String* (String object) of file name that store on SD card after backup operation.

```C++
String getBackupFilename();
```



**Determine the size of backup file.**

return *Size* of backup file in byte after backup operation.

```C++
size_t getBackupFileSize();
```



**Clear or empty data in Firebase Data object.**

```C++
void clear();
```



**Determine the error description for file transfering (pushFile, setFile, backup and restore).**

return *Error description string* (String object).

```C++
String fileTransferError();
```



**Return the server's payload data.**

return *Payload string* (String object).

```C++
String payload();
```


## Installing

Click on **Clone or download** dropdown at the top of repository, select **Download ZIP** and save file on your computer.

From Arduino IDE, goto menu **Sketch** -> **Include Library** -> **Add .ZIP Library...** and choose **Firebase-ESP32-master.zip** that previously downloaded.

Go to menu **Files** -> **Examples** -> **Firebase-ESP32-master** and choose one from examples


## To do


* Add support to upload/download backup data to/from Google cloud storage (in the same Firebase project bucket).

* Add support to Cloud messaging

