/*
 * Google's Firebase real time database - ESP32 library for Arduino
 * 
 * This library provides ESP32 to post, get, put and stream data to Google's Firebase database
 * through REST API. 
 * 
 * The library was test and work well with ESP32s based module with no certificate or fingerprint verification.
 * 
 * The MIT License (MIT)
 * Copyright (c) 2018 K. Suwatchai (Mobizt)
 * 
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
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

#ifndef FirebaseESP32_CPP
#define FirebaseESP32_CPP

#if CONFIG_FREERTOS_UNICORE
#define CPU_CORE 0
#else
#define CPU_CORE 1
#endif

#include "FirebaseESP32.h"

FirebaseESP32::FirebaseESP32() {
}

void FirebaseESP32::begin(String host) {
	begin(host, "");
}

void FirebaseESP32::begin(String host, String auth) {
	host.replace("https:", "");
	host.replace("/", "");
	_host = host;
	_auth = auth;
}

int FirebaseESP32::getInt(String path) {
	return firebaseSendHttpRequest(path, "GET").toInt();
}

float FirebaseESP32::getFloat(String path) {
	return firebaseSendHttpRequest(path, "GET").toFloat();
}

String FirebaseESP32::getString(String path) {
	String value = firebaseSendHttpRequest(path, "GET");
	return value.substring(1, value.length() - 1);
}

bool FirebaseESP32::getBool(String path) {
	return firebaseSendHttpRequest(path, "GET").indexOf("true") >= 0;
}

JsonVariant FirebaseESP32::get(String path) {
	return StaticJsonBuffer<JSON_BUFFER_GET_SIZE>().parseObject(firebaseSendHttpRequest(path, "GET"));
}

void FirebaseESP32::setInt(String path, int value) {
	set(path, (int)value);
}

void FirebaseESP32::setFloat(String path, float value) {
	set(path, (float)value);
}

void FirebaseESP32::setString(String path, String value) {
	set(path, (String)value);
}

void FirebaseESP32::setBool(String path, bool value) {
	set(path, (bool)value);
}

void FirebaseESP32::set(String path, int value) {
	firebaseSendHttpRequest(path, "PUT", String(value));
}

void FirebaseESP32::set(String path, float value) {
	firebaseSendHttpRequest(path, "PUT", String(value));
}

void FirebaseESP32::set(String path, String value) {
	String buf = "";
	JsonVariant(value.c_str()).printTo(buf);
	firebaseSendHttpRequest(path, "PUT", buf);
}

void FirebaseESP32::set(String path, bool value) {
	firebaseSendHttpRequest(path, "PUT", value ? "true" : "false");
}

void FirebaseESP32::set(String path, JsonVariant value) {
	String jsonString = "";
	JsonObject& data = value;
	data.printTo(jsonString);
	firebaseSendHttpRequest(path, "PUT", jsonString);
}

String FirebaseESP32::pushInt(String path, int value) {
	return push(path, value);
}

String FirebaseESP32::pushFloat(String path, float value) {
	return push(path, value);
}

String FirebaseESP32::pushBool(String path, bool value) {
	return push(path, value);
}

String FirebaseESP32::pushString(String path, String value) {
	return push(path, value);
}

String FirebaseESP32::push(String path, int value) {
	return pushJSON(path, String(value));
}

String FirebaseESP32::push(String path, float value) {
	return pushJSON(path, String(value));
}

String FirebaseESP32::push(String path, String value) {
	String buf;
	JsonVariant(value.c_str()).printTo(buf);
	return pushJSON(path, buf);
}

String FirebaseESP32::push(String path, bool value) {
	return pushJSON(path, value ? "true" : "false");
}



String FirebaseESP32::push(String path, JsonVariant value) {
	String jsonString = "";
	JsonObject& data = value;
	data.printTo(jsonString);
	return pushJSON(path, jsonString);
}

String FirebaseESP32::pushJSON(String path, String data) {
	String jsonString = firebaseSendHttpRequest(path, "POST", data);
	if (failed()) return String();
	
	StaticJsonBuffer<JSON_BUFFER_PUSH_RETURN_SIZE> jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(jsonString);
	if (!root.success()) {
		_errCode = 1;
		_errMsg = "Error, parsing result";
		return String();
	}
	
	if (!root.containsKey("name")) {
		_errCode = 2;
		_errMsg = "Error, push was unsuccess";
		return String();
	}
	
	return root["name"].as<String>();
}


void FirebaseESP32::stream(String path, StreamCallback callback) {
	stramCallback = callback;
	streamUrl = buildRequestedURL(path);
	
	xTaskCreatePinnedToCore([](void* param) {
		
		
		
		for (;;) {
			
			if (!streamHttpClient.connected()) {
				streamHttpClient.end();
				streamHttpClient.setReuse(true);
				streamHttpClient.begin(streamUrl);
				streamHttpClient.setTimeout(5000);				
				streamHttpClient.addHeader("Accept", "text/event-stream");
				int httpCode = streamHttpClient.GET();
				if (httpCode != HTTP_CODE_OK) {
					Serial.println("Error !, Http stream was failed: " + String(httpCode));
				}else{
					streamWiFiClient = streamHttpClient.getStreamPtr();	
				}							
			}
						
			
			if (!streamWiFiClient) continue;
			
			if (streamWiFiClient->available()) {
				
				String response = streamWiFiClient->readStringUntil('\n');				
			
				if (response.length() == 0) {
					if (stramCallback) stramCallback(streamResult(streamEventType, streamJsonData));
				}else{					
					if (response.startsWith("event:")) {
						streamEventType = response.substring(7, response.length());
						streamEventType.trim();
					} else if (response.startsWith("data:")) {
						streamJsonData = response.substring(6, response.length());
						streamJsonData.trim();
					}
				}
			}
			
			vTaskDelay(10);	
		}
		vTaskDelete(NULL);
		streamTaskHandle = NULL;
	}, "streamTask", STACK_SIZE_IN_WORD, NULL, 3, &streamTaskHandle, CPU_CORE);
	return;
}

void FirebaseESP32::stopStream() {
	if (streamTaskHandle) vTaskDelete(streamTaskHandle);
	if (streamHttpClient.connected()) {
		streamHttpClient.end();
	}
	stramCallback = NULL;
	streamEventType = "";
	streamJsonData = "";
}


void FirebaseESP32::remove(String path) {
	firebaseSendHttpRequest(path, "DELETE");
}

int FirebaseESP32::failed() {
	return _errCode;
}

String FirebaseESP32::error() {
	return _errMsg;
}

String FirebaseESP32::firebaseSendHttpRequest(String path, String type, String payload) {
	HTTPClient http;
	http.setReuse(true);
	http.begin(buildRequestedURL(path));
	int httpCode = http.sendRequest(type.c_str(), payload);
	if (httpCode == HTTP_CODE_OK) {
		_errCode = 0;
		_errMsg = "";
	} else {
		_errCode = httpCode;
		_errMsg = http.errorToString(httpCode);
	}
	String result = http.getString();
	return result == "null" ? "" : result;
}

String FirebaseESP32::buildRequestedURL(String path) {
	String url = "https://" + _host + "/" + path + ".json";
	if (_auth.length() > 0) url += "?auth=" + _auth;
	return url;
}

FirebaseESP32 Firebase = FirebaseESP32();

#endif
