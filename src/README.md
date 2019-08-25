# Firebase Realtime Database Arduino Library for ESP32


Google's Firebase Realtime Database Arduino Library for ESP32 v 3.2.0


## Global functions


#### Store Firebase's authentication credentials

param **`host`** - Your Firebase database project host without http:// or https:// protocol e.g. Your_ProjectID.firebaseio.com.

param **`auth`** - Your database secret.

param **`rootCA`** - Base64 encoded root certificate string.

param **`rootCAFile`** - Root CA certificate base64 format file.

param **`storageType`** - Type of storage, StorageType::SD and StorageType::SPIFFS.

```C++
void begin(const String &host, const String &auth);

void begin(const String &host, const String &auth, const char *rootCA);

void begin(const String &host, const String &auth, const String &rootCAFile, uint8_t storageType);
```








#### Stop Firebase and release all resources

param **`dataObj`** - Firebase Data Object to hold data and instances.

```C++
void end(FirebaseData &dataObj);
```






#### Reconnect WiFi if lost connection

param **`reconnect`** - The boolean to set/unset WiFi AP reconnection.

```C++
void reconnectWiFi(bool reconnect);
```






#### Set the timeouts of get function.

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`millisec`** - The missiseconds to limit the request (0 - 900,000 ms or 15 min).

```C++
void setReadTimeout(FirebaseData &dataObj, int millisec);
```






#### Set the timeouts of get function.

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`size`** - The size identified string e.g. tiny, small, medium, large and unlimited.

Size string and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).

```C++
void setwriteSizeLimit(FirebaseData &dataObj, const String &size);
```






#### Read the database rules

param **`dataObj`** - Firebase Data Object to hold data and instances.

return - **`Boolean`** type status indicates the success of operation.

```C++
bool getRules(FirebaseData &dataObj);
```







#### Write the database rules

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`rules`** - Database rules in jSON String format.

return - **`Boolean`** type status indicates the success of operation.

```C++
bool setRules(FirebaseData &dataObj, const String &rules);
```







#### Determine whether defined database path is existed or not

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Database path to be checked.

return - **`Boolean`** type result indicates whether the defined database
path was existed or not.

```C++
bool pathExist(FirebaseData &dataObj, const String &path);
```





#### Determine the unique identifier (ETag) of current data at the defined database path

return **`String`** of unique identifier.

```C++
String getETag(FirebaseData &dataObj, const String &path);
```






#### Get the shallowed data at defined node path.

aram **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Database path being read the data.

return - **`Boolean`** type status indicates the success of operation.


Return the child data with its value or JSON object (its values will be truncated to true).

Call [FirebaseData object].dataType to determine what type of data that returned from database. 

Call **[FirebaseData object].dataType** to determine what type of data that returned from database. 

Call **[FirebaseData object].intData** to get integer data.

Call **[FirebaseData object].floatData** to get float data.

Call **[FirebaseData object].doubleData** to get double data.

Call **[FirebaseData object].boolData** to get boolean data.

Call **[FirebaseData object].stringData** to get string data.

Call **[FirebaseData object].jsonData** to get JSON string data.

Call **[FirebaseData object].jsonObject** to get FirebaseJson object.

```C++
bool getShallowData(FirebaseData &dataObj, const String &path); 
```







#### Enable the library to use only classic HTTP GET and POST methods

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`flag`** - Boolean value to enable.

This option used to escape the Firewall restriction (if device is connected through Firewall) that allows only HTTP GET and POST
    
HTTP PATCH request was sent as PATCH which not affected by this option.

```C++
void enableClassicRequest(FirebaseData &dataObj, bool flag);
```





#### Set the virtual child node ".priority" to the defined database path. 
    
param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which to set the prioity value.

param **`priority`** - The priority value.
    
return - **`Boolean`** type status indicates the success of operation.



This allows us to set priority to any node other than priority that set through setJSON, pushJSON, updateNode and updateNodeSilent functions.
    
The returned priority value from server can read from function [FirebaseData object].priority().

```C++
bool setPriority(FirebaseData &dataObj, const String &path, float priority);
```







#### Read the virtual child node ".priority" value at the defined database path. 
    
param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which to set the prioity value.
    
return - **`Boolean`** type status indicates the success of operation.


The priority value from server can read from function [FirebaseData object].priority().

 ```C++
bool getPriority(FirebaseData &dataObj, const String &path);
```







#### Append new integer value to the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which integer value will be appended.

param **`intValue`** - The appended value.

return **`Boolean`** type status indicates the success of operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushInt(FirebaseData &dataObj, const String &path, int intValue);
```





#### Append new integer value and the virtual child ".priority" to the defined database path.

```C++
bool pushInt(FirebaseData &dataObj, const String &path, int intValue, float priority);
```






#### Append new float value to the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which float value will be appended.

param **`floatValue`** - The appended value.

return **`Boolean`** type status indicates the success of operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushFloat(FirebaseData &dataObj, const String &path, float floatValue);
```





#### Append new float value and the virtual child ".priority" to the defined database path.

```C++
bool pushFloat(FirebaseData &dataObj, const String &path, float floatValue, float priority);
```







#### Append new double value (8 bytes) to the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which float value will be appended.

param **`doubleValue`** - The appended value.

return **`Boolean`** type status indicates the success of operation.

The new appended node's key will be stored in Firebase Data object,
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushDouble(FirebaseData &dataObj, const String &path, double doubleValue);
```





#### Append new double value (8 bytes) and the virtual child ".priority" to the defined database path.

```C++
bool pushDouble(FirebaseData &dataObj, const String &path, double doubleValue, float priority);
```





#### Append new Boolean value to the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which Boolean value will be appended.

param **`boolValue`** - The appended value.

return **`Boolean`** type status indicates the success of operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushBool(FirebaseData &dataObj, const String &path, bool boolValue);
```





#### Append new Boolean value and the virtual child ".priority" to the defined database path.

```C++
bool pushBool(FirebaseData &dataObj, const String &path, bool boolValue, float priority);
```





#### Append new string (text) to the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which string will be appended.

param **`stringValue`** - The appended value.

return **`Boolean`** type status indicates the success of operation.

The new appended node's key stored in Firebase Data object, 
which can be accessed via function [FirebaseData object].pushName().

```C++
bool pushString(FirebaseData &dataObj, const String &path, const String &stringValue);
```






#### Append new string (text) and the virtual child ".priority" to the defined database path.

```C++
bool pushString(FirebaseData &dataObj, const String &path, const String &stringValue, float priority);
```






#### Append new child nodes's key and value (using JSON data) to the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which key and value in JSON data will be appended.

param **`jsonString`** - The appended JSON string (should be valid JSON data).

