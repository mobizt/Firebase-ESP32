# Firebase Realtime Database Arduino Client Library for ESP32


Google's Firebase Realtime Database Arduino Library for ESP32 v 2.3.7

This client library provides the most reliable operations for read, store, update, delete, backup and restore the database data.

In addition, the library allows you to read and store binary data from/to device's memory or external SD memory.

This library comunicated with Google Firebase Realtime Database using the REST API. 

This library supports all ESP32 MCU based modules. For ESP8266 please try [ESP8266 Firebase Arduino library]( https://github.com/mobizt/Firebase-ESP8266)


Copyright (c) 2019 K. Suwatchai (Mobizt).







## Tested Devices

This following devices were tested and work well.

 * Sparkfun ESP32 Thing
 * NodeMCU-32
 * WEMOS LOLIN32

 





 
## Features

* **Not required fingerprint** or **certificate data** to connect.

* Using **Firebase Data object** that holds all data and instances.

* **Read data** at the defined database path using get functions e.g. **getInt**, **getFloat**, **getBool**, **getString**, **getJSON**, **getBlob** and **getFile**.

* **Store data** at the defined database path using set functions e.g. **setInt**, **setFloat**, **setBool**, **setString**, **setJSON**, **setBlob** and **setFile**.

* **Append data** to the defined database path using push functions e.g. **pushInt**, **pushFloat**, **pushBool**,  **pushString**, **pushJSON**, **pushBlob** and **pushFile**.

* **Update data** at the defined database path using **updateNode** and **updateNodeSilent** functions.

* **Delete data** at the defined database path (include all child nodes) using **deleteNode** function.

* **Read and write database rules** using **getRules** and **setRules** functions.

* **Quit and release all resources** which belong to Firbase data using **end** function.

* Supports **Data Filtering** using the orderBy, limitToFirst, limitToLast, startAt, endAt, and equalTo query parameters.

* Supports integer, float, boolean, string and JSON string data types.   

  For JSON string data type, parsing as an object required external JSON parser library e.g. [**ArduinoJson**](https://github.com/bblanchon/ArduinoJson).

* Supports **automatic stream resuming** when the stream connection was timeout. 

* Supports **multiple streaming paths** using multiple Firebase Data objects. 

* Supports set and remove **stream event callbacks**.

* Supports **Firebase pause**.

* Supports data **backup and restore** that working with SD card. 

* Supports **blob** or binary data from memory and **File stream** from SD card.







## Dependencies


This library required [HTTPClientESP32Ex library](https://github.com/mobizt/HTTPClientESP32Ex) to be installed. The HTTPClientESP32Ex library was the customized HTTPClient wrapper used for making SSL connection.






## Installing

This library relies on [HTTPClientESP32Ex library](https://github.com/mobizt/HTTPClientESP32Ex) which needs to install it manually as it is the helper library and not available from Library Manager.


### Using Library Manager


At Arduino IDE goto menu **Sketch** -> **Include Library** -> **Manage Libraries...**


In Library Manager Window, search **"firebase"** in the search form then select **"Firebase ESP32 Client"** and click **"Install"** button.



For PlatformIO IDE, using the following command.

**pio lib install "Firebase ESP32 Client"**


Or at **PIO Home** -> **Library** -> **Registry** then search **Firebase ESP32 Client**.


[More on PlatformIO...](https://platformio.org/lib/show/6217/Firebase%20ESP32%20Client/examples)




### Manual installing


For Arduino IDE, click on **Clone or download** dropdown at the top of repository, select **Download ZIP** and save file on your computer.


From Arduino IDE, goto menu **Sketch** -> **Include Library** -> **Add .ZIP Library...** and choose **Firebase-ESP32-master.zip** that previously downloaded.


Go to menu **Files** -> **Examples** -> **Firebase-ESP32-master** and choose one from examples.


For PlatformIO, create folder **"Firebase-ESP32"** in folder **"lib"** and store **[these two files](https://github.com/mobizt/Firebase-ESP32/tree/master/src)** in it.



### Dependent Library Installing

Go to [HTTPClientESP32Ex Github repository](https://github.com/mobizt/HTTPClientESP32Ex) and click on **Clone or download** dropdown at the top of repository, select **Download ZIP** and save file on your computer.

For Arduino IDE, goto menu **Sketch** -> **Include Library** -> **Add .ZIP Library...** and choose **HTTPClientESP32Ex-master.zip** that previously downloaded.


For PlatformIO IDE, create folder **"HTTPClientESP32Ex"** in folder **"lib"** and store **HTTPClientESP32Ex.h** and **HTTPClientESP32Ex.cpp** which extracted from **HTTPClientESP32Ex-master.zip** in created folder.






## Usages


### Declaration and Initialization


#### The first thing to do to use this library.

```C++

//1. Download  HTTPClientESP32Ex library above and add to Arduino library

//2. Include Firebase ESP32 library (this library)

#include "FirebaseESP32.h"

//3. Declare the Firebase Data object in global scope

FirebaseData firebaseData;

//4. Setup Firebase credential in setup()

Firebase.begin("yout_project_id.firebaseio.com", "your_Firebase_database_secret");

//5. Optional, set AP reconnection in setup()

Firebase.reconnectWiFi(true);

```

___


### Read, Store, Update, Delete, Backup and Restore Data


**To read the data, use `get<Data Type>` functions i.e. getInt, getFlot, getBool, getString, getJSON, getBlob and getFile.**



The `get<Data Type>` function returned boolean value for success of operation. The success of operation determined from

payload that Firebase server returned back to client. The http status and matching between data type request and response were determined to set the success status.

To read the payload data, one of theses functions can be called i.e. intData, floatData, boolData, stringData, jsonData and blobData.

The data you read from returned payload will tell actual data type stored or existed in database, not the modification version data type e.g. string "1.5" stored in database, can be read only from stringData as it returned from server.

Any attempt to read integer from intData, float from floatData, and JSON from JsonData will return zero and empty string. 

This allow you to know what exactly type of data stored in database and how to manage it instead of cast all data as string (int -> string or float -> string) or as number. 

You can call getDataType to determine what type of data returned to be manipulated.

BLOB and file stream daya types were only implemented by this library.

Normally BLOB or any binary data type is not supported by Firebase Realtime database, this library working with binary data by encoding the data into string before sending to server.

Then getBlob and getFile functions will read the encoded string from database and decoded it back to binary data before return data to client.

Encoding binary to string in this library is using base64 binary-to-text encoding schemes, the encoded string will be prefixed with some header string ("file,base64," and "blob,base64,") for data type manipulation. 

The encoded string length will larger than the original binary data by 30%.

The terms used in this document and library, Blob data is byte array in memory that encoded to string to store or decoded when read from database.

While file stream is binary data that being write to SD card which obtained from decoded string that read from database or binary data that being read from SD card and encoded to string to store in database.

Then getBlob function reads data in database and decoded it into byte array, while getFile reads data in database, then decoded and save it to SD card.

Here is the example usage to read integer value from defined database path "/test/int".


```C++

//Read integer value from database at "/test/int" 

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



**To store the data, use `set<Data Type>` functions i.e. setInt, setFlot, setBool, setString, setJSON, setBlob and setFile.**



The `set<Data Type>` function returned boolean value for success of operation. The success of operation determined from

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


**To append new data to database, `push<Data Type>` should be called e.g. pushInt, pushFloat, pushBool, pushString, pushJSON, pushBlob and pushFile.**


With push operation, server will return payload (key or name of newly appended node) to client.

Working with JSON data allow us to read or store multiple data at once because JSON data can store many key/value pairs, array of object and nested objects.

Function setJSON will set/replace value at defined database path with value in JSON data, and also create child nodes.  

While in function pushJSON, all key/value in JSON data  will be appended to the defined database path as new node.

Below is the example for appending new data (using JSON) to the path "/test/append.


```C++

//Append many data (multiple keys included nest data) to the database at "/test/append"

String jsonData = "{\"parent_001\":\"parent 001 text\", \"parent 002\":{\"child_of_002\":123.456}}";

if (Firebase.pushJSON(firebaseData, "/test/append", jsonData)) {

  //Success, then read the payload value

  //Database path to be appended
  Serial.println(firebaseData.dataPath()); //Should be "/test/append"

  //New created key/name
  Serial.println(firebaseData.pushName());

  //Absolute path of new appended data
    Serial.println(firebaseData.dataPath() + "/"+ firebaseData.pushName());


} else {
  //Failed, then print out the error detail
  Serial.println(firebaseData.errorReason()));
}

```



**To update database at defined path and its child nodes, updateNode or updateNodeSilent should be called.**

JSON data is needed, and passed to these functions. The result from update operation, the database data at defined path will be partialy or completed update depending on the keys in JSON data. 

If any key provided in JSON data was not existed in database at the path, new key will be created.

Everytime you call updateNode, the payload that exactly the JSON data you sent will return back from server. 

Update database with large JSON will consume as much as double network data. Function updateNodeSilent can reduce the network data usage in this case.

Below is the example for database update at "/test" using JSON data.


```C++

//Append many data (multiple keys included nest data) to the database at "/test/update"

String updateData = "{\"data1\":\"value1\", \"data2\":{\"_data2\":\"_value2\"}}";

if (Firebase.updateNode(firebaseData, "/test/update", updateData)) {

  //Success, then try to read the payload value

  //Database path that updated
  Serial.println(firebaseData.dataPath());

  //Data type at updated database path
  Serial.println(firebaseData.dataType()); //Should be "json"

  //Print the JSON string payload that returned from server
  Serial.println(firebaseData.jsonData()); //Should mathes the value in updateData variable

  //Actual sent payload JSON data
  Serial.println(updateData);

} else {
  //Failed, then print out the error detail
  Serial.println(firebaseData.errorReason()));
}

```



**To delete the data in database, just call deleteNode function.**

Below example will delete data and its child nodes at "/test/append"

```C++

Firebase.deleteNode(firebaseData, "/test/append");

```

**Data Filtering via query parameters**

The quey parameters that can set using the QueryFilter class.

These parameters are `orderBy`, `limitToFirst`, `limitToLast`, `startAt`, `endAt`, and `equalTo`.

To filter data, parameter `orderBy` should be assigned which you can assign "$key" for filtering all nodes under defined database path
using their keys, assign "$value" for filtering all nodes under defined database path using their values. 

Assign specific node key or name for filtering the specified child node under defined database path using their key, and assign "$priority" for filtering all nodes under defined database path using their "virtual child" named .priority.

And using the follower query properties to limit the queries.

`QueryFilter.limitToFirst` -  The total children (number) to filter from the first child.

`QueryFilter.limitToLast` -   The total last children (number) to filter. 

`QueryFilter.startAt` -       Starting value of range (number or string) of query upon orderBy param.

`QueryFilter.endAt` -         Ending value of range (number or string) of query upon orderBy param.

`QueryFilter.equalTo` -       Value (number or string) matches the orderBy param



Below example show how to using queries parameter in QueryFilter class to filter the data at database path "/test/data"

```C++

//Assume that children that have key "sensor" are under "/test/data"

//Instantiate the QueryFilter class
QueryFilter query;

//Build query using specified child node key "sensor" under "/test/data"
query.orderBy("sensor");

//Query any child that its value is begin with 2 (number), assumed that its data type is float or integer
query.startAt(2);

//Query any child that its value is end with 8 (number), assumed that its data type is float or integer
query.endAt(8);

//Limit the maximum query result to return only the last 5 nodes
query.limitToLast(5);


if (Firebase.getJSON(firebaseData, "/test/data", query))
{
  //Success, then try to read the JSON payload value
  Serial.println(firebaseData.jsonData());
}
else
{
  //Failed to get JSON data at defined database path, print out the error reason
  Serial.println(firebaseData.errorReason());
}

//Clear all query parameters
query.clear();



```



___


### Data changes monitoring (Stream)



**The Firebase Realtime Database update or change event was sent to client using the HTTP stream connection. The connection was keep alive as long as no network communication interruption.**

The function to handle the update and change event monitoring are beginStream, setStreamCallback and readStream.

To subscribe, monitor or follow the data change on defined database path, the function beginStream must be called first.

To handle the data that will be received when database at that path updates or changes, the function readStream should be called in loop,
otherwise callback function is required and assigned using the function setStreamCallback.

When no callback is assigned, the data that return from stream should be routinely veriy inside the loop function. To determine the stream data is available, function streamAvailable should be called. 

The function streamAvailable returned true when stream data was received in buffer. 

Then stream data can be accessed directly by calling intData, floatData, stringData, jsonData and blobData.

If stream callback function was assigned, it should accept StreamData as parameter which is a class that contains all data from stream, included streamPath, dataPath, dataType and value changes at defined streaming path.

To end the stream, call endStream. 

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
void streamTimeoutCallback(bool timeout)
{
  if(timeout){
    //Stream timeout occurred
    Serial.println("Stream timeout, resume streaming...");
  }  
}

```

___


### Database Backup and Restore




**The backup and restore are very easy, no need to specify the filename, just defined the database path to backup or restore and the folder or path on SD card to save the backup data.**

The downloaded backup file is actually JSON text file with .json extension. The file name is constructed from the defined database path,
e.g. "/root.json" for "/" or "/mydb.backup.data.json" for "/mydb/backup/data".

The database restoration returned completed status only when Firebase server completely update the data. 

Any failed operation will not affected the database (no updates or changes).

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



___


### Quit and release all resources




**To quit and release all resources that used by Firebase Data object included WiFi client, call end function**

After end was called, all resources in Firebase Data object included callbacks and WiFi client will be deleted and released, re-initialize is need when reuse the Firebase Data object.

```C++

 Firebase.end(firebaseData);

```



See [full examples](https://github.com/mobizt/Firebase-ESP32/tree/master/examples) for all features usages.



## All Supported Functions


**These are all functions available from the library and the descriptions.**


### Global functions


**Store Firebase's authentication credentials.**

param *`host`* - Your Firebase database project host without http:// or https:// protocol e.g. Your_ProjectID.firebaseio.com.

param *`auth`* - Your database secret.

```C++
void begin(const String &host, const String &auth);
```







**Store Firebase's authentication credentials.**

param *`host`* - Your Firebase database project host without http:// or https:// protocol e.g. Your_ProjectID.firebaseio.com.

param *`auth`* - Your database secret.

param *`rootCA`* - Base64 encoded root certificate string.

```C++
void begin(const String &host, const String &auth, const char *rootCA);
```






**Stop Firebase and release all resources.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

```C++
void end(FirebaseData &dataObj);
```






**Reconnect WiFi if lost connection.**

param *`reconnect`* - The boolean to set/unset WiFi AP reconnection.

```C++
void reconnectWiFi(bool reconnect);
```







**Read the database rules.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

return - *`Boolean`* type status indicates the success of operation.

```C++
bool getRules(FirebaseData &dataObj);
```







**Write the database rules.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`rules`* - Database rules in jSON String format.

return - *`Boolean`* type status indicates the success of operation.

```C++
bool setRules(FirebaseData &dataObj, const String &rules);
```







**Determine whether defined database path is existed or not.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Database path to be checked.

return - *`Boolean`* type result indicates whether the defined database
path was existed or not.

```C++
bool pathExist(FirebaseData &dataObj, const String &path);
```







**Append new integer value to the defined database path.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Target database path which integer value will be appended.

param *`intValue`* - The appended value.

return *`Boolean`* type status indicates the success of operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushInt(FirebaseData &dataObj, const String &path, int intValue);
```







**Append new float value to the defined database path.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Target database path which float value will be appended.

param *`floatValue`* - The appended value.

return *`Boolean`* type status indicates the success of operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushFloat(FirebaseData &dataObj, const String &path, float floatValue);
```






**Append new Boolean value to the defined database path.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Target database path which Boolean value will be appended.

param *`boolValue`* - The appended value.

return *`Boolean`* type status indicates the success of operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushBool(FirebaseData &dataObj, const String &path, bool boolValue);
```







**Append new string (text) to the defined database path.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Target database path which string will be appended.

param *`stringValue`* - The appended value.

return *`Boolean`* type status indicates the success of operation.

The new appended node's key stored in Firebase Data object, 
which can be accessed via function [FirebaseData object].pushName().

```C++
bool pushString(FirebaseData &dataObj, const String &path, const String &stringValue);
```







**Append new child nodes's key and value (using JSON data) to the defined database path.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Target database path which key and value in JSON data will be appended.

param *`jsonString`* - The appended JSON string (should be valid JSON data).

return *`Boolean`* type status indicates the success of operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushJSON(FirebaseData &dataObj, const String &path, const String &jsonString);
```







**Append new blob (binary data) to the defined database path.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Target database path which binary data will be appended.

param *`blob`* - Byte array of data.

param *`size`* - Size of byte array.

return *`Boolean`* type status indicates the success of operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushBlob(FirebaseData &dataObj, const String &path, uint8_t *blob, size_t size);
```







**Append new binary data from file stores on SD card to the defined database path.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Target database path which binary data from file will be appended.

param *`fileName`* - File name (full file path) in SD card.

return *`Boolean`* type status indicates the success of operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushFile(FirebaseData &dataObj, const String &path, const String &fileName);
```







**Set integer data at the defined database path.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Target database path which integer data will be set.

param *`intValue`* - Integer value to set.

return *`Boolean`* type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 
    
Call [FirebaseData object].intData will return the integer value of
payload returned from server.

```C++
bool setInt(FirebaseData &dataObj, const String &path, int intValue);
```







**Set float data at the defined database path.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Target database path which float data will be set.

param *`floatValue`* - Float value to set.

return *`Boolean`* type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].floatData will return the float value of
payload returned from server.

```C++
bool setFloat(FirebaseData &dataObj, const String &path, float floatValue);
```






**Set Boolean data at the defined database path.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Target database path which float data will be set.

param *`boolValue`* - Boolean value to set.

return *`Boolean`* type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].boolData will return the Boolean value of
payload returned from server.

```C++
bool setBool(FirebaseData &dataObj, const String &path, bool boolValue);
```







**Set string (text) at the defined database path.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Target database path which string data will be set.

param *`stringValue`* - String or text to set.

return *`Boolean`* type status indicates the success of operation.

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

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Target database path which key and value in JSON data will be replaced or set.

param *`jsonString`* - The JSON string to set (should be valid JSON data).

return *`Boolean`* type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 
 
Call [FirebaseData object].jsonData will return the JSON string value of
payload returned from server.

```C++
bool setJSON(FirebaseData &dataObj, const String &path, const String &jsonString);
```







**Set blob (binary data) at the defined database path.**

This will replace any child nodes inside the defined path with blob or binary data.

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Target database path which binary data will be set.

param *`blob`* - Byte array of data.

param *`size`* - Size of byte array.

return *`Boolean`* type status indicates the success of operation.

No payload returned from server.

```C++
bool setBlob(FirebaseData &dataObj, const String &path, uint8_t *blob, size_t size);
```







**Set binary data from file stores on SD card to the defined database path.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Target database path which binary data from file will be set.

param *`fileName`* - File name (full file path) in SD card.

return *`Boolean`* type status indicates the success of operation.

No payload returned from server.

```C++
bool setFile(FirebaseData &dataObj, const String &path, const String &fileName);
```







**Update child nodes's key or exising key's value (using JSON data) under the defined database path.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Target database path which key and value in JSON data will be update.

param *`jsonString`* - The JSON string use for update.

return *`Boolean`* type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 
 
Call [FirebaseData object].jsonData will return the json string value of
payload returned from server.

To reduce the network data usage, use updateNodeSilent instead.

```C++
bool updateNode(FirebaseData &dataObj, const String &path, const String &jsonString);
```







**Update child nodes's key or exising key's value (using JSON data) under the defined database path.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Target database path which key and value in JSON data will be update.

param *`jsonString`* - The JSON string use for update.

return *`Boolean`* type status indicates the success of operation.

Owing to the objective of this function to reduce the netwok data usage, 
no payload will be returned from server.

```C++
bool updateNodeSilent(FirebaseData &dataObj, const String &path, const String &jsonString);
```







**Read the integer value at the defined database path.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Database path which the integer value is being read.

return *`Boolean`* type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].intData will return the integer value of
payload returned from server.

If the payload returned from server is not integer or float type, 
the function [FirebaseData object].intData will return zero (0).

If the payload returned from server is float type, 
the function [FirebaseData object].intData will return rounded integer value.

```C++
bool getInt(FirebaseData &dataObj, const String &path);
```







**Read the float value at the defined database path.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Database path which the float value is being read.

return *`Boolean`* type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].floatData will return the float value of
payload returned from server.

If the payload returned from server is not integer or float type, 
the function [FirebaseData object].intData will return zero (0).

```C++
bool getFloat(FirebaseData &dataObj, const String &path);
```







**Read the Boolean value at the defined database path.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Database path which the Boolean value is being read.

return *`Boolean`* type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].boolData will return the Boolean value of
payload returned from server.

If the payload returned from server is not boolean type, 
the function [FirebaseData object].boolData will return false.

```C++
bool getBool(FirebaseData &dataObj, const String &path);
```







**Read the string or text at the defined database path.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Database path which the string value is being read.

return *`Boolean`* type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].stringData will return the string value of
payload returned from server.

