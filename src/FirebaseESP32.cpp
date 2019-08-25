/*
 * Google's Firebase Realtime Database Arduino Library for ESP32, version 3.2.0
 * 
 * August 25, 2019
 * 
 * Feature Added:
 * - Add JSON builder and parser.
 * - Add support SPIFFS for backup and restore, file set and get.
 * 
 * Feature Fixed:
 * - Fixed pointer unhandle error.
 * - Fixed Memory error.
 * 
 * 
 * This library provides ESP32 to perform REST API by GET PUT, POST, PATCH, DELETE data from/to with Google's Firebase database using get, set, update
 * and delete calls. 
 * 
 * The library was test and work well with ESP32s based module and add support for multiple stream event path.
 * 
 * The MIT License (MIT)
 * Copyright (c) 2019 K. Suwatchai (Mobizt)
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

#ifndef FirebaseESP32_CPP
#define FirebaseESP32_CPP

#include "FirebaseESP32.h"

struct FirebaseESP32::FirebaseDataType
{
  static const uint8_t NULL_ = 1;
  static const uint8_t INTEGER = 2;
  static const uint8_t FLOAT = 3;
  static const uint8_t DOUBLE = 4;
  static const uint8_t BOOLEAN = 5;
  static const uint8_t STRING = 6;
  static const uint8_t JSON = 7;
  static const uint8_t BLOB = 8;
  static const uint8_t FILE = 9;
  static const uint8_t TIMESTAMP = 10;
  static const uint8_t SHALLOW = 11;
};

struct FirebaseESP32::FirebaseMethod
{
  static const uint8_t PUT = 0;
  static const uint8_t PUT_SILENT = 1;
  static const uint8_t POST = 2;
  static const uint8_t GET = 3;
  static const uint8_t GET_SILENT = 4;
  static const uint8_t STREAM = 5;
  static const uint8_t PATCH = 6;
  static const uint8_t PATCH_SILENT = 7;
  static const uint8_t DELETE = 8;
  static const uint8_t BACKUP = 9;
  static const uint8_t RESTORE = 10;
  static const uint8_t GET_RULES = 11;
  static const uint8_t SET_RULES = 12;
  static const uint8_t GET_SHALLOW = 13;
  static const uint8_t GET_PRIORITY = 14;
  static const uint8_t SET_PRIORITY = 15;
};

struct FirebaseESP32::FCMMessageType
{
  static const uint8_t SINGLE = 0;
  static const uint8_t MULTICAST = 1;
  static const uint8_t TOPIC = 2;
};

FirebaseESP32::FirebaseESP32()
{
}

FirebaseESP32::~FirebaseESP32()
{
  std::string().swap(_host);
  std::string().swap(_auth);
  _rootCA = nullptr;
}

void FirebaseESP32::begin(const String &host, const String &auth)
{
  int p1 = std::string::npos;
  int p2 = 0;
  char *h = new char[host.length() + 1];
  memset(h, 0, host.length() + 1);
  strcpy(h, host.c_str());
  char *_h = new char[host.length() + 1];
  memset(_h, 0, host.length() + 1);

  _host.clear();
  _auth.clear();
  _rootCAFile.clear();
  _rootCA = nullptr;

  _host = host.c_str();

  p1 = _host.find(ESP32_FIREBASE_STR_111);
  if (p1 != std::string::npos)
  {
    if (h[strlen(h) - 1] == '/')
      p2 = 1;

    strncpy(_h, h + p1 + strlen(ESP32_FIREBASE_STR_111), strlen(h) - p1 - p2 - strlen(ESP32_FIREBASE_STR_111));
    _host = _h;
  }

  if (p1 == std::string::npos)
  {
    p1 = _host.find(ESP32_FIREBASE_STR_112);
    if (p1 != std::string::npos)
    {
      if (h[strlen(h) - 1] == '/')
        p2 = 1;

      strncpy(_h, h + p1 + strlen(ESP32_FIREBASE_STR_112), strlen(h) - p1 - p2 - strlen(ESP32_FIREBASE_STR_112));
      _host = _h;
    }
  }

  _auth = auth.c_str();
  _port = FIEBASE_PORT;

  delete[] h;
  delete[] _h;
}

void FirebaseESP32::begin(const String &host, const String &auth, const char *rootCA)
{
  begin(host, auth);
  if (rootCA)
  {
    _rootCA = std::shared_ptr<const char>(rootCA);
  }
}

void FirebaseESP32::begin(const String &host, const String &auth, const String &rootCAFile, uint8_t storageType)
{
  begin(host, auth);
  if (rootCAFile.length() > 0)
  {

    _rootCAFile = rootCAFile.c_str();
    _rootCAFileStoreageType = storageType;
    if (storageType == StorageType::SD && !_sdOk)
      _sdOk = sdTest();
  }
}

void FirebaseESP32::end(FirebaseData &dataObj)
{

  endStream(dataObj);

  removeStreamCallback(dataObj);

  dataObj.clear();
}

void FirebaseESP32::reconnectWiFi(bool reconnect)
{
  _reconnectWiFi = reconnect;
  WiFi.setAutoReconnect(reconnect);
}

void FirebaseESP32::setReadTimeout(FirebaseData &dataObj, int millisec)
{
  if (millisec <= 900000)
    dataObj._readTimeout = millisec;
}

void FirebaseESP32::setwriteSizeLimit(FirebaseData &dataObj, const String &size)
{
  dataObj._writeLimit = size.c_str();
}

bool FirebaseESP32::getRules(FirebaseData &dataObj)
{
  dataObj.queryFilter.clear();
  return sendRequest(dataObj, 0, ESP32_FIREBASE_STR_103, FirebaseMethod::GET_RULES, FirebaseDataType::JSON, "", "", "");
}

bool FirebaseESP32::setRules(FirebaseData &dataObj, const String &rules)
{
  dataObj.queryFilter.clear();
  return sendRequest(dataObj, 0, ESP32_FIREBASE_STR_103, FirebaseMethod::SET_RULES, FirebaseDataType::JSON, rules.c_str(), "", "");
}

bool FirebaseESP32::buildRequest(FirebaseData &dataObj, uint8_t firebaseMethod, uint8_t firebaseDataType, const std::string &path, const char *buff, bool queue, const std::string &priority, const std::string &etag)
{

  bool flag;
  dataObj.queryFilter.clear();

  uint8_t errCount = 0;
  uint8_t maxRetry = dataObj._maxRetry;
  if (maxRetry == 0)
    maxRetry = 1;

  unsigned long lastTime = millis();

  if (_reconnectWiFi && WiFi.status() != WL_CONNECTED)
    WiFi.reconnect();

  if (dataObj._streamCall || dataObj._cfmCall)
    while ((dataObj._streamCall || dataObj._cfmCall) && millis() - lastTime < 1000)
      delay(1);

  if (dataObj._streamCall || dataObj._cfmCall)
  {
    dataObj._httpCode = HTTPC_ERROR_CONNECTION_INUSED;
    return false;
  }

  for (int i = 0; i < maxRetry; i++)
  {
    flag = sendRequest(dataObj, 0, path.c_str(), firebaseMethod, firebaseDataType, buff, priority, etag);
    if (flag)
      break;

    if (dataObj._httpCode == HTTPC_ERROR_CONNECTION_INUSED && dataObj._firebaseCall)
    {
      //Waiting for queue to finish
      while (dataObj._firebaseCall && WiFi.status() != WL_CONNECTED)
        delay(1);
    }
    else
    {

      if (dataObj._maxRetry > 0)
        if (!flag && commError(dataObj))
          errCount++;
    }
  }

  dataObj._qID = 0;

  if (!queue && !flag && errCount == maxRetry && dataObj._qMan._maxQueue > 0)
    if (firebaseMethod == FirebaseMethod::PUT || firebaseMethod == FirebaseMethod::PUT_SILENT || firebaseMethod == FirebaseMethod::POST || firebaseMethod == FirebaseMethod::PATCH || firebaseMethod == FirebaseMethod::PATCH_SILENT)
      dataObj.addQueue(firebaseMethod, 0, firebaseDataType, path.c_str(), "", buff, false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

  if (flag)
    dataObj._firebaseCall = false;
  return flag;
}

bool FirebaseESP32::buildRequestFile(FirebaseData &dataObj, uint8_t storageType, uint8_t firebaseMethod, const std::string &path, const std::string &fileName, bool queue, const std::string &priority, const std::string &etag)
{

  dataObj.queryFilter.clear();
  dataObj._fileName = fileName.c_str();

  bool flag;

  uint8_t errCount = 0;
  uint8_t maxRetry = dataObj._maxRetry;
  if (maxRetry == 0)
    maxRetry = 1;

  unsigned long lastTime = millis();

  if (_reconnectWiFi && WiFi.status() != WL_CONNECTED)
    WiFi.reconnect();

  if (dataObj._streamCall || dataObj._cfmCall)
    while ((dataObj._streamCall || dataObj._cfmCall) && millis() - lastTime < 1000)
      delay(1);

  if (dataObj._streamCall || dataObj._cfmCall)
  {
    dataObj._httpCode = HTTPC_ERROR_CONNECTION_INUSED;
    return false;
  }

  for (int i = 0; i < maxRetry; i++)
  {

    flag = sendRequest(dataObj, storageType, path.c_str(), firebaseMethod, FirebaseDataType::FILE, "", priority, etag);
    if (flag)
      break;

    if (dataObj._httpCode == HTTPC_ERROR_CONNECTION_INUSED && dataObj._firebaseCall)
    {
      //Waiting for queue to finish
      while (dataObj._firebaseCall && WiFi.status() != WL_CONNECTED)
        delay(1);
    }
    else
    {

      if (dataObj._maxRetry > 0)
        if (!flag && commError(dataObj))
          errCount++;
    }
  }

  dataObj._qID = 0;

  if (!queue && !flag && errCount == maxRetry && dataObj._qMan._maxQueue > 0)
    dataObj.addQueue(firebaseMethod, storageType, FirebaseDataType::FILE, path.c_str(), fileName.c_str(), "", false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

  if (flag)
    dataObj._firebaseCall = false;

  return flag;
}

bool FirebaseESP32::pathExist(FirebaseData &dataObj, const String &path)
{
  dataObj.queryFilter.clear();
  if (sendRequest(dataObj, 0, path.c_str(), FirebaseMethod::GET_SILENT, FirebaseDataType::STRING, "", "", ""))
    return !dataObj._pathNotExist;
  else
    return false;
}

String FirebaseESP32::getETag(FirebaseData &dataObj, const String &path)
{
  dataObj.queryFilter.clear();
  if (sendRequest(dataObj, 0, path.c_str(), FirebaseMethod::GET_SILENT, FirebaseDataType::STRING, "", "", ""))
    return dataObj._etag.c_str();
  else
    return String();
}

bool FirebaseESP32::getShallowData(FirebaseData &dataObj, const String &path)
{
  bool flag = false;
  flag = sendRequest(dataObj, 0, path.c_str(), FirebaseMethod::GET_SHALLOW, FirebaseDataType::STRING, "", "", "");
  return flag;
}

void FirebaseESP32::enableClassicRequest(FirebaseData &dataObj, bool flag)
{
  dataObj._classicRequest = flag;
}

bool FirebaseESP32::setPriority(FirebaseData &dataObj, const String &path, float priority)
{
  char *num = new char[20];
  memset(num, 0, 20);
  dtostrf(priority, 7, 6, num);
  char *p = new char[path.length() + strlen_P(ESP32_FIREBASE_STR_156) + 1];
  strcpy(p, path.c_str());
  strcat_P(p, ESP32_FIREBASE_STR_156);
  bool flag = buildRequest(dataObj, FirebaseMethod::SET_PRIORITY, FirebaseDataType::FLOAT, p, num, false, "", "");
  delete[] num;
  delete[] p;
  return flag;
}

bool FirebaseESP32::getPriority(FirebaseData &dataObj, const String &path)
{
  char *p = new char[path.length() + strlen_P(ESP32_FIREBASE_STR_156) + 1];
  strcpy(p, path.c_str());
  strcat_P(p, ESP32_FIREBASE_STR_156);
  bool flag = buildRequest(dataObj, FirebaseMethod::GET_PRIORITY, FirebaseDataType::FLOAT, p, "", false, "", "");
  delete[] p;
  return flag;
}

bool FirebaseESP32::pushInt(FirebaseData &dataObj, const String &path, int intValue)
{
  return pushInt(dataObj, path.c_str(), intValue, false, "");
}

bool FirebaseESP32::pushInt(FirebaseData &dataObj, const String &path, int intValue, float priority)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = pushInt(dataObj, path.c_str(), intValue, false, "");
  delete[] buff;
  return flag;
}

bool FirebaseESP32::pushInt(FirebaseData &dataObj, const std::string &path, int intValue, bool queue, const std::string &priority)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  itoa(intValue, buff, 10);
  bool flag = buildRequest(dataObj, FirebaseMethod::POST, FirebaseDataType::INTEGER, path, buff, queue, priority, "");
  delete[] buff;
  return flag;
}

bool FirebaseESP32::pushFloat(FirebaseData &dataObj, const String &path, float floatValue)
{
  return pushInt(dataObj, path.c_str(), floatValue, false, "");
}

bool FirebaseESP32::pushFloat(FirebaseData &dataObj, const String &path, float floatValue, float priority)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = pushFloat(dataObj, path.c_str(), floatValue, false, buff);
  delete[] buff;
  return flag;
}

bool FirebaseESP32::pushFloat(FirebaseData &dataObj, const std::string &path, float floatValue, bool queue, const std::string &priority)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(floatValue, 7, 6, buff);
  bool flag = buildRequest(dataObj, FirebaseMethod::POST, FirebaseDataType::FLOAT, path, buff, queue, priority, "");
  delete[] buff;
  return flag;
}

bool FirebaseESP32::pushDouble(FirebaseData &dataObj, const String &path, double doubleValue)
{
  return pushDouble(dataObj, path.c_str(), doubleValue, false, "");
}

bool FirebaseESP32::pushDouble(FirebaseData &dataObj, const String &path, double doubleValue, float priority)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = pushDouble(dataObj, path.c_str(), doubleValue, false, buff);
  delete[] buff;
  return flag;
}

bool FirebaseESP32::pushDouble(FirebaseData &dataObj, const std::string &path, double doubleValue, bool queue, const std::string &priority)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(doubleValue, 12, 9, buff);
  bool flag = buildRequest(dataObj, FirebaseMethod::POST, FirebaseDataType::DOUBLE, path, buff, queue, priority, "");
  delete[] buff;
  return flag;
}

bool FirebaseESP32::pushBool(FirebaseData &dataObj, const String &path, bool boolValue)
{
  return pushBool(dataObj, path.c_str(), boolValue, false, "");
}

bool FirebaseESP32::pushBool(FirebaseData &dataObj, const String &path, bool boolValue, float priority)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = pushBool(dataObj, path.c_str(), boolValue, false, buff);
  delete[] buff;
  return flag;
}

bool FirebaseESP32::pushBool(FirebaseData &dataObj, const std::string &path, bool boolValue, bool queue, const std::string &priority)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);

  if (boolValue)
    strcpy(buff, ESP32_FIREBASE_STR_107);
  else
    strcpy(buff, ESP32_FIREBASE_STR_106);

  bool flag = buildRequest(dataObj, FirebaseMethod::POST, FirebaseDataType::BOOLEAN, path, buff, queue, priority, "");
  delete[] buff;
  return flag;
}

bool FirebaseESP32::pushString(FirebaseData &dataObj, const String &path, const String &stringValue)
{
  bool flag = buildRequest(dataObj, FirebaseMethod::POST, FirebaseDataType::STRING, path.c_str(), stringValue.c_str(), false, "", "");
  return flag;
}

bool FirebaseESP32::pushString(FirebaseData &dataObj, const String &path, const String &stringValue, float priority)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = buildRequest(dataObj, FirebaseMethod::POST, FirebaseDataType::STRING, path.c_str(), stringValue.c_str(), false, buff, "");
  delete[] buff;
  return flag;
}

bool FirebaseESP32::pushJSON(FirebaseData &dataObj, const String &path, const String &jsonString)
{
  bool flag = buildRequest(dataObj, FirebaseMethod::POST, FirebaseDataType::JSON, path.c_str(), jsonString.c_str(), false, "", "");
  return flag;
}

bool FirebaseESP32::pushJSON(FirebaseData &dataObj, const String &path, FirebaseJson &json)
{
  bool flag = buildRequest(dataObj, FirebaseMethod::POST, FirebaseDataType::JSON, path.c_str(), json.toStdString().c_str(), false, "", "");
  return flag;
}

bool FirebaseESP32::pushJSON(FirebaseData &dataObj, const String &path, const String &jsonString, float priority)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = buildRequest(dataObj, FirebaseMethod::POST, FirebaseDataType::JSON, path.c_str(), jsonString.c_str(), false, buff, "");
  delete[] buff;
  return flag;
}

bool FirebaseESP32::pushJSON(FirebaseData &dataObj, const String &path, FirebaseJson &json, float priority)
{
  return pushJSON(dataObj, path, json.toStdString().c_str(), priority);
}

bool FirebaseESP32::pushBlob(FirebaseData &dataObj, const String &path, uint8_t *blob, size_t size)
{
  return pushBlob(dataObj, path.c_str(), blob, size, false, "");
}

bool FirebaseESP32::pushBlob(FirebaseData &dataObj, const String &path, uint8_t *blob, size_t size, float priority)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = pushBlob(dataObj, path.c_str(), blob, size, false, buff);
  delete[] buff;
  return flag;
}

bool FirebaseESP32::pushBlob(FirebaseData &dataObj, const std::string &path, uint8_t *blob, size_t size, bool queue, const std::string &priority)
{
  if (dataObj._maxBlobSize < size)
  {
    dataObj._httpCode = HTTP_CODE_PAYLOAD_TOO_LARGE;
    return false;
  }

  size_t tmpSize = strlen(ESP32_FIREBASE_STR_92) + 1;
  char *tmp = new char[tmpSize];
  memset(tmp, 0, tmpSize);

  std::string blobStr = "";
  strcpy_P(tmp, ESP32_FIREBASE_STR_92);
  blobStr = tmp;
  blobStr += base64_encode_string((const unsigned char *)blob, size);
  memset(tmp, 0, tmpSize);
  strcpy_P(tmp, ESP32_FIREBASE_STR_3);
  blobStr += tmp;

  bool flag = buildRequest(dataObj, FirebaseMethod::POST, FirebaseDataType::BLOB, path.c_str(), blobStr.c_str(), queue, priority, "");

  std::string().swap(blobStr);

  delete[] tmp;
  return flag;
}

bool FirebaseESP32::pushFile(FirebaseData &dataObj, uint8_t storageType, const String &path, const String &fileName)
{
  return buildRequestFile(dataObj, storageType, FirebaseMethod::POST, path.c_str(), fileName.c_str(), false, "", "");
}

bool FirebaseESP32::pushFile(FirebaseData &dataObj, uint8_t storageType, const String &path, const String &fileName, float priority)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = buildRequestFile(dataObj, storageType, FirebaseMethod::POST, path.c_str(), fileName.c_str(), false, buff, "");
  delete[] buff;
  return flag;
}

bool FirebaseESP32::pushTimestamp(FirebaseData &dataObj, const String &path)
{
  return buildRequest(dataObj, FirebaseMethod::POST, FirebaseDataType::TIMESTAMP, path.c_str(), ESP32_FIREBASE_STR_154, false, "", "");
}

bool FirebaseESP32::setInt(FirebaseData &dataObj, const String &path, int intValue)
{
  return setInt(dataObj, path.c_str(), intValue, false, "", "");
}

bool FirebaseESP32::setInt(FirebaseData &dataObj, const String &path, int intValue, float priority)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = setInt(dataObj, path.c_str(), intValue, false, buff, "");
  delete[] buff;
  return flag;
}

bool FirebaseESP32::setInt(FirebaseData &dataObj, const String &path, int intValue, const String &ETag)
{
  return setInt(dataObj, path.c_str(), intValue, false, "", ETag.c_str());
}

bool FirebaseESP32::setInt(FirebaseData &dataObj, const String &path, int intValue, float priority, const String &ETag)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = setInt(dataObj, path.c_str(), intValue, false, buff, ETag.c_str());
  delete[] buff;
  return flag;
}

bool FirebaseESP32::setInt(FirebaseData &dataObj, const std::string &path, int intValue, bool queue, const std::string &priority, const std::string &etag)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  itoa(intValue, buff, 10);
  bool flag = buildRequest(dataObj, FirebaseMethod::PUT, FirebaseDataType::INTEGER, path, buff, queue, priority, etag);
  delete[] buff;
  return flag;
}

bool FirebaseESP32::setFloat(FirebaseData &dataObj, const String &path, float floatValue)
{
  return setFloat(dataObj, path.c_str(), floatValue, false, "", "");
}

bool FirebaseESP32::setFloat(FirebaseData &dataObj, const String &path, float floatValue, float priority)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = setFloat(dataObj, path.c_str(), floatValue, false, buff, "");
  delete[] buff;
  return flag;
}

bool FirebaseESP32::setFloat(FirebaseData &dataObj, const String &path, float floatValue, const String &ETag)
{
  return setFloat(dataObj, path.c_str(), floatValue, false, "", ETag.c_str());
}

bool FirebaseESP32::setFloat(FirebaseData &dataObj, const String &path, float floatValue, float priority, const String &ETag)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = setFloat(dataObj, path.c_str(), floatValue, false, buff, ETag.c_str());
  delete[] buff;
  return flag;
}

bool FirebaseESP32::setFloat(FirebaseData &dataObj, const std::string &path, float floatValue, bool queue, const std::string &priority, const std::string &etag)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(floatValue, 7, 6, buff);
  bool flag = buildRequest(dataObj, FirebaseMethod::PUT, FirebaseDataType::FLOAT, path, buff, queue, priority, etag);
  delete[] buff;
  return flag;
}

bool FirebaseESP32::setDouble(FirebaseData &dataObj, const String &path, double doubleValue)
{
  return setDouble(dataObj, path.c_str(), doubleValue, false, "", "");
}

bool FirebaseESP32::setDouble(FirebaseData &dataObj, const String &path, double doubleValue, float priority)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = setDouble(dataObj, path.c_str(), doubleValue, false, buff, "");
  delete[] buff;
  return flag;
}

bool FirebaseESP32::setDouble(FirebaseData &dataObj, const String &path, double doubleValue, const String &ETag)
{
  return setDouble(dataObj, path.c_str(), doubleValue, false, "", ETag.c_str());
}

bool FirebaseESP32::setDouble(FirebaseData &dataObj, const String &path, double doubleValue, float priority, const String &ETag)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = setDouble(dataObj, path.c_str(), doubleValue, false, buff, ETag.c_str());
  delete[] buff;
  return flag;
}

bool FirebaseESP32::setDouble(FirebaseData &dataObj, const std::string &path, double doubleValue, bool queue, const std::string &priority, const std::string &etag)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(doubleValue, 12, 9, buff);
  bool flag = buildRequest(dataObj, FirebaseMethod::PUT, FirebaseDataType::DOUBLE, path, buff, false, priority, etag);
  delete[] buff;
  return flag;
}

bool FirebaseESP32::setBool(FirebaseData &dataObj, const String &path, bool boolValue)
{
  return setBool(dataObj, path.c_str(), boolValue, false, "", "");
}

bool FirebaseESP32::setBool(FirebaseData &dataObj, const String &path, bool boolValue, float priority)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = setBool(dataObj, path.c_str(), boolValue, false, buff, "");
  delete[] buff;
  return flag;
}

bool FirebaseESP32::setBool(FirebaseData &dataObj, const String &path, bool boolValue, const String &ETag)
{
  return setBool(dataObj, path.c_str(), boolValue, false, "", ETag.c_str());
}

bool FirebaseESP32::setBool(FirebaseData &dataObj, const String &path, bool boolValue, float priority, const String &ETag)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = setBool(dataObj, path.c_str(), boolValue, false, buff, ETag.c_str());
  delete[] buff;
  return flag;
}

bool FirebaseESP32::setBool(FirebaseData &dataObj, const std::string &path, bool boolValue, bool queue, const std::string &priority, const std::string &etag)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  if (boolValue)
    strcpy_P(buff, ESP32_FIREBASE_STR_107);
  else
    strcpy_P(buff, ESP32_FIREBASE_STR_106);

  bool flag = buildRequest(dataObj, FirebaseMethod::PUT, FirebaseDataType::BOOLEAN, path, buff, false, priority, etag);
  delete[] buff;
  return flag;
}

bool FirebaseESP32::setString(FirebaseData &dataObj, const String &path, const String &stringValue)
{
  return buildRequest(dataObj, FirebaseMethod::PUT, FirebaseDataType::STRING, path.c_str(), stringValue.c_str(), false, "", "");
}

bool FirebaseESP32::setString(FirebaseData &dataObj, const String &path, const String &stringValue, float priority)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = buildRequest(dataObj, FirebaseMethod::PUT, FirebaseDataType::STRING, path.c_str(), stringValue.c_str(), false, buff, "");
  delete[] buff;
  return flag;
}

bool FirebaseESP32::setString(FirebaseData &dataObj, const String &path, const String &stringValue, const String &ETag)
{
  return buildRequest(dataObj, FirebaseMethod::PUT, FirebaseDataType::STRING, path.c_str(), stringValue.c_str(), false, "", ETag.c_str());
}

bool FirebaseESP32::setString(FirebaseData &dataObj, const String &path, const String &stringValue, float priority, const String &ETag)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = buildRequest(dataObj, FirebaseMethod::PUT, FirebaseDataType::STRING, path.c_str(), stringValue.c_str(), false, buff, ETag.c_str());
  delete[] buff;
  return flag;
}

bool FirebaseESP32::setJSON(FirebaseData &dataObj, const String &path, const String &jsonString)
{
  return buildRequest(dataObj, FirebaseMethod::PUT, FirebaseDataType::JSON, path.c_str(), jsonString.c_str(), false, "", "");
}

bool FirebaseESP32::setJSON(FirebaseData &dataObj, const String &path, FirebaseJson &json)
{
  return buildRequest(dataObj, FirebaseMethod::PUT, FirebaseDataType::JSON, path.c_str(), json.toStdString().c_str(), false, "", "");
}

bool FirebaseESP32::setJSON(FirebaseData &dataObj, const String &path, const String &jsonString, float priority)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = buildRequest(dataObj, FirebaseMethod::PUT, FirebaseDataType::JSON, path.c_str(), jsonString.c_str(), false, buff, "");
  delete[] buff;
  return flag;
}

bool FirebaseESP32::setJSON(FirebaseData &dataObj, const String &path, FirebaseJson &json, float priority)
{
  return setJSON(dataObj, path, json.toStdString().c_str(), priority);
}

bool FirebaseESP32::setJSON(FirebaseData &dataObj, const String &path, const String &jsonString, const String &ETag)
{
  return buildRequest(dataObj, FirebaseMethod::PUT, FirebaseDataType::JSON, path.c_str(), jsonString.c_str(), false, "", ETag.c_str());
}

bool FirebaseESP32::setJSON(FirebaseData &dataObj, const String &path, FirebaseJson &json, const String &ETag)
{
  return buildRequest(dataObj, FirebaseMethod::PUT, FirebaseDataType::JSON, path.c_str(), json.toStdString().c_str(), false, "", ETag.c_str());
}

bool FirebaseESP32::setJSON(FirebaseData &dataObj, const String &path, const String &jsonString, float priority, const String &ETag)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = buildRequest(dataObj, FirebaseMethod::PUT, FirebaseDataType::JSON, path.c_str(), jsonString.c_str(), false, buff, ETag.c_str());
  delete[] buff;
  return flag;
}

bool FirebaseESP32::setJSON(FirebaseData &dataObj, const String &path, FirebaseJson &json, float priority, const String &ETag)
{
  return setJSON(dataObj, path, json.toStdString().c_str(), priority, ETag);
}

bool FirebaseESP32::setBlob(FirebaseData &dataObj, const String &path, uint8_t *blob, size_t size)
{
  return setBlob(dataObj, path.c_str(), blob, size, false, "", "");
}

bool FirebaseESP32::setBlob(FirebaseData &dataObj, const String &path, uint8_t *blob, size_t size, float priority)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = setBlob(dataObj, path.c_str(), blob, size, false, buff, "");
  delete[] buff;
  return flag;
}

bool FirebaseESP32::setBlob(FirebaseData &dataObj, const String &path, uint8_t *blob, size_t size, const String &ETag)
{
  return setBlob(dataObj, path.c_str(), blob, size, false, "", ETag.c_str());
}

bool FirebaseESP32::setBlob(FirebaseData &dataObj, const String &path, uint8_t *blob, size_t size, float priority, const String &ETag)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = setBlob(dataObj, path.c_str(), blob, size, false, buff, ETag.c_str());
  delete[] buff;
  return flag;
}

bool FirebaseESP32::setBlob(FirebaseData &dataObj, const std::string &path, uint8_t *blob, size_t size, bool queue, const std::string &priority, const std::string &etag)
{
  if (dataObj._maxBlobSize < size)
  {
    dataObj._httpCode = HTTP_CODE_PAYLOAD_TOO_LARGE;
    return false;
  }

  size_t tmpSize = strlen(ESP32_FIREBASE_STR_92) + 1;
  char *tmp = new char[tmpSize];
  memset(tmp, 0, tmpSize);

  std::string blobStr = "";
  strcpy(tmp, ESP32_FIREBASE_STR_92);
  blobStr = tmp;
  blobStr += base64_encode_string((const unsigned char *)blob, size);
  memset(tmp, 0, tmpSize);
  strcpy_P(tmp, ESP32_FIREBASE_STR_3);
  blobStr += tmp;
  bool flag = buildRequest(dataObj, FirebaseMethod::PUT_SILENT, FirebaseDataType::BLOB, path, blobStr.c_str(), false, priority, etag);
  delete[] tmp;
  std::string().swap(blobStr);
  return flag;
}

bool FirebaseESP32::setFile(FirebaseData &dataObj, uint8_t storageType, const String &path, const String &fileName)
{
  return buildRequestFile(dataObj, storageType, FirebaseMethod::PUT_SILENT, path.c_str(), fileName.c_str(), false, "", "");
}

bool FirebaseESP32::setFile(FirebaseData &dataObj, uint8_t storageType, const String &path, const String &fileName, float priority)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = buildRequestFile(dataObj, storageType, FirebaseMethod::PUT_SILENT, path.c_str(), fileName.c_str(), false, buff, "");
  delete[] buff;
  return flag;
}

bool FirebaseESP32::setFile(FirebaseData &dataObj, uint8_t storageType, const String &path, const String &fileName, const String &ETag)
{
  return buildRequestFile(dataObj, storageType, FirebaseMethod::PUT_SILENT, path.c_str(), fileName.c_str(), false, "", ETag.c_str());
}

bool FirebaseESP32::setFile(FirebaseData &dataObj, uint8_t storageType, const String &path, const String &fileName, float priority, const String &ETag)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = buildRequestFile(dataObj, storageType, FirebaseMethod::PUT_SILENT, path.c_str(), fileName.c_str(), false, buff, ETag.c_str());
  delete[] buff;
  return flag;
}

bool FirebaseESP32::setTimestamp(FirebaseData &dataObj, const String &path)
{
  return buildRequest(dataObj, FirebaseMethod::PUT, FirebaseDataType::TIMESTAMP, path.c_str(), ESP32_FIREBASE_STR_154, false, "", "");
}

bool FirebaseESP32::updateNode(FirebaseData &dataObj, const String &path, const String &jsonString)
{
  return buildRequest(dataObj, FirebaseMethod::PATCH, FirebaseDataType::JSON, path.c_str(), jsonString.c_str(), "", "", "");
}

bool FirebaseESP32::updateNode(FirebaseData &dataObj, const String path, FirebaseJson &json)
{
  return buildRequest(dataObj, FirebaseMethod::PATCH, FirebaseDataType::JSON, path.c_str(), json.toStdString().c_str(), false, "", "");
}

bool FirebaseESP32::updateNode(FirebaseData &dataObj, const String &path, const String &jsonString, float priority)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = buildRequest(dataObj, FirebaseMethod::PATCH, FirebaseDataType::JSON, path.c_str(), jsonString.c_str(), false, buff, "");
  delete[] buff;
  return flag;
}

bool FirebaseESP32::updateNode(FirebaseData &dataObj, const String &path, FirebaseJson &json, float priority)
{
  return updateNode(dataObj, path, json.toStdString().c_str(), priority);
}

bool FirebaseESP32::updateNodeSilent(FirebaseData &dataObj, const String &path, const String &jsonString)
{
  return buildRequest(dataObj, FirebaseMethod::PATCH_SILENT, FirebaseDataType::JSON, path.c_str(), jsonString.c_str(), false, "", "");
}

bool FirebaseESP32::updateNodeSilent(FirebaseData &dataObj, const String &path, FirebaseJson &json)
{
  return buildRequest(dataObj, FirebaseMethod::PATCH_SILENT, FirebaseDataType::JSON, path.c_str(), json.toStdString().c_str(), false, "", "");
}

bool FirebaseESP32::updateNodeSilent(FirebaseData &dataObj, const String &path, const String &jsonString, float priority)
{
  size_t buffSize = 50;
  char *buff = new char[buffSize];
  memset(buff, 0, buffSize);
  dtostrf(priority, 7, 6, buff);
  bool flag = buildRequest(dataObj, FirebaseMethod::PATCH_SILENT, FirebaseDataType::JSON, path.c_str(), jsonString.c_str(), false, buff, "");
  delete[] buff;
  return flag;
}

bool FirebaseESP32::updateNodeSilent(FirebaseData &dataObj, const String &path, FirebaseJson &json, float priority)
{
  return updateNodeSilent(dataObj, path, json.toStdString().c_str(), priority);
}

bool FirebaseESP32::getInt(FirebaseData &dataObj, const String &path)
{
  return getFloat(dataObj, path);
}

bool FirebaseESP32::getInt(FirebaseData &dataObj, const String &path, int &target)
{

  bool flag;
  dataObj.queryFilter.clear();

  uint8_t errCount = 0;
  uint8_t maxRetry = dataObj._maxRetry;
  if (maxRetry == 0)
    maxRetry = 1;

  for (int i = 0; i < maxRetry; i++)
  {
    flag = sendRequest(dataObj, 0, path.c_str(), FirebaseMethod::GET, FirebaseDataType::INTEGER, "", "", "");
    target = dataObj.intData();
    if (flag)
      break;

    if (dataObj._maxRetry > 0)
      if (!flag && commError(dataObj))
        errCount++;
  }

  if (!flag && errCount == maxRetry && dataObj._qMan._maxQueue > 0)
    dataObj.addQueue(FirebaseMethod::GET, 0, FirebaseDataType::INTEGER, path.c_str(), "", "", false, &target, nullptr, nullptr, nullptr, nullptr, nullptr);

  if (flag && dataObj._dataType != FirebaseDataType::INTEGER && dataObj._dataType != FirebaseDataType::FLOAT && dataObj._dataType != FirebaseDataType::DOUBLE)
    flag = false;

  return flag;
}

bool FirebaseESP32::getFloat(FirebaseData &dataObj, const String &path)
{
  dataObj.queryFilter.clear();
  bool flag = sendRequest(dataObj, 0, path.c_str(), FirebaseMethod::GET, FirebaseDataType::FLOAT, "", "", "");
  if (dataObj._dataType != FirebaseDataType::INTEGER && dataObj._dataType != FirebaseDataType::FLOAT && dataObj._dataType != FirebaseDataType::DOUBLE)
    flag = false;
  return flag;
}

bool FirebaseESP32::getFloat(FirebaseData &dataObj, const String &path, float &target)
{

  bool flag;
  dataObj.queryFilter.clear();

  uint8_t errCount = 0;
  uint8_t maxRetry = dataObj._maxRetry;
  if (maxRetry == 0)
    maxRetry = 1;

  for (int i = 0; i < maxRetry; i++)
  {
    flag = sendRequest(dataObj, 0, path.c_str(), FirebaseMethod::GET, FirebaseDataType::FLOAT, "", "", "");
    target = dataObj.floatData();
    if (flag)
      break;

    if (dataObj._maxRetry > 0)
      if (!flag && commError(dataObj))
        errCount++;
  }

  if (!flag && errCount == maxRetry && dataObj._qMan._maxQueue > 0)
    dataObj.addQueue(FirebaseMethod::GET, 0, FirebaseDataType::FLOAT, path.c_str(), "", "", false, nullptr, &target, nullptr, nullptr, nullptr, nullptr);

  if (flag && dataObj._dataType != FirebaseDataType::INTEGER && dataObj._dataType != FirebaseDataType::FLOAT && dataObj._dataType != FirebaseDataType::DOUBLE)
    flag = false;

  return flag;
}

bool FirebaseESP32::getDouble(FirebaseData &dataObj, const String &path)
{
  dataObj.queryFilter.clear();
  bool flag = sendRequest(dataObj, 0, path.c_str(), FirebaseMethod::GET, FirebaseDataType::DOUBLE, "", "", "");
  if (dataObj._dataType != FirebaseDataType::INTEGER && dataObj._dataType != FirebaseDataType::FLOAT && dataObj._dataType != FirebaseDataType::DOUBLE)
    flag = false;
  return flag;
}

bool FirebaseESP32::getDouble(FirebaseData &dataObj, const String &path, double &target)
{

  bool flag;
  dataObj.queryFilter.clear();

  uint8_t errCount = 0;
  uint8_t maxRetry = dataObj._maxRetry;
  if (maxRetry == 0)
    maxRetry = 1;

  for (int i = 0; i < maxRetry; i++)
  {
    flag = sendRequest(dataObj, 0, path.c_str(), FirebaseMethod::GET, FirebaseDataType::DOUBLE, "", "", "");
    target = dataObj.floatData();
    if (flag)
      break;

    if (dataObj._maxRetry > 0)
      if (!flag && commError(dataObj))
        errCount++;
  }

  if (!flag && errCount == maxRetry && dataObj._qMan._maxQueue > 0)
    dataObj.addQueue(FirebaseMethod::GET, 0, FirebaseDataType::DOUBLE, path.c_str(), "", "", false, nullptr, nullptr, &target, nullptr, nullptr, nullptr);

  if (flag && dataObj._dataType != FirebaseDataType::INTEGER && dataObj._dataType != FirebaseDataType::FLOAT && dataObj._dataType != FirebaseDataType::DOUBLE)
    flag = false;

  return flag;
}

bool FirebaseESP32::getBool(FirebaseData &dataObj, const String &path)
{
  dataObj.queryFilter.clear();
  bool flag = sendRequest(dataObj, 0, path.c_str(), FirebaseMethod::GET, FirebaseDataType::BOOLEAN, "", "", "");
  if (dataObj._dataType != FirebaseDataType::BOOLEAN)
    flag = false;
  return flag;
}

bool FirebaseESP32::getBool(FirebaseData &dataObj, const String &path, bool &target)
{

  bool flag;
  dataObj.queryFilter.clear();

  uint8_t errCount = 0;
  uint8_t maxRetry = dataObj._maxRetry;
  if (maxRetry == 0)
    maxRetry = 1;

  for (int i = 0; i < maxRetry; i++)
  {
    flag = sendRequest(dataObj, 0, path.c_str(), FirebaseMethod::GET, FirebaseDataType::BOOLEAN, "", "", "");
    target = dataObj.boolData();
    if (flag)
      break;

    if (dataObj._maxRetry > 0)
      if (!flag && commError(dataObj))
        errCount++;
  }

  if (!flag && errCount == maxRetry && dataObj._qMan._maxQueue > 0)
    dataObj.addQueue(FirebaseMethod::GET, 0, FirebaseDataType::BOOLEAN, path.c_str(), "", "", false, nullptr, nullptr, nullptr, &target, nullptr, nullptr);

  if (flag && dataObj._dataType != FirebaseDataType::BOOLEAN)
    flag = false;

  return flag;
}

bool FirebaseESP32::getString(FirebaseData &dataObj, const String &path)
{
  dataObj.queryFilter.clear();
  bool flag = sendRequest(dataObj, 0, path.c_str(), FirebaseMethod::GET, FirebaseDataType::STRING, "", "", "");
  if (dataObj._dataType != FirebaseDataType::STRING)
    flag = false;
  return flag;
}

bool FirebaseESP32::getString(FirebaseData &dataObj, const String &path, String &target)
{

  bool flag;
  dataObj.queryFilter.clear();

  uint8_t errCount = 0;
  uint8_t maxRetry = dataObj._maxRetry;
  if (maxRetry == 0)
    maxRetry = 1;

  for (int i = 0; i < maxRetry; i++)
  {
    flag = sendRequest(dataObj, 0, path.c_str(), FirebaseMethod::GET, FirebaseDataType::STRING, "", "", "");
    target = dataObj.stringData();
    if (flag)
      break;

    if (dataObj._maxRetry > 0)
      if (!flag && commError(dataObj))
        errCount++;
  }

  if (!flag && errCount == maxRetry && dataObj._qMan._maxQueue > 0)
    dataObj.addQueue(FirebaseMethod::GET, 0, FirebaseDataType::STRING, path.c_str(), "", "", false, nullptr, nullptr, nullptr, nullptr, &target, nullptr);

  if (flag && dataObj._dataType != FirebaseDataType::STRING)
    flag = false;

  return flag;
}

bool FirebaseESP32::getJSON(FirebaseData &dataObj, const String &path)
{
  dataObj.queryFilter.clear();
  bool flag = sendRequest(dataObj, 0, path.c_str(), FirebaseMethod::GET, FirebaseDataType::JSON, "", "", "");
  if (dataObj._dataType != FirebaseDataType::JSON)
    flag = false;
  return flag;
}

bool FirebaseESP32::getJSON(FirebaseData &dataObj, const String &path, String &target)
{

  bool flag;
  dataObj.queryFilter.clear();

  uint8_t errCount = 0;
  uint8_t maxRetry = dataObj._maxRetry;
  if (maxRetry == 0)
    maxRetry = 1;

  for (int i = 0; i < maxRetry; i++)
  {
    flag = sendRequest(dataObj, 0, path.c_str(), FirebaseMethod::GET, FirebaseDataType::JSON, "", "", "");
    target = dataObj.jsonData();
    if (flag)
      break;

    if (dataObj._maxRetry > 0)
      if (!flag && commError(dataObj))
        errCount++;
  }

  if (!flag && errCount == maxRetry && dataObj._qMan._maxQueue > 0)
    dataObj.addQueue(FirebaseMethod::GET, 0, FirebaseDataType::JSON, path.c_str(), "", "", false, nullptr, nullptr, nullptr, nullptr, &target, nullptr);

  if (flag && dataObj._dataType != FirebaseDataType::JSON)
    flag = false;

  return flag;
}

bool FirebaseESP32::getJSON(FirebaseData &dataObj, const String &path, QueryFilter &query)
{
  dataObj.queryFilter.clear();
  if (query._orderBy != "")
    dataObj.setQuery(query);

  bool flag = sendRequest(dataObj, 0, path.c_str(), FirebaseMethod::GET, FirebaseDataType::JSON, "", "", "");
  if (dataObj._dataType != FirebaseDataType::JSON)
    flag = false;
  return flag;
}

bool FirebaseESP32::getJSON(FirebaseData &dataObj, const String &path, QueryFilter &query, String &target)
{
  dataObj.queryFilter.clear();
  if (query._orderBy != "")
    dataObj.setQuery(query);

  bool flag;

  uint8_t errCount = 0;
  uint8_t maxRetry = dataObj._maxRetry;
  if (maxRetry == 0)
    maxRetry = 1;

  for (int i = 0; i < maxRetry; i++)
  {
    flag = sendRequest(dataObj, 0, path.c_str(), FirebaseMethod::GET, FirebaseDataType::JSON, "", "", "");
    target = dataObj.jsonData();
    if (flag)
      break;

    if (dataObj._maxRetry > 0)
      if (!flag && commError(dataObj))
        errCount++;
  }

  if (!flag && errCount == maxRetry && dataObj._qMan._maxQueue > 0)
    dataObj.addQueue(FirebaseMethod::GET, 0, FirebaseDataType::JSON, path.c_str(), "", "", true, nullptr, nullptr, nullptr, nullptr, &target, nullptr);

  if (flag && dataObj._dataType != FirebaseDataType::JSON)
    flag = false;

  return flag;
}

bool FirebaseESP32::getBlob(FirebaseData &dataObj, const String &path)
{
  dataObj.queryFilter.clear();
  bool flag = sendRequest(dataObj, 0, path.c_str(), FirebaseMethod::GET, FirebaseDataType::BLOB, "", "", "");
  if (dataObj._dataType != FirebaseDataType::BLOB)
    flag = false;
  return flag;
}

bool FirebaseESP32::getBlob(FirebaseData &dataObj, const String &path, std::vector<uint8_t> &target)
{

  dataObj.queryFilter.clear();

  bool flag;

  uint8_t errCount = 0;
  uint8_t maxRetry = dataObj._maxRetry;
  if (maxRetry == 0)
    maxRetry = 1;

  for (int i = 0; i < maxRetry; i++)
  {
    flag = sendRequest(dataObj, 0, path.c_str(), FirebaseMethod::GET, FirebaseDataType::BLOB, "", "", "");
    target = dataObj.blobData();
    if (flag)
      break;

    if (dataObj._maxRetry > 0)
      if (!flag && commError(dataObj))
        errCount++;
  }

  if (!flag && errCount == maxRetry && dataObj._qMan._maxQueue > 0)
    dataObj.addQueue(FirebaseMethod::GET, 0, FirebaseDataType::BLOB, path.c_str(), "", "", false, nullptr, nullptr, nullptr, nullptr, nullptr, &target);

  if (flag && dataObj._dataType != FirebaseDataType::BLOB)
    flag = false;

  return flag;
}

bool FirebaseESP32::getFile(FirebaseData &dataObj, uint8_t storageType, const String &nodePath, const String &fileName)
{
  dataObj.queryFilter.clear();
  dataObj._fileName.clear();
  dataObj._fileName = fileName.c_str();

  bool flag;

  uint8_t errCount = 0;
  uint8_t maxRetry = dataObj._maxRetry;
  if (maxRetry == 0)
    maxRetry = 1;

  for (int i = 0; i < maxRetry; i++)
  {
    flag = sendRequest(dataObj, storageType, nodePath.c_str(), FirebaseMethod::GET, FirebaseDataType::FILE, "", "", "");
    if (flag)
      break;

    if (dataObj._maxRetry > 0)
      if (!flag && (commError(dataObj) || dataObj._file_transfer_error.length() > 0))
        errCount++;
  }

  if (!flag && errCount == maxRetry && dataObj._qMan._maxQueue > 0)
    dataObj.addQueue(FirebaseMethod::GET, storageType, FirebaseDataType::FILE, nodePath.c_str(), fileName.c_str(), "", false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

  return flag;
}

bool FirebaseESP32::deleteNode(FirebaseData &dataObj, const String &path)
{
  dataObj.queryFilter.clear();
  return sendRequest(dataObj, 0, path.c_str(), FirebaseMethod::DELETE, FirebaseDataType::STRING, "", "", "");
}

bool FirebaseESP32::deleteNode(FirebaseData &dataObj, const String &path, const String &ETag)
{
  dataObj.queryFilter.clear();
  return sendRequest(dataObj, 0, path.c_str(), FirebaseMethod::DELETE, FirebaseDataType::STRING, "", "", ETag.c_str());
}

bool FirebaseESP32::beginStream(FirebaseData &dataObj, const String &path)
{
  dataObj._pause = false;
  return firebaseConnectStream(dataObj, path.c_str());
}

bool FirebaseESP32::readStream(FirebaseData &dataObj)
{
  if (dataObj._streamStop)
    return true;

  if (_reconnectWiFi && WiFi.status() != WL_CONNECTED)
    WiFi.reconnect();

  return getServerStreamResponse(dataObj);
}

bool FirebaseESP32::endStream(FirebaseData &dataObj)
{
  dataObj._pause = true;
  bool flag = false;
  dataObj._streamStop = true;

  if (!apConnected(dataObj))
  {
    dataObj._isStream = false;
    dataObj._streamStop = true;
    return true;
  }

  if (dataObj._net.getStreamPtr())
  {
    if (dataObj._net.getStreamPtr()->available() > 0)
      dataObj._net.getStreamPtr()->read();

    dataObj._net.getStreamPtr()->stop();
  }

  flag = dataObj._net.connected();
  if (!flag)
  {
    dataObj._isStream = false;
    dataObj._streamStop = true;
  }
  return !flag;
}

int FirebaseESP32::firebaseConnect(FirebaseData &dataObj, const std::string &path, const uint8_t method, uint8_t dataType, const std::string &payload, const std::string &priority)
{

  dataObj._firebaseError.clear();

  if (dataObj._pause)
    return 0;

  if (!apConnected(dataObj))
    return HTTPC_ERROR_CONNECTION_LOST;

  if (path.length() == 0 || _host.length() == 0 || _auth.length() == 0)
  {
    dataObj._httpCode = HTTP_CODE_BAD_REQUEST;
    return HTTP_CODE_BAD_REQUEST;
  }

  uint8_t retryCount = 0;
  uint8_t maxRetry = 5;

  size_t buffSize = 32;
  char *buff = new char[buffSize];

  int len = 0;
  size_t toRead = 0;
  bool httpConnected = false;

  int httpCode = HTTPC_ERROR_CONNECTION_REFUSED;

  //build requested uri
  std::string payloadStr = "";
  std::string header = "";
  std::string uri = "";

  if (dataObj._redirectURL == "")
  {
    if (path[0] != '/')
      uri = ESP32_FIREBASE_STR_1;

    uri += path;

    if (method == FirebaseMethod::PATCH || method == FirebaseMethod::PATCH_SILENT || method == FirebaseMethod::RESTORE)
      uri += ESP32_FIREBASE_STR_1;

    uri += ESP32_FIREBASE_STR_2;
    uri += _auth;
  }
  else
  {
    uri = dataObj._redirectURL;
  }

  //init the firebase data
  resetFirebasedataFlag(dataObj);
  dataObj._path.clear();
  dataObj._etag.clear();

  if (method == FirebaseMethod::STREAM)
  {
    //stream path change? reset the current (keep alive) connection
    if (path != dataObj._streamPath)
      dataObj._streamPathChanged = true;
    if (!dataObj._isStream || dataObj._streamPathChanged)
    {
      if (dataObj._net.connected())
        forceEndHTTP(dataObj);
    }

    dataObj._streamPath.clear();

    if (path.length() > 0)
      if (path[0] != '/')
        dataObj._streamPath = ESP32_FIREBASE_STR_1;

    dataObj._streamPath += path;
  }
  else
  {
    //last requested method was stream?, reset the connection
    if (dataObj._isStream)
      forceEndHTTP(dataObj);
    if (method != FirebaseMethod::BACKUP && method != FirebaseMethod::RESTORE)
    {
      dataObj._path.clear();
      if (path.length() > 0)
        if (path[0] != '/')
          dataObj._path = ESP32_FIREBASE_STR_1;
      dataObj._path += path;
    }

    dataObj._isStreamTimeout = false;
  }

  setSecure(dataObj);

  httpConnected = dataObj._net.begin(_host.c_str(), _port, uri.c_str());

  if (!httpConnected)
  {
    dataObj._httpCode = HTTPC_ERROR_CONNECTION_REFUSED;
    goto EXIT_1;
  }

  //Prepare for string and JSON payloads
  if (method != FirebaseMethod::GET && method != FirebaseMethod::GET_RULES && method != FirebaseMethod::GET_SHALLOW && method != FirebaseMethod::GET_PRIORITY && method != FirebaseMethod::GET_SILENT && method != FirebaseMethod::STREAM &&
      method != FirebaseMethod::DELETE && method != FirebaseMethod::RESTORE)
  {

    if (priority.length() > 0)
    {
      if (dataType == FirebaseDataType::JSON)
      {
        if (payload.length() > 0)
        {
          payloadStr.clear();
          payloadStr = payload;
          int x = payloadStr.find_last_of(ESP32_FIREBASE_STR_127);
          if (x != std::string::npos && x != 0)
            for (size_t i = 0; i < payloadStr.length() - x; i++)
              payloadStr.pop_back();

          payloadStr += ESP32_FIREBASE_STR_157;
          payloadStr += priority;
          payloadStr += ESP32_FIREBASE_STR_127;
        }
      }
      else
      {
        payloadStr.clear();
        payloadStr += ESP32_FIREBASE_STR_161;

        if (dataType == FirebaseDataType::STRING)
          payloadStr += ESP32_FIREBASE_STR_3;
        payloadStr += payload;
        if (dataType == FirebaseDataType::STRING)
          payloadStr += ESP32_FIREBASE_STR_3;

        payloadStr += ESP32_FIREBASE_STR_157;
        payloadStr += priority;
        payloadStr += ESP32_FIREBASE_STR_127;
      }
    }
    else
    {
      payloadStr.clear();
      if (dataType == FirebaseDataType::STRING)
        payloadStr += ESP32_FIREBASE_STR_3;
      payloadStr += payload;
      if (dataType == FirebaseDataType::STRING)
        payloadStr += ESP32_FIREBASE_STR_3;
    }
  }

  //Prepare request header

  if (method != FirebaseMethod::BACKUP && method != FirebaseMethod::RESTORE && dataType != FirebaseDataType::FILE)
    buildFirebaseRequest(dataObj, _host, method, dataType, path, _auth, payloadStr.length(), header);
  else
  {

    if (dataObj._storageType == StorageType::SPIFFS)
    {
      if (!SPIFFS.begin(true))
      {
        dataObj._file_transfer_error.clear();
        dataObj._file_transfer_error = ESP32_FIREBASE_STR_164;
        goto EXIT_1;
      }
    }
    else if (dataObj._storageType == StorageType::SD)
    {
      if (_sdInUse)
      {
        dataObj._file_transfer_error.clear();
        dataObj._file_transfer_error = ESP32_FIREBASE_STR_84;
        goto EXIT_1;
      }

      if (!_sdOk)
        _sdOk = sdTest();

      if (!_sdOk)
      {
        dataObj._file_transfer_error.clear();
        dataObj._file_transfer_error = ESP32_FIREBASE_STR_85;
        goto EXIT_1;
      }

      _sdInUse = true;
    }

    if (method == FirebaseMethod::BACKUP || method == FirebaseMethod::RESTORE)
    {

      dataObj._backupFilename.clear();
      dataObj._backupFilename = dataObj._backupDir + ESP32_FIREBASE_STR_1;

      for (int i = 1; i < dataObj._backupNodePath.length(); i++)
      {
        if (dataObj._backupNodePath[i] == '/')
          dataObj._backupFilename += ESP32_FIREBASE_STR_4;
        else
          dataObj._backupFilename += dataObj._backupNodePath[i];
      }
      if (dataObj._backupFilename == dataObj._backupDir + ESP32_FIREBASE_STR_1)
        dataObj._backupFilename = ESP32_FIREBASE_STR_90;
      else
        dataObj._backupFilename += ESP32_FIREBASE_STR_89;

      if (method == FirebaseMethod::BACKUP)
      {
        if (dataObj._storageType == StorageType::SPIFFS)
        {
          if (SPIFFS.exists(dataObj._backupFilename.c_str()))
            SPIFFS.remove(dataObj._backupFilename.c_str());
          file = SPIFFS.open(dataObj._backupFilename.c_str(), "w");
        }
        else if (dataObj._storageType == StorageType::SD)
        {
          if (SD.exists(dataObj._backupFilename.c_str()))
            SD.remove(dataObj._backupFilename.c_str());
          file = SD.open(dataObj._backupFilename.c_str(), FILE_WRITE);
        }
      }
      else if (method == FirebaseMethod::RESTORE)
      {
        if (dataObj._storageType == StorageType::SPIFFS && SPIFFS.exists(dataObj._backupFilename.c_str()))
          file = SPIFFS.open(dataObj._backupFilename.c_str(), "r");
        else if (dataObj._storageType == StorageType::SD && SD.exists(dataObj._backupFilename.c_str()))
          file = SD.open(dataObj._backupFilename.c_str(), FILE_READ);
        else
        {
          dataObj._file_transfer_error.clear();
          dataObj._file_transfer_error = ESP32_FIREBASE_STR_83;
          goto EXIT_1;
        }

        len = file.size();
      }

      if (!file)
      {
        dataObj._file_transfer_error.clear();
        dataObj._file_transfer_error = ESP32_FIREBASE_STR_86;
        goto EXIT_1;
      }
    }

    if (dataType == FirebaseDataType::FILE)
    {

      if (method == FirebaseMethod::PUT_SILENT || method == FirebaseMethod::POST)
      {
        if (dataObj._storageType == StorageType::SPIFFS)
        {
          if (SPIFFS.exists(dataObj._fileName.c_str()))
            file = SPIFFS.open(dataObj._fileName.c_str(), "r");
          else
          {
            dataObj._file_transfer_error.clear();
            dataObj._file_transfer_error = ESP32_FIREBASE_STR_83;
            goto EXIT_1;
          }
        }
        else if (dataObj._storageType == StorageType::SD)
        {
          if (SD.exists(dataObj._fileName.c_str()))
            file = SD.open(dataObj._fileName.c_str(), FILE_READ);
          else
          {
            dataObj._file_transfer_error.clear();
            dataObj._file_transfer_error = ESP32_FIREBASE_STR_83;
            goto EXIT_1;
          }
        }

        len = (4 * ceil(file.size() / 3.0)) + strlen_P(ESP32_FIREBASE_STR_93) + 1;
      }
      else if (method == FirebaseMethod::GET)
      {

        int p1 = dataObj._fileName.find_last_of(ESP32_FIREBASE_STR_1);
        std::string folder = "/";

        if (p1 != std::string::npos && p1 != 0)
          folder = dataObj._fileName.substr(p1 - 1);

        if (dataObj._storageType == StorageType::SPIFFS)
        {
          file = SPIFFS.open(dataObj._fileName.c_str(), "w");
        }
        else if (dataObj._storageType == StorageType::SD)
        {
          if (!SD.exists(folder.c_str()))
            createDirs(folder, dataObj._storageType);

          SD.remove(dataObj._fileName.c_str());

          file = SD.open(dataObj._fileName.c_str(), FILE_WRITE);
        }
        std::string().swap(folder);
      }

      if (!file)
      {
        dataObj._file_transfer_error.clear();
        dataObj._file_transfer_error = ESP32_FIREBASE_STR_86;
        goto EXIT_1;
      }
    }

    if (dataType == FirebaseDataType::FILE)
      buildFirebaseRequest(dataObj, _host, method, dataType, dataObj._path.c_str(), _auth, len, header);
    else
      buildFirebaseRequest(dataObj, _host, method, dataType, dataObj._backupNodePath.c_str(), _auth, len, header);
  }

  if (method == FirebaseMethod::GET_SILENT || method == FirebaseMethod::PATCH_SILENT || (method == FirebaseMethod::PUT_SILENT && dataType == FirebaseDataType::BLOB))
    dataObj._isSilentResponse = true;

  if (dataType == FirebaseDataType::BLOB)
    std::vector<uint8_t>().swap(dataObj._blob);

  //Send request w/wo payload

  if (!apConnected(dataObj))
    return HTTPC_ERROR_CONNECTION_LOST;

  httpCode = dataObj._net.sendRequest(header.c_str(), payloadStr.c_str());

  retryCount = 0;
  while (httpCode != 0)
  {
    retryCount++;
    if (retryCount > maxRetry)
      break;

    if (!apConnected(dataObj))
      return HTTPC_ERROR_CONNECTION_LOST;

    httpCode = dataObj._net.sendRequest(header.c_str(), payloadStr.c_str());
  }

  if (method == FirebaseMethod::RESTORE || (dataType == FirebaseDataType::FILE && (method == FirebaseMethod::PUT_SILENT || method == FirebaseMethod::POST)))
  {

    if (dataType == FirebaseDataType::FILE && (method == FirebaseMethod::PUT_SILENT || method == FirebaseMethod::POST))
    {
      memset(buff, 0, buffSize);
      strcpy_P(buff, ESP32_FIREBASE_STR_93);

      if (!apConnected(dataObj))
        return HTTPC_ERROR_CONNECTION_LOST;

      httpCode = dataObj._net.sendRequest("", buff);

      send_base64_encode_file(dataObj._net.getStreamPtr(), dataObj._fileName, dataObj._storageType);

      memset(buff, 0, buffSize);
      buff[0] = '"';
      buff[1] = '\0';

      if (!apConnected(dataObj))
        return HTTPC_ERROR_CONNECTION_LOST;

      httpCode = dataObj._net.sendRequest("", buff);
    }
    else
    {

      while (len)
      {
        toRead = len;
        if (toRead > buffSize)
          toRead = buffSize - 1;

        memset(buff, 0, buffSize);
        file.read((uint8_t *)buff, toRead);
        buff[toRead] = '\0';

        if (!apConnected(dataObj))
          return HTTPC_ERROR_CONNECTION_LOST;

        httpCode = dataObj._net.sendRequest("", buff);

        len -= toRead;

        if (len <= 0)
          break;
      }
    }

    endFileTransfer(dataObj);
  }

  delete[] buff;
  std::string().swap(payloadStr);
  std::string().swap(header);
  std::string().swap(uri);

  return httpCode;

EXIT_1:

  delete[] buff;
  std::string().swap(payloadStr);
  std::string().swap(header);
  std::string().swap(uri);

  return HTTPC_ERROR_CONNECTION_REFUSED;
}

bool FirebaseESP32::sendRequest(FirebaseData &dataObj, uint8_t storageType, const std::string &path, const uint8_t method, uint8_t dataType, const std::string &payload, const std::string &priority, const std::string &etag)
{

  bool flag = false;
  dataObj._firebaseError.clear();

  if (_reconnectWiFi && WiFi.status() != WL_CONNECTED)
    WiFi.reconnect();

  if (dataObj._pause || dataObj._file_transfering)
    return true;

  if (path.length() == 0 || _host.length() == 0 || _auth.length() == 0)
  {
    dataObj._httpCode = HTTP_CODE_BAD_REQUEST;
    return false;
  }

  if ((method == FirebaseMethod::PUT || method == FirebaseMethod::POST || method == FirebaseMethod::PATCH || method == FirebaseMethod::PATCH_SILENT || method == FirebaseMethod::SET_RULES) && payload.length() == 0 && dataType != FirebaseDataType::STRING && dataType != FirebaseDataType::BLOB && dataType != FirebaseDataType::FILE)
  {
    dataObj._httpCode = HTTP_CODE_BAD_REQUEST;
    return false;
  }

  //Try to reconnect WiFi if lost connection
  reconnect();

  //If WiFi is not connected, return false
  if (!apConnected(dataObj))
    return false;

  if (dataObj._firebaseCall)
  {
    dataObj._httpCode = HTTPC_ERROR_CONNECTION_INUSED;
    return false;
  }

  dataObj._firebaseCall = true;
  dataObj._qID = 0;
  dataObj._etag2 = etag;
  dataObj._priority = priority;
  dataObj._storageType = storageType;

  //Get the current WiFi client from current firebase data
  //Check for connection status
  if (dataObj._net.getStreamPtr())
  {
    if (dataObj._net.getStreamPtr()->connected())
      dataObj._httpConnected = true;
    else
      dataObj._httpConnected = false;
  }

  if (dataObj._httpConnected)
  {

    if (method == FirebaseMethod::STREAM)
    {

      dataObj._streamMillis = millis();
      dataObj._firebaseCall = false;
      return false;
    }
    else
    {
      if (!dataObj._keepAlive)
      {
        dataObj._streamMillis = millis() + 50;
        dataObj._interruptRequest = true;

        if (dataObj._net.connected())
        {

          forceEndHTTP(dataObj);
          if (dataObj._net.connected())
            if (!dataObj._isStream)
            {
              dataObj._firebaseCall = false;
              return false;
            }
        }
        dataObj._httpConnected = false;
      }
    }
  }

  if (!dataObj._keepAlive && dataObj._httpConnected)
  {
    dataObj._firebaseCall = false;
    return false;
  }

  dataObj._httpConnected = true;
  dataObj._interruptRequest = false;
  dataObj._redirectURL = "";
  dataObj._r_method = method;
  dataObj._r_dataType = dataType;

  if (!apConnected(dataObj))
    return false;

  int httpCode = firebaseConnect(dataObj, path, method, dataType, payload, priority);

  if (httpCode == 0)
  {
    if (method == FirebaseMethod::BACKUP)
    {
      dataObj._file_transfering = true;
    }
    else if (method == FirebaseMethod::RESTORE || (dataType == FirebaseDataType::FILE && (method == FirebaseMethod::PUT_SILENT || method == FirebaseMethod::POST)))
    {
      dataObj._file_transfering = true;
    }

    dataObj._dataType2 = dataType;

    if (method == FirebaseMethod::STREAM)
    {

      dataObj._dataMillis = millis();
      flag = getServerStreamResponse(dataObj);
      if (!flag)
        forceEndHTTP(dataObj);
    }
    else if (method == FirebaseMethod::BACKUP || (dataType == FirebaseDataType::FILE && method == FirebaseMethod::GET))
    {
      flag = getDownloadResponse(dataObj);
    }
    else if (method == FirebaseMethod::RESTORE || (dataType == FirebaseDataType::FILE && method == FirebaseMethod::PUT_SILENT))
    {
      flag = getUploadResponse(dataObj);
    }
    else
    {
      dataObj._path = path;
      flag = getServerResponse(dataObj);
      dataObj._dataAvailable = dataObj._data.length() > 0 || dataObj._blob.size() > 0;

      if (dataObj._file_transfering)
        endFileTransfer(dataObj);
    }

    if (!flag)
      forceEndHTTP(dataObj);
  }
  else
  {
    //can't establish connection
    dataObj._httpCode = httpCode;
    dataObj._httpConnected = false;
    dataObj._firebaseCall = false;
    return false;
  }
  dataObj._firebaseCall = false;
  return flag;
}

bool FirebaseESP32::getServerResponse(FirebaseData &dataObj)
{

  if (dataObj._pause)
    return true;

  if (!apConnected(dataObj))
    return false;

  if (!dataObj._net.getStreamPtr() || dataObj._interruptRequest)
    return cancelCurrentResponse(dataObj);
  if (!handleNetNotConnected(dataObj) || !dataObj._httpConnected)
    return false;

  bool flag = false;
  std::string linebuff = "";
  std::string eventType = "";
  std::string jsonRes = "";

  char c;
  int p1, p2;
  bool isStream = false;
  dataObj._httpCode = -1000;
  dataObj._contentLength = -1;
  dataObj._pushName.clear();

  bool hasEvent = false;
  bool hasEventData = false;
  bool hasBlob = false;
  bool rulesBegin = false;

  size_t lfCount = 0;
  size_t charPos = 0;

  unsigned long dataTime = millis();

  if (!dataObj._isStream)
    while (dataObj._net.getStreamPtr()->connected() && !dataObj._net.getStreamPtr()->available() && millis() - dataTime < dataObj._net.tcpTimeout)
    {
      if (!apConnected(dataObj))
        return false;

      delay(1);
    }

  dataTime = millis();

  if (dataObj._net.getStreamPtr()->connected() && !dataObj._net.getStreamPtr()->available() && !dataObj._isStream)
    dataObj._httpCode = HTTPC_ERROR_READ_TIMEOUT;

  if (dataObj._net.getStreamPtr()->connected() && dataObj._net.getStreamPtr()->available())
  {

    while (dataObj._net.getStreamPtr()->available())
    {
      if (dataObj._interruptRequest)
        return cancelCurrentResponse(dataObj);

      if (!apConnected(dataObj))
        return false;

      c = dataObj._net.getStreamPtr()->read();

      if (!hasBlob)
      {
        if (c != '\r' && c != '\n')
          linebuff.append(1, c);
      }
      else
      {

        if (c != '"' && c != '\n' && dataObj._httpCode != HTTP_CODE_NO_CONTENT)
          linebuff.append(1, c);

        continue;
      }

      if (charPos >= strlen(ESP32_FIREBASE_STR_92) - 1 && !hasBlob)
      {

        if (linebuff.find(ESP32_FIREBASE_STR_92) == 0)
        {
          hasBlob = true;
          if (dataObj._dataType2 != FirebaseDataType::BLOB)
            dataObj._httpCode = FIREBASE_ERROR_DATA_TYPE_MISMATCH;
          linebuff.clear();
        }
      }
      charPos++;

      if (c == '\n')
      {

        if (rulesBegin)
          jsonRes += linebuff + '\n';

        dataTime = millis();

        p1 = linebuff.find(ESP32_FIREBASE_STR_5);
        if (p1 != std::string::npos)
        {
          p2 = linebuff.find(ESP32_FIREBASE_STR_6, p1 + strlen(ESP32_FIREBASE_STR_5));
          if (p2 != std::string::npos)
            dataObj._httpCode = atoi(linebuff.substr(p1 + strlen(ESP32_FIREBASE_STR_5), p2 - p1 - strlen(ESP32_FIREBASE_STR_5)).c_str());
        }

        if (dataObj._httpCode == HTTP_CODE_TEMPORARY_REDIRECT)
        {

          p1 = linebuff.find(ESP32_FIREBASE_STR_95);
          if (p1 != std::string::npos)
          {
            dataObj._redirectURL = linebuff.substr(p1 + strlen(ESP32_FIREBASE_STR_95));

            int res = firebaseConnect(dataObj, dataObj._redirectURL, dataObj._r_method, dataObj._r_dataType, "", dataObj._priority);

            if (res == 0)
              goto EXIT_4;

            goto EXIT_3;
          }
        }

        p1 = linebuff.find(ESP32_FIREBASE_STR_150);
        if (p1 != std::string::npos)
          dataObj._etag = linebuff.substr(p1 + strlen(ESP32_FIREBASE_STR_150)).c_str();

        if (dataObj._httpCode == HTTP_CODE_NO_CONTENT)
        {
          linebuff.clear();
          continue;
        }

        p1 = linebuff.find(ESP32_FIREBASE_STR_7);
        if (p1 != std::string::npos)
        {
          p1 = linebuff.find(ESP32_FIREBASE_STR_102);
          if (p1 != std::string::npos)
          {
            dataObj._firebaseError = linebuff.substr(p1 + strlen(ESP32_FIREBASE_STR_102) + 1);
            dataObj._firebaseError = dataObj._firebaseError.substr(0, dataObj._firebaseError.length() - 1);
          }

          p1 = linebuff.find(ESP32_FIREBASE_STR_8);
          if (p1 != std::string::npos)
            if (linebuff.substr(p1 + strlen(ESP32_FIREBASE_STR_8)) == ESP32_FIREBASE_STR_9)
              isStream = true;

          p1 = linebuff.find(ESP32_FIREBASE_STR_10);
          if (p1 != std::string::npos)
          {
            if (linebuff.substr(p1 + strlen(ESP32_FIREBASE_STR_10)) == ESP32_FIREBASE_STR_11)
              dataObj._keepAlive = true;
            else
              dataObj._keepAlive = false;
          }

          p1 = linebuff.find(ESP32_FIREBASE_STR_12);
          if (p1 != std::string::npos)
            dataObj._contentLength = atoi(linebuff.substr(p1 + strlen(ESP32_FIREBASE_STR_12)).c_str());

          p1 = linebuff.find(ESP32_FIREBASE_STR_13);
          if (p1 != std::string::npos)
          {
            eventType = linebuff.substr(p1 + strlen(ESP32_FIREBASE_STR_13));
            hasEvent = true;
            isStream = true;
            dataObj._httpCode = HTTP_CODE_OK;
            linebuff.clear();
          }

          p1 = linebuff.find(ESP32_FIREBASE_STR_14);
          if (p1 != std::string::npos)
          {
            hasEventData = true;
            isStream = true;
            dataObj._httpCode = HTTP_CODE_OK;
            break;
          }
        }

        if (dataObj._httpCode == HTTP_CODE_OK && lfCount > 0 && linebuff.length() == 0 && dataObj._r_method == FirebaseMethod::GET_RULES)
          rulesBegin = true;

        if (!hasEventData || !hasEvent)
          linebuff.clear();

        lfCount++;
        charPos = 0;
      }

      if (millis() - dataTime > dataObj._net.tcpTimeout)
      {
        dataObj._httpCode = HTTPC_ERROR_READ_TIMEOUT;
        break;
      }
    }

    if (dataObj._httpCode == HTTP_CODE_OK || dataObj._httpCode == HTTP_CODE_PRECONDITION_FAILED)
    {
      if (rulesBegin)
      {
        jsonRes += linebuff;
        linebuff = jsonRes;
      }

      if (dataObj._r_method == FirebaseMethod::SET_RULES)
      {
        p1 = linebuff.find(ESP32_FIREBASE_STR_104);
        if (p1 != std::string::npos)
        {
          linebuff.clear();
        }
      }

      //JSON stream data?
      if (isStream)
      {
        if (hasEventData && hasEvent)
        {
          if (eventType == ESP32_FIREBASE_STR_15 || eventType == ESP32_FIREBASE_STR_16)
          {
            dataObj._eventType = eventType;

            //Parses json response for path
            p1 = linebuff.find(ESP32_FIREBASE_STR_17);
            if (p1 != std::string::npos)
            {
              p2 = linebuff.find(ESP32_FIREBASE_STR_3, p1 + strlen(ESP32_FIREBASE_STR_17));
              if (p2 != std::string::npos)
              {
                dataObj._path.clear();
                dataObj._path = linebuff.substr(p1 + strlen(ESP32_FIREBASE_STR_17), p2 - p1 - strlen(ESP32_FIREBASE_STR_17));
              }
            }

            //Parses json response for data
            p1 = linebuff.find(ESP32_FIREBASE_STR_18);
            if (p1 != std::string::npos)
            {
              dataObj._data.clear();
              dataObj._data = linebuff.substr(p1 + strlen(ESP32_FIREBASE_STR_18), linebuff.length() - p1 - strlen(ESP32_FIREBASE_STR_18) - 1);

              setDataType(dataObj, dataObj._data);
              bool samePath = dataObj._path == dataObj._path2;
              bool rootPath = dataObj._path == ESP32_FIREBASE_STR_1;
              bool emptyPath = dataObj._path2.length() == 0;
              bool sameData = dataObj._data == dataObj._data2;

              if (dataObj._data.length() >= strlen(ESP32_FIREBASE_STR_92) && !hasBlob)
              {
                if (dataObj._data.substr(0, strlen(ESP32_FIREBASE_STR_92)) == ESP32_FIREBASE_STR_92)
                {
                  linebuff.clear();
                  linebuff = dataObj._data;
                  dataObj._data = linebuff.substr(strlen(ESP32_FIREBASE_STR_92), linebuff.length() - strlen(ESP32_FIREBASE_STR_92) - 1);
                  hasBlob = true;
                  std::vector<uint8_t>().swap(dataObj._blob);
                  dataObj._dataType = FirebaseDataType::BLOB;
                  base64_decode_string(dataObj._data, dataObj._blob);
                  dataObj._data.clear();
                  dataObj._data2.clear();
                }
              }

              //Any stream update?
              if ((!samePath && (!rootPath || emptyPath)) || (samePath && !sameData && !dataObj._streamPathChanged))
              {

                dataObj._streamDataChanged = true;
                dataObj._data2.clear();
                dataObj._data2 = dataObj._data;
                dataObj._path2.clear();
                dataObj._path2 = dataObj._path;
              }
              else
                dataObj._streamDataChanged = false;
              dataObj._dataMillis = millis();
              flag = true;
              dataObj._dataAvailable = flag;
              dataObj._isStreamTimeout = false;
            }

            dataObj._streamPathChanged = false;
          }
          else
          {
            //Firebase keep alive event
            if (eventType == ESP32_FIREBASE_STR_11)
            {
              dataObj._isStreamTimeout = false;
              dataObj._dataMillis = millis();

              if (dataObj._timeoutCallback)
                dataObj._timeoutCallback(false);
            }

            //Firebase cancel and auth_revoked events
            else if (eventType == ESP32_FIREBASE_STR_109 || eventType == ESP32_FIREBASE_STR_110)
            {
              dataObj._isStreamTimeout = false;
              dataObj._dataMillis = millis();
              dataObj._eventType = eventType;
              //make stream available status
              dataObj._streamDataChanged = true;
              dataObj._dataAvailable = true;
            }
          }
        }
        dataObj._streamMillis = millis();
      }
      else
      {
        //Just text payload
        dataObj._data.clear();

        if (hasBlob)
        {
          dataObj._dataType = FirebaseDataType::BLOB;
          base64_decode_string(linebuff, dataObj._blob);
        }
        else
        {
          dataObj._data = linebuff;
          setDataType(dataObj, linebuff);

          if (dataObj._priority_val_flag)
          {
            char *path = new char[dataObj._path.length()];
            memset(path, 0, dataObj._path.length());
            strncpy(path, dataObj._path.c_str(), dataObj._path.length() - strlen_P(ESP32_FIREBASE_STR_156));
            dataObj._path = path;
            delete[] path;
          }

          //Push (POST) data?
          p1 = linebuff.find(ESP32_FIREBASE_STR_20);
          if (p1 != std::string::npos)
          {
            p2 = linebuff.find(ESP32_FIREBASE_STR_3, p1 + strlen(ESP32_FIREBASE_STR_20));
            if (p2 != std::string::npos)
            {
              dataObj._pushName.clear();
              dataObj._pushName = linebuff.substr(p1 + strlen(ESP32_FIREBASE_STR_20), p2 - p1 - strlen(ESP32_FIREBASE_STR_20));
              dataObj._dataType = -1;
              dataObj._dataType2 = -1;
              dataObj._data.clear();
            }
          }
        }
      }
    }
    else
    {
      if (dataObj._isSilentResponse)
      {
        if (dataObj._httpCode == HTTP_CODE_NO_CONTENT)
        {
          dataObj._httpCode = HTTP_CODE_OK;
          dataObj._path.clear();
          dataObj._data.clear();
          dataObj._pushName.clear();
          dataObj._dataType = -1;
          dataObj._dataType2 = -1;
          dataObj._dataAvailable = false;
        }
      }
      else
        dataObj._contentLength = -1;
    }

    if (dataObj._etag == ESP32_FIREBASE_STR_151)
      dataObj._pathNotExist = true;
    else
      dataObj._pathNotExist = false;

    if (dataObj._dataType != FirebaseDataType::NULL_)
    {

      bool _n1 = dataObj._dataType == FirebaseDataType::FLOAT || dataObj._dataType == FirebaseDataType::INTEGER;
      bool _n2 = dataObj._dataType2 == FirebaseDataType::FLOAT || dataObj._dataType2 == FirebaseDataType::INTEGER;

      if (dataObj._dataType2 == dataObj._dataType || (_n1 && _n2))
        dataObj._mismatchDataType = false;
      else
        dataObj._mismatchDataType = true;
    }

    if (!dataObj._httpCode)
    {
      dataObj._contentLength = -1;
      dataObj._httpCode = HTTPC_ERROR_NO_HTTP_SERVER;
    }
    dataObj._httpConnected = false;
    dataObj._streamMillis = millis();

    goto EXIT_2;
  }

  if (dataObj._httpCode == -1000 && dataObj._isStream)
    flag = true;

  dataObj._httpConnected = false;
  dataObj._streamMillis = millis();

  return flag;

EXIT_2:

  std::string().swap(linebuff);
  std::string().swap(eventType);
  std::string().swap(jsonRes);

  if (dataObj._httpCode == HTTPC_ERROR_READ_TIMEOUT)
    return false;
  return dataObj._httpCode == HTTP_CODE_OK || (dataObj._isStream && dataObj._httpCode == -1000);

EXIT_3:

  std::string().swap(linebuff);
  std::string().swap(eventType);
  std::string().swap(jsonRes);
  return true;

EXIT_4:
  std::string().swap(linebuff);
  std::string().swap(eventType);
  std::string().swap(jsonRes);
  return getServerResponse(dataObj);
}

bool FirebaseESP32::getDownloadResponse(FirebaseData &dataObj)
{

  if (dataObj._pause)
  {
    endFileTransfer(dataObj);
    return true;
  }

  if (!apConnected(dataObj))
    return false;

  if (!dataObj._net.getStreamPtr())
  {
    endFileTransfer(dataObj);
    return false;
  }

  dataObj._httpCode = -1000;
  char c = 0;
  size_t buffSize = 1024;
  char *buff = new char[buffSize + 1];

  std::string linebuff = "";
  std::string contentType = "";
  std::string contentDisposition = "";

  size_t contentLength = 0;
  int p1 = 0;
  int p2 = 0;
  bool beginPayload = false;
  int count = 0;
  size_t toRead = count;
  int cnt = 0;

  unsigned long dataTime = millis();

  while (dataObj._net.getStreamPtr()->connected() && !dataObj._net.getStreamPtr()->available() && millis() - dataTime < dataObj._net.tcpTimeout + 5000)
  {

    if (!apConnected(dataObj))
      return false;

    delay(1);
  }

  dataTime = millis();
  if (dataObj._net.getStreamPtr()->connected() && dataObj._net.getStreamPtr()->available())
  {

    while (dataObj._net.getStreamPtr()->available() || count > 0)
    {
      if (dataObj._interruptRequest)
        return cancelCurrentResponse(dataObj);

      if (!apConnected(dataObj))
        return false;

      if (!beginPayload)
      {
        c = dataObj._net.getStreamPtr()->read();
        if (c != '\r' && c != '\n')
          linebuff.append(1, c);
      }
      else
      {

        if (contentLength <= 4)
        {
          dataObj._httpCode = HTTP_CODE_NOT_FOUND;
          continue;
        }
        else
        {

          toRead = count;
          cnt = 0;
          yield();
          if (toRead > buffSize)
            toRead = buffSize;

          memset(buff, 0, buffSize + 1);
          cnt = 0;

          while (cnt < toRead)
          {

            c = dataObj._net.getStreamPtr()->read();
            if (c >= 0x20)
            {
              if (dataObj._fileName == "" && c < 0xff)
              {
                buff[cnt] = c;
                cnt++;
              }
              else if (dataObj._fileName != "" && c != '"')
              {
                buff[cnt] = c;
                cnt++;
              }
            }
          }

          if (cnt > 0)
          {
            count -= cnt;
            dataTime = millis();
            toRead = cnt;
            buff[cnt] = '\0';

            if (dataObj._storageType == StorageType::SPIFFS)
            {
              if (dataObj._fileName == "")
                file.write((uint8_t *)buff, toRead);
              else
                base64_decode_SPIFFS(file, buff, toRead);
            }
            else if (dataObj._storageType == StorageType::SD)
            {
              if (dataObj._fileName == "")
                file.write((uint8_t *)buff, toRead);
              else
                base64_decode_file(file, buff, toRead);
            }
          }

          if (count == 0)
            break;
        }
      }

      if (c == '\n' && !beginPayload)
      {

        dataTime = millis();

        p1 = linebuff.find(ESP32_FIREBASE_STR_5);
        if (p1 != std::string::npos)
        {
          p2 = linebuff.find(ESP32_FIREBASE_STR_6, p1 + strlen(ESP32_FIREBASE_STR_5));
          if (p2 != std::string::npos)
          {
            dataObj._httpCode = atoi(linebuff.substr(p1 + strlen(ESP32_FIREBASE_STR_5), p2 - p1 - strlen(ESP32_FIREBASE_STR_5)).c_str());
          }
        }

        p1 = linebuff.find(ESP32_FIREBASE_STR_102);
        if (p1 != std::string::npos)
        {
          dataObj._firebaseError = linebuff.substr(p1 + strlen(ESP32_FIREBASE_STR_102) + 1);
          dataObj._firebaseError = dataObj._firebaseError.substr(0, dataObj._firebaseError.length() - 1);
        }

        p1 = linebuff.find(ESP32_FIREBASE_STR_8);
        if (p1 != std::string::npos)
        {
          p2 = linebuff.find(ESP32_FIREBASE_STR_79, p1 + strlen(ESP32_FIREBASE_STR_8));
          if (p2 != std::string::npos)
          {
            contentType = linebuff.substr(p1 + strlen(ESP32_FIREBASE_STR_8), p2 - p1 - strlen(ESP32_FIREBASE_STR_8)).c_str();
          }
        }

        p1 = linebuff.find(ESP32_FIREBASE_STR_12);
        if (p1 != std::string::npos)
        {
          contentLength = atoi(linebuff.substr(p1 + strlen(ESP32_FIREBASE_STR_12)).c_str());
          dataObj._backupzFileSize = contentLength;
          count = contentLength;
          if (dataObj._fileName != "")
            count -= strlen(ESP32_FIREBASE_STR_93) - 1;
        }

        p1 = linebuff.find(ESP32_FIREBASE_STR_80);
        if (p1 != std::string::npos)
        {
          p2 = linebuff.find(ESP32_FIREBASE_STR_79, p1 + strlen(ESP32_FIREBASE_STR_80));
          if (p2 != std::string::npos)
          {
            contentDisposition = linebuff.substr(p1 + strlen(ESP32_FIREBASE_STR_80), p2 - p1 - strlen(ESP32_FIREBASE_STR_80)).c_str();
          }
        }

        if (dataObj._httpCode == HTTP_CODE_OK && linebuff.length() == 0 && contentLength > 0 && contentType == ESP32_FIREBASE_STR_81 && contentDisposition == ESP32_FIREBASE_STR_82)
        {
          beginPayload = true;
          if (dataObj._fileName != "")
          {
            for (int i = 0; i < strlen(ESP32_FIREBASE_STR_93); i++)
              dataObj._net.getStreamPtr()->read();
          }
        }

        linebuff.clear();
      }

      if (millis() - dataTime > dataObj._net.tcpTimeout)
      {
        dataObj._httpCode = HTTPC_ERROR_READ_TIMEOUT;
        break;
      }
    }
  }

  delete[] buff;

  std::string().swap(linebuff);
  std::string().swap(contentType);
  std::string().swap(contentDisposition);

  endFileTransfer(dataObj);

  if (dataObj._httpCode != -1000 && dataObj._httpCode != HTTP_CODE_OK)
  {
    dataObj._file_transfer_error.clear();
    if (dataObj._httpCode == HTTP_CODE_NOT_FOUND)
    {
      dataObj._file_transfer_error = ESP32_FIREBASE_STR_88;
    }
    else
    {
      dataObj._file_transfer_error = ESP32_FIREBASE_STR_87;
      dataObj._file_transfer_error += dataObj.errorReason().c_str();
    }
    return false;
  }

  return true;
}

bool FirebaseESP32::getUploadResponse(FirebaseData &dataObj)
{

  if (dataObj._pause)
    return true;

  if (!apConnected(dataObj))
    return false;

  if (!dataObj._net.getStreamPtr())
    return false;

  int _httpCode = -1000;
  char c = 0;

  std::string linebuff = "";
  int p1, p2;
  bool beginPayload = false;

  unsigned long dataTime = millis();

  if (!dataObj._isStream)
    while (dataObj._net.getStreamPtr()->connected() && !dataObj._net.getStreamPtr()->available() && millis() - dataTime < dataObj._net.tcpTimeout + 5000)
    {
      if (!apConnected(dataObj))
        return false;
      delay(1);
    }

  dataTime = millis();

  if (dataObj._net.getStreamPtr()->connected() && dataObj._net.getStreamPtr()->available())
  {

    while (dataObj._net.getStreamPtr()->available())
    {
      if (dataObj._interruptRequest)
        return cancelCurrentResponse(dataObj);

      if (!apConnected(dataObj))
        return false;

      c = dataObj._net.getStreamPtr()->read();

      if (c != '\r' && c != '\n')
        linebuff.append(1, c);

      if (c == '\n' && !beginPayload)
      {
        dataTime = millis();

        p1 = linebuff.find(ESP32_FIREBASE_STR_102);
        if (p1 != std::string::npos)
        {
          dataObj._firebaseError = linebuff.substr(p1 + strlen(ESP32_FIREBASE_STR_102) + 1);
          dataObj._firebaseError = dataObj._firebaseError.substr(0, dataObj._firebaseError.length() - 1);
        }

        p1 = linebuff.find(ESP32_FIREBASE_STR_5);
        if (p1 != std::string::npos)
        {
          p2 = linebuff.find(ESP32_FIREBASE_STR_6, p1 + strlen(ESP32_FIREBASE_STR_5));
          if (p2 != std::string::npos)
          {
            _httpCode = atoi(linebuff.substr(p1 + strlen(ESP32_FIREBASE_STR_5), p2 - p1 - strlen(ESP32_FIREBASE_STR_5)).c_str());
          }
        }

        linebuff.clear();
      }

      if (millis() - dataTime > dataObj._net.tcpTimeout)
      {
        _httpCode = HTTPC_ERROR_READ_TIMEOUT;
        dataObj._httpCode = HTTPC_ERROR_READ_TIMEOUT;
        break;
      }
    }
  }

  dataObj._file_transfering = false;

  if (_httpCode == HTTP_CODE_NO_CONTENT)
    dataObj._httpCode = HTTP_CODE_OK;
  else
    dataObj._httpCode = _httpCode;

  if (dataObj._httpCode != -1000 && _httpCode != HTTP_CODE_NO_CONTENT)
  {
    dataObj._file_transfer_error.clear();
    dataObj._file_transfer_error = ESP32_FIREBASE_STR_87;
    dataObj._file_transfer_error += dataObj.errorReason().c_str();
  }

  return _httpCode == HTTP_CODE_NO_CONTENT;
}

void FirebaseESP32::endFileTransfer(FirebaseData &dataObj)
{

  file.close();
  if (dataObj._storageType == StorageType::SD)
  {
    _sdInUse = false;
    _sdOk = false;
    SD.end();
  }

  dataObj._file_transfering = false;
}

bool FirebaseESP32::firebaseConnectStream(FirebaseData &dataObj, const std::string &path)
{

  if (dataObj._pause || dataObj._file_transfering)
    return true;

  dataObj._streamStop = false;

  if (!dataObj._isStreamTimeout && dataObj._isStream && path == dataObj._streamPath)
    return true;

  if (path.length() == 0 || _host.length() == 0 || _auth.length() == 0)
  {
    dataObj._httpCode = HTTP_CODE_BAD_REQUEST;
    return false;
  }

  if (!apConnected(dataObj))
    return false;

  if (millis() - dataObj._streamResetMillis > 50)
    delay(20);

  bool flag;
  flag = dataObj._streamPath.length() == 0;
  flag |= firebaseConnect(dataObj, path, FirebaseMethod::STREAM, FirebaseDataType::STRING, "", "") == 0;
  dataObj._dataMillis = millis();
  return flag;
}

bool FirebaseESP32::getServerStreamResponse(FirebaseData &dataObj)
{

  bool res = false;

  if (dataObj._pause)
    return true;

  if (dataObj._streamStop)
    return true;

  unsigned long ml = millis();
  if (dataObj._streamMillis == 0)
    dataObj._streamMillis = ml;
  if (dataObj._streamResetMillis == 0)
    dataObj._streamResetMillis = ml;

  //Reset firebase data every 50 ms and extend delay for 50 ms before stream response checking
  //to allow other http connection that may happen
  if (ml - dataObj._streamResetMillis > 50)
  {
    dataObj._streamResetMillis = ml;
    dataObj._streamDataChanged = false;
    dataObj._data2.clear();
    dataObj._path2.clear();
    dataObj._dataAvailable = false;
    dataObj._isStreamTimeout = false;
    if (dataObj._index == -1)
      delay(20);
    return true;
  }

  if (ml - dataObj._streamMillis > 50)
  {

    dataObj._streamMillis = ml;
    std::string path = "";

    //Stream timeout
    if (dataObj._dataMillis > 0 && millis() - dataObj._dataMillis > KEEP_ALIVE_TIMEOUT)
    {
      dataObj._dataMillis = millis();
      dataObj._isStreamTimeout = true;
      path = dataObj._streamPath;

      reconnect();

      if (!apConnected(dataObj))
        return false;

      while (dataObj._firebaseCall || dataObj._cfmCall)
        delay(1);

      dataObj._streamCall = true;

      firebaseConnectStream(dataObj, path);

      res = false;

      res = getServerResponse(dataObj);

      if (!dataObj._httpConnected)
        dataObj._httpCode = HTTPC_ERROR_NOT_CONNECTED;

      dataObj._streamCall = false;

      return res;
    }

    //last connection was not close
    if (dataObj._httpConnected)
      return true;

    dataObj._httpConnected = true;
    resetFirebasedataFlag(dataObj);

    if (!apConnected(dataObj))
      return false;

    while (dataObj._firebaseCall || dataObj._cfmCall)
      delay(1);

    dataObj._streamCall = true;

    if (dataObj._net.getStreamPtr())
    {
      if (dataObj._net.getStreamPtr()->connected() && !dataObj._isStream)
        forceEndHTTP(dataObj);

      if (dataObj._net.getStreamPtr())
      {

        if (!dataObj._net.getStreamPtr()->connected())
        {
          path = dataObj._streamPath;
          firebaseConnectStream(dataObj, path);
        }
      }
      else
      {
        path = dataObj._streamPath;
        firebaseConnectStream(dataObj, path);
      }
    }
    else
    {
      path = dataObj._streamPath;
      firebaseConnectStream(dataObj, path);
    }

    std::string().swap(path);

    bool res = false;

    res = getServerResponse(dataObj);

    if (!dataObj._httpConnected)
      dataObj._httpCode = HTTPC_ERROR_NOT_CONNECTED;

    dataObj._streamCall = false;

    return res;
  }

  return true;
}

bool FirebaseESP32::apConnected(FirebaseData &dataObj)
{
  if (WiFi.status() != WL_CONNECTED)
  {
    dataObj._httpCode = HTTPC_ERROR_CONNECTION_LOST;
    dataObj._firebaseCall = false;
    dataObj._streamCall = false;
    dataObj._cfmCall = false;
    return false;
  }
  return true;
}

void FirebaseESP32::forceEndHTTP(FirebaseData &dataObj)
{

  if (!apConnected(dataObj))
    return;

  if (dataObj._net.getStreamPtr())
  {
    if (dataObj._net.getStreamPtr()->available() > 0)
      dataObj._net.getStreamPtr()->read();

    dataObj._net.getStreamPtr()->stop();
    delay(20);
  }
}

void FirebaseESP32::processErrorQueue(FirebaseData &dataObj, QueueInfoCallback callback)
{
  if (dataObj._firebaseCall)
    return;

  if (!apConnected(dataObj))
    return;

  if (dataObj._qMan._queueCollection.size() > 0)
  {

    for (uint8_t i = 0; i < dataObj._qMan._queueCollection.size(); i++)
    {
      QueueItem item = dataObj._qMan._queueCollection[i];

      if (callback)
      {
        QueueInfo qinfo;
        qinfo._isQueue = true;
        qinfo._dataType = dataObj.getDataType(item.firebaseDataType);
        qinfo._path = item.path;
        qinfo._currentQueueID = item.qID;
        qinfo._method = dataObj.getMethod(item.firebaseMethod);
        qinfo._totalQueue = dataObj._qMan._queueCollection.size();
        qinfo._isQueueFull = dataObj._qMan._queueCollection.size() == dataObj._qMan._maxQueue;
        callback(qinfo);
      }

      if (item.firebaseMethod == FirebaseMethod::GET)
      {

        switch (item.firebaseDataType)
        {

        case FirebaseDataType::INTEGER:

          if (Firebase.getInt(dataObj, item.path.c_str()))
          {

            if (item.intPtr)
              *item.intPtr = dataObj.intData();

            dataObj.clearQueueItem(item);
            dataObj._qMan.remove(i);
          }

          break;

        case FirebaseDataType::FLOAT:

          if (Firebase.getFloat(dataObj, item.path.c_str()))
          {

            if (item.floatPtr)
              *item.floatPtr = dataObj.floatData();

            dataObj.clearQueueItem(item);
            dataObj._qMan.remove(i);
          }

          break;

        case FirebaseDataType::DOUBLE:

          if (Firebase.getDouble(dataObj, item.path.c_str()))
          {

            if (item.doublePtr)
              *item.doublePtr = dataObj.doubleData();

            dataObj.clearQueueItem(item);
            dataObj._qMan.remove(i);
          }

          break;

        case FirebaseDataType::BOOLEAN:

          if (Firebase.getBool(dataObj, item.path.c_str()))
          {

            if (item.boolPtr)
              *item.boolPtr = dataObj.boolData();

            dataObj.clearQueueItem(item);
            dataObj._qMan.remove(i);
          }

          break;

        case FirebaseDataType::STRING:

          if (Firebase.getString(dataObj, item.path.c_str()))
          {

            if (item.stringPtr)
              *item.stringPtr = dataObj.stringData();

            dataObj.clearQueueItem(item);
            dataObj._qMan.remove(i);
          }

          break;

        case FirebaseDataType::JSON:

          if (item.queryFilter._orderBy.length() > 0)
          {
            if (Firebase.getJSON(dataObj, item.path.c_str(), item.queryFilter))
            {

              if (item.stringPtr)
                *item.stringPtr = dataObj.jsonData();

              dataObj.clearQueueItem(item);
              dataObj._qMan.remove(i);
            }
          }
          else
          {
            if (Firebase.getJSON(dataObj, item.path.c_str()))
            {

              if (item.stringPtr)
                *item.stringPtr = dataObj.jsonData();

              dataObj.clearQueueItem(item);
              dataObj._qMan.remove(i);
            }
          }

          break;

        case FirebaseDataType::BLOB:

          if (Firebase.getBlob(dataObj, item.path.c_str()))
          {
            if (item.blobPtr)
              *item.blobPtr = dataObj.blobData();

            dataObj.clearQueueItem(item);
            dataObj._qMan.remove(i);
          }

          break;

        case FirebaseDataType::FILE:

          if (Firebase.getFile(dataObj, item.storageType, item.path.c_str(), item.filename.c_str()))
          {
            dataObj.clearQueueItem(item);
            dataObj._qMan.remove(i);
          }

          break;

        default:
          break;
        }
      }
      else if (item.firebaseMethod == FirebaseMethod::POST || item.firebaseMethod == FirebaseMethod::PUT || item.firebaseMethod == FirebaseMethod::PUT_SILENT || item.firebaseMethod == FirebaseMethod::PATCH || item.firebaseMethod == FirebaseMethod::PATCH_SILENT)
      {

        if (item.firebaseDataType == FirebaseDataType::FILE)
        {
          if (buildRequestFile(dataObj, item.storageType, item.firebaseMethod, item.path.c_str(), item.filename.c_str(), true, "", ""))
          {
            dataObj.clearQueueItem(item);
            dataObj._qMan.remove(i);
          }
        }
        else
        {
          if (buildRequest(dataObj, item.firebaseMethod, item.firebaseDataType, item.path.c_str(), item.payload.c_str(), true, "", ""))
          {
            dataObj.clearQueueItem(item);
            dataObj._qMan.remove(i);
          }
        }
      }
    }
  }
}

uint32_t FirebaseESP32::getErrorQueueID(FirebaseData &dataObj)
{
  return dataObj._qID;
}

bool FirebaseESP32::isErrorQueueExisted(FirebaseData &dataObj, uint32_t errorQueueID)
{

  for (uint8_t i = 0; i < dataObj._qMan._queueCollection.size(); i++)
  {
    QueueItem q = dataObj._qMan._queueCollection[i];
    if (q.qID == errorQueueID)
      return true;
  }
  return false;
}

void FirebaseESP32::buildFirebaseRequest(FirebaseData &dataObj, const std::string &host, uint8_t method, uint8_t dataType, const std::string &path, const std::string &auth, int payloadLength, std::string &request)
{
  uint8_t http_method = 0;
  char *contentLength = new char[20];
  memset(contentLength, 0, 20);

  char *temp = new char[20];
  memset(temp, 0, 20);
  dataObj._shallow_flag = false;
  dataObj._priority_val_flag = false;

  char *num = new char[20];

  request.clear();
  if (method == FirebaseMethod::STREAM)
  {
    request = ESP32_FIREBASE_STR_22;
    dataObj._isStream = true;
  }
  else
  {
    if (method == FirebaseMethod::PUT || method == FirebaseMethod::PUT_SILENT || method == FirebaseMethod::SET_PRIORITY || method == FirebaseMethod::SET_RULES)
    {
      http_method = FirebaseMethod::PUT;
      if (!dataObj._classicRequest)
        request = ESP32_FIREBASE_STR_23;
      else
        request = ESP32_FIREBASE_STR_24;
    }
    else if (method == FirebaseMethod::POST)
    {
      http_method = FirebaseMethod::POST;
      request = ESP32_FIREBASE_STR_24;
    }
    else if (method == FirebaseMethod::GET || method == FirebaseMethod::GET_SILENT || method == FirebaseMethod::GET_SHALLOW || method == FirebaseMethod::GET_PRIORITY || method == FirebaseMethod::BACKUP || method == FirebaseMethod::GET_RULES)
    {
      http_method = FirebaseMethod::GET;
      request = ESP32_FIREBASE_STR_25;
    }
    else if (method == FirebaseMethod::PATCH || method == FirebaseMethod::PATCH_SILENT || method == FirebaseMethod::RESTORE)
    {
      http_method = FirebaseMethod::PATCH;
      request = ESP32_FIREBASE_STR_26;
    }
    else if (method == FirebaseMethod::DELETE)
    {
      http_method = FirebaseMethod::DELETE;
      if (!dataObj._classicRequest)
        request = ESP32_FIREBASE_STR_23;
      else
        request = ESP32_FIREBASE_STR_27;
    }

    request += ESP32_FIREBASE_STR_6;
    dataObj._isStream = false;
  }

  if (path.length() > 0)
    if (path[0] != '/')
      request += ESP32_FIREBASE_STR_1;

  request += path;

  if (method == FirebaseMethod::PATCH || method == FirebaseMethod::PATCH_SILENT)
    request += ESP32_FIREBASE_STR_1;

  request += ESP32_FIREBASE_STR_2;
  request += auth;

  if (dataObj._readTimeout > 0)
  {
    request += ESP32_FIREBASE_STR_158;
    itoa(dataObj._readTimeout, temp, 10);
    request += temp;
    request += ESP32_FIREBASE_STR_159;
  }

  if (dataObj._writeLimit.length() > 0)
  {
    request += ESP32_FIREBASE_STR_160;
    request += dataObj._writeLimit;
  }

  if (method == FirebaseMethod::GET_SHALLOW)
  {
    request += ESP32_FIREBASE_STR_155;
    dataObj._shallow_flag = true;
  }

  if (method == FirebaseMethod::GET && dataObj.queryFilter._orderBy != "")
  {
    request += ESP32_FIREBASE_STR_96;
    request += dataObj.queryFilter._orderBy;

    if (method == FirebaseMethod::GET && dataObj.queryFilter._limitToFirst != "")
    {
      request += ESP32_FIREBASE_STR_97;
      request += dataObj.queryFilter._limitToFirst;
    }

    if (method == FirebaseMethod::GET && dataObj.queryFilter._limitToLast != "")
    {
      request += ESP32_FIREBASE_STR_98;
      request += dataObj.queryFilter._limitToLast;
    }

    if (method == FirebaseMethod::GET && dataObj.queryFilter._startAt != "")
    {
      request += ESP32_FIREBASE_STR_99;
      request += dataObj.queryFilter._startAt;
    }

    if (method == FirebaseMethod::GET && dataObj.queryFilter._endAt != "")
    {
      request += ESP32_FIREBASE_STR_100;
      request += dataObj.queryFilter._endAt;
    }

    if (method == FirebaseMethod::GET && dataObj.queryFilter._equalTo != "")
    {
      request += ESP32_FIREBASE_STR_101;
      request += dataObj.queryFilter._equalTo;
    }
  }

  if (method == FirebaseMethod::BACKUP)
  {
    request += ESP32_FIREBASE_STR_162;
    request += ESP32_FIREBASE_STR_28;
    std::string filename = "";

    for (int i = 0; i < dataObj._backupNodePath.length(); i++)
    {
      if (dataObj._backupNodePath.c_str()[i] == '/')
        filename += ESP32_FIREBASE_STR_4;
      else
        filename += dataObj._backupNodePath.c_str()[i];
    }

    request += filename;
    std::string().swap(filename);
  }

  if (method == FirebaseMethod::GET && dataObj._fileName != "")
  {
    request += ESP32_FIREBASE_STR_28;
    request += dataObj._fileName;
  }

  if (method == FirebaseMethod::GET_SILENT || method == FirebaseMethod::RESTORE || method == FirebaseMethod::PUT_SILENT || method == FirebaseMethod::PATCH_SILENT)
    request += ESP32_FIREBASE_STR_29;

  request += ESP32_FIREBASE_STR_30;
  request += ESP32_FIREBASE_STR_31;
  request += host;
  request += ESP32_FIREBASE_STR_21;
  request += ESP32_FIREBASE_STR_32;
  request += ESP32_FIREBASE_STR_33;

  //Timestamp cannot use with ETag header, otherwise cases internal server error
  if (dataObj.queryFilter._orderBy.length() == 0 && dataType != FirebaseDataType::TIMESTAMP && (method == FirebaseMethod::DELETE || method == FirebaseMethod::GET || method == FirebaseMethod::GET_SILENT || method == FirebaseMethod::PUT || method == FirebaseMethod::PUT_SILENT || method == FirebaseMethod::POST))
    request += ESP32_FIREBASE_STR_148;

  if (dataObj._etag2.length() > 0 && (method == FirebaseMethod::PUT || method == FirebaseMethod::PUT_SILENT || method == FirebaseMethod::DELETE))
  {
    request += ESP32_FIREBASE_STR_149;
    request += dataObj._etag2;
    request += ESP32_FIREBASE_STR_21;
  }

  if (dataObj._classicRequest && http_method != FirebaseMethod::GET && http_method != FirebaseMethod::POST && http_method != FirebaseMethod::PATCH)
  {
    request += ESP32_FIREBASE_STR_153;

    if (http_method == FirebaseMethod::PUT)
      request += ESP32_FIREBASE_STR_23;
    else if (http_method == FirebaseMethod::DELETE)
      request += ESP32_FIREBASE_STR_27;

    request += ESP32_FIREBASE_STR_21;
  }

  if (method == FirebaseMethod::STREAM)
  {
    request += ESP32_FIREBASE_STR_34;
    request += ESP32_FIREBASE_STR_35;
  }
  else if (method == FirebaseMethod::BACKUP || method == FirebaseMethod::RESTORE)
  {
    request += ESP32_FIREBASE_STR_34;
  }
  else
  {
    request += ESP32_FIREBASE_STR_36;
    request += ESP32_FIREBASE_STR_37;
  }

  if (method != FirebaseMethod::BACKUP && method != FirebaseMethod::RESTORE)
    request += ESP32_FIREBASE_STR_38;

  if (method == FirebaseMethod::GET_PRIORITY || method == FirebaseMethod::SET_PRIORITY)
    dataObj._priority_val_flag = true;

  if (method == FirebaseMethod::PUT || method == FirebaseMethod::PUT_SILENT || method == FirebaseMethod::POST || method == FirebaseMethod::PATCH || method == FirebaseMethod::PATCH_SILENT || method == FirebaseMethod::RESTORE || method == FirebaseMethod::SET_RULES || method == FirebaseMethod::SET_PRIORITY)
  {
    request += ESP32_FIREBASE_STR_12;
    itoa(payloadLength, contentLength, 10);
    request += contentLength;
  }
  request += ESP32_FIREBASE_STR_21;
  request += ESP32_FIREBASE_STR_21;

  delete[] contentLength;
  delete[] num;
  delete[] temp;
}

bool FirebaseESP32::cancelCurrentResponse(FirebaseData &dataObj)
{
  forceEndHTTP(dataObj);
  dataObj._isStream = false;
  dataObj._streamDataChanged = false;
  dataObj._dataMillis = millis();
  dataObj._data2.clear();
  dataObj._path2.clear();
  dataObj._dataAvailable = false;
  dataObj._isStreamTimeout = false;
  dataObj._httpCode = HTTPC_ERROR_CONNECTION_REFUSED;
  return false;
}

void FirebaseESP32::setDataType(FirebaseData &dataObj, const std::string &data)
{

  if (data.length() > 0)
  {
    if (data[0] == '{')
      dataObj._dataType = FirebaseDataType::JSON;
    else if (data == ESP32_FIREBASE_STR_106 || data == ESP32_FIREBASE_STR_107)
      dataObj._dataType = FirebaseDataType::BOOLEAN;
    else if (data.substr(0, strlen(ESP32_FIREBASE_STR_92)) == ESP32_FIREBASE_STR_92)
      dataObj._dataType = FirebaseDataType::BLOB;
    else if (data.substr(0, strlen(ESP32_FIREBASE_STR_93)) == ESP32_FIREBASE_STR_93)
      dataObj._dataType = FirebaseDataType::FILE;
    else if (data[0] == '"')
      dataObj._dataType = FirebaseDataType::STRING;
    else if (data.find(ESP32_FIREBASE_STR_4) != std::string::npos)
    {

      if (data.length() <= 7)
        dataObj._dataType = FirebaseDataType::FLOAT;
      else
        dataObj._dataType = FirebaseDataType::DOUBLE;
    }
    else if (data.find(ESP32_FIREBASE_STR_19) != std::string::npos)
      dataObj._dataType = FirebaseDataType::NULL_;
    else
    {
      double d = atof(data.c_str());
      if (d > 0x7fffffff)
        dataObj._dataType = FirebaseDataType::DOUBLE;
      else
        dataObj._dataType = FirebaseDataType::INTEGER;
    }

    if (data == ESP32_FIREBASE_STR_19 && dataObj.queryFilter._orderBy == "")
      dataObj._data.clear();
    else if (data == ESP32_FIREBASE_STR_19 && dataObj.queryFilter._orderBy != "")
    {
      dataObj._dataType = FirebaseDataType::JSON;
    }
  }
  else
  {
    dataObj._dataType = FirebaseDataType::NULL_;
  }

  dataObj._dataTypeNum = dataObj._dataType;
}

void FirebaseESP32::setSecure(FirebaseData &dataObj)
{
  if (dataObj._net._certType == -1)
  {

    if (_rootCAFile.length() == 0)
    {
      if (_rootCA)
        dataObj._net.setRootCA(_rootCA.get());
      else
        dataObj._net.setRootCA(nullptr);
    }
    else
    {
      if (_rootCAFileStoreageType == StorageType::SD)
      {
        if (!_sdOk)
          _sdOk = sdTest();

        if (!_sdOk)
          return;
      }

      dataObj._net.setRootCAFile(_rootCAFile, _rootCAFileStoreageType);
    }
  }
}

bool FirebaseESP32::commError(FirebaseData &dataObj)
{
  return dataObj._httpCode == HTTPC_ERROR_CONNECTION_REFUSED || dataObj._httpCode == HTTPC_ERROR_CONNECTION_LOST ||
         dataObj._httpCode == HTTPC_ERROR_SEND_PAYLOAD_FAILED || dataObj._httpCode == HTTPC_ERROR_SEND_HEADER_FAILED ||
         dataObj._httpCode == HTTPC_ERROR_NOT_CONNECTED || dataObj._httpCode == HTTPC_ERROR_READ_TIMEOUT;
}

void FirebaseESP32::resetFirebasedataFlag(FirebaseData &dataObj)
{
  dataObj._streamDataChanged = false;
  dataObj._streamPathChanged = false;
  dataObj._dataAvailable = false;
  dataObj._pushName.clear();
}
bool FirebaseESP32::handleNetNotConnected(FirebaseData &dataObj)
{
  if (!dataObj._net.connected())
  {
    dataObj._contentLength = -1;
    dataObj._dataType = FirebaseDataType::NULL_;
    dataObj._httpCode = HTTPC_ERROR_NOT_CONNECTED;
    dataObj._data.clear();
    dataObj._path.clear();
    resetFirebasedataFlag(dataObj);
    return false;
  }
  return true;
}

bool FirebaseESP32::sdBegin(uint8_t sck, uint8_t miso, uint8_t mosi, uint8_t ss)
{
  _sck = sck;
  _miso = miso;
  _mosi = mosi;
  _ss = ss;
  _sdConfigSet = true;
  SPI.begin(_sck, _miso, _mosi, _ss);
  return SD.begin(_ss, SPI);
}

bool FirebaseESP32::sdBegin(void)
{
  _sdConfigSet = false;
  return SD.begin();
}

void FirebaseESP32::reconnect()
{
  if (_reconnectWiFi && WiFi.status() != WL_CONNECTED)
  {
    uint8_t tryCount = 0;
    WiFi.reconnect();
    while (WiFi.status() != WL_CONNECTED)
    {
      tryCount++;
      delay(20);
      if (tryCount > 20)
        break;
    }
  }
}

void FirebaseESP32::errorToString(int httpCode, std::string &buff)
{
  buff.clear();
  switch (httpCode)
  {
  case HTTPC_ERROR_CONNECTION_REFUSED:
    buff = ESP32_FIREBASE_STR_39;
    return;
  case HTTPC_ERROR_SEND_HEADER_FAILED:
    buff = ESP32_FIREBASE_STR_40;
    return;
  case HTTPC_ERROR_SEND_PAYLOAD_FAILED:
    buff = ESP32_FIREBASE_STR_41;
    return;
  case HTTPC_ERROR_NOT_CONNECTED:
    buff = ESP32_FIREBASE_STR_42;
    return;
  case HTTPC_ERROR_CONNECTION_LOST:
    buff = ESP32_FIREBASE_STR_43;
    return;
  case HTTPC_ERROR_NO_HTTP_SERVER:
    buff = ESP32_FIREBASE_STR_44;
    return;
  case HTTP_CODE_BAD_REQUEST:
    buff = ESP32_FIREBASE_STR_45;
    return;
  case HTTP_CODE_NON_AUTHORITATIVE_INFORMATION:
    buff = ESP32_FIREBASE_STR_46;
    return;
  case HTTP_CODE_NO_CONTENT:
    buff = ESP32_FIREBASE_STR_47;
    return;
  case HTTP_CODE_MOVED_PERMANENTLY:
    buff = ESP32_FIREBASE_STR_48;
    return;
  case HTTP_CODE_USE_PROXY:
    buff = ESP32_FIREBASE_STR_49;
    return;
  case HTTP_CODE_TEMPORARY_REDIRECT:
    buff = ESP32_FIREBASE_STR_50;
    return;
  case HTTP_CODE_PERMANENT_REDIRECT:
    buff = ESP32_FIREBASE_STR_51;
    return;
  case HTTP_CODE_UNAUTHORIZED:
    buff = ESP32_FIREBASE_STR_52;
    return;
  case HTTP_CODE_FORBIDDEN:
    buff = ESP32_FIREBASE_STR_53;
    return;
  case HTTP_CODE_NOT_FOUND:
    buff = ESP32_FIREBASE_STR_54;
    return;
  case HTTP_CODE_METHOD_NOT_ALLOWED:
    buff = ESP32_FIREBASE_STR_55;
    return;
  case HTTP_CODE_NOT_ACCEPTABLE:
    buff = ESP32_FIREBASE_STR_56;
    return;
  case HTTP_CODE_PROXY_AUTHENTICATION_REQUIRED:
    buff = ESP32_FIREBASE_STR_57;
    return;
  case HTTP_CODE_REQUEST_TIMEOUT:
    buff = ESP32_FIREBASE_STR_58;
    return;
  case HTTP_CODE_LENGTH_REQUIRED:
    buff = ESP32_FIREBASE_STR_59;
    return;
  case HTTP_CODE_TOO_MANY_REQUESTS:
    buff = ESP32_FIREBASE_STR_60;
    return;
  case HTTP_CODE_REQUEST_HEADER_FIELDS_TOO_LARGE:
    buff = ESP32_FIREBASE_STR_61;
    return;
  case HTTP_CODE_INTERNAL_SERVER_ERROR:
    buff = ESP32_FIREBASE_STR_62;
    return;
  case HTTP_CODE_BAD_GATEWAY:
    buff = ESP32_FIREBASE_STR_63;
    return;
  case HTTP_CODE_SERVICE_UNAVAILABLE:
    buff = ESP32_FIREBASE_STR_64;
    return;
  case HTTP_CODE_GATEWAY_TIMEOUT:
    buff = ESP32_FIREBASE_STR_65;
    return;
  case HTTP_CODE_HTTP_VERSION_NOT_SUPPORTED:
    buff = ESP32_FIREBASE_STR_66;
    return;
  case HTTP_CODE_NETWORK_AUTHENTICATION_REQUIRED:
    buff = ESP32_FIREBASE_STR_67;
    return;
  case HTTP_CODE_PRECONDITION_FAILED:
    buff = ESP32_FIREBASE_STR_152;
    return;
  case HTTPC_ERROR_READ_TIMEOUT:
    buff = ESP32_FIREBASE_STR_69;
    return;
  case FIREBASE_ERROR_DATA_TYPE_MISMATCH:
    buff = ESP32_FIREBASE_STR_70;
    return;
  case FIREBASE_ERROR_PATH_NOT_EXIST:
    buff = ESP32_FIREBASE_STR_71;
    return;
  case HTTPC_ERROR_CONNECTION_INUSED:
    buff = ESP32_FIREBASE_STR_94;
    return;
  case HTTPC_NO_FCM_TOPIC_PROVIDED:
    buff = ESP32_FIREBASE_STR_144;
    return;
  case HTTPC_NO_FCM_DEVICE_TOKEN_PROVIDED:
    buff = ESP32_FIREBASE_STR_145;
    return;
  case HTTPC_NO_FCM_SERVER_KEY_PROVIDED:
    buff = ESP32_FIREBASE_STR_146;
    return;
  case HTTPC_NO_FCM_INDEX_NOT_FOUND_IN_DEVICE_TOKEN_PROVIDED:
    buff = ESP32_FIREBASE_STR_147;
    return;

  default:
    return;
  }
}

bool FirebaseESP32::sendFCMMessage(FirebaseData &dataObj, uint8_t messageType)
{

  if (dataObj.fcm._server_key.length() == 0)
  {
    dataObj._httpCode = HTTPC_NO_FCM_SERVER_KEY_PROVIDED;
    return false;
  }

  if (dataObj.fcm._deviceToken.size() == 0)
  {
    dataObj._httpCode = HTTPC_NO_FCM_DEVICE_TOKEN_PROVIDED;
    return false;
  }

  if (messageType == FirebaseESP32::FCMMessageType::SINGLE && dataObj.fcm._deviceToken.size() > 0 && dataObj.fcm._index > dataObj.fcm._deviceToken.size() - 1)
  {
    dataObj._httpCode = HTTPC_NO_FCM_INDEX_NOT_FOUND_IN_DEVICE_TOKEN_PROVIDED;
    return false;
  }

  if (messageType == FirebaseESP32::FCMMessageType::TOPIC && dataObj.fcm._topic.length() == 0)
  {
    dataObj._httpCode = HTTPC_NO_FCM_TOPIC_PROVIDED;
    return false;
  }

  //Try to reconnect WiFi if lost connection
  reconnect();

  //If WiFi is not connected, return false
  if (!apConnected(dataObj))
    return false;

  bool res = false;
  unsigned long lastTime = millis();

  if (dataObj._streamCall || dataObj._firebaseCall || dataObj._cfmCall)
    while ((dataObj._streamCall || dataObj._firebaseCall || dataObj._cfmCall) && millis() - lastTime < 1000)
      delay(1);

  if (dataObj._streamCall || dataObj._firebaseCall || dataObj._cfmCall)
  {
    dataObj._httpCode = HTTPC_ERROR_CONNECTION_INUSED;
    return false;
  }

  dataObj._cfmCall = true;

  if (dataObj._net.connected())
    forceEndHTTP(dataObj);

  setSecure(dataObj);
  res = dataObj.fcm.fcm_connect(dataObj._net);

  if (!res)
  {
    dataObj._httpCode = HTTPC_ERROR_CONNECTION_REFUSED;
    dataObj._cfmCall = false;
    return false;
  }

  res = dataObj.fcm.fcm_send(dataObj._net, dataObj._httpCode, messageType);
  dataObj._cfmCall = false;
  return res;
}

bool FirebaseESP32::sendMessage(FirebaseData &dataObj, uint16_t index)
{
  dataObj.fcm._index = index;
  return sendFCMMessage(dataObj, FirebaseESP32::FCMMessageType::SINGLE);
}

bool FirebaseESP32::broadcastMessage(FirebaseData &dataObj)
{
  return sendFCMMessage(dataObj, FirebaseESP32::FCMMessageType::MULTICAST);
}

bool FirebaseESP32::sendTopic(FirebaseData &dataObj)
{
  return sendFCMMessage(dataObj, FirebaseESP32::FCMMessageType::TOPIC);
}

void FirebaseESP32::setStreamCallback(FirebaseData &dataObj, StreamEventCallback dataAvailablecallback, StreamTimeoutCallback timeoutCallback)
{
  int index = dataObj._index;

  std::string taskName = ESP32_FIREBASE_STR_72;
  size_t idxSize = 20;
  char *idx = new char[idxSize];
  memset(idx, 0, idxSize);

  bool hasHandle = false;

  if (dataObj._handle)
  {
    hasHandle = true;
    vTaskDelete(dataObj._handle);
    dataObj._handle = NULL;
  }

  if (dataObj._q_handle)
  {
    hasHandle = true;
  }

  memset(idx, 0, idxSize);

  if (index == -1)
  {
    index = dataObjectIndex;
    dataObjectIndex++;
  }

  itoa(index, idx, 10);
  taskName += ESP32_FIREBASE_STR_113;
  taskName += idx;

  dataObj._index = index;
  streamIndex = index;
  dataObj._dataAvailableCallback = dataAvailablecallback;
  dataObj._timeoutCallback = timeoutCallback;

  //object created
  if (hasHandle)
    firebaseDataObject[index] = dataObj;
  else
    firebaseDataObject.push_back(dataObj);

  delete[] idx;

  TaskFunction_t taskCode = [](void *param) {
    int id = streamIndex;

    for (;;)
    {
      if ((firebaseDataObject[id].get()._dataAvailableCallback || firebaseDataObject[id].get()._timeoutCallback))
      {

        Firebase.readStream(firebaseDataObject[id].get());

        if (firebaseDataObject[id].get().streamTimeout() && firebaseDataObject[id].get()._timeoutCallback)
          firebaseDataObject[id].get()._timeoutCallback(true);

        if (firebaseDataObject[id].get().streamAvailable() && firebaseDataObject[id].get()._dataAvailableCallback)
        {
          StreamData s;
          s._json = &firebaseDataObject[id].get()._json;
          s._streamPath = firebaseDataObject[id].get()._streamPath;
          s._data = firebaseDataObject[id].get()._data;
          s._path = firebaseDataObject[id].get()._path;

          s._dataType = firebaseDataObject[id].get()._dataType;
          s._dataTypeStr = firebaseDataObject[id].get().getDataType(s._dataType);
          s._eventTypeStr = firebaseDataObject[id].get()._eventType;

          if (s._dataType == FirebaseESP32::FirebaseDataType::BLOB)
          {
            s._blob = firebaseDataObject[id].get()._blob;
            //Free ram in case of callback data was used
            firebaseDataObject[id].get()._blob.clear();
          }

          firebaseDataObject[id].get()._dataAvailableCallback(s);
          s.empty();
        }
      }

      yield();
      vTaskDelay(30);
    }

    vTaskDelete(NULL);
    firebaseDataObject[id].get()._handle = NULL;
  };

  xTaskCreatePinnedToCore(taskCode, taskName.c_str(), 10000, NULL, 3, &dataObj._handle, 1);
}

void FirebaseESP32::removeStreamCallback(FirebaseData &dataObj)
{
  int index = dataObj._index;

  if (index != -1)
  {
    bool hasOherHandle = false;

    if (dataObj._q_handle)
    {
      hasOherHandle = true;
    }

    if (!hasOherHandle)
      dataObj._index = -1;

    if (dataObj._handle)
      vTaskDelete(dataObj._handle);

    dataObj._handle = NULL;

    dataObj._dataAvailableCallback = NULL;
    dataObj._timeoutCallback = NULL;

    if (!hasOherHandle)
      firebaseDataObject.erase(firebaseDataObject.begin() + index);
  }
}

void FirebaseESP32::beginAutoRunErrorQueue(FirebaseData &dataObj, QueueInfoCallback callback)
{

  int index = dataObj._index;

  std::string taskName = ESP32_FIREBASE_STR_72;
  size_t idxSize = 20;
  char *idx = new char[idxSize];
  memset(idx, 0, idxSize);

  bool hasHandle = false;

  if (dataObj._handle || dataObj._q_handle)
  {
    hasHandle = true;
  }

  memset(idx, 0, idxSize);

  if (index == -1)
  {
    index = dataObjectIndex;
    dataObjectIndex++;
  }

  itoa(index, idx, 10);
  taskName += ESP32_FIREBASE_STR_114;
  taskName += idx;

  if (callback)
    dataObj._queueInfoCallback = callback;
  else
    dataObj._queueInfoCallback = NULL;

  dataObj._index = index;
  errorQueueIndex = index;

  //object created
  if (hasHandle)
    firebaseDataObject[index] = dataObj;
  else
    firebaseDataObject.push_back(dataObj);

  delete[] idx;

  TaskFunction_t taskCode = [](void *param) {
    int id = errorQueueIndex;

    for (;;)
    {

      if (firebaseDataObject[id].get()._queueInfoCallback)
        Firebase.processErrorQueue(firebaseDataObject[id].get(), firebaseDataObject[id].get()._queueInfoCallback);
      else
        Firebase.processErrorQueue(firebaseDataObject[id].get(), NULL);

      yield();
      vTaskDelay(200);
    }

    vTaskDelete(NULL);
    firebaseDataObject[id].get()._q_handle = NULL;
  };

  xTaskCreatePinnedToCore(taskCode, taskName.c_str(), 10000, NULL, 3, &dataObj._q_handle, 1);
}

void FirebaseESP32::endAutoRunErrorQueue(FirebaseData &dataObj)
{
  int index = dataObj._index;

  if (index != -1)
  {
    bool hasOherHandle = false;

    if (dataObj._handle)
    {
      hasOherHandle = true;
    }

    if (!hasOherHandle)
      dataObj._index = -1;

    if (dataObj._q_handle)
      vTaskDelete(dataObj._q_handle);

    dataObj._q_handle = NULL;
    dataObj._queueInfoCallback = NULL;

    if (!hasOherHandle)
      firebaseDataObject.erase(firebaseDataObject.begin() + index);
  }
}
void FirebaseESP32::clearErrorQueue(FirebaseData &dataObj)
{
  for (uint8_t i = 0; i < dataObj._qMan._queueCollection.size(); i++)
  {
    QueueItem item = dataObj._qMan._queueCollection[i];
    dataObj.clearQueueItem(item);
  }
}

bool FirebaseESP32::backup(FirebaseData &dataObj, uint8_t storageType, const String &nodePath, const String &dirPath)
{
  dataObj._backupDir = dirPath.c_str();
  dataObj._backupNodePath = nodePath.c_str();
  dataObj._backupFilename.clear();
  dataObj._fileName.clear();
  bool flag = sendRequest(dataObj, storageType, nodePath.c_str(), FirebaseMethod::BACKUP, FirebaseDataType::JSON, "", "", "");
  return flag;
}

bool FirebaseESP32::restore(FirebaseData &dataObj, uint8_t storageType, const String &nodePath, const String &dirPath)
{
  dataObj._backupDir = dirPath.c_str();
  dataObj._backupNodePath = nodePath.c_str();
  dataObj._backupFilename.clear();
  dataObj._fileName.clear();
  bool flag = sendRequest(dataObj, storageType, nodePath.c_str(), FirebaseMethod::RESTORE, FirebaseDataType::JSON, "", "", "");
  return flag;
}

bool FirebaseESP32::sdTest()
{

  if (_sdConfigSet)
    sdBegin(_sck, _miso, _mosi, _ss);
  else
    sdBegin();

  File file = SD.open(ESP32_FIREBASE_STR_73, FILE_WRITE);
  if (!file)
    return false;

  if (!file.write(32))
    return false;
  file.close();

  file = SD.open(ESP32_FIREBASE_STR_73);
  if (!file)
    return false;

  while (file.available())
  {
    if (file.read() != 32)
      return false;
  }
  file.close();

  SD.remove(ESP32_FIREBASE_STR_73);

  return true;
}

void FirebaseESP32::setMaxRetry(FirebaseData &dataObj, uint8_t num)
{
  dataObj._maxRetry = num;
}

void FirebaseESP32::setMaxErrorQueue(FirebaseData &dataObj, uint8_t num)
{
  dataObj._qMan._maxQueue = num;

  if (dataObj._qMan._queueCollection.size() > num)
  {
    for (uint8_t i = dataObj._qMan._queueCollection.size() - 1; i >= num; i--)
    {
      QueueItem item = dataObj._qMan._queueCollection[i];
      dataObj.clearQueueItem(item);
    }
  }
}

bool FirebaseESP32::saveErrorQueue(FirebaseData &dataObj, const String &filename, uint8_t storageType)
{
   if (storageType == StorageType::SD)
    {
        if (!sdTest())
            return false;
        file = SD.open(filename.c_str(), FILE_WRITE);
    }
    else if (storageType == StorageType::SPIFFS)
    {
        SPIFFS.begin();
        file = SPIFFS.open(filename.c_str(), "w");
    }

    if (!file)
      return false;

  uint8_t idx = 0;
  std::string buff = "";

  size_t nbuffSize = 10;
  char *nbuff = new char[nbuffSize];

  for (uint8_t i = 0; i < dataObj._qMan._queueCollection.size(); i++)
  {
    QueueItem item = dataObj._qMan._queueCollection[i];

    if (item.firebaseMethod != FirebaseESP32::FirebaseMethod::GET)
    {
      if (idx > 0)
        buff.append("\r");

      memset(nbuff, 0, nbuffSize);
      itoa(item.firebaseDataType, nbuff, 10);
      buff.append(nbuff);
      buff.append("~");

      memset(nbuff, 0, nbuffSize);
      itoa(item.firebaseMethod, nbuff, 10);
      buff.append(nbuff);
      buff.append("~");

      buff += item.path.c_str();
      buff.append("~");

      buff += item.payload.c_str();
      buff.append("~");

      for (int j = 0; j < item.blob.size(); j++)
      {
        memset(nbuff, 0, nbuffSize);
        itoa(item.blob[j], nbuff, 10);
      }
      buff.append("~");

      buff += item.filename.c_str();

      idx++;
    }
  }

  file.print(buff.c_str());
  file.close();

  delete[] nbuff;
  std::string().swap(buff);

  return true;
}

bool FirebaseESP32::restoreErrorQueue(FirebaseData &dataObj, const String &filename, uint8_t storageType)
{
  return openErrorQueue(dataObj, filename, storageType, 1) != 0;
}

uint8_t FirebaseESP32::errorQueueCount(FirebaseData &dataObj, const String &filename, uint8_t storageType)
{
  return openErrorQueue(dataObj, filename, storageType, 0);
}

bool FirebaseESP32::deleteStorageFile(const String &filename, uint8_t storageType)
{

  if (storageType == StorageType::SD)
  {
    if (!sdTest())
      return false;
    return SD.remove(filename.c_str());
  }
  else
  {
    SPIFFS.begin(true);
    return SPIFFS.remove(filename.c_str());
  }
}

uint8_t FirebaseESP32::openErrorQueue(FirebaseData &dataObj, const String &filename, uint8_t storageType, uint8_t mode)
{

  uint8_t count = 0;

  File file;

  if (storageType == StorageType::SD)
  {

    if (!sdTest())
      return 0;

    file = SD.open(filename.c_str(), FILE_READ);
  }
  else
  {

    SPIFFS.begin(true);

    file = SPIFFS.open(filename.c_str(), "r");
  }

  if (!file)
    return 0;

  std::string t = "";

  uint8_t c = 0;

  while (file.available())
  {
    c = file.read();
    t += (char)c;
  }

  file.close();

  std::vector<std::string> p = splitString(dataObj._maxBlobSize, t.c_str(), '\r');

  for (int i = 0; i < p.size(); i++)
  {

    std::vector<std::string> q = splitString(dataObj._maxBlobSize, p[i].c_str(), '~');

    if (q.size() == 6)
    {
      count++;

      if (mode == 1)
      {

        //Restore Firebase Error Queues
        QueueItem item;

        item.firebaseDataType = atoi(q[0].c_str());
        item.firebaseMethod = atoi(q[1].c_str());
        item.path.append(q[2].c_str());
        item.payload.append(q[3].c_str());

        for (int j = 0; j < q[4].length(); j++)
          item.blob.push_back(atoi(q[4].c_str()));

        item.filename.append(q[5].c_str());

        //backwards compatibility to old APIs
        if (q.size() == 7)
          item.storageType = atoi(q[6].c_str());

        dataObj._qMan._queueCollection.push_back(item);
      }
    }
  }
  std::string().swap(t);

  return count;
}

bool FirebaseESP32::isErrorQueueFull(FirebaseData &dataObj)
{
  if (dataObj._qMan._maxQueue > 0)
    return dataObj._qMan._queueCollection.size() >= dataObj._qMan._maxQueue;
  return false;
}

uint8_t FirebaseESP32::errorQueueCount(FirebaseData &dataObj)
{
  return dataObj._qMan._queueCollection.size();
}

std::vector<std::string> FirebaseESP32::splitString(int size, const char *str, const char delim)
{
  uint16_t index = 0;
  uint16_t len = strlen(str);
  int buffSize = (int)(size * 1.4f);
  char *buff = new char[buffSize];
  std::vector<std::string> out;

  for (uint16_t i = 0; i < len; i++)
  {
    if (str[i] == delim)
    {
      memset(buff, 0, buffSize);
      strncpy(buff, (char *)str + index, i - index);
      buff[i - index] = '\0';
      index = i + 1;
      out.push_back(buff);
    }
  }
  if (index < len + 1)
  {
    memset(buff, 0, buffSize);
    strncpy(buff, (char *)str + index, len - index);
    buff[len - index] = '\0';
    out.push_back(buff);
  }

  delete[] buff;

  return out;
}

void FirebaseESP32::createDirs(std::string dirs, uint8_t storageType)
{
  std::string dir = "";
  size_t count = 0;
  for (size_t i = 0; i < dirs.length(); i++)
  {
    dir.append(1, dirs[i]);
    count++;
    if (dirs[i] == '/')
    {
      if (dir.length() > 0)
      {
        if (storageType == StorageType::SD)
          SD.mkdir(dir.substr(0, dir.length() - 1).c_str());
      }

      count = 0;
    }
  }
  if (count > 0)
  {
    if (storageType == StorageType::SD)
      SD.mkdir(dir.c_str());
  }
  std::string().swap(dir);
}

bool FirebaseESP32::replace(std::string &str, const std::string &from, const std::string &to)
{
  size_t start_pos = str.find(from);
  if (start_pos == std::string::npos)
    return false;
  str.replace(start_pos, from.length(), to);
  return true;
}

std::string FirebaseESP32::base64_encode_string(const unsigned char *src, size_t len)
{
  unsigned char *out, *pos;
  const unsigned char *end, *in;

  size_t olen;

  olen = 4 * ((len + 2) / 3); /* 3-byte blocks to 4-byte */

  if (olen < len)
    return std::string();

  std::string outStr;
  outStr.resize(olen);
  out = (unsigned char *)&outStr[0];

  end = src + len;
  in = src;
  pos = out;

  while (end - in >= 3)
  {
    *pos++ = ESP32_FIREBASE_base64_table[in[0] >> 2];
    *pos++ = ESP32_FIREBASE_base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
    *pos++ = ESP32_FIREBASE_base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
    *pos++ = ESP32_FIREBASE_base64_table[in[2] & 0x3f];
    in += 3;
    yield();
  }

  if (end - in)
  {
    *pos++ = ESP32_FIREBASE_base64_table[in[0] >> 2];
    if (end - in == 1)
    {
      *pos++ = ESP32_FIREBASE_base64_table[(in[0] & 0x03) << 4];
      *pos++ = '=';
    }
    else
    {
      *pos++ = ESP32_FIREBASE_base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
      *pos++ = ESP32_FIREBASE_base64_table[(in[1] & 0x0f) << 2];
    }
    *pos++ = '=';
  }

  return outStr;
}