return **`Boolean`** type status indicates the success of operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushJSON(FirebaseData &dataObj, const String &path, const String &jsonString);
```







#### Append new child nodes's key and value (using FirebaseJson object) to the defined database path.

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which key and value in FirebaseJson object will be appended.

param **`json`** - The appended FirebaseJson object.

return **`Boolean`** type status indicates the success of operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushJSON(FirebaseData &dataObj, const String &path, FirebaseJson &json);
```








#### Append new child nodes's key and value (using JSON data or FirebaseJson object) and the virtual child ".priority" to the defined database path.

```C++
  bool pushJSON(FirebaseData &dataObj, const String &path, const String &jsonString, float priority);

  bool pushJSON(FirebaseData &dataObj, const String &path, FirebaseJson &json, float priority);
```






#### Append new blob (binary data) to the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which binary data will be appended.

param **`blob`** - Byte array of data.

param **`size`** - Size of byte array.

return **`Boolean`** type status indicates the success of operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushBlob(FirebaseData &dataObj, const String &path, uint8_t *blob, size_t size);
```







#### Append new blob (binary data) and the virtual child ".priority" to the defined database path.

```C++
bool pushBlob(FirebaseData &dataObj, const String &path, uint8_t *blob, size_t size, float priority);
```







#### Append new binary data from file stores on SD card/Flash memory to the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

@param **`storageType`** - Type of storage to read file data, StorageType::SPIFS or StorageType::SD.

param **`path`** - Target database path which binary data from file will be appended.

param **`fileName`** - File name (full file path) in SD card.

return **`Boolean`** type status indicates the success of operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushFile(FirebaseData &dataObj, uint8_t storageType, const String &path, const String &fileName);
```






#### Append new binary data from file store on SD card/Flash memory and the virtual child ".priority" to the defined database path.

```C++
bool pushFile(FirebaseData &dataObj, uint8_t storageType, const String &path, const String &fileName, float priority);
```







 #### Append new Firebase server's timestamp to the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which timestamp will be appended.

return - **`Boolean`** type status indicates the success of operation.
    
The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushTimestamp(FirebaseData &dataObj, const String &path);
```






#### Set integer data at the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which integer data will be set.

param **`intValue`** - Integer value to set.

return **`Boolean`** type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 
    
Call [FirebaseData object].intData will return the integer value of
payload returned from server.

```C++
bool setInt(FirebaseData &dataObj, const String &path, int intValue);
```







#### Set integer data and virtual child ".priority" at the defined database path.

```C++
bool setInt(FirebaseData &dataObj, const String &path, int intValue, float priority);
```






#### Set integer data at the defined database path if defined database path's ETag matched the ETag value

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which integer data will be set.

param **`intValue`** - Integer value to set.

param **`ETag`** - Known unique identifier string (ETag) of defined database path.

return - **`Boolean`** type status indicates the success of operation.


Call [FirebaseData object].dataType to determine what type of data that successfully stores in database. 
    
If ETag at the defined database path is not match the provided ETag parameter,the operation will failed with HTTP code 412, Precondition Failed (ETag is not match).

If operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value.
Also call [FirebaseData object].intData to get the current integer value.
    
```C++
bool setInt(FirebaseData &dataObj, const String &path, int intValue, const String &ETag);
```







#### Set integer data and the virtual child ".priority" if defined ETag matches at the defined database path 

```C++
bool setInt(FirebaseData &dataObj, const String &path, int intValue, float priority, const String &ETag);
```





#### Set float data at the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which float data will be set.

param **`floatValue`** - Float value to set.

return **`Boolean`** type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].floatData will return the float value of
payload returned from server.

```C++
bool setFloat(FirebaseData &dataObj, const String &path, float floatValue);
```






#### Set float data and virtual child ".priority" at the defined database path.

```C++
bool setFloat(FirebaseData &dataObj, const String &path, float floatValue, float priority);
```






#### Set float data at the defined database path if defined database path's ETag matched the ETag value

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which float data will be set.

param **`floatValue`** - Float value to set.

return **`Boolean`** type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].floatData will return the float value of
payload returned from server.

If ETag at the defined database path is not match the provided ETag parameter,the operation will failed with HTTP code 412, Precondition Failed (ETag is not match).

If operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value.
Also call [FirebaseData object].floatData to get the current float value.

```C++
bool setFloat(FirebaseData &dataObj, const String &path, float floatValue, const String &ETag);
```




#### Set float data and the virtual child ".priority" if defined ETag matches at the defined database path 

```C++
bool setFloat(FirebaseData &dataObj, const String &path, float floatValue, float priority, const String &ETag);
```






#### Set double data at the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which float data will be set.

param **`doubleValue`** - Double value to set.

return **`Boolean`** type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database.

Call [FirebaseData object].doubleData will return the double value of
payload returned from server.

Due to bugs in Serial.print in Arduino, to print large double value with zero decimal place, 
use printf("%.9lf\n", firebaseData.doubleData()); for print the returned double value up to 9 decimal places.

```C++
bool setDouble(FirebaseData &dataObj, const String &path, double doubleValue);
```






#### Set double data and virtual child ".priority" at the defined database path.

```C++
bool setDouble(FirebaseData &dataObj, const String &path, double doubleValue, float priority);
```







#### Set double data at the defined database path if defined database path's ETag matched the ETag value

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which float data will be set.

param **`doubleValue`** - Double value to set.

param **`ETag`** - Known unique identifier string (ETag) of defined database path.

return - **`Boolean`** type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully stores in database.

Call [FirebaseData object].doubleData will return the double value of payload returned from server.

If ETag at the defined database path is not match the provided ETag parameter, the operation will failed with HTTP code 412, Precondition Failed (ETag is not match).

If operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value.
Also call [FirebaseData object].doubeData to get the current double value.

```C++
bool setDouble(FirebaseData &dataObj, const String &path, double doubleValue, const String &ETag);
```





#### Set double data and the virtual child ".priority" if defined ETag matches at the defined database path 

```C++
bool setDouble(FirebaseData &dataObj, const String &path, double doubleValue, float priority, const String &ETag);
```






#### Set Boolean data at the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which float data will be set.

param **`boolValue`** - Boolean value to set.

return **`Boolean`** type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].boolData will return the Boolean value of
payload returned from server.

```C++
bool setBool(FirebaseData &dataObj, const String &path, bool boolValue);
```





#### Set boolean data and virtual child ".priority" at the defined database path.

```C++
bool setBool(FirebaseData &dataObj, const String &path, bool boolValue, float priority);
```






#### Set Boolean data at the defined database path if defined database path's ETag matched the ETag value

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which float data will be set.

param **`boolValue`** - Boolean value to set.

param **`ETag`** - Known unique identifier string (ETag) of defined database path.

return **`Boolean`** type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].boolData will return the Boolean value of
payload returned from server.


If ETag at the defined database path is not match the provided ETag parameter,
the operation will failed with HTTP code 412, Precondition Failed (ETag is not match).

If operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value.
Also call [FirebaseData object].doubeData to get the current boolean value.

```C++
bool setBool(FirebaseData &dataObj, const String &path, bool boolValue, const String &ETag);
```





#### Set boolean data and the virtual child ".priority" if defined ETag matches at the defined database path 

```C++
bool setBool(FirebaseData &dataObj, const String &path, bool boolValue, float priority, const String &ETag);
```





#### Set string (text) at the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which string data will be set.

param **`stringValue`** - String or text to set.

return **`Boolean`** type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].stringData will return the string value of
payload returned from server.

```C++
bool setString(FirebaseData &dataObj, const String &path, const String &stringValue);
```







#### Set string data and virtual child ".priority" at the defined database path.

```C++
bool setString(FirebaseData &dataObj, const String &path, const String &stringValue, float priority);
```





#### Set string (text) at the defined database path if defined database path's ETag matched the ETag value

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which string data will be set.

param **`stringValue`** - String or text to set.

param **`ETag`** - Known unique identifier string (ETag) of defined database path.

return **`Boolean`** type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].stringData will return the string value of
payload returned from server.

If ETag at the defined database path is not match the provided ETag parameter, 
the operation will failed with HTTP code 412, Precondition Failed (ETag is not match).

If operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value.
Also call [FirebaseData object].stringData to get the current string value.

```C++
bool setString(FirebaseData &dataObj, const String &path, const String &stringValue, const String &ETag);
```






#### Set string data and the virtual child ".priority" if defined ETag matches at the defined database path 

```C++
bool setString(FirebaseData &dataObj, const String &path, const String &stringValue, float priority, const String &ETag);
```






#### Set child nodes's key and value (using JSON data) to the defined database path

This will replace any child nodes inside the defined path with node' s key
and value defined in JSON data.

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which key and value in JSON data will be replaced or set.

param **`jsonString`** - The JSON string to set (should be valid JSON data).

return **`Boolean`** type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 
 
Call [FirebaseData object].jsonData will return the JSON string value of
payload returned from server.

Call [FirebaseData object].jsonObject will return the FirebaseJson object of
payload returned from server.

```C++
bool setJSON(FirebaseData &dataObj, const String &path, const String &jsonString);
```







#### Set child nodes's key and value (using FirebaseJson object) to the defined database path

This will replace any child nodes inside the defined path with node' s key
and value defined in FirebaseJson object.

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which key and value in FirebaseJson object will be replaced or set.

param **`json`** - The FirebaseJson object.

return **`Boolean`** type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 
 
Call [FirebaseData object].jsonData will return the JSON string value of
payload returned from server.

Call [FirebaseData object].jsonObject will return the FirebaseJson object of
payload returned from server.

```C++
bool setJSON(FirebaseData &dataObj, const String &path, FirebaseJson &json);
```








#### Set JSON data or FirebaseJson object and virtual child ".priority" at the defined database path.

```C++
bool setJSON(FirebaseData &dataObj, const String &path, const String &jsonString, float priority);