If the payload returned from server is not string type, 
the function [FirebaseData object].stringData will return empty string (String object).

```C++
bool getString(FirebaseData &dataObj, const String &path);
```







**Read the JSON string at the defined database path.**

The returned payload JSON string represents the child nodes and their value.

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Database path which the string value is being read.

return *`Boolean`* type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].jsonData will return the JSON string value of
payload returned from server.

If the payload returned from server is not json type, 
the function [FirebaseData object].jsonData will return empty string (String object).

```C++
bool getJSON(FirebaseData &dataObj, const String &path);
```








**Read the JSON string with data filtering at the defined database path.**

The returned payload JSON string represents the child nodes and their value.

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Database path which the string value is being read.

param *`query`* - QueryFilter class to set query parameters to filter data.

return *`Boolean`* type status indicates the success of operation.

Available query parameters for filtering the data are the following.

*`QueryFilter.orderBy`* - Required parameter to specify which data used for data filtering included child key, key and value.

Use "$key" for filtering data by keys of all nodes at the defined database path.

Use "$value" for filtering data by value of all nodes at the defined database path.

Use "$priority" for filtering data by "virtual child" named .priority of all nodes.

Use any child key to filter by that key.


*`QueryFilter.limitToFirst`* -  The total children (number) to filter from the first child.

