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

#ifndef FirebaseESP32_CPP
#define FirebaseESP32_CPP

#ifdef ESP32

#include "FirebaseESP32.h"

extern "C"
{
#include <esp_err.h>
#include <esp_wifi.h>
}

FirebaseESP32::FirebaseESP32()
{
}

FirebaseESP32::~FirebaseESP32()
{
    if (ut)
        delete ut;
}

void FirebaseESP32::begin(FirebaseConfig *config, FirebaseAuth *auth)
{
    _auth = auth;
    _cfg = config;

    if (_cfg == nullptr)
        _cfg = &_cfg_;

    if (_auth == nullptr)
        _auth = &_auth_;

    ut = new UtilsClass(_cfg);

    RTDB.begin(ut);

    _cfg->_int.fb_reconnect_wifi = WiFi.getAutoReconnect();

    _cfg->signer.signup = false;
    _cfg_.signer.signup = false;
    Signer.begin(ut, _cfg, _auth);
    std::string().swap(_cfg_.signer.tokens.error.message);

    if (_cfg->service_account.json.path.length() > 0)
    {
        if (!Signer.parseSAFile())
            _cfg->signer.tokens.status = token_status_uninitialized;
    }

    if (_cfg->signer.tokens.legacy_token.length() > 0)
        Signer.setTokenType(token_type_legacy_token);
    else if (Signer.tokenSigninDataReady())
    {
        if (_auth->token.uid.length() == 0)
            Signer.setTokenType(token_type_oauth2_access_token);
        else
            Signer.setTokenType(token_type_custom_token);
    }
    else if (Signer.userSigninDataReady())
        Signer.setTokenType(token_type_id_token);

    struct fb_esp_url_info_t uinfo;
    _cfg->_int.fb_auth_uri = _cfg->signer.tokens.token_type == token_type_legacy_token || _cfg->signer.tokens.token_type == token_type_id_token;

    if (_cfg->host.length() > 0)
    {
        ut->getUrlInfo(_cfg->host.c_str(), uinfo);
        _cfg->host = uinfo.host;
    }

    if (strlen_P(_cfg->cert.data))
        _cfg->_int.fb_caCert = _cfg->cert.data;

    if (_cfg->cert.file.length() > 0)
    {
        if (_cfg->cert.file_storage == mem_storage_type_sd && !_cfg->_int.fb_sd_rdy)
            _cfg->_int.fb_sd_rdy = ut->sdTest(_cfg->_int.fb_file);
        else if (_cfg->cert.file_storage == mem_storage_type_flash && !_cfg->_int.fb_flash_rdy)
            ut->flashTest();
    }

    Signer.handleToken();
}

void FirebaseESP32::begin(const String &host, const String &auth)
{
    _cfg_.host = host.c_str();
    _cfg_.signer.tokens.legacy_token = auth.c_str();
    begin(&_cfg_, &_auth_);
}

void FirebaseESP32::begin(const String &host, const String &auth, const char *caCert)
{
    _cfg_.host = host.c_str();
    _cfg_.signer.tokens.legacy_token = auth.c_str();
    _cfg_.cert.data = caCert;
    begin(&_cfg_, &_auth_);
}

void FirebaseESP32::begin(const String &host, const String &auth, const String &caCertFile, uint8_t storageType)
{
    _cfg_.host = host.c_str();
    _cfg_.signer.tokens.legacy_token = auth.c_str();
    _cfg_.cert.file = caCertFile.c_str();
    _cfg_.cert.file_storage = storageType;
    begin(&_cfg_, &_auth_);
}

bool FirebaseESP32::signUp(FirebaseConfig *config, FirebaseAuth *auth, const char *email, const char *password)
{
    _auth = auth;
    _cfg = config;

    if (_auth == nullptr)
        _auth = &_auth_;
    if (_cfg == nullptr)
        _cfg = &_cfg_;

    return Signer.getIdToken(true, email, password);
}

bool FirebaseESP32::sendEmailVerification(FirebaseConfig *config, const char *idToken)
{
    _cfg = config;
    if (_cfg == nullptr)
        _cfg = &_cfg_;
    return Signer.handleEmailSending(idToken, fb_esp_user_email_sending_type_verify);
}

bool FirebaseESP32::sendResetPassword(FirebaseConfig *config, const char *email)
{
    _cfg = config;
    if (_cfg == nullptr)
        _cfg = &_cfg_;
    return Signer.handleEmailSending(email, fb_esp_user_email_sending_type_reset_psw);
}

void FirebaseESP32::end(FirebaseData &fbdo)
{
    endStream(fbdo);
    removeStreamCallback(fbdo);
    fbdo.clear();
}

void FirebaseESP32::setStreamTaskStackSize(size_t size)
{
    Serial.println(F("[INFO]: Function setStreamTaskStackSize is obsoleted, the stack size can set via setStreamCallback"));
}

void FirebaseESP32::allowMultipleRequests(bool enable)
{
    Signer.getCfg()->_int.fb_multiple_requests = enable;
}

struct token_info_t FirebaseESP32::authTokenInfo()
{
    return Signer.tokenInfo;
}

void FirebaseESP32::reconnectWiFi(bool reconnect)
{
    WiFi.setAutoReconnect(reconnect);
}

void FirebaseESP32::setFloatDigits(uint8_t digits)
{
    if (digits < 7)
        _cfg->_int.fb_float_digits = digits;
}

