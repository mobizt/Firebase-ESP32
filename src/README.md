# Firebase Realtime Database Arduino Library for ESP32


Google's Firebase Realtime Database Arduino Library for ESP32 v 3.8.26


## Global functions


#### Initialize Firebase with the config and Firebase's authentication credentials.

param **`config`** The pointer to FirebaseConfig data.

param **`auth`** The pointer to FirebaseAuth data.

 note: For FirebaseConfig and FirebaseAuth data usage, see the examples.

```C++
void begin(FirebaseConfig *config, FirebaseAuth *auth);
```





#### Provide the details of token generation.

return **`token_info_t`** The token_info_t structured data that indicates the status.

note: Use type property to get the type enum value.

token_type_undefined or 0,

token_type_legacy_token or 1,

token_type_id_token or 2,

token_type_custom_token or 3,

token_type_oauth2_access_token or 4


Use status property to get the status enum value.

token_status_uninitialized or 0,

token_status_on_signing or 1,

token_status_on_request or 2,

token_status_on_refresh or 3,

token_status_ready or 4


In case of token generation and refreshment errors, use error.code property to get the error code number.

Use error.message property to get the error message string.

```C++
struct token_info_t authTokenInfo();
```





#### Store Firebase's authentication credentials using database secret (obsoleted).

param **`host`** Your Firebase database project host e.g. Your_ProjectID.firebaseio.com.

param **`auth`** Your database secret.

param **`caCert`** Root CA certificate base64 string (PEM file).

param **`caCertFile`** Root CA certificate DER file (binary).

param **`StorageType`** Type of storage, StorageType::SD and StorageType::FLASH.

The file systems for flash and sd memory can be changed in FirebaseFS.h.

```C++
void begin(const String &host, const String &auth);

void begin(const String &host, const String &auth, const char *caCert);

void begin(const String &host, const String &auth, const String &caCertFile, uint8_t storageType);
```





#### Stop Firebase and release all resources.

param **`fbdo`** Firebase Data Object to hold data and instance.

```C++
void end(FirebaseData &fbdo);
```






#### Sign up for a new user.

param **`config`** The pointer to FirebaseConfig data.

param **`auth`** The pointer to FirebaseAuth data.

param **`email`** The user Email.

param **`password`** The user password.

return **`Boolean`** type status indicates the success of the operation. 

note: By calling Firebase.begin with config and auth after sign up will be signed in.

This required Email/Password provider to be enabled,

From Firebase console, select Authentication, select Sign-in method tab, under 
the Sign-in providers list, enable Email/Password provider.

If the assigned email and passowrd are empty, the anonymous user will be created if Anonymous provider is enabled.

To enable Anonymous provider, from Firebase console, select Authentication, 
select Sign-in method tab, under the Sign-in providers list, enable Anonymous provider.

```C++
bool signUp(FirebaseConfig *config, FirebaseAuth *auth, const char *email, const char *password);
```





#### Send a user a verification Email.

param **`config`** The pointer to FirebaseConfig data.

param **`idToken`** The id token of user that was already signed in with Email and password (optional).

return **`Boolean`** type status indicates the success of the operation. 

note: The id token can be obtained from config.signer.tokens.id_token after begin with config and auth data

If the idToken is not assigned, the internal config.signer.tokens.id_token will be used. 

See the Templates of Email address verification in the Firebase console, Authentication.

```C++
bool sendEmailVerification(FirebaseConfig *config, const char *idToken = "");
```





#### Send a user a password reset link to Email.

param **`config`** The pointer to FirebaseConfig data.

param **`email`** The user Email to send the password resset link.

return **`Boolean`** type status indicates the success of the operation. 

```C++
bool sendResetPassword(FirebaseConfig *config, const char *email);
```






#### Set the stream task (RTOS task) reserved stack memory in bytes.

#### [Obsoleted]
    
param **`size`** The number of stack size in bytes.

The stream task will be created only when the user sets the stream callbacks.

```C++
  void setStreamTaskStackSize(size_t size);
```







#### Enable multiple HTTP requests at a time.
    
param **`enable`** The boolean value to enable/disable.

The multiple HTTP requessts at a time is disable by default to prevent 
the large memory used in multiple requests.

```C++
  void allowMultipleRequests(bool enable);
```







#### Reconnect WiFi if lost connection

param **`reconnect`** The boolean to set/unset WiFi AP reconnection.

```C++
void reconnectWiFi(bool reconnect);
```






#### Set the decimal places for float value to be stored in database.

param **`digits`** The decimal places. 

```C++
void setFloatDigits(uint8_t digits);
```
  




#### Set the decimal places for double value to be stored in database.

param **`digits`** The decimal places. 

```C++
void setDoubleDigits(uint8_t digits);
```






#### Set the timeouts of Firebase.get function.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`millisec`** The milliseconds to limit the request (0 - 900,000 ms or 15 min).

```C++
void setReadTimeout(FirebaseData &fbdo, int millisec);
```






#### Set the size limit of payload data that will write to the database for each request.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`size`** The size identified string e.g. tiny, small, medium, large and unlimited.

Size string and its write timeout in seconds e.g. tiny (1s), small (10s), medium (30s) and large (60s).

```C++
void setwriteSizeLimit(FirebaseData &fbdo, const String &size);
```






#### Read the database rules

param **`fbdo`** Firebase Data Object to hold data and instances.

return - **`Boolean`** type status indicates the success of the operation.

```C++
bool getRules(FirebaseData &fbdo);
```







#### Write the database rules

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`rules`** Database rules in jSON String format.

return - **`Boolean`** type status indicates the success of the operation.

```C++
bool setRules(FirebaseData &fbdo, const String &rules);
```







#### Determine whether defined database path exists or not

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path to be checked.

return - **`Boolean`** type result indicates whether the defined database path has existed or not.

```C++
bool pathExist(FirebaseData &fbdo, const String &path);
```





#### Determine the unique identifier (ETag) of current data at the defined database path

return **`String`** of unique identifier.

```C++
String getETag(FirebaseData &fbdo, const String &path);
```






#### Get the shallowed data at defined node path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path being read the data.

return - **`Boolean`** type status indicates the success of the operation.


Return the child data with its value or JSON object (its values will be truncated to true).

Call [FirebaseData object].dataType to determine what type of data that returned from the database. 

Call **[FirebaseData object].dataType** to determine what type of data that returned from database. 

Call **[FirebaseData object].intData** to get integer data.

Call **[FirebaseData object].floatData** to get float data.

Call **[FirebaseData object].doubleData** to get double data.

Call **[FirebaseData object].boolData** to get boolean data.

Call **[FirebaseData object].stringData** to get string data.

Call **[FirebaseData object].jsonObject** to get FirebaseJson object.

```C++
bool getShallowData(FirebaseData &fbdo, const String &path); 
```







#### Enable the library to use only classic HTTP GET and POST methods

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`flag`** Boolean value to enable.

This option used to escape the Firewall restriction (if device is connected through Firewall) 
that allows only HTTP GET and POST
    
HTTP PATCH request was sent as PATCH which not affected by this option.

```C++
void enableClassicRequest(FirebaseData &fbdo, bool flag);
```





#### Set the virtual child node ".priority" to the defined database path. 
    
param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which to set the priority value.

param **`priority`** The priority value.
    
return - **`Boolean`** type status indicates the success of the operation.


This allows us to set priority to any node other than a priority that set through setJSON, 
pushJSON, updateNode, and updateNodeSilent functions.
    
The returned priority value from server can read from function [FirebaseData object].priority().

```C++
bool setPriority(FirebaseData &fbdo, const String &path, float priority);
```







#### Read the virtual child node ".priority" value at the defined database path. 
    
param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which to set the priority value.
    
return - **`Boolean`** type status indicates the success of the operation.


The priority value from server can read from function [FirebaseData object].priority().

 ```C++
bool getPriority(FirebaseData &fbdo, const String &path);
```






#### Append new integer value to the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which integer value will be appended.

param **`intValue`** The appended value.

return **`Boolean`** type status indicates the success of the operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushInt(FirebaseData &fbdo, const String &path, int intValue);

bool push(FirebaseData &fbdo, const String &path, int intValue);
```






#### Append new integer value and the virtual child ".priority" to the defined database path.

```C++
bool pushInt(FirebaseData &fbdo, const String &path, int intValue, float priority);

bool push(FirebaseData &fbdo, const String &path, int intValue, float priority);
```






#### Append new float value to the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which float value will be appended.

param **`floatValue`** The appended value.

return **`Boolean`** type status indicates the success of the operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushFloat(FirebaseData &fbdo, const String &path, float floatValue);

bool push(FirebaseData &fbdo, const String &path, float floatValue);
```







#### Append new float value and the virtual child ".priority" to the defined database path.

```C++
bool pushFloat(FirebaseData &fbdo, const String &path, float floatValue, float priority);

bool push(FirebaseData &fbdo, const String &path, float floatValue, float priority);
```








#### Append new double value (8 bytes) to the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which float value will be appended.

param **`doubleValue`** The appended value.

return **`Boolean`** type status indicates the success of the operation.

The new appended node's key will be stored in Firebase Data object,
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushDouble(FirebaseData &fbdo, const String &path, double doubleValue);

bool push(FirebaseData &fbdo, const String &path, double doubleValue);
```





#### Append new double value (8 bytes) and the virtual child ".priority" to the defined database path.

```C++
bool pushDouble(FirebaseData &fbdo, const String &path, double doubleValue, float priority);

bool push(FirebaseData &fbdo, const String &path, double doubleValue, float priority);
```






#### Append new Boolean value to the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which Boolean value will be appended.

param **`boolValue`** The appended value.

return **`Boolean`** type status indicates the success of the operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushBool(FirebaseData &fbdo, const String &path, bool boolValue);

bool push(FirebaseData &fbdo, const String &path, bool boolValue);
```







#### Append new Boolean value and the virtual child ".priority" to the defined database path.

```C++
bool pushBool(FirebaseData &fbdo, const String &path, bool boolValue, float priority);

bool push(FirebaseData &fbdo, const String &path, bool boolValue, float priority);
```