*`QueryFilter.limitToLast`* -   The total last children (number) to filter. 

*`QueryFilter.startAt`* -       Starting value of range (number or string) of query upon orderBy param.

*`QueryFilter.endAt`* -         Ending value of range (number or string) of query upon orderBy param.

*`QueryFilter.equalTo`* -       Value (number or string) matches the orderBy param


Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].jsonData will return the JSON string value of
payload returned from server.

If the payload returned from server is not json type, 
the function [FirebaseData object].jsonData will return empty string (String object).

[FirebaseData object].jsonData will return null when the filtered data is empty.

```C++
bool getJSON(FirebaseData &dataObj, const String &path, QueryFilter &quer);
```







**Read the blob (binary data) at the defined database path.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Database path which the binary data is being read.

return *`Boolean`* type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 
    
Call [FirebaseData object].blobData will return the dynamic array of unsigned 8-bit data (i.e. `std::vector<uint8_t>`) of
payload returned from server.

If the payload returned from server is not blob type, 
the function [FirebaseData object].blobData will return empty array.

```C++
bool getBlob(FirebaseData &dataObj, const String &path);
```







**Download file data in database at defined database path and save to SD card.**

The downloaded data will be decoded to binary and save to SD card, then

please make sure that data at the defined database path is file type.

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`nodePath`* - Database path that file data will be downloaded.

