# Firebase Realtime Database Arduino Library for ESP32


Google's Firebase Realtime Database Arduino Library for ESP32 v 3.1.4


This library supports ESP32 MCU from Espressif. The following are platforms which library are also available.

* [ESP8266 Firebase Arduino library]( https://github.com/mobizt/Firebase-ESP8266)

* [Arduino MKR WiFi 1010, Arduino MKR VIDOR 4000 and Arduino UNO WiFi Rev.2](https://github.com/mobizt/Firebase-Arduino-WiFiNINA)

* [Arduino WiFi Shield 101 and Arduino MKR1000 WIFI](https://github.com/mobizt/Firebase-Arduino-WiFi101)




## Tested Devices

 * Sparkfun ESP32 Thing
 * NodeMCU-32
 * WEMOS LOLIN32
 * TTGO T8 V1.8


## Features


* **Not Required Fingerprint and Certificate, CA certificate can be set for more secure connection.**

* **Completed Google REST APIs Implementation.**

* **No Delay for Contiuous Read and Store Data.**

* **Supports Read (get), Store (set), Append (push), Patch (update) and Delete Data**

* **Supports Primitive data types: Integer, Float, Double, Boolean, String and JSON.**

* **Supports BLOB and File Stream Data.**

* **Support Read and Write Database Rules.**

* **Supports Conditional Requests (ETag).**

* **Supports Set and Get Data Priority.**

* **Supports Read and Store Data Limits.**

* **Supports Set and Push Server's Timestamp.**

* **Supports Classic HTTP requests Override.**

* **Supports Error Retry and Restorable Queues.** 

* **Supports Query or Data Filtering.**

* **Supports Automatic Stream Resuming.** 

* **Supports Multiple Streamings.**

* **Supports Stream Event Callbacks**

* **Supports Pause and Resume Firebase Operations.**

* **Supports Data Backup and Restore.**

* **Supports Firebase Cloud Messaging.**

* **Supports SD and SPIFFS's CA certificate file.**



## Dependencies


This library required **ESP32 Core SDK version 1.0.1 and above**.

For Arduino IDE, ESP32 Core SDK can be installed through **Boards Manager**. 

For PlatfoemIO IDE, ESP32 Core SDK can be installed through **PIO Home** > **Platforms** > **Espressif 32**.


This library also required [HTTPClientESP32Ex library](https://github.com/mobizt/HTTPClientESP32Ex) to be installed. 

The HTTPClientESP32Ex library was the customized HTTPClient wrapper used for making SSL connection.






## Installation



### Using Library Manager


At Arduino IDE goto menu **Sketch** -> **Include Library** -> **Manage Libraries...**


In Library Manager Window, search **"firebase"** in the search form then select **"Firebase ESP32 Client"** 

Click **"Install"** button.



For PlatformIO IDE, using the following command.

**pio lib install "Firebase ESP32 Client"**


Or at **PIO Home** -> **Library** -> **Registry** then search **Firebase ESP32 Client**.


[More on PlatformIO...](https://platformio.org/lib/show/6217/Firebase%20ESP32%20Client/examples)




### Manual installing


For Arduino IDE, click on **Clone or download** dropdown at the top of repository, select **Download ZIP**.


From Arduino IDE, goto menu **Sketch** -> **Include Library** -> **Add .ZIP Library...**

Choose **Firebase-ESP32-master.zip** that previously downloaded.


Go to menu **Files** -> **Examples** -> **Firebase-ESP32-master** and choose one from examples.


For PlatformIO, create folder **"Firebase-ESP32"** in folder **"lib"** and store **[these files](https://github.com/mobizt/Firebase-ESP32/tree/master/src)** in there.



### Dependency Library Installing (for older version, 3.1.3 and below)

For library version below than 3.1.4, it relies on [HTTPClientESP32Ex library](https://github.com/mobizt/HTTPClientESP32Ex) which needs to install manually.

Go to [HTTPClientESP32Ex Github repository](https://github.com/mobizt/HTTPClientESP32Ex)

Click on **Clone or download** dropdown at the top of repository, select **Download ZIP** and save file on your computer.

For Arduino IDE, goto menu **Sketch** -> **Include Library** -> **Add .ZIP Library...** 

Choose **HTTPClientESP32Ex-master.zip** that previously downloaded.


For PlatformIO IDE, create folder **"HTTPClientESP32Ex"** in folder **"lib"** 

Store **HTTPClientESP32Ex.h** and **HTTPClientESP32Ex.cpp** from **HTTPClientESP32Ex-master.zip** in that folder.







## Usages


### Initialization


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

//6. Optional, set number of auto retry when read/store data
 Firebase.setMaxRetry(firebaseData, 3);

//7. Optional, set number of read/store data error can be added to queue collection
Firebase.setMaxErrorQueue(firebaseData, 30);

//8. Optional, use classic HTTP GET and POST requests.
//This option allows get and delete functions (PUT and DELETE HTTP requests) works for 
//device connected behind the Firewall that allows only GET and POST requests.  
Firebase.enableClassicRequest(firebaseData, true);
```


### Read Data


Various types of data can be read through Firebase's get functions.

These functions are `getInt`, `getFlot`, `getDouble`, `getBool`, `getString`, `getJSON`, `getBlob` and `getFile`


These functions return boolean indicates the success of operation which will be `true` if all of the following conditions matched.

* Server returns HTTP status 200

* The data types matched between request and response.



The database data's payload (response) can be read through the following Firebase Data object's functions.

* `firebaseData.intData`

* `firebaseData.floatData`

* `firebaseData.doubleData`

* `firebaseData.boolData`

* `firebaseData.stringData`

* `firebaseData.jsonData` and 

* `firebaseData.blobData`


Read the data which its type is not match the data type in database from above functions will return empty string or zero.


The data type of returned payload data can be read through `firebaseData.getDataType`.


BLOB and file stream data are store as special base64 encode string which only supported and implemented by this library.


Thhe encoded string will be prefixed with some header string ("file,base64," and "blob,base64,") for data type manipulation. 


The following example showed how to read integer value from "/test/int".


```C++
  if (Firebase.getInt(firebaseData, "/test/int")) {

    if (firebaseData.dataType() == "int")) {
      Serial.println(firebaseData.intData());
    }

  } else {
    Serial.println(firebaseData.errorReason());
  }
```



### Store Data

Various types of data can be store through Firebase's set functions.

These functions are `setInt`, `setFlot`, `setDouble`, `setBool`, `setString`, `setJSON`, `setBlob` and `setFile`.


These functions return boolean indicates the success of operation which will be `true` if all of the following conditions matched.

* Server returns HTTP status 200

* The data types matched between request and response.


Only setBlob and setFile functions that make a silent request to Firebase server, thus no payload response returned. 

The **priority**, virtual node **".priority"** of each database node can be set through Firebase's set functions.

The priority value can be used in query or filtering the children data under defined database path.

**ETag** (unique identifier value) assigned to Firebase's set functions is used as conditional checking.

If defined Etag is not match the defined path's ETag, the set operation will fail with result **412 Precondition Failed**.

ETag at any database path can be read through `Firebase.getETag`.  ETag value changed upon the data was set or delete.

The server's **Timestamp** can be store in database through `Firebase.setTimestamp`. 

The returned **Timestamp** value can get from `firebaseData.getInt()`. 

The following example showed how to store file data to "/test/file_data".


```C++
if (Firebase.setFile(firebaseData, "/test/file_data", "/test.txt"))
{
  File file = SD.open("/test.txt", FILE_READ);

  while (file.available())
  {     
    Serial.print(file.read(), HEX);     
  }    
  file.close();
  Serial.println();

} else {
  Serial.println(firebaseData.fileTransferError());
}
```



### Append Data

Various types of data can be appened through Firebase's push functions.

These functions are `pushInt`, `pushFlot`, `pushDouble`, `pushBool`, `pushString`, `pushJSON`, `pushBlob` and `pushFile`.

These functions return boolean indicates the success of operation.

The **unique key** of new appended node can get through `firebaseData.pushName`.

As get functions, the Firebase's push functions support **priority**.

**ETag** was not available after push unless read **ETag** at that new unique key later through `Firebase.getETag`.

The server's **Timestamp** can be append in database through `Firebase.pushTimestamp`.

The unique key of Timestamp was available after push the Timestamp.


The following example showed how to append new data (using JSON) to "/test/append.


```C++
String jsonData = "{\"parent_001\":\"parent 001 text\", \"parent 002\":{\"child_of_002\":123.456}}";

if (Firebase.pushJSON(firebaseData, "/test/append", jsonData)) {

  Serial.println(firebaseData.dataPath());

  Serial.println(firebaseData.pushName());

  Serial.println(firebaseData.dataPath() + "/"+ firebaseData.pushName());

} else {
  Serial.println(firebaseData.errorReason());
}
```



### Patch Data

Firebase's update functions used to pach or update new or existing database path.

These functions, `updateNode` and `updateNodeSilent` are available and work with JSON object (string)

If any key provided in JSON object was not existed at defined database path, new key will be created.

The server returns JSON data payload which was successfully patched.

Return of large JSON payload will cost the network data, use `updateNodeSilent` instead to save the network data.


The following example showed how to patch data at "/test".


```C++
String updateData = "{\"data1\":\"value1\", \"data2\":{\"_data2\":\"_value2\"}}";

if (Firebase.updateNode(firebaseData, "/test/update", updateData)) {

  Serial.println(firebaseData.dataPath());

  Serial.println(firebaseData.dataType());

  Serial.println(firebaseData.jsonData()); 

} else {
  Serial.println(firebaseData.errorReason());
}
```





### Delete Data


The following example showed how to delete data and its childs at "/test/append"

```C++
Firebase.deleteNode(firebaseData, "/test/append");
```





### Filtering Data

The quey parameters that can be set through the QueryFilter class.

These parameters are `orderBy`, `limitToFirst`, `limitToLast`, `startAt`, `endAt`, and `equalTo`.

To filter data, parameter `orderBy` should be assigned.

Use **"$key"** as `orderBy` parameter if the key of child nodes was used for query.

Use **"$value"** as `orderBy` parameter if the value of child nodes was used for query.

Use **key (or full path) of child nodes** as `orderBy` parameter if all values of specific key were used for query.

Use **"$priority"** as `orderBy` parameter if child nodes's **"priority"** was used for query.



The above `orderBy` parameter can be combined with the following parameters for limited and ranged the queries.

`QueryFilter.limitToFirst` -  The total children (number) to filter from the first child.

`QueryFilter.limitToLast` -   The total last children (number) to filter. 

`QueryFilter.startAt` -       Starting value of range (number or string) of query upon orderBy param.

`QueryFilter.endAt` -         Ending value of range (number or string) of query upon orderBy param.

`QueryFilter.equalTo` -       Value (number or string) matches the orderBy param



The following example showed how to using queries parameter in QueryFilter class to filter the data at "/test/data"

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



### Stream Data



This library uses HTTP GET request with stream header to request the stream event and data at defined database path.

The Firebase's functions that handle the stream are `beginStream`, `setStreamCallback` and/or `readStream`.

Function `beginStream` used to subscribe the stream changes at defined database path.

Function `setStreamCallback` used to assign the callback function that accept the **StreamData** class as parameter.

The **StreamData** contains stream event and data and interface function calls are similar to Firebase Data object.

To check the stream manually, use function `readStream`.

Function `readStream` used in loop() task to continuous read the stream changes event and data.

After `readStream`, determine the availability of stream with Firebase Data object function `firebaseData.streamAvailable` 

Function `firebaseData.streamAvailable` returned true when new stream data was available. 

After new stream data was available, it can be accessed with the following Firebase Data object functions.

* `firebaseData.intData`

* `firebaseData.floatData`

* `firebaseData.doubleData`

* `firebaseData.boolData`

* `firebaseData.stringData`

* `firebaseData.jsonData` and 

* `firebaseData.blobData`


Function `endStream` ends the stream operation.

The following example showed how to subscribe the stream changes at "/test/data" with callback function.

```C++

//In setup(), set the stream callback function to handle data
//streamCallback is the function that called when database data changes or updates occurred
//streamTimeoutCallback is the function that called when connection between server 
//and client was timeout during HTTP stream

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
    Serial.println(data.floatData(), 5);
  else if (data.dataType() == "double")
    printf("%.9lf\n", data.doubleData());
  else if (data.dataType() == "boolean")
    Serial.println(data.boolData() == 1 ? "true" : "false");
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

The following example showed how to subscribe the stream changes at "/test/data" and read the stream manually.

```C++
//In setup(), set the streaming path to "/test/data" and begin stream connection
if (!Firebase.beginStream(firebaseData, "/test/data"))
{
  Serial.println(firebaseData.errorReason());
}

//In loop()
if (!Firebase.readStream(firebaseData))
{
  Serial.println(firebaseData.errorReason());
}

if (firebaseData.streamTimeout())
{
  Serial.println("Stream timeout, resume streaming...");
  Serial.println();
}

if (firebaseData.streamAvailable())
{

  if (firebaseData.dataType() == "int")
    Serial.println(firebaseData.intData());
  else if (firebaseData.dataType() == "float")
    Serial.println(firebaseData.floatData(), 5);
  else if (firebaseData.dataType() == "double")
    printf("%.9lf\n", firebaseData.doubleData());
  else if (firebaseData.dataType() == "boolean")
    Serial.println(firebaseData.boolData() == 1 ? "true" : "false");
  else if (firebaseData.dataType() == "string")
    Serial.println(firebaseData.stringData());
  else if (firebaseData.dataType() == "json")
    Serial.println(firebaseData.jsonData());
    
}
```


### Backup and Restore Data

This library allows you to backup and restore database at the definded path.

The backup file will store in SD card.

The backup file is actually JSON text file with .json extension. 

The file name is constructed from the defined database path, e.g. "/mydb.backup.data.json" for "/mydb/backup/data".

The database restoration returned completed status only when Firebase server completely update the data. 

Any failed operation will not affected the database (no updates or changes).


The following example showed how to backup all database at "/" and restore.

```C++
 //Don't need to provide the filename, just the folder on SD card.
 if (!Firebase.backup(firebaseData, "/", "/myBackup"))
 {
   Serial.println(firebaseData.fileTransferError());
 }
 else
 {
   Serial.println(firebaseData.getBackupFilename());
   Serial.println(firebaseData.getBackupFileSize());
  }


  //Begin restore backed dup data back to database
  //Don't need to provide the filename, just the folder on SD card that keep the backup file.

  if (!Firebase.restore(firebaseData, "/", "/myBackup"))
  {
    Serial.println(firebaseData.fileTransferError());
  }
  else
  {
    Serial.println(firebaseData.getBackupFilename());
  }

```


### Database Error Handling

When read store, append and update operations were failed due to buffer overflow and network problems.

These operations can retry and queued after the retry amount was reach maximum retry set in function `setMaxRetry`.

```C++
//set maximum retry amount to 3
 Firebase.setMaxRetry(firebaseData, 3);
```

The function `setMaxErrorQueue` limits the maximum queues in Error Queue collection.

The full of queue collection can be checked through function `isErrorQueueFull`.


```C++
 //set maximum queues to 10
 Firebase.setMaxErrorQueue(firebaseData, 10);

 //determine whether Error Queue collection is full or not
 Firebase.isErrorQueueFull(firebaseData);
```

This library provides two approaches to run or process Error Queues with two functions. 

* `beginAutoRunErrorQueue`
* `processErrorQueue`

The function `beginAutoRunErrorQueue` will run or process queues automatically and can be call once. 

While function `processErrorQueue` will run or process queues and should call inside the loop().

With function `beginAutoRunErrorQueue`, you can assigned callback function that accept **QueueInfo** object as parameter.

Which contains all information about being processed queue, number of remaining queues and Error Queue collection status.

Otherwise, Error Queues can be trackacked manually with the following functions.

Function `getErrorQueueID` will return the unsigned integer presents the id of queue which will keep to use later.

Use `getErrorQueueID` and `isErrorQueueExisted` to check whether this queue id is still existed or not. 

If Error Queue ID is not existed in Error Queues collection, that queue is already done.

The following example showed how to run Error Queues automatically and track the status with callback function.

```C++

//In setup()

//Set the maximum Firebase Error Queues in collection (0 - 255).
//Firebase read/store operation causes by network problems and buffer overflow will be 
//added to Firebase Error Queues ollection.
Firebase.setMaxErrorQueue(firebaseData, 10);

//Begin to run Error Queues in Error Queue collection  
Firebase.beginAutoRunErrorQueue(firebaseData, callback);


//Use to stop the auto run queues
//Firebase.endAutoRunErrorQueue(firebaseData);

void errorQueueCallback (QueueInfo queueinfo){

  if (queueinfo.isQueueFull())
  {
    Serial.println("Queue is full");
  }

  Serial.print("Remaining queues: ");
  Serial.println(queueinfo.totalQueues());

  Serial.print("Being processed queue ID: ");
  Serial.println(queueinfo.currentQueueID());  

  Serial.print("Data type:");
  Serial.println(queueinfo.dataType()); 

  Serial.print("Method: ");
  Serial.println(queueinfo.method());

  Serial.print("Path: ");
  Serial.println(queueinfo.path());

  Serial.println();
}
```

The following example showed how to run Error Queues and track its status manually.

```C++
//In setup()

//Set the maximum Firebase Error Queues in collection (0 - 255).
//Firebase read/store operation causes by network problems and buffer overflow will be added to 
//Firebase Error Queues ollection.
Firebase.setMaxErrorQueue(firebaseData, 10);


//All of the following are in loop()

Firebase.processErrorQueue(firebaseData);

//Detrnine the queue status
if (Firebase.isErrorQueueFull(firebaseData))
{
  Serial.println("Queue is full");
}

//Remaining Error Queues in Error Queue collection
Serial.print("Remaining queues: ");
Serial.println(Firebase.errorQueueCount(firebaseData));

//Assumed that queueID is unsigned integer array of queue that added to Error Queue collection 
//when error and use Firebase.getErrorQueueID to get this Error Queue id.

for (uint8_t i = 0; i < LENGTH_OF_QUEUEID_ARRAY; i++)
{
  Serial.print("Error Queue ");
  Serial.print(queueID[i]);
  if (Firebase.isErrorQueueExisted(firebaseData, queueID[i]))
    Serial.println(" is queuing");
  else
    Serial.println(" is done");
}
Serial.println();
```


Error Queus can be saved as file in SD card or Flash memory with function `saveErrorQueue`.

Error Queues store as file can be restored to Error Queue collection with function `restoreErrorQueue`.

Two types of storage can be assigned with these functions, `QueueStorageType::SPIFFS` and `QueueStorageType::SD`.

Read data (get) operation is not support queues restore

The following example showed how to restore and save Error Queues in /test.txt file.

```C++
//To restore Error Queues

if (Firebase.errorQueueCount(firebaseData, "/test.txt", QueueStorageType::SPIFFS) > 0)
{
    Firebase.restoreErrorQueue(firebaseData, "/test.txt", QueueStorageType::SPIFFS);
    Firebase.deleteStorageFile("/test.txt", QueueStorageType::SPIFFS);
}

//To save Error Queues to file
Firebase.saveErrorQueue(firebaseData, "/test.txt", QueueStorageType::SPIFFS);

```


## Firebase Cloud Messaging (FCM)

There are two types of FCM message data that can be sent using this library e.g. **notification** and **custom data**.

These two types data can send all together or separately.

Function `Firebase.sendMessage` will send message to one recipient.

Function `Firebase.broadcastMessage` will broadcast or send message to multiple recipients.  

Function `Firebase.sendTopic` will send message to any recipient who subscribed to the topic.

The FCM message itself offers broad range of messaging options and capabilities for various recipient device platforms. 

For Android, iOS and web platforms, these basic options can be set and work for all platforms. 


Function `firebaseData.fcm.begin` used to assign server key of your Firebase project.

Function `firebaseData.fcm.addDeviceToken` used to add recipient registered device token which want to send message to. 

Functions `firebaseData.fcm.removeDeviceToken` and `firebaseData.fcm.clearDeviceToken` used to remove or clear recipient device.


For the notification message, title, body, icon (optional), and click_action (optional) can be set through `firebaseData.fcm.setNotifyMessage`. 

And clear these notify message data with `firebaseData.fcm.clearNotifyMessage`.

For the data message, provide your custom data as JSON object (string) to `firebaseData.fcm.setDataMessage` which can be clear with `firebaseData.fcm.clearDataMessage`.

The other options are `priority`, `collapse key`, `Time to Live` of message and `topic` to send message to, can be set from the following functions.

Call `firebaseData.fcm.setPriority` for priority ("normal" or "high"), `firebaseData.fcm.setCollapseKey` for collapse key setup, `firebaseData.fcm.setTimeToLive` for life span of message setup between 0 sec. to 2,419,200 sec.  (or 4 weeks), and `firebaseData.fcm.setTopic` for assigning the topic that message to send to.


The following example showed how to send FCM message.

```C++
//Provide your Firebase project's server key here
firebaseData.fcm.begin(FIREBASE_FCM_SERVER_KEY);

//Prvide one or more the recipient registered token or instant ID token
firebaseData.fcm.addDeviceToken(FIREBASE_FCM_DEVICE_TOKEN);

//Provide the priority (optional)
firebaseData.fcm.setPriority("normal");

//Provide the time to live (optional)
firebaseData.fcm.setTimeToLive(5000);

//Set the notification message data
firebaseData.fcm.setNotifyMessage("Notification", "Hello World!", "firebase-logo.png", "http://www.google.com");

//Set the custom message data
firebaseData.fcm.setDataMessage("{\"myData\":\"myValue\"}");

//Send message to one recipient with inddex 1 (index starts from 0)
if (Firebase.sendMessage(firebaseData, 1))
{
  //Success, print the result returned from server
  Serial.println(firebaseData.fcm.getSendResult());
}
else
{
  //Failed, print the error reason
  Serial.println(firebaseData.errorReason());
}
```


See [Full Examples](/examples) for complete usages.

See [Function Description](/src/README.md) for all available functions.


## License

The MIT License (MIT)

Copyright (c) 2019 K. Suwatchai (Mobizt)


Permission is hereby granted, free of charge, to any person returning a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