#### Append new string (text) to the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which string will be appended.

param **`stringValue`** The appended value.

return **`Boolean`** type status indicates the success of the operation.

The new appended node's key stored in Firebase Data object, 
which can be accessed via function [FirebaseData object].pushName().

```C++
bool pushString(FirebaseData &fbdo, const String &path, const String &stringValue);

bool push(FirebaseData &fbdo, const String &path, const char *stringValue);

bool push(FirebaseData &fbdo, const String &path, const String &stringValue);
```







#### Append new string (text) and the virtual child ".priority" to the defined database path.

```C++
bool pushString(FirebaseData &fbdo, const String &path, const String &stringValue, float priority);

bool push(FirebaseData &fbdo, const String &path, const char *stringValue, float priority);

bool push(FirebaseData &fbdo, const String &path, const String &stringValue, float priority);
```







#### Append new child node key and value (using FirebaseJson object) to the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which key and value in FirebaseJson object will be appended.

param **`json`** The appended FirebaseJson object.

return **`Boolean`** type status indicates the success of the operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json);

bool push(FirebaseData &fbdo, const String &path, FirebaseJson &json);
```









#### Append new child node key and value (using JSON data or FirebaseJson object) and the virtual child ".priority" to the defined database path.

```C++
bool pushJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority);

bool push(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority);
```




#### Append child node array (using FirebaseJsonArray object) to the defined database path.

This will replace any child nodes inside the defined path with array defined in FirebaseJsonArray object.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which key and value in FirebaseJsonArray object will be appended.

param **`arr`** The appended FirebaseJsonArray object.

return **`Boolean`** type status indicates the success of the operation.

The new appended node's key will be stored in Firebase Data object,
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr);

bool push(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr);
```






#### Append FirebaseJsonArray object and virtual child ".priority" at the defined database path.

```C++
bool pushArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority);

bool push(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority);
```









#### Append new blob (binary data) to the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which binary data will be appended.

param **`blob`** Byte array of data.

param **`size`** Size of array in byte.

return **`Boolean`** type status indicates the success of the operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size);

bool push(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size);
```






#### Append new blob (binary data) and the virtual child ".priority" to the defined database path.

```C++
bool pushBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority);

bool push(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority);
```






#### Append new binary data from file stored on SD card/Flash memory to the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`storageType`** Type of storage to read file data, StorageType::FLASH or StorageType::SD.

param **`path`** Target database path in which binary data from the file will be appended.

param **`fileName`** File name included its path in SD card/Flash memory.

return **`Boolean`** type status indicates the success of the operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

The file systems for flash and sd memory can be changed in FirebaseFS.h.

```C++
bool pushFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName);

bool push(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName);
```






#### Append new binary data from file stored on SD card/Flash memory and the virtual child ".priority" to the defined database path.

```C++
bool pushFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority);

bool push(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority);
```







 #### Append new Firebase server's timestamp to the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which timestamp will be appended.

return - **`Boolean`** type status indicates the success of the operation.
    
The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function [FirebaseData object].pushName().

```C++
bool pushTimestamp(FirebaseData &fbdo, const String &path);
```







#### Set integer data at the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which integer data will be set.

param **`intValue`** Integer value to set.

return **`Boolean`** type status indicates the success of the operation.

Call [FirebaseData object].dataType to determine what type of data successfully
stores in the database. 
    
Call [FirebaseData object].intData will return the integer value of
the payload returned from the server.

```C++
bool setInt(FirebaseData &fbdo, const String &path, int intValue);

bool set(FirebaseData &fbdo, const String &path, int intValue);
```




#### Set integer data and virtual child ".priority" at the defined database path.

```C++
bool setInt(FirebaseData &fbdo, const String &path, int intValue, float priority);

bool set(FirebaseData &fbdo, const String &path, int intValue, float priority);
```



#### Set integer data at the defined database path if defined database path's ETag matched the ETag value.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which integer data will be set.

param **`intValue`** Integer value to set.

param **`ETag`** Known unique identifier string (ETag) of defined database path.

return - **`Boolean`** type status indicates the success of the operation.


Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
    
If ETag at the defined database path does not match the provided ETag parameter, the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).

If the operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value.
Also call [FirebaseData object].intData to get the current integer value.
    
```C++
bool setInt(FirebaseData &fbdo, const String &path, int intValue, const String &ETag);

bool set(FirebaseData &fbdo, const String &path, int intValue, const String &ETag);
```






#### Set integer data and the virtual child ".priority" if defined ETag matches at the defined database path 

```C++
bool setInt(FirebaseData &fbdo, const String &path, int intValue, float priority, const String &ETag);

bool set(FirebaseData &fbdo, const String &path, int intValue, float priority, const String &ETag);
```






#### Set float data at the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which float data will be set.

param **`floatValue`** Float value to set.

return **`Boolean`** type status indicates the success of the operation.

Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 

Call [FirebaseData object].floatData will return the float value of
the payload returned from the server.

```C++
bool setFloat(FirebaseData &fbdo, const String &path, float floatValue);

bool set(FirebaseData &fbdo, const String &path, float floatValue);
```




#### Set float data and virtual child ".priority" at the defined database path.

```C++
bool setFloat(FirebaseData &fbdo, const String &path, float floatValue, float priority);

bool set(FirebaseData &fbdo, const String &path, float floatValue, float priority);
```





#### Set float data at the defined database path if defined database path's ETag matched the ETag value.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which float data will be set.

param **`floatValue`** Float value to set.

return **`Boolean`** type status indicates the success of the operation.

Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 

Call [FirebaseData object].floatData will return the float value of
the payload returned from the server.

If ETag at the defined database path does not match the provided ETag parameter, the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).

If the operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value.
Also call [FirebaseData object].floatData to get the current float value.

```C++
bool setFloat(FirebaseData &fbdo, const String &path, float floatValue, const String &ETag);

bool set(FirebaseData &fbdo, const String &path, float floatValue, const String &ETag);
```




#### Set float data and the virtual child ".priority" if defined ETag matches at the defined database path 

```C++
bool setFloat(FirebaseData &fbdo, const String &path, float floatValue, float priority, const String &ETag);

bool set(FirebaseData &fbdo, const String &path, float floatValue, float priority, const String &ETag);
```





#### Set double data at the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which float data will be set.

param **`doubleValue`** Double value to set.

return **`Boolean`** type status indicates the success of the operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in the database.

Call [FirebaseData object].doubleData will return the double value of
the payload returned from the server.

Due to bugs in Serial.print in Arduino, to print large double value with zero decimal place, 
use printf("%.9lf\n", firebaseData.doubleData()); for print the returned double value up to 9 decimal places.


```C++
bool setDouble(FirebaseData &fbdo, const String &path, double doubleValue);

bool set(FirebaseData &fbdo, const String &path, double doubleValue);
```





#### Set double data and virtual child ".priority" at the defined database path.

```C++
bool setDouble(FirebaseData &fbdo, const String &path, double doubleValue, float priority);

bool set(FirebaseData &fbdo, const String &path, double doubleValue, float priority);
```





#### Set double data at the defined database path if defined database path's ETag matched the ETag value.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which float data will be set.

param **`doubleValue`** Double value to set.

param **`ETag`** Known unique identifier string (ETag) of defined database path.

return - **`Boolean`** type status indicates the success of the operation.

Call [FirebaseData object].dataType to determine what type of data successfully stores in the database.

Call [FirebaseData object].doubleData will return the double value of payload returned from the server.

If ETag at the defined database path does not match the provided ETag parameter, the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).

If the operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value.
Also call [FirebaseData object].doubeData to get the current double value.

```C++
bool setDouble(FirebaseData &fbdo, const String &path, double doubleValue, const String &ETag);

bool set(FirebaseData &fbdo, const String &path, double doubleValue, const String &ETag);
```






#### Set double data and the virtual child ".priority" if defined ETag matches at the defined database path 

```C++
bool setDouble(FirebaseData &fbdo, const String &path, double doubleValue, float priority, const String &ETag);

bool set(FirebaseData &fbdo, const String &path, double doubleValue, float priority, const String &ETag);
```





#### Set Boolean data at the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which float data will be set.

param **`boolValue`** Boolean value to set.

return **`Boolean`** type status indicates the success of the operation.

Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 

Call [FirebaseData object].boolData will return the Boolean value of
the payload returned from the server.

```C++
bool setBool(FirebaseData &fbdo, const String &path, bool boolValue);

bool set(FirebaseData &fbdo, const String &path, bool boolValue);
```





#### Set boolean data and virtual child ".priority" at the defined database path.

```C++
bool setBool(FirebaseData &fbdo, const String &path, bool boolValue, float priority);

bool set(FirebaseData &fbdo, const String &path, bool boolValue, float priority);
```





#### Set Boolean data at the defined database path if defined database path's ETag matched the ETag value

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which float data will be set.

param **`boolValue`** Boolean value to set.

param **`ETag`** Known unique identifier string (ETag) of defined database path.

return **`Boolean`** type status indicates the success of the operation.

Call [FirebaseData object].dataType to determine what type of data that successfully stores in the database. 

Call [FirebaseData object].boolData will return the Boolean value of
the payload returned from the server.


If ETag at the defined database path does not match the provided ETag parameter,
the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).

If the operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value.
Also call [FirebaseData object].doubeData to get the current boolean value.

```C++
bool setBool(FirebaseData &fbdo, const String &path, bool boolValue, const String &ETag);

bool set(FirebaseData &fbdo, const String &path, bool boolValue, const String &ETag);
```





#### Set boolean data and the virtual child ".priority" if defined ETag matches at the defined database path 

```C++
bool setBool(FirebaseData &fbdo, const String &path, bool boolValue, float priority, const String &ETag);

bool set(FirebaseData &fbdo, const String &path, bool boolValue, float priority, const String &ETag);
```






#### Set string (text) at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which string data will be set.

param **`stringValue`** String or text to set.

return **`Boolean`** type status indicates the success of the operation.

Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 

Call [FirebaseData object].stringData will return the string value of
the payload returned from the server.

```C++
bool setString(FirebaseData &fbdo, const String &path, const String &stringValue);

