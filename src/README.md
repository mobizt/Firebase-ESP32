# Firebase Realtime Database Arduino Library for ESP32


Google's Firebase Realtime Database Arduino Library for ESP32 v3.11.9

<br/>

## Global functions

<br/>

#### Initialize Firebase with the config and Firebase's authentication credentials.

param **`config`** The pointer to FirebaseConfig data.

param **`auth`** The pointer to FirebaseAuth data.

note: For FirebaseConfig and FirebaseAuth data usage, see the examples.

```cpp
void begin(FirebaseConfig *config, FirebaseAuth *auth);
```

<br/>

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

```cpp
struct token_info_t authTokenInfo();
```

<br/>

#### Provide the ready status of token generation.

return **`Boolean`** type status indicates the token generation is completed.

```cpp
bool ready();
```

<br/>

#### Provide the grant access status for Firebase Services.

return **`Boolean`** type status indicates the device can access to the services.

```cpp
bool authenticated();
```

<br/>

#### Store Firebase's legacy authentication credentials.

param **`databaseURL`** Your RTDB URL e.g. \<databaseName\>.firebaseio.com or \<databaseName\>.\<region\>.firebasedatabase.app

param **`databaseSecret`** Your database secret.

param **`caCert`** Root CA certificate base64 string (PEM file).

param **`caCertFile`** Root CA certificate DER file (binary).

param **`StorageType`** Type of storage, StorageType::SD and StorageType::FLASH.

The file systems for flash and sd memory can be changed in FirebaseFS.h.

```cpp
void begin(<string> databaseURL, <string> databaseSecret);

void begin(<string> databaseURL, <string> databaseSecret, <char array> caCert);

void begin(<string> databaseURL, <string> databaseSecret, <string> caCertFile, uint8_t storageType);
```

<br/>

#### Stop Firebase and release all resources.

param **`fbdo`** Firebase Data Object to hold data and instance.

```cpp
void end(FirebaseData &fbdo);
```

<br/>

#### Sign up for a new user.

param **`config`** The pointer to FirebaseConfig data.

param **`auth`** The pointer to FirebaseAuth data.

param **`email`** The user Email.

param **`password`** The user password.

return **`Boolean`** type status indicates the success of the operation. 

<br/>

note: By calling Firebase.begin with config and auth after sign up will be signed in.

This required Email/Password provider to be enabled,

From Firebase console, select Authentication, select Sign-in method tab, under 
the Sign-in providers list, enable Email/Password provider.

If the assigned email and passowrd are empty, the anonymous user will be created if Anonymous provider is enabled.

To enable Anonymous provider, from Firebase console, select Authentication, 
select Sign-in method tab, under the Sign-in providers list, enable Anonymous provider.

```cpp
bool signUp(FirebaseConfig *config, FirebaseAuth *auth, <string> email, <string> password);
```

<br/>

#### Send a user a verification Email.

param **`config`** The pointer to FirebaseConfig data.

param **`idToken`** The id token of user that was already signed in with Email and password (optional).

return **`Boolean`** type status indicates the success of the operation. 

<br/>

note: The id token can be obtained from config.signer.tokens.id_token after begin with config and auth data

If the idToken is not assigned, the internal config.signer.tokens.id_token will be used. 

See the Templates of Email address verification in the Firebase console, Authentication.

```cpp
bool sendEmailVerification(FirebaseConfig *config,<string> idToken);
```

<br/>

#### Send a user a password reset link to Email.

param **`config`** The pointer to FirebaseConfig data.

param **`email`** The user Email to send the password resset link.

return **`Boolean`** type status indicates the success of the operation. 

```cpp
bool sendResetPassword(FirebaseConfig *config, <string> email);
```

<br/>

#### Reconnect WiFi if lost connection

param **`reconnect`** The boolean to set/unset WiFi AP reconnection.

```cpp
void reconnectWiFi(bool reconnect);
```

<br/>

#### Get currently used auth token string

return **`constant char*`** of currently used auth token.

```cpp
const char *getToken();
```

<br/>

#### Set the decimal places for float value to be stored in database.

param **`digits`** The decimal places. 

```cpp
void setFloatDigits(uint8_t digits);
```

<br/>

#### Set the decimal places for double value to be stored in database.

param **`digits`** The decimal places. 

```cpp
void setDoubleDigits(uint8_t digits);
```

<br/>

#### Get http response code of reading JSON data from WiFi/Ethernet Client.

return **`the response code`** of reading JSON data from WiFi/Ethernet Client 

```cpp
int responseCode();
```

<br/>

#### Enable multiple HTTP requests at a time (ESP32 only).
    
param **`enable`** The boolean value to enable/disable.

The multiple HTTP requessts at a time is disable by default to prevent 
the large memory used in multiple requests.

```cpp
  void allowMultipleRequests(bool enable);
```

<br/>

#### Set the timeouts of Firebase.get function.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`millisec`** The milliseconds to limit the request (0 - 900,000 ms or 15 min).

```cpp
void setReadTimeout(FirebaseData &fbdo, int millisec);
```

<br/>

#### Set the size limit of payload data that will write to the database for each request.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`size`** The size identified string e.g. tiny, small, medium, large and unlimited.

Size string and its write timeout in seconds e.g. tiny (1s), small (10s), medium (30s) and large (60s).

```cpp
void setwriteSizeLimit(FirebaseData &fbdo, <string> size);
```

<br/>

#### Read the database rules

param **`fbdo`** Firebase Data Object to hold data and instances.

return - **`Boolean`** type status indicates the success of the operation.

```cpp
bool getRules(FirebaseData &fbdo);
```

<br/>

#### Write the database rules

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`rules`** Database rules in jSON String format.

return - **`Boolean`** type status indicates the success of the operation.

```cpp
bool setRules(FirebaseData &fbdo, <string> rules);
```

<br/>

#### Set the .read and .write database rules.

param **`fbdo`** The pointer to Firebase Data Object.

param **`path`** The parent path of child's node that the .read and .write rules are being set.

param **`var`** The child node key that the .read and .write rules are being set.

param **`readVal`** The child node key .read value.

param **`writeVal`** The child node key .write value.

param **`databaseSecret`** The database secret.

return - **`Boolean`** value, indicates the success of the operation.

note: The databaseSecret can be empty if the auth type is OAuth2.0 or legacy and required if auth type is Email/Password sign-in. 

```cpp
bool setReadWriteRules(FirebaseData &fbdo, <string> path, <string> var, <string> readVal, <string> writeVal, <string> databaseSecret);
```

<br/>

#### Set the query index to the database rules.

param **`fbdo`** The pointer to Firebase Data Object.

param **`path`** The parent path of child's node that being query.

param **`node`** The child node key that being query.

param **`databaseSecret`** The database secret.

return - **`Boolean`** value, indicates the success of the operation.

note: The databaseSecret can be empty if the auth type is OAuth2.0 or legacy and required if auth type is Email/Password sign-in.

```cpp
bool setQueryIndex(FirebaseData &fbdo, <string> path, <string> node, <string> databaseSecret);
```

<br/>

#### Remove the query index from the database rules.

param **`fbdo`** The pointer to Firebase Data Object.

param **`path`** The parent path of child's node that the index is being removed.

param **`databaseSecret`** The database secret.

return - **`Boolean`** value, indicates the success of the operation.

note: The databaseSecret can be empty if the auth type is OAuth2.0 or legacy and required if auth type is Email/Password sign-in.

```cpp
bool removeQueryIndex(FirebaseData &fbdo, <string> path, <string> databaseSecret);
```

<br/>

#### Get whether defined database path exists or not

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path to be checked.

return - **`Boolean`** type result indicates whether the defined database path has existed or not.

```cpp
bool pathExisted(FirebaseData &dataObj, <string> path);
```

<br/>

#### Get the unique identifier (ETag) of current data at the defined database path

return **`String`** of unique identifier.

```cpp
String getETag(FirebaseData &fbdo, <string> path);
```

<br/>

#### Get the shallowed data at defined node path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path being read the data.

return - **`Boolean`** type status indicates the success of the operation.

<br/>

Return the child data with its value or JSON object (its values will be truncated to true). 

The data can be read from FirebaseData object.

```cpp
bool getShallowData(FirebaseData &fbdo, <string> path); 
```

<br/>

#### Enable the library to use only classic HTTP GET and POST methods

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`flag`** Boolean value to enable.

<br/>

This option used to escape the Firewall restriction (if device is connected through Firewall) 
that allows only HTTP GET and POST
    
HTTP PATCH request was sent as PATCH which not affected by this option.

```cpp
void enableClassicRequest(FirebaseData &fbdo, bool flag);
```

<br/>

#### Set the virtual child node ".priority" to the defined database path. 
    
param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which to set the priority value.

param **`priority`** The priority value.
    
return - **`Boolean`** type status indicates the success of the operation.

<br/>

This allows us to set priority to any node other than a priority that set through setJSON, 
pushJSON, updateNode, and updateNodeSilent functions.
    
The returned priority value from server can read from function \<FirebaseData\>.priority().

```cpp
bool setPriority(FirebaseData &fbdo, <string> path, float priority);

bool setPriorityAsync(FirebaseData &fbdo, <string> path, float priority);
```

<br/>

#### Read the virtual child node ".priority" value at the defined database path. 
    
param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which to set the priority value.
    
return - **`Boolean`** type status indicates the success of the operation.

The priority value from server can read from function \<FirebaseData\>.priority().

 ```cpp
bool getPriority(FirebaseData &fbdo, <string> path);
```

<br/>

####  Append new generic value to the defined node.

param **`fbdo`** The pointer to Firebase Data Object.

param **`path`** The path to the node.

param **`value`** The appended value of any type e.g. bool, int, float, double, String, FirebaseJson object and array.

param **`blob`** Byte array of data (blob).

param **`size`** Size of the byte array (blob).

param **`storageType`** The enum of memory storage type e.g. mem_storage_type_flash and mem_storage_type_sd.

The file systems can be changed in FirebaseFS.h.

param **`fileName`** The file path includes its name.

return **`Boolean`** value, indicates the success of the operation.

The key or name of new created node will be stored in Firebase Data object, 
call \<FirebaseData\>.pushName() to get the key.

```cpp
bool push(FirebaseData &fbdo, <string> path, <type> value);

bool pushAsync(FirebaseData &fbdo, <string> path, <type> value);

bool push(FirebaseData &fbdo, <string> path, uint8_t *blob, size_t size);

bool pushAsync(FirebaseData &fbdo, <string> path, uint8_t *blob, size_t size);

bool push(FirebaseData &fbdo, fb_esp_mem_storage_type storageType, <string> path, <string> fileName);

bool pushAsync(FirebaseData &fbdo, fb_esp_mem_storage_type storageType, <string> path, <string> fileName);
```

<br/>

#### Append new genric value and the virtual child ".priority" to the defined node.

param **`fbdo`** The pointer to Firebase Data Object.

param **`path`** The path to the node.

param **`value`** The appended value of any type e.g. bool, int, float, double, String, FirebaseJson object and array.

param **`priority`** The priority value.

return **`Boolean`** value, indicates the success of the operation.

The key or name of new created node will be stored in Firebase Data object, 
call \<FirebaseData\>.pushName() to get the key.

```cpp
bool push(FirebaseData &fbdo, <string> path, <type> value,  float priority);

bool pushAsync(FirebaseData &fbdo, <string> path, <type> value,  float priority);
```

<br/>

#### Append new integer value to the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which integer value will be appended.

param **`value`** The appended value.

return **`Boolean`** type status indicates the success of the operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function \<FirebaseData\>.pushName().

```cpp
bool pushInt(FirebaseData &fbdo, <string> path, <integer> value);

bool pushIntAsync(FirebaseData &fbdo, <string> path, <integer> value);
```

<br/>

#### Append new integer value and the virtual child ".priority" to the defined database path.

```cpp
bool pushInt(FirebaseData &fbdo, <string> path, <integer> value, float priority);

bool pushIntAsync(FirebaseData &fbdo, <string> path, <integer> value, float priority);
```

<br/>

#### Append new float value to the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which float value will be appended.

param **`value`** The appended value.

return **`Boolean`** type status indicates the success of the operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function \<FirebaseData\>.pushName().