void FirebaseESP32::setDoubleDigits(uint8_t digits)
{
    if (digits < 9)
        _cfg->_int.fb_double_digits = digits;
}

void FirebaseESP32::setReadTimeout(FirebaseData &fbdo, int millisec)
{
    RTDB.setReadTimeout(&fbdo, millisec);
}

void FirebaseESP32::setwriteSizeLimit(FirebaseData &fbdo, const String &size)
{
    fbdo._ss.rtdb.write_limit = size.c_str();
}

bool FirebaseESP32::getRules(FirebaseData &fbdo)
{
    return RTDB.getRules(&fbdo);
}

bool FirebaseESP32::setRules(FirebaseData &fbdo, const String &rules)
{
    return RTDB.setRules(&fbdo, rules.c_str());
}

bool FirebaseESP32::pathExist(FirebaseData &fbdo, const String &path)
{
    fbdo.queryFilter.clear();
    struct fb_esp_rtdb_request_info_t req;
    req.path = path.c_str();
    req.method = m_get_nocontent;
    req.dataType = d_string;
    if (RTDB.handleRequest(&fbdo, &req))
        return !fbdo._ss.rtdb.path_not_found;
    else
        return false;
}

String FirebaseESP32::getETag(FirebaseData &fbdo, const String &path)
{
    return RTDB.getETag(&fbdo, path.c_str());
}

bool FirebaseESP32::getShallowData(FirebaseData &fbdo, const String &path)
{
    return RTDB.getShallowData(&fbdo, path.c_str());
}

void FirebaseESP32::enableClassicRequest(FirebaseData &fbdo, bool flag)
{
    fbdo._ss.classic_request = flag;
}

bool FirebaseESP32::setPriority(FirebaseData &fbdo, const String &path, float priority)
{
    return RTDB.setPriority(&fbdo, path.c_str(), priority);
}

bool FirebaseESP32::getPriority(FirebaseData &fbdo, const String &path)
{
    return RTDB.getPriority(&fbdo, path.c_str());
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, int intValue)
{
    return RTDB.pushInt(&fbdo, path.c_str(), intValue);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, int intValue, float priority)
{
    return RTDB.pushInt(&fbdo, path.c_str(), intValue, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, float floatValue)
{
    return RTDB.pushFloat(&fbdo, path.c_str(), floatValue);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, float floatValue, float priority)
{
    return RTDB.pushFloat(&fbdo, path.c_str(), floatValue, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, double doubleValue)
{
    return RTDB.pushDouble(&fbdo, path.c_str(), doubleValue);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, double doubleValue, float priority)
{
    return RTDB.pushDouble(&fbdo, path.c_str(), doubleValue, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, bool boolValue)
{
    return RTDB.pushBool(&fbdo, path.c_str(), boolValue);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, bool boolValue, float priority)
{
    return RTDB.pushBool(&fbdo, path.c_str(), boolValue, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, const char *stringValue)
{
    return RTDB.pushString(&fbdo, path.c_str(), stringValue);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, const String &stringValue)
{
    return RTDB.pushString(&fbdo, path.c_str(), stringValue);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, const char *stringValue, float priority)
{
    return RTDB.pushString(&fbdo, path.c_str(), stringValue, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, const String &stringValue, float priority)
{
    return RTDB.pushString(&fbdo, path.c_str(), stringValue, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, FirebaseJson &json)
{
    return RTDB.pushJSON(&fbdo, path.c_str(), &json);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
    return RTDB.pushJSON(&fbdo, path.c_str(), &json, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr)
{
    return RTDB.pushArray(&fbdo, path.c_str(), &arr);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority)
{
    return RTDB.pushArray(&fbdo, path.c_str(), &arr, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size)
{
    return RTDB.pushBlob(&fbdo, path.c_str(), blob, size);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority)
{
    return RTDB.pushBlob(&fbdo, path.c_str(), blob, size, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName)
{
    return RTDB.pushFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str());
}

bool FirebaseESP32::push(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority)
{
    return RTDB.pushFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), priority);
}

template <typename T>
bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, T value)
{
    if (std::is_same<T, int>::value)
        return pushInt(fbdo, path, value);
    else if (std::is_same<T, double>::value)
        return pushDouble(fbdo, path, value);
    else if (std::is_same<T, bool>::value)
        return pushBool(fbdo, path, value);
    else if (std::is_same<T, const char *>::value)
        return pushString(fbdo, path, value);
    else if (std::is_same<T, const String &>::value)
        return pushString(fbdo, path, value);
    else if (std::is_same<T, FirebaseJson &>::value)
        return pushJson(fbdo, path, value);
    else if (std::is_same<T, FirebaseJsonArray &>::value)
        return pushArray(fbdo, path, value);
}

template <typename T>
bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, T value, size_t size)
{
    if (std::is_same<T, uint8_t *>::value)
        return RTDB.pushBlob(&fbdo, path.c_str(), value, size);
}

template <typename T>
bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, T value, float priority)
{
    if (std::is_same<T, int>::value)
        return pushInt(fbdo, path, value, priority);
    else if (std::is_same<T, double>::value)
        return pushDouble(fbdo, path, value, priority);
    else if (std::is_same<T, bool>::value)
        return pushBool(fbdo, path, value, priority);
    else if (std::is_same<T, const char *>::value)
        return pushString(fbdo, path, value, priority);
    else if (std::is_same<T, const String &>::value)
        return pushString(fbdo, path, value, priority);
    else if (std::is_same<T, FirebaseJson &>::value)
        return pushJson(fbdo, path, value, priority);
    else if (std::is_same<T, FirebaseJsonArray &>::value)
        return pushArray(fbdo, path, value, priority);
}

template <typename T>
bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, T value, size_t size, float priority)
{
    if (std::is_same<T, uint8_t *>::value)
        return RTDB.pushBlob(&fbdo, path.c_str(), value, size, priority);
}

bool FirebaseESP32::pushInt(FirebaseData &fbdo, const String &path, int intValue)
{
    return RTDB.pushInt(&fbdo, path.c_str(), intValue);
}

bool FirebaseESP32::pushInt(FirebaseData &fbdo, const String &path, int intValue, float priority)
{
    return RTDB.pushInt(&fbdo, path.c_str(), intValue, priority);
}

bool FirebaseESP32::pushFloat(FirebaseData &fbdo, const String &path, float floatValue)
{
    return RTDB.pushFloat(&fbdo, path.c_str(), floatValue);
}

bool FirebaseESP32::pushFloat(FirebaseData &fbdo, const String &path, float floatValue, float priority)
{
    return RTDB.pushFloat(&fbdo, path.c_str(), floatValue, priority);
}

bool FirebaseESP32::pushDouble(FirebaseData &fbdo, const String &path, double doubleValue)
{
    return RTDB.pushDouble(&fbdo, path.c_str(), doubleValue);
}

bool FirebaseESP32::pushDouble(FirebaseData &fbdo, const String &path, double doubleValue, float priority)
{
    return RTDB.pushDouble(&fbdo, path.c_str(), doubleValue, priority);
}

bool FirebaseESP32::pushBool(FirebaseData &fbdo, const String &path, bool boolValue)
{
    return RTDB.pushBool(&fbdo, path.c_str(), boolValue);
}

bool FirebaseESP32::pushBool(FirebaseData &fbdo, const String &path, bool boolValue, float priority)
{
    return RTDB.pushBool(&fbdo, path.c_str(), boolValue, priority);
}

bool FirebaseESP32::pushString(FirebaseData &fbdo, const String &path, const String &stringValue)
{
    return RTDB.pushString(&fbdo, path.c_str(), stringValue);
}

bool FirebaseESP32::pushString(FirebaseData &fbdo, const String &path, const String &stringValue, float priority)
{
    return RTDB.pushString(&fbdo, path.c_str(), stringValue, priority);
}

bool FirebaseESP32::pushJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json)
{
    return RTDB.pushJSON(&fbdo, path.c_str(), &json);
}

bool FirebaseESP32::pushJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
    return RTDB.pushJSON(&fbdo, path.c_str(), &json, priority);
}