bool set(FirebaseData &fbdo, const String &path, const char *stringValue);

bool set(FirebaseData &fbdo, const String &path, const String &stringValue);
```






#### Set string data and virtual child ".priority" at the defined database path.

```C++
bool setString(FirebaseData &fbdo, const String &path, const String &stringValue, float priority);

bool set(FirebaseData &fbdo, const String &path, const char *stringValue, float priority);

bool set(FirebaseData &fbdo, const String &path, const String &stringValue, float priority);
```






#### Set string (text) at the defined database path if defined database path's ETag matched the ETag value

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which string data will be set.

param **`stringValue`** String or text to set.

param **`ETag`** Known unique identifier string (ETag) of defined database path.

return **`Boolean`** type status indicates the success of the operation.

Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 

Call [FirebaseData object].stringData will return the string value of
the payload returned from the server.

If ETag at the defined database path does not match the provided ETag parameter, 
the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).

If the operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value.
Also, call [FirebaseData object].stringData to get the current string value.

```C++
bool setString(FirebaseData &fbdo, const String &path, const String &stringValue, const String &ETag);

bool set(FirebaseData &fbdo, const String &path, const char *stringValue, const String &ETag);

bool set(FirebaseData &fbdo, const String &path, const String &stringValue, const String &ETag);
```





#### Set string data and the virtual child ".priority" if defined ETag matches at the defined database path 

```C++
bool setString(FirebaseData &fbdo, const String &path, const String &stringValue, float priority, const String &ETag);

bool set(FirebaseData &fbdo, const String &path, const char *stringValue, float priority, const String &ETag);

bool set(FirebaseData &fbdo, const String &path, const String &stringValue, float priority, const String &ETag);
```







#### Set child nodes key and value (using FirebaseJson object) to the defined database path

This will replace any child nodes inside the defined path with node' s key
and value defined in FirebaseJson object.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which key and value in FirebaseJson object will be replaced or set.

param **`json`** The FirebaseJson object.

return **`Boolean`** type status indicates the success of the operation.

Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
 
Call [FirebaseData object].jsonData will return the JSON string value of
the payload returned from the server.

Call [FirebaseData object].jsonObject will return the FirebaseJson object of
payload returned from server.

```C++
bool setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json);

bool set(FirebaseData &fbdo, const String &path, FirebaseJson &json);
```







#### Set JSON data or FirebaseJson object and virtual child ".priority" at the defined database path.

```C++
bool setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority);

bool set(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority);
```







#### Set child nodes key and value (using JSON data or FirebaseJson object) to the defined database path if defined database path's ETag matched the ETag value

This will replace any child nodes inside the defined path with node' s key
and value defined in JSON data or FirebaseJson object.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which key and value in JSON data or FirebaseJson object will be replaced or set.

param **`jsonString`** The JSON string to set (should be valid JSON data).

param **`json`** The FirebaseJson object.

param **`ETag`** Known unique identifier string (ETag) of defined database path.

return **`Boolean`** type status indicates the success of the operation.

Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
 
Call [FirebaseData object].jsonData will return the JSON string value of
the payload returned from the server.

Call [FirebaseData object].jsonObject will return the FirebaseJson object of
payload returned from server.


If ETag at the defined database path does not match the provided ETag parameter,
the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).

If the operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value.
Also call [FirebaseData object].jsonData to get the current JSON string value [FirebaseData object].jsonObject 
to get the FirebaseJson object.


```C++
bool setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, const String &ETag);

bool set(FirebaseData &fbdo, const String &path, FirebaseJson &json, const String &ETag);
```






#### Set JSON data or FirebaseJson object and the virtual child ".priority" if defined ETag matches at the defined database path 

```C++
bool setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority, const String &ETag);

bool set(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority, const String &ETag);
```




 
 
#### Set child nodes's array (using FirebaseJsonArray object) to the defined database path.

This will replace any child nodes inside the defined path with array defined in FirebaseJsonArray object.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which key and value in FirebaseJsonArray object will be replaced or set.

param **`arr`** The FirebaseJsonArray object.

return - **`Boolean`** type status indicates the success of the operation.

Call [FirebaseData object].dataType to determine what type of data that successfully
stores in the database.

Call [FirebaseData object].jsonArray will return pointer to FirebaseJsonArray object contains array
payload returned from server, get the array payload using FirebaseJsonArray *arr = firebaseData.jsonArray();

```C++
bool setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr);

bool set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr);
```







#### Set FirebaseJsonArray object and virtual child ".priority" at the defined database path.

```C++
bool setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority);

bool set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority);
```






#### Set array (using JSON data or FirebaseJson object) to the defined database path if defined database path's ETag matched the ETag value.

This will replace any child nodes inside the defined path with array defined in FirebaseJsonArray object.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which key and value in JSON data will be replaced or set.

param **`arr`** The FirebaseJsonArray object.

param **`ETag`** Known unique identifier string (ETag) of defined database path.

return - **`Boolean`** type status indicates the success of the operation.
    
Call [FirebaseData object].dataType to determine what type of data that successfully
stores in the database.

Call [FirebaseData object].jsonArray will return pointer to FirebaseJsonArray object contains array
payload returned from server, get the array payload using FirebaseJsonArray *arr = firebaseData.jsonArray();

If ETag at the defined database path does not match the provided ETag parameter,
the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).

If the operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value.

Also call [FirebaseData object].jsonArray to get the pointer to FirebaseJsonArray object of current array value.

```C++
bool setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, const String &ETag);

bool set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, const String &ETag);
```

    Set FirebaseJsonArray object and the virtual child ".priority" if defined ETag matches at the defined database path 

```C++
bool setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority, const String &ETag);

bool set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority, const String &ETag);
```





#### Set blob (binary data) at the defined database path

This will replace any child nodes inside the defined path with a blob of binary data.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which binary data will be set.

param **`blob`** Byte array of data.

param **`size`** Size of the byte array.

return **`Boolean`** type status indicates the success of the operation.

No payload returned from the server.

```C++
bool setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size);

bool set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size);
```





#### Set blob data and virtual child ".priority" at the defined database path.

```C++
bool setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority);

bool set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority);
```






#### Set blob (binary data) at the defined database path if defined database path's ETag matched the ETag value

This will replace any child nodes inside the defined path with a blob of binary data.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which binary data will be set.

param **`blob`** Byte array of data.

param **`size`** Size of the byte array.

param **`ETag`** Known unique identifier string (ETag) of defined database path.

return **`Boolean`** type status indicates the success of the operation.

No payload returned from the server.

If ETag at the defined database path does not match the provided ETag parameter,
the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).

```C++
bool setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, const String &ETag);

bool set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, const String &ETag);
```





#### Set blob data and the virtual child ".priority" if defined ETag matches at the defined database path 

```C++
bool setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority, const String &ETag);

bool set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority, const String &ETag);
```





#### Set binary data from the file store on SD card/Flash memory to the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`storageType`** Type of storage to read file data, StorageType::FLASH or StorageType::SD.

param **`path`** Target database path in which binary data from the file will be set.

param **`fileName`** File name included its path in SD card/Flash memory.

return **`Boolean`** type status indicates the success of the operation.

No payload returned from the server.

The file systems for flash and sd memory can be changed in FirebaseFS.h.

```C++
bool setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName);

bool set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName);
```





#### Set binary data from the file and virtual child ".priority" at the defined database path.

```C++
bool setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority);

bool set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority);
```






#### Set binary data from a file stored on SD card/Flash memory to the defined database path if defined database path's ETag matched the ETag value

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`storageType`** Type of storage to read file data, StorageType::FLASH or StorageType::SD.

param **`path`** Target database path in which binary data from the file will be set.

param **`fileName`** File name included its path in SD card/Flash memory.

param **`ETag`** Known unique identifier string (ETag) of defined database path.

return **`Boolean`** type status indicates the success of the operation.

No payload returned from the server.

If ETag at the defined database path does not match the provided ETag parameter,
the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).

The file systems for flash and sd memory can be changed in FirebaseFS.h.

```C++
bool setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, const String &ETag);

bool set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, const String &ETag);
```






#### Set binary data from a file and the virtual child ".priority" if defined ETag matches at the defined database path 

```C++
bool setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority, const String &ETag);

bool set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority, const String &ETag);

```





 #### Set Firebase server's timestamp to the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which timestamp will be set.

return - **`Boolean`** type status indicates the success of the operation.
    
Call [FirebaseData object].intData will return the integer value of timestamp in seconds
or [FirebaseData object].doubleData to get millisecond timestamp.

Due to bugs in Serial.print in Arduino, to print large double value with zero decimal place, 
use printf("%.0lf\n", firebaseData.doubleData());.

```C++
bool setTimestamp(FirebaseData &fbdo, const String &path);
```







#### Update child node key or existing key's value (using FirebaseJson object) under the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which key and value in FirebaseJson object will be updated.

param **`json`** The FirebaseJson object used for update.

return **`Boolean`** type status indicates the success of the operation.

Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
 
Call [FirebaseData object].jsonData will return the json string value of
payload returned from server.

Call [FirebaseData object].jsonObject will return the FirebaseJson object of
payload returned from server.

To reduce network data usage, use updateNodeSilent instead.

```C++
bool updateNode(FirebaseData &fbdo, const String &path, FirebaseJson &json);

```







#### Update child node key or existing key's value and virtual child ".priority" (using JSON data or FirebaseJson object) under the defined database path.

```C++

bool updateNode(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority);

```







#### Update child node key or existing key's value (using FirebaseJson object) under the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which key and value in FirebaseJson object will be updated.

param **`json`** The FirebaseJson object used for update.

return **`Boolean`** type status indicates the success of the operation.

Owing to the objective of this function to reduce network data usage, 
no payload will be returned from the server.

```C++
bool updateNodeSilent(FirebaseData &fbdo, const String &path, FirebaseJson &json);

```





#### Update child node key and virtual child ".priority" (using JSON data or FirebaseJson object) under the defined database path.

```C++