```cpp
bool pushFloat(FirebaseData &fbdo, <string> path, float value);

bool pushFloatAsync(FirebaseData &fbdo, <string> path, float value);
```

<br/>

#### Append new float value and the virtual child ".priority" to the defined database path.

```cpp
bool pushFloat(FirebaseData &fbdo, <string> path, float value, float priority);

bool pushFloatAsync(FirebaseData &fbdo, <string> path, float value, float priority);
```

<br/>

#### Append new double value (8 bytes) to the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which float value will be appended.

param **`value`** The appended value.

return **`Boolean`** type status indicates the success of the operation.

The new appended node's key will be stored in Firebase Data object,
which its value can be accessed via function \<FirebaseData\>.pushName().

```cpp
bool pushDouble(FirebaseData &fbdo, <string> path, double value);

bool pushDoubleAsync(FirebaseData &fbdo, <string> path, double value);
```

<br/>

#### Append new double value (8 bytes) and the virtual child ".priority" to the defined database path.

```cpp
bool pushDouble(FirebaseData &fbdo, <string> path, double value, float priority);

bool pushDoubleAsync(FirebaseData &fbdo, <string> path, double value, float priority);
```

<br/>

#### Append new Boolean value to the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which Boolean value will be appended.

param **`value`** The appended value.

return **`Boolean`** type status indicates the success of the operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function \<FirebaseData\>.pushName().

```cpp
bool pushBool(FirebaseData &fbdo, <string> path, bool value);

bool pushBoolAsync(FirebaseData &fbdo, <string> path, bool value);
```

<br/>

#### Append new Boolean value and the virtual child ".priority" to the defined database path.

```cpp
bool pushBool(FirebaseData &fbdo, <string> path, bool value, float priority);

bool pushBoolAsync(FirebaseData &fbdo, <string> path, bool value, float priority);
```

<br/>

#### Append new string (text) to the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which string will be appended.

param **`value`** The appended value.

return **`Boolean`** type status indicates the success of the operation.

The new appended node's key stored in Firebase Data object, 
which can be accessed via function \<FirebaseData\>.pushName().

```cpp
bool pushString(FirebaseData &fbdo, <string> path, <string> value);

bool pushStringAsync(FirebaseData &fbdo, <string> path, <string> value);
```

<br/>

#### Append new string (text) and the virtual child ".priority" to the defined database path.

```cpp
bool pushString(FirebaseData &fbdo, <string> path, <string> value, float priority);

bool pushStringAsync(FirebaseData &fbdo, <string> path, <string> value, float priority);
```

<br/>

#### Append new child node key and value (using FirebaseJson object) to the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which key and value in FirebaseJson object will be appended.

param **`json`** The appended FirebaseJson object.

return **`Boolean`** type status indicates the success of the operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function \<FirebaseData\>.pushName().

```cpp
bool pushJSON(FirebaseData &fbdo, <string> path, FirebaseJson &json);

bool pushJSONAsync(FirebaseData &fbdo, <string> path, FirebaseJson &json);
```

<br/>

#### Append new child node key and value (using JSON data or FirebaseJson object) and the virtual child ".priority" to the defined database path.

```cpp
bool pushJSON(FirebaseData &fbdo, <string> path, FirebaseJson &json, float priority);

bool pushJSONAsync(FirebaseData &fbdo, <string> path, FirebaseJson &json, float priority);
```

<br/>

#### Append child node array (using FirebaseJsonArray object) to the defined database path.

This will replace any child nodes inside the defined path with array defined in FirebaseJsonArray object.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which key and value in FirebaseJsonArray object will be appended.

param **`arr`** The appended FirebaseJsonArray object.

return **`Boolean`** type status indicates the success of the operation.

The new appended node's key will be stored in Firebase Data object,
which its value can be accessed via function \<FirebaseData\>.pushName().

```cpp
bool pushArray(FirebaseData &fbdo, <string> path, FirebaseJsonArray &arr);

bool pushArrayAsync(FirebaseData &fbdo, <string> path, FirebaseJsonArray &arr);
```

<br/>

#### Append FirebaseJsonArray object and virtual child ".priority" at the defined database path.

```cpp
bool pushArray(FirebaseData &fbdo, <string> path, FirebaseJsonArray &arr, float priority);

bool pushArrayAsync(FirebaseData &fbdo, <string> path, FirebaseJsonArray &arr, float priority);
```

<br/>

#### Append new blob (binary data) to the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which binary data will be appended.

param **`blob`** Byte array of data.

param **`size`** Size of array in byte.

return **`Boolean`** type status indicates the success of the operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function \<FirebaseData\>.pushName().

```cpp
bool pushBlob(FirebaseData &fbdo, <string> path, uint8_t *blob, size_t size);

bool pushBlobAsync(FirebaseData &fbdo, <string> path, uint8_t *blob, size_t size);
```

<br/>

#### Append new binary data from file stored on SD card/Flash memory to the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`storageType`** Type of storage to read file data, StorageType::FLASH or StorageType::SD.

param **`path`** Target database path in which binary data from the file will be appended.

param **`fileName`** File name included its path in SD card/Flash memory.

return **`Boolean`** type status indicates the success of the operation.

The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function \<FirebaseData\>.pushName().

The file systems for flash and sd memory can be changed in FirebaseFS.h.

```cpp
bool pushFile(FirebaseData &fbdo, uint8_t storageType, <string> path, <string> fileName);

bool pushFileAsync(FirebaseData &fbdo, uint8_t storageType, <string> path, <string> fileName);
```

<br/>

 #### Append new Firebase server's timestamp to the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which timestamp will be appended.

return - **`Boolean`** type status indicates the success of the operation.
    
The new appended node's key will be stored in Firebase Data object, 
which its value can be accessed via function \<FirebaseData\>.pushName().

```cpp
bool pushTimestampAsync(FirebaseData &fbdo, <string> path);

bool pushTimestampAsync(FirebaseData &fbdo, <string> path);
```

<br/>

#### Set the generic value at the defined node.

param **`fbdo`** The pointer to Firebase Data Object.

param **`path`** The path to the node which integer value will be set.

param **`value`** the value to set of any type e.g. bool, int, float, double, String, FirebaseJson object and array.

param **`blob`** Byte array of data (blob).

param **`size`** Size of the byte array (blob).

param **`fileName`** The file path includes its name.

return **`Boolean`** value, indicates the success of the operation.

<br/>

Call \<FirebaseData\>.dataType or \<FirebaseData\>.dataTypeNum to get the type of data that successfully stored in the database. 

Call \<FirebaseData\>.to<type>() to get value (cast) that stored on the defined node e.g. \<FirebaseData\>.to<String>() to get the String value.

```cpp
bool set(FirebaseData &fbdo, <string> path, <type> value);

bool setAsync(FirebaseData &fbdo, <string> path, <type> value);

bool set(FirebaseData &fbdo, <string> path, uint8_t *blob, size_t size);

bool setAsync(FirebaseData &fbdo, <string> path, uint8_t *blob, size_t size);

bool set(FirebaseData &fbdo, fb_esp_mem_storage_type storageType, <string> path, <string> fileName);

bool setAsync(FirebaseData &fbdo, fb_esp_mem_storage_type storageType, <string> path, <string> fileName);
```

<br/>

#### Set the generic value and virtual child ".priority" at the defined node.

param **`fbdo`** The pointer to Firebase Data Object.

param **`path`** The path to the node which integer value will be set.

param **`value`** the value to set of any type e.g. bool, int, float, double, String, FirebaseJson object and array.

param **`priority`** The priority value.

return **`Boolean`** value, indicates the success of the operation.

<br/>

Call \<FirebaseData\>.dataType or \<FirebaseData\>.dataTypeNum to get the type of data that successfully stored in the database. 

Call \<FirebaseData\>.to<type>() to get value (cast) that stored on the defined node e.g. \<FirebaseData\>.to<String>() to get the String value.

```cpp
bool set(FirebaseData &fbdo, <string> path, <type> value, float priority);

bool setAsync(FirebaseData &fbdo, <string> path, <type> value, float priority);
```

<br/>

#### Set the generic value if defined node's ETag matched the defined ETag value.

param **`fbdo`** The pointer to Firebase Data Object.

param **`path`** The path to the node which integer value will be set.

param **`value`** the value to set of any type e.g. bool, int, float, double, String, FirebaseJson object and array.

param **`blob`** Byte array of data (blob).

param **`size`** Size of the byte array (blob).

param **`fileName`** The file path includes its name.

param **`ETag`** Known unique identifier string (ETag) of defined node.

return **`Boolean`** value, indicates the success of the operation.

<br/>

If ETag at the defined node does not match the provided ETag parameter,
the operation will be failed with the http return code 412, Precondition Failed (ETag is not matched). 

If the operation failed due to ETag is not match, call \<FirebaseData\>.ETag() to get the current ETag value. 

Also call \<FirebaseData\>.dataType or \<FirebaseData\>.dataTypeNum to get the type of data that successfully stored in the database. 

And \<FirebaseData\>.to<type>() to get value (cast) that stored on the defined node e.g. \<FirebaseData\>.to<String>() to get the String value.

```cpp
bool set(FirebaseData &fbdo, <string> path, <type> value, <string> ETag);

bool setAsync(FirebaseData &fbdo, <string> path, <type> value, <string> ETag);

bool set(FirebaseData &fbdo, <string> path, uint8_t *blob, size_t size, <string> ETag);

bool setAsync(FirebaseData &fbdo, <string> path, uint8_t *blob, size_t size, <string> ETag);

bool set(FirebaseData &fbdo, fb_esp_mem_storage_type storageType, <string> path, <string> fileName, <string> ETag);

bool setAsync(FirebaseData &fbdo, fb_esp_mem_storage_type storageType, <string> path, <string> fileName, <string> ETag);
```

<br/>

#### Set the generic value and the virtual child ".priority" if defined ETag matches at the defined node. 

param **`fbdo`** The pointer to Firebase Data Object.

param **`path`** The path to the node which integer value will be set.

param **`value`** the value to set of any type e.g. bool, int, float, double, String, FirebaseJson object and array.

param **`priority`** The priority value.

param **`ETag`** Known unique identifier string (ETag) of defined node.

return **`Boolean`** value, indicates the success of the operation.

<br/>

If ETag at the defined node does not match the provided ETag parameter,
the operation will be failed with the http return code 412, Precondition Failed (ETag is not matched). 

If the operation failed due to ETag is not match, call \<FirebaseData\>.ETag() to get the current ETag value. 

Also call \<FirebaseData\>.dataType or \<FirebaseData\>.dataTypeNum to get the type of data that successfully stored in the database. 

And \<FirebaseData\>.to<type>() to get value (cast) that stored on the defined node e.g. \<FirebaseData\>.to<String>() to get the String value.

```cpp
bool set(FirebaseData &fbdo, <string> path, <type> value, float priority, <string> ETag);

bool setAsync(FirebaseData &fbdo, <string> path, <type> value, float priority, <string> ETag);
```

<br/>

#### Set integer data at the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which integer data will be set.

param **`value`** Integer value to set.

return **`Boolean`** type status indicates the success of the operation.

<br/>

Call \<FirebaseData\>.dataType to determine what type of data successfully
stores in the database. 
    
Call \<FirebaseData\>.to<int>() will return the integer value of
the payload returned from the server.

```cpp
bool setInt(FirebaseData &fbdo, <string> path, <integer> value);

bool setIntAsync(FirebaseData &fbdo, <string> path, <integer> value);
```

<br/>

#### Set integer data and virtual child ".priority" at the defined database path.

```cpp
bool setInt(FirebaseData &fbdo, <string> path, <integer> value, float priority);

bool setIntAsync(FirebaseData &fbdo, <string> path, <integer> value, float priority);
```

<br/>

#### Set integer data at the defined database path if defined database path's ETag matched the ETag value.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which integer data will be set.

param **`value`** Integer value to set.

param **`ETag`** Known unique identifier string (ETag) of defined database path.

return - **`Boolean`** type status indicates the success of the operation.

<br/>

Call \<FirebaseData\>.dataType to determine what type of data successfully stores in the database. 
    
If ETag at the defined database path does not match the provided ETag parameter, the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).

If the operation failed due to ETag is not match, call \<FirebaseData\>.ETag() to get the current ETag value.
Also call \<FirebaseData\>.to<int>() to get the current integer value.
    