bool FirebaseESP32::pushArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr)
{
    return RTDB.pushArray(&fbdo, path.c_str(), &arr);
}

bool FirebaseESP32::pushArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority)
{
    return RTDB.pushArray(&fbdo, path.c_str(), &arr, priority);
}

bool FirebaseESP32::pushBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size)
{
    return RTDB.pushBlob(&fbdo, path.c_str(), blob, size);
}

bool FirebaseESP32::pushBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority)
{
    return RTDB.pushBlob(&fbdo, path.c_str(), blob, size, priority);
}

bool FirebaseESP32::pushFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName)
{
    return RTDB.pushFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str());
}

bool FirebaseESP32::pushFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority)
{
    return RTDB.pushFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), priority);
}

bool FirebaseESP32::pushTimestamp(FirebaseData &fbdo, const String &path)
{
    return RTDB.pushTimestamp(&fbdo, path.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, int intValue)
{
    return RTDB.setInt(&fbdo, path.c_str(), intValue);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, int intValue, float priority)
{
    return RTDB.setInt(&fbdo, path.c_str(), intValue, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, int intValue, const String &ETag)
{
    return RTDB.setInt(&fbdo, path.c_str(), intValue, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, int intValue, float priority, const String &ETag)
{
    return RTDB.setInt(&fbdo, path.c_str(), intValue, priority, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, float floatValue)
{
    return RTDB.setFloat(&fbdo, path.c_str(), floatValue);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, float floatValue, float priority)
{
    return RTDB.setFloat(&fbdo, path.c_str(), floatValue, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, float floatValue, const String &ETag)
{
    return RTDB.setFloat(&fbdo, path.c_str(), floatValue, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, float floatValue, float priority, const String &ETag)
{
    return RTDB.setFloat(&fbdo, path.c_str(), floatValue, priority, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, double doubleValue)
{
    return RTDB.setDouble(&fbdo, path.c_str(), doubleValue);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, double doubleValue, float priority)
{
    return RTDB.setDouble(&fbdo, path.c_str(), doubleValue, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, double doubleValue, const String &ETag)
{
    return RTDB.setDouble(&fbdo, path.c_str(), doubleValue, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, double doubleValue, float priority, const String &ETag)
{
    return RTDB.setDouble(&fbdo, path.c_str(), doubleValue, priority, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, bool boolValue)
{
    return RTDB.setBool(&fbdo, path.c_str(), boolValue);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, bool boolValue, float priority)
{
    return RTDB.setBool(&fbdo, path.c_str(), boolValue, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, bool boolValue, const String &ETag)
{
    return RTDB.setBool(&fbdo, path.c_str(), boolValue, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, bool boolValue, float priority, const String &ETag)
{
    return RTDB.setBool(&fbdo, path.c_str(), boolValue, priority, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const char *stringValue)
{
    return RTDB.setString(&fbdo, path.c_str(), stringValue);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const String &stringValue)
{
    return RTDB.setString(&fbdo, path.c_str(), stringValue);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const char *stringValue, float priority)
{
    return RTDB.setString(&fbdo, path.c_str(), stringValue, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const String &stringValue, float priority)
{
    return RTDB.setString(&fbdo, path.c_str(), stringValue, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const char *stringValue, const String &ETag)
{
    return RTDB.setString(&fbdo, path.c_str(), stringValue, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const String &stringValue, const String &ETag)
{
    return RTDB.setString(&fbdo, path.c_str(), stringValue, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const char *stringValue, float priority, const String &ETag)
{
    return RTDB.setString(&fbdo, path.c_str(), stringValue, priority, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const String &stringValue, float priority, const String &ETag)
{
    return RTDB.setString(&fbdo, path.c_str(), stringValue, priority, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJson &json)
{
    return RTDB.setJSON(&fbdo, path.c_str(), &json);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr)
{
    return RTDB.setArray(&fbdo, path.c_str(), &arr);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
    return RTDB.setJSON(&fbdo, path.c_str(), &json, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority)
{
    return RTDB.setArray(&fbdo, path.c_str(), &arr, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJson &json, const String &ETag)
{
    return RTDB.setJSON(&fbdo, path.c_str(), &json, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, const String &ETag)
{
    return RTDB.setArray(&fbdo, path.c_str(), &arr, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority, const String &ETag)
{
    return RTDB.setJSON(&fbdo, path.c_str(), &json, priority, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority, const String &ETag)
{
    return RTDB.setArray(&fbdo, path.c_str(), &arr, priority, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size)
{
    return RTDB.setBlob(&fbdo, path.c_str(), blob, size);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority)
{
    return RTDB.setBlob(&fbdo, path.c_str(), blob, size, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, const String &ETag)
{
    return RTDB.setBlob(&fbdo, path.c_str(), blob, size, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority, const String &ETag)
{
    return RTDB.setBlob(&fbdo, path.c_str(), blob, size, priority, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName)
{
    return RTDB.setFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority)
{
    return RTDB.setFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, const String &ETag)
{
    return RTDB.setFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority, const String &ETag)
{
    return RTDB.setFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), priority, ETag.c_str());
}

template <typename T>
bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, T value)
{
    if (std::is_same<T, int>::value)
        return setInt(fbdo, path, value);
    else if (std::is_same<T, double>::value)
        return setDouble(fbdo, path, value);
    else if (std::is_same<T, bool>::value)
        return setBool(fbdo, path, value);
    else if (std::is_same<T, const char *>::value)
        return setString(fbdo, path, value);
    else if (std::is_same<T, const String &>::value)
        return setString(fbdo, path, value);
    else if (std::is_same<T, FirebaseJson &>::value)
        return setJson(fbdo, path, value);
    else if (std::is_same<T, FirebaseJson *>::value)
        return setJson(fbdo, path, &value);
    else if (std::is_same<T, FirebaseJsonArray &>::value)
        return setArray(fbdo, path, value);
}

template <typename T>
bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, T value, size_t size)
{
    if (std::is_same<T, uint8_t *>::value)
        return RTDB.setBlob(&fbdo, path.c_str(), value, size);
}

template <typename T>
bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, T value, float priority)
{
    if (std::is_same<T, int>::value)
        return setInt(fbdo, path, value, priority);
    else if (std::is_same<T, double>::value)
        return setDouble(fbdo, path, value, priority);
    else if (std::is_same<T, bool>::value)
        return setBool(fbdo, path, value, priority);
    else if (std::is_same<T, const char *>::value)
        return setString(fbdo, path, value, priority);
    else if (std::is_same<T, const String &>::value)
        return setString(fbdo, path, value, priority);
    else if (std::is_same<T, FirebaseJson &>::value)
        return setJson(fbdo, path, value, priority);
    else if (std::is_same<T, FirebaseJsonArray &>::value)
        return setArray(fbdo, path, value, priority);
}

template <typename T>
bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, T value, size_t size, float priority)
{
    if (std::is_same<T, uint8_t *>::value)
        return RTDB.setBlob(&fbdo, path.c_str(), value, size, priority);
}

template <typename T>
bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, T value, const String &ETag)
{
    if (std::is_same<T, int>::value)
        return setInt(fbdo, path, value, ETag);
    else if (std::is_same<T, double>::value)
        return setDouble(fbdo, path, value, ETag);
    else if (std::is_same<T, bool>::value)
        return setBool(fbdo, path, value, ETag);
    else if (std::is_same<T, const char *>::value)
        return setString(fbdo, path, value, ETag);
    else if (std::is_same<T, const String &>::value)
        return setString(fbdo, path, value, ETag);
    else if (std::is_same<T, FirebaseJson &>::value)
        return setJson(fbdo, path, value, ETag);
    else if (std::is_same<T, FirebaseJsonArray &>::value)
        return setArray(fbdo, path, value, ETag);
}

template <typename T>
bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, T value, size_t size, const String &ETag)
{
    if (std::is_same<T, uint8_t *>::value)
        return RTDB.setBlob(&fbdo, path.c_str(), value, size, ETag.c_str());
}

template <typename T>
bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, T value, float priority, const String &ETag)
{
    if (std::is_same<T, int>::value)
        return setInt(fbdo, path, value, priority, ETag);
    else if (std::is_same<T, double>::value)
        return setDouble(fbdo, path, value, priority, ETag);
    else if (std::is_same<T, bool>::value)
        return setBool(fbdo, path, value, priority, ETag);
    else if (std::is_same<T, const char *>::value)
        return setString(fbdo, path, value, priority, ETag);
    else if (std::is_same<T, const String &>::value)
        return setString(fbdo, path, value, priority, ETag);
    else if (std::is_same<T, FirebaseJson &>::value)
        return setJson(fbdo, path, value, priority, ETag);
    else if (std::is_same<T, FirebaseJsonArray &>::value)
        return setArray(fbdo, path, value, priority, ETag);
}

template <typename T>
bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, T value, size_t size, float priority, const String &ETag)
{
    if (std::is_same<T, uint8_t *>::value)
        return RTDB.setBlob(&fbdo, path.c_str(), value, size, priority, ETag.c_str());
}

bool FirebaseESP32::setInt(FirebaseData &fbdo, const String &path, int intValue)
{
    return RTDB.setInt(&fbdo, path.c_str(), intValue);
}

bool FirebaseESP32::setInt(FirebaseData &fbdo, const String &path, int intValue, float priority)
{
    return RTDB.setInt(&fbdo, path.c_str(), intValue, priority);
}

bool FirebaseESP32::setInt(FirebaseData &fbdo, const String &path, int intValue, const String &ETag)
{
    return RTDB.setInt(&fbdo, path.c_str(), intValue, ETag.c_str());
}

bool FirebaseESP32::setInt(FirebaseData &fbdo, const String &path, int intValue, float priority, const String &ETag)
{
    return RTDB.setInt(&fbdo, path.c_str(), intValue, priority, ETag.c_str());
}

bool FirebaseESP32::setFloat(FirebaseData &fbdo, const String &path, float floatValue)
{
    return RTDB.setFloat(&fbdo, path.c_str(), floatValue);
}

bool FirebaseESP32::setFloat(FirebaseData &fbdo, const String &path, float floatValue, float priority)
{
    return RTDB.setFloat(&fbdo, path.c_str(), floatValue, priority);
}

bool FirebaseESP32::setFloat(FirebaseData &fbdo, const String &path, float floatValue, const String &ETag)
{
    return RTDB.setFloat(&fbdo, path.c_str(), floatValue, ETag.c_str());
}

bool FirebaseESP32::setFloat(FirebaseData &fbdo, const String &path, float floatValue, float priority, const String &ETag)
{
    return RTDB.setFloat(&fbdo, path.c_str(), floatValue, priority, ETag.c_str());
}

bool FirebaseESP32::setDouble(FirebaseData &fbdo, const String &path, double doubleValue)
{
    return RTDB.setDouble(&fbdo, path.c_str(), doubleValue);
}

bool FirebaseESP32::setDouble(FirebaseData &fbdo, const String &path, double doubleValue, float priority)
{
    return RTDB.setDouble(&fbdo, path.c_str(), doubleValue, priority);
}

bool FirebaseESP32::setDouble(FirebaseData &fbdo, const String &path, double doubleValue, const String &ETag)
{
    return RTDB.setDouble(&fbdo, path.c_str(), doubleValue, ETag.c_str());
}

bool FirebaseESP32::setDouble(FirebaseData &fbdo, const String &path, double doubleValue, float priority, const String &ETag)
{
    return RTDB.setDouble(&fbdo, path.c_str(), doubleValue, priority, ETag.c_str());
}

bool FirebaseESP32::setBool(FirebaseData &fbdo, const String &path, bool boolValue)
{
    return RTDB.setBool(&fbdo, path.c_str(), boolValue);
}

bool FirebaseESP32::setBool(FirebaseData &fbdo, const String &path, bool boolValue, float priority)
{
    return RTDB.setBool(&fbdo, path.c_str(), boolValue, priority);
}

bool FirebaseESP32::setBool(FirebaseData &fbdo, const String &path, bool boolValue, const String &ETag)
{
    return RTDB.setBool(&fbdo, path.c_str(), boolValue, ETag.c_str());
}

bool FirebaseESP32::setBool(FirebaseData &fbdo, const String &path, bool boolValue, float priority, const String &ETag)
{
    return RTDB.setBool(&fbdo, path.c_str(), boolValue, priority, ETag.c_str());
}

bool FirebaseESP32::setString(FirebaseData &fbdo, const String &path, const String &stringValue)
{
    return RTDB.setString(&fbdo, path.c_str(), stringValue);
}

bool FirebaseESP32::setString(FirebaseData &fbdo, const String &path, const String &stringValue, float priority)
{

    return RTDB.setString(&fbdo, path.c_str(), stringValue, priority);
}

bool FirebaseESP32::setString(FirebaseData &fbdo, const String &path, const String &stringValue, const String &ETag)
{
    return RTDB.setString(&fbdo, path.c_str(), stringValue, ETag.c_str());
}

bool FirebaseESP32::setString(FirebaseData &fbdo, const String &path, const String &stringValue, float priority, const String &ETag)
{
    return RTDB.setString(&fbdo, path.c_str(), stringValue, priority, ETag.c_str());
}

bool FirebaseESP32::setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json)
{
    return RTDB.setJSON(&fbdo, path.c_str(), &json);
}

bool FirebaseESP32::setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
    return RTDB.setJSON(&fbdo, path.c_str(), &json, priority);
}

bool FirebaseESP32::setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, const String &ETag)
{
    return RTDB.setJSON(&fbdo, path.c_str(), &json, ETag.c_str());
}

bool FirebaseESP32::setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority, const String &ETag)
{
    return RTDB.setJSON(&fbdo, path.c_str(), &json, priority, ETag.c_str());
}

bool FirebaseESP32::setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr)
{
    return RTDB.setArray(&fbdo, path.c_str(), &arr);
}

bool FirebaseESP32::setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority)
{
    return RTDB.setArray(&fbdo, path.c_str(), &arr, priority);
}

bool FirebaseESP32::setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, const String &ETag)
{
    return RTDB.setArray(&fbdo, path.c_str(), &arr, ETag.c_str());
}

bool FirebaseESP32::setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority, const String &ETag)
{
    return RTDB.setArray(&fbdo, path.c_str(), &arr, priority, ETag.c_str());
}

bool FirebaseESP32::setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size)
{
    return RTDB.setBlob(&fbdo, path.c_str(), blob, size);
}

bool FirebaseESP32::setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority)
{
    return RTDB.setBlob(&fbdo, path.c_str(), blob, size, priority);
}

bool FirebaseESP32::setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, const String &ETag)
{
    return RTDB.setBlob(&fbdo, path.c_str(), blob, size, ETag.c_str());
}

bool FirebaseESP32::setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority, const String &ETag)
{
    return RTDB.setBlob(&fbdo, path.c_str(), blob, size, priority, ETag.c_str());
}

bool FirebaseESP32::setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName)
{
    return RTDB.setFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str());
}

bool FirebaseESP32::setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority)
{
    return RTDB.setFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), priority);
}

bool FirebaseESP32::setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, const String &ETag)
{
    return RTDB.setFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), ETag.c_str());
}