void FirebaseESP32::send_base64_encode_file(WiFiClient *net, const std::string &filePath, uint8_t storageType)
{

  if (storageType == StorageType::SPIFFS)
    file = SPIFFS.open(filePath.c_str(), "r");
  else if (storageType == StorageType::SD)
    file = SD.open(filePath.c_str(), FILE_READ);

  if (!file)
    return;

  size_t chunkSize = 512;
  size_t fbuffSize = 3;
  size_t byteAdd = 0;
  size_t byteSent = 0;

  unsigned char *buff = new unsigned char[chunkSize];
  memset(buff, 0, chunkSize);

  size_t len = file.size();
  size_t fbuffIndex = 0;
  unsigned char *fbuff = new unsigned char[3];

  while (file.available())
  {
    memset(fbuff, 0, fbuffSize);
    if (len - fbuffIndex >= 3)
    {
      file.read(fbuff, 3);

      buff[byteAdd++] = ESP32_FIREBASE_base64_table[fbuff[0] >> 2];
      buff[byteAdd++] = ESP32_FIREBASE_base64_table[((fbuff[0] & 0x03) << 4) | (fbuff[1] >> 4)];
      buff[byteAdd++] = ESP32_FIREBASE_base64_table[((fbuff[1] & 0x0f) << 2) | (fbuff[2] >> 6)];
      buff[byteAdd++] = ESP32_FIREBASE_base64_table[fbuff[2] & 0x3f];

      if (len > chunkSize)
      {
        if (byteAdd >= chunkSize)
        {
          byteSent += byteAdd;
          net->write(buff, byteAdd);
          memset(buff, 0, chunkSize);
          byteAdd = 0;
        }
      }
      fbuffIndex += 3;

      yield();
    }
    else
    {

      if (len - fbuffIndex == 1)
      {
        fbuff[0] = file.read();
      }
      else if (len - fbuffIndex == 2)
      {
        fbuff[0] = file.read();
        fbuff[1] = file.read();
      }

      break;
    }
  }

  file.close();

  if (byteAdd > 0)
    net->write(buff, byteAdd);

  if (len - fbuffIndex > 0)
  {

    memset(buff, 0, chunkSize);
    byteAdd = 0;

    buff[byteAdd++] = ESP32_FIREBASE_base64_table[fbuff[0] >> 2];
    if (len - fbuffIndex == 1)
    {
      buff[byteAdd++] = ESP32_FIREBASE_base64_table[(fbuff[0] & 0x03) << 4];
      buff[byteAdd++] = '=';
    }
    else
    {
      buff[byteAdd++] = ESP32_FIREBASE_base64_table[((fbuff[0] & 0x03) << 4) | (fbuff[1] >> 4)];
      buff[byteAdd++] = ESP32_FIREBASE_base64_table[(fbuff[1] & 0x0f) << 2];
    }
    buff[byteAdd++] = '=';

    net->write(buff, byteAdd);
  }

  delete[] buff;
  delete[] fbuff;
}