param *`fileName`* - File name (full path) to save in SD card.

return *`Boolean`* type status indicates the success of operation.

```C++
bool getFile(FirebaseData &dataObj, const String &nodePath, const String &fileName);
```








**Delete all child nodes at the defined database path.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Database path to be deleted.

return *`Boolean`* type status indicates the success of operation.*

```C++
bool deleteNode(FirebaseData &dataObj, const String &path);
```







**Start monitoring the value changes at the defined path and its children.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`path`* - Database path being monitor.

return *`Boolean`* type status indicates the success of operation.*

```C++
bool beginStream(FirebaseData &dataObj, const String &path);
```







**Read the stream event data at defined database path.**

Once beginStream was called e.g. in setup(), the readStream function
should call inside the loop function.

param *`dataObj`* - Firebase Data Object to hold data and instances.

return *`Boolean`* type status indicates the success of operation.

Using the same Firebase Data object for stream read/monitoring associated 
with read, store, update, delete will break or quit the current stream connection. 
    
The stream will be resumed or reconnected automatically when calling readStream.

```C++
bool readStream(FirebaseData &dataObj);
```







**End the stream connection at defined path.**

Can be restart again by calling beginStream.

param *`dataObj`* - Firebase Data Object to hold data and instances.

return *`Boolean`* type status indicates the success of operation.
 