bool FirebaseESP32::setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority, const String &ETag)
{
    return RTDB.setFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), priority, ETag.c_str());
}

bool FirebaseESP32::setTimestamp(FirebaseData &fbdo, const String &path)
{
    return RTDB.setTimestamp(&fbdo, path.c_str());
}

bool FirebaseESP32::updateNode(FirebaseData &fbdo, const String path, FirebaseJson &json)
{
    return RTDB.updateNode(&fbdo, path.c_str(), &json);
}

bool FirebaseESP32::updateNode(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
    return RTDB.updateNode(&fbdo, path.c_str(), &json, priority);
}

bool FirebaseESP32::updateNodeSilent(FirebaseData &fbdo, const String &path, FirebaseJson &json)
{
    return RTDB.updateNodeSilent(&fbdo, path.c_str(), &json);
}

bool FirebaseESP32::updateNodeSilent(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
    return RTDB.updateNodeSilent(&fbdo, path.c_str(), &json, priority);
}

bool FirebaseESP32::get(FirebaseData &fbdo, const String &path)
{
    return RTDB.get(&fbdo, path.c_str());
}

bool FirebaseESP32::getInt(FirebaseData &fbdo, const String &path)
{
    return RTDB.getInt(&fbdo, path.c_str());
}

bool FirebaseESP32::getInt(FirebaseData &fbdo, const String &path, int &target)
{
    return RTDB.getInt(&fbdo, path.c_str(), &target);
}