```cpp
bool setInt(FirebaseData &fbdo, <string> path, <integer> value, <string> ETag);

bool setIntAsync(FirebaseData &fbdo, <string> path, <integer> value, <string> ETag);
```

<br/>

#### Set integer data and the virtual child ".priority" if defined ETag matches at the defined database path 

```cpp
bool setInt(FirebaseData &fbdo, <string> path, <integer> value, float priority, <string> ETag);

bool setIntAsync(FirebaseData &fbdo, <string> path, <integer> value, float priority, <string> ETag);
```

<br/>

#### Set float data at the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which float data will be set.

param **`value`** Float value to set.

return **`Boolean`** type status indicates the success of the operation.

<br/>

Call \<FirebaseData\>.dataType to determine what type of data successfully stores in the database. 

Call \<FirebaseData\>.to<float>() will return the float value of
the payload returned from the server.

```cpp
bool setFloat(FirebaseData &fbdo, <string> path, float value);

bool setFloatAsync(FirebaseData &fbdo, <string> path, float value);
```

<br/>

#### Set float data and virtual child ".priority" at the defined database path.

```cpp
bool setFloat(FirebaseData &fbdo, <string> path, float value, float priority);

bool setFloatAsync(FirebaseData &fbdo, <string> path, float value, float priority);
```

<br/>

#### Set float data at the defined database path if defined database path's ETag matched the ETag value.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which float data will be set.

param **`value`** Float value to set.

return **`Boolean`** type status indicates the success of the operation.

<br/>

Call \<FirebaseData\>.dataType to determine what type of data successfully stores in the database. 

Call \<FirebaseData\>.to<float>() will return the float value of
the payload returned from the server.

If ETag at the defined database path does not match the provided ETag parameter, the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).

If the operation failed due to ETag is not match, call \<FirebaseData\>.ETag() to get the current ETag value.
Also call \<FirebaseData\>.to<float>() to get the current float value.

```cpp
bool setFloat(FirebaseData &fbdo, <string> path, float value, <string> ETag);

bool setFloatAsync(FirebaseData &fbdo, <string> path, float value, <string> ETag);
```

<br/>

#### Set float data and the virtual child ".priority" if defined ETag matches at the defined database path 

```cpp
bool setFloat(FirebaseData &fbdo, <string> path, float value, float priority, <string> ETag);

bool setFloatAsync(FirebaseData &fbdo, <string> path, float value, float priority, <string> ETag);
```

<br/>

#### Set double data at the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which float data will be set.

param **`value`** Double value to set.

return **`Boolean`** type status indicates the success of the operation.

<br/>

Call \<FirebaseData\>.dataType to determine what type of data that successfully
stores in the database.

Call \<FirebaseData\>.to<double>() will return the double value of
the payload returned from the server.

Due to bugs in Serial.print in Arduino, to print large double value with zero decimal place, 
use printf("%.9lf\n", firebaseData.to<double>()); for print the returned double value up to 9 decimal places.


```cpp
bool setDouble(FirebaseData &fbdo, <string> path, double value);

bool setDoubleAsync(FirebaseData &fbdo, <string> path, double value);
```

<br/>

#### Set double data and virtual child ".priority" at the defined database path.

```cpp
bool setDouble(FirebaseData &fbdo, <string> path, double value, float priority);

bool setDoubleAsync(FirebaseData &fbdo, <string> path, double value, float priority);
```

<br/>

#### Set double data at the defined database path if defined database path's ETag matched the ETag value.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which float data will be set.

param **`value`** Double value to set.

param **`ETag`** Known unique identifier string (ETag) of defined database path.

return - **`Boolean`** type status indicates the success of the operation.

<br/>

Call \<FirebaseData\>.dataType to determine what type of data successfully stores in the database.

Call \<FirebaseData\>.to<double>() will return the double value of payload returned from the server.

<br/>

If ETag at the defined database path does not match the provided ETag parameter, the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).

If the operation failed due to ETag is not match, call \<FirebaseData\>.ETag() to get the current ETag value.
Also call \<FirebaseData\>.doubeData to get the current double value.

```cpp
bool setDouble(FirebaseData &fbdo, <string> path, double value, <string> ETag);

bool setDoubleAsync(FirebaseData &fbdo, <string> path, double value, <string> ETag);
```

<br/>

#### Set double data and the virtual child ".priority" if defined ETag matches at the defined database path 

```cpp
bool setDouble(FirebaseData &fbdo, <string> path, double value, float priority, <string> ETag);

bool setDoubleAsync(FirebaseData &fbdo, <string> path, double value, float priority, <string> ETag);
```

<br/>

#### Set Boolean data at the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which float data will be set.

param **`value`** Boolean value to set.

return **`Boolean`** type status indicates the success of the operation.

<br/>

Call \<FirebaseData\>.dataType to determine what type of data successfully stores in the database. 

Call \<FirebaseData\>.to<bool>() will return the Boolean value of
the payload returned from the server.

```cpp
bool setBool(FirebaseData &fbdo, <string> path, bool value);

bool setBoolAsync(FirebaseData &fbdo, <string> path, bool value);
```

<br/>

#### Set boolean data and virtual child ".priority" at the defined database path.

```cpp
bool setBool(FirebaseData &fbdo, <string> path, bool value, float priority);

bool setBoolAsync(FirebaseData &fbdo, <string> path, bool value, float priority);
```

<br/>

#### Set Boolean data at the defined database path if defined database path's ETag matched the ETag value

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which float data will be set.

param **`value`** Boolean value to set.

param **`ETag`** Known unique identifier string (ETag) of defined database path.

return **`Boolean`** type status indicates the success of the operation.

<br/>

Call \<FirebaseData\>.dataType to determine what type of data that successfully stores in the database. 

Call \<FirebaseData\>.to<bool>() will return the Boolean value of
the payload returned from the server.


If ETag at the defined database path does not match the provided ETag parameter,
the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).

If the operation failed due to ETag is not match, call \<FirebaseData\>.ETag() to get the current ETag value.
Also call \<FirebaseData\>.doubeData to get the current boolean value.

```cpp
bool setBool(FirebaseData &fbdo, <string> path, bool value, <string> ETag);

bool setBoolAsync(FirebaseData &fbdo, <string> path, bool value, <string> ETag);
```

<br/>

#### Set boolean data and the virtual child ".priority" if defined ETag matches at the defined database path 

```cpp
bool setBool(FirebaseData &fbdo, <string> path, bool value, float priority, <string> ETag);

bool setBoolAsync(FirebaseData &fbdo, <string> path, bool value, float priority, <string> ETag);
```

<br/>


#### Set string (text) at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which string data will be set.

param **`value`** String or text to set.

return **`Boolean`** type status indicates the success of the operation.

<br/>

Call \<FirebaseData\>.dataType to determine what type of data successfully stores in the database. 

Call \<FirebaseData\>.to<String>() will return the string value of
the payload returned from the server.

```cpp
bool setString(FirebaseData &fbdo, <string> path, <string> value);

bool setStringAsync(FirebaseData &fbdo, <string> path, <string> value);
```

<br/>

#### Set string data and virtual child ".priority" at the defined database path.

```cpp
bool setString(FirebaseData &fbdo, <string> path, <string> value, float priority);

bool setStringAsync(FirebaseData &fbdo, <string> path, <string> value, float priority);
```

<br/>

#### Set string (text) at the defined database path if defined database path's ETag matched the ETag value

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which string data will be set.

param **`value`** String or text to set.

param **`ETag`** Known unique identifier string (ETag) of defined database path.

return **`Boolean`** type status indicates the success of the operation.

<br/>

Call \<FirebaseData\>.dataType to determine what type of data successfully stores in the database. 

Call \<FirebaseData\>.to<String>() will return the string value of
the payload returned from the server.

If ETag at the defined database path does not match the provided ETag parameter, 
the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).

If the operation failed due to ETag is not match, call \<FirebaseData\>.ETag() to get the current ETag value.
Also, call \<FirebaseData\>.to<String>() to get the current string value.

```cpp
bool setString(FirebaseData &fbdo, <string> path, <string> value, <string> ETag);

bool setStringAsync(FirebaseData &fbdo, <string> path, <string> value, <string> ETag);
```

<br/>

#### Set string data and the virtual child ".priority" if defined ETag matches at the defined database path 

```cpp
bool setString(FirebaseData &fbdo, <string> path, <string> value, float priority, <string> ETag);

bool setStringAsync(FirebaseData &fbdo, <string> path, <string> value, float priority, <string> ETag);
```

<br/>

#### Set child nodes key and value (using FirebaseJson object) to the defined database path

This will replace any child nodes inside the defined path with node' s key
and value defined in FirebaseJson object.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which key and value in FirebaseJson object will be replaced or set.

param **`json`** The FirebaseJson object.

return **`Boolean`** type status indicates the success of the operation.

<br/>

Call \<FirebaseData\>.dataType or \<FirebaseData\>.dataTypeNum to determine what type of data successfully stores in the database. 

Call \<FirebaseData\>.to<FirebaseJson>() and \<FirebaseData\>.to<FirebaseJson *>() will return reference to object and pointer to FirebaseJson object from payload.

```cpp
bool setJSON(FirebaseData &fbdo, <string> path, FirebaseJson &json);

bool setJSONAsync(FirebaseData &fbdo, <string> path, FirebaseJson &json);
```

<br/>

#### Set JSON data or FirebaseJson object and virtual child ".priority" at the defined database path.

```cpp
bool setJSON(FirebaseData &fbdo, <string> path, FirebaseJson &json, float priority);

bool setJSONAsync(FirebaseData &fbdo, <string> path, FirebaseJson &json, float priority);
```

<br/>

#### Set child nodes key and value (using JSON data or FirebaseJson object) to the defined database path if defined database path's ETag matched the ETag value

This will replace any child nodes inside the defined path with node' s key
and value defined in JSON data or FirebaseJson object.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which key and value in JSON data or FirebaseJson object will be replaced or set.

param **`jsonString`** The JSON string to set (should be valid JSON data).

param **`json`** The FirebaseJson object.

param **`ETag`** Known unique identifier string (ETag) of defined database path.

return **`Boolean`** type status indicates the success of the operation.

<br/>

Call \<FirebaseData\>.dataType to determine what type of data successfully stores in the database. 


If ETag at the defined database path does not match the provided ETag parameter,
the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).

If the operation failed due to ETag is not match, call \<FirebaseData\>.ETag() to get the current ETag value.
Also call \<FirebaseData\>.jsonData to get the current JSON string value \<FirebaseData\>.to<FirebaseJson>() 
to get the FirebaseJson object.

Also call \<FirebaseData\>.jsonData will return the JSON string value of
the payload returned from the server.

And \<FirebaseData\>.to<FirebaseJson>() and \<FirebaseData\>.to<FirebaseJson *>() will return reference to object and pointer to FirebaseJson object from payload.


```cpp
bool setJSON(FirebaseData &fbdo, <string> path, FirebaseJson &json, <string> ETag);

bool setJSONAsync(FirebaseData &fbdo, <string> path, FirebaseJson &json, <string> ETag);
```

<br/>

#### Set JSON data or FirebaseJson object and the virtual child ".priority" if defined ETag matches at the defined database path 

```cpp
bool setJSON(FirebaseData &fbdo, <string> path, FirebaseJson &json, float priority, <string> ETag);

bool setJSONAsync(FirebaseData &fbdo, <string> path, FirebaseJson &json, float priority, <string> ETag);
```

<br/>

#### Set child nodes's array (using FirebaseJsonArray object) to the defined database path.

This will replace any child nodes inside the defined path with array defined in FirebaseJsonArray object.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which key and value in FirebaseJsonArray object will be replaced or set.

param **`arr`** The FirebaseJsonArray object.

return - **`Boolean`** type status indicates the success of the operation.

Call \<FirebaseData\>.dataType or \<FirebaseData\>.dataTypeNum to determine what type of data that successfully
stores in the database.

Call \<FirebaseData\>.to<FirebaseJsonArray>() and \<FirebaseData\>.to<FirebaseJsonArray*>() will return reference to object and pointer to FirebaseJsonArray object that contains the array from payload. 