bool setJSON(FirebaseData &dataObj, const String &path, FirebaseJson &json, float priority);
```







#### Set child nodes's key and value (using JSON data or FirebaseJson object) to the defined database path if defined database path's ETag matched the ETag value

This will replace any child nodes inside the defined path with node' s key
and value defined in JSON data or FirebaseJson object.

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which key and value in JSON data or FirebaseJson objectwill be replaced or set.

param **`jsonString`** - The JSON string to set (should be valid JSON data).

param **`json`** - The FirebaseJson object.

param **`ETag`** - Known unique identifier string (ETag) of defined database path.

return **`Boolean`** type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 
 
Call [FirebaseData object].jsonData will return the JSON string value of
payload returned from server.

Call [FirebaseData object].jsonObject will return the FirebaseJson object of
payload returned from server.


If ETag at the defined database path is not match the provided ETag parameter,
the operation will failed with HTTP code 412, Precondition Failed (ETag is not match).

If operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value.
Also call [FirebaseData object].jsonData to get the current JSON string value [FirebaseData object].jsonObject 
to get the FirebaseJson object.


```C++
bool setJSON(FirebaseData &dataObj, const String &path, const String &jsonString, const String &ETag);

bool setJSON(FirebaseData &dataObj, const String &path, FirebaseJson &json, const String &ETag);
```






#### Set JSON data or FirebaseJson object and the virtual child ".priority" if defined ETag matches at the defined database path 

```C++
bool setJSON(FirebaseData &dataObj, const String &path, const String &jsonString, float priority, const String &ETag);