bool updateNodeSilent(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority);
```







#### Read any type of value at the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the float value is being read.

return - **`Boolean`** type status indicates the success of the operation.

Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
    
Call [FirebaseData object].intData, [FirebaseData object].floatData, [FirebaseData object].doubleData,
[FirebaseData object].boolData, [FirebaseData object].stringData, [FirebaseData object].jsonObject (pointer), 
[FirebaseData object].jsonArray (pointer) and [FirebaseData object].blobData corresponded to 
its type from [FirebaseData object].dataType.

```C++
bool get(FirebaseData &fbdo, const String &path);
```




#### Read the integer value at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the integer value is being read.

return **`Boolean`** type status indicates the success of the operation.

Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 

Call [FirebaseData object].intData will return the integer value of
payload returned from the server.

If the payload returned from server is not integer or float type, 
the function [FirebaseData object].intData will return zero (0).

If the payload returned from server is float type, 
the function [FirebaseData object].intData will return rounded integer value.

```C++
bool getInt(FirebaseData &fbdo, const String &path);
```





#### Read the integer value at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the integer value is being read.

param **`target`** The integer type variable to store value.

return **`Boolean`** type status indicates the success of the operation.

If the type of payload returned from the server is not an integer, float and double, 
the target variable's value will be zero (0).

```C++
bool getInt(FirebaseData &fbdo, const String &path, int &target);
```






#### Read the float value at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the float value is being read.

return **`Boolean`** type status indicates the success of the operation.

Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 

Call [FirebaseData object].floatData will return the float value of
the payload returned from the server.

If the payload returned from server is not integer or float type, 
the function [FirebaseData object].intData will return zero (0).

```C++
bool getFloat(FirebaseData &fbdo, const String &path);
```






#### Read the float value at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the float value is being read.

param **`target`** The float type variable to store value.

return **`Boolean`** type status indicates the success of the operation.

If the type of payload returned from the server is not an integer, float and double, 
the target variable's value will be zero (0).

```C++
bool getFloat(FirebaseData &fbdo, const String &path, float &target);
```






#### Read the double value at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the float value is being rea.

return **`Boolean`** type status indicates the success of the operation.

Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
    
Call [FirebaseData object].doubleData will return the double value of
the payload returned from the server.

If the payload returned from server is not integer, float and double, 
the function [FirebaseData object].doubleData will return zero (0).

Due to bugs in Serial.print in Arduino, to print large double value with zero decimal place, 
use printf("%.9lf\n", firebaseData.doubleData()); for print the returned double value up to 9 decimal places.

```C++
bool getDouble(FirebaseData &fbdo, const String &path);
```







#### Read the float value at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the float value is being read.

param **`target`** The double type variable to store value.

return **`Boolean`** type status indicates the success of the operation.

If the type of payload returned from the server is not an integer, float and double, 
the target variable's value will be zero (0).

```C++
bool getDouble(FirebaseData &fbdo, const String &path, double &target);
```






#### Read the Boolean value at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the Boolean value is being read.

return **`Boolean`** type status indicates the success of the operation.

Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 

Call [FirebaseData object].boolData will return the Boolean value of
the payload returned from the server.

If the payload returned from server is not boolean type, 
the function [FirebaseData object].boolData will return false.

```C++
bool getBool(FirebaseData &fbdo, const String &path);
```







#### Read the Boolean value at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the Boolean value is being read.

param **`target`** The boolean type variable to store value.

return **`Boolean`** type status indicates the success of the operation.

If the type of payload returned from the server is not Boolean, 
the target variable's value will be false.

```C++
bool getBool(FirebaseData &fbdo, const String &path, bool &target);
```






#### Read the string of text at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the string value is being read.

return **`Boolean`** type status indicates the success of the operation.

Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 

Call [FirebaseData object].stringData will return the string value of
the payload returned from the server.

If the payload returned from the server is not string type, 
the function [FirebaseData object].stringData will return empty string (String object).

```C++
bool getString(FirebaseData &fbdo, const String &path);
```






#### Read the string of text at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the string value is being read.

param **`target`** The String object to store value.

return **`Boolean`** type status indicates the success of the operation.

If the type of payload returned from the server is not a string,
the target String object's value will be empty.

```C++
bool getString(FirebaseData &fbdo, const String &path, String &target);
```





#### Read the JSON string at the defined database path

The returned payload JSON string represents the child nodes and their value.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the JSON string value is being read.

return **`Boolean`** type status indicates the success of the operation.

Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 

Call [FirebaseData object].jsonData will return the JSON string value of
the payload returned from the server.

Call [FirebaseData object].jsonObject will return the FirebaseJson object of
payload returned from server.

If the type of payload returned from server is not json,
the function [FirebaseData object].jsonObject will contain empty object.


```C++
bool getJSON(FirebaseData &fbdo, const String &path);
```






#### Read the JSON string at the defined database path

The returned payload JSON string represents the child nodes and their value.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the JSON string value is being read.

param **`target`** The FirebaseJson object pointer to get json data

return **`Boolean`** type status indicates the success of the operation.

If the type of payload returned from the server is not JSON,
the target FirebaseJson object will contain an empty object.

```C++
bool getJSON(FirebaseData &fbdo, const String &path, FirebaseJson *target);
```







#### Read the JSON string with data filtering at the defined database path

The returned payload JSON string represents the child nodes and their value.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the JSON string value is being read.

param **`query`** QueryFilter class to set query parameters to filter data.

return **`Boolean`** type status indicates the success of the operation.

Available query parameters for filtering the data are the following.

**query filter.orderBy`** A required parameter to specify which data used for data filtering included child key, key, and value.

Use "$key" for filtering data by keys of all nodes at the defined database path.

Use "$value" for filtering data by value of all nodes at the defined database path.

Use "$priority" for filtering data by "virtual child" named .priority of all nodes.

Use any child key to filter by that key.


**`QueryFilter.limitToFirst`**  The total children (number) to filter from the first child.

**`QueryFilter.limitToLast`**   The total last children (number) to filter. 

**`QueryFilter.startAt`**       Starting value of range (number or string) of query upon orderBy param.

**`QueryFilter.endAt`**         Ending value of range (number or string) of query upon orderBy param.

**`QueryFilter.equalTo`**       Value (number or string) matches the orderBy param


Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 

Call [FirebaseData object].jsonData will return the JSON string value of
the payload returned from the server.

Call [FirebaseData object].jsonObject will return the FirebaseJson object of
payload returned from server.

If the type of payload returned from server is not json,
the function [FirebaseData object].jsonObject will contain empty object.

```C++
bool getJSON(FirebaseData &fbdo, const String &path, QueryFilter &quer);
```







#### Read the JSON string at the defined database path as above

The returned payload JSON string represents the child nodes and their value.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the JSON string value is being read.

param **`target`** The FirebaseJson object pointer to get json data.

return **`Boolean`** type status indicates the success of the operation.

If the type of payload returned from the server is not JSON,
the target FirebaseJson object will contain an empty object.

```C++
bool getJSON(FirebaseData &fbdo, const String &path, QueryFilter &query, FirebaseJson *target);
```









#### Read the array data at the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the array is being read.

return - **`Boolean`** type status indicates the success of the operation.

    
Call [FirebaseData object].dataType to determine what type of data successfully stores in the database.

Call [FirebaseData object].jsonArray will return the pointer to FirebaseJsonArray object contains array value of
the payload returned from the server.

If the type of payload returned from the server is not an array,
the array element in [FirebaseData object].jsonArray will be empty.

```C++
bool getArray(FirebaseData &fbdo, const String &path);
```




#### Read the array data at the defined database path and assign data to the target.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the array is being read.

