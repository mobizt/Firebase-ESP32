# Firebase Arduino Client Library for ESP32

Google's Firebase real time database - ESP32 library for Arduino

This is not the official Firebase Arduino library from Google, unless this introduces some similar and extra features with 
difference core functions.

This library provides ESP32 to complete working with Google's Firebase database through the REST API. 

The library was test and work well with ESP32s based module. For ESP8266 please try my ESP8266 Arduino library here https://github.com/mobizt/Firebase-ESP8266

Copyright (c) 2019 K. Suwatchai (Mobizt).

## Tested Devices

This library works well in the following tested devices.

 * Sparkfun ESP32 Thing
 * NodeMCU-32
 * WEMOS LOLIN32
 
## Features
Performs Firebase's REST API requests.

Introduces the FirebaseData object that keeps the information and http client to work and reused.

Able to GET, PUT, POST, STREAM, PATCH, DELETE data using get, set, push, stream, update and delete functions.

Supports integer, float, String and raw JSON String data types. Boolean data type was removed due to I implement the feature that can return the actual data type after Firebase calls. You can use int or float data type and use non-zero value for True and zero value for False instead.  For JSON string data type, you need to include the external JSON library like ArduinoJson to parse raw JSON string as the object.

Auto resume streaming when connection was timeout. Call FirebaseData.streamTimeout() to check the stream timeout status.

Able to set multiple stream paths which may use more memory and single stream path which can be switch between stream monitoring call and other set/get/push calls in the same Loop function or in difference CPU core tasks functions using the same or difference FirebaseData object.  For multi-tasking, while using the same FirebaseData object, the current Firebase call will force the previous unfinished Firebase call to stop. The normal get/set/push/update/delete calls will have the same priority and have higher priority than stream event call.

Able to set stream calback and remove stream callback functions.

Able to pause all Firebase calls to perform other http connections.

## Dependencies
Required HTTPClientESP32Ex library to be installed. The library can be found here https://github.com/mobizt/HTTPClientESP32Ex

Once the implementation of Firebase Arduino library using WiFiClientSecure library works very well for ESP8266 but make the unreliable operation in ESP32 when connection was sucessfully made for few minutes and then always failed in SSL handshake which mbedTLS returns some random errors even root certificate was provided, then customized http client library HTTPClientESP32Ex will be used in this case.


## Supported functions