bool FirebaseESP32::base64_decode_string(const std::string src, std::vector<uint8_t> &out)
{
  unsigned char *dtable = new unsigned char[256];
  memset(dtable, 0x80, 256);
  for (size_t i = 0; i < sizeof(ESP32_FIREBASE_base64_table) - 1; i++)
    dtable[ESP32_FIREBASE_base64_table[i]] = (unsigned char)i;
  dtable['='] = 0;

  unsigned char *block = new unsigned char[4];
  unsigned char tmp;
  size_t i, count;
  int pad = 0;
  size_t extra_pad;
  size_t len = src.length();

  count = 0;

  for (i = 0; i < len; i++)
  {
    if ((uint8_t)dtable[(uint8_t)src[i]] != 0x80)
      count++;
  }

  if (count == 0)
    goto exit;

  extra_pad = (4 - count % 4) % 4;

  count = 0;
  for (i = 0; i < len + extra_pad; i++)
  {
    unsigned char val;

    if (i >= len)
      val = '=';
    else
      val = src[i];

    tmp = dtable[val];

    if (tmp == 0x80)
      continue;

    if (val == '=')
      pad++;

    block[count] = tmp;
    count++;
    if (count == 4)
    {
      out.push_back((block[0] << 2) | (block[1] >> 4));
      count = 0;
      if (pad)
      {
        if (pad == 1)
          out.push_back((block[1] << 4) | (block[2] >> 2));
        else if (pad > 2)
          goto exit;

        break;
      }
      else
      {
        out.push_back((block[1] << 4) | (block[2] >> 2));
        out.push_back((block[2] << 6) | block[3]);
      }
    }
  }

  delete[] block;
  delete[] dtable;

  return true;

exit:
  delete[] block;
  delete[] dtable;
  return false;
}