param **`target - The FirebaseJsonArray object pointer to get array value.

return - **`Boolean`** type status indicates the success of the operation.


If the type of payload returned from the server is not an array,
the target FirebaseJsonArray object will contain an empty array.

```C++
bool getArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray *target);
```




#### Read the array data at the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the array is being read.

param **`query`** QueryFilter class to set query parameters to filter data.

return - **`Boolean`** type status indicates the success of the operation.

Available query parameters for filtering the data are the following.

QueryFilter.orderBy -       Required parameter to specify which data used for data filtering included child key, key, and value.
                            Use "$key" for filtering data by keys of all nodes at the defined database path.
                            Use "$value" for filtering data by value of all nodes at the defined database path.
                            Use "$priority" for filtering data by "virtual child" named .priority of all nodes.
                            Use any child key to filter by that key.


QueryFilter.limitToFirst -  The total children (number) to filter from the first child.

QueryFilter.limitToLast -   The total last children (number) to filter.

QueryFilter.startAt -       Starting value of range (number or string) of query upon orderBy param.

QueryFilter.endAt -         Ending value of range (number or string) of query upon orderBy param.

QueryFilter.equalTo -       Value (number or string) matches the orderBy param


Call [FirebaseData object].dataType to determine what type of data that successfully
stores in the database.

Call [FirebaseData object].jsonArray will return the pointer to FirebaseJsonArray object contains array of
the payload returned from the server.

If the type of payload returned from the server is not an array,
the function [FirebaseData object].jsonArray will contain empty array.


```C++
bool getArray(FirebaseData &fbdo, const String &path, QueryFilter &query);
```






#### Read the array data at the defined database path as above

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the array is being read.

param **`target`** The FirebaseJsonArray object to get array value.

return - **`Boolean`** type status indicates the success of the operation.

If the type of payload returned from the server is not an array,
the target FirebaseJsonArray object will contain an empty array.

```C++
bool getArray(FirebaseData &fbdo, const String &path, QueryFilter &query, FirebaseJsonArray *target);
```







#### Read the blob (binary data) at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the binary data is being read.

return **`Boolean`** type status indicates the success of the operation.

Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
    
Call [FirebaseData object].blobData will return the dynamic array of unsigned 8-bit data (i.e. `std::vector<uint8_t>`) of
payload returned from server.

If the payload returned from the server is not blob type, 
the function [FirebaseData object].blobData will return empty array.

```C++
bool getBlob(FirebaseData &fbdo, const String &path);
```






#### Read the blob (binary data) at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the binary data is being read.

param **`target`** Dynamic array of unsigned 8-bit data (i.e. std::vector<uint8_t>) to store value.

return **`Boolean`** type status indicates the success of the operation.

If the type of payload returned from the server is not a blob,
the target variable value will be an empty array.

```C++
bool getBlob(FirebaseData &fbdo, const String &path, std::vector<uint8_t> &target);
```







#### Download file data in a database at the defined database path and save to SD card/Flash memory.

The downloaded data will be decoded to binary and save to SD card/Flash memory, then

please make sure that data at the defined database path is the file type.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`storageType`** Type of storage to write file data, StorageType::FLASH or StorageType::SD.

param **`nodePath`** Database path that file data will be downloaded.

param **`fileName`** File name included its path in SD card/Flash memory.

return **`Boolean`** type status indicates the success of the operation.

The file systems for flash and sd memory can be changed in FirebaseFS.h.

```C++
bool getFile(FirebaseData &fbdo, uint8_t storageType, const String &nodePath, const String &fileName);
```








#### Delete all child nodes at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path to be deleted.

return **`Boolean`** type status indicates the success of the operation.*

```C++
bool deleteNode(FirebaseData &fbdo, const String &path);
```






#### Delete all child nodes at the defined database path if defined database path's ETag matched the ETag value

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path to be deleted.

param **`ETag`** Known unique identifier string (ETag) of defined database path.

return **`Boolean`** type status indicates the success of the operation.*

If ETag at the defined database path does not match the provided ETag parameter,
the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).

```C++
bool deleteNode(FirebaseData &fbdo, const String &path, const String &ETag);
```





#### Start monitoring the value changes at the defined path and its children

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path to monitor.

return **`Boolean`** type status indicates the success of the operation.*

```C++
bool beginStream(FirebaseData &fbdo, const String &path);
```







#### Start subscribe to the value changes at the defined parent node path with multiple nodes paths parsing.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`parentPath`** Database parent node path to subscribe.

param **`childPath`** The string array of child nodes paths for parsing.

param **`size`** The size of string array of child nodes paths for parsing.

return **`Boolean`** type status indicates the success of the operation.*

```C++
bool beginMultiPathStream(FirebaseData &fbdo, const String &parentPath, const String *childPath, size_t size);
```








#### Read the stream event data at the defined database path

Once beginStream was called e.g. in setup(), the readStream function
should call inside the loop function.

param **`fbdo`** Firebase Data Object to hold data and instances.

return **`Boolean`** type status indicates the success of the operation.

Using the same Firebase Data object for stream read/monitoring associated with read, store, update, delete will break or quit the current stream connection. 
    
The stream will be resumed or reconnected automatically when calling readStream.

```C++
bool readStream(FirebaseData &fbdo);
```







#### End the stream connection at the defined path

It can be restart again by calling beginStream.

param **`fbdo`** Firebase Data Object to hold data and instances.

return **`Boolean`** type status indicates the success of the operation.
 
```C++
bool endStream(FirebaseData &fbdo);
```







#### Set the stream callback functions

setStreamCallback should be called before Firebase.beginStream.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`dataAvailablecallback`** The Callback function that accepts streamData parameter.

param **`timeoutCallback`** The Callback function will be called when stream connection was timeout (optional).

param **`streamTaskStackSize`** The stream task (RTOS task) reserved stack memory in byte (optional) (8192 is default).

dataAvailablecallback will be called When data in the defined path changed or the stream path changed or stream connection
was resumed from read, store, update, and deleteNode.

The payload returned from the server will be one of these integer, float, string, JSON and blob types.

Call [streamData object].dataType to determine what type of data that successfully
store in the database. 
    
Call [streamData object].xxxData will return the appropriate data type of
the payload returned from the server.

```C++
void setStreamCallback(FirebaseData &fbdo, StreamEventCallback dataAvailablecallback, StreamTimeoutCallback timeoutCallback = NULL, size_t streamTaskStackSize = 8192);
```









#### Set the multiple paths stream callback functions.

setMultiPathStreamCallback should be called before Firebase.beginMultiPathStream.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`multiPathDataCallback`** The Callback function that accepts MultiPathStreamData parameter.

param **`timeoutCallback`** The Callback function will be called when stream connection was timeout (optional).

param **`streamTaskStackSize`** The stream task (RTOS task) reserved stack memory in byte (optional) (8192 is default).

multiPathDataCallback will be called When data in the defined path changed or the stream path changed or stream connection
was resumed from getXXX, setXXX, pushXXX, updateNode, deleteNode.

The payload returned from the server will be one of these integer, float, string and JSON.

Call [MultiPathStreamData object].get to get the child node value, type and data path. 
    
The properties [MultiPathStreamData object].value, [MultiPathStreamData object].dataPath, and [MultiPathStreamData object].type will return the value, path of data, and type of data respectively.

These properties will store the result from calling the function [MultiPathStreamData object].get.

```C++
void setMultiPathStreamCallback(FirebaseData &fbdo, MultiPathStreamEventCallback multiPathDataCallback, StreamTimeoutCallback timeoutCallback = NULL, size_t streamTaskStackSize = 8192);
```









#### Remove stream callback functions

param **`fbdo`** Firebase Data Object to hold data and instances.

```C++
void removeStreamCallback(FirebaseData &fbdo);
```







#### Remove multiple paths stream callback functions.

param **`fbdo`** Firebase Data Object to hold data and instances.

```C++
void removeMultiPathStreamCallback(FirebaseData &fbdo);
```





#### Backup (download) database at defined database path to SD card/Flash memory

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`storageType`** Type of storage to save file, StorageType::FLASH or StorageType::SD.

param **`nodePath`** Database path to be back up.

param **`fileName`** File name to save.

Only 8.3 DOS format (max. 8 bytes file name and 3 bytes file extension) can be saved to SD card.

return **`Boolean`** type status indicates the success of the operation.

The file systems for flash and sd memory can be changed in FirebaseFS.h.

```C++
bool backup(FirebaseData &fbdo, uint8_t storageType, const String &nodePath, const String &fileName);
```





#### Restore database at defined path usin backup file saved on SD card/Flash memory

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`storageType`** Type of storage to read file, StorageType::FLASH or StorageType::SD.

param **`nodePath`** Database path to be restored.

param **`fileName`** File name to read.

return **`Boolean`** type status indicates the success of the operation.

The file systems for flash and sd memory can be changed in FirebaseFS.h.

```C++
bool restore(FirebaseData &fbdo, uint8_t storageType const String &nodePath, const String &fileName);
```







#### Set maximum Firebase's read/store retry operation (0 - 255) in case of network problems and buffer overflow

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`num`** The maximum retry.

```C++
void setMaxRetry(FirebaseData &fbdo, uint8_t num);
```





#### Set the maximum Firebase Error Queues in collection (0 - 255)

Firebase read/store operation causes by network problems and buffer overflow will be added to Firebase Error Queues collection.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`num`** The maximum Firebase Error Queues.

```C++
void setMaxErrorQueue(FirebaseData &fbdo, uint8_t num);
```




   
#### Save Firebase Error Queues as FLASH file (save only database store queues)

Firebase read (get) operation will not be saved.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`filename`** File name to be saved.

param **`storageType`** Type of storage to save file, StorageType::FLASH or StorageType::SD.

The file systems for flash and sd memory can be changed in FirebaseFS.h.
    
```C++
bool saveErrorQueue(FirebaseData &fbdo, const String &filename, uint8_t storageType);
```
   





#### Delete file in Flash (FLASH) or SD card

param **`filename`** File name to delete.

param **`storageType`** Type of storage to save file, StorageType::FLASH or StorageType::SD.

The file systems for flash and sd memory can be changed in FirebaseFS.h.
    
```C++
bool deleteStorageFile(const String &filename, uint8_t storageType);
```




   
#### Restore Firebase Error Queues from FLASH file

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`filename`** Filename to be read and restore queues.

param **`storageType`** Type of storage to read file, StorageType::FLASH or StorageType::SD.

The file systems for flash and sd memory can be changed in FirebaseFS.h.
    
```C++
bool restoreErrorQueue(FirebaseData &fbdo, const String &filename, uint8_t storageType);
```





#### Determine the number of Firebase Error Queues stored in defined FLASH file

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`filename`** Filename to be read and count for queues.

param **`storageType`** Type of storage to read file, StorageType::FLASH or StorageType::SD.

return **`Number`** (0-255) of queues store in defined FLASH file.

The file systems for flash and sd memory can be changed in FirebaseFS.h.

```C++
uint8_t errorQueueCount(FirebaseData &fbdo, const String &filename, uint8_t storageType);
```






#### Determine number of queues in Firebase Data object Firebase Error Queues collection

param **`fbdo`** Firebase Data Object to hold data and instances.

return **`Number`** (0-255) of queues in Firebase Data object queue collection.

```C++
uint8_t errorQueueCount(FirebaseData &fbdo);
```






#### Determine whether the Firebase Error Queues collection was full or not

param **`fbdo`** Firebase Data Object to hold data and instances.

return **`Boolean`** type status indicates whether the  Firebase Error Queues collection was full or not.

```C++
bool isErrorQueueFull(FirebaseData &fbdo);
```






#### Process all failed Firebase operation queue items when network is available

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`callback`** Callback function that accepts QueueInfo parameter.
  
```C++
void processErrorQueue(FirebaseData &fbdo, QueueInfoCallback callback = NULL);
```






#### Return Firebase Error Queue ID of last Firebase Error

Return 0 if there is no Firebase Error from the last operation.

param **`fbdo`** Firebase Data Object to hold data and instances.
    
return **`Number`** of Queue ID.

```C++
uint32_t getErrorQueueID(FirebaseData &fbdo);
```




#### Determine whether Firebase Error Queue currently exists is Error Queue collection or not

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`errorQueueID`** The Firebase Error Queue ID get from getErrorQueueID.
    
return - **`Boolean type`** status indicates the queue existence.

```C++
bool isErrorQueueExisted(FirebaseData &fbdo, uint32_t errorQueueID);
```





#### Start the Firbase Error Queues Auto Run Process

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`callback`** Callback function that accepts QueueInfo Object as parameter, optional.

param **`queueTaskStackSize`** The queue error recovery task (RTOS task) reserved stack memory in byte (optional) (8192 is default).

The following functions are available from QueueInfo Object accepted by callback.

**queueInfo.totalQueues()**, get the total Error Queues in Error Queue Collection.

**queueInfo.currentQueueID()**, get current Error Queue ID that being process.

**queueInfo.isQueueFull()**, determine whether Error Queue Collection is full or not.

**queueInfo.dataType()**, get a string of the Firebase call data type that being process of current Error Queue.

**queueInfo.method()**, get a string of the Firebase call method that being process of current Error Queue.

**queueInfo.path()**, get a string of the Firebase call path that being process of current Error Queue.


```C++
void beginAutoRunErrorQueue(FirebaseData &fbdo, QueueInfoCallback callback = NULL, size_t queueTaskStackSize = 8192);
```





#### Stop the Firebase Error Queues Auto Run Process

param **`fbdo`** Firebase Data Object to hold data and instances.


```C++
void endAutoRunErrorQueue(FirebaseData &fbdo);
```




#### Clear all Firbase Error Queues in Error Queue collection

param **`fbdo`** Firebase Data Object to hold data and instances.


```C++
void clearErrorQueue(FirebaseData &fbdo);
```






#### Send Firebase Cloud Messaging to device with first registeration token which added by firebaseData.fcm.addDeviceToken

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`index`** The index (starts from 0) of recipient device token which added by firebaseData.fcm.addDeviceToken
    
return - **`Boolean type`** status indicates the success of the operation.

```C++
bool sendMessage(FirebaseData &fbdo, uint16_t index);
```






#### Send Firebase Cloud Messaging to all devices (multicast) which added by firebaseData.fcm.addDeviceToken

param **`fbdo`** Firebase Data Object to hold data and instances.
    
return - **`Boolean type`** status indicates the success of the operation.

```C++
bool broadcastMessage(FirebaseData &fbdo);
```





#### Send Firebase Cloud Messaging to devices that subscribed to a topic

param **`fbdo`** Firebase Data Object to hold data and instances.
    
return - **`Boolean type`** status indicates the success of the operation.

```C++
bool sendTopic(FirebaseData &fbdo);
```






#### SD card config with GPIO pins

param **`ss`** SPI Chip/Slave Select pin.

param **`sck`** SPI Clock pin.

param **`miso`** SPI MISO pin.

param **`mosi`** SPI MOSI pin.

return **`Boolean`** type status indicates the success of the operation.


```C++
void sdBegin(int8_t ss = -1, int8_t sck = -1, int8_t miso = -1, int8_t mosi = -1);
```







#### Provide the http code error string

param **`httpCode`** The http code.

param **`buff`** The C++ string buffer out.

```C++
void errorToString(int httpCode, std::string &buff);
```






## Firebase Data Object Functions


 ## Set the HTTP response size limit.

param **`len`** The server response buffer size limit (4096 is minimum). 

```C++
void setResponseSize(size_t len);
```






#### Pause/Unpause WiFiClient from all Firebase operations

param **`pause`** True for pause and False for unpause.

return **`Boolean`** type status indicates the success of the operation.

```C++
bool pauseFirebase(bool pause);
```





## Firebase Data Object Functions


#### Get WiFi client instance


return **`WiFi client instance`**.

```C++
WiFiClient &getWiFiClient();
```







#### Close the keep-alive connection of the internal WiFi client.

This will release the memory used by internal WiFi client.

The next server connection will take time (a second) due to SSL/TLS handshaking

```C++
void stopWiFiClient();
```







#### Determine the data type of payload returned from the server

return **`The one of these data type e.g. integer, float, double, boolean, string, JSON and blob.`**

```C++
String dataType();
```






#### Determine the event type of stream

return **`The one of these event type String e.g. put, patch, cancel, and auth_revoked.`**

The event type "put" indicated that data at the event path relative to the stream path was completely changed. 

The event path can be determined by dataPath().

The event type "patch" indicated that data at the event path relative to stream path was updated. 

The event path can be determined by dataPath().

The event type "cancel" indicated something wrong and cancel by the server.

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

return **`The database path which belongs to server' s returned payload.`**