```C++
bool endStream(FirebaseData &dataObj);
```







**Set the stream callback functions.**

setStreamCallback should be called before Firebase.beginStream.

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`dataAvailablecallback`* - Callback function that accepts streamData parameter.

param *`timeoutCallback`* - Callback function will be called when stream connection was timeout (optional).

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

param *`dataObj`* - Firebase Data Object to hold data and instances.

```C++
void removeStreamCallback(FirebaseData &dataObj);
```







**Backup (download) database at defined database path to SD card.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`nodePath`* - Database path to be backuped.

param *`dirPath`* - Folder in SD card to save the downloaed file.

return *`Boolean`* type status indicates the success of operation.

The backup .json filename is constructed from the database path by replace slash (/) with dot (.).

```C++
bool backup(FirebaseData &dataObj, const String &nodePath, const String &dirPath);
```







**Restore database at defined path usin backup file saved on SD card.**

param *`dataObj`* - Firebase Data Object to hold data and instances.

param *`nodePath`* - Database path to  be restored.

param *`dirPath`* - Path/Folder in SD card that the backup file was saved.

return *`Boolean`* type status indicates the success of operation.

```C++
bool restore(FirebaseData &dataObj, const String &nodePath, const String &dirPath);
```







### Firebase Data object functions

**Pause/Unpause WiFiClient from all Firebase operations.**