bool FirebaseESP32::base64_decode_file(File &file, const char *src, size_t len)
{
  unsigned char *dtable = new unsigned char[256];
  memset(dtable, 0x80, 256);
  for (size_t i = 0; i < sizeof(ESP32_FIREBASE_base64_table) - 1; i++)
    dtable[ESP32_FIREBASE_base64_table[i]] = (unsigned char)i;
  dtable['='] = 0;

  unsigned char *block = new unsigned char[4];
  unsigned char tmp;
  size_t i, count;
  int pad = 0;
  size_t extra_pad;

  count = 0;

  for (i = 0; i < len; i++)
  {
    if ((uint8_t)dtable[(uint8_t)src[i]] != 0x80)
      count++;
  }

  if (count == 0)
    goto exit;

  extra_pad = (4 - count % 4) % 4;

  count = 0;
  for (i = 0; i < len + extra_pad; i++)
  {

    unsigned char val;

    if (i >= len)
      val = '=';
    else
      val = src[i];
    tmp = dtable[val];
    if (tmp == 0x80)
      continue;

    if (val == '=')
      pad++;

    block[count] = tmp;
    count++;
    if (count == 4)
    {
      file.write((block[0] << 2) | (block[1] >> 4));
      count = 0;
      if (pad)
      {
        if (pad == 1)
          file.write((block[1] << 4) | (block[2] >> 2));
        else if (pad > 2)
          goto exit;
        break;
      }
      else
      {
        file.write((block[1] << 4) | (block[2] >> 2));
        file.write((block[2] << 6) | block[3]);
      }
    }
  }

  delete[] block;
  delete[] dtable;

  return true;

exit:
  delete[] block;
  delete[] dtable;
  return false;
}