bool FirebaseESP32::getFloat(FirebaseData &fbdo, const String &path)
{
    return RTDB.getFloat(&fbdo, path.c_str());
}

bool FirebaseESP32::getFloat(FirebaseData &fbdo, const String &path, float &target)
{
    return RTDB.getFloat(&fbdo, path.c_str(), &target);
}

bool FirebaseESP32::getDouble(FirebaseData &fbdo, const String &path)
{
    return RTDB.getDouble(&fbdo, path.c_str());
}

bool FirebaseESP32::getDouble(FirebaseData &fbdo, const String &path, double &target)
{
    return RTDB.getDouble(&fbdo, path.c_str(), &target);
}

bool FirebaseESP32::getBool(FirebaseData &fbdo, const String &path)
{
    return RTDB.getBool(&fbdo, path.c_str());
}

bool FirebaseESP32::getBool(FirebaseData &fbdo, const String &path, bool &target)
{
    return RTDB.getBool(&fbdo, path.c_str(), &target);
}

bool FirebaseESP32::getString(FirebaseData &fbdo, const String &path)
{
    return RTDB.getString(&fbdo, path.c_str());
}

bool FirebaseESP32::getString(FirebaseData &fbdo, const String &path, String &target)
{
    return RTDB.getString(&fbdo, path.c_str(), &target);
}