The database path returned from this function in case of stream, also changed upon the child or parent's stream
value changes.

```C++
String dataPath();
```







#### Determine the error reason String from the process

return **`The error description string (String object).`**

```C++
String errorReason();
```







#### Return the integer data of server returned payload

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
String jsonString();
```





#### Return the Firebase JSON object of server returned payload.

return **`FirebaseJson object.`**

```C++
FirebaseJson &jsonObject();
```





#### Return the Firebase JSON object pointer of server returned payload.

return **`FirebaseJson object `**pointer.

```C++
FirebaseJson *jsonObjectPtr();
```





#### Return the Firebase JSON Array object of server returned payload.

return **`FirebaseJsonArray object`**.

```C++
FirebaseJsonArray &jsonArray();
```




#### Return the Firebase JSON Array object pointer of server returned payload.

return **`FirebaseJsonArray object pointer`**.

```C++
FirebaseJsonArray *jsonArrayPtr();
```




#### the Firebase JSON Data object that keeps the get(parse) result.

return **`FirebaseJsonData object `**.

```C++
FirebaseJsonData &jsonData();
```





#### the Firebase JSON Data object pointer that keeps the get(parse) result.

return **`FirebaseJsonData object `**pointer.

```C++
FirebaseJsonData *jsonDataPtr();
```






#### Return the blob data (uint8_t) array of server returned payload

return **`Dynamic array`** of 8-bit unsigned integer i.e. `std::vector<uint8_t>`.

```C++
std::vector<uint8_t> blobData();
```





 #### Return the file stream of server returned payload.

 return the **`file stream`**.

 ```C++
File fileStream();
```





#### Return the new appended node's name or key of server returned payload when calling pushXXX function

return **`String`** (String object).

```C++
String pushName();
```







#### Determine the stream connection status

return **`Boolean`** type status indicates whether the Firebase Data object is working with a stream or not.

```C++
bool isStream();
```







#### Determine the server connection status

return **`Boolean`** type status indicates whether the Firebase Data object is connected to the server or not.

```C++
bool httpConnected();
```







#### Determine the timeout event of server's stream (30 sec is the default)

Nothing to do when stream connection timeout, the stream connection will be automatically resumed.

return **`Boolean`** type status indicates whether the stream was a timeout or not.

```C++
bool streamTimeout();
```







#### Determine the availability of data or payload returned from the server

return **`Boolean`** type status indicates whether the server returns the new payload or not.

```C++
bool dataAvailable();
```







#### Determine the availability of stream event-data payload returned from the server

return **`Boolean`** type status indicates whether the server returns the stream event-data 
payload or not.

```C++
bool streamAvailable();
```







#### Determine the matching between data type that intend to get from/store to database and the server's return payload data type

return **`Boolean`** type status indicates whether the type of data being get from/store to database 
and the server's returned payload is matched or not.

```C++
bool mismatchDataType();
```







#### Determine the HTTP status code return from the server

return **`Integer`** number of HTTP status.

```C++
int httpCode();
```







#### Check overflow of the returned payload data buffer

return **`Boolean`** of the overflow status.


Total default HTTP response buffer size is 400 bytes which can be set through Firebase.setResponseSize.


```C++
bool bufferOverflow();
```







#### Determine the name (full path) of backup file in SD card

return **`String`** (String object) of the file name that stores on SD card after backup operation.

```C++
String getBackupFilename();
```







#### Determine the size of the backup file

return **`Number of byte`** of backup file in byte after backup operation.

```C++
size_t getBackupFileSize();
```







#### Clear or empty data in Firebase Data object

```C++
void clear();
```







#### Determine the error description for file transferring (pushFile, setFile, backup and restore)

return **`Error description string* (String object).`**

```C++
String fileTransferError();
```







#### Return the server's payload data

return **`Payload string* (String object).`**

The returned String will be empty when the response data is File, BLOB, JSON and JSON Array objects.

For File data type, call fileStream to get the file stream.

For BLOB data type, call blobData to get the dynamic array of unsigned 8-bit data.

For JSON object data type, call jsonObject and jsonObjectPtr to get the object and its pointer.

For JSON Array data type, call jsonArray and jsonArrayPtr to get the object and its pointer.

```C++
String payload();
```





## Firebase Cloud Messaging Object Functions


#### Store Firebase Cloud Messaging's authentication credentials
    
param **`serverKey`** Server key found on Console: Project settings > Cloud Messaging

```C++
void begin(const String &serverKey);
```






#### Add recipient's device registration token or instant ID token
    
param **`deviceToken`** Recipient's device registration token to add that message will be sent to.

```C++
void addDeviceToken(const String &deviceToken);
```





 #### Remove recipient's device registration token or instant ID token
    
param **`index`** Index (start from zero) of recipient's device registration token that added to FCM Data Object of Firebase Data object.

```C++
void removeDeviceToken(uint16_t index);
```




#### Clear all recipient's device registration tokens
    
 ```C++
 void clearDeviceToken();