bool FirebaseESP32::base64_decode_SPIFFS(File &file, const char *src, size_t len)
{
  unsigned char *dtable = new unsigned char[256];
  memset(dtable, 0x80, 256);
  for (size_t i = 0; i < sizeof(ESP32_FIREBASE_base64_table) - 1; i++)
    dtable[ESP32_FIREBASE_base64_table[i]] = (unsigned char)i;
  dtable['='] = 0;

  unsigned char *block = new unsigned char[4];
  unsigned char tmp;
  size_t i, count;
  int pad = 0;
  size_t extra_pad;

  count = 0;

  for (i = 0; i < len; i++)
  {
    if (dtable[(uint8_t)src[i]] != 0x80)
      count++;
  }

  if (count == 0)
    goto exit;

  extra_pad = (4 - count % 4) % 4;

  count = 0;
  for (i = 0; i < len + extra_pad; i++)
  {
    unsigned char val;

    if (i >= len)
      val = '=';
    else
      val = src[i];
    tmp = dtable[val];
    if (tmp == 0x80)
      continue;

    if (val == '=')
      pad++;

    block[count] = tmp;
    count++;
    if (count == 4)
    {
      file.write((block[0] << 2) | (block[1] >> 4));
      count = 0;
      if (pad)
      {
        if (pad == 1)
          file.write((block[1] << 4) | (block[2] >> 2));
        else if (pad > 2)
          goto exit;
        break;
      }
      else
      {
        file.write((block[1] << 4) | (block[2] >> 2));
        file.write((block[2] << 6) | block[3]);
      }
    }
  }

  delete[] block;
  delete[] dtable;

  return true;

exit:
  delete[] block;
  delete[] dtable;

  return false;
}

