# Firebase Realtime Database Arduino Library for ESP32

![Compile](https://github.com/mobizt/Firebase-ESP32/actions/workflows/compile_library.yml/badge.svg) ![Examples](https://github.com/mobizt/Firebase-ESP32/actions/workflows/compile_examples.yml/badge.svg) [![Github Stars](https://img.shields.io/github/stars/mobizt/Firebase-ESP32?logo=github)](https://github.com/mobizt/Firebase-ESP32/stargazers) ![Github Issues](https://img.shields.io/github/issues/mobizt/Firebase-ESP32?logo=github)

![arduino-library-badge](https://www.ardu-badge.com/badge/Firebase%20ESP32%20Client.svg) ![PlatformIO](https://badges.registry.platformio.org/packages/mobizt/library/Firebase%20ESP32%20Client.svg) [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.6462672.svg)](https://doi.org/10.5281/zenodo.6462672)


This library provides Firebase Realtime database and Firebase Cloud Messaging functions and supports only ESP32 MCU from Espressif with or without External network module. The ESP8266 version is available [here](https://github.com/mobizt/Firebase-ESP8266).

The features can be configurable to add and exclude some unused features, see [Library Build Options](#library-build-options).

If you use other Arduino devices or you want to use more Firebase services included Cloud Firestore database, Firebase Storage, Google Cloud Storage and Cloud Functions for Firebase, please use [Firebase-ESP-Client](https://github.com/mobizt/Firebase-ESP-Client) library instead.


The RTDB (Realtime database), FCM (Cloud Messaing) and core code used in `Firebase-ESP-Client`, `FirebaseESP8266` and `FirebaseESP32` libraries are the same unless the syntaxes are different. Please see the library examples for usage guidlines.

The `FirebaseESP8266` and `FirebaseESP32` libraries are the old and limited features Firebase client library while `Firebase-ESP-Client` is the newver version of Firebase client library that developed to support more Firebase services.

The new version of new async [FirebaseClient](https://github.com/mobizt/FirebaseClient) library is availavle for you to try.

## Contents

[1. Features](#features)

[2. Supported Devices](#supported-devices)

[3. Dependencies](#dependencies)

[4. Installation](#installation)

- [Using Library Manager](#using-library-manager)

- [Manual installation](#manual-installation)

[5. Usages](#usages)

- [Initialization](#initialization)

[6. Memory Options](#memory-options)

- [Memory Options for ESP32](#memory-options-for-esp32)

- [Arduino IDE](#arduino-ide)

- [PlatformIO IDE](#platformio-ide)

[7. Authentication](#authentication)

- [Access in Test Mode (No Auth)](#access-in-test-mode-no-auth)

- [The authenication credentials and prerequisites](#the-authenication-credentials-and-prerequisites)

[8. Library Build Options](#library-build-options)

- [Predefined Options](#predefined-options)

- [Optional Options](#optional-options)

[9. TCP Keep Alive](#tcp-keep-alive)

[10. Realtime Database](#realtime-database)

- [Read Data](#read-data)

- [Store Data](#store-data)

- [Append Data](#append-data)

- [Patch Data](#patch-data)

- [Delete Data](#delete-data)

- [Filtering Data](#filtering-data)

- [Monitoring data](#monitoring-data)

- [Enable TCP KeepAlive for reliable HTTP Streaming](#enable-tcp-keepalive-for-reliable-http-streaming)

- [HTTP Streaming examples](#http-streaming-examples)

- [Backup and Restore Data](#backup-and-restore-data)

- [Database Error Handling](#database-error-handling)

[11. Add On](#add-on)

- [FireSense, The Programmable Data Logging and IO Control (Deprecated)](#firesense-the-programmable-data-logging-and-io-control-deprecated)

[12. Firebase Cloud Messaging (FCM)](#firebase-cloud-messaging-fcm)

[13. Create, Edit, Serializing and Deserializing the JSON Objects](#create-edit-serializing-and-deserializing-the-json-objects)

[14. License](#license)


## Features

* Supports ESP32 devices.

* Supports external Heap via SRAM/PSRAM.

* TinyGSMClient and Ethernet Client integration.

* Faster server reconnection with SSL Session Resumption.

* Supports external network module.

* Supports Firebase Realtime database.

* Supports Firebase Cloud Messaging.

* Supports Test Mode (No Auth).

* Supports Firmware OTA updates.


## Supported Devices

 * NodeMCU-32
 * WEMOS LOLIN32
 * TTGO T8 V1.8
 
 Most ESP32 boards are supported unless Sparkfun ESP32 Thing ([old version](https://www.sparkfun.com/products/13907)) is not recommended due to it built with non-standard 26 MHz clock on board instead of 40 MHz which causes the bugs and unstable network connection.


## Dependencies


This library required **Platform's Core SDK** to be installed.

For Arduino IDE, ESP32 Core SDK can be installed through **Boards Manager**. 

For PlatfoemIO IDE, ESP32 Core SDK can be installed through **PIO Home** > **Platforms** > **Espressif 8266 or Espressif 32**.



## Installation


### Using Library Manager

At Arduino IDE, go to menu **Sketch** -> **Include Library** -> **Manage Libraries...**

In Library Manager Window, search **"firebase"** in the search form then select **"FirebaseESP32"**. 

Click **"Install"** button.



For PlatformIO IDE, using the following command.

**pio lib install "FirebaseESP32""**

Or at **PIO Home** -> **Library** -> **Registry** then search **FirebaseESP32**.



### Manual installation

For Arduino IDE, download zip file from the repository (Github page) by select **Code** dropdown at the top of repository, select **Download ZIP** 

From Arduino IDE, select menu **Sketch** -> **Include Library** -> **Add .ZIP Library...**.

Choose **FirebaseESP32-master.zip** that previously downloaded.

Rename **FirebaseESP32-master** folder to **FirebaseESP32**.

Go to menu **Files** -> **Examples** -> **FirebaseESP32-master** and choose one from examples.




## Usages


See [all examples](/examples) for complete usages.

See [function description](/src/README.md) for all available functions.



### Initialization


```cpp

#include <Arduino.h>
#include <WiFi.h>

#include <FirebaseESP32.h>

// Define the Firebase Data object
FirebaseData fbdo;

// Define the FirebaseAuth data for authentication data
FirebaseAuth auth;

// Define the FirebaseConfig data for config data
FirebaseConfig config;

// Assign the project host and api key 
config.host = FIREBASE_HOST;

config.api_key = API_KEY;

// Assign the user sign in credentials
auth.user.email = USER_EMAIL;
auth.user.password = USER_PASSWORD;

// Initialize the library with the Firebase authen and config.
Firebase.begin(&config, &auth);

// Optional, set AP reconnection in setup()
Firebase.reconnectWiFi(true);

// Optional, set number of error retry
Firebase.setMaxRetry(fbdo, 3);

// Optional, set number of error resumable queues
Firebase.setMaxErrorQueue(fbdo, 30);

// Optional, use classic HTTP GET and POST requests.
// This option allows get and delete functions (PUT and DELETE HTTP requests) works for
// device connected behind the Firewall that allows only GET and POST requests.
Firebase.enableClassicRequest(fbdo, true);

// Since v4.4.x, BearSSL engine was used, the SSL buffer need to be set.
// Large data transmission may require larger RX buffer, otherwise connection issue or data read time out can be occurred.
fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

// Limit the size of response payload to be collected in FirebaseData
fbdo.setResponseSize(2048);

```
See [other authentication examples](/examples/Authentications) for more sign in methods.


## Memory Options


### Memory Options for ESP32

In ESP32 module that has PSRAM installed, you can enable it and set the library to use this external memory instead.

#### Arduino IDE

To enable PSRAM in ESP32 module.

![Enable PSRAM in ESP32](/media/images/ESP32-PSRAM.png)


#### PlatformIO IDE


In PlatformIO on VSCode or Atom IDE, add the following build_flags in your project's platformio.ini file.

```ini
build_flags = -DBOARD_HAS_PSRAM -mfix-esp32-psram-cache-issue
```

Once the external Heap memory was enabled in IDE, to allow the library to use the external memory, you can set it in [**FirebaseFS.h**](src/FirebaseFS.h) by define this macro.

```cpp
#define FIREBASE_USE_PSRAM
```




## Authentication

This library supports many types of authentications.

See [other authentication examples](/examples/Authentications) for more authentication methods.

Some authentication methods require the token generaion and exchanging process which take more time than using the legacy token.

The system time must be set before authenticate using the custom and OAuth2.0 tokens or when the root certificate was set for data transfer. 

The authentication with custom and OAuth2.0 tokens takes the time, several seconds in overall process which included the NTP time acquisition (system time setup), JWT token generation and signing process.

By setting the system time prior to calling the **`Firebase.begin`**, the internal NTP time acquisition process will be ignored.

You can set the system time using the RTC chip or manually by calling **`Firebase.setSystemTime`**.


While authenticate using Email and password, the process will be perform faster because no token generation and time setup required. 

The authenticate using the legacy token (database secret) does not have these delay time because the token is ready to use.



### Access in Test Mode (No Auth)

In Test Mode, token generation will be ignored and no authentication applied to the request.

You can access RTDB database in Test Mode by set the security rules like this.

```json
{
  "rules": {
    ".read": true, 
    ".write": true
  }
}
```
And set the `config.signer.test_mode = true;`, see [TestMode.ino](/examples/Authentications/TestMode/TestMode.ino) example.




### The authenication credentials and prerequisites


To use Email/Password sign-in authentication as in the examples, the Email/Password Sign-in provider must be enabled.



![Enable Email/Password Sign-in provider](/media/images/Enable_Email_Password_Provider.png)

![Enable Email/Password Sign-in provider](/media/images/Enable_Email_Password_Provider2.png)



Add Email and password for first user in your project then use this Email and password to sign in.

![Enable Email/Password Sign-in provider](/media/images/Enable_Email_Password_Provider3.png)



To use Anonymous sign-in, the Anonymous Sign-in provider must be enabled by follow the below steps.

![Enable Anonymous Sign-in provider](/media/images/Anonymous1.png)

![Enable Anonymous Sign-in provider](/media/images/Anonymous2.png)

![Enable Anonymous Sign-in provider](/media/images/Anonymous3.png)



To get API Key used in Email/Password sign-in

![API Key](/media/images/API_Key.png)



To get the Service accounts key JSON file used in Custom and OAuth2.0 tokens athentications.

![Service Account Key File](/media/images/Service_Account_Key.png)



For RTDB usages, create new real-time database (if not setup yet)

![Firebase Host](/media/images/Create_New_RTDB.png)

![Firebase Host](/media/images/Create_New_RTDB2.png)

![Firebase Host](/media/images/Create_New_RTDB3.png)



Edit the default database rules as following



![Firebase Host](/media/images/Create_New_RTDB4.png)

```json
{
  "rules": {
    ".read": "auth != null", 
    ".write": "auth != null"
  }
}
```

[This document](https://firebase.google.com/docs/database/security) provides the RTDB security rules details.

The Firebase RTDB security rules are JSON-based rules which it should valid to used with this library RTDB functions that involved the security rules modification and reading, otherwise the rules wont be changed or read by these functions.


To get the database URL and secret (legacy token).

![Firebase Host](/media/images/RTDB_URL.png)

![Firebase Auth](/media/images/RTDB_Secret.png)


For server SSL authentication by providing the server root certificate.

Server SSL certificate verification is the process to ensure that the server that client is being connected is a trusted (valid) server instead of fake server.

The Google's GlobalSign R2 root certificate can be download from https://pki.goog/repository/

Select the .PEM (base-64 encoded string) or .DER (binary) file to download.

From the test as of July 2021, GlobalSign Root CA was missing from Google server, the certificate chain, GTS Root R1 can be used instead of root certificate.

![Firebase Host](/media/images/PEM_Download.png)

Below is how to assign the certificate data for server verification.

```cpp
  /* In case the certificate data was used  */
  config.cert.data = rootCACert;

  // Or custom set the root certificate for each FirebaseData object
  fbdo.setCert(rootCACert);

  /* Or assign the certificate file */

  /** From the test as of July 2021, GlobalSign Root CA was missing from Google server
   * as described above, GTS Root R1 (gsr1.pem or gsr1.der) can be used instead.
   * ESP32 Arduino SDK supports PEM format only even mBedTLS supports DER format too.
   * ESP8266 SDK supports both PEM and DER format certificates.
  */
  // config.cert.file = "/gsr1.pem";
  // config.cert.file_storage = StorageType::FLASH;   //or StorageType::SD
```



## Library Build Options 

The library build options are defined as preprocessor macros (`#define name`).

Some options can be disabled to reduce program space.

### Predefined Options

The predefined options that are already set in [**FirebaseFS.h**](src/FirebaseFS.h) are following.

```cpp
ENABLE_NTP_TIME // For enabling the device or library time setup from NTP server
ENABLE_ERROR_STRING // For enabling the error string from error reason
FIREBASE_ENABLE_RTDB // For RTDB class compilation
FIREBASE_ENABLE_ERROR_QUEUE // For RTDB Error Queue compilation
FIREBASE_ENABLE_FCM // For Firebase Cloud Messaging compilation
FIREBASE_USE_PSRAM // For enabling PSRAM support
ENABLE_OTA_FIRMWARE_UPDATE // For enabling OTA updates support via RTDB, Firebase Storage and Google Cloud Storage buckets
USE_CONNECTION_KEEP_ALIVE_MODE // For enabling Keep Alive connection mode
DEFAULT_FLASH_FS // For enabling Flash filesystem support
DEFAULT_SD_FS // For enabling SD filesystem support 
CARD_TYPE_SD or CARD_TYPE_SD_MMC // The SD card type for SD filesystem
```

The Flash and SD filesystems are predefined.

SD is the default SD filesystem for all devices.

For ESP8266 and Arduino Pico, LittleFS is the default flash filesystem.

For ESP32 since v2.0.x, LittleFS is the default flash filesystem otherwise SPIFFS is the default flash filesystem.

In otherr devices, SPIFFS is the default flash filesystem.

User can change `DEFAULT_FLASH_FS` and `DEFAULT_SD_FS` with `CARD_TYPE_SD` or `CARD_TYPE_SD_MMC` defined values for other filesystems.

### Optional Options

The following options are not yet defined in [**FirebaseFS.h**](src/FirebaseFS.h) and can be assigned by user.

```cpp
FIREBASE_ETHERNET_MODULE_LIB `"EthernetLibrary.h"` // For the Ethernet library to work with external Ethernet module
FIREBASE_ETHERNET_MODULE_CLASS EthernetClass // For the Ethernet class object of Ethernet library to work with external Ethernet module
FIREBASE_ETHERNET_MODULE_TIMEOUT 2000 // For the time out in milliseconds to wait external Ethernet module to connect to network
FIREBASE_DISABLE_ONBOARD_WIFI // For disabling on-board WiFI functionality in case external Client usage
FIREBASE_DISABLE_NATIVE_ETHERNET // For disabling native (sdk) Ethernet functionality in case external Client usage
FIREBASE_DEFAULT_DEBUG_PORT // For debug port assignment
```

You can assign the optional build options using one of the following methods.

- By creating user config file `CustomFirebaseFS.h` in library installed folder and define these optional options.

- By adding compiler build flags with `-D name`.

In PlatformIO IDE, using `build_flags` in PlatformIO IDE's platformio.ini is more convenient 

```ini
build_flags = -D DISABLE_FCM
              -D EFIREBASE_DISABLE_ONBOARD_WIFI
```

For external Ethernet module integation used with function `setEthernetClient`, both `FIREBASE_ETHERNET_MODULE_LIB` and `FIREBASE_ETHERNET_MODULE_CLASS` should be defined.

`FIREBASE_ETHERNET_MODULE_LIB` is the Ethernet library name with extension (.h) and should be inside `""` or `<>` e.g. `"Ethernet.h"`.

`FIREBASE_ETHERNET_MODULE_CLASS` is the name of static object defined from class e.g. `Ethernet`.

`FIREBASE_ETHERNET_MODULE_TIMEOUT` is the time out in milliseconds to wait network connection.

For disabling predefined options instead of editing the [**FirebaseFS.h**](src/FirebaseFS.h) or using `#undef` in `CustomFirebaseFS.h`, you can define these build flags with these names or macros in `CustomFirebaseFS.h`.

```cpp
DISABLE_NTP_TIME // For disabling the NTP time setting
DISABLE_ERROR_STRING // For disabling the error string from error reason
DISABLE_RTDB // For disabling RTDB support
DISABLE_ERROR_QUEUE // For disabling RTDB Error Queue support
DISABLE_FCM // For disabling Firebase Cloud Messaging support
DISABLE_PSRAM // For disabling PSRAM support
DISABLE_OTA // For disabling OTA updates support
DISABLE_KEEP_ALIVE // For disabling TCP Keep Alive support (See TCP Keep Alive)
DISABLE_SD // For disabling flash filesystem support
DISABLE_FLASH // For disabling SD filesystem support
DISABLE_DEBUG // For disable debug port

FIREBASE_DISABLE_ALL_OPTIONS // For disabling all predefined build options above
```

Note that, `CustomFirebaseFS.h` for user config should be placed in the library install folder inside src folder.

This `CustomFirebaseFS.h` will not change or overwrite when update the library.



## TCP Keep Alive


The TCP KeepAlive can be enabled from executing `<FirebaseData>.keepAlive` with providing TCP options as arguments, i.e.,

`tcpKeepIdleSeconds`, `tcpKeepIntervalSeconds` and `tcpKeepCount`.

Ex.

```cpp
fbdo.keepAlive(5 /* tcp KeepAlive idle 5 seconds */, 5 /* tcp KeeAalive interval 5 seconds */, 1 /* tcp KeepAlive count 1 */);

// If one of three arguments is zero, the KeepAlive will be disabled.
```

To check the KeepAlive status, use `<FirebaseData>.isKeepAlive`.


For the TCP (KeepAlive) options, see [here](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/lwip.html#tcp-options).

You can check the server connecting status, by executing `<FirebaseData>.httpConnected()` which will return true when connection to the server is still alive. 


The TCP KeepAlive was currently available in ESP32 unless in ESP8266, [this ESP8266 PR #8940](https://github.com/esp8266/Arduino/pull/8940) should be merged in the [ESP8266 Arduino Core SDK](https://github.com/esp8266/Arduino/releases), i.e., it will be supported in the ESP8266 core version newer than v3.1.2.


In ESP8266 core v3.1.2 and older, the error can be occurred when executing `<FirebaseData>.keepAlive` because of object slicing.


The Arduino Pico is currently not support TCP KeepAlive until it's implemented in WiFiClientSecure library as in ESP8266.

 
For External Client, this TCP KeepAlive option is not appliable and should be managed by external Client library.


## Realtime Database

See [RTDB examples](/examples) for complete usages.

### Read Data


Data at a specific node in Firebase RTDB can be read through these get functions.

The functions included `get`, `getInt`, `getFloat`, `getDouble`, `getBool`, `getString`, `getJSON`, `getArray`, `getBlob`, `getFile`.


These functions return boolean value indicates the success of the operation which will be `true` if all of the following conditions were met.

* Server returns HTTP status code 200

* The data types matched between request and response.


For generic get, use Firebase.get(fbdo, \<path\>).

And check its type with fbdo.dataType() or fbdo.dataTypeEnum() and cast the value from it e.g. fbdo.to\<int\>(), fbdo.to\<std::string\>().

The data type of returning payload can be determined by `fbdo.dataType()` which returns String or `fbdo.dataTypeEnum()` returns enum value.

The String of type returns from `fbdo.dataType()` can be string, boolean, int, float, double, json, array, blob, file and null.

The enum value type, firebase_rtdb_data_type returns from `fbdo.dataTypeEnum()` can be firebase_rtdb_data_type_null (1), firebase_rtdb_data_type_integer, firebase_rtdb_data_type_float, firebase_rtdb_data_type_double, firebase_rtdb_data_type_boolean, firebase_rtdb_data_type_string, firebase_rtdb_data_type_json, firebase_rtdb_data_type_array, firebase_rtdb_data_type_blob, and firebase_rtdb_data_type_file (10)



The database data's payload (response) can be read or access through the casting value from FirebaseData object with to\<type\>() functions (since v2.4.0).

* `String s = fbdo.to<String>();`

* `std::string _s = fbdo.to<std::string>();`

* `const char *str = fbdo.to<const char *>();`

* `bool b = fbdo.to<bool>();`

* `int16_t _i = fbdo.to<int16_t>();`

* `int i = fbdo.to<int>();`

* `double d = fbdo.to<double>();`

* `float f = fbdo.to<float>();`

* `FirebaseJson *json = fbdo.to<FirebaseJson *>();` or

* `FirebaseJson &json = fbdo.to<FirebaseJson>();`

* `FirebaseJsonArray *arr = fbdo.to<FirebaseJsonArray *>();` or

* `FirebaseJsonArray &arr = fbdo.to<FirebaseJsonArray>();`

* `std::vector<uint8_t> *blob = fbdo.to<std::vector<uint8_t> *>();`

* `File file = fbdo.to<File>();`

Or through the legacy methods

* `int i = fbdo.intData();`

* `float f = fbdo.floatData();`

* `double d = fbdo.doubleData();`

* `bool b = fbdo.boolData();`

* `String s = fbdo.stringData();`

* `String js = fbdo.jsonString();`

* `FirebaseJson &json = fbdo.jsonObject();`

* `FirebaseJson *jsonPtr = fbdo.jsonObjectPtr();`

* `FirebaseJsonArray &arr = fbdo.jsonArray();` 

* `FirebaseJsonArray *arrPtr = fbdo.jsonArrayPtr();`

* `std::vector<uint8_t> blob = fbdo.blobData();`

 * `File file = fbdo.fileStream();`



Read the data which its type does not match the data type in the database from above functions will return empty (string, object or array).

BLOB and file stream data are stored as special base64 encoded string which are only supported and implemented by this library.

The encoded base64 string will be prefixed with some header string ("file,base64," and "blob,base64,") for data type manipulation. 




The following example showed how to read integer value from node "/test/int".


```cpp
  if (Firebase.getInt(fbdo, "/test/int")) {

      if (fbdo.dataTypeEnum() == firebase_rtdb_data_type_integer) {
      Serial.println(fbdo.to<int>());
    }

  } else {
    Serial.println(fbdo.errorReason());
  }
```




### Store Data

To store data at a specific node in Firebase RTDB, use these set functions.

The function included `set`, `setInt`, `setFloat`, `setDouble`, `setBool`, `setString`, `setJSON`, `setArray`, `setBlob` and `setFile`. 

For faster sending data, non-waits or async mode functions are available e.g. `setAsync`, `setIntAsync`, `setFloatAsync`, `setDoubleAsync`, `setBoolAsync`, `setStringAsync`, `setJSONAsync`, `setArrayAsync`, `setBlobAsync` and `setFileAsync`. 

For generic set, use Firebase.set(fbdo, \<path\>, \<any variable or value\>).

These async functions will ignore the server responses.


The above functions return boolean value indicates the success of the operation which will be `true` if all of the following conditions matched.

* Server returns HTTP status code 200

* The data types matched between request and response.


Only setBlob and setFile functions that make a silent request to Firebase server, thus no payload response returned. 

The **priority**, virtual node **".priority"** of each database node can be set through Firebase's set functions.

The priority value can be used in a query or filtering the children's data under a defined node.

Priority option was removed from File and Blob functions since v2.4.0.

**ETag** (unique identifier value) assigned to Firebase's set functions is used as conditional checking.

If defined Etag is not matched the defined path's ETag, the set operation will fail with result **412 Precondition Failed**.

ETag at any node can be read through `Firebase.getETag`.  ETag value changed upon the data was set or delete.

The server's **Timestamp** can be stored in the database through `Firebase.setTimestamp`. 

The returned **Timestamp** value can get from `fbdo.to<int>()`. 

The file systems for flash and sd memory can be changed in [**FirebaseFS.h**](/src/FirebaseFS.h).



The following example showed how to store file data to flash memory at node "/test/file_data".


```cpp

if (Firebase.getFile(fbdo, StorateType::FLASH, "/test/file_data", "/test.txt"))
{
  // FLASH.begin(); // not need to begin again due to it has been called in function.
  File file = DEFAULT_FLASH_FS.open("/test.txt", "r");

  while (file.available())
  {     
    Serial.print(file.read(), HEX);     
  }    
  file.close();
  Serial.println();

} else {
  Serial.println(fbdo.fileTransferError());
}
```



### Append Data

To append new data to a specific node in Firebase RTDB, use these push functions.

The function included `push`, `pushInt`, `pushFloat`, `pushDouble`, `pushBool`, `pushString`, `pushJSON`, `pushArray`, `pushBlob`, and `pushFile`.

For faster sending data, non-waits or async mode functions are available e.g. `pushAsync`, `pushIntAsync`, `pushFloatAsync`, `pushDoubleAsync`, `pushBoolAsync`, `pushStringAsync`, `pushJSONAsync`, `pushArrayAsync`, `pushBlobAsync` and `pushFileAsync`. 

These functions return boolean value indicates the success of the operation.

The **unique key** of a new appended node can be determined from `fbdo.pushName()`.

As set functions, the Firebase's push functions support **priority**.

**ETag** was not available after push unless read the **ETag** at that new appended unique key later with `Firebase.getETag`.

The server's **Timestamp** can be appended in the database through `Firebase.pushTimestamp`.

The unique key of Timestamp can be determined after Timestamp was appended.



The following example showed how to append new data (using FirebaseJson object) to node "/test/append.


```cpp

FirebaseJson json;
FirebaseJson json2;

json2.set("child_of_002", 123.456);
json.set("parent_001", "parent 001 text");
json.set("parent 002", json2);

if (Firebase.pushJSON(fbdo, "/test/append", json)) {

  Serial.println(fbdo.dataPath());

  Serial.println(fbdo.pushName());

  Serial.println(fbdo.dataPath() + "/"+ fbdo.pushName());

} else {
  Serial.println(fbdo.errorReason());
}
```



### Patch Data

Firebase's update functions used to patch or update new or existing data at the defined node.

These functions, `updateNode` and `updateNodeSilent` are available and work with JSON object (FirebaseJson object only).

For faster sending data, non-waits or async mode functions are available e.g. `updateNodeAsync`, and `updateNodeSilentAsync`.

If any key name provided at a defined node in JSON object has not existed, a new key will be created.

The server returns JSON data payload which was successfully patched.

Return of large JSON payload will cost the network data, alternative function `updateNodeSilent` or `updateNodeSilentAsync` should be used to save the network data.



The following example showed how to patch data at "/test".


```cpp

FirebaseJson updateData;
FirebaseJson json;
json.set("_data2","_value2");
updateData.set("data1","value1");
updateData.set("data2", json);

if (Firebase.updateNode(fbdo, "/test/update", updateData)) {

  Serial.println(fbdo.dataPath());

  Serial.println(fbdo.dataType());

  Serial.println(fbdo.jsonString()); 

} else {
  Serial.println(fbdo.errorReason());
}
```



### Delete Data


The following example showed how to delete data and its children at "/test/append"

```cpp
Firebase.deleteNode(fbdo, "/test/append");
```



### Filtering Data

To filter or query the data, the following query parameters are available through the QueryFilter class.

These parameters are `orderBy`, `limitToFirst`, `limitToLast`, `startAt`, `endAt`, and `equalTo`.

To filter data, parameter `orderBy` should be assigned.

Use **"$key"** as the `orderBy` parameter if the key of child nodes was used for the query.

Use **"$value"** as the `orderBy` parameter if the value of child nodes was used for the query.

Use **key (or full path) of child nodes** as the `orderBy` parameter if all values of the specific key were used for the query.

Use **"$priority"** as `orderBy` parameter if child nodes's **"priority"** was used for query.




The above `orderBy` parameter can be combined with the following parameters for limited and ranged the queries.

`QueryFilter.limitToFirst` -  The total children (number) to filter from the first child.

`QueryFilter.limitToLast` -   The total last children (number) to filter. 

`QueryFilter.startAt` -       Starting value of range (number or string) of query upon orderBy param.

`QueryFilter.endAt` -         Ending value of range (number or string) of query upon orderBy param.

`QueryFilter.equalTo` -       Value (number or string) matches the orderBy param




The following example showed how to use queries parameter in QueryFilter class to filter the data at node "/test/data"

```cpp
// Assume that children that have key "sensor" are under "/test/data"

// Instantiate the QueryFilter class
QueryFilter query;

// Build query using specified child node key "sensor" under "/test/data"
query.orderBy("sensor");

// Query any child that its value begins with 2 (number), assumed that its data type is float or integer
query.startAt(2);

// Query any child that its value ends with 8 (number), assumed that its data type is float or integer
query.endAt(8);

// Limit the maximum query result to return only the last 5 nodes
query.limitToLast(5);


if (Firebase.getJSON(fbdo, "/test/data", query))
{
  // Success, then try to read the JSON payload value
  Serial.println(fbdo.jsonString());
}
else
{
  // Failed to get JSON data at defined database path, print out the error reason
  Serial.println(fbdo.errorReason());
}

// Clear all query parameters
query.clear();
```



### Monitoring Data


This library uses HTTP GET request with `text/event-stream` header to make [**HTTP streaming**](https://en.wikipedia.org/wiki/Server-sent_events) connection.

The Firebase's functions that involved the stream operations are `beginStream`, `beginMultiPathStream`, 
`setStreamCallback`, `setMultiPathStreamCallback` and/or `readStream`.

Function `beginStream` is to subscribe to the data changes at a defined node.

Function `beginMultiPathStream` is to subscribe to the data changes at a defined parent node path with multiple child nodes value parsing and works with setMultiPathStreamCallback.

Function `setStreamCallback` is to assign the callback function that accepts the **FirebaseStream** class as parameter.

Function `setMultiPathStreamCallback` is to assign the callback function that accepts the **MultiPathStream** class as parameter.


The **FirebaseStream** contains stream's event/data payloadd and interface function calls are similar to `FirebaseData` object.

The **MultiPathStream** contains stream's event/data payload for various child nodes.


To polling the stream's event/data payload manually, use `readStream` in loop().

Function `readStream` used in the loop() task to continuously read the stream's event and data.

Since polling the stream's event/data payload with `readStream`, use `fbdo.streamAvailable` to check if stream event/data payoad is available.

Function `fbdo.streamAvailable` returned true when new stream's event/data payload was available. 

When new stream payload was available, its data and event can be accessed from `FirebaseData` object functions.

Function `endStream` ends the stream operation.


Note that, when using the shared `FirebaseData` object for stream and CRUD usages(normal operation to create,read, update and delete data), the stream connection will be interrupted (closed) to connect in other HTTP mode, the stream will be resumed (open) after the CRUD usages.

For the above case, you need to provide the idle time for `FirebaseData` object to established the streaming connection and received the stream payload. The changes on the server at the streaming node path during the stream interruption will be missed.

To avoid this sitation, don't share the usage of stream's `FirebaseData` object, another `FirebaseData` object should be used.

In addition, delay function used in the same loop of `readStream()` will defer the streaming, the server data changes may be missed.

Keep in mind that `FirebaseData` object will create the SSL client inside of HTTPS data transaction and uses large memory.



### Enable TCP KeepAlive for reliable HTTP Streaming

In general, the RTDB stream timed out occurred when no data included keep-alive event data received in the specific period (45 seconds) which can be set via `config.timeout.rtdbKeepAlive`.

Now you can take the pros of TCP KeepAlive in Stream mode by probing the server connection at some intervals to help the stream time out more reliable.

You can check the server connecting status, by executing `<FirebaseData>.httpConnected()` which will return true when connection to the server is still alive. 

As previousely described, using [TCP KeepAlive in `FirebaseData` object](#about-firebasedata-object) in Stream has pros and cons.

The TCP KeepAlive can be enabled from executing `<FirebaseData>.keepAlive` with providing TCP options as arguments, i.e.,

`tcpKeepIdleSeconds`, `tcpKeepIntervalSeconds` and `tcpKeepCount`.

Ex.

```cpp
stream.keepAlive(5 /* tcp KeepAlive idle 5 seconds */, 5 /* tcp KeeAalive interval 5 seconds */, 1 /* tcp KeepAlive count 1 */);
```


### HTTP Streaming examples


The following example showed how to subscribe to the data changes at node "/test/data" with a callback function.

```cpp

// In setup(), set the stream callback function to handle data
// streamCallback is the function that called when database data changes or updates occurred
// streamTimeoutCallback is the function that called when the connection between the server 
// and client was timeout during HTTP stream

Firebase.setStreamCallback(fbdo, streamCallback, streamTimeoutCallback);

// In setup(), set the streaming path to "/test/data" and begin stream connection

if (!Firebase.beginStream(fbdo, "/test/data"))
{
  // Could not begin stream connection, then print out the error detail
  Serial.println(fbdo.errorReason());
}

  
  // Global function that handles stream data
void streamCallback(StreamData data)
{

  // Print out all information

  Serial.println("Stream Data...");
  Serial.println(data.streamPath());
  Serial.println(data.dataPath());
  Serial.println(data.dataType());

  // Print out the value
  // Stream data can be many types which can be determined from function dataType

  if (data.dataTypeEnum() == firebase_rtdb_data_type_integer)
      Serial.println(data.to<int>());
  else if (data.dataTypeEnum() == firebase_rtdb_data_type_float)
      Serial.println(data.to<float>(), 5);
  else if (data.dataTypeEnum() == firebase_rtdb_data_type_double)
      printf("%.9lf\n", data.to<double>());
  else if (data.dataTypeEnum() == firebase_rtdb_data_type_boolean)
      Serial.println(data.to<bool>()? "true" : "false");
  else if (data.dataTypeEnum() == firebase_rtdb_data_type_string)
      Serial.println(data.to<String>());
  else if (data.dataTypeEnum() == firebase_rtdb_data_type_json)
  {
      FirebaseJson *json = data.to<FirebaseJson *>();
      Serial.println(json->raw());
  }
  else if (data.dataTypeEnum() == firebase_rtdb_data_type_array)
  {
      FirebaseJsonArray *arr = data.to<FirebaseJsonArray *>();
      Serial.println(arr->raw());
  }

}

// Global function that notifies when stream connection lost
// The library will resume the stream connection automatically
void streamTimeoutCallback(bool timeout)
{
  if(timeout){
    // Stream timeout occurred
    Serial.println("Stream timeout, resume streaming...");
  }  
}

// For authentication except for legacy token, Firebase.ready() should be called repeatedly 
// in loop() to handle authentication tasks.

void loop()
{
  if (Firebase.ready())
  {
    // Firebase is ready to use now.
    
  }
}

```



For multiple paths stream, see the MultiPath_stream example.


The following example showed how to subscribe to the stream changes at "/test/data" and read the stream manually.

```cpp
// In setup(), set the streaming path to "/test/data" and begin stream connection
if (!Firebase.beginStream(fbdo, "/test/data"))
{
  Serial.println(fbdo.errorReason());
}

//In loop()
if (!Firebase.readStream(fbdo))
{
  Serial.println(fbdo.errorReason());
}

if (fbdo.streamTimeout())
{
  Serial.println("Stream timeout, resume streaming...");
  Serial.println();
}

if (fbdo.streamAvailable())
{

  if (fbdo.dataTypeEnum() == firebase_rtdb_data_type_integer)
    Serial.println(fbdo.to<int>());
  else if (fbdo.dataTypeEnum() == firebase_rtdb_data_type_float)
    Serial.println(fbdo.to<float>(), 5);
  else if (fbdo.dataTypeEnum() == firebase_rtdb_data_type_double)
    printf("%.9lf\n", fbdo.to<double>());
  else if (fbdo.dataTypeEnum() == firebase_rtdb_data_type_boolean)
    Serial.println(fbdo.to<bool>() ? "true" : "false");
  else if (fbdo.dataTypeEnum() == firebase_rtdb_data_type_string)
    Serial.println(fbdo.to<String>());
  else if (fbdo.dataTypeEnum() == firebase_rtdb_data_type_json)
  {
      FirebaseJson *json = fbdo.to<FirebaseJson *>();
      Serial.println(json->raw());
  }
  else if (fbdo.dataTypeEnum() == firebase_rtdb_data_type_array)
  {
      FirebaseJsonArray *arr = fbdo.to<FirebaseJsonArray *>();
      Serial.println(arr->raw());
  }
    
}

// For authentication except for legacy token, Firebase.ready() should be called repeatedly 
// in loop() to handle authentication tasks.

void loop()
{
  if (Firebase.ready())
  {
    // Firebase is ready to use now.
    
  }
}
```


### Backup and Restore Data


This library allows data backup and restores at a defined path.

The backup file will store in SD/SDMMC card or flash memory.

The file systems for flash and SD memory can be changed via [**FirebaseFS.h**](/src/FirebaseFS.h).

Due to SD library used, only 8.3 DOS format file name supported.

The maximum 8 characters for a file name and 3 characters for file extension.

The database restoration returned completed status only when Firebase server successfully updates the data. 

Any failed operation will not affect the database (no updates or changes).

The following example showed how to backup all database data at "/" and restore.

```cpp
 String backupFileName;

 if (!Firebase.backup(fbdo, StorateType::SD, "/", "/backup.txt"))
 {
   Serial.println(fbdo.fileTransferError());
 }
 else
 {
   Serial.println(fbdo.getBackupFilename());
   Serial.println(fbdo.getBackupFileSize());
   backupFileName = fbdo.getBackupFilename();
  }


  // Begin restore backed dup data back to database
  if (!Firebase.restore(fbdo, StorateType::SD, "/", backupFileName))
  {
    Serial.println(fbdo.fileTransferError());
  }
  else
  {
    Serial.println(fbdo.getBackupFilename());
  }
```



### Database Error Handling

When read store, append and update operations were failed due to buffer overflow and network problems.

These operations can retry and queued after the retry amount was reached the maximum retry set in function `setMaxRetry`.

```cpp
// set maximum retry amount to 3
 Firebase.setMaxRetry(fbdo, 3);
```

The function `setMaxErrorQueue` limits the maximum queues in Error Queue collection.

The full of queue collection can be checked through function `isErrorQueueFull`.


```cpp
 // set maximum queues to 10
 Firebase.setMaxErrorQueue(fbdo, 10);

 // determine whether Error Queue collection is full or not
 Firebase.isErrorQueueFull(fbdo);
```


This library provides two approaches to run or process Error Queues with two functions. 

* `beginAutoRunErrorQueue`
* `processErrorQueue`

The function `beginAutoRunErrorQueue` will run or process queues automatically and can be called once. 

While function `processErrorQueue` will run or process queues and should call inside the loop().

With function `beginAutoRunErrorQueue`, you can assigned callback function that accept **QueueInfo** object as parameter.

Which contains all information about being processed queue, number of remaining queues and Error Queue collection status.

Otherwise, Error Queues can be tracked manually with the following functions.

Function `getErrorQueueID` will return the unsigned integer presents the id of the queue which will keep using later.

Use `getErrorQueueID` and `isErrorQueueExisted` to check whether this queue id is still existed or not. 

If Error Queue ID does not exist in Error Queues collection, that queue is already done.

The following example showed how to run Error Queues automatically and track the status with the callback function.

```cpp

// In setup()

// Set the maximum Firebase Error Queues in collection (0 - 255).
// Firebase read/store operation causes by network problems and buffer overflow will be 
// added to Firebase Error Queues collection.
Firebase.setMaxErrorQueue(fbdo, 10);

// Begin to run Error Queues in Error Queue collection  
Firebase.beginAutoRunErrorQueue(fbdo, callback);


// Use to stop the auto run queues
// Firebase.endAutoRunErrorQueue(fbdo);

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
  Serial.println(queueinfo.firebaseMethod());

  Serial.print("Path: ");
  Serial.println(queueinfo.dataType());

  Serial.println();
}
```



The following example showed how to run Error Queues and track its status manually.

```cpp
// In setup()

// Set the maximum Firebase Error Queues in collection (0 - 255).
// Firebase read/store operation causes by network problems and buffer overflow will be added to 
// Firebase Error Queues collection.
Firebase.setMaxErrorQueue(fbdo, 10);


// All of the following are in loop()

Firebase.processErrorQueue(fbdo);

// Detrnine the queue status
if (Firebase.isErrorQueueFull(fbdo))
{
  Serial.println("Queue is full");
}

// Remaining Error Queues in Error Queue collection
Serial.print("Remaining queues: ");
Serial.println(Firebase.errorQueueCount(fbdo));

// Assumed that queueID is unsigned integer array of queue that added to Error Queue collection 
// when error and use Firebase.getErrorQueueID to get this Error Queue id.

for (uint8_t i = 0; i < LENGTH_OF_QUEUEID_ARRAY; i++)
{
  Serial.print("Error Queue ");
  Serial.print(queueID[i]);
  if (Firebase.isErrorQueueExisted(fbdo, queueID[i]))
    Serial.println(" is queuing");
  else
    Serial.println(" is done");
}
Serial.println();
```



Error Queues can be saved as a file in SD card or Flash memory with function `saveErrorQueue`.

Error Queues store as a file can be restored to Error Queue collection with function `restoreErrorQueue`.

Two types of storage can be assigned with these functions, `StorageType::FLASH` and `StorageType::SD`.

Read data (get) operation is not support queues restore

The following example showed how to restore and save Error Queues in /test.txt file.

```cpp
// To restore Error Queues

if (Firebase.errorQueueCount(fbdo, "/test.txt", StorageType::FLASH) > 0)
{
    Firebase.restoreErrorQueue(fbdo, "/test.txt", StorageType::FLASH);
    Firebase.deleteStorageFile("/test.txt", StorageType::FLASH);
}

// To save Error Queues to file
Firebase.saveErrorQueue(fbdo, "/test.txt", StorageType::FLASH);

```

## Add On


### FireSense, The Programmable Data Logging and IO Control (Deprecated)

This add on library is for the advance usages and works with Firebase RTDB.

With this add on library, you can remotely program your device to control its IOs or do some task or call predefined functions on the fly.

This allows you to change your device behaviour and functions without to flash a new firmware via serial or OTA.

For FireSense function description, see [src/addons/FireSense/README.md](src/addons/FireSense/README.md).

FireSense is now inactive development and deprecated.




## Firebase Cloud Messaging (FCM)

The library acts as a app server to sends the message to registeration devices by sending request to the Google's FCM backend via the legacy HTTP and HTTPv1 APIs.

The functions available are setServerKey, send, subscibeTopic, unsubscibeTopic, appInstanceInfo and regisAPNsTokens.

Function `Firebase.FCM.setServerKey` to setup the Server Key which required by the legacy protocols.

Function `Firebase.FCM.send` to send the message with the selectable legacy and HTTPv1 messages constructors.  

Function `Firebase.FCM.subscribeTopic` to add the subscription for instance ID (IID) tokens to the defined topic.

Function `Firebase.FCM.unsubscribeTopic` to remove the subscription for instance ID (IID) tokens from the defined topic.

Function `Firebase.FCM.appInstanceInfo` to get the app instance info for a device. This also provides the subscribed topics info.

Function `Firebase.FCM.regisAPNsTokens` to create the registration tokens for iOS APNs tokens.

The library provides two message constructors that hold the data to construct the JSON object payload internally.

For legacy message, see https://firebase.google.com/docs/cloud-messaging/http-server-ref

For HTTPv1 message, see ttps://firebase.google.com/docs/reference/fcm/rest/v1/projects.messages

The HTTPv1 APIs requires OAUth2.0 authentication using the Service Account credential.



The following example showed how to send FCM message.

```cpp
// Provide your Firebase project's server key to send messsage using the legacy protocols
Firebase.FCM.setServerKey(FIREBASE_FCM_SERVER_KEY);

// Construct the legacy message
FCM_HTTPv1_JSON_Message msg;

// Assign the device registration token
msg.token = DEVICE_REGISTRATION_ID_TOKEN;

// Assign the notification payload
msg.notification.body = "Notification body";
msg.notification.title = "Notification title";

FirebaseJson json;
String payload;

// Assign the data payload
// all data key-values should be in string
json.add("humidity", "70");
json.toString(payload);
msg.data = payload.c_str();

// Send message
if (Firebase.FCM.send(&fbdo, &msg))
{
   erial.println("Message sent to FCM backend.");
   Serial.println(Firebase.FCM.payload(&fbdo));
}
else
{
   Serial.println("Something wrong, can't send request to FCM backend.");
   Serial.println(fbdo.errorReason());
}

```



## Create, Edit, Serializing and Deserializing the JSON Objects


This library has built-in FirebaseJson Arduino library, the easiest JSON parser, builder and editor.

FirebaseJson usages are so simple as you read, store and update(edit) the JSON node in Firebase RTDB.

It doesn't use the recursive call to parse or deserialize complex or nested JSON objects and arrays. 

This makes the library can use with a limited memory device. 


Since you declare the FirebaseJson or FirebaseJsonArray object, use the functions `setJsonData`, `setJsonArrayData`, `add`, `set` and `remove` to build or edit the JSON/Array object and use `get` to parse the node's contents. 

Defined the relative path of the specific node to `add`, `set`, `remove` and `get` functions to add, set, remove and get its contents.


Function `FirebaseJson.setJsonData` is to deserialize the JSON string to JSON object.

In addition, function `FirebaseJson.readFrom` can be used to read the streaming JSON contents from WiFi/Ethernet Client, File and Harware Serial and serialize it as the streaming content contains valid JSON data. 


Function `FirebaseJson.add` is used to add the new node with the contents e.g. String, Number (int and double), Boolean, Array and Object to the defined node.


Function `FirebaseJson.set` is used for edit, overwrite, create new (if not exist) node with contents e.g. String, Number (int and double), Boolean, Array and Object at the defined relative path and node.


Function `FirebaseJson.get` is used for parsing or deserializee the JSON object and array. The deserialized or parsed result will keep in FirebaseJsonData object which can be casted to any type of value or variable e.g string, bool, int, float, double by using `FirebaseJsonData.to<type>`. 

The casting from FirebaseJsonData to FirebaseJson and FirebaseJsonArray objects is different, by using `FirebaseJsonData.getJSON(FirebaseJson)` and `FirebaseJsonData.getArray(FirebaseJsonArray)`.


Function `FirebaseJson.remove` is used to remove the node and all its children's contents at the defined relative path and node. 


Function `FirebaseJson.toString` is used for serializeing the JSON object to writable objects e.g. char array, Arduino String, C/C++ string, WiFi/Ethernet Client and Hardware/Software Serial.


Function `FirebaseJson.serializedBufferLength` is used for calculating the serialized buffer size that required for reserved buffer in serialization.


Function `FirebaseJson.responseCode` is used to get the http code response header while read the WiFi/Ethernet Client using `FirebaseJson.toString`.


Functions `FirebaseJson.iteratorBegin`, `FirebaseJson.iteratorGet` and `FirebaseJson.iteratorEnd` are used to parse all JSON object contents as a list which can be iterated with index.


Function `FirebaseJson.clear` is used to clear JSON object contents.


Function `FirebaseJson.setFloatDigits` is for float number precision when serialized to string.


Function `FirebaseJson.setDoubleDigits` is for double number precision when serialized to string.


Function `FirebaseJsonArray.add` is used for adding the new contents e.g. String, Number (int and double), Boolean, Array and Object to JSON array.


Function `FirebaseJsonArray.set` is for edit, overwrite, create new (if not exist) contents e.g. String, Number (int and double), Boolean, Array and Object at the defined relative path or defined index of JSON array.


Function `FirebaseJsonArray.get` and `FirebaseJsonArray.search`work in the same way as FirebaseJson objects


Function `FirebaseJsonArray.remove` is used to remove the array's contents at the defined relative path or defined index of JSON array.


Function `FirebaseJsonArray.toString` is used for serializeing the JSON array object to writable objects e.g. char array, Arduino String, C/C++ string, WiFi/Ethernet Client and Hardware/Software Serial.


Function `FirebaseJsonArray.serializedBufferLength` is used for calculating the serialized buffer size that required for reserved buffer in serialization.


Function `FirebaseJsonArray.responseCode` is used to get the http code response header while read the WiFi/Ethernet Client using `FirebaseJson.toString`.


Function `FirebaseJsonArray.clear` is used to clear JSON array object contents.


Function `FirebaseJsonArray.setFloatDigits` is for float number precision when serialized to string.


Function `FirebaseJsonArray.setDoubleDigits` is for double number precision when serialized to string.


See [examples/FirebaseJson](examples/FirebaseJson) for the usage.

For FirebaseJson function description, see [FirebaseJSON object Functions](src#firebasejson-object-functions).


The following example shows how to use FirebaseJson.

```cpp
// Declare FirebaseJson object (global or local)
FirebaseJson json;

// Add name with value Living Room to JSON object
json.add("name", "Living Room");

// Add temp1 with value 120 and temp1 with 40 to JSON object
// Note: temp2 is not the child of temp1 as in previous version.
json.add("temp1", 120).add("temp2", 40);

// Add nested child contents directly
json.set("unit/temp1", "Farenheit");
json.set("unit/temp2", "Celcius");

// Deserialize to serial with prettify option
json.toString(Serial, true);
Serial.println();
Serial.println();

/**
This is the result of the above code

{
    "name": "Living Room",
    "temp1": 120,
    "temp2": 40,
    "unit": {
        "temp1": "Farenheit",
        "temp2": "Celcius"
    }
}
*/

// To set array to the above JSON using FirebaseJson directly
// Set (add) array indexes 0,1,2,5,7 under temp1, the original value will be replaced with new one.
json.set("temp1/[0]", 47);
json.set("temp1/[1]", 28);
json.set("temp1/[2]", 34);
json.set("temp1/[5]", 23); // null will be created at array index 3,4 due to it's not yet assigned
json.set("temp1/[7]", 25); // null will be created at array index 6

// Print out as prettify string
json.toString(Serial, true);
Serial.println();
Serial.println();

/**
The result of the above code

{
    "name": "Living Room",
    "temp1": [
        47,
        28,
        34,
        null,
        null,
         23,
        null,
        25
     ],
    "temp2": 40,
    "unit": {
        "temp1": "Farenheit",
        "temp2": "Celcius"
    }
 }
*/

// Try to remove temp1 array at index 1
json.remove("temp1/[1]");

// Try to remove temp2
json.remove("temp2");

// Print out as prettify string
json.toString(Serial, true);
Serial.println();
Serial.println();

/**
The result of the above code

{
    "name": "Living Room",
    "temp1": [
         47,
         34,
         null,
         null,
         23,
         null,
         25
    ],
    "unit": {
        "temp1": "Farenheit",
        "temp2": "Celcius"
    }
}
*/

// Now parse/read the contents from specific node unit/temp2
// FirebaseJsonData is required to keep the parse results which can be accessed later
FirebaseJsonData result;

json.get(result, "unit/temp2");

if (result.success)
{
  // Print type of parsed data e.g string, int, double, bool, object, array, null and undefined
  Serial.println(result.type);
  // Print its content e.g.string, int, double, bool whereas object, array and null also can access as string
  Serial.println(result.to<String>());
  // Serial.println(result.to<int>());
  // Serial.println(result.to<bool>());
  // Serial.println(result.to<float>());
  // Serial.println(result.to<double>());
}

// The above code will show
/**
string
Celcius
*/

// To get the array temp from FirebaseJson

json.get(result, "temp1");

// Prepare FirebaseJsonArray to take the array from FirebaseJson
FirebaseJsonArray arr;

// Get array data
result.get<FirebaseJsonArray>(arr);

// Call get with FirebaseJsonData to parse the array at defined index i
for (size_t i = 0; i < arr.size(); i++)
{
  // result now used as temporary object to get the parse results
  arr.get(result, i);

  // Print its value
  Serial.print("Array index: ");
  Serial.print(i);
  Serial.print(", type: ");
  Serial.print(result.type);
  Serial.print(", value: ");
  Serial.println(result.to<String>());
}

/**
The result of above code
Array index: 0, type: int, value: 47
Array index: 1, type: int, value: 34
Array index: 2, type: null, value: null
Array index: 3, type: null, value: null
Array index: 4, type: int, value: 23
Array index: 5, type: null, value: null
Array index: 6, type: int, value: 25
*/
 

```



The following example shows how to use FirebaseJsonArray.

```cpp
// Declare FirebaseJsonArray object (global or local)
FirebaseJsonArray arr;

// Add some data
arr.add("banana");
arr.add("mango");
arr.add("coconut");


// Change the array contents
arr.set("[1]/food", "salad");
arr.set("[1]/sweet", "cake");
arr.set("[1]/appetizer", "snack");
arr.set("[2]", "apple"); // or arr.set(2, "apple");
arr.set("[4]/[0]/[1]/amount", 20);

// Print out array as prettify string
arr.toString(Serial, true);
Serial.println();
Serial.println();

/**
This is the result of the above code

[
    "banana",
    {
        "food": "salad",
        "sweet": "cake",
        "appetizer": "snack"
    },
    "apple",
    null,
    [
        [
            null,
            {
                "amount": 20
            }
        ]
    ]
]
*/

// Remove array content at /4/0/1/amount
arr.remove("[4]/[0]/[1]/amount");

// Print out as prettify string
arr.toString(Serial, true);
Serial.println();
Serial.println();
/**
The result of the above code

[
    "banana",
    {
        "food": "salad",
        "sweet": "cake",
        "appetizer": "snack"
    },
    "apple",
    null,
    [
        [
            null
        ]
    ]
]

*/

// Now parse/read the array contents at some index

FirebaseJsonData result;

arr.get(result, "[1]/food");

if(result.success)
{
  // Type of parsed data
  Serial.println(result.type);
  // Its value
  Serial.println(result.to<String>());
  // Serial.println(result.to<int>());
  // Serial.println(result.to<bool>());
  // Serial.println(result.to<float>());
  // Serial.println(result.to<double>());

}

// The above code will show
/**
string
salad
*/


// To get the JSON object at array index 1 from FirebaseJsonArray
arr.get(result, "[1]");// or arr.get(result, 1);

// Prepare FirebaseJson to take the JSON object from FirebaseJsonArray
FirebaseJson json;

// Get FirebaseJson data
result.get<FirebaseJson>(json);

// Parse the JSON object as list
// Get the number of items
size_t len = json.iteratorBegin();
FirebaseJson::IteratorValue value;
for (size_t i = 0; i < len; i++)
{
    value = json.valueAt(i);
    Serial.printf("%d, Type: %s, Name: %s, Value: %s\n", i, value.type == FirebaseJson::JSON_OBJECT ? "object" : "array", value.key.c_str(), value.value.c_str());
}

// Clear all list to free memory
json.iteratorEnd();


/**
The result of the above code

0, Type: object, Key: food, Value: salad
1, Type: object, Key: sweet, Value: cake
2, Type: object, Key: appetizer, Value: snack

*/


```



## License

The MIT License (MIT)

Copyright (c) 2024 K. Suwatchai (Mobizt)


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