```cpp
bool setArray(FirebaseData &fbdo, <string> path, FirebaseJsonArray &arr);

bool setArrayAsync(FirebaseData &fbdo, <string> path, FirebaseJsonArray &arr);
```

<br/>

#### Set FirebaseJsonArray object and virtual child ".priority" at the defined database path.

```cpp
bool setArray(FirebaseData &fbdo, <string> path, FirebaseJsonArray &arr, float priority);

bool setArrayAsync(FirebaseData &fbdo, <string> path, FirebaseJsonArray &arr, float priority);
```

<br/>

#### Set array (using JSON data or FirebaseJson object) to the defined database path if defined database path's ETag matched the ETag value.

This will replace any child nodes inside the defined path with array defined in FirebaseJsonArray object.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which key and value in JSON data will be replaced or set.

param **`arr`** The FirebaseJsonArray object.

param **`ETag`** Known unique identifier string (ETag) of defined database path.

return - **`Boolean`** type status indicates the success of the operation.

<br/>

If ETag at the defined database path does not match the provided ETag parameter,
the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).

If the operation failed due to ETag is not match, call \<FirebaseData\>.ETag() to get the current ETag value.

Also call \<FirebaseData\>.dataType to determine what type of data that successfully
stores in the database.

And \<FirebaseData\>.to<FirebaseJsonArray>() and \<FirebaseData\>.to<FirebaseJsonArray*>() will return reference to object and pointer to FirebaseJsonArray object that contains the array from payload. 

```cpp
bool setArray(FirebaseData &fbdo, <string> path, FirebaseJsonArray &arr, <string> ETag);

bool setArray(AsyncFirebaseData &fbdo, <string> path, FirebaseJsonArray &arr, <string> ETag);
```

<br/>

Set FirebaseJsonArray object and the virtual child ".priority" if defined ETag matches at the defined database path 

```cpp
bool setArray(FirebaseData &fbdo, <string> path, FirebaseJsonArray &arr, float priority, <string> ETag);

bool setArrayAsync(FirebaseData &fbdo, <string> path, FirebaseJsonArray &arr, float priority, <string> ETag);
```

<br/>

#### Set blob (binary data) at the defined database path

This will replace any child nodes inside the defined path with a blob of binary data.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which binary data will be set.

param **`blob`** Byte array of data.

param **`size`** Size of the byte array.

return **`Boolean`** type status indicates the success of the operation.

No payload returned from the server.

```cpp
bool setBlob(FirebaseData &fbdo, <string> path, uint8_t *blob, size_t size);

bool setBlobAsync(FirebaseData &fbdo, <string> path, uint8_t *blob, size_t size);
```

<br/>

#### Set blob (binary data) at the defined database path if defined database path's ETag matched the ETag value

This will replace any child nodes inside the defined path with a blob of binary data.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path in which binary data will be set.

param **`blob`** Byte array of data.

param **`size`** Size of the byte array.

param **`ETag`** Known unique identifier string (ETag) of defined database path.

return **`Boolean`** type status indicates the success of the operation.

<br/>

No payload returned from the server.

If ETag at the defined database path does not match the provided ETag parameter,
the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).

```cpp
bool setBlob(FirebaseData &fbdo, <string> path, uint8_t *blob, size_t size, <string> ETag);

bool setBlobAsync(FirebaseData &fbdo, <string> path, uint8_t *blob, size_t size, <string> ETag);
```

<br/>

#### Set binary data from the file store on SD card/Flash memory to the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`storageType`** Type of storage to read file data, StorageType::FLASH or StorageType::SD.

param **`path`** Target database path in which binary data from the file will be set.

param **`fileName`** File name included its path in SD card/Flash memory.

return **`Boolean`** type status indicates the success of the operation.

<br/>

No payload returned from the server.

The file systems for flash and sd memory can be changed in FirebaseFS.h.

```cpp
bool setFile(FirebaseData &fbdo, uint8_t storageType, <string> path, <string> fileName);

bool setFileAsync(FirebaseData &fbdo, uint8_t storageType, <string> path, <string> fileName);
```

<br/>

#### Set binary data from a file stored on SD card/Flash memory to the defined database path if defined database path's ETag matched the ETag value

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`storageType`** Type of storage to read file data, StorageType::FLASH or StorageType::SD.

param **`path`** Target database path in which binary data from the file will be set.

param **`fileName`** File name included its path in SD card/Flash memory.

param **`ETag`** Known unique identifier string (ETag) of defined database path.

return **`Boolean`** type status indicates the success of the operation.

<br/>

No payload returned from the server.

If ETag at the defined database path does not match the provided ETag parameter,
the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).

The file systems for flash and sd memory can be changed in FirebaseFS.h.

```cpp
bool setFile(FirebaseData &fbdo, uint8_t storageType, <string> path, <string> fileName, <string> ETag);

bool setFileAsync(FirebaseData &fbdo, uint8_t storageType, <string> path, <string> fileName, <string> ETag);
```

<br/>

#### Set Firebase server's timestamp to the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which timestamp will be set.

return - **`Boolean`** type status indicates the success of the operation.

<br/>

Call \<FirebaseData\>.to<int>() will return the integer value of timestamp in seconds
or \<FirebaseData\>.to<double>() to get millisecond timestamp.

Due to bugs in Serial.print in Arduino, to print large double value with zero decimal place, 
use printf("%.0lf\n", firebaseData.to<double>());.

```cpp
bool setTimestamp(FirebaseData &fbdo, <string> path);

bool setTimestampAsync(FirebaseData &fbdo, <string> path);
```

<br/>

#### Update child node key or existing key's value (using FirebaseJson object) under the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which key and value in FirebaseJson object will be updated.

param **`json`** The FirebaseJson object used for update.

return **`Boolean`** type status indicates the success of the operation.

<br/>

Call \<FirebaseData\>.dataType or \<FirebaseData\>.dataTypeNum to determine what type of data successfully stores in the database. 

Call \<FirebaseData\>.to<FirebaseJson>() and \<FirebaseData\>.to<FirebaseJson *>() will return reference to object and pointer to FirebaseJson object from payload.

To reduce network data usage, use updateNodeSilent instead.

```cpp
bool updateNode(FirebaseData &fbdo, <string> path, FirebaseJson &json);

bool updateNodeAsync(FirebaseData &fbdo, <string> path, FirebaseJson &json);
```

<br/>

#### Update child node key or existing key's value and virtual child ".priority" (using JSON data or FirebaseJson object) under the defined database path.

```cpp
bool updateNode(FirebaseData &fbdo, <string> path, FirebaseJson &json, float priority);

bool updateNodeAsync(FirebaseData &fbdo, <string> path, FirebaseJson &json, float priority);
```

<br/>

#### Update child node key or existing key's value (using FirebaseJson object) under the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Target database path which key and value in FirebaseJson object will be updated.

param **`json`** The FirebaseJson object used for update.

return **`Boolean`** type status indicates the success of the operation.

Owing to the objective of this function to reduce network data usage, 
no payload will be returned from the server.

```cpp
bool updateNodeSilent(FirebaseData &fbdo, <string> path, FirebaseJson &json);

bool updateNodeSilentAsync(FirebaseData &fbdo, <string> path, FirebaseJson &json);
```

<br/>

#### Update child node key and virtual child ".priority" (using JSON data or FirebaseJson object) under the defined database path.

```cpp
bool updateNodeSilent(FirebaseData &fbdo, <string> path, FirebaseJson &json, float priority);

bool updateNodeSilentAsync(FirebaseData &fbdo, <string> path, FirebaseJson &json, float priority);
```

<br/>

#### Read generic type of value at the defined node.

param **`fbdo`** The pointer to Firebase Data Object.

param **`path`** The path to the node.

return - **`Boolean`** value, indicates the success of the operation.

<br/>

Call \<FirebaseData\>.dataType or \<FirebaseData\>.dataTypeNum to determine what type of data successfully stores in the database. 

Call \<FirebaseData\>.to<int>(), \<FirebaseData\>.to<float>, \<FirebaseData\>.to<double>, 
\<FirebaseData\>.to<bool>, \<FirebaseData\>.to<String>, \<FirebaseData\>.to<FirebaseJson>(),
\<FirebaseData\>.to<FirebaseJson*>(), \<FirebaseData\>.to<FirebaseJsonArray>(),
\<FirebaseData\>.to<FirebaseJsonArray*>(), \<FirebaseData\>.to<std::vector<uint8_t> *> and \<FirebaseData\>.to<File>() corresponded to its type that get from \<FirebaseData\>.dataType.

```cpp
bool get(FirebaseData &fbdo, <string> path);
```

<br/>

#### Read the integer value at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the integer value is being read.

return **`Boolean`** type status indicates the success of the operation.

<br/>

Call \<FirebaseData\>.dataType to determine what type of data successfully stores in the database. 

Call \<FirebaseData\>.to<int>() will return the integer value of
payload returned from the server.

If the payload returned from server is not integer or float type, 
the function \<FirebaseData\>.to<int>() will return zero (0).

If the payload returned from server is float type, 
the function \<FirebaseData\>.to<int>() will return rounded integer value.

```cpp
bool getInt(FirebaseData &fbdo, <string> path);
```

<br/>

#### Read the integer value at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the integer value is being read.

param **`target`** The integer type variable to store value.

return **`Boolean`** type status indicates the success of the operation.

If the type of payload returned from the server is not an integer, float and double, 
the target variable's value will be zero (0).

```cpp
bool getInt(FirebaseData &fbdo, <string> path,<integer> &target);
```

<br/>

#### Read the float value at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the float value is being read.

return **`Boolean`** type status indicates the success of the operation.

Call \<FirebaseData\>.dataType to determine what type of data successfully stores in the database. 

Call \<FirebaseData\>.to<float>() will return the float value of
the payload returned from the server.

If the payload returned from server is not integer or float type, 
the function \<FirebaseData\>.to<int>() will return zero (0).

```cpp
bool getFloat(FirebaseData &fbdo, <string> path);
```

<br/>

#### Read the float value at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the float value is being read.

param **`target`** The float type variable to store value.

return **`Boolean`** type status indicates the success of the operation.

If the type of payload returned from the server is not an integer, float and double, 
the target variable's value will be zero (0).

```cpp
bool getFloat(FirebaseData &fbdo, <string> path, float &target);
```

<br/>

#### Read the double value at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the float value is being rea.

return **`Boolean`** type status indicates the success of the operation.

<br/>

Call \<FirebaseData\>.dataType to determine what type of data successfully stores in the database. 
    
Call \<FirebaseData\>.to<double>() will return the double value of
the payload returned from the server.

If the payload returned from server is not integer, float and double, 
the function \<FirebaseData\>.to<double>() will return zero (0).

Due to bugs in Serial.print in Arduino, to print large double value with zero decimal place, 
use printf("%.9lf\n", firebaseData.to<double>()); for print the returned double value up to 9 decimal places.

```cpp
bool getDouble(FirebaseData &fbdo, <string> path);
```

<br/>

#### Read the float value at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the float value is being read.

param **`target`** The double type variable to store value.

return **`Boolean`** type status indicates the success of the operation.

If the type of payload returned from the server is not an integer, float and double, 
the target variable's value will be zero (0).

```cpp
bool getDouble(FirebaseData &fbdo, <string> path, double &target);
```

<br/>

#### Read the Boolean value at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the Boolean value is being read.

return **`Boolean`** type status indicates the success of the operation.

<br/>

Call \<FirebaseData\>.dataType to determine what type of data successfully stores in the database. 

Call \<FirebaseData\>.to<bool>() will return the Boolean value of
the payload returned from the server.

If the payload returned from server is not boolean type, 
the function \<FirebaseData\>.to<bool>() will return false.

```cpp
bool getBool(FirebaseData &fbdo, <string> path);
```

<br/>

#### Read the Boolean value at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the Boolean value is being read.

param **`target`** The boolean type variable to store value.

return **`Boolean`** type status indicates the success of the operation.

If the type of payload returned from the server is not Boolean, 
the target variable's value will be false.

```cpp
bool getBool(FirebaseData &fbdo, <string> path, bool &target);
```

<br/>

#### Read the string of text at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the string value is being read.

return **`Boolean`** type status indicates the success of the operation.

<br/>

Call \<FirebaseData\>.dataType to determine what type of data successfully stores in the database. 

Call \<FirebaseData\>.to<String>() will return the string value of
the payload returned from the server.