bool FirebaseESP32::getJSON(FirebaseData &fbdo, const String &path)
{
    return RTDB.getJSON(&fbdo, path.c_str());
}

bool FirebaseESP32::getJSON(FirebaseData &fbdo, const String &path, FirebaseJson *target)
{
    return RTDB.getJSON(&fbdo, path.c_str(), target);
}

bool FirebaseESP32::getJSON(FirebaseData &fbdo, const String &path, QueryFilter &query)
{
    return RTDB.getJSON(&fbdo, path.c_str(), &query);
}

bool FirebaseESP32::getJSON(FirebaseData &fbdo, const String &path, QueryFilter &query, FirebaseJson *target)
{
    return RTDB.getJSON(&fbdo, path.c_str(), &query, target);
}

bool FirebaseESP32::getArray(FirebaseData &fbdo, const String &path)
{
    return RTDB.getArray(&fbdo, path.c_str());
}

bool FirebaseESP32::getArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray *target)
{
    return RTDB.getArray(&fbdo, path.c_str(), target);
}

bool FirebaseESP32::getArray(FirebaseData &fbdo, const String &path, QueryFilter &query)
{
    return RTDB.getArray(&fbdo, path.c_str(), &query);
}

bool FirebaseESP32::getArray(FirebaseData &fbdo, const String &path, QueryFilter &query, FirebaseJsonArray *target)
{
    return RTDB.getArray(&fbdo, path.c_str(), &query, target);
}