```





#### Set the notify message type information
    
param **`title`** The title text of notification message.

param **`body`** The body text of notification message.

```C++
void setNotifyMessage(const String &title, const String &body);
```




#### Set the notify message type information
    
param **`title`** The title text of notification message.

param **`body`** The body text of notification message.

param **`icon`** The name and/or included URI/URL of the icon to show on notifying the message.

```C++
void setNotifyMessage(const String &title, const String &body, const String &icon);
```





#### Set the notify message type information
    
param **`title`** The title text of notification message.

param **`body`** The body text of notification message.

param **`icon`** The name and/or included URI/URL of the icon to show on notifying the message.

param **`click_action`** The URL or intent to accept click event on the notification message.

```C++
void setNotifyMessage(const String &title, const String &body, const String &icon, const String &click_action);
```






#### add the custom key/value in the notify message type information.
    
param **`key`** The key field in notification message.

param **`value`** The value field in the notification message.

```C++
void addCustomNotifyMessage(const String &key, const String &value);
```






#### Clear all notify message information
    
```C++
void clearNotifyMessage();
```





#### Set the custom data message type information
    
param **`jsonString`** The JSON structured data string.

```C++
void setDataMessage(const String &jsonString);
```





#### Set the custom data message type information
    
param **`json`** The FirebaseJson object.

```C++
void setDataMessage(FirebaseJson &json);
```





#### Clear custom data message type information
    
```C++
void clearDataMessage();
```





#### Set the priority of the message (notification and custom data)
    
param **`priority`** The priority string i.e. normal and high.

```C++
void setPriority(const String &priority);
```






#### Set the collapse key of the message (notification and custom data)
    
param **`key`** String of collapse key.

```C++
void setCollapseKey(const String &key);
```





#### Set the Time To Live of the message (notification and custom data)
    
param **`seconds`** Number of seconds from 0 to 2,419,200 (4 weeks).

```C++
void setTimeToLive(uint32_t seconds);
```




#### Set topic of the message will be sent to
    
param **`topic - Topic string.

```C++
void setTopic(const String &topic);
```




#### Get the send result
    
return **`String`** of payload returned from the server.

```C++
String getSendResult();
```






## FirebaseJSON object Functions


#### Clear internal buffer of FirebaseJson object.
    
return **`instance of an object.`**

```C++
FirebaseJson &clear();
```






#### Set JSON data (JSON object string) to FirebaseJson object.
    
param **`data`** The JSON object string.

return **`instance of an object.`**

```C++
FirebaseJson &setJsonData(const String &data);
```






#### Add null to FirebaseJson object.
    
param **`key`** The new key string that null to be added.

return **`instance of an object.`**

```C++
FirebaseJson &add(const String &key);
```






#### Add string to FirebaseJson object.
    
param **`key`** The new key string that string value to be added.

param **`value`** The String value for the new specified key.

return **`instance of an object.`**

```C++
FirebaseJson &add(const String &key, const String &value);
```






#### Add string (chars array) to FirebaseJson object.
    
param **`key`** The new key string that string (chars array) value to be added.

param **`value`** The char array for the new specified key.

return **`instance of an object.`**

```C++
FirebaseJson &add(const String &key, const char *value);
```






#### Add integer/unsigned short to FirebaseJson object.
    
param **`key`** The new key string that the value to be added.

param **`value`** The integer/unsigned short value for new specified key.

return **`instance of an object.`**

```C++
FirebaseJson &add(const String &key, int value);
FirebaseJson &add(const String &key, unsigned short value);
```





#### Add float to FirebaseJson object.
    
param **`key`** The new key string that float value to be added.

param **`value`** The float value for the new specified key.

return **`instance of an object.`**

```C++
FirebaseJson &add(const String &key, float value);
```






#### Add double to FirebaseJson object.
    
param **`key`** The new key string that double value to be added.

param **`value`** The double value for the new specified key.

return **`instance of an object.`**

```C++
FirebaseJson &add(const String &key, double value);
```





#### Add boolean to FirebaseJson object.
    
param **`key`** The new key string that bool value to be added.

param **`value`** The boolean value for new specified key.

return **`instance of an object.`**

```C++
FirebaseJson &add(const String &key, bool value);
```





#### Add nested FirebaseJson object into FirebaseJson object.
    
param **`key`** The new key string that FirebaseJson object to be added.

param **`json`** The FirebaseJson object for the new specified key.

return **`instance of an object.`**

```C++
FirebaseJson &add(const String &key, FirebaseJson &json);
```






#### Add nested FirebaseJsonArray object into FirebaseJson object.
    
param **`key`** The new key string that FirebaseJsonArray object to be added.

param **`arr`** The FirebaseJsonArray for the new specified key.

return **`instance of an object.`**

```C++
FirebaseJson &add(const String &key, FirebaseJsonArray &arr);
```






#### Get the FirebaseJson object serialized string.

param **`buf`** The returning String object.

param **`prettify`** Boolean flag for return the pretty format string i.e. with text indentation and newline. 


```C++
void toString(String &buf, bool prettify = false);
```






#### Get the value from the specified node path in FirebaseJson object.

param **`jsonData`** The returning FirebaseJsonData that hold the returned data.

param **`path`** Relative path to the specific node in FirebaseJson object.

param **`prettify`** The bool flag for the prettifying string in FirebaseJsonData's stringValue.

return **`boolean status of the operation.`**

    The FirebaseJsonData object holds the returned data which can be read from the following properties.

    jsonData.stringValue - contains the returned string.

    jsonData.intValue - contains the returned integer value.

    jsonData.floatValue - contains the returned float value.

    jsonData.doubleValue - contains the returned double value.

    jsonData.boolValue - contains the returned boolean value.

    jsonData.success - used to determine the result of the get operation.

    jsonData.type - used to determine the type of returned value in string represent 
    the types of value e.g. string, int, double, boolean, array, object, null and undefined.

    jsonData.typeNum used to determine the type of returned value is an integer as represented by the following value.
    
    FirebaseJson::UNDEFINED = 0
    FirebaseJson::OBJECT = 1
    FirebaseJson::ARRAY = 2
    FirebaseJson::STRING = 3
    FirebaseJson::INT = 4
    FirebaseJson::FLOAT = 5
    FirebaseJson::DOUBLE = 6
    FirebaseJson::BOOL = 7 and
    FirebaseJson::NULL = 8

 
 ```C++
 bool get(FirebaseJsonData &jsonData, const String &path, bool prettify = false);
 ```






#### Parse and collect all node/array elements in FirebaseJson object.  

param **`data`** The JSON data string to parse (optional for replacing the internal buffer with new data).

return **`number`** of child/array elements in FirebaseJson object.

 ```C++
 size_t iteratorBegin(const char* data = NULL);
 ```






#### Get child/array elements from FirebaseJson objects at specified index.
    
param **`index`** The element index to get.

param **`type`** The integer which holds the type of data i.e. FirebaseJson::OBJECT and FirebaseJson::ARR

param **`key`** The string which holds the key/name of the object, can return empty String if the data type is an array.

param **`value`** The string which holds the value for the element key or array.   

 ```C++
 void iteratorGet(size_t index, int &type, String &key, String &value);
 ```





#### Clear all iterator buffer (should be called since iteratorBegin was called).

 ```C++
 void iteratorEnd();
 ```




#### Set null to FirebaseJson object at the specified node path.
    
param **`path`** The relative path that null to be set.


The relative path can be mixed with array index (number placed inside square brackets) and node names e.g. /myRoot/[2]/Sensor1/myData/[3].


```C++
void set(const String &path);
```





#### Set String value to FirebaseJson object at the specified node path.
    
param **`path`** The relative path that string value to be set.

param **`value`** The string value to set.


The relative path can be mixed with array index (number placed inside square brackets) and node names 
e.g. /myRoot/[2]/Sensor1/myData/[3].

```C++
void set(const String &path, const String &value);
```





#### Set string (chars array) value to FirebaseJson object at the specified node path.
    
param **`path`** The relative path that string (chars array) to be set.

param **`value`** The char array to set.

The relative path can be mixed with array index (number placed inside square brackets) and node names 
e.g. /myRoot/[2]/Sensor1/myData/[3].

```C++
void set(const String &path, const char *value);
```





#### Set integer/unsigned short value to FirebaseJson object at specified node path.
    
param **`path`** The relative path that int value to be set.

param **`value`** The integer/unsigned short value to set.

The relative path can be mixed with array index (number placed inside square brackets) and node names 
e.g. /myRoot/[2]/Sensor1/myData/[3].

```C++
void set(const String &path, int value);
void set(const String &path, unsigned short value);
```






#### Set the float value to FirebaseJson object at the specified node path.
    
param **`path`** The relative path that float value to be set.

param **`value`** The float value to set.

The relative path can be mixed with array index (number placed inside square brackets) and node names 
e.g. /myRoot/[2]/Sensor1/myData/[3].

```C++
void set(const String &path, float value);
```






#### Set the double value to FirebaseJson object at the specified node path.
    
param **`path`** The relative path that double value to be set.

param **`value`** The double value to set.

The relative path can be mixed with array index (number placed inside square brackets) and node names 
e.g. /myRoot/[2]/Sensor1/myData/[3].

```C++
void set(const String &path, double value);
```







#### Set the boolean value to FirebaseJson object at the specified node path.
    
param **`path`** The relative path that bool value to be set.

param **`value`** The boolean value to set.

The relative path can be mixed with array index (number placed inside square brackets) and node names 
e.g. /myRoot/[2]/Sensor1/myData/[3].

```C++
void set(const String &path, bool value);
```







#### Set nested FirebaseJson object to FirebaseJson object at the specified node path.
    
param **`path`** The relative path that nested FirebaseJson object to be set.

param **`json`** The FirebaseJson object to set.

The relative path can be mixed with array index (number placed inside square brackets) and node names 
e.g. /myRoot/[2]/Sensor1/myData/[3].

 ```C++
void set(const String &path, FirebaseJson &json);
```







#### Set nested FirebaseJsonAtrray object to FirebaseJson object at specified node path.
    
param **`path`** The relative path that nested FirebaseJsonAtrray object to be set.

param **`arr`** The FirebaseJsonAtrray object to set.

The relative path can be mixed with array index (number placed inside square brackets) and node names 
e.g. /myRoot/[2]/Sensor1/myData/[3].

```C++
void set(const String &path, FirebaseJsonArray &arr);
```






#### Remove the specified node and its content.

param **`path`** The relative path to remove its contents/children.

return **`bool`** value represents the successful operation.

```C++
bool remove(const String &path);
```





### FirebaseJsonArray object functions


#### Add null to FirebaseJsonArray object.

return **`instance of an object.`**

```C++
FirebaseJsonArray &add();
```






#### Add string to FirebaseJsonArray object.

param **`value`** The String value to add.

return **`instance of an object.`**

```C++
FirebaseJsonArray &add(const String &value);
```






#### Add string (chars arrar) to FirebaseJsonArray object.