If the payload returned from the server is not string type, 
the function \<FirebaseData\>.to<String>() will return empty string (String object).

```cpp
bool getString(FirebaseData &fbdo, <string> path);
```

<br/>

#### Read the string of text at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the string value is being read.

param **`target`** The String object to store value.

return **`Boolean`** type status indicates the success of the operation.

If the type of payload returned from the server is not a string,
the target String object's value will be empty.

```cpp
bool getString(FirebaseData &fbdo, <string> path, String &target);
```

<br/>

#### Read the JSON string at the defined database path

The returned payload JSON string represents the child nodes and their value.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the JSON string value is being read.

return **`Boolean`** type status indicates the success of the operation.

<br/>

Call \<FirebaseData\>.dataType or \<FirebaseData\>.dataTypeNum to determine what type of data successfully stores in the database. 

Call \<FirebaseData\>.to<FirebaseJson>() and \<FirebaseData\>.to<FirebaseJson *>() will return reference to object and pointer to FirebaseJson object from payload.

If the type of payload returned from server is not json,
the function \<FirebaseData\>.to<FirebaseJson>() will contain empty object.


```cpp
bool getJSON(FirebaseData &fbdo, <string> path);
```

<br/>

#### Read the JSON string at the defined database path

The returned payload JSON string represents the child nodes and their value.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the JSON string value is being read.

param **`target`** The FirebaseJson object pointer to get json data

return **`Boolean`** type status indicates the success of the operation.

If the type of payload returned from the server is not JSON,
the target FirebaseJson object will contain an empty object.

```cpp
bool getJSON(FirebaseData &fbdo, <string> path, FirebaseJson *target);
```

<br/>

#### Read the JSON string with data filtering at the defined database path

The returned payload JSON string represents the child nodes and their value.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the JSON string value is being read.

param **`query`** QueryFilter class to set query parameters to filter data.

return **`Boolean`** type status indicates the success of the operation.

<br/>

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


Call \<FirebaseData\>.dataType or \<FirebaseData\>.dataTypeNum to determine what type of data successfully stores in the database. 

Call \<FirebaseData\>.to<FirebaseJson>() and \<FirebaseData\>.to<FirebaseJson *>() will return reference to object and pointer to FirebaseJson object from payload.

If the type of payload returned from server is not json,
the function \<FirebaseData\>.to<FirebaseJson>() will contain empty object.

```cpp
bool getJSON(FirebaseData &fbdo, <string> path, QueryFilter &quer);
```

<br/>

#### Read the JSON string at the defined database path as above

The returned payload JSON string represents the child nodes and their value.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the JSON string value is being read.

param **`target`** The FirebaseJson object pointer to get json data.

return **`Boolean`** type status indicates the success of the operation.

If the type of payload returned from the server is not JSON,
the target FirebaseJson object will contain an empty object.

```cpp
bool getJSON(FirebaseData &fbdo, <string> path, QueryFilter &query, FirebaseJson *target);
```

<br/>

#### Read the array data at the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the array is being read.

return - **`Boolean`** type status indicates the success of the operation.

<br/>

Call \<FirebaseData\>.dataType or \<FirebaseData\>.dataTypeNum to determine what type of data successfully stores in the database.

Call \<FirebaseData\>.to<FirebaseJsonArray>() and \<FirebaseData\>.to<FirebaseJsonArray*>() will return reference to object and pointer to FirebaseJsonArray object that contains the array from payload. 

If the type of payload returned from the server is not an array,
the array element in \<FirebaseData\>.to<FirebaseJsonArray>() will be empty.

```cpp
bool getArray(FirebaseData &fbdo, <string> path);
```

<br/>

#### Read the array data at the defined database path and assign data to the target.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the array is being read.

param **`target - The FirebaseJsonArray object pointer to get array value.

return - **`Boolean`** type status indicates the success of the operation.

If the type of payload returned from the server is not an array,
the target FirebaseJsonArray object will contain an empty array.

```cpp
bool getArray(FirebaseData &fbdo, <string> path, FirebaseJsonArray *target);
```

<br/>

#### Read the array data at the defined database path.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the array is being read.

param **`query`** QueryFilter class to set query parameters to filter data.

return - **`Boolean`** type status indicates the success of the operation.

<br/>

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

<br/>

Call \<FirebaseData\>.dataType or \<FirebaseData\>.dataTypeNum to determine what type of data that successfully
stores in the database.

Call \<FirebaseData\>.to<FirebaseJsonArray>() and \<FirebaseData\>.to<FirebaseJsonArray*>() will return reference to object and pointer to FirebaseJsonArray object that contains the array from payload. 

If the type of payload returned from the server is not an array,
the function \<FirebaseData\>.to<FirebaseJsonArray>() will contain empty array.

```cpp
bool getArray(FirebaseData &fbdo, <string> path, QueryFilter &query);
```

<br/>

#### Read the array data at the defined database path as above

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the array is being read.

param **`target`** The FirebaseJsonArray object to get array value.

return - **`Boolean`** type status indicates the success of the operation.

If the type of payload returned from the server is not an array,
the target FirebaseJsonArray object will contain an empty array.

```cpp
bool getArray(FirebaseData &fbdo, <string> path, QueryFilter &query, FirebaseJsonArray *target);
```

<br/>

#### Read the blob (binary data) at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the binary data is being read.

return **`Boolean`** type status indicates the success of the operation.

<br/>

Call \<FirebaseData\>.dataType() to determine what type of data successfully stores in the database. 
    
Call \<FirebaseData\>.to<std::vector<uint8_t> *>() will return the dynamic array of unsigned 8-bit data of
payload returned from server.

If the payload returned from the server is not blob type, 
the function \<FirebaseData\>.to<std::vector<uint8_t> *>() will return empty array.

```cpp
bool getBlob(FirebaseData &fbdo, <string> path);
```

<br/>

#### Read the blob (binary data) at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path in which the binary data is being read.

param **`target`** Dynamic array of unsigned 8-bit data (i.e. std::vector<uint8_t>) to store value.

return **`Boolean`** type status indicates the success of the operation.

If the type of payload returned from the server is not a blob,
the target variable value will be an empty array.

```cpp
bool getBlob(FirebaseData &fbdo, <string> path, std::vector<uint8_t> &target);
```

<br/>

#### Download file data in a database at the defined database path and save to SD card/Flash memory.

The downloaded data will be decoded to binary and save to SD card/Flash memory, then

please make sure that data at the defined database path is the file type.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`storageType`** Type of storage to write file data, StorageType::FLASH or StorageType::SD.

param **`nodePath`** Database path that file data will be downloaded.

param **`fileName`** File name included its path in SD card/Flash memory.

return **`Boolean`** type status indicates the success of the operation.

The file systems for flash and sd memory can be changed in FirebaseFS.h.

```cpp
bool getFile(FirebaseData &fbdo, uint8_t storageType, <string> nodePath, <string> fileName);
```

<br/>

#### Delete all child nodes at the defined database path

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path to be deleted.

return **`Boolean`** type status indicates the success of the operation.*

```cpp
bool deleteNode(FirebaseData &fbdo, <string> path);
```

<br/>

#### Delete all child nodes at the defined database path if defined database path's ETag matched the ETag value

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path to be deleted.

param **`ETag`** Known unique identifier string (ETag) of defined database path.

return **`Boolean`** type status indicates the success of the operation.*

If ETag at the defined database path does not match the provided ETag parameter,
the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).

```cpp
bool deleteNode(FirebaseData &fbdo, <string> path, <string> ETag);
```

<br/>

#### Delete nodes that its timestamp node exceeded the data retaining period.

param **`fbdo`** The pointer to Firebase Data Object.

param **`path`** The parent path of children nodes that being delete.

param **`timestampNode`** The sub-child node that keep the timestamp. 

param **`limit`** The maximum number of children nodes to delete at once, 30 is maximum.

param **`dataRetentionPeriod`**The period in seconds of data in the past which will be retained.

return **`Boolean`** value, indicates the success of the operation.*

note: The databaseSecret can be empty if the auth type is OAuth2.0 or legacy and required if auth type is Email/Password sign-in.

```cpp
 bool deleteNodesByTimestamp(FirebaseData &fbdo, <string> path, <string> timestampNode, size_t limit, unsigned long dataRetentionPeriod);