param *`pause`* True for pause and False for unpause.

return *`Boolean`* type status indicates the success of operation.

```C++
bool pauseFirebase(bool pause);
```







**Determine the data type of payload returned from server.**

return *The one of these data type e.g. integer, float, string, json and blob.*

```C++
String dataType();
```






**Determine the event type of stream.**

@return The one of these event type String e.g. put, patch, cancel, and auth_revoked.

The event type "put" indicated that data at event path relative to stream path was completely changed. Event path can be determined from dataPath().

The event type "patch" indicated that data at event path relative to stream path was updated. Event path can be determined from dataPath().

The event type "cancel" indeicated something wrong and cancel by server.

The event type "auth_revoked" indicated the provided Firebase Authentication Data (Database secret) is no longer valid.

```C++
String eventType();
```







**Determine the current stream path.**

return *The database streaming path.*

```C++
String streamPath();
```







**Determine the current data path.**

return *The database path which belong to server' s returned payload.*

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





**Return the Boolean data of server returned payload.**

return *Boolean value.*

```C++
float boolData();
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

return *`String`* (String object).

```C++
String pushName();
```







**Determine the stream connection status.**

return *`Boolean`* type status indicates whether the Firebase Data object is working with stream or not.

```C++
bool isStream();
```







**Determine the server connection status.**

return *`Boolean`* type status indicates whether the Firebase Data object is connected to server or not.

```C++
bool httpConnected();
```







**Determine the timeout event of server's stream (30 sec is default).**

Nothing to do when stream connection timeout, the stream connection will be automatic resumed.

return *`Boolean`* type status indicates whether the stream was timeout or not.

```C++
bool streamTimeout();
```







**Determine the availability of data or paylaod returned from server.**

return *`Boolean`* type status indicates whether the server return back the new payload or not.

```C++
bool dataAvailable();
```







**Determine the availability of stream event-data paylaod returned from server.**

return *`Boolean`* type status indicates whether the server return back the stream event-data 
payload or not.

```C++
bool streamAvailable();
```







**Determine the matching between data type that intend to get from/store to database and the server's return payload data type.**

return *`Boolean`* type status indicates whether the type of data being get from/store to database 
and server's returned payload are matched or not.

```C++
bool mismatchDataType();
```







**Determine the http status code return from server.**

return *`Integer`* number of HTTP status.

```C++
int httpCode();
```







**Determine the name (full path) of backup file in SD card.**

return *`String`* (String object) of file name that store on SD card after backup operation.

```C++
String getBackupFilename();
```







**Determine the size of backup file.**

return *`Number of byte`* of backup file in byte after backup operation.

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







## To do


* Add support to upload/download backup data to/from Google cloud storage (in the Firebase projectID Bucket).

* Add support to Cloud messaging