bool FirebaseESP32::getBlob(FirebaseData &fbdo, const String &path)
{
    return RTDB.getBlob(&fbdo, path.c_str());
}

bool FirebaseESP32::getBlob(FirebaseData &fbdo, const String &path, std::vector<uint8_t> &target)
{
    return RTDB.getBlob(&fbdo, path.c_str(), &target);
}

bool FirebaseESP32::getFile(FirebaseData &fbdo, uint8_t storageType, const String &nodePath, const String &fileName)
{
    return RTDB.getFile(&fbdo, getMemStorageType(storageType), nodePath.c_str(), fileName.c_str());
}

bool FirebaseESP32::deleteNode(FirebaseData &fbdo, const String &path)
{
    return RTDB.deleteNode(&fbdo, path.c_str());
}

bool FirebaseESP32::deleteNode(FirebaseData &fbdo, const String &path, const String &ETag)
{
    return RTDB.deleteNode(&fbdo, path.c_str(), ETag.c_str());
}

bool FirebaseESP32::beginStream(FirebaseData &fbdo, const String &path)
{
    return RTDB.beginStream(&fbdo, path.c_str());
}

bool FirebaseESP32::beginMultiPathStream(FirebaseData &fbdo, const String &parentPath, const String *childPath, size_t size)
{
    return RTDB.beginMultiPathStream(&fbdo, parentPath.c_str(), childPath, size);
}

bool FirebaseESP32::readStream(FirebaseData &fbdo)
{
    return RTDB.readStream(&fbdo);
}

bool FirebaseESP32::endStream(FirebaseData &fbdo)
{
    return RTDB.endStream(&fbdo);
}

void FirebaseESP32::processErrorQueue(FirebaseData &fbdo, FirebaseData::QueueInfoCallback callback)
{
    return RTDB.processErrorQueue(&fbdo, callback);
}

uint32_t FirebaseESP32::getErrorQueueID(FirebaseData &fbdo)
{
    return RTDB.getErrorQueueID(&fbdo);
}

bool FirebaseESP32::isErrorQueueExisted(FirebaseData &fbdo, uint32_t errorQueueID)
{
    return RTDB.isErrorQueueExisted(&fbdo, errorQueueID);
}

void FirebaseESP32::errorToString(int httpCode, std::string &buff)
{
    Signer.errorToString(httpCode, buff);
}

void FirebaseESP32::setStreamCallback(FirebaseData &fbdo, FirebaseData::StreamEventCallback dataAvailableCallback, FirebaseData::StreamTimeoutCallback timeoutCallback, size_t streamTaskStackSize)
{
    RTDB.setStreamCallback(&fbdo, dataAvailableCallback, timeoutCallback, streamTaskStackSize);
}

void FirebaseESP32::setMultiPathStreamCallback(FirebaseData &fbdo, FirebaseData::MultiPathStreamEventCallback multiPathDataCallback, FirebaseData::StreamTimeoutCallback timeoutCallback, size_t streamTaskStackSize)
{
    RTDB.setMultiPathStreamCallback(&fbdo, multiPathDataCallback, timeoutCallback, streamTaskStackSize);
}

void FirebaseESP32::removeStreamCallback(FirebaseData &fbdo)
{
    RTDB.removeStreamCallback(&fbdo);
}

void FirebaseESP32::removeMultiPathStreamCallback(FirebaseData &fbdo)
{
    RTDB.removeMultiPathStreamCallback(&fbdo);
}

void FirebaseESP32::beginAutoRunErrorQueue(FirebaseData &fbdo, FirebaseData::QueueInfoCallback callback, size_t queueTaskStackSize)
{
    RTDB.beginAutoRunErrorQueue(&fbdo, callback, queueTaskStackSize);
}

void FirebaseESP32::endAutoRunErrorQueue(FirebaseData &fbdo)
{
    RTDB.endAutoRunErrorQueue(&fbdo);
}

void FirebaseESP32::clearErrorQueue(FirebaseData &fbdo)
{
    RTDB.clearErrorQueue(&fbdo);
}

bool FirebaseESP32::backup(FirebaseData &fbdo, uint8_t storageType, const String &nodePath, const String &dirPath)
{
    return RTDB.backup(&fbdo, getMemStorageType(storageType), nodePath.c_str(), dirPath.c_str());
}