FirebaseData::FirebaseData()
{
  _index = -1;
}

FirebaseData ::~FirebaseData()
{
  clear();
}

void FirebaseData::clear()
{
  std::string().swap(_path);
  std::string().swap(_path2);
  std::string().swap(_data);
  std::string().swap(_data2);
  std::string().swap(_streamPath);
  std::string().swap(_pushName);
  std::string().swap(_backupNodePath);
  std::string().swap(_backupDir);
  std::string().swap(_backupFilename);
  std::string().swap(_file_transfer_error);
  std::string().swap(_fileName);
  std::string().swap(_redirectURL);
  std::string().swap(_firebaseError);
  std::string().swap(_eventType);
  std::string().swap(_etag);
  std::string().swap(_etag2);
  std::string().swap(_priority);

  for (uint8_t i = 0; i < _qMan._queueCollection.size(); i++)
  {
    QueueItem item = _qMan._queueCollection[i];
    clearQueueItem(item);
  }

  _dataAvailableCallback = NULL;
  _timeoutCallback = NULL;
  _queueInfoCallback = NULL;
  _handle = NULL;
  _q_handle = NULL;
}

void FirebaseData::addQueue(uint8_t FirebaseMethod,
                            uint8_t storageType,
                            uint8_t FirebaseDataType,
                            const std::string path,
                            const std::string filename,
                            const std::string payload,
                            bool isQuery,
                            int *intTarget,
                            float *floatTarget,
                            double *doubleTarget,
                            bool *boolTarget,
                            String *stringTarget,
                            std::vector<uint8_t> *blobTarget)
{
  if (_qMan._queueCollection.size() < _qMan._maxQueue && payload.length() <= _maxBlobSize)
  {
    QueueItem item;
    item.firebaseMethod = FirebaseMethod;
    item.firebaseDataType = FirebaseDataType;
    item.path = path;
    item.filename = filename;
    item.payload = payload;

    if (isQuery)
      item.queryFilter = queryFilter;
    else
      item.queryFilter.clear();

    item.stringPtr = stringTarget;
    item.intPtr = intTarget;
    item.floatPtr = floatTarget;
    item.doublePtr = doubleTarget;
    item.boolPtr = boolTarget;
    item.blobPtr = blobTarget;
    item.qID = random(100000, 200000);
    item.storageType = storageType;
    if (_qMan.add(item))
      _qID = item.qID;
    else
      _qID = 0;
  }
}