bool setJSON(FirebaseData &dataObj, const String &path, FirebaseJson &json, float priority, const String &ETag);
```







#### Set blob (binary data) at the defined database path

This will replace any child nodes inside the defined path with blob or binary data.

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which binary data will be set.

param **`blob`** - Byte array of data.

param **`size`** - Size of byte array.

return **`Boolean`** type status indicates the success of operation.

No payload returned from server.

```C++
bool setBlob(FirebaseData &dataObj, const String &path, uint8_t *blob, size_t size);
```





#### Set blob data and virtual child ".priority" at the defined database path.

```C++
bool setBlob(FirebaseData &dataObj, const String &path, uint8_t *blob, size_t size, float priority);
```







#### Set blob (binary data) at the defined database path if defined database path's ETag matched the ETag value

This will replace any child nodes inside the defined path with blob or binary data.

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which binary data will be set.

param **`blob`** - Byte array of data.

param **`size`** - Size of byte array.

param **`ETag`** - Known unique identifier string (ETag) of defined database path.

return **`Boolean`** type status indicates the success of operation.

No payload returned from server.

If ETag at the defined database path is not match the provided ETag parameter,
the operation will failed with HTTP code 412, Precondition Failed (ETag is not match).

```C++
bool setBlob(FirebaseData &dataObj, const String &path, uint8_t *blob, size_t size, const String &ETag);
```





#### Set blob data and the virtual child ".priority" if defined ETag matches at the defined database path 

```C++
bool setBlob(FirebaseData &dataObj, const String &path, uint8_t *blob, size_t size, float priority, const String &ETag);
```





#### Set binary data from file stores on SD card/Flash memory to the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

@param **`storageType`** - Type of storage to read file data, StorageType::SPIFS or StorageType::SD.

param **`path`** - Target database path which binary data from file will be set.

param **`fileName`** - File name (full file path) in SD card.

return **`Boolean`** type status indicates the success of operation.

No payload returned from server.

```C++
bool setFile(FirebaseData &dataObj, uint8_t storageType, const String &path, const String &fileName);
```





#### Set binary data from file and virtual child ".priority" at the defined database path.

```C++
bool setFile(FirebaseData &dataObj, uint8_t storageType, const String &path, const String &fileName, float priority);
```






#### Set binary data from file stores on SD card/Flash memory to the defined database path if defined database path's ETag matched the ETag value

param **`dataObj`** - Firebase Data Object to hold data and instances.

@param **`storageType`** - Type of storage to read file data, StorageType::SPIFS or StorageType::SD.

param **`path`** - Target database path which binary data from file will be set.

param **`fileName`** - File name in 8.3 DOS format (max. 8 bytes file name and 3 bytes file extension) stored in SD card.

param **`ETag`** - Known unique identifier string (ETag) of defined database path.

return **`Boolean`** type status indicates the success of operation.

No payload returned from server.

If ETag at the defined database path is not match the provided ETag parameter,
the operation will failed with HTTP code 412, Precondition Failed (ETag is not match).

```C++
bool setFile(FirebaseData &dataObj, uint8_t storageType, const String &path, const String &fileName, const String &ETag);
```




#### Set binary data from file and the virtual child ".priority" if defined ETag matches at the defined database path 

```C++
bool setFile(FirebaseData &dataObj, uint8_t storageType, const String &path, const String &fileName, float priority, const String &ETag);
```





 #### Set Firebase server's timestamp to the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which timestamp will be set.

return - **`Boolean`** type status indicates the success of operation.
    
Call [FirebaseData object].intData will return the integer value of timestamp in seconds
or [FirebaseData object].doubleData to get millisecond timestamp.

Due to bugs in Serial.print in Arduino, to print large double value with zero decimal place, 
use printf("%.0lf\n", firebaseData.doubleData());.

```C++
bool setTimestamp(FirebaseData &dataObj, const String &path);
```







#### Update child nodes's key or exising key's value (using JSON data) under the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which key and value in JSON data will be update.

param **`jsonString`** - The JSON string use for update.

return **`Boolean`** type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 
 
Call [FirebaseData object].jsonData will return the json string value of
payload returned from server.

Call [FirebaseData object].jsonObject will return the FirebaseJson object of
payload returned from server.

To reduce the network data usage, use updateNodeSilent instead.

```C++
bool updateNode(FirebaseData &dataObj, const String &path, const String &jsonString);
```






#### Update child nodes's key or exising key's value (using FirebaseJson object) under the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which key and value in FirebaseJson object will be update.

param **`json`** - The FirebaseJson object used for update.

return **`Boolean`** type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 
 
Call [FirebaseData object].jsonData will return the json string value of
payload returned from server.

Call [FirebaseData object].jsonObject will return the FirebaseJson object of
payload returned from server.

To reduce the network data usage, use updateNodeSilent instead.

```C++
bool updateNode(FirebaseData &dataObj, const String &path, FirebaseJson &json);
```







#### Update child nodes's key or exising key's value and virtual child ".priority" (using JSON data or FirebaseJson object) under the defined database path.

```C++
bool updateNode(FirebaseData &dataObj, const String &path, const String &jsonString, float priority);

bool updateNode(FirebaseData &dataObj, const String &path, FirebaseJson &json, float priority);
```





#### Update child nodes's key or exising key's value (using JSON data) under the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which key and value in JSON data will be update.

param **`jsonString`** - The JSON string use for update.

return **`Boolean`** type status indicates the success of operation.

Owing to the objective of this function to reduce the netwok data usage, 
no payload will be returned from server.

```C++
bool updateNodeSilent(FirebaseData &dataObj, const String &path, const String &jsonString);
```





#### Update child nodes's key or exising key's value (using FirebaseJson object) under the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Target database path which key and value in FirebaseJson object will be update.

param **`json`** - The FirebaseJson object used for update.

return **`Boolean`** type status indicates the success of operation.

Owing to the objective of this function to reduce the netwok data usage, 
no payload will be returned from server.

```C++
bool updateNodeSilent(FirebaseData &dataObj, const String &path, FirebaseJson &json);
```






#### Update child nodes's key and virtual child ".priority" (using JSON data or FirebaseJson object) under the defined database path.

```C++
bool updateNodeSilent(FirebaseData &dataObj, const String &path, const String &jsonString, float priority);

bool updateNodeSilent(FirebaseData &dataObj, const String &path, FirebaseJson &json, float priority);
```





#### Read the integer value at the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Database path which the integer value is being read.

return **`Boolean`** type status indicates the success of operation.

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







#### Read the integer value at the defined database path**

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Database path which the integer value is being read.

param **`target`** - The integer type variable to store value.

return **`Boolean`** type status indicates the success of operation.

If the type of payload returned from server is not integer, float and double, 
the target variable's value will be zero (0).

```C++
bool getInt(FirebaseData &dataObj, const String &path, int &target);
```






#### Read the float value at the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Database path which the float value is being read.

return **`Boolean`** type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].floatData will return the float value of
payload returned from server.

If the payload returned from server is not integer or float type, 
the function [FirebaseData object].intData will return zero (0).

```C++
bool getFloat(FirebaseData &dataObj, const String &path);
```





#### Read the float value at the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Database path which the float value is being read.

param **`target`** - The float type variable to store value.

return **`Boolean`** type status indicates the success of operation.

If the type of payload returned from server is not integer, float and double, 
the target variable's value will be zero (0).

```C++
bool getFloat(FirebaseData &dataObj, const String &path, float &target);
```





#### Read the double value at the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Database path which the float value is being rea.

return **`Boolean`** type status indicates the success of operation.

Call [FirebaseData object].doubleData will return the double value of
payload returned from server.

If the payload returned from server is not integer, float and double, 
the function [FirebaseData object].doubleData will return zero (0).

Due to bugs in Serial.print in Arduino, to print large double value with zero decimal place, 
use printf("%.9lf\n", firebaseData.doubleData()); for print the returned double value up to 9 decimal places.

```C++
bool getDouble(FirebaseData &dataObj, const String &path);
```







#### Read the float value at the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Database path which the float value is being read.

param **`target`** - The double type variable to store value.

return **`Boolean`** type status indicates the success of operation.

If the type of payload returned from server is not integer, float and double, 
the target variable's value will be zero (0).

```C++
bool getDouble(FirebaseData &dataObj, const String &path, double &target);
```







#### Read the Boolean value at the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Database path which the Boolean value is being read.

return **`Boolean`** type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].boolData will return the Boolean value of
payload returned from server.

If the payload returned from server is not boolean type, 
the function [FirebaseData object].boolData will return false.

```C++
bool getBool(FirebaseData &dataObj, const String &path);
```






#### Read the Boolean value at the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Database path which the Boolean value is being read.

param **`target`** - The boolean type variable to store value.

return **`Boolean`** type status indicates the success of operation.

If the type of payload returned from server is not Boolean, 
the target variable's value will be false.

```C++
bool getBool(FirebaseData &dataObj, const String &path, bool &target);
```







#### Read the string or text at the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Database path which the string value is being read.

return **`Boolean`** type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].stringData will return the string value of
payload returned from server.

If the payload returned from server is not string type, 
the function [FirebaseData object].stringData will return empty string (String object).

```C++
bool getString(FirebaseData &dataObj, const String &path);
```






#### Read the string or text at the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Database path which the string value is being read.

param **`target`** - The String object to store value.

return **`Boolean`** type status indicates the success of operation.

If the type of payload returned from server is not string,
the target String object's value will be empty.

```C++
bool getString(FirebaseData &dataObj, const String &path, String &target);
```





#### Read the JSON string at the defined database path

The returned payload JSON string represents the child nodes and their value.

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Database path which the JSON string value is being read.

return **`Boolean`** type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].jsonData will return the JSON string value of
payload returned from server.

Call [FirebaseData object].jsonObject will return the FirebaseJson object of
payload returned from server.

If the payload returned from server is not json type, 
the function [FirebaseData object].jsonData will return empty string (String object).

```C++
bool getJSON(FirebaseData &dataObj, const String &path);
```








#### Read the JSON string at the defined database path

The returned payload JSON string represents the child nodes and their value.

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Database path which the JSON string value is being read.

param **`target`** - The String object to store JSON string.

return **`Boolean`** type status indicates the success of operation.

If the type of payload returned from server is not string,
the target String object's value will be empty.

```C++
bool getJSON(FirebaseData &dataObj, const String &path, String &target);
```







#### Read the JSON string with data filtering at the defined database path

The returned payload JSON string represents the child nodes and their value.

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Database path which the JSON string value is being read.

param **`query`** - QueryFilter class to set query parameters to filter data.

return **`Boolean`** type status indicates the success of operation.

Available query parameters for filtering the data are the following.

**`QueryFilter.orderBy`** - Required parameter to specify which data used for data filtering included child key, key and value.

Use "$key" for filtering data by keys of all nodes at the defined database path.

Use "$value" for filtering data by value of all nodes at the defined database path.

Use "$priority" for filtering data by "virtual child" named .priority of all nodes.

Use any child key to filter by that key.


**`QueryFilter.limitToFirst`** -  The total children (number) to filter from the first child.

**`QueryFilter.limitToLast`** -   The total last children (number) to filter. 

**`QueryFilter.startAt`** -       Starting value of range (number or string) of query upon orderBy param.

**`QueryFilter.endAt`** -         Ending value of range (number or string) of query upon orderBy param.

**`QueryFilter.equalTo`** -       Value (number or string) matches the orderBy param


Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 

Call [FirebaseData object].jsonData will return the JSON string value of
payload returned from server.

Call [FirebaseData object].jsonObject will return the FirebaseJson object of
payload returned from server.

If the payload returned from server is not json type, 
the function [FirebaseData object].jsonData will return empty string (String object).

[FirebaseData object].jsonData will return null when the filtered data is empty.

```C++
bool getJSON(FirebaseData &dataObj, const String &path, QueryFilter &quer);
```







#### Read the JSON string at the defined database path as above**

The returned payload JSON string represents the child nodes and their value.

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Database path which the JSON string value is being read.

param **`target`** - The String object to store JSON string.

return **`Boolean`** type status indicates the success of operation.

If the type of payload returned from server is not json,
the target String object's value will be empty.

```C++
bool getJSON(FirebaseData &dataObj, const String &path, QueryFilter &query, String &target);
```







#### Read the blob (binary data) at the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Database path which the binary data is being read.

return **`Boolean`** type status indicates the success of operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in database. 
    
Call [FirebaseData object].blobData will return the dynamic array of unsigned 8-bit data (i.e. `std::vector<uint8_t>`) of
payload returned from server.

If the payload returned from server is not blob type, 
the function [FirebaseData object].blobData will return empty array.

```C++
bool getBlob(FirebaseData &dataObj, const String &path);
```






#### Read the blob (binary data) at the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Database path which the binary data is being read.

param **`target`** - Dynamic array of unsigned 8-bit data (i.e. std::vector<uint8_t>) to store value.

return **`Boolean`** type status indicates the success of operation.

If the type of payload returned from server is not blob,
the target variable value will be empty array.

```C++
bool getBlob(FirebaseData &dataObj, const String &path, std::vector<uint8_t> &target);
```







#### Download file data in database at defined database path and save to SD card/Flash memory.

The downloaded data will be decoded to binary and save to SD card/Flash memory, then

please make sure that data at the defined database path is file type.

param **`dataObj`** - Firebase Data Object to hold data and instances.

@param **`storageType`** - Type of storage to write file data, StorageType::SPIFS or StorageType::SD.

param **`nodePath`** - Database path that file data will be downloaded.

param **`fileName`** - File name included its path in SD card/Flash memory.

return **`Boolean`** type status indicates the success of operation.

```C++
bool getFile(FirebaseData &dataObj, uint8_t storageType, const String &nodePath, const String &fileName);
```








#### Delete all child nodes at the defined database path

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Database path to be deleted.

return **`Boolean`** type status indicates the success of operation.*

```C++
bool deleteNode(FirebaseData &dataObj, const String &path);
```






#### Delete all child nodes at the defined database path if defined database path's ETag matched the ETag value

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Database path to be deleted.

param **`ETag`** - Known unique identifier string (ETag) of defined database path.

return **`Boolean`** type status indicates the success of operation.*

If ETag at the defined database path is not match the provided ETag parameter,
the operation will failed with HTTP code 412, Precondition Failed (ETag is not match).

```C++
bool deleteNode(FirebaseData &dataObj, const String &path, const String &ETag);
```





#### Start monitoring the value changes at the defined path and its children

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`path`** - Database path being monitor.

return **`Boolean`** type status indicates the success of operation.*

```C++
bool beginStream(FirebaseData &dataObj, const String &path);
```







#### Read the stream event data at defined database path

Once beginStream was called e.g. in setup(), the readStream function
should call inside the loop function.

param **`dataObj`** - Firebase Data Object to hold data and instances.

return **`Boolean`** type status indicates the success of operation.

Using the same Firebase Data object for stream read/monitoring associated 
with read, store, update, delete will break or quit the current stream connection. 
    
The stream will be resumed or reconnected automatically when calling readStream.

```C++
bool readStream(FirebaseData &dataObj);
```







#### End the stream connection at defined path

Can be restart again by calling beginStream.

param **`dataObj`** - Firebase Data Object to hold data and instances.

return **`Boolean`** type status indicates the success of operation.
 
```C++
bool endStream(FirebaseData &dataObj);
```







#### Set the stream callback functions

setStreamCallback should be called before Firebase.beginStream.

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`dataAvailablecallback`** - Callback function that accepts streamData parameter.

param **`timeoutCallback`** - Callback function will be called when stream connection was timeout (optional).

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







#### Remove stream callback functions

param **`dataObj`** - Firebase Data Object to hold data and instances.

```C++
void removeStreamCallback(FirebaseData &dataObj);
```






#### Start the Firbase Error Queues Auto Run Process

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`callback`** - Callback function that accepts QueueInfo Object as parameter, optional.

The following functions are available from QueueInfo Object accepted by callback.

**queueInfo.totalQueues()**, get the total Error Queues in Error Queue Collection.

**queueInfo.currentQueueID()**, get current Error Queue ID that being process.

**queueInfo.isQueueFull()**, determine whether Error Queue Collection is full or not.

**queueInfo.dataType()**, get string of the Frebase call data type that being process of current Error Queue.

**queueInfo.method()**, get string of the Firebase call method that being process of current Error Queue.

**queueInfo.path()**, get string of the Firebase call path that being process of current Error Queue.


```C++
void beginAutoRunErrorQueue(FirebaseData &dataObj, QueueInfoCallback callback = NULL);
```





#### Stop the Firbase Error Queues Auto Run Process

param **`dataObj`** - Firebase Data Object to hold data and instances.


```C++
void endAutoRunErrorQueue(FirebaseData &dataObj);
```




#### Clear all Firbase Error Queues in Error Queue collection

param **`dataObj`** - Firebase Data Object to hold data and instances.


```C++
void clearErrorQueue(FirebaseData &dataObj);
```





#### Backup (download) database at defined database path to SD card/Flash memory

param **`dataObj`** - Firebase Data Object to hold data and instances.

@param **`storageType`** - Type of storage to save file, StorageType::SPIFS or StorageType::SD.

param **`nodePath`** - Database path to be backuped.

param **`fileName`** - File name to save.

Only 8.3 DOS format (max. 8 bytes file name and 3 bytes file extension) can be saved to SD card.

return **`Boolean`** type status indicates the success of operation.


```C++
bool backup(FirebaseData &dataObj, uint8_t storageType, const String &nodePath, const String &fileName);
```







#### Restore database at defined path usin backup file saved on SD card/Flash memory

param **`dataObj`** - Firebase Data Object to hold data and instances.

@param **`storageType`** - Type of storage to read file, StorageType::SPIFS or StorageType::SD.

param **`nodePath`** - Database path to  be restored.

param **`fileName`** - File name to read.

return **`Boolean`** type status indicates the success of operation.

```C++
bool restore(FirebaseData &dataObj, uint8_t storageType const String &nodePath, const String &fileName);
```






#### Init SD card with GPIO pins

param **`sck`** -SPI Clock pin.

param **`miso`** - SPI MISO pin.

param **`mosi`** - SPI MOSI pin.

param **`ss`** - SPI Chip/Slave Select pin.

return **`Boolean`** type status indicates the success of operation.


```C++
void sdBegin(uint8_t sck, uint8_t miso, uint8_t mosi, uint8_t ss);
```






#### Init SD card with default GPIO pins

return **`Boolean`** type status indicates the success of operation.


```C++
void sdBegin(void);
```





#### Set maximum Firebase read/store retry operation (0 - 255) in case of network problems and buffer overflow

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`num`** - The maximum retry.

```C++
void setMaxRetry(FirebaseData &dataObj, uint8_t num);
```





#### Set the maximum Firebase Error Queues in collection (0 - 255)

Firebase read/store operation causes by network problems and buffer overflow will be added to Firebase Error Queues collection.

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`num`** - The maximum Firebase Error Queues.

```C++
void setMaxErrorQueue(FirebaseData &dataObj, uint8_t num);
```




   
#### Save Firebase Error Queues as SPIFFS file (save only database store queues)

Firebase read (get) operation will not be saved.

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`filename`** - File name to be saved.

param **`storageType`** - Type of storage to save file, StorageType::SPIFS or StorageType::SD.
    
```C++
bool saveErrorQueue(FirebaseData &dataObj, const String &filename, uint8_t storageType);
```
   





#### Delete file in Flash (SPIFFS) or SD card

param **`filename`** - File name to delete.

param **`storageType`** - Type of storage to save file, StorageType::SPIFS or StorageType::SD.
    
```C++
bool deleteStorageFile(const String &filename, uint8_t storageType);
```




   
#### Restore Firebase Error Queues from SPIFFS file

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`filename`** - File name to be read and restore queues.

param **`storageType`** - Type of storage to read file, StorageType::SPIFS or StorageType::SD.
    
```C++
bool restoreErrorQueue(FirebaseData &dataObj, const String &filename, uint8_t storageType);
```





#### Determine number of Firebase Error Queues stored in defined SPIFFS file

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`filename`** - File name to be read and count for queues.

param **`storageType`** - Type of storage to read file, StorageType::SPIFS or StorageType::SD.


return **`Number`** (0-255) of queues store in defined SPIFFS file.

```C++
uint8_t errorQueueCount(FirebaseData &dataObj, const String &filename, uint8_t storageType);
```






#### Determine number of queues in Firebase Data object Firebase Error Queues collection

param **`dataObj`** - Firebase Data Object to hold data and instances.

return **`Number`** (0-255) of queues in Firebase Data object queue collection.

```C++
uint8_t errorQueueCount(FirebaseData &dataObj);
```






#### Determine whether the Firebase Error Queues collection was full or not

param **`dataObj`** - Firebase Data Object to hold data and instances.

return **`Boolean`** type status indicates whether the  Firebase Error Queues collection was full or not.

```C++
bool isErrorQueueFull(FirebaseData &dataObj);
```






#### Pocess all failed Firebase operation queue items when network is available

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`callback`** - Callback function that accepts QueueInfo parameter.
  
```C++
void processErrorQueue(FirebaseData &dataObj, QueueInfoCallback callback = NULL);
```






#### Return Firebase Error Queue ID of last Firebase Error

Return 0 if there is no Firebase Error from last operation.

param **`dataObj`** - Firebase Data Object to hold data and instances.
    
return **`Number`** of Queue ID.

```C++
uint32_t getErrorQueueID(FirebaseData &dataObj);
```




#### Determine whether Firebase Error Queue is currently exsted is Error Queue collection or not

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`errorQueueID`** - The Firebase Error Queue ID get from getErrorQueueID.
    
return - **`Boolean type`** status indicates the queue existence.

```C++
bool isErrorQueueExisted(FirebaseData &dataObj, uint32_t errorQueueID);
```






#### Send Firebase Cloud Messaging to device with first registeration token which added by firebaseData.fcm.addDeviceToken

param **`dataObj`** - Firebase Data Object to hold data and instances.

param **`index`** - The index (starts from 0) of recipient device token which added by firebaseData.fcm.addDeviceToken
    
return - **`Boolean type`** status indicates the success of operation.

```C++
bool sendMessage(FirebaseData &dataObj, uint16_t index);
```





#### Send Firebase Cloud Messaging to all devices (multicast) which added by firebaseData.fcm.addDeviceToken

param **`dataObj`** - Firebase Data Object to hold data and instances.
    
return - **`Boolean type`** status indicates the success of operation.

```C++
bool broadcastMessage(FirebaseData &dataObj);
```




#### Send Firebase Cloud Messaging to devices that subscribed to topic

param **`dataObj`** - Firebase Data Object to hold data and instances.
    
return - **`Boolean type`** status indicates the success of operation.

```C++
bool sendTopic(FirebaseData &dataObj);
```




## Firebase Data Object Functions

#### Pause/Unpause WiFiClient from all Firebase operations

param **`pause`** True for pause and False for unpause.

return **`Boolean`** type status indicates the success of operation.

```C++
bool pauseFirebase(bool pause);
```





## Firebase Data Object Functions


#### Get WiFi client instance


return **`WiFi client instance`**.

```C++
WiFiClient &getWiFiClient();
```







#### Determine the data type of payload returned from server

return **`The one of these data type e.g. integer, float, double, boolean, string, json and blob.`**

```C++
String dataType();
```






#### Determine the event type of stream

return **`The one of these event type String e.g. put, patch, cancel, and auth_revoked.`**

The event type "put" indicated that data at event path relative to stream path was completely changed. 

Event path can be determined from dataPath().

The event type "patch" indicated that data at event path relative to stream path was updated. 

Event path can be determined from dataPath().

The event type "cancel" indeicated something wrong and cancel by server.

The event type "auth_revoked" indicated the provided Firebase Authentication Data (Database secret) is no longer valid.

```C++
String eventType();
```





#### Determine the unique identifier (ETag) of current data

return **`String.`** of unique identifier.

```C++
String ETag();
```






#### Determine the current stream path

return **`The database streaming path.`**

```C++
String streamPath();
```







#### Determine the current data path

return **`The database path which belong to server' s returned payload.`**