```

<br/>

#### Start monitoring the value changes at the defined path and its children

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`path`** Database path to monitor.

return **`Boolean`** type status indicates the success of the operation.*

```cpp
bool beginStream(FirebaseData &fbdo, <string> path);
```

<br/>

#### Start subscribe to the value changes at the defined parent node path with multiple nodes paths parsing.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`parentPath`** Database parent node path to subscribe.

return **`Boolean`** type status indicates the success of the operation.*

```cpp
bool beginMultiPathStream(FirebaseData &fbdo, <string> parentPath,);
```
<br/>

#### Read the stream event data at the defined database path

Once beginStream was called e.g. in setup(), the readStream function
should call inside the loop function.

param **`fbdo`** Firebase Data Object to hold data and instances.

return **`Boolean`** type status indicates the success of the operation.

<br/>

Using the same Firebase Data object for stream read/monitoring associated with read, store, update, delete will break or quit the current stream connection. 
    
The stream will be resumed or reconnected automatically when calling readStream.

```cpp
bool readStream(FirebaseData &fbdo);
```

<br/>

#### End the stream connection at the defined path

It can be restart again by calling beginStream.

param **`fbdo`** Firebase Data Object to hold data and instances.

return **`Boolean`** type status indicates the success of the operation.
 
```cpp
bool endStream(FirebaseData &fbdo);
```

<br/>

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

```cpp
void setStreamCallback(FirebaseData &fbdo, StreamEventCallback dataAvailablecallback, StreamTimeoutCallback timeoutCallback = NULL, size_t streamTaskStackSize = 8192);
```
<br/>

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

```cpp
void setMultiPathStreamCallback(FirebaseData &fbdo, MultiPathStreamEventCallback multiPathDataCallback, StreamTimeoutCallback timeoutCallback = NULL, size_t streamTaskStackSize = 8192);
```

<br/>

#### Remove stream callback functions

param **`fbdo`** Firebase Data Object to hold data and instances.

```cpp
void removeStreamCallback(FirebaseData &fbdo);
```

<br/>

#### Remove multiple paths stream callback functions.

param **`fbdo`** Firebase Data Object to hold data and instances.

```cpp
void removeMultiPathStreamCallback(FirebaseData &fbdo);
```

<br/>

#### Backup (download) database at defined database path to SD card/Flash memory

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`storageType`** Type of storage to save file, StorageType::FLASH or StorageType::SD.

param **`nodePath`** Database path to be back up.

param **`fileName`** File name to save.

return **`Boolean`** type status indicates the success of the operation.

The file systems for flash and sd memory can be changed in FirebaseFS.h.

```cpp
bool backup(FirebaseData &fbdo, uint8_t storageType, <string> nodePath, <string> fileName);
```

<br/>

#### Restore database at defined path usin backup file saved on SD card/Flash memory

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`storageType`** Type of storage to read file, StorageType::FLASH or StorageType::SD.

param **`nodePath`** Database path to be restored.

param **`fileName`** File name to read.

return **`Boolean`** type status indicates the success of the operation.

The file systems for flash and sd memory can be changed in FirebaseFS.h.

```cpp
bool restore(FirebaseData &fbdo, uint8_t storageType <string> nodePath, <string> fileName);
```

<br/>

#### Set maximum Firebase's read/store retry operation (0 - 255) in case of network problems and buffer overflow

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`num`** The maximum retry.

```cpp
void setMaxRetry(FirebaseData &fbdo, uint8_t num);
```

<br/>

#### Set the maximum Firebase Error Queues in collection (0 - 255)

Firebase read/store operation causes by network problems and buffer overflow will be added to Firebase Error Queues collection.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`num`** The maximum Firebase Error Queues.

```cpp
void setMaxErrorQueue(FirebaseData &fbdo, uint8_t num);
```

<br/>

#### Save Firebase Error Queues as FLASH file (save only database store queues)

Firebase read (get) operation will not be saved.

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`filename`** File name to be saved.

param **`storageType`** Type of storage to save file, StorageType::FLASH or StorageType::SD.

The file systems for flash and sd memory can be changed in FirebaseFS.h.
    
```cpp
bool saveErrorQueue(FirebaseData &fbdo, <string> filename, uint8_t storageType);
```

<br/>

#### Delete file in Flash (FLASH) or SD card

param **`filename`** File name to delete.

param **`storageType`** Type of storage to save file, StorageType::FLASH or StorageType::SD.

The file systems for flash and sd memory can be changed in FirebaseFS.h.
    
```cpp
bool deleteStorageFile(<string> filename, uint8_t storageType);
```

<br/>

#### Restore Firebase Error Queues from FLASH file

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`filename`** Filename to be read and restore queues.

param **`storageType`** Type of storage to read file, StorageType::FLASH or StorageType::SD.

The file systems for flash and sd memory can be changed in FirebaseFS.h.
    
```cpp
bool restoreErrorQueue(FirebaseData &fbdo, <string> filename, uint8_t storageType);
```

<br/>

#### Get the number of Firebase Error Queues stored in defined FLASH file

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`filename`** Filename to be read and count for queues.

param **`storageType`** Type of storage to read file, StorageType::FLASH or StorageType::SD.

return **`Number`** (0-255) of queues store in defined FLASH file.

The file systems for flash and sd memory can be changed in FirebaseFS.h.

```cpp
uint8_t errorQueueCount(FirebaseData &fbdo, <string> filename, uint8_t storageType);
```

<br/>

#### Get number of queues in Firebase Data object Firebase Error Queues collection

param **`fbdo`** Firebase Data Object to hold data and instances.

return **`Number`** (0-255) of queues in Firebase Data object queue collection.

```cpp
uint8_t errorQueueCount(FirebaseData &fbdo);
```

<br/>

#### Get whether the Firebase Error Queues collection was full or not

param **`fbdo`** Firebase Data Object to hold data and instances.

return **`Boolean`** type status indicates whether the  Firebase Error Queues collection was full or not.

```cpp
bool isErrorQueueFull(FirebaseData &fbdo);
```

<br/>

#### Process all failed Firebase operation queue items when network is available

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`callback`** Callback function that accepts QueueInfo parameter.
  
```cpp
void processErrorQueue(FirebaseData &fbdo, QueueInfoCallback callback = NULL);
```

<br/>

#### Return Firebase Error Queue ID of last Firebase Error

Return 0 if there is no Firebase Error from the last operation.

param **`fbdo`** Firebase Data Object to hold data and instances.
    
return **`Number`** of Queue ID.

```cpp
uint32_t getErrorQueueID(FirebaseData &fbdo);
```

<br/>

#### Get whether Firebase Error Queue currently exists is Error Queue collection or not

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`errorQueueID`** The Firebase Error Queue ID get from getErrorQueueID.
    
return - **`Boolean type`** status indicates the queue existence.

```cpp
bool isErrorQueueExisted(FirebaseData &fbdo, uint32_t errorQueueID);
```

<br/>

#### Start the Firbase Error Queues Auto Run Process

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`callback`** Callback function that accepts QueueInfo Object as parameter, optional.

param **`queueTaskStackSize`** The queue error recovery task (RTOS task) reserved stack memory in byte (optional) (8192 is default).

<br/>

The following functions are available from QueueInfo Object accepted by callback.

**queueInfo.totalQueues()**, get the total Error Queues in Error Queue Collection.

**queueInfo.currentQueueID()**, get current Error Queue ID that being process.

**queueInfo.isQueueFull()**, determine whether Error Queue Collection is full or not.

**queueInfo.dataType()**, get a string of the Firebase call data type that being process of current Error Queue.

**queueInfo.method()**, get a string of the Firebase call method that being process of current Error Queue.

**queueInfo.path()**, get a string of the Firebase call path that being process of current Error Queue.


```cpp
void beginAutoRunErrorQueue(FirebaseData &fbdo, QueueInfoCallback callback = NULL, size_t queueTaskStackSize = 8192);
```

<br/>

#### Stop the Firebase Error Queues Auto Run Process

param **`fbdo`** Firebase Data Object to hold data and instances.


```cpp
void endAutoRunErrorQueue(FirebaseData &fbdo);
```

<br/>

#### Clear all Firbase Error Queues in Error Queue collection

param **`fbdo`** Firebase Data Object to hold data and instances.


```cpp
void clearErrorQueue(FirebaseData &fbdo);
```

<br/>

#### Send Firebase Cloud Messaging to device with first registeration token which added by firebaseData.fcm.addDeviceToken

param **`fbdo`** Firebase Data Object to hold data and instances.

param **`index`** The index (starts from 0) of recipient device token which added by firebaseData.fcm.addDeviceToken
    
return - **`Boolean type`** status indicates the success of the operation.

```cpp
bool sendMessage(FirebaseData &fbdo, uint16_t index);
```

<br/>

#### Send Firebase Cloud Messaging to all devices (multicast) which added by firebaseData.fcm.addDeviceToken

param **`fbdo`** Firebase Data Object to hold data and instances.
    
return - **`Boolean type`** status indicates the success of the operation.

```cpp
bool broadcastMessage(FirebaseData &fbdo);
```

<br/>

#### Send Firebase Cloud Messaging to devices that subscribed to a topic

param **`fbdo`** Firebase Data Object to hold data and instances.
    
return - **`Boolean type`** status indicates the success of the operation.

```cpp
bool sendTopic(FirebaseData &fbdo);
```
<br/>

#### SD card config with GPIO pins

param **`ss`** SPI Chip/Slave Select pin.

param **`sck`** SPI Clock pin.

param **`miso`** SPI MISO pin.

param **`mosi`** SPI MOSI pin.

return **`Boolean`** type status indicates the success of the operation.


```cpp
void sdBegin(int8_t ss = -1, int8_t sck = -1, int8_t miso = -1, int8_t mosi = -1);
```
<br/>

#### Initialize the SD_MMC card.

param **`mountpoint`** The mounting point.

param **`mode1bit`** Allow 1 bit data line (SPI mode).

param **`format_if_mount_failed`** Format SD_MMC card if mount failed.

return **`Boolean`** type status indicates the success of the operation.

```cpp
bool sdMMCBegin(<string> mountpoint = "/sdcard", bool mode1bit = false, bool format_if_mount_failed = false);
```
<br/>

#### Set system time with timestamp.

param **`ts`** timestamp in seconds from midnight Jan 1, 1970.

return **`Boolean`** type status indicates the success of the operation.

This function allows the internal time setting by timestamp i.e. timestamp from external RTC. 

```cpp
bool setSystemTime(time_t ts);
```

<br/>

#### Provide the http code error string

param **`httpCode`** The http code.

param **`buff`** The C++ string buffer out.

```cpp
void errorToString(int httpCode, std::string &buff);
```

<br/><br/>

## FirebaseData Object Functions

<br/>

#### Set the HTTP response size limit.

param **`len`** The server response buffer size limit (4096 is minimum). 

```cpp
void setResponseSize(size_t len);
```

<br/>

#### Set the Root certificate for a FirebaseData object

param **`ca`** PEM format certificate string.

```cpp
void setCert(const char* ca);
```

<br/>

#### Pause/Unpause WiFiClient from all Firebase operations

param **`pause`** True for pause and False for unpause.

return **`Boolean`** type status indicates the success of the operation.

```cpp
bool pauseFirebase(bool pause);
```

<br/>

#### Check the pause status of FirebaseData object.

return **`Boolean`** type value of pause status.

```cpp
bool isPause();
```

<br/>

#### Get Internal WiFiClientSecure.

return **`Internal WiFiClientSecure pointer`**.

```cpp
WiFiClientSecure *getWiFiClient();
```

<br/>

#### Close the keep-alive connection of the internal WiFi client.

This will release the memory used by internal WiFi client.

The next server connection will take time (a second) due to SSL/TLS handshaking

```cpp
void stopWiFiClient();
```

<br/>

#### Get the data type of payload returned from the server (RTDB only).

return **`The one of these data type e.g. string, boolean, int, float, double, json, array, blob, file and null.`**

```cpp
String dataType();
```

<br/>

#### Get the data type of payload returned from the server (RTDB only).

return **`The enumeration value of fb_esp_rtdb_data_type.`**

<br/>

fb_esp_rtdb_data_type_null or 1,

fb_esp_rtdb_data_type_integer or 2,

fb_esp_rtdb_data_type_float or 3,

fb_esp_rtdb_data_type_double or 4,

fb_esp_rtdb_data_type_boolean or 5,

fb_esp_rtdb_data_type_string or 6,

fb_esp_rtdb_data_type_json or 7,

fb_esp_rtdb_data_type_array or 8,

fb_esp_rtdb_data_type_blob or 9,

fb_esp_rtdb_data_type_file or 10

```cpp
uint8_t dataTypeEnum();
```

<br/>

#### Get the event type of stream (RTDB only).

return **`The one of these event type String e.g. put, patch, cancel, and auth_revoked.`**

<br/>

The event type "put" indicated that data at the event path relative to the stream path was completely changed. 

The event path can be determined by dataPath().

The event type "patch" indicated that data at the event path relative to stream path was updated. 

The event path can be determined by dataPath().

The event type "cancel" indicated something wrong and cancel by the server.

The event type "auth_revoked" indicated the provided Firebase Authentication Data (Database secret) is no longer valid.

```cpp
String eventType();
```

<br/>

#### Get the unique identifier (ETag) of RTDB data (RTDB only).

return **`String.`** of unique identifier.

```cpp
String ETag();
```

<br/>

#### Get the current stream path (RTDB only).

return **`The database streaming path.`**

```cpp
String streamPath();
```

<br/>

#### Get the current data path (RTDB only).

return **`The database path which belongs to server' s returned payload.`**

The database path returned from this function in case of stream, also changed upon the child or parent's stream
value changes.

```cpp
String dataPath();
```

<br/>

#### Get the error reason String from the process.

return **`The error description string (String object).`**

```cpp
String errorReason();
```

<br/>

#### Return the integer data of server returned payload (RTDB only).

return **`Integer value.`**

```cpp
int intData();
```

<br/>

#### Return the float data of server returned payload (RTDB only).

return **`Float value.`**

```cpp
float floatData();
```

<br/>

#### Return the double data of server returned payload (RTDB only).

return **`Double value.`**

```cpp
float doubleData();
```

<br/>

#### Return the Boolean data of server returned payload (RTDB only).

return **`Boolean value.`**

```cpp
float boolData();
```

<br/>

#### Return the String data of server returned payload (RTDB only).

return **`String (String object).`**

```cpp
String stringData();
```

<br/>

#### Return the JSON String data of server returned payload (RTDB only).

return **`String (String object).`**

```cpp
String jsonString();
```

<br/>

#### Return the Firebase JSON object of server returned payload (RTDB only).

return **`FirebaseJson object.`**

```cpp
FirebaseJson &jsonObject();
```

<br/>

#### Return the Firebase JSON object pointer of server returned payload (RTDB only).

return **`FirebaseJson object `**pointer.

```cpp
FirebaseJson *jsonObjectPtr();
```

<br/>

#### Return the Firebase JSON Array object of server returned payload (RTDB only).

return **`FirebaseJsonArray object`**.

```cpp
FirebaseJsonArray &jsonArray();
```

<br/>

#### Return the Firebase JSON Array object pointer of server returned payload (RTDB only).

return **`FirebaseJsonArray object pointer`**.

```cpp
FirebaseJsonArray *jsonArrayPtr();
```

<br/>

#### Return the blob data (uint8_t) array of server returned payload (RTDB only).

return **`Dynamic array`** of 8-bit unsigned integer i.e. `std::vector<uint8_t>`.

```cpp
std::vector<uint8_t> blobData();
```

<br/>

#### Return the file stream of server returned payload (RTDB only).

return the **`file stream`**.

```cpp
File fileStream();
```

<br/>

#### Cast the FirebaseData object to object or primitive type variable (RTDB only).

return the **`The object or primitive type variable`**.

```cpp
to<type>();

e.g. to<String>(), to<int>(), to<bool>()
```

<br/>

#### Return the new appended node's name or key of server returned payload when calling pushXXX function (RTDB only).

return **`String`** (String object).

```cpp
String pushName();
```

<br/>

#### Get the stream connection status (RTDB only).

return **`Boolean`** type status indicates whether the Firebase Data object is working with a stream or not.