param **`value`** The chars array to add.

return **`instance of an object.`**

```C++
FirebaseJsonArray &add(const char *value);
```





#### Add integer/unsigned short to FirebaseJsonArray object.

param **`value`** The integer/unsigned short value to add.

return **`instance of an object.`**

```C++
FirebaseJsonArray &add(int value);
FirebaseJsonArray &add(unsigned short value);
```





#### Add float to FirebaseJsonArray object.

param **`value`** The float value to add.

return **`instance of an object.`**

```C++
FirebaseJsonArray &add(float value);
```






#### Add double to FirebaseJsonArray object.

param **`value`** The double value to add.

return **`instance of an object.`**

```C++
FirebaseJsonArray &add(double value);
```






#### Add boolean to FirebaseJsonArray object.

param **`value`** The boolean value to add.

return **`instance of an object.`**

```C++
FirebaseJsonArray &add(bool value);
```






#### Add nested FirebaseJson object  to FirebaseJsonArray object.

param **`json`** The FirebaseJson object to add.

return **`instance of an object.`**

```C++
FirebaseJsonArray &add(FirebaseJson &json);
```





#### Add nested FirebaseJsonArray object  to FirebaseJsonArray object.

param **`arr`** The FirebaseJsonArray object to add.

return **`instance of an object.`**

```C++
FirebaseJsonArray &add(FirebaseJsonArray &arr);
```





#### Get the array value at specified index from FirebaseJsonArray object.

param **`jsonObj`** The returning FirebaseJsonData object that holds data at the specified index.

param **`index`** Index of data in FirebaseJsonArray object.    

return **`boolean`** status of the operation.

```C++
bool get(FirebaseJsonData &jsonObj, int index);
bool get(FirebaseJsonData *jsonData, int index);
```







#### Get the array value at the specified path from FirebaseJsonArray object.

param **`jsonObj`** The returning FirebaseJsonData object that holds data at the specified path.

param **`path`** Relative path to data in FirebaseJsonArray object.    

return **`boolean status of the operation.`**

The relative path must begin with array index (number placed inside square brackets) followed by 
other array indexes or node names e.g. /[2]/myData would get the data from myData key inside the array indexes 2.

```C++
bool get(FirebaseJsonData &jsonData, const String &path);
```






#### Get the length of array in FirebaseJsonArray object.  

return **`length of the array.`**

```C++
size_t size();
```






#### Get the FirebaseJsonArray object serialized string.

param **`buf`** The returning String object.

param **`prettify`** Boolean flag for return the pretty format string i.e. with text indentation and newline. 


```C++
void toString(String &buf, bool prettify = false);
```






#### Clear all array in FirebaseJsonArray object.

return **`instance of an object.`**

```C++
FirebaseJsonArray &clear();
```







#### Set null to FirebaseJsonArray object at specified index.
    
param **`index`** The array index to set null.

```C++
void set(int index);
```







#### Set String to FirebaseJsonArray object at specified index.
    
param **`index`**The array index that String value to be set.

param **`value`** The String to set.


```C++
void set(int index, const String &value);
```







#### Set string (chars array) to FirebaseJsonArray object at specified index.
    
param **`index`** The array index that string (chars array) to be set.

param **`value`** The char array to set.

```C++
void set(int index, const char *value);
```





#### Set integer/unsigned short value to FirebaseJsonArray object at specified index.
    
param **`index`** The array index that int/unsigned short to be set.

param **`value`** The integer/unsigned short value to set.

```C++
void set(int index, int value);
void set(int index, unsigned short value);
```




#### Set float value to FirebaseJsonArray object at specified index.
    
param **`index`** The array index that float value to be set.

param **`value`** The float value to set.

```C++
void set(int index, float value);
```






#### Set double value to FirebaseJsonArray object at specified index.
    
param **`index`** The array index that double value to be set.

param **`value`** The double value to set.

```C++
void set(int index, double value);
```






#### Set boolean value to FirebaseJsonArray object at specified index.
    
param **`index`** The array index that bool value to be set.

param **`value`** The boolean value to set.

```C++
void set(int index, bool value);
```






#### Set nested FirebaseJson object to FirebaseJsonArray object at specified index.
    
param **`index`** The array index that nested FirebaseJson object to be set.

param **`value`** The FirebaseJson object to set.

```C++
void set(int index, FirebaseJson &json);
```






#### Set nested FirebaseJsonArray object to FirebaseJsonArray object at specified index.
    
param **`index`** The array index that nested FirebaseJsonArray object to be set.

param **`value`** The FirebaseJsonArray object to set.

```C++
void set(int index, FirebaseJsonArray &arr);
```






    
#### Set null to FirebaseJson object at the specified path.
    
param **`path`** The relative path that null to be set.

The relative path must begin with array index (number placed inside square brackets) followed by 
other array indexes or node names e.g. /[2]/myData would get the data from myData key inside the array indexes 2.

```C++
void set(const String &path);
```





#### Set String to FirebaseJsonArray object at specified path.
    
param **`path`** The relative path that string value to be set.

param **`value`** The String to set.

The relative path must begin with array index (number placed inside square brackets) followed by 
other array indexes or node names e.g. /[2]/myData would get the data from myData key inside the array indexes 2.

```C++
void set(const String &path, const String &value);
```




#### Set string (chars array) to FirebaseJsonArray object at specified path.
    
param **`path`** The relative path that string (chars array) value to be set.

param **`value`** The char array to set.

The relative path must begin with array index (number placed inside square brackets) followed by 
other array indexes or node names e.g. /[2]/myData would get the data from myData key inside the array indexes 2.

```C++
void set(const String &path, const char *value);
```





#### Set integer/unsigned short value to FirebaseJsonArray object at specified path.
    
param **`path`** The relative path that integer/unsigned short value to be set.

param **`value`** The integer value to set.

The relative path must begin with array index (number placed inside square brackets) followed by 
other array indexes or node names e.g. /[2]/myData would get the data from myData key inside the array indexes 2.

```C++
void set(const String &path, int value);
void set(const String &path, unsigned short value);
```





#### Set float value to FirebaseJsonArray object at specified path.
    
param **`path`** The relative path that float value to be set.

param **`value`** The float to set.

The relative path must begin with array index (number placed inside square brackets) followed by 
other array indexes or node names e.g. /[2]/myData would get the data from myData key inside the array indexes 2.

 ```C++
void set(const String &path, float value);
```







#### Set double value to FirebaseJsonArray object at specified path.
    
param **`path`** The relative path that double value to be set.

param **`value`** The double to set.

The relative path must begin with array index (number placed inside square brackets) followed by 
other array indexes or node names e.g. /[2]/myData would get the data from myData key inside the array indexes 2.

 ```C++
void set(const String &path, double value);
```





#### Set boolean value to FirebaseJsonArray object at specified path.
    
param **`path`** The relative path that bool value to be set.

param **`value`** The boolean value to set.

The relative path must begin with array index (number placed inside square brackets) followed by 
other array indexes or node names e.g. /[2]/myData would get the data from myData key inside the array indexes 2.

```C++
void set(const String &path, bool value);
```





#### Set the nested FirebaseJson object to FirebaseJsonArray object at the specified path.
    
param **`path`** The relative path that nested FirebaseJson object to be set.

param **`value`** The FirebaseJson object to set.

The relative path must begin with array index (number placed inside square brackets) followed by 
other array indexes or node names e.g. /[2]/myData would get the data from myData key inside the array indexes 2.

```C++
void set(const String &path, FirebaseJson &json);
```






#### Set the nested FirebaseJsonArray object to FirebaseJsonArray object at specified path.
    
param **`path`** The relative path that nested FirebaseJsonArray object to be set.

param **`value`** The FirebaseJsonArray object to set.

The relative path must begin with array index (number placed inside square brackets) followed by 
other array indexes or node names e.g. /[2]/myData would get the data from myData key inside the array indexes 2.

```C++
void set(const String &path, FirebaseJsonArray &arr);
```






#### Remove the array value at specified index from FirebaseJsonArray object.

param **`index`** The array index to be removed.

return **`bool`** value represents the successful operation.

```C++
bool remove(int index);
```






#### Remove the array value at the specified path from FirebaseJsonArray object.

param **`path`** The relative path to array in FirebaseJsonArray object to be removed.

return **`bool`** value represents the successful operation.

The relative path must begin with array index (number placed inside square brackets) followed by 
other array indexes or node names e.g. /[2]/myData would get the data from myData key inside the array indexes 2.

```C++
bool remove(const String &path);
```





### FirebaseJsonData object functions


#### Get array data as FirebaseJsonArray object from FirebaseJsonData object.
    
param **`jsonArray`**The returning FirebaseJsonArray object.

return **`bool`** status for successful operation.

This should call after pares or get functions.

```C++
bool getArray(FirebaseJsonArray &jsonArray);
```





#### Get array data as FirebaseJson object from FirebaseJsonData object.
    
param **`jsonArray`** The returning FirebaseJson object.

return **`bool`** status for successful operation.

This should call after pares or get functions.

```C++
bool getJSON(FirebaseJson &json);
```



### FirebaseJsonData object properties


**`stringValue`** The String value of parses data.

**`intValue`** The int value of parses data.

**`floatValue`** The double value of parses data.

**`doubleValue`** The double value of parses data.

**`boolValue`** The bool value of parses data.

**`success`** used to determine the result of the get operation.

**`type`** The type String of parses data e.g. string, int, double, boolean, array, object, null and undefined.

**`typeNum`** The type (number) of parses data in form of the following **`jsonDataType`** value.

**FirebaseJson::UNDEFINED = 0**

**FirebaseJson::OBJECT = 1**

**FirebaseJson::ARRAY = 2**

**FirebaseJson::STRING = 3**

**FirebaseJson::INT = 4**

**FirebaseJson::FLOAT = 5**

**FirebaseJson::DOUBLE = 6**

**FirebaseJson::BOOL = 7 and**

**FirebaseJson::NULL = 8**

**`success`** The success flag of parsing data.






## License

The MIT License (MIT)

Copyright (c) 2020 K. Suwatchai (Mobizt)


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