The database path returned from this function in case of stream, also changed upon the child or parent's stream
value changes.

```C++
String dataPath();
```







#### Determine the error reason String from process

return **`The error description string (String object).`**

```C++
String errorReason();
```







#### Return the ineger data of server returned payload

return **`Integer value.`**

```C++
int intData();
```







#### Return the float data of server returned payload

return **`Float value.`**

```C++
float floatData();
```





#### Return the double data of server returned payload

return **`Double value.`**

```C++
float doubleData();
```





#### Return the Boolean data of server returned payload

return **`Boolean value.`**

```C++
float boolData();
```






#### Return the String data of server returned payload

return **`String (String object).`**

```C++
String stringData();
```







#### Return the JSON String data of server returned payload

return **`String (String object).`**

```C++
String jsonData();
```






#### Return the Firebase JSON object of server returned payload.

return **`FirebaseJson object.`**

```C++
FirebaseJson &jsonObject();
```






#### Return the blob data (uint8_t) array of server returned payload

return **`Dynamic array`** of 8-bit unsigned integer i.e. `std::vector<uint8_t>`.

```C++
std::vector<uint8_t> blobData();
```







#### Return the new appended node's name or key of server returned payload when calling pushXXX function

return **`String`** (String object).

```C++
String pushName();
```