```cpp
bool isStream();
```

<br/>

#### Get the server connection status.

return **`Boolean`** type status indicates whether the Firebase Data object is connected to the server or not.

```cpp
bool httpConnected();
```

<br/>

#### Get the timeout event of server's stream (30 sec is the default) (RTDB only).

Nothing to do when stream connection timeout, the stream connection will be automatically resumed.

return **`Boolean`** type status indicates whether the stream was a timeout or not.

```cpp
bool streamTimeout();
```

<br/>

#### Get the availability of data or payload returned from the server (RTDB only).

return **`Boolean`** type status indicates whether the server returns the new payload or not.

```cpp
bool dataAvailable();
```

<br/>

#### Get the availability of stream event-data payload returned from the server (RTDB only).

return **`Boolean`** type status indicates whether the server returns the stream event-data 
payload or not.

```cpp
bool streamAvailable();
```

<br/>

#### Get the matching between data type that intend to get from/store to database and the server's return payload data type (RTDB only.

return **`Boolean`** type status indicates whether the type of data being get from/store to database 
and the server's returned payload is matched or not.

Data type checking was disable by default, which can be enabled via the Firebase Config e.g. config.rtdb.data_type_stricted = true

```cpp
bool mismatchDataType();
```

<br/>

#### Get the HTTP status code return from the server.

return **`Integer`** number of HTTP status.

```cpp
int httpCode();
```

<br/>

#### Get the HTTP payload length returned from the server.

return **`Integer`** number of payload length.

```cpp
int payloadLength();
```

<br/>

#### Get the maximum size of HTTP payload length returned from the server.

return **`Integer`** number of max payload length.

```cpp
int maxPayloadLength();
```

<br/>

#### Check overflow of the returned payload data buffer (RTDB only).

return **`Boolean`** of the overflow status.


Total default HTTP response buffer size is 400 bytes which can be set through Firebase.setResponseSize.


```cpp
bool bufferOverflow();
```

<br/>

#### Get the name (full path) of backup file in SD card (RTDB only).

return **`String`** (String object) of the file name that stores on SD card after backup operation.

```cpp
String getBackupFilename();
```

<br/>

#### Get the size of the backup file (RTDB only).

return **`Number of byte`** of backup file in byte after backup operation.

```cpp
size_t getBackupFileSize();
```

<br/>

#### Clear or empty data in Firebase Data object.

```cpp
void clear();
```

<br/>

#### Get the error description for file transferring (pushFile, setFile, backup and restore) (RTDB only).

return **`Error description string* (String object).`**

```cpp
String fileTransferError();
```

<br/>

#### Return the server's payload data.

return **`Payload string* (String object).`**

```cpp
String payload();
```

<br/><br/>

## Firebase Cloud Messaging Object Functions

<br/>

#### Store Firebase Cloud Messaging's authentication credentials.
    
param **`serverKey`** Server key found on Console: Project settings > Cloud Messaging

param **`spi_ethernet_module`** SPI_ETH_Module struct data, optional for ESP8266 use with Ethernet module.

SPI_ETH_Module struct data is for ESP8266 Ethernet supported module lwip interface.

<br/>

The usage example for Ethernet.

```cpp
#include <ENC28J60lwIP.h>

#define ETH_CS_PIN 16 //GPIO 16 connected to Ethernet module (ENC28J60) CS pin
 
ENC28J60lwIP eth(ETH_CS_PIN);

FirebaseData fbdo;

SPI_ETH_Module spi_ethernet_module;

//in setup()

spi_ethernet_module.enc28j60 = &eth;

fbdo.fcm.begin(FIREBASE_FCM_SERVER_KEY, &spi_ethernet_module);

```

```cpp
void begin(<string> serverKey, SPI_ETH_Module *spi_ethernet_module = NULL);
```

<br/>

#### Add recipient's device registration token or instant ID token.
    
param **`deviceToken`** Recipient's device registration token to add that message will be sent to.

```cpp
void addDeviceToken(<string> deviceToken);
```

<br/>

#### Remove recipient's device registration token or instant ID token.
    
param **`index`** Index (start from zero) of recipient's device registration token that added to FCM Data Object of Firebase Data object.

```cpp
void removeDeviceToken(uint16_t index);
```

<br/>

#### Clear all recipient's device registration tokens.
    
```cpp
 void clearDeviceToken();
```

<br/>

#### Set the notify message type information.
    
param **`title`** The title text of notification message.

param **`body`** The body text of notification message.

```cpp
void setNotifyMessage(<string> title, <string> body);
```

<br/>

#### Set the notify message type information.
    
param **`title`** The title text of notification message.

param **`body`** The body text of notification message.

param **`icon`** The name and/or included URI/URL of the icon to show on notifying the message.

```cpp
void setNotifyMessage(<string> title, <string> body, <string> icon);
```

<br/>

#### Set the notify message type information.
    
param **`title`** The title text of notification message.

param **`body`** The body text of notification message.

param **`icon`** The name and/or included URI/URL of the icon to show on notifying the message.

param **`click_action`** The URL or intent to accept click event on the notification message.

```cpp
void setNotifyMessage(<string> title, <string> body, <string> icon, <string> click_action);
```

<br/>

#### add the custom key/value in the notify message type information.
    
param **`key`** The key field in notification message.

param **`value`** The value field in the notification message.

```cpp
void addCustomNotifyMessage(<string> key, <string> value);
```

<br/>

#### Clear all notify message information.
    
```cpp
void clearNotifyMessage();
```

<br/>

#### Set the custom data message type information.
    
param **`jsonString`** The JSON structured data string.

```cpp
void setDataMessage(<string> jsonString);
```

<br/>

#### Set the custom data message type information.
    
param **`json`** The FirebaseJson object.

```cpp
void setDataMessage(FirebaseJson &json);
```

<br/>

#### Clear custom data message type information
    
```cpp
void clearDataMessage();
```

<br/>

#### Set the priority of the message (notification and custom data)
    
param **`priority`** The priority string i.e. normal and high.

```cpp
void setPriority(<string> priority);
```

<br/>

#### Set the collapse key of the message (notification and custom data)
    
param **`key`** String of collapse key.

```cpp
void setCollapseKey(<string> key);
```

<br/>

#### Set the Time To Live of the message (notification and custom data)
    
param **`seconds`** Number of seconds from 0 to 2,419,200 (4 weeks).

```cpp
void setTimeToLive(uint32_t seconds);
```

<br/>

#### Set topic of the message will be sent to
    
param **`topic - Topic string.

```cpp
void setTopic(<string> topic);
```

<br/>

#### Get the send result
    
return **`const char*`** of payload returned from the server.

```cpp
const char* getSendResult();
```

<br/><br/>

## FirebaseJSON object Functions

<br/>

#### Set or deserialize the JSON object data (JSON object literal) as FirebaseJson object.

param **`data`** The JSON object literal string to set or deserialize.

return **`bool`** value represents the successful operation.

Call FirebaseJson.errorPosition to get the error.

```cpp
bool setJsonData(<string> data);
```

<br/>

#### Clear internal buffer of FirebaseJson object.
    
return **`instance of an object.`**

```cpp
FirebaseJson &clear();
```

<br/>

#### Set JSON data (Client response) to FirebaseJson object.
    
param **`client`** The pointer to or instance of Client object.

return **`instance of an object.`**

```cpp
bool readFrom(Client *client);

bool readFrom(Client &client);
```

<br/>

#### Set JSON data (WiFiClient response) to FirebaseJson object.
    
param **`client`** The pointer to or instance of WiFiClient object.

return **`instance of an object.`**

```cpp
bool readFrom(WiFiClient *client);

bool readFrom(WiFiClient &client);
```

<br/>

#### Set JSON data (WiFiClientSecure response) to FirebaseJson object.
    
param **`client`** The pointer to or instance of WiFiClientSecure object.

return **`instance of an object.`**

```cpp
bool readFrom(WiFiClientSecure *client);

bool readFrom(WiFiClientSecure &client);
```

<br/>

#### Set JSON data (Seral object) to FirebaseJson object.
    
param **`ser`** The HW or SW Serial object.

param **`timeoutMS`** The timeout in millisecond to wait for Serial data to be completed.

return **`instance of an object.`**

```cpp
bool readFrom(HardwareSerial &ser, uint32_t timeoutMS = 5000);

bool readFrom(SoftwareSerial &ser, uint32_t timeoutMS = 5000);
```

<br/>

#### Set JSON data (File object) to FirebaseJson object.
    
param **`file`** The File object.

return **`instance of an object.`**

```cpp
bool readFrom(fs::File &file);
```

<br/>

#### Add null to FirebaseJson object.
    
param **`key`** The new key string that null to be added.

return **`instance of an object.`**

```cpp
FirebaseJson &add(<string> key);
```

<br/>

#### Add value to FirebaseJson object.
    
param **`key`** The new key string that string value to be added.

param **`value`** The value for the new specified key.

return **`instance of an object.`**

The value that can be added is the following supported types e.g. flash string (PROGMEM and FPSTR), String, C/C++ std::string, const char*, char array, string literal, all integer and floating point numbers, boolean, FirebaseJson object and array.

```cpp
FirebaseJson &add(<string> key, <type> value);
```

<br/>

#### Get the FirebaseJson object serialized string.

param **`out`** The writable object e.g. String, std::string, char array, Stream e.g ile, WiFi/Ethernet Client and LWMQTT, that accepts the returning string.

param **`topic`** The MQTT topic (LWMQTT).

param **`prettify`** Boolean flag for return the pretty format string i.e. with text indentation and newline. 

```cpp
void toString(<type> out, bool prettify = false);

void toString(<type> out, <string> topic, bool prettify = false);
```

<br/>

#### Get the value from the specified node path in FirebaseJson object.

param **`result`** The reference of FirebaseJsonData that holds the result.

param **`path`** Relative path to the specific node in FirebaseJson object.

param **`prettify`** The text indentation and new line serialization option.

return **`boolean status of the operation.`**

<br/>

The FirebaseJsonData object holds the returned data which can be read from the following properties.

jsonData.stringValue - contains the returned string.

jsonData.intValue - contains the returned integer value.

jsonData.floatValue - contains the returned float value.

jsonData.doubleValue - contains the returned double value.

jsonData.boolValue - contains the returned boolean value.

jsonData.success - used to determine the result of the get operation.

jsonData.type - used to determine the type of returned value in string represents 
the types of value e.g. string, int, double, boolean, array, object, null and undefined.

<br/>

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
 
 ```cpp
 bool get(FirebaseJsonData &result, <string> path, bool prettify = false);
 ```

<br/>

#### Search element by key or path in FirebaseJsonArray object.

param **`result`** The reference of FirebaseJsonData that holds the result.

param **`criteria`** The FirebaseJson::SearchCriteria data.

param **`prettify`** The text indentation and new line serialization option.

return **`number`** of elements found from search.

The SearchCriteria data has the properties e.g.

**`path`** - The key of path to search.

Path can be wildcard with * in search path and * should use as key in part and do not mix with any character.

**`value`** - The value string to search.

**`depth`** - The begin depth (int) of element to search, default is 0.

**`endDepth`** - The end depth (int) of element to search, default is -1.

**`searchAll`** - The boolean option to search all occurrences of elements.

```cpp
size_t search(SearchCriteria &criteria);