void FirebaseData::clearQueueItem(QueueItem &item)
{
  std::string().swap(item.path);
  std::string().swap(item.filename);
  std::string().swap(item.payload);

  item.stringPtr = nullptr;
  item.intPtr = nullptr;
  item.floatPtr = nullptr;
  item.doublePtr = nullptr;
  item.boolPtr = nullptr;
  item.blobPtr = nullptr;
  item.queryFilter.clear();
}

void FirebaseData::setQuery(QueryFilter &query)
{
  queryFilter._orderBy = query._orderBy;
  queryFilter._limitToFirst = query._limitToFirst;
  queryFilter._limitToLast = query._limitToLast;
  queryFilter._startAt = query._startAt;
  queryFilter._endAt = query._endAt;
  queryFilter._equalTo = query._equalTo;
}

WiFiClient &FirebaseData::getWiFiClient()
{
  return *_net._client.get();
}

bool FirebaseData::pauseFirebase(bool pause)
{
  if (WiFi.status() != WL_CONNECTED)
    return false;

  if (_net.connected() && pause != _pause)
  {

    if (_net.getStreamPtr()->available() > 0)
      _net.getStreamPtr()->read();

    _net.getStreamPtr()->stop();

    if (!_net.connected())
    {
      _pause = pause;
      return true;
    }
    return false;
  }
  else
  {
    _pause = pause;
    return true;
  }
}

String FirebaseData::dataType()
{
  std::string res = getDataType(_dataType);
  return res.c_str();
}

String FirebaseData::eventType()
{
  std::string res = _eventType;
  return res.c_str();
}

String FirebaseData::ETag()
{
  std::string res = _etag;
  return res.c_str();
}

std::string FirebaseData::getDataType(uint8_t type)
{
  if (type == FirebaseESP32::FirebaseDataType::JSON)
    return ESP32_FIREBASE_STR_74;

  if (type == FirebaseESP32::FirebaseDataType::STRING)
    return ESP32_FIREBASE_STR_75;

  if (type == FirebaseESP32::FirebaseDataType::FLOAT)
    return ESP32_FIREBASE_STR_76;

  if (type == FirebaseESP32::FirebaseDataType::DOUBLE)
    return ESP32_FIREBASE_STR_108;

  if (type == FirebaseESP32::FirebaseDataType::BOOLEAN)
    return ESP32_FIREBASE_STR_105;

  if (type == FirebaseESP32::FirebaseDataType::INTEGER)
    return ESP32_FIREBASE_STR_77;

  if (type == FirebaseESP32::FirebaseDataType::BLOB)
    return ESP32_FIREBASE_STR_91;

  if (type == FirebaseESP32::FirebaseDataType::NULL_)
    return ESP32_FIREBASE_STR_78;

  return std::string();
}

std::string FirebaseData::getMethod(uint8_t method)
{
  if (method == FirebaseESP32::FirebaseMethod::GET)
    return ESP32_FIREBASE_STR_115;

  if (method == FirebaseESP32::FirebaseMethod::PUT || method == FirebaseESP32::FirebaseMethod::PATCH_SILENT)
    return ESP32_FIREBASE_STR_116;

  if (method == FirebaseESP32::FirebaseMethod::POST)
    return ESP32_FIREBASE_STR_117;

  if (method == FirebaseESP32::FirebaseMethod::PATCH || method == FirebaseESP32::FirebaseMethod::PATCH_SILENT)
    return ESP32_FIREBASE_STR_118;

  if (method == FirebaseESP32::FirebaseMethod::DELETE)
    return ESP32_FIREBASE_STR_119;

  return std::string();
}

String FirebaseData::streamPath()
{
  return _streamPath.c_str();
}

String FirebaseData::dataPath()
{
  return _path.c_str();
}

int FirebaseData::intData()
{
  if ((_data.length() > 0 && (_dataType == FirebaseESP32::FirebaseDataType::INTEGER || _dataType == FirebaseESP32::FirebaseDataType::FLOAT || _dataType == FirebaseESP32::FirebaseDataType::DOUBLE)))
  {
    if (_r_dataType == FirebaseESP32::FirebaseDataType::TIMESTAMP)
    {
      double d = atof(_data.c_str());
      int ts = d / 1000;
      return ts;
    }
    else
      return atoi(_data.c_str());
  }
  else
    return 0;
}

float FirebaseData::floatData()
{
  if (_data.length() > 0 && (_dataType == FirebaseESP32::FirebaseDataType::INTEGER || _dataType == FirebaseESP32::FirebaseDataType::FLOAT || _dataType == FirebaseESP32::FirebaseDataType::DOUBLE))
    return atof(_data.c_str());
  else
    return 0.0;
}

double FirebaseData::doubleData()
{
  if (_data.length() > 0 && (_dataType == FirebaseESP32::FirebaseDataType::INTEGER || _dataType == FirebaseESP32::FirebaseDataType::FLOAT || _dataType == FirebaseESP32::FirebaseDataType::DOUBLE))
    return atof(_data.c_str());
  else
    return 0.0;
}