```c++
   /**
   * The Firebase's credentials initialization.
   * \param host - Your Firebase database project host i.e. Your_ProjectID.firebaseio.com.
   * \param auth - Your database secret.
   */
    void begin(const String host, const String auth);
	
   /**
   * The Firebase's credentials initialization.
   * \param host - Your Firebase database project host i.e. Your_ProjectID.firebaseio.com.
   * \param auth - Your database secret.
   * \param rootCA - Base64 encoded root certificate string.
   */
    void begin(const String host, const String auth, const char* rootCA);
	
   /**
   * Auto reconnect WiFi when connection lost during request.
   * \param reconnect - True for auto reconnect.
   */
    void reconnectWiFi(bool reconnect);
	
   /**
   * Check for the existence of node path.
   * \param dataObj - FirebaseData object that requred for internal works.
   * \param path - Node path to be checked.
   */
    bool pathExist(FirebaseData &dataObj, const String path);
	
   /**
   * Post or append/create new child node integer data.
   * \param dataObj - FirebaseData object that requred for internal works.
   * \param path - Parent node path to be append.
   * \param intValue - Integer value to be append.
   * \return The operating status. True for success operation and False for failed operation.
   * To get new push node name, call FirebaseData.pushName().
   * To get error reason, call FirebaseData.errorReason().
   * To get actual data type, call FirebaseData.dataType().
   */
    bool pushInt(FirebaseData &dataObj, const String path, int intValue);
	
   /**
   * Post or append/create new child node float data.
   * \param dataObj - FirebaseData object that requred for internal works.
   * \param path - Parent node path to be append.
   * \param floatValue - Float value to be append.
   * \return The operating status. TTrue for success operation and False for failed operation.
   * To get new push node name, call FirebaseData.pushName().
   * To get error reason, call FirebaseData.errorReason().
   * To get actual data type, call FirebaseData.dataType().
   */
    bool pushFloat(FirebaseData &dataObj, const String path, float floatValue);
	
   /**
   * Post or append/create new child node String data.
   * \param dataObj - FirebaseData object that requred for internal works.
   * \param path - Parent node path to be append.
   * \param StringValue - String value to be append.
   * \return The operating status. True for success operation and False for failed operation.
   * To get new push node name, call FirebaseData.pushName().
   * To get error reason, call FirebaseData.errorReason().
   * To get actual data type, call FirebaseData.dataType().
   */
    bool pushString(FirebaseData &dataObj, const String path, const String stringValue);
	
   /**
   * Post or append/create new node JSON data.
   * \param dataObj - FirebaseData object that requred for internal works.
   * \param path - Parent node path to be append.
   * \param jsonString - Raw JSON String to be append (should be correct JSONformat).
   * \return The operating status. True for success operation and False for failed operation.
   * To get new push node name, call FirebaseData.pushName().
   * To get error reason, call FirebaseData.errorReason().
   * To get actual data type, call FirebaseData.dataType().
   */
    bool pushJSON(FirebaseData &dataObj, const String path, const String jsonString);
	
   /**
   * Put or set integer data to child node at the specified path.
   * \param dataObj - FirebaseData object that requred for internal works.
   * \param path - Node path to be set value.
   * \param intValue - Integer value to be set.
   * \return The operating status. True for success operation and False for failed operation.
   * To get error reason, call FirebaseData.errorReason().
   * To get actual data type, call FirebaseData.dataType().
   * To get actual payload data from operation, call FirebaseData.intData().
   */
    bool setInt(FirebaseData &dataObj, const String path, int intValue);
	
   /**
   * Put or set float data to node at the specified path.
   * \param dataObj - FirebaseData object that requred for internal works.
   * \param path - Node path to be set value.
   * \param floatValue - Float value to be set.
   * \return The operating status. True for success operation and False for failed operation.
   * To get error reason, call FirebaseData.errorReason().
   * To get actual data type, call FirebaseData.dataType().
   * To get actual payload data from operation, call FirebaseData.floatData().
   */
    bool setFloat(FirebaseData &dataObj, const String path, float floatValue);
	
   /**
   * Put or set String data to child node at the specified path.
   * \param dataObj - FirebaseData object that requred for internal works.
   * \param path - Node path to be set value.
   * \param stringValue - String value to be set.
   * \return The operating status. True for success operation and False for failed operation.
   * To get error reason, call FirebaseData.errorReason().
   * To get actual data type, call FirebaseData.dataType().
   * To get actual payload data from operation, call FirebaseData.stringData().
   */
    bool setString(FirebaseData &dataObj, const String path, const String stringValue);
	
   /**
   * Put or set JSON data to child node at the specified path.
   * \param dataObj - FirebaseData object that requred for internal works.
   * \param path - Node path to be set value.
   * \param jsonString - Raw JSON string to be set (should be correct JSONformat).
   * \return The operating status. True for success operation and False for failed operation.
   * To get error reason, call FirebaseData.errorReason().
   * To get actual data type, call FirebaseData.dataType().
   * To get actual payload data from operation, call FirebaseData.jsonData().
   */
    bool setJSON(FirebaseData &dataObj, const String path, const String jsonString);
	
   /**
   * Patch or update JSON data to child node at the specified path.
   * \param dataObj - FirebaseData object that requred for internal works.
   * \param path - Node path to be update the value.
   * \param jsonString - Raw JSON string to be update (should be correct JSONformat contains key/value pairs).
   * \return The operating status. True for success operation and False for failed operation.
   * To get error reason, call FirebaseData.errorReason().
   * To get actual data type, call FirebaseData.dataType().
   * To get actual payload data from operation, call FirebaseData.jsonData().
   */
    bool updateNode(FirebaseData &dataObj, const String path, const String jsonString);
	
   /**
   * Get the integer data from child node at the specified path.
   * \param dataObj - FirebaseData object that requred for internal works.
   * \param path - Node path to be read the value.
   * \return The operating status. True for success operation and False for failed operation.
   * To get error reason, call FirebaseData.errorReason().
   * To get actual data type, call FirebaseData.dataType().
   * To get actual return integer data, call FirebaseData.intData(). If the data at node path is float type, 
   * the return data will be rounded  and if the data at node path is other types than integer or float, 
   * the return integer data will be zero.
   */
    bool getInt(FirebaseData &dataObj, String path);
	
   /**
   * Get the float data from child node at the specified path.
   * \param dataObj - FirebaseData object that requred for internal works.
   * \param path - Node path to be read the value.
   * \return The operating status. True for success operation and False for failed operation.
   * To get error reason, call FirebaseData.errorReason().
   * To get actual data type, call FirebaseData.dataType().
   * To get actual return float data, call FirebaseData.floatData(). If the data at node path is other types than float or integer, 
   * the return float data will be zero.
   */
    bool getFloat(FirebaseData &dataObj, String path);
	
   /**
   * Get the String data from child node at the specified path.
   * \param dataObj - FirebaseData object that requred for internal works.
   * \param path - Node path to be read the value.
   * \return The operating status. True for success operation and False for failed operation.
   * To get error reason, call FirebaseData.errorReason().
   * To get actual data type, call FirebaseData.dataType().
   * To get actual return String data, call FirebaseData.stringData(). If the data at node path is other types than String, 
   * the return String data will be empty string.
   */
    bool getString(FirebaseData &dataObj, String path);
	
   /**
   * Get the raw JSON String data from child node at the specified path.
   * \param dataObj - FirebaseData object that requred for internal works.
   * \param path - Node path to be read the value.
   * \return The operating status. True for success operation and False for failed operation.
   * To get error reason, call FirebaseData.errorReason().
   * To get actual data type, call FirebaseData.dataType().
   * To get actual return raw JSON String data, call FirebaseData.jsonData(). If the data at node path is other types than JSON, 
   * the return String data will be empty string.
   */
    bool getJSON(FirebaseData &dataObj, String path);
	
   /**
   * Delete the node at specified path. The entire children nodes also deleted.
   * \param dataObj - FirebaseData object that requred for internal works.
   * \param path - Node path to be deleted.
   * \return The operating status. True for success operation and False for failed operation.
   * To get error reason, call FirebaseData.errorReason().
   */
    bool deleteNode(FirebaseData &dataObj, const String path);
	
   /**
   * Begin monitoring the change of data at node and its children at specified path.
   * \param dataObj - FirebaseData object that requred for internal works.
   * \param path - Parent node path to be monitor.
   * \return The operating status. True for success operation and False for failed operation.
   * To get error reason, call FirebaseData.errorReason().
   */
    bool beginStream(FirebaseData &dataObj, const String path);
	
   /**
   * Read the stream event data at specified path. Should be called in loop or loop tasks after begin stream called in setup().
   * \param dataObj - FirebaseData object that requred for internal works.
   * \return The operating status. True for success operation and False for failed operation.
   * To get error reason, call FirebaseData.errorReason().
   */
    bool readStream(FirebaseData &dataObj);
	
   /**
   * End the stream operation at specified path. Can be start stream again by call beginStream once.
   * \param dataObj - FirebaseData object that requred for internal works.
   * \return The operating status. True for success operation and False for failed operation.
   * To get error reason, call FirebaseData.errorReason().
   */
    bool endStream(FirebaseData &dataObj);
    
   /**
   * Set the stream callback function to FirebaseData object when new event stream data is available or 
   * the stream event call parameter was changes (stream path or switch from get/set/push/update/delete call).
   * \param dataObj - FirebaseData object that requred for internal works.
   * \param dataAvailablecallback - Callback function that get streamData parameter when stream data are available.
   * \param timeoutCallback - Callback function that call when stream connection is timeout (optional).
   * To get the stream data call streamData.intData(), streamData.floatData, streamData.stringData, streamData.jsonData.
   * To get actual data type from stream call streamData.dataType which returns int, float, string or json String.
   * setStreamCallback can call before or after Firebase.beginStream.
   */
   void setStreamCallback(FirebaseData &dataObj, StreamEventCallback dataAvailablecallback, StreamTimeoutCallback timeoutCallback = NULL);


  /**
   * Remove stream callback function from FirebaseData object.
   * \param dataObj - FirebaseData object that requred for internal works.
   */
   void removeStreamCallback(FirebaseData &dataObj);
	


 For FirebaseData object call

	
   /**
   * Get the WiFi ssl client pointer.
   */
    WiFiClient* getHTTPClient();
	
   /**
   * Return the actual data type that return as payload from get/set/push calls.
   * \return The data type String (int, float, string and json).
   */
    String dataType();
	
   /**
   * Return the current stream path.
   * \return The path String.
   */
    String streamPath();
	
   /**
   * Return the current data path.
   * \return The current path String from get/set/push calls or current changed data path from stream call.
   */
    String dataPath();
	
   /**
   * Return the error reason String from current call.
   * \return The error String.
   */
    String errorReason();
	
   /**
   * Return the ineger data from current get/set/push and stream calls.
   * \return The payload or returned integer data.
   */
    int intData();
	
   /**
   * Return the float data from current get/set/push and stream calls.
   * \return The payload or returned float data.
   */
    float floatData();
	
   /**
   * Return the String data from current get/set/push and stream calls.
   * \return The payload or returned String data.
   */
    String stringData();
	
   /**
   * Return the raw JSON String data from current get/set/push and stream calls.
   * \return The payload or returned raw JSON String data.
   */
    String jsonData();
	
   /**
   * Return the new created child node name (String) from push call.
   * \return The String of new append node name.
   */
    String pushName();
	
   /**
   * Check the current FirebaseData object is currently work with stream.
   * \return The status. True for being stream.
   */
    bool isStream();
	
   /**
   * Check the current FirebaseData object is currently connected to server.
   * \return The connected status. True for connected.
   */
    bool httpConnected();
	
   /**
   * Check the current stream connection of FirebaseData object is timeout from server (30 sec is default).
   * Nothing to do when timeout, the stream is automatic resume.
   * \return The timeout status. True for timeout occurred.
   */
    bool streamTimeout();
	
   /**
   * Check the data is available from get/set/push calls.
   * \return The data available status. True for data is available.
   */
    bool dataAvailable();
	
   /**
   * Check the update/changes data is available from current stream  of FirebaseData object.
   * \return The stream data available status. True for stream data is available.
   */
    bool streamAvailable();
	
   /**
   * Check the matching between data type being call and the server return data type.
   * \return The mismatch status. True for mismatch.
   */
    bool mismatchDataType();
	
   /**
   * Check the http code return from all calls.
   * \return The http status code from Firebase's call response.
   */
    int httpCode();
	
   /**
   * Check overflow of the return data buffer.
   * \return The overflow status. True for overflow. If overflow change the defined value in FirebaseESP32.h
   * #define FIREBASE_RESPONSE_SIZE xxxx
   * #define FIREBASE_DATA_SIZE xxxx
   */
   bool bufferOverflow();

```
## Installing

Click on Clone or download dropdown at the top of repository, select Download ZIP and save zip file to your computer.

From Arduino IDE, goto menu Sketch -> Include Library -> Add .ZIP Library... and choose Firebase-ESP32-master.zip from last download.

Go to menu Files -> Examples -> Firebase-ESP32-master and choose one from examples

## Usages

Recommend to call updateNode instead of push which has more convenient when you need to assign your own parent node key/name yourself instead of Firebase generated random key/name from push call.

Just assign the path parameter that never existed to updateNode call, Firebase will create child nodes under that assigned path automatically.

See the examples folder for the usage sketch.