size_t search(FirebaseJsonData &result, SearchCriteria &criteria, bool prettify = false);
```

<br/>

#### Get the full path to any element in FirebaseJson object.

param **`path`** The key or path to search in to.

param **`searchAll`** Search all occurrences.

return **`full path string`** in case of found.

```cpp
String getPath(<string> path, bool searchAll = false);
```

<br/>

#### Check whether key or path to the child element existed in FirebaseJson object or not.

param **`path`** The key or path of child element check.

return **`boolean`** status indicated the existence of element.

```cpp
bool isMember(<string> path);
```

<br/>

#### Parse and collect all node/array elements in FirebaseJson object.

return **`number`** of child/array elements in FirebaseJson object.

```cpp
size_t iteratorBegin();
```

<br/>

#### Get child/array elements from FirebaseJson objects at specified index.
    
param **`index`** The element index to get.

param **`type`** The integer which holds the type of data i.e. FirebaseJson::OBJECT and FirebaseJson::ARRAY

param **`key`** The string which holds the key/name of the object, can return empty String if the data type is an array.

param **`value`** The string which holds the value for the element key or array.   

```cpp
void iteratorGet(size_t index, int &type, String &key, String &value);
```

<br/>

#### Get child/array elements from FirebaseJson objects at specified index.

param **`index`** The element index to get.   

return **` IteratorValue struct`** 

This should call after iteratorBegin.

The IteratorValue struct contains the following members:
int type
String key
String value

```cpp
IteratorValue valueAt(size_t index);
```

<br/>

#### Clear all iterator buffer (should be called since iteratorBegin was called).

```cpp
void iteratorEnd();
```

<br/>

#### Set null to FirebaseJson object at the specified node path.
    
param **`path`** The relative path that null to be set.


The relative path can be mixed with array index (number placed inside square brackets) and node names e.g. /myRoot/[2]/Sensor1/myData/[3].


```cpp
void set(<string> path);
```

<br/>

#### Set value to FirebaseJson object at the specified node path.
    
param **`path`** The relative path that string value to be set.

param **`value`** The value to set.

<br/>

The relative path can be mixed with array index (number placed inside square brackets) and node names 
e.g. /myRoot/[2]/Sensor1/myData/[3].

The value that can be added is the following supported types e.g. flash string (PROGMEM and FPSTR), String, C/C++ std::string, const char*, char array, string literal, all integer and floating point numbers, boolean, FirebaseJson object and array.

```cpp
void set(<string> path, <type> value);
```

<br/>

#### Remove the specified node and its content.

param **`path`** The relative path to remove its contents/children.

return **`bool`** value represents the successful operation.

```cpp
bool remove(<string> path);
```

<br/>

#### Get raw JSON.

return **`raw JSON string`**

```cpp
const char *raw();
```

<br/>

#### Get the error position at the JSON object literal from parsing.

return **`the position of error in JSON object literal`**

Return -1 when for no parsing error.

```cpp
int errorPosition();
```

<br/>

#### Get the size of serialized JSON object buffer.

param **`prettify`** The text indentation and new line serialization option.

return **`size in byte of buffer`**

```cpp
size_t serializedBufferLength(bool prettify = false);
```

<br/>

#### Set the precision for float to JSON object.

param **`digits`** The number of decimal places.

```cpp
void setFloatDigits(uint8_t digits);
```

<br/>

#### Set the precision for double to JSON object.

param **`digits`** The number of decimal places.

```cpp
void setDoubleDigits(uint8_t digits);
```

<br/>

#### Get http response code of reading JSON data from WiFi/Ethernet Client.

return **`the response code`** of reading JSON data from WiFi/Ethernet Client 

```cpp
int responseCode();
```

<br/><br/>

## FirebaseJsonArray object functions

<br/>

#### Set or deserialize the JSON array data (JSON array literal) as FirebaseJsonArray object.

param **`data`** The JSON array literal string to set or deserialize.

return **`bool`** value represents the successful operation.

Call FirebaseJsonArray.errorPosition to get the error.

```cpp
bool setJsonArrayData(<string> data);
```

<br/>

#### Add null to FirebaseJsonArray object.

return **`instance of an object.`**

```cpp
FirebaseJsonArray &add();
```

<br/>

#### Add value to FirebaseJsonArray object.

param **`value`** The value to add.

return **`instance of an object.`**

The value that can be added is the following supported types e.g. flash string (PROGMEM and FPSTR), String, C/C++ std::string, const char*, char array, string literal, all integer and floating point numbers, boolean, FirebaseJson object and array.


```cpp
FirebaseJsonArray &add(<type> value);
```

<br/>

#### Add multiple values to FirebaseJsonArray object.

param **`v`** The value of any type to add.

param **`n`** The consecutive values of any type to add.

return **`instance of an object.`**

e.g. add("a","b",1,2)

```cpp
FirebaseJsonArray &add(First v, Next... n);
```

<br/>

#### Set JSON data (Client response) to FirebaseJsonArray object.
    
param **`client`** The pointer to or instance of Client object.

return **`instance of an object.`**

```cpp
bool readFrom(Client *client);

bool readFrom(Client &client);
```

<br/>

#### Set JSON data (WiFiClient response) to FirebaseJsonArray object.
    
param **`client`** The pointer to or instance of WiFiClient object.

return **`instance of an object.`**

```cpp
bool readFrom(WiFiClient *client);

bool readFrom(WiFiClient &client);
```

<br/>

#### Set JSON data (WiFiClientSecure response) to FirebaseJsonArray object.
    
param **`client`** The pointer to or instance of WiFiClientSecure object.

return **`instance of an object.`**

```cpp
bool readFrom(WiFiClientSecure *client);

bool readFrom(WiFiClientSecure &client);
```

<br/>

#### Set JSON data (Seral object) to FirebaseJsonArray object.
    
param **`ser`** The HW or SW Serial object.

param **`timeoutMS`** The timeout in millisecond to wait for Serial data to be completed.

return **`instance of an object.`**

```cpp
bool readFrom(HardwareSerial &ser, uint32_t timeoutMS = 5000);

bool readFrom(SoftwareSerial &ser, uint32_t timeoutMS = 5000);
```

<br/>

#### Set JSON data (File object) to FirebaseJsonArray object.
    
param **`file`** The File object.

return **`instance of an object.`**

```cpp
bool readFrom(fs::File &file);
```

<br/>

#### Get the array value at the specified index or path from the FirebaseJsonArray object.

param **`result`** The reference of FirebaseJsonData object that holds data at the specified index.

param **`index_or_path`** Index of data or relative path to data in FirebaseJsonArray object.    

return **`boolean`** status of the operation.

The relative path must begin with array index (number placed inside square brackets) followed by
other array indexes or node names e.g. /[2]/myData would get the data from myData key inside the array indexes 2

```cpp
bool get(FirebaseJsonData &result, <int or string> index_or_path);
```

<br/>

#### Search element by key or path in FirebaseJsonArray object.

param **`result`** The reference of FirebaseJsonData that holds the result.

param **`criteria`** The FirebaseJson::SearchCriteria data.

param **`prettify`** The text indentation and new line serialization option.

return **`number`** of elements found from search.

The SearchCriteria data has the properties e.g.

**`path`** - The key of path to search.

Path can be wildcard with * in search path and * should use as key in part and do not mix with any character.

**`value`** - The value string to search.

**`depth`** - The begin depth (int) of element to search, default is 0.

**`endDepth`** - The end depth (int) of element to search, default is -1.

**`searchAll`** - The boolean option to search all occurrences of elements.

```cpp
size_t search(SearchCriteria &criteria);

size_t search(FirebaseJsonData &result, SearchCriteria &criteria, bool prettify = false);
```

<br/>

#### Get the full path to any element in FirebaseJsonArray.

param **`path`** The key or path to search in to.

param **`searchAll`** Search all occurrences.

return **`full path string`** in case of found.

```cpp
String getPath(<string> path, bool searchAll = false);
```

<br/>

#### Check whether key or path to the child element existed in FirebaseJsonArray or not.

param **`path`** The key or path of child element check.

return **`boolean`** status indicated the existence of element.

```cpp
bool isMember(<string> path);
```

<br/>

#### Parse and collect all node/array elements in FirebaseJsonArray object.

return **`number`** of child/array elements in FirebaseJsonArray object.

```cpp
size_t iteratorBegin();
```

<br/>

#### Get child/array elements from FirebaseJsonArray objects at specified index.
    
param **`index`** The element index to get.

param **`type`** The integer which holds the type of data i.e. FirebaseJson::OBJECT and FirebaseJson::ARRAY

param **`key`** The string which holds the key/name of the object, can return empty String if the data type is an array.

param **`value`** The string which holds the value for the element key or array.   

```cpp
void iteratorGet(size_t index, int &type, String &key, String &value);
```

<br/>

#### Get child/array elements from FirebaseJsonArray objects at specified index.

param **`index`** The element index to get.   

return **` IteratorValue struct`** 

This should call after iteratorBegin.

The IteratorValue struct contains the following members:
int type
String key
String value

```cpp
IteratorValue valueAt(size_t index);
```

<br/>

#### Clear all iterator buffer (should be called since iteratorBegin was called).

```cpp
void iteratorEnd();
```

<br/>

#### Get the length of array in FirebaseJsonArray object.  

return **`length of the array.`**

```cpp
size_t size();
```

<br/>

#### Get the FirebaseJsonArray object serialized string.

param **`out`** The writable object e.g. String, std::string, char array, Stream e.g ile, WiFi/Ethernet Client and LWMQTT, that accepts the returning string.

param **`prettify`** Boolean flag for return the pretty format string i.e. with text indentation and newline. 

```cpp
void toString(<type> out, bool prettify = false);
```

<br/>

#### Get raw JSON Array.

return **`raw JSON Array string`**

```cpp
const char *raw();
```

<br/>

#### Get the size of serialized JSON array buffer.

param **`prettify`** The text indentation and new line serialization option.

return **`size in byte of buffer`**

```cpp
size_t serializedBufferLength(bool prettify = false);
```

<br/>

#### Clear all array in FirebaseJsonArray object.

return **`instance of an object.`**

```cpp
FirebaseJsonArray &clear();
```

<br/>

#### Set null to FirebaseJsonArray object at at specified index or path.
    
param **`index_or_path`** The array index or path that null to be set.

```cpp
void set(<int or string> index_or_path);
```

<br/>

####  Set String to FirebaseJsonArray object at the specified index.
    
param **`index_or_path`** The array index or path that value to be set.

param **`value`** The value to set.


```cpp
void set(<int or string> index_or_path, <type> value);
```

<br/>

#### Remove the array value at the specified index or path from the FirebaseJsonArray object.

param **`index_or_path`** The array index or relative path to array to be removed.

return **`bool`** value represents the successful operation.

```cpp
bool remove(<int or string> index_or_path);
```

<br/>

#### Get the error position at the JSON array literal from parsing.

return **`the position of error in JSON array literal`**

Return -1 when for no parsing error.

```cpp
int errorPosition();
```

<br/>

#### Set the precision for float to JSON Array object.

param **`digits`** The number of decimal places.

```cpp
void setFloatDigits(uint8_t digits);
```

<br/>

#### Set the precision for double to JSON Array object.

param **`digits`** The number of decimal places.

```cpp
void setDoubleDigits(uint8_t digits);
```

<br/><br/>

## FirebaseJsonData object functions

<br/>

#### Get array data as FirebaseJsonArray object from FirebaseJsonData object.
    
param **`jsonArray`** The returning FirebaseJsonArray object.

return **`bool`** status for successful operation.

This should call after pares or get functions.

```cpp
bool getArray(FirebaseJsonArray &jsonArray);
```

<br/>

#### Get array data as FirebaseJsonArray object from FirebaseJsonData object.

param **`source`** The JSON array string.
    
param **`jsonArray`** The returning FirebaseJsonArray object.

return **`bool`** status for successful operation.

This should call after pares or get functions.

```cpp
bool getArray(<string> source, FirebaseJsonArray &jsonArray);
```

<br/>

#### Get array data as FirebaseJson object from FirebaseJsonData object.
    
param **`jsonArray`** The returning FirebaseJson object.

return **`bool`** status for successful operation.

This should call after pares or get functions.

```cpp
bool getJSON(FirebaseJson &json);
```

<br/>

#### Get JSON data as FirebaseJson object from string.

param **`source`** The JSON string.
    
param **`json`** The returning FirebaseJsonArray object.

return **`bool`** status for successful operation.

This should call after pares or get functions.

```cpp
bool getJSON(<string> source, FirebaseJson &json);
```

<br/>

#### Cast the FirebaseJsonData object to object or primitive type variable.

return the **`The object or primitive type variable`**.

```cpp
to<type>();

e.g. to<String>(), to<int>(), to<bool>()
```

<br/><br/>

### FirebaseJsonData object properties

**`stringValue`** The String value of parses data.

**`intValue`** The int value of parses data.

**`doubleValue`** The double value of parses data.

**`floatValue`** The float value of parses data.

**`boolVlue`** The bool value of parses data.

**`success`** used to determine the result of the deserialize operation.

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

<br/><br/>

## License

The MIT License (MIT)

Copyright (c) 2021 K. Suwatchai (Mobizt)


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