bool FirebaseData::boolData()
{
  bool res = false;

  if (_data.length() > 0 && _dataType == FirebaseESP32::FirebaseDataType::BOOLEAN)
    res = _data == ESP32_FIREBASE_STR_107;

  return res;
}

String FirebaseData::stringData()
{
  if (_data.length() > 0 && _dataType == FirebaseESP32::FirebaseDataType::STRING)
    return _data.substr(1, _data.length() - 2).c_str();
  else
    return std::string().c_str();
}

String FirebaseData::jsonData()
{
  if (_data.length() > 0 && _dataType == FirebaseESP32::FirebaseDataType::JSON)
    return _data.c_str();
  else
    return std::string().c_str();
}

FirebaseJson &FirebaseData::jsonObject()
{

  if (_data.length() > 0 && _dataType == FirebaseESP32::FirebaseDataType::JSON)
  {
    _json.setJsonData(_data);
    return _json;
  }
  else
  {
    return _json;
  }
}

std::vector<uint8_t> FirebaseData::blobData()
{
  if (_blob.size() > 0 && _dataType == FirebaseESP32::FirebaseDataType::BLOB)
  {
    return _blob;
  }
  else
    return std::vector<uint8_t>();
}

String FirebaseData::pushName()
{
  if (_pushName.length() > 0)
    return _pushName.c_str();
  else
    return std::string().c_str();
}

bool FirebaseData::isStream()
{
  return _isStream;
}

bool FirebaseData::httpConnected()
{
  return _httpConnected;
}

bool FirebaseData::streamTimeout()
{
  return _isStreamTimeout;
}

bool FirebaseData::dataAvailable()
{
  return _dataAvailable;
}

bool FirebaseData::streamAvailable()
{
  return !_httpConnected && _dataAvailable && _streamDataChanged;
}

bool FirebaseData::mismatchDataType()
{
  return _mismatchDataType;
}

size_t FirebaseData::getBackupFileSize()
{
  return _backupzFileSize;
}

String FirebaseData::getBackupFilename()
{
  return _backupFilename.c_str();
}

String FirebaseData::fileTransferError()
{
  return _file_transfer_error.c_str();
}

String FirebaseData::payload()
{
  return _data.c_str();
}

String FirebaseData::errorReason()
{
  std::string buff = "";

  if (_httpCode == HTTP_CODE_OK)
  {
    if (_mismatchDataType)
      _httpCode = FIREBASE_ERROR_DATA_TYPE_MISMATCH;
    else if (_pathNotExist)
      _httpCode = FIREBASE_ERROR_PATH_NOT_EXIST;
  }

  Firebase.errorToString(_httpCode, buff);

  if (_firebaseError != "")
  {
    buff += ESP32_FIREBASE_STR_132;
    buff += ESP32_FIREBASE_STR_6;
    buff += _firebaseError;
  }

  return buff.c_str();
}

int FirebaseData::httpCode()
{
  return _httpCode;
}

StreamData::StreamData()
{
}
StreamData::~StreamData()
{
  empty();
}

String StreamData::dataPath()
{
  return _path.c_str();
}

String StreamData::streamPath()
{
  return _streamPath.c_str();
}

int StreamData::intData()
{
  if (_data.length() > 0 && (_dataType == FirebaseESP32::FirebaseDataType::INTEGER || _dataType == FirebaseESP32::FirebaseDataType::FLOAT || _dataType == FirebaseESP32::FirebaseDataType::DOUBLE))
    return atoi(_data.c_str());
  else
    return 0;
}

float StreamData::floatData()
{
  if (_data.length() > 0 && (_dataType == FirebaseESP32::FirebaseDataType::INTEGER || _dataType == FirebaseESP32::FirebaseDataType::FLOAT || _dataType == FirebaseESP32::FirebaseDataType::DOUBLE))
    return atof(_data.c_str());
  else
    return 0;
}

double StreamData::doubleData()
{

  if (_data.length() > 0 && (_dataType == FirebaseESP32::FirebaseDataType::INTEGER || _dataType == FirebaseESP32::FirebaseDataType::FLOAT || _dataType == FirebaseESP32::FirebaseDataType::DOUBLE))
    return atof(_data.c_str());
  else
    return 0.0;
}

bool StreamData::boolData()
{
  bool res = false;

  if (_data.length() > 0 && _dataType == FirebaseESP32::FirebaseDataType::BOOLEAN)
    res = _data == ESP32_FIREBASE_STR_107;

  return res;
}

String StreamData::stringData()
{
  if (_dataType == FirebaseESP32::FirebaseDataType::STRING)
    return _data.c_str();
  else
    return std::string().c_str();
}

String StreamData::jsonData()
{
  if (_dataType == FirebaseESP32::FirebaseDataType::JSON)
    return _data.c_str();
  else
    return std::string().c_str();
}

FirebaseJson &StreamData::jsonObject()
{
  if (_dataType == FirebaseESP32::FirebaseDataType::JSON)
  {
    _json->setJsonData(_data);
    return *_json;
  }
  else
  {
    return *_json;
  }
}

std::vector<uint8_t> StreamData::blobData()
{
  if (_blob.size() > 0 && _dataType == FirebaseESP32::FirebaseDataType::BLOB)
  {
    return _blob;
  }
  else
    return std::vector<uint8_t>();
}

String StreamData::dataType()
{
  return _dataTypeStr.c_str();
}

String StreamData::eventType()
{
  return _eventTypeStr.c_str();
}

void StreamData::empty()
{
  std::string().swap(_streamPath);
  std::string().swap(_path);
  std::string().swap(_data);
  std::string().swap(_dataTypeStr);
  std::string().swap(_eventTypeStr);
  std::vector<uint8_t>().swap(_blob);
}

QueryFilter::QueryFilter()
{
}

QueryFilter::~QueryFilter()
{
  clear();
}

QueryFilter &QueryFilter::clear()
{
  std::string().swap(_orderBy);
  std::string().swap(_limitToFirst);
  std::string().swap(_limitToLast);
  std::string().swap(_startAt);
  std::string().swap(_endAt);
  std::string().swap(_equalTo);
  return *this;
}

QueryFilter &QueryFilter::orderBy(const String &val)
{
  _orderBy = ESP32_FIREBASE_STR_3;
  _orderBy += val.c_str();
  _orderBy += ESP32_FIREBASE_STR_3;
  return *this;
}
QueryFilter &QueryFilter::limitToFirst(int val)
{
  char *num = new char[20];
  _limitToFirst = itoa(val, num, 10);
  delete[] num;
  return *this;
}

QueryFilter &QueryFilter::limitToLast(int val)
{
  char *num = new char[20];
  _limitToLast = itoa(val, num, 10);
  delete[] num;
  return *this;
}

QueryFilter &QueryFilter::startAt(float val)
{
  char *num = new char[20];
  memset(num, 0, 20);
  dtostrf(val, 7, 6, num);
  _startAt = num;
  delete[] num;
  return *this;
}

QueryFilter &QueryFilter::endAt(float val)
{
  char *num = new char[20];
  memset(num, 0, 20);
  dtostrf(val, 7, 6, num);
  _endAt = num;
  delete[] num;
  return *this;
}

QueryFilter &QueryFilter::startAt(const String &val)
{
  _startAt = ESP32_FIREBASE_STR_3;
  _startAt += val.c_str();
  _startAt += ESP32_FIREBASE_STR_3;
  return *this;
}

QueryFilter &QueryFilter::endAt(const String &val)
{
  _endAt = ESP32_FIREBASE_STR_3;
  _endAt += val.c_str();
  _endAt += ESP32_FIREBASE_STR_3;
  return *this;
}

QueryFilter &QueryFilter::equalTo(int val)
{
  char *num = new char[20];
  _equalTo = itoa(val, num, 10);
  delete[] num;
  return *this;
}

QueryFilter &QueryFilter::equalTo(const String &val)
{
  _equalTo = ESP32_FIREBASE_STR_3;
  _equalTo += val.c_str();
  _equalTo += ESP32_FIREBASE_STR_3;
  return *this;
}

QueueManager::QueueManager()
{
}
QueueManager::~QueueManager()
{
  clear();
}

void QueueManager::clear()
{
  for (uint8_t i = 0; i < _queueCollection.size(); i++)
  {
    QueueItem item = _queueCollection[i];

    std::string().swap(item.path);
    std::string().swap(item.filename);
    std::string().swap(item.payload);

    item.stringPtr = nullptr;
    item.intPtr = nullptr;
    item.floatPtr = nullptr;
    item.doublePtr = nullptr;
    item.boolPtr = nullptr;
    item.blobPtr = nullptr;
    item.queryFilter.clear();
  }
}

bool QueueManager::add(QueueItem q)
{

  if (_queueCollection.size() < _maxQueue)
  {
    _queueCollection.push_back(q);
    return true;
  }
  return false;
}

void QueueManager::remove(uint8_t index)
{
  _queueCollection.erase(_queueCollection.begin() + index);
}

QueueInfo::QueueInfo()
{
}

QueueInfo::~QueueInfo()
{
  clear();
}

uint8_t QueueInfo::totalQueues()
{
  return _totalQueue;
}

uint32_t QueueInfo::currentQueueID()
{
  return _currentQueueID;
}

bool QueueInfo::isQueueFull()
{
  return _isQueueFull;
}

String QueueInfo::dataType()
{
  return _dataType.c_str();
}

String QueueInfo::firebaseMethod()
{
  return _method.c_str();
}

String QueueInfo::dataPath()
{
  return _path.c_str();
}

void QueueInfo::clear()
{
  std::string().swap(_dataType);
  std::string().swap(_method);
  std::string().swap(_path);
}

FCMObject::FCMObject() {}
FCMObject::~FCMObject()
{
  clear();
}

void FCMObject::begin(const String &serverKey)
{
  _server_key = serverKey.c_str();
}

void FCMObject::addDeviceToken(const String &deviceToken)
{
  _deviceToken.push_back(deviceToken.c_str());
}
void FCMObject::removeDeviceToken(uint16_t index)
{
  if (_deviceToken.size() > 0)
  {
    std::string().swap(_deviceToken[index]);
    _deviceToken.erase(_deviceToken.begin() + index);
  }
}
void FCMObject::clearDeviceToken()
{
  for (size_t i = 0; i < _deviceToken.size(); i++)
  {
    std::string().swap(_deviceToken[i]);
    _deviceToken.erase(_deviceToken.begin() + i);
  }
}

void FCMObject::setNotifyMessage(const String &title, const String &body)
{
  _notify_title = title.c_str();
  _notify_body = body.c_str();
  _notify_icon = "";
  _notify_click_action = "";
}

void FCMObject::setNotifyMessage(const String &title, const String &body, const String &icon)
{
  _notify_title = title.c_str();
  _notify_body = body.c_str();
  _notify_icon = icon.c_str();
  _notify_click_action = "";
}

void FCMObject::setNotifyMessage(const String &title, const String &body, const String &icon, const String &click_action)
{
  _notify_title = title.c_str();
  _notify_body = body.c_str();
  _notify_icon = icon.c_str();
  _notify_click_action = click_action.c_str();
}

void FCMObject::clearNotifyMessage()
{
  _notify_title = "";
  _notify_body = "";
  _notify_icon = "";
  _notify_click_action = "";
}

void FCMObject::setDataMessage(const String &jsonString)
{
  _data_msg = jsonString.c_str();
}

void FCMObject::setDataMessage(FirebaseJson &json)
{
  _data_msg = json.toStdString();
}

void FCMObject::clearDataMessage()
{
  _data_msg = "";
}

void FCMObject::setPriority(const String &priority)
{
  _priority = priority.c_str();
}

void FCMObject::setCollapseKey(const String &key)
{
  _collapse_key = key.c_str();
}

void FCMObject::setTimeToLive(uint32_t seconds)
{
  if (seconds <= 2419200)
    _ttl = seconds;
  else
    _ttl = -1;
}

void FCMObject::setTopic(const String &topic)
{
  _topic = ESP32_FIREBASE_STR_134;
  _topic += topic.c_str();
}

String FCMObject::getSendResult()
{
  return _sendResult.c_str();
}

bool FCMObject::fcm_connect(FirebaseESP32HTTPClient &net)
{
  int httpConnected = net.begin(ESP32_FIREBASE_STR_120, _port, ESP32_FIREBASE_STR_121);

  if (!httpConnected)
  {
    return false;
  }
  return true;
}
void FCMObject::fcm_buildHeader(std::string &header, size_t payloadSize)
{
  char *len = new char[20];
  memset(len, 0, 20);

  header = ESP32_FIREBASE_STR_24;
  header += ESP32_FIREBASE_STR_6;
  header += ESP32_FIREBASE_STR_121;
  header += ESP32_FIREBASE_STR_30;

  header += ESP32_FIREBASE_STR_31;
  header += ESP32_FIREBASE_STR_120;
  header += ESP32_FIREBASE_STR_21;

  header += ESP32_FIREBASE_STR_131;
  header += _server_key;
  header += ESP32_FIREBASE_STR_21;

  header += ESP32_FIREBASE_STR_32;

  header += ESP32_FIREBASE_STR_8;
  header += ESP32_FIREBASE_STR_129;
  header += ESP32_FIREBASE_STR_21;

  header += ESP32_FIREBASE_STR_12;
  itoa(payloadSize, len, 10);
  header += len;
  header += ESP32_FIREBASE_STR_21;
  header += ESP32_FIREBASE_STR_34;
  header += ESP32_FIREBASE_STR_21;
  delete[] len;
}

void FCMObject::fcm_buildPayload(std::string &msg, uint8_t messageType)
{

  bool noti = _notify_title.length() > 0 || _notify_body.length() > 0 || _notify_icon.length() > 0 || _notify_click_action.length() > 0;
  size_t c = 0;

  msg += ESP32_FIREBASE_STR_163;

  if (noti)
    msg += ESP32_FIREBASE_STR_122;

  if (noti && _notify_title.length() > 0)
  {
    msg += ESP32_FIREBASE_STR_123;
    msg += _notify_title;
    msg += ESP32_FIREBASE_STR_3;
    c++;
  }

  if (noti && _notify_body.length() > 0)
  {
    if (c > 0)
      msg += ESP32_FIREBASE_STR_132;
    msg += ESP32_FIREBASE_STR_124;
    msg += _notify_body;
    msg += ESP32_FIREBASE_STR_3;
    c++;
  }

  if (noti && _notify_icon.length() > 0)
  {
    if (c > 0)
      msg += ESP32_FIREBASE_STR_132;
    msg += ESP32_FIREBASE_STR_125;
    msg += _notify_icon;
    msg += ESP32_FIREBASE_STR_3;
    c++;
  }

  if (noti && _notify_click_action.length() > 0)
  {
    if (c > 0)
      msg += ESP32_FIREBASE_STR_132;
    msg += ESP32_FIREBASE_STR_126;
    msg += _notify_click_action;
    msg += ESP32_FIREBASE_STR_3;
    c++;
  }

  if (noti)
    msg += ESP32_FIREBASE_STR_127;

  c = 0;

  if (messageType == FirebaseESP32::FCMMessageType::SINGLE)
  {
    if (noti)
      msg += ESP32_FIREBASE_STR_132;

    msg += ESP32_FIREBASE_STR_128;
    msg += _deviceToken[_index];
    msg += ESP32_FIREBASE_STR_3;

    c++;
  }
  else if (messageType == FirebaseESP32::FCMMessageType::MULTICAST)
  {
    if (noti)
      msg += ESP32_FIREBASE_STR_132;

    msg += ESP32_FIREBASE_STR_130;

    for (uint16_t i = 0; i < _deviceToken.size(); i++)
    {
      if (i > 0)
        msg += ESP32_FIREBASE_STR_132;

      msg += ESP32_FIREBASE_STR_3;
      msg += _deviceToken[i];
      msg += ESP32_FIREBASE_STR_3;
    }

    msg += ESP32_FIREBASE_STR_133;

    c++;
  }
  else if (messageType == FirebaseESP32::FCMMessageType::TOPIC)
  {
    if (noti)
      msg += ESP32_FIREBASE_STR_132;

    msg += ESP32_FIREBASE_STR_128;
    msg += _topic.c_str();
    msg += ESP32_FIREBASE_STR_3;

    c++;
  }

  if (_data_msg.length() > 0)
  {
    if (c > 0)
      msg += ESP32_FIREBASE_STR_132;

    msg += ESP32_FIREBASE_STR_135;
    msg += _data_msg;
    c++;
  }

  if (_priority.length() > 0)
  {
    if (c > 0)
      msg += ESP32_FIREBASE_STR_132;

    msg += ESP32_FIREBASE_STR_136;
    msg += _priority;
    msg += ESP32_FIREBASE_STR_3;
    c++;
  }

  if (_ttl > -1)
  {
    if (c > 0)
      msg += ESP32_FIREBASE_STR_132;
    char *ttl = new char[20];
    memset(ttl, 0, 20);
    msg += ESP32_FIREBASE_STR_137;
    itoa(_ttl, ttl, 10);
    msg += ttl;
    delete[] ttl;
    c++;
  }

  if (_collapse_key.length() > 0)
  {
    if (c > 0)
      msg += ESP32_FIREBASE_STR_132;
    msg += ESP32_FIREBASE_STR_138;
    msg += _collapse_key;
    msg += ESP32_FIREBASE_STR_3;
    c++;
  }

  msg += ESP32_FIREBASE_STR_127;
}

bool FCMObject::getFCMServerResponse(FirebaseESP32HTTPClient &net, int &httpcode)
{
  if (WiFi.status() != WL_CONNECTED)
  {
    httpcode = HTTPC_ERROR_CONNECTION_LOST;
    return false;
  }

  if (!net.getStreamPtr())
  {
    httpcode = HTTPC_ERROR_CONNECTION_LOST;
    return false;
  }

  bool flag = false;
  std::string linebuff = "";

  _sendResult = "";

  char c;
  int p1, p2;
  httpcode = -1000;

  size_t lfCount = 0;
  size_t charPos = 0;

  bool payloadBegin = false;

  unsigned long dataTime = millis();

  while (net.getStreamPtr()->connected() && !net.getStreamPtr()->available() && millis() - dataTime < 5000)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      httpcode = HTTPC_ERROR_CONNECTION_LOST;
      return false;
    }
    delay(1);
  }

  dataTime = millis();
  if (net.getStreamPtr()->connected() && net.getStreamPtr()->available())
  {

    while (net.getStreamPtr()->available())
    {

      if (WiFi.status() != WL_CONNECTED)
      {
        httpcode = HTTPC_ERROR_CONNECTION_LOST;
        return false;
      }

      c = net.getStreamPtr()->read();

      if (c != '\r' && c != '\n')
        linebuff.append(1, c);

      charPos++;

      if (c == '\n')
      {

        dataTime = millis();

        p1 = linebuff.find(ESP32_FIREBASE_STR_5);
        if (p1 != std::string::npos)
        {
          p2 = linebuff.find(ESP32_FIREBASE_STR_6, p1 + strlen(ESP32_FIREBASE_STR_5));
          if (p2 != std::string::npos)
            httpcode = atoi(linebuff.substr(p1 + strlen(ESP32_FIREBASE_STR_5), p2 - p1 - strlen(ESP32_FIREBASE_STR_5)).c_str());
        }

        if (httpcode == HTTP_CODE_OK && lfCount > 0 && linebuff.length() == 0)
          payloadBegin = true;

        if (!payloadBegin)
          linebuff.clear();

        lfCount++;
        charPos = 0;
      }

      if (millis() - dataTime > 5000)
      {
        httpcode = HTTPC_ERROR_READ_TIMEOUT;
        break;
      }
    }

    _sendResult = linebuff;

    if (!httpcode)
      httpcode = HTTPC_ERROR_NO_HTTP_SERVER;

    goto EXIT_5;
  }

  std::string().swap(linebuff);

  if (httpcode == -1000)
  {
    httpcode = 0;
    flag = true;
  }

  return flag;

EXIT_5:

  std::string().swap(linebuff);

  if (httpcode == HTTPC_ERROR_READ_TIMEOUT)
    return false;

  return httpcode == HTTP_CODE_OK;
}

bool FCMObject::fcm_send(FirebaseESP32HTTPClient &net, int &httpcode, uint8_t messageType)
{

  std::string msg = "";
  std::string header = "";

  fcm_buildPayload(msg, messageType);

  fcm_buildHeader(header, msg.length());

  if (WiFi.status() != WL_CONNECTED)
  {
    httpcode = HTTPC_ERROR_CONNECTION_LOST;
    return false;
  }

  uint8_t retryCount = 0;
  httpcode = net.sendRequest(header.c_str(), "");
  while (httpcode != 0)
  {
    retryCount++;
    if (retryCount > 5)
      break;

    httpcode = net.sendRequest(header.c_str(), "");
  }

  if (httpcode != 0)
    return false;

  httpcode = net.sendRequest("", msg.c_str());

  std::string().swap(msg);
  std::string().swap(header);

  if (httpcode != 0)
    return false;

  bool res = getFCMServerResponse(net, httpcode);

  return res;
}

void FCMObject::clear()
{
  std::string().swap(_notify_title);
  std::string().swap(_notify_body);
  std::string().swap(_notify_icon);
  std::string().swap(_notify_click_action);
  std::string().swap(_data_msg);
  std::string().swap(_priority);
  std::string().swap(_collapse_key);
  std::string().swap(_topic);
  std::string().swap(_server_key);
  std::string().swap(_sendResult);
  _ttl = -1;
  _index = 0;
  clearDeviceToken();
  std::vector<std::string>().swap(_deviceToken);
}

FirebaseESP32 Firebase = FirebaseESP32();

#endif
