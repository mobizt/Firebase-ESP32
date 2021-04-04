/**
 * Google's Firebase Realtime Database Arduino Library for ESP32, version 3.8.26
 * 
 * April 4, 2021
 * 
 *   Updates:
 * - Fix the memory leaks in internal JSON parser.
 * - Fix the token pre-refreshment issue.
 * 
 * This library provides ESP32 to perform REST API by GET PUT, POST, PATCH, DELETE data from/to with Google's Firebase database using get, set, update
 * and delete calls. 
 * 
 * The library was tested and work well with ESP32 based modules.
 * 
 * The MIT License (MIT)
 * Copyright (c) 2021 K. Suwatchai (Mobizt)
 * 
 * 
 * Permission is hereby granted, free of charge, to any person returning a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef FirebaseESP32_H
#define FirebaseESP32_H

#ifdef ESP32

#include <Arduino.h>
#include "signer/Signer.h"
#include "rtdb/FB_RTDB.h"
#include "Utils.h"


class FirebaseESP32
{
  friend class QueryFilter;
  friend class FirebaseSession;

public:
  FB_RTDB RTDB;

  FirebaseESP32();
  ~FirebaseESP32();

  /** Initialize Firebase with the config and Firebase's authentication credentials.
   * 
   * @param config The pointer to FirebaseConfig data.
   * @param auth The pointer to FirebaseAuth data.
   * 
   * @note For FirebaseConfig and FirebaseAuth data usage, see the examples.
  */
  void begin(FirebaseConfig *config, FirebaseAuth *auth);

  /** Provide the details of token generation.
   * 
   * @return token_info_t The token_info_t structured data that indicates the status.
   * 
   * @note Use type property to get the type enum value.
   * token_type_undefined or 0,
   * token_type_legacy_token or 1,
   * token_type_id_token or 2,
   * token_type_custom_token or 3,
   * token_type_oauth2_access_token or 4
   * 
   * Use status property to get the status enum value.
   * token_status_uninitialized or 0,
   * token_status_on_signing or 1,
   * token_status_on_request or 2,
   * token_status_on_refresh or 3,
   * token_status_ready or 4
   * 
   * In case of token generation and refreshment errors,
   * use error.code property to get the error code number.
   * Use error.message property to get the error message string.
   * 
  */
  struct token_info_t authTokenInfo();

  /** Store Firebase's authentication credentials using database secret (obsoleted).
   * 
   * @param host Your Firebase database project host e.g. Your_ProjectID.firebaseio.com.
   * @param auth Your database secret.
   * @param caCert Root CA certificate base64 string (PEM file).
   * @param caCertFile Root CA certificate DER file (binary).
   * @param StorageType Type of storage, StorageType::SD and StorageType::FLASH.
   * 
   * The file systems for flash and sd memory can be changed in FirebaseFS.h.
  */
  void begin(const String &host, const String &auth);

  void begin(const String &host, const String &auth, const char *caCert);

  void begin(const String &host, const String &auth, const String &caCertFile, uint8_t storageType);

  /** Stop Firebase and release all resources.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
  */
  void end(FirebaseData &fbdo);

  /** Sign up for a new user.
   * 
   * @param config The pointer to FirebaseConfig data.
   * @param auth The pointer to FirebaseAuth data.
   * @param email The user Email.
   * @param password The user password.
   * @return Boolean type status indicates the success of the operation. 
   * 
   * @note By calling Firebase.begin with config and auth after sign up will be signed in.
   * 
   * This required Email/Password provider to be enabled,
   * From Firebase console, select Authentication, select Sign-in method tab, 
   * under the Sign-in providers list, enable Email/Password provider.
   * 
   * If the assigned email and passowrd are empty,
   * the anonymous user will be created if Anonymous provider is enabled.
   * 
   * To enable Anonymous provider,
   * from Firebase console, select Authentication, select Sign-in method tab, 
   * under the Sign-in providers list, enable Anonymous provider.
  */
  bool signUp(FirebaseConfig *config, FirebaseAuth *auth, const char *email, const char *password);

  /** Send a user a verification Email.
   * 
   * @param config The pointer to FirebaseConfig data.
   * @param idToken The id token of user that was already signed in with Email and password (optional).
   * @return Boolean type status indicates the success of the operation. 
   * 
   * @note The id token can be obtained from config.signer.tokens.id_token 
   * after begin with config and auth data
   * 
   * If the idToken is not assigned, the internal config.signer.tokens.id_token
   * will be used. 
   * 
   * See the Templates of Email address verification in the Firebase console
   * , Authentication.
   * 
  */
  bool sendEmailVerification(FirebaseConfig *config, const char *idToken = "");

  /** Send a user a password reset link to Email.
   * 
   * @param config The pointer to FirebaseConfig data.
   * @param email The user Email to send the password resset link.
   * @return Boolean type status indicates the success of the operation. 
   * 
  */
  bool sendResetPassword(FirebaseConfig *config, const char *email);

  /** [Obsoleted] 
   * Set the stream task (RTOS task) reserved stack memory in bytes.
   * 
   * @param size - The number of stack size in bytes.
   * 
   * @note The stream task will be created only when the user sets the stream callbacks.
  */
  void setStreamTaskStackSize(size_t size);

  /** Enable multiple HTTP requests at a time.
   * 
   * @param enable - The boolean value to enable/disable.
   * 
   * @note The multiple HTTP requessts at a time is disable by default to prevent the large memory used in multiple requests.
  */
  void allowMultipleRequests(bool enable);

  /** Reconnect WiFi if lost connection.
   * 
   * @param reconnect The boolean to set/unset WiFi AP reconnection.
  */
  void reconnectWiFi(bool reconnect);

  /** Set the decimal places for float value to be stored in database.
   * 
   * @param digits The decimal places. 
  */
  void setFloatDigits(uint8_t digits);

  /** Set the decimal places for double value to be stored in database.
   * 
   * @param digits The decimal places. 
  */
  void setDoubleDigits(uint8_t digits);

  /** Set the timeout of Firebase.get functions.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param millisec The milliseconds to limit the request (0 900,000 ms or 15 min).
  */
  void setReadTimeout(FirebaseData &fbdo, int millisec);

  /** Set the size limit of payload data that will write to the database for each request.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param size The size identified string e.g. tiny, small, medium, large and unlimited. 
   * 
   * @note Size string and its write timeout in seconds e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  */
  void setwriteSizeLimit(FirebaseData &fbdo, const String &size);

  /** Read the database rules.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @return Boolean type status indicates the success of the operation. 
   * 
   * @note Call [FirebaseData object].jsonData will return the JSON string value of 
   * database rules returned from the server.
  */
  bool getRules(FirebaseData &fbdo);

  /** Write the database rules.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param rules Database rules in JSON String format.
   * @return Boolean type status indicates the success of the operation.
  */
  bool setRules(FirebaseData &fbdo, const String &rules);

  /** Determine whether the defined database path exists or not.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path to be checked.
   * @return Boolean type result indicates whether the defined path existed or not.
   */
  bool pathExist(FirebaseData &fbdo, const String &path);

  /** Determine the unique identifier (ETag) of current data at the defined database path.
   * 
   * @return String of unique identifier.
  */
  String getETag(FirebaseData &fbdo, const String &path);

  /** Get the shallowed data at a defined node path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path being read the data.
   * @return Boolean type status indicates the success of the operation. 
   * 
   * @note Return the child data with its value or JSON object (its values will be truncated to true). 
   * Call [FirebaseData object].stringData() to get shallowed string data (number, string and JSON object).
  */
  bool getShallowData(FirebaseData &fbdo, const String &path);

  /** Enable the library to use only classic HTTP GET and POST methods.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param flag Boolean value to enable.
   * 
   * @note This option used to escape the Firewall restriction (if the device is connected through 
   * Firewall) that allows only HTTP GET and POST
   * HTTP PATCH request was sent as PATCH which not affected by this option.
  */
  void enableClassicRequest(FirebaseData &fbdo, bool flag);

  /** Set the virtual child node ".priority" to the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which to set the priority value.
   * @param priority The priority value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note This allows us to set priority to any node other than a priority that set through setJSON, 
   * pushJSON, updateNode, and updateNodeSilent functions. 
   * The returned priority value from server can read from function [FirebaseData object].priority().
   */
  bool setPriority(FirebaseData &fbdo, const String &path, float priority);

  /** Read the virtual child node ".priority" value at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which to set the priority value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note The priority value from server can read from function [FirebaseData object].priority().
   */
  bool getPriority(FirebaseData &fbdo, const String &path);

  /** Append new integer value to the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which integer value will be appended.
   * @param intValue The appended value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note The new appended node's key will be stored in Firebase Data object, 
   * which its value can be accessed via function [FirebaseData object].pushName().
  */
  bool pushInt(FirebaseData &fbdo, const String &path, int intValue);

  bool push(FirebaseData &fbdo, const String &path, int intValue);

  /** Append new integer value and the virtual child ".priority" to the defined database path.
  */
  bool pushInt(FirebaseData &fbdo, const String &path, int intValue, float priority);

  bool push(FirebaseData &fbdo, const String &path, int intValue, float priority);

  /** Append new float value to the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path in which float value will be appended.
   * @param floatValue The appended value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note The new appended node's key will be stored in Firebase Data object, 
   * which its value can be accessed via function [FirebaseData object].pushName().
  */
  bool pushFloat(FirebaseData &fbdo, const String &path, float floatValue);

  bool push(FirebaseData &fbdo, const String &path, float floatValue);

  /** Append new float value and the virtual child ".priority" to the defined database path.
  */
  bool pushFloat(FirebaseData &fbdo, const String &path, float floatValue, float priority);

  bool push(FirebaseData &fbdo, const String &path, float floatValue, float priority);

  /** Append new double value (8 bytes) to the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path in which float value will be appended.
   * @param doubleValue The appended value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note The new appended node's key will be stored in Firebase Data object, 
   * which its value can be accessed via function [FirebaseData object].pushName().
  */
  bool pushDouble(FirebaseData &fbdo, const String &path, double doubleValue);

  bool push(FirebaseData &fbdo, const String &path, double doubleValue);

  /** Append new double value (8 bytes) and the virtual child ".priority" to the defined database path.
  */
  bool pushDouble(FirebaseData &fbdo, const String &path, double doubleValue, float priority);

  bool push(FirebaseData &fbdo, const String &path, double doubleValue, float priority);

  /** Append new Boolean value to the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which Boolean value will be appended.
   * @param boolValue The appended value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note The new appended node's key will be stored in Firebase Data object, 
   * which its value can be accessed via function [FirebaseData object].pushName().

  */
  bool pushBool(FirebaseData &fbdo, const String &path, bool boolValue);

  bool push(FirebaseData &fbdo, const String &path, bool boolValue);

  /** Append the new Boolean value and the virtual child ".priority" to the defined database path.
  */
  bool pushBool(FirebaseData &fbdo, const String &path, bool boolValue, float priority);

  bool push(FirebaseData &fbdo, const String &path, bool boolValue, float priority);

  /** Append a new string (text) to the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which string will be appended.
   * @param StringValue The appended value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note The new appended node's key stored in Firebase Data object, 
   * which can be accessed via function [FirebaseData object].pushName().
  */
  bool pushString(FirebaseData &fbdo, const String &path, const String &stringValue);

  bool push(FirebaseData &fbdo, const String &path, const char *stringValue);

  bool push(FirebaseData &fbdo, const String &path, const String &stringValue);

  /** Append new string (text) and the virtual child ".priority" to the defined database path.
  */
  bool pushString(FirebaseData &fbdo, const String &path, const String &stringValue, float priority);

  bool push(FirebaseData &fbdo, const String &path, const char *stringValue, float priority);

  bool push(FirebaseData &fbdo, const String &path, const String &stringValue, float priority);

  /** Append new child nodes key and value (using FirebaseJson object) to the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which key and value in FirebaseJson object will be appended.
   * @param json The appended FirebaseJson object.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note The new appended node key will be stored in Firebase Data object, 
   * which its value can be accessed via function [FirebaseData object].pushName().
  */
  bool pushJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json);

  bool push(FirebaseData &fbdo, const String &path, FirebaseJson &json);

  /** Append new child node key and value (FirebaseJson object) and the virtual child ".priority" to the defined database path.
  */
  bool pushJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority);

  bool push(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority);

  /** Append child node array (using FirebaseJsonArray object) to the defined database path. 
   * This will replace any child nodes inside the defined path with array defined in FirebaseJsonArray object.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which key and value in FirebaseJsonArray object will be appended.
   * @param arr The appended FirebaseJsonArray object.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note The new appended node's key will be stored in Firebase Data object, 
   * which its value can be accessed via function [FirebaseData object].pushName().
  */
  bool pushArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr);

  bool push(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr);

  /** Append FirebaseJsonArray object and virtual child ".priority" at the defined database path.
  */

  bool pushArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority);

  bool push(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority);

  /** Append new blob (binary data) to the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which binary data will be appended.
   * @param blob Byte array of data.
   * @param size Size of the byte array.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note The new appended node's key will be stored in Firebase Data object, 
   * which its value can be accessed via function [FirebaseData object].pushName().
  */
  bool pushBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size);

  bool push(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size);

  /** Append new blob (binary data) and the virtual child ".priority" to the defined database path.
  */
  bool pushBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority);

  bool push(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority);

  /** Append new binary data from the file stores on SD card/Flash memory to the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param storageType Type of storage to read file data, StorageType::FLASH or StorageType::SD.
   * @param path Target database path in which binary data from the file will be appended.
   * @param fileName File name included its path in SD card/Flash memory.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note The new appended node's key will be stored in Firebase Data object, 
   * which its value can be accessed via function [FirebaseData object].pushName().
   * 
   * The file systems for flash and sd memory can be changed in FirebaseFS.h.
  */
  bool pushFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName);

  bool push(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName);

  /** Append new binary data from the file stores on SD card/Flash memory and the virtual child ".priority" 
   * to the defined database path.
  */
  bool pushFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority);

  bool push(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority);

  /** Append the new Firebase server's timestamp to the defined database path.*
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which timestamp will be appended.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note The new appended node's key will be stored in Firebase Data object, 
   * which its value can be accessed via function [FirebaseData object].pushName().
   */
  bool pushTimestamp(FirebaseData &fbdo, const String &path);

  /** Set integer data at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which integer data will be set.
   * @param intValue Integer value to set.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data that successfully stores in the database. 
   * Call [FirebaseData object].intData will return the integer value of 
   * payload returned from server.
  */
  bool setInt(FirebaseData &fbdo, const String &path, int intValue);

  bool set(FirebaseData &fbdo, const String &path, int intValue);

  /** Set integer data and virtual child ".priority" at the defined database path.
  */
  bool setInt(FirebaseData &fbdo, const String &path, int intValue, float priority);

  bool set(FirebaseData &fbdo, const String &path, int intValue, float priority);

  /** Set integer data at the defined database path if defined database path's ETag matched the ETag value.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which integer data will be set.
   * @param intValue Integer value to set.
   * @param ETag Known unique identifier string (ETag) of defined database path.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * If ETag at the defined database path does not match the provided ETag parameter,
   * the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched). 
   * 
   * If the operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value. 
   * Also call [FirebaseData object].intData to get the current integer value.
  */
  bool setInt(FirebaseData &fbdo, const String &path, int intValue, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, int intValue, const String &ETag);

  /** Set integer data and the virtual child ".priority" if defined ETag matches at the defined database path 
  */
  bool setInt(FirebaseData &fbdo, const String &path, int intValue, float priority, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, int intValue, float priority, const String &ETag);

  /** Set float data at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path in which float data will be set.
   * @param floatValue Float value to set.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * Call [FirebaseData object].floatData will return the float value of 
   * payload returned from server.
  */
  bool setFloat(FirebaseData &fbdo, const String &path, float floatValue);

  bool set(FirebaseData &fbdo, const String &path, float floatValue);

  /** Set float data and virtual child ".priority" at the defined database path.
  */
  bool setFloat(FirebaseData &fbdo, const String &path, float floatValue, float priority);

  bool set(FirebaseData &fbdo, const String &path, float floatValue, float priority);

  /** Set float data at the defined database path if defined database path's ETag matched the ETag value.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path in which float data will be set.
   * @param floatValue Float value to set.
   * @param ETag Known unique identifier string (ETag) of defined database path.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * Call [FirebaseData object].floatData will return the float value of payload returned from server. 
   * 
   * If ETag at the defined database path does not match the provided ETag parameter, 
   * the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).
   * 
   * If the operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value. 
   * Also call [FirebaseData object].floatData to get the current float value.
   */
  bool setFloat(FirebaseData &fbdo, const String &path, float floatValue, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, float floatValue, const String &ETag);

  /** Set float data and the virtual child ".priority" if defined ETag matches at the defined database path 
  */
  bool setFloat(FirebaseData &fbdo, const String &path, float floatValue, float priority, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, float floatValue, float priority, const String &ETag);

  /** Set double data at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path in which float data will be set.
   * @param doubleValue Double value to set.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * Call [FirebaseData object].doubleData will return the double value of the payload returned from the server. 
   * 
   * Due to bugs in Serial.print in Arduino, to print large double value with zero decimal place, 
   * use printf("%.9lf\n", firebaseData.doubleData()); for print the returned double value up to 9 decimal places.
  */
  bool setDouble(FirebaseData &fbdo, const String &path, double doubleValue);

  bool set(FirebaseData &fbdo, const String &path, double doubleValue);

  /** Set double data and virtual child ".priority" at the defined database path.
  */
  bool setDouble(FirebaseData &fbdo, const String &path, double doubleValue, float priority);

  bool set(FirebaseData &fbdo, const String &path, double doubleValue, float priority);

  /** Set double data at the defined database path if defined database path's ETag matched the ETag value.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path in which float data will be set.
   * @param doubleValue Double value to set.
   * @param ETag Known unique identifier string (ETag) of defined database path.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * Call [FirebaseData object].doubleData will return the double value of the payload returned from the server. 
   * 
   * If ETag at the defined database path does not match the provided ETag parameter, 
   * the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched). 
   * 
   * If the operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value. 
   * Also call [FirebaseData object].doubeData to get the current double value.
  */
  bool setDouble(FirebaseData &fbdo, const String &path, double doubleValue, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, double doubleValue, const String &ETag);

  /** Set double data and the virtual child ".priority" if defined ETag matches at the defined database path 
  */
  bool setDouble(FirebaseData &fbdo, const String &path, double doubleValue, float priority, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, double doubleValue, float priority, const String &ETag);

  /** Set Boolean data at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which Boolean data will be set.
   * @param boolValue Boolean value to set.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * Call [FirebaseData object].boolData will return the Boolean value of 
   * the payload returned from the server.
  */
  bool setBool(FirebaseData &fbdo, const String &path, bool boolValue);

  bool set(FirebaseData &fbdo, const String &path, bool boolValue);

  /** Set boolean data and virtual child ".priority" at the defined database path.
  */
  bool setBool(FirebaseData &fbdo, const String &path, bool boolValue, float priority);

  bool set(FirebaseData &fbdo, const String &path, bool boolValue, float priority);

  /** Set Boolean data at the defined database path if defined database path's ETag matched the ETag value.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which Boolean data will be set.
   * @param boolValue Boolean value to set.
   * @param ETag Known unique identifier string (ETag) of defined database path.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * Call [FirebaseData object].boolData will return the Boolean value of 
   * the payload returned from the server. 
   * 
   * If ETag at the defined database path does not match the provided ETag parameter, 
   * the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched). 
   * 
   * If the operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value. 
   * Also call [FirebaseData object].doubeData to get the current boolean value.
  */
  bool setBool(FirebaseData &fbdo, const String &path, bool boolValue, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, bool boolValue, const String &ETag);

  /** Set boolean data and the virtual child ".priority" if defined ETag matches at the defined database path 
  */
  bool setBool(FirebaseData &fbdo, const String &path, bool boolValue, float priority, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, bool boolValue, float priority, const String &ETag);

  /** Set string (text) at the defined database path. 
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path in which string data will be set.
   * @param stringValue String or text to set.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data that successfully 
   * stores in the database.
   * 
   * Call [FirebaseData object].stringData will return the string value of 
   * the payload returned from the server.
  */
  bool setString(FirebaseData &fbdo, const String &path, const String &stringValue);

  bool set(FirebaseData &fbdo, const String &path, const char *stringValue);

  bool set(FirebaseData &fbdo, const String &path, const String &stringValue);

  /** Set string data and virtual child ".priority" at the defined database path.
  */
  bool setString(FirebaseData &fbdo, const String &path, const String &stringValue, float priority);

  bool set(FirebaseData &fbdo, const String &path, const char *stringValue, float priority);

  bool set(FirebaseData &fbdo, const String &path, const String &stringValue, float priority);

  /** Set string (text) at the defined database path if defined database path's ETag matched the ETag value.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path in which string data will be set.
   * @param stringValue String or text to set.
   * @param ETag Known unique identifier string (ETag) of defined database path.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * 
   * Call [FirebaseData object].stringData will return the string value of 
   * the payload returned from the server. 
   * 
   * If ETag at the defined database path does not match the provided ETag parameter, 
   * the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched). 
   * 
   * If the operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value.
   * Also, call [FirebaseData object].stringData to get the current string value.
   */
  bool setString(FirebaseData &fbdo, const String &path, const String &stringValue, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, const char *stringValue, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, const String &stringValue, const String &ETag);

  /** Set string data and the virtual child ".priority" if defined ETag matches at the defined database path 
  */
  bool setString(FirebaseData &fbdo, const String &path, const String &stringValue, float priority, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, const char *stringValue, float priority, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, const String &stringValue, float priority, const String &ETag);

  /** Set the child node key and value (using FirebaseJson object) to the defined database path. 
   * This will replace any child nodes inside the defined path with node' s key 
   * and value defined in FirebaseJson object.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which key and value in FirebaseJson object will be replaced or set.
   * @param json The FirebaseJson object.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * 
   * Call [FirebaseData object].jsonData will return the JSON string value of payload returned from server.
  */
  bool setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json);

  bool set(FirebaseData &fbdo, const String &path, FirebaseJson &json);

  /** Set JSON data or FirebaseJson object and virtual child ".priority" at the defined database path.
  */

  bool setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority);

  bool set(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority);

  /** Set child node key and value (using JSON data or FirebaseJson object) to the defined database path 
   * if defined database path's ETag matched the ETag value. 
   * This will replace any child nodes inside the defined path with node' s key 
   * and value defined in JSON data or FirebaseJson object.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which key and value in JSON data will be replaced or set.
   * @param json The FirebaseJson object.
   * @param ETag Known unique identifier string (ETag) of defined database path.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * 
   * Call [FirebaseData object].jsonData will return the JSON string value of 
   * payload returned from server. 
   * 
   * If ETag at the defined database path does not match the provided ETag parameter, 
   * the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched). 
   * If the operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value. 
   * Also call [FirebaseData object].jsonData to get the current JSON string value.
  */
  bool setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, FirebaseJson &json, const String &ETag);

  /** Set JSON data or FirebaseJson object and the virtual child ".priority" if defined ETag matches at the defined database path 
  */

  bool setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority, const String &ETag);

  /** Set child node array (using FirebaseJsonArray object) to the defined database path. 
   * This will replace any child nodes inside the defined path with array defined in FirebaseJsonArray object.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which key and value in FirebaseJsonArray object will be replaced or set.
   * @param arr The FirebaseJsonArray object.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data that successfully stores in the database. 
   * 
   * Call [FirebaseData object].jsonArray will return pointer to FirebaseJsonArray object contains array 
   * payload returned from server, get the array payload using FirebaseJsonArray *arr = firebaseData.jsonArray();
  */
  bool setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr);

  bool set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr);

  /** Set FirebaseJsonArray object and virtual child ".priority" at the defined database path.
  */
  bool setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority);

  bool set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority);

  /** Set array (using JSON data or FirebaseJson object) to the defined database path if defined database path's ETag matched the ETag value. 
   * This will replace any child nodes inside the defined path with array defined in FirebaseJsonArray object.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which key and value in JSON data will be replaced or set.
   * @param arr The FirebaseJsonArray object.
   * @param ETag Known unique identifier string (ETag) of defined database path.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * 
   * Call [FirebaseData object].jsonArray will return pointer to FirebaseJsonArray object contains array 
   * payload returned from server, get the array payload using FirebaseJsonArray *arr = firebaseData.jsonArray(); 
   * 
   * If ETag at the defined database path does not match the provided ETag parameter, 
   * the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched). 
   * 
   * If the operation failed due to ETag is not match, call [FirebaseData object].ETag() to get the current ETag value. 
   * Also call [FirebaseData object].jsonArray to get the pointer to FirebaseJsonArray object of current array value.
  */
  bool setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, const String &ETag);

  /** Set FirebaseJsonArray object and the virtual child ".priority" if defined ETag matches at the defined database path 
  */
  bool setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority, const String &ETag);

  /** Set blob (binary data) at the defined database path. 
   * This will replace any child nodes inside the defined path with a blob or binary data.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which binary data will be set.
   * @param blob Byte array of data.
   * @param size Size of the byte array.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note No payload returned from the server.
  */
  bool setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size);

  bool set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size);

  /** Set blob data and virtual child ".priority" at the defined database path.
  */
  bool setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority);

  bool set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority);

  /** Set blob (binary data) at the defined database path if defined database path's ETag matched the ETag value. 
   * This will replace any child nodes inside the defined path with a blob or binary data.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which binary data will be set.
   * @param blob Byte array of data.
   * @param size Size of the byte array.
   * @param ETag Known unique identifier string (ETag) of defined database path.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note No payload returned from the server. 
   * If ETag at the defined database path does not match the provided ETag parameter, 
   * the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).
  */
  bool setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, const String &ETag);

  /** Set blob data and the virtual child ".priority" if defined ETag matches at the defined database path 
  */
  bool setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority, const String &ETag);

  bool set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority, const String &ETag);

  /** Set binary data from the file store on SD card/Flash memory to the defined database path. 
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param storageType Type of storage to read file data, StorageType::FLASH or StorageType::SD.
   * @param path Target database path in which binary data from the file will be set.
   * @param fileName File name included its path in SD card/Flash memory
   * @return Boolean type status indicates the success of the operation. 
   * 
   * The file systems for flash and sd memory can be changed in FirebaseFS.h.
   * 
   * @note No payload returned from the server.
  */
  bool setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName);

  bool set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName);

  /** Set binary data from the file and virtual child ".priority" at the defined database path.
  */
  bool setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority);

  bool set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority);

  /** Set binary data from file stored on SD card/Flash memory to the defined database path if defined database path's ETag matched the ETag value.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param storageType Type of storage to read file data, StorageType::FLASH or StorageType::SD.
   * @param path Target database path in which binary data from the file will be set.
   * @param fileName File name included its path in SD card/Flash memory.
   * @param ETag Known unique identifier string (ETag) of defined database path.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note No payload returned from the server. 
   * 
   * If ETag at the defined database path does not match the provided ETag parameter, 
   * the operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).
   * 
   * The file systems for flash and sd memory can be changed in FirebaseFS.h.
  */
  bool setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, const String &ETag);

  bool set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, const String &ETag);

  /** Set binary data from the file and the virtual child ".priority" if defined ETag matches at the defined database path 
  */
  bool setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority, const String &ETag);

  bool set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority, const String &ETag);

  /** Set Firebase server's timestamp to the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which timestamp will be set.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].intData will return the integer value of timestamp in seconds 
   * or [FirebaseData object].doubleData to get millisecond timestamp. 
   * 
   * Due to bugs in Serial.print in Arduino, to print large double value with zero decimal place, 
   * use printf("%.0lf\n", firebaseData.doubleData());.
  */
  bool setTimestamp(FirebaseData &fbdo, const String &path);

  /** Update the child node key or existing key's value (using FirebaseJson object) under the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which key and value in FirebaseJson object will be updated.
   * @param json The FirebaseJson object used for the update.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * 
   * Call [FirebaseData object].jsonData will return the json string value of payload returned from server. 
   * To reduce network data usage, use updateNodeSilent instead.
  */
  bool updateNode(FirebaseData &fbdo, const String path, FirebaseJson &json);

  /** Update child node key or existing key's value and virtual child ".priority" (using JSON data or FirebaseJson object) under the defined database path.
  */
  bool updateNode(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority);

  /** Update the child node key or existing key's value (using FirebaseJson object) under the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Target database path which key and value in FirebaseJson object will be updated.
   * @param json The FirebaseJson object used for the update.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Owing to the objective of this function to reduce network data usage, 
   * no payload will be returned from the server.
  */
  bool updateNodeSilent(FirebaseData &fbdo, const String &path, FirebaseJson &json);

  /** Update child node key or existing key's value and virtual child ".priority" (using JSON data or FirebaseJson object) under the defined database path.
  */

  bool updateNodeSilent(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority);

  /** Read any type of value at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the float value is being read.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * 
   * Call [FirebaseData object].intData, [FirebaseData object].floatData, [FirebaseData object].doubleData, 
   * [FirebaseData object].boolData, [FirebaseData object].stringData, [FirebaseData object].jsonObject (pointer), 
   * [FirebaseData object].jsonArray (pointer) and [FirebaseData object].blobData corresponded to 
   * its type from [FirebaseData object].dataType.
  */
  bool get(FirebaseData &fbdo, const String &path);

  /** Read the integer value at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the float value is being read.

    @return Boolean type status indicates the success of the operation.

    Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
    
    Call [FirebaseData object].intData will return the integer value of
    payload returned from server.

    If the type of payload returned from server is not integer, float and double, 
    the function [FirebaseData object].intData will return zero (0).

  */
  bool getInt(FirebaseData &fbdo, const String &path);

  /** Read the integer value at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the float value is being read.
   * @param target The integer type variable to store value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note If the type of payload returned from the server is not an integer, float and double, 
   * the target variable's value will be zero (0).
  */
  bool getInt(FirebaseData &fbdo, const String &path, int &target);

  /** Read the float value at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the float value is being read.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database.
   * 
   * Call [FirebaseData object].floatData will return the float value of 
   * payload returned from server. 
   * 
   * If the payload returned from server is not integer, float and double, 
   * the function [FirebaseData object].floatData will return zero (0).
  */
  bool getFloat(FirebaseData &fbdo, const String &path);

  /** Read the float value at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the float value is being read.
   * @param target The float type variable to store value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note If the type of payload returned from the server is not an integer, float and double, 
   * the target variable's value will be zero (0).
  */
  bool getFloat(FirebaseData &fbdo, const String &path, float &target);

  /** Read the double value at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the float value is being read.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * 
   * Call [FirebaseData object].doubleData will return the double value of
   * the payload returned from the server.
   * 
   * If the payload returned from server is not integer, float and double, 
   * the function [FirebaseData object].doubleData will return zero (0).
   * 
   * Due to bugs in Serial.print in Arduino, to print large double value with zero decimal place, 
   * use printf("%.9lf\n", firebaseData.doubleData()); for print value up to 9 decimal places.
  */
  bool getDouble(FirebaseData &fbdo, const String &path);

  /** Read the float value at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the float value is being read.
   * @param target The double type variable to store value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note If the type of payload returned from the server is not an integer, float and double, 
   * the target variable's value will be zero (0).
  */
  bool getDouble(FirebaseData &fbdo, const String &path, double &target);

  /** Read the Boolean value at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the Boolean value is being read.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database. 
   * 
   * Call [FirebaseData object].boolData will return the Boolean value of
   * the payload returned from the server.
   * 
   * If the type of payload returned from the server is not Boolean, 
   * the function [FirebaseData object].boolData will return false.
  */
  bool getBool(FirebaseData &fbdo, const String &path);

  /** Read the Boolean value at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the Boolean value is being read.
   * @param target The boolean type variable to store value.
   *  @return Boolean type status indicates the success of the operation.
   * 
   * @note If the type of payload returned from the server is not Boolean, 
   * the target variable's value will be false.
  */
  bool getBool(FirebaseData &fbdo, const String &path, bool &target);

  /** Read the string at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the string value is being read.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data that successfully
   * stores in the database.
   * 
   * Call [FirebaseData object].stringData will return the string value of
   * the payload returned from the server.
   * 
   * If the type of payload returned from the server is not a string,
   * the function [FirebaseData object].stringData will return empty string (String object).
  */
  bool getString(FirebaseData &fbdo, const String &path);

  /** Read the string at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the string value is being read.
   * @param target The String object to store value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note If the type of payload returned from the server is not a string,
   * the target String object's value will be empty.
  */
  bool getString(FirebaseData &fbdo, const String &path, String &target);

  /** Read the JSON string at the defined database path. 
   * The returned payload JSON string represents the child nodes and their value.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the JSON string value is being read.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data that successfully stores in the database.
   * 
   * Call [FirebaseData object].jsonObject will return the pointer to FirebaseJson object contains the value of
   * the payload returned from the server.
   * 
   * If the type of payload returned from server is not json,
   * the function [FirebaseData object].jsonObject will contain empty object.
  */
  bool getJSON(FirebaseData &fbdo, const String &path);

  /** Read the JSON string at the defined database path. 
   * The returned the pointer to FirebaseJson that contains JSON payload represents the child nodes and their value.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the JSON string value is being read.
   * @param target The FirebaseJson object pointer to get JSON data.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note If the type of payload returned from the server is not JSON,
   * the target FirebaseJson object will contain an empty object.
  */
  bool getJSON(FirebaseData &fbdo, const String &path, FirebaseJson *target);

  /** Read the JSON string at the defined database path. 
   * The returned the pointer to FirebaseJson that contains JSON payload represents the child nodes and their value.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the JSON string value is being read.
   * @param query QueryFilter class to set query parameters to filter data.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Available query parameters for filtering the data are the following.
   * 
   * QueryFilter.orderBy       Required parameter to specify which data used for data filtering included child key, key, and value.
   *                           Use "$key" for filtering data by keys of all nodes at the defined database path.
   *                           Use "$value" for filtering data by value of all nodes at the defined database path.
   *                           Use "$priority" for filtering data by "virtual child" named .priority of all nodes.
   *                           Use any child key to filter by that key.
   * 
   * QueryFilter.limitToFirst  The total children (number) to filter from the first child.
   * QueryFilter.limitToLast   The total last children (number) to filter.
   * QueryFilter.startAt       Starting value of range (number or string) of query upon orderBy param.
   * QueryFilter.endAt         Ending value of range (number or string) of query upon orderBy param.
   * QueryFilter.equalTo       Value (number or string) matches the orderBy param
   * 
   * Call [FirebaseData object].dataType to determine what type of data successfully stores in the database.
   * 
   * Call [FirebaseData object].jsonObject will return the pointer to FirebaseJson object contains the value of
   * the payload returned from the server.
   * 
   * If the type of payload returned from server is not json,
   * the function [FirebaseData object].jsonObject will contain empty object.
  */
  bool getJSON(FirebaseData &fbdo, const String &path, QueryFilter &query);

  /** Read the JSON string at the defined database path as above
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the JSON string value is being read.
   * @param target The FirebaseJson object pointer to get JSON data.
   * @return Boolean type status indicates the success of the operation.
   * 
   * If the type of payload returned from the server is not JSON,
   * the target FirebaseJson object will contain an empty object.
  */
  bool getJSON(FirebaseData &fbdo, const String &path, QueryFilter &query, FirebaseJson *target);

  /** Read the array data at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the array is being read.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data that successfully
   * stores in the database.
   * 
   * Call [FirebaseData object].jsonArray will return the pointer to FirebaseJsonArray object contains array value of
   * payload returned from server.
   * 
   * If the type of payload returned from the server is not an array,
   * the array element in [FirebaseData object].jsonArray will be empty.
  */
  bool getArray(FirebaseData &fbdo, const String &path);

  /** Read the array data at the defined database path, and assign data to the target.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the array is being read.
   * @param target The FirebaseJsonArray object pointer to get array value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note If the type of payload returned from the server is not an array, 
   * the target FirebaseJsonArray object will contain an empty array.
  */
  bool getArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray *target);

  /** Read the array data at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the array is being read.
   * @param query QueryFilter class to set query parameters to filter data.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Available query parameters for filtering the data are the following.
   * 
   * QueryFilter.orderBy       Required parameter to specify which data used for data filtering included child key, key, and value.
   *                           Use "$key" for filtering data by keys of all nodes at the defined database path.
   *                           Use "$value" for filtering data by value of all nodes at the defined database path.
   *                           Use "$priority" for filtering data by "virtual child" named .priority of all nodes.
   *                           Use any child key to filter by that key.
   *                     
   * QueryFilter.limitToFirst  The total children (number) to filter from the first child.
   * QueryFilter.limitToLast   The total last children (number) to filter.
   * QueryFilter.startAt       Starting value of range (number or string) of query upon orderBy param.
   * QueryFilter.endAt         Ending value of range (number or string) of query upon orderBy param.
   * QueryFilter.equalTo       Value (number or string) matches the orderBy param
   * 
   * Call [FirebaseData object].dataType to determine what type of data that successfully 
   * stores in the database.
   * 
   * Call [FirebaseData object].jsonArray will return the pointer to FirebaseJsonArray object contains array of 
   * payload returned from server.
   * 
   * If the type of payload returned from the server is not an array,
   * the function [FirebaseData object].jsonArray will contain empty array.
  */
  bool getArray(FirebaseData &fbdo, const String &path, QueryFilter &query);

  /** Read the array data at the defined database path as above
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the array is being read.
   * @param target The FirebaseJsonArray object to get array value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note If the type of payload returned from the server is not an array,
   * the target FirebaseJsonArray object will contain an empty array.
  */
  bool getArray(FirebaseData &fbdo, const String &path, QueryFilter &query, FirebaseJsonArray *target);

  /** Read the blob (binary data) at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the binary data is being read.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Call [FirebaseData object].dataType to determine what type of data successfully stores in the database.
   * 
   * Call [FirebaseData object].blobData will return the dynamic array of unsigned 8-bit data (i.e. std::vector<uint8_t>) of
   * payload returned from server.
   * 
   * If the type of payload returned from the server is not a blob,
   * the function [FirebaseData object].blobData will return empty array.
  */
  bool getBlob(FirebaseData &fbdo, const String &path);

  /** Read the blob (binary data) at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path in which the binary data is being read.
   * @param target Dynamic array of unsigned 8-bit data (i.e. std::vector<uint8_t>) to store value.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note If the type of payload returned from the server is not a blob, 
   * the target variable value will be an empty array.
  */
  bool getBlob(FirebaseData &fbdo, const String &path, std::vector<uint8_t> &target);

  /** Download file data in a database at the defined database path and save it to SD card/Flash memory. 
   * The downloaded data will be decoded to binary and save to SD card/Flash memory, then please make sure that data at the defined database path is the file type.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param storageType Type of storage to write file data, StorageType::FLASH or StorageType::SD.
   * @param nodePath Database path that file data will be downloaded.
   * @param fileName File name included its path in SD card/Flash memory to save in SD card/Flash memory.
   * @return Boolean type status indicates the success of the operation.
   * 
   * The file systems for flash and sd memory can be changed in FirebaseFS.h.
  */
  bool getFile(FirebaseData &fbdo, uint8_t storageType, const String &nodePath, const String &fileName);

  /** Delete all child nodes at the defined database path.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path to be deleted.
   * @return Boolean type status indicates the success of the operation.
  */
  bool deleteNode(FirebaseData &fbdo, const String &path);

  /** Delete all child nodes at the defined database path if defined database path's ETag matched the ETag value.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path to be deleted.
   * @param ETag Known unique identifier string (ETag) of defined database path.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note If ETag at the defined database path does not match the provided ETag parameter,
   * he operation will fail with HTTP code 412, Precondition Failed (ETag is not matched).
  */
  bool deleteNode(FirebaseData &fbdo, const String &path, const String &ETag);

  /** Start subscribe to the value changes at the defined path and its children.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param path Database path to subscribe.
   * @return Boolean type status indicates the success of the operation.
  */
  bool beginStream(FirebaseData &fbdo, const String &path);

  /** Start subscribe to the value changes at the defined parent node path with multiple nodes paths parsing.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param parentPath Database parent node path to subscribe.
   * @param childPath The string array of child nodes paths for parsing.
   * @param size The size of string array of child nodes paths for parsing.
   * @return Boolean type status indicates the success of the operation.
  */
  bool beginMultiPathStream(FirebaseData &fbdo, const String &parentPath, const String *childPath, size_t size);

  /** Read the stream event data at the defined database path. 
   * Once beginStream was called e.g. in setup(), the readStream function 
   * should call inside the loop function.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Using the same Firebase Data object for stream read/monitoring associated 
   * with getXXX, setXXX, pushXXX, updateNode and deleteNode will break or quit
   * the current stream connection.
   * 
   * he stream will be resumed or reconnected automatically when calling readStream.
  */
  bool readStream(FirebaseData &fbdo);

  /** End the stream connection at a defined path. 
   * It can be restart again by calling beginStream.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @return Boolean type status indicates the success of the operation.
  */
  bool endStream(FirebaseData &fbdo);

  /** Set the stream callback functions.
   * setStreamCallback should be called before Firebase.beginStream.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param dataAvailablecallback a Callback function that accepts streamData parameter.
   * @param timeoutCallback Callback function will be called when the stream connection was timed out (optional).
   * @param streamTaskStackSize - The stream task (RTOS task) reserved stack memory in byte (optional) (8192 is default).
   * 
   * @note dataAvailablecallback will be called When data in the defined path changed or the stream path changed or stream connection 
   * was resumed from getXXX, setXXX, pushXXX, updateNode, deleteNode.
   * 
   * The payload returned from the server will be one of these integer, float, string, JSON and blob types.
   * 
   * Call [streamData object].dataType to determine what type of data successfully stores in the database. 
   * 
   * Call [streamData object].xxxData will return the appropriate data type of 
   * the payload returned from the server.
  */
  void setStreamCallback(FirebaseData &fbdo, FirebaseData::StreamEventCallback dataAvailableCallback, FirebaseData::StreamTimeoutCallback timeoutCallback, size_t streamTaskStackSize = 8192);

  /** Set the multiple paths stream callback functions. 
   * setMultiPathStreamCallback should be called before Firebase.beginMultiPathStream.
   * 
   * @param fbdo - Firebase Data Object to hold data and instance.
   * @param multiPathDataCallback - a Callback function that accepts MultiPathStreamData parameter.
   * @param timeoutCallback - a Callback function will be called when the stream connection was timed out (optional).
   * @param streamTaskStackSize - The stream task (RTOS task) reserved stack memory in byte (optional) (8192 is default).
   * 
   * @note multiPathDataCallback will be called When data in the defined path changed or the stream path changed or stream connection 
   * was resumed from getXXX, setXXX, pushXXX, updateNode, deleteNode.
   * 
   * The payload returned from the server will be one of these integer, float, string and JSON.
   * 
   * Call [MultiPathStreamData object].get to get the child node value, type and data path. 
   * 
   * The properties [MultiPathStreamData object].value, [MultiPathStreamData object].dataPath, and [MultiPathStreamData object].type will return the value, path of data, and type of data respectively.
   * 
   * These properties will store the result from calling the function [MultiPathStreamData object].get.
  */
  void setMultiPathStreamCallback(FirebaseData &fbdo, FirebaseData::MultiPathStreamEventCallback multiPathDataCallback, FirebaseData::StreamTimeoutCallback timeoutCallback, size_t streamTaskStackSize  = 8192);

  /** Remove stream callback functions.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
  */
  void removeStreamCallback(FirebaseData &fbdo);

  /** Remove multiple paths stream callback functions.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
  */
  void removeMultiPathStreamCallback(FirebaseData &fbdo);

  /** Backup (download) database at the defined database path to SD card/Flash memory.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param storageType Type of storage to save file, StorageType::FLASH or StorageType::SD.
   * @param nodePath Database path to be backuped.
   * @param fileName File name to save.
   * @return Boolean type status indicates the success of the operation.
   * 
   * @note Only 8.3 DOS format (max. 8 bytes file name and 3 bytes file extension) can be saved to SD card/Flash memory.
   * 
   * The file systems for flash and sd memory can be changed in FirebaseFS.h.
  */
  bool backup(FirebaseData &fbdo, uint8_t storageType, const String &nodePath, const String &fileName);

  /** Restore database at a defined path using backup file saved on SD card/Flash memory.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param storageType Type of storage to read file, StorageType::FLASH or StorageType::SD.
   * @param nodePath Database path to  be restored.
   * @param fileName File name to read
   * @return Boolean type status indicates the success of the operation.
   * 
   * The file systems for flash and sd memory can be changed in FirebaseFS.h.
  */
  bool restore(FirebaseData &fbdo, uint8_t storageType, const String &nodePath, const String &fileName);

  /** Set maximum Firebase read/store retry operation (0 255) in case of network problems and buffer overflow.
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param num The maximum retry.
  */
  void setMaxRetry(FirebaseData &fbdo, uint8_t num);

  /** Set the maximum Firebase Error Queues in the collection (0 255). 
   * Firebase read/store operation causes by network problems and buffer overflow will be added to Firebase Error Queues collection.
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param num The maximum Firebase Error Queues.
  */
  void setMaxErrorQueue(FirebaseData &fbdo, uint8_t num);

  /** Save Firebase Error Queues as FLASH file (save only database store queues). 
   * Firebase read (get) operation will not be saved.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param filename Filename to be saved.
   * @param storageType Type of storage to save file, StorageType::FLASH or StorageType::SD.
   * 
   * The file systems for flash and sd memory can be changed in FirebaseFS.h.
  */
  bool saveErrorQueue(FirebaseData &fbdo, const String &filename, uint8_t storageType);

  /** Delete file in Flash or SD card.
   * 
   * @param filename File name to delete.
   * @param storageType Type of storage to save file, StorageType::FLASH or StorageType::SD.
   * 
   * The file systems for flash and sd memory can be changed in FirebaseFS.h.
  */
  bool deleteStorageFile(const String &filename, uint8_t storageType);

  /** estore Firebase Error Queues from the FLASH file.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param filename Filename to be read and restore queues.
   * @param storageType Type of storage to read file, StorageType::FLASH or StorageType::SD.
   * 
   * The file systems for flash and sd memory can be changed in FirebaseFS.h.
  */
  bool restoreErrorQueue(FirebaseData &fbdo, const String &filename, uint8_t storageType);

  /** Determine the number of Firebase Error Queues stored in a defined FLASH file.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param filename Filename to be read and count for queues.
   * @param storageType Type of storage to read file, StorageType::FLASH or StorageType::SD.
   * @return Number (0-255) of queues store in defined FLASH file.
   * 
   * The file systems for flash and sd memory can be changed in FirebaseFS.h.
  */
  uint8_t errorQueueCount(FirebaseData &fbdo, const String &filename, uint8_t storageType);

  /** Determine number of queues in Firebase Data object Firebase Error Queues collection.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @return Number (0-255) of queues in Firebase Data object queue collection.
  */
  uint8_t errorQueueCount(FirebaseData &fbdo);

  /** Determine whether the  Firebase Error Queues collection was full or not.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @return Boolean type status indicates whether the  Firebase Error Queues collection was full or not.
  */
  bool isErrorQueueFull(FirebaseData &fbdo);

  /** Process all failed Firebase operation queue items when the network is available.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param callback a Callback function that accepts QueueInfo parameter.
  */
  void processErrorQueue(FirebaseData &fbdo, FirebaseData::QueueInfoCallback callback = NULL);

  /** Return Firebase Error Queue ID of last Firebase Error. 
   * Return 0 if there is no Firebase Error from the last operation.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @return Number of Queue ID.
  */
  uint32_t getErrorQueueID(FirebaseData &fbdo);

  /** Determine whether the Firebase Error Queue currently exists is Error Queue collection or not.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param errorQueueID The Firebase Error Queue ID get from getErrorQueueID.
   * @return Boolean type status indicates the queue existence.
  */
  bool isErrorQueueExisted(FirebaseData &fbdo, uint32_t errorQueueID);

  /** Start the Firebase Error Queues Auto Run Process.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param callback a Callback function that accepts QueueInfo Object as a parameter, optional.
   * @param queueTaskStackSize - The queue error recovery task (RTOS task) reserved stack memory in byte (optional) (8192 is default).
   * 
   * @note The following functions are available from QueueInfo Object accepted by callback.
   * 
   * queueInfo.totalQueues(), get the total Error Queues in Error Queue Collection.
   * 
   * queueInfo.currentQueueID(), get current Error Queue ID that being process.
   * 
   * queueInfo.isQueueFull(), determine whether Error Queue Collection is full or not.
   * 
   * queueInfo.dataType(), get a string of the Firebase call data type that being process of current Error Queue.
   * 
   * queueInfo.method(), get a string of the Firebase call method that being process of current Error Queue.
   * 
   * queueInfo.path(), get a string of the Firebase call path that being process of current Error Queue.
  */
  void beginAutoRunErrorQueue(FirebaseData &fbdo, FirebaseData::QueueInfoCallback callback = NULL, size_t queueTaskStackSize = 8192);

  /** Stop the Firebase Error Queues Auto Run Process.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
  */
  void endAutoRunErrorQueue(FirebaseData &fbdo);

  /** Clear all Firbase Error Queues in Error Queue collection.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
  */
  void clearErrorQueue(FirebaseData &fbdo);

  /** Send Firebase Cloud Messaging to the device with the first registration token which added by firebaseData.fcm.addDeviceToken.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @param index The index (starts from 0) of recipient device token which added by firebaseData.fcm.addDeviceToken
   * @return Boolean type status indicates the success of the operation.
  */
  bool sendMessage(FirebaseData &fbdo, uint16_t index);

  /** Send Firebase Cloud Messaging to all devices (multicast) which added by firebaseData.fcm.addDeviceToken.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @return Boolean type status indicates the success of the operation.
  */
  bool broadcastMessage(FirebaseData &fbdo);

  /** Send Firebase Cloud Messaging to devices that subscribed to the topic.
   * 
   * @param fbdo Firebase Data Object to hold data and instance.
   * @return Boolean type status indicates the success of the operation.
  */
  bool sendTopic(FirebaseData &fbdo);

  /** SD card config with GPIO pins.
     * 
     * @param ss -   SPI Chip/Slave Select pin.
     * @param sck -  SPI Clock pin.
     * @param miso - SPI MISO pin.
     * @param mosi - SPI MOSI pin.
     * @return Boolean type status indicates the success of the operation.
    */
  bool sdBegin(int8_t ss = -1, int8_t sck = -1, int8_t miso = -1, int8_t mosi = -1);

  /** Provide the http code error string
   * 
   * @param httpCode The http code.
   * @param buff The C++ string buffer out.
  */
  void errorToString(int httpCode, std::string &buff);

  template <typename T>
  bool set(FirebaseData &fbdo, const String &path, T value);

  template <typename T>
  bool set(FirebaseData &fbdo, const String &path, T value, size_t size);

  template <typename T>
  bool set(FirebaseData &fbdo, const String &path, T value, float priority);

  template <typename T>
  bool set(FirebaseData &fbdo, const String &path, T value, size_t size, float priority);

  template <typename T>
  bool set(FirebaseData &fbdo, const String &path, T value, const String &ETag);

  template <typename T>
  bool set(FirebaseData &fbdo, const String &path, T value, size_t size, const String &ETag);

  template <typename T>
  bool set(FirebaseData &fbdo, const String &path, T value, float priority, const String &ETag);

  template <typename T>
  bool set(FirebaseData &fbdo, const String &path, T value, size_t size, float priority, const String &ETag);

  template <typename T>
  bool push(FirebaseData &fbdo, const String &path, T value);

  template <typename T>
  bool push(FirebaseData &fbdo, const String &path, T value, size_t size);

  template <typename T>
  bool push(FirebaseData &fbdo, const String &path, T value, float priority);

  template <typename T>
  bool push(FirebaseData &fbdo, const String &path, T value, size_t size, float priority);

private:
  bool handleFCMRequest(FirebaseData &fbdo, fb_esp_fcm_msg_type messageType);
  fb_esp_mem_storage_type getMemStorageType(uint8_t old_type);

  UtilsClass *ut = nullptr;
  FirebaseAuth *_auth = nullptr;
  FirebaseConfig *_cfg = nullptr;
  //internal or used by legacy data
  FirebaseAuth _auth_;
  FirebaseConfig _cfg_;
  
};

extern FirebaseESP32 Firebase;

#endif /* ESP32 */

#endif /* FirebaseESP32_H */