bool FirebaseESP32::restore(FirebaseData &fbdo, uint8_t storageType, const String &nodePath, const String &dirPath)
{
    return RTDB.restore(&fbdo, getMemStorageType(storageType), nodePath.c_str(), dirPath.c_str());
}

void FirebaseESP32::setMaxRetry(FirebaseData &fbdo, uint8_t num)
{
    RTDB.setMaxRetry(&fbdo, num);
}

void FirebaseESP32::setMaxErrorQueue(FirebaseData &fbdo, uint8_t num)
{
    RTDB.setMaxErrorQueue(&fbdo, num);
}

bool FirebaseESP32::saveErrorQueue(FirebaseData &fbdo, const String &filename, uint8_t storageType)
{
    return RTDB.saveErrorQueue(&fbdo, filename.c_str(), getMemStorageType(storageType));
}

bool FirebaseESP32::restoreErrorQueue(FirebaseData &fbdo, const String &filename, uint8_t storageType)
{
    return RTDB.restoreErrorQueue(&fbdo, filename.c_str(), getMemStorageType(storageType));
}

uint8_t FirebaseESP32::errorQueueCount(FirebaseData &fbdo, const String &filename, uint8_t storageType)
{
    return RTDB.errorQueueCount(&fbdo, filename.c_str(), getMemStorageType(storageType));
}

bool FirebaseESP32::deleteStorageFile(const String &filename, uint8_t storageType)
{
    return RTDB.deleteStorageFile(filename.c_str(), getMemStorageType(storageType));
}

bool FirebaseESP32::isErrorQueueFull(FirebaseData &fbdo)
{
    return RTDB.isErrorQueueFull(&fbdo);
}

uint8_t FirebaseESP32::errorQueueCount(FirebaseData &fbdo)
{
    return RTDB.errorQueueCount(&fbdo);
}

bool FirebaseESP32::handleFCMRequest(FirebaseData &fbdo, fb_esp_fcm_msg_type messageType)
{
    if (!fbdo.reconnect())
        return false;

    if (!ut->waitIdle(fbdo._ss.http_code))
        return false;

    if (fbdo.fcm._server_key.length() == 0)
    {
        fbdo._ss.http_code = FIREBASE_ERROR_HTTPC_NO_FCM_SERVER_KEY_PROVIDED;
        return false;
    }

    if (fbdo.fcm._deviceToken.size() == 0 && messageType == fb_esp_fcm_msg_type::msg_single)
    {
        fbdo._ss.http_code = FIREBASE_ERROR_HTTPC_NO_FCM_DEVICE_TOKEN_PROVIDED;
        return false;
    }

    if (messageType == fb_esp_fcm_msg_type::msg_single && fbdo.fcm._deviceToken.size() > 0 && fbdo.fcm._index > fbdo.fcm._deviceToken.size() - 1)
    {
        fbdo._ss.http_code = FIREBASE_ERROR_HTTPC_NO_FCM_INDEX_NOT_FOUND_IN_DEVICE_TOKEN_PROVIDED;
        return false;
    }

    if (messageType == fb_esp_fcm_msg_type::msg_topic && fbdo.fcm._topic.length() == 0)
    {
        fbdo._ss.http_code = FIREBASE_ERROR_HTTPC_NO_FCM_TOPIC_PROVIDED;
        return false;
    }

    if (Signer.getCfg()->_int.fb_processing)
    {
        fbdo._ss.http_code = FIREBASE_ERROR_HTTPC_ERROR_CONNECTION_INUSED;
        return false;
    }

    Signer.getCfg()->_int.fb_processing = true;

    fbdo.fcm.fcm_begin(fbdo);

    return fbdo.fcm.fcm_send(fbdo, messageType);
}

bool FirebaseESP32::sendMessage(FirebaseData &fbdo, uint16_t index)
{
    fbdo.fcm._index = index;
    return handleFCMRequest(fbdo, fb_esp_fcm_msg_type::msg_single);
}

bool FirebaseESP32::broadcastMessage(FirebaseData &fbdo)
{
    return handleFCMRequest(fbdo, fb_esp_fcm_msg_type::msg_multicast);
}

bool FirebaseESP32::sendTopic(FirebaseData &fbdo)
{
    return handleFCMRequest(fbdo, fb_esp_fcm_msg_type::msg_topic);
}

bool FirebaseESP32::sdBegin(int8_t ss, int8_t sck, int8_t miso, int8_t mosi)
{
    if (Signer.getCfg())
    {
        Signer.getCfg()->_int.sd_config.sck = sck;
        Signer.getCfg()->_int.sd_config.miso = miso;
        Signer.getCfg()->_int.sd_config.mosi = mosi;
        Signer.getCfg()->_int.sd_config.ss = ss;
    }
#if defined(ESP32)
    if (ss > -1)
    {
        SPI.begin(sck, miso, mosi, ss);
        return SD_FS.begin(ss, SPI);
    }
    else
        return SD_FS.begin();
#elif defined(ESP8266)
    if (ss > -1)
        return SD_FS.begin(ss);
    else
        return SD_FS.begin(SD_CS_PIN);
#endif
    return false;
}

fb_esp_mem_storage_type FirebaseESP32::getMemStorageType(uint8_t old_type)
{
    return (fb_esp_mem_storage_type)(old_type);
}

FirebaseESP32 Firebase = FirebaseESP32();

#endif /* ESP32 */

#endif /* FirebaseESP32_CPP */