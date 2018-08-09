#ifndef FirebaseESP32_H
#define FirebaseESP32_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Stream.h"

#define JSON_BUFFER_GET_SIZE 1024
#define JSON_BUFFER_PUSH_RETURN_SIZE 512
#define STACK_SIZE_IN_WORD 32768

typedef void(*StreamCallback)(streamResult);
static HTTPClient streamHttpClient;
static WiFiClient *streamWiFiClient;
static StreamCallback stramCallback;
static TaskHandle_t streamTaskHandle;
static String streamUrl = "";
static String streamEventType;
static String streamJsonData;

class FirebaseESP32 {
	public:
		FirebaseESP32();
		void begin(String host);
		void begin(String host, String auth);
		
		
		int getInt(String path);
		float getFloat(String path);
		String getString(String path);
		bool getBool(String path);
		JsonVariant get(String path);

		
		void setInt(String path, int value);
		void setFloat(String path, float value);
		void setString(String path, String value);
		void setBool(String path, bool value);
		void set(String path, int value) ;
		void set(String path, float value) ;
		void set(String path, String value) ;
		void set(String path, bool value) ;
		void set(String path, JsonVariant value) ;		
		
		
		String pushInt(String path, int value);
		String pushFloat(String path, float value);
		String pushBool(String path, bool value);
		String pushString(String path, String value);
		String push(String path, int value) ;
		String push(String path, float value) ;
		String push(String path, String value) ;
		String push(String path, bool value) ;
		String push(String path, JsonVariant value);
		
		
		void stream(String path, StreamCallback callback);
		void stopStream();		
		
		void remove(String path);		
		
		int failed() ;
		String error() ;
		
	private:
		String _host = "", _auth = "";
		int _errCode = 0;
		String _errMsg = "";
		
		String pushJSON(String path, String data);
		String firebaseSendHttpRequest(String url, String type, String payload = "") ;
		String buildRequestedURL(String path);
		
}
;

extern FirebaseESP32 Firebase;

#endif