#### Determine the stream connection status

return **`Boolean`** type status indicates whether the Firebase Data object is working with stream or not.

```C++
bool isStream();
```







#### Determine the server connection status

return **`Boolean`** type status indicates whether the Firebase Data object is connected to server or not.

```C++
bool httpConnected();
```







#### Determine the timeout event of server's stream (30 sec is default)

Nothing to do when stream connection timeout, the stream connection will be automatic resumed.

return **`Boolean`** type status indicates whether the stream was timeout or not.

```C++
bool streamTimeout();
```







#### Determine the availability of data or paylaod returned from server

return **`Boolean`** type status indicates whether the server return back the new payload or not.

```C++
bool dataAvailable();
```







#### Determine the availability of stream event-data paylaod returned from server

return **`Boolean`** type status indicates whether the server return back the stream event-data 
payload or not.

```C++
bool streamAvailable();
```







#### Determine the matching between data type that intend to get from/store to database and the server's return payload data type

return **`Boolean`** type status indicates whether the type of data being get from/store to database 
and server's returned payload are matched or not.

```C++
bool mismatchDataType();
```







#### Determine the http status code return from server

return **`Integer`** number of HTTP status.

```C++
int httpCode();
```







#### Determine the name (full path) of backup file in SD card

return **`String`** (String object) of file name that store on SD card after backup operation.

```C++
String getBackupFilename();
```







#### Determine the size of backup file

return **`Number of byte`** of backup file in byte after backup operation.

```C++
size_t getBackupFileSize();
```







#### Clear or empty data in Firebase Data object

```C++
void clear();
```







#### Determine the error description for file transfering (pushFile, setFile, backup and restore)

return **`Error description string* (String object).`**

```C++
String fileTransferError();
```







#### Return the server's payload data

return **`Payload string* (String object).`**

```C++
String payload();
```





## Firebase Cloud Messaging Object Functions


#### Store Firebase Cloud Messaging's authentication credentials
    
param **`serverKey`** - Server key found on Console: Project settings > Cloud Messaging

```C++
void begin(const String &serverKey);
```






#### Add recipient's device registration token or instant ID token
    
param **`deviceToken`** - Recipient's device registration token to addd that message will be sent to.

```C++
void addDeviceToken(const String &deviceToken);
```





 #### Remove recipient's device registration token or instant ID token
    
param **`index`** - Index (start from zero) of recipient's device registration token that added to FCM Data Object of Firebase Data object.

```C++
void removeDeviceToken(uint16_t index);
```




#### Clear all recipient's device registration tokens
    
 ```C++
 void clearDeviceToken();
```





