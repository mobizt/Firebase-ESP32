#ifndef Stream_H
#define Stream_H

#include <Arduino.h>
#include <ArduinoJson.h>

#define JSON_BUFFER_STREAM_SIZE 1024
#define JSON_BUFFER_SIZE 1024

class streamResult {
	public:
		streamResult(String eventType, String jsonData) {
			_eventType = eventType;
			StaticJsonBuffer<JSON_BUFFER_STREAM_SIZE> jsonBuffer;
			JsonObject &root = jsonBuffer.parseObject(jsonData);
			if (root.success()) {
				if (root.containsKey("path") && root.containsKey("data")) {
					_path = root["path"].as<String>();
					_data = root["data"].as<String>();
				}
			}
		}
		
		String eventType() {
			return _eventType;
		}
		
		String path() {
			return _path;
		}
		
		int getInt() {
			return _data.toInt();
		}
		
		float getFloat() {
			return _data.toFloat();
		}
		
		String getString() {
			return _data;
		}
		
		float getBool() {
			return _data.indexOf("true") >= 0;
		}
		
		
		JsonVariant getPayloadData() {
			return StaticJsonBuffer<JSON_BUFFER_SIZE>().parseObject(_data);
		}
		
	private:
		String _eventType, _path, _data = "";
		bool _dataError = false, _begin = false;

}
;

#endif