#### Set the notify message type information
    
param **`title`** - The title text of notification message.

param **`body`** - The body text of notification message.

```C++
void setNotifyMessage(const String &title, const String &body);
```




#### Set the notify message type information
    
param **`title`** - The title text of notification message.

param **`body`** - The body text of notification message.

param **`icon`** - The name and/or included URI/URL of icon to show on notify message.

```C++
void setNotifyMessage(const String &title, const String &body, const String &icon);
```





#### Set the notify message type information
    
param **`title`** - The title text of notification message.

param **`body`** - The body text of notification message.

param **`icon`** - The name and/or included URI/URL of icon to show on notify message.

param **`click_action`** - The URL or intent to accept click event on notification message.

```C++
void setNotifyMessage(const String &title, const String &body, const String &icon, const String &click_action);
```





#### Clear all notify message information
    
```C++
void clearNotifyMessage();
```





#### Set the custom data message type information
    
param **`jsonString`** - The JSON structured data string.

```C++
void setDataMessage(const String &jsonString);
```





#### Set the custom data message type information
    
param **`json`** - The FirebaseJson object.

```C++
void setDataMessage(FirebaseJson &json);
```





#### Clear custom data message type information
    
```C++
void clearDataMessage();
```





#### Set the prioiry of message (notification and custom data)
    
param **`priority`** - The priority string i.e. normal and high.

```C++
void setPriority(const String &priority);
```






#### Set the collapse key of message (notification and custom data)
    
param **`key`** - String of collapse key.

```C++
void setCollapseKey(const String &key);
```





#### Set the Time To Live of message (notification and custom data)
    
param **`seconds`** - Number of seconds from 0 to 2,419,200 (4 weeks).

```C++
void setTimeToLive(uint32_t seconds);
```




#### Set topic of message will be send to
    
param **`topic - Topic string.

```C++
void setTopic(const String &topic);
```




#### Get the send result
    
return **`String`** of payload returned from server.

```C++
String getSendResult();
```





## JSON object Functions


#### Clear buffer data and tokens pointer.
    
return **`instance of an object.`** 

```C++
FirebaseJson &clear();
```




#### Set JSON data to buffer.

param **`data`** - The JSON object string. 
    
return **`instance of an object.`** 

The internal buffer will be overwritten by add functions.

```C++
FirebaseJson &setJsonData(const String &data);
```




#### Add the string to JSON object.

param **`key`** - Key string of data. 

param **`value`** - The String data. 
    
return **`instance of an object.`** 


```C++
FirebaseJson &addString(const String &key, const String &value);
```




#### Add the array of objects to JSON object.

param **`key`** - Key string of data. 

param **`arr`** - The FirebaseJsonArray object. 
    
return **`instance of an object.`** 


```C++
FirebaseJson &addString(const String &key, const String &value);FirebaseJson &addArray(const String &key, FirebaseJsonArray *arr);
```





#### Add the integer value to JSON object.
    
param **`key`** - Key string of data.

param **`value`** - Integer data.    

return **`instance of an object.`**

```C++
FirebaseJson &addInt(const String &key, int value);
```




#### Add the double value to JSON object.
    
param **`key`** - Key string of data.

param **`value`** - Double data.    

return **`instance of an object.`**

```C++
FirebaseJson &addDouble(const String &key, double value);
```
 



#### Add the boolean value to JSON object.
    
param **`key`** - Key string of data.

param **`value`** - Boolean data.    

return **`instance of an object.`**

```C++
FirebaseJson &addBool(const String &key, bool value);
```





#### Add the JSON data to JSON object.
    
param **`key`** - Key string of data.

param **`json`** - FirebaseJson object that hold the data to be added to this JSON object.    

return **`instance of an object.`**

```C++
FirebaseJson &addJson(const String &key, FirebaseJson *json);
```


#### Get the JSON object's string. 

return **`serialized string of JSON object.`**

```C++
String toString();
```





#### Parse the internal buffer JSON object.
    
param **`skipChild`** - Flag to skip any children nodes under root or array.

param **`len`** - Optional. The expected number of JSON tokens (key and value). Should be number of keys and values + 1.

Default value is 50. If object parse failed, increase this value.     

return **`instance of an object.`**

```C++
FirebaseJson &parse(bool skipChild = true, size_t len = 0);
```





####  Get the value from key of JSON object.
    
param **`key`** - Key string of data to get.

return **`instance of an object.`**

This should call after parse().

```C++
FirebaseJson &get(const String &key);
```




#### Set the numbers of JSON tokens (keys + values + 1) to be available for read from jsonObjectIterator.  

return **`numbers of tokens.`**

```C++
size_t getJsonObjectIteratorCount();
```





#### Read data from JSON objects by providing tokens ID.
    
param **`index`** - The referenced token index. This will auto increase to the next token index after read.

param **`key`** - The referenced key data string. This provided the key data output.

param **`value`** - The referenced value string. This provided the value of current key output.   

return **`instance of an object.`**

```C++
FirebaseJson &jsonObjectiterator(size_t &index,String &key, String &value);
```





#### Get the parse result of JSON object.   

return **`FirebaseJsonObject.`**

The properties available are: stringValue, intValue, doubleValue, boolValue, type, success.

The type property provides the types of value string e.g. string, int, double, boolean, array, object, null and undefined.

The success property provides the success status of get or parse functions.

```C++
FirebaseJsonObject parseResult();
```


## JSON Array Functions



#### Add the string value to JSON Array object.

param **`value`** - String data.    

return **`instance of an object.`**

```C++
FirebaseJsonArray &addString(const String &value);
```




#### Add the integer value to JSON Array object.

param **`value`** - Integer data.    

return **`instance of an object.`**

```C++
FirebaseJsonArray &addInt(int value);
```





#### Add the double value to JSON Array object.

param **`value`** - double data.    

return **`instance of an object.`**

```C++
FirebaseJsonArray &addDouble(double value);
```





#### Add the boolean value to JSON Array object.

param **`value`** - Boolean data.    

return **`instance of an object.`**

```C++
FirebaseJsonArray &addBool(bool value);
```




#### Add the JSON object to JSON Array object.

param **`json`** - FirebaseJson object that holds data to be added to array.    

return **`instance of an object.`**

```C++
FirebaseJsonArray &addJson(FirebaseJson *json);
```




#### Add the JSON array object to JSON Array object.

param **`arr`** - JSON array object that holds data to be added to this JSON array.    

return **`instance of an object.`**

```C++
FirebaseJsonArray &addArray(FirebaseJsonArray *arr);
```





#### Get the JSON Array object's string.  

return **`string of an object.`**

```C++
String toString();
```





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

