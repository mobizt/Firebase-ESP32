/**
 * Google's Firebase Realtime Database Arduino Library for ESP32, v3.10.0
 *
 * June 22, 2021
 *
 *   Updates:
 * 
 * - Improve memory usage.
 * - Add support classes exclusion.
 * 
 *
 * 
 * This library provides ESP32 to perform REST API by GET PUT, POST, PATCH,
 * DELETE data from/to with Google's Firebase database using get, set, update
 * and delete calls.
 *
 * The library was tested and work well with ESP32 based modules.
 *
 * The MIT License (MIT)
 * Copyright (c) 2021 K. Suwatchai (Mobizt)
 *
 *
 * Permission is hereby granted, free of charge, to any person returning a copy
 * of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of
 * the Software, and to permit persons to whom the Software is furnished to do
 * so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
 * OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER
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

    if (cfg)
        delete cfg;

    if (auth)
        delete auth;
}

void FirebaseESP32::begin(FirebaseConfig *config, FirebaseAuth *auth)
{
    init(config, auth);

    if (cfg->service_account.json.path.length() > 0)
    {
        if (!Signer.parseSAFile())
            cfg->signer.tokens.status = token_status_uninitialized;
    }

    if (cfg->signer.tokens.legacy_token.length() > 0)
        Signer.setTokenType(token_type_legacy_token);
    else if (Signer.tokenSigninDataReady())
    {
        if (auth->token.uid.length() == 0)
            Signer.setTokenType(token_type_oauth2_access_token);
        else
            Signer.setTokenType(token_type_custom_token);
    }
    else if (Signer.userSigninDataReady())
        Signer.setTokenType(token_type_id_token);

    struct fb_esp_url_info_t uinfo;
    cfg->_int.fb_auth_uri = cfg->signer.tokens.token_type == token_type_legacy_token || cfg->signer.tokens.token_type == token_type_id_token;

    if (cfg->host.length() > 0)
        cfg->database_url = cfg->host;

    if (cfg->database_url.length() > 0)
    {
        ut->getUrlInfo(cfg->database_url.c_str(), uinfo);
        cfg->database_url = uinfo.host;
    }

    if (strlen_P(cfg->cert.data))
        cfg->_int.fb_caCert = cfg->cert.data;

    if (cfg->cert.file.length() > 0)
    {
        if (cfg->cert.file_storage == mem_storage_type_sd && !cfg->_int.fb_sd_rdy)
            cfg->_int.fb_sd_rdy = ut->sdTest(cfg->_int.fb_file);
        else if (cfg->cert.file_storage == mem_storage_type_flash && !cfg->_int.fb_flash_rdy)
            ut->flashTest();
    }

    Signer.handleToken();
}

void FirebaseESP32::begin(const String &databaseURL, const String &databaseSecret)
{
    if (!cfg)
        cfg = new FirebaseConfig();

    if (!auth)
        auth = new FirebaseAuth();

    cfg->database_url = databaseURL.c_str();
    cfg->signer.tokens.legacy_token = databaseSecret.c_str();
    begin(cfg, auth);
}

void FirebaseESP32::begin(const String &databaseURL, const String &databaseSecret, const char *caCert)
{
    if (!cfg)
        cfg = new FirebaseConfig();

    if (!auth)
        auth = new FirebaseAuth();

    cfg->database_url = databaseURL.c_str();
    cfg->signer.tokens.legacy_token = databaseSecret.c_str();
    cfg->cert.data = caCert;
    begin(cfg, auth);
}

void FirebaseESP32::begin(const String &databaseURL, const String &databaseSecret, const String &caCertFile, uint8_t storageType)
{
    if (!cfg)
        cfg = new FirebaseConfig();

    if (!auth)
        auth = new FirebaseAuth();

    cfg->database_url = databaseURL.c_str();
    cfg->signer.tokens.legacy_token = databaseSecret.c_str();
    cfg->cert.file = caCertFile.c_str();
    cfg->cert.file_storage = storageType;
    begin(cfg, auth);
}

bool FirebaseESP32::signUp(FirebaseConfig *config, FirebaseAuth *auth, const char *email, const char *password)
{
    init(config, auth);
    return Signer.getIdToken(true, email, password);
}

bool FirebaseESP32::sendEmailVerification(FirebaseConfig *config, const char *idToken)
{
    init(config, nullptr);
    return Signer.handleEmailSending(idToken, fb_esp_user_email_sending_type_verify);
}

bool FirebaseESP32::sendResetPassword(FirebaseConfig *config, const char *email)
{
    init(config, nullptr);
    return Signer.handleEmailSending(email, fb_esp_user_email_sending_type_reset_psw);
}

void FirebaseESP32::end(FirebaseData &fbdo)
{
#ifdef ENABLE_RTDB
    endStream(fbdo);
    removeStreamCallback(fbdo);
#endif
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

bool FirebaseESP32::ready()
{
    return Signer.tokenReady();
}

bool FirebaseESP32::authenticated()
{
    return Signer.authenticated;
}

void FirebaseESP32::init(FirebaseConfig *config, FirebaseAuth *auth)
{
    if (this->cfg)
        delete this->cfg;

    if (this->auth)
        delete this->auth;

    this->auth = auth;
    this->cfg = config;

    if (!this->cfg)
        this->cfg = new FirebaseConfig();

    if (!this->auth)
        this->auth = new FirebaseAuth();

    if (ut)
        delete ut;

    ut = new UtilsClass(this->cfg);
#ifdef ENABLE_RTDB
    RTDB.begin(ut);
#endif
    cfg->_int.fb_reconnect_wifi = WiFi.getAutoReconnect();

    cfg->signer.signup = false;
    cfg->signer.signup = false;
    Signer.begin(ut, this->cfg, this->auth);
    ut->clearS(cfg->signer.tokens.error.message);
}

void FirebaseESP32::reconnectWiFi(bool reconnect)
{
    WiFi.setAutoReconnect(reconnect);
}

void FirebaseESP32::setFloatDigits(uint8_t digits)
{
    if (digits < 7)
        cfg->_int.fb_float_digits = digits;
}

void FirebaseESP32::setDoubleDigits(uint8_t digits)
{
    if (digits < 9)
        cfg->_int.fb_double_digits = digits;
}

#ifdef ENABLE_RTDB
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

bool FirebaseESP32::setQueryIndex(FirebaseData &fbdo, const String &path, const String &node, const String &databaseSecret)
{
    return RTDB.setQueryIndex(&fbdo, path.c_str(), node.c_str(), databaseSecret.c_str());
}

bool FirebaseESP32::removeQueryIndex(FirebaseData &fbdo, const String &path, const String &databaseSecret)
{
    return RTDB.removeQueryIndex(&fbdo, path.c_str(), databaseSecret.c_str());
}

bool FirebaseESP32::setReadWriteRules(FirebaseData &fbdo, const String &path, const String &var, const String &readVal, const String &writeVal, const String &databaseSecret)
{
    return RTDB.setReadWriteRules(&fbdo, path.c_str(), var.c_str(), readVal.c_str(), writeVal.c_str(), databaseSecret.c_str());
}

bool FirebaseESP32::pathExist(FirebaseData &fbdo, const String &path)
{
    return RTDB.pathExisted(&fbdo, path.c_str());
}

bool FirebaseESP32::pathExisted(FirebaseData &fbdo, const String &path)
{
    return RTDB.pathExisted(&fbdo, path.c_str());
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

bool FirebaseESP32::setPriorityAsync(FirebaseData &fbdo, const String &path, float priority)
{
    return RTDB.setPriorityAsync(&fbdo, path.c_str(), priority);
}

bool FirebaseESP32::getPriority(FirebaseData &fbdo, const String &path)
{
    return RTDB.getPriority(&fbdo, path.c_str());
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, int intValue)
{
    return RTDB.pushInt(&fbdo, path.c_str(), intValue);
}

bool FirebaseESP32::pushAsync(FirebaseData &fbdo, const String &path, int intValue)
{
    return RTDB.pushIntAsync(&fbdo, path.c_str(), intValue);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, int intValue, float priority)
{
    return RTDB.pushInt(&fbdo, path.c_str(), intValue, priority);
}

bool FirebaseESP32::pushAsync(FirebaseData &fbdo, const String &path, int intValue, float priority)
{
    return RTDB.pushIntAsync(&fbdo, path.c_str(), intValue, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, float floatValue)
{
    return RTDB.pushFloat(&fbdo, path.c_str(), floatValue);
}

bool FirebaseESP32::pushAsync(FirebaseData &fbdo, const String &path, float floatValue)
{
    return RTDB.pushFloatAsync(&fbdo, path.c_str(), floatValue);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, float floatValue, float priority)
{
    return RTDB.pushFloat(&fbdo, path.c_str(), floatValue, priority);
}

bool FirebaseESP32::pushAsync(FirebaseData &fbdo, const String &path, float floatValue, float priority)
{
    return RTDB.pushFloatAsync(&fbdo, path.c_str(), floatValue, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, double doubleValue)
{
    return RTDB.pushDouble(&fbdo, path.c_str(), doubleValue);
}

bool FirebaseESP32::pushAsync(FirebaseData &fbdo, const String &path, double doubleValue)
{
    return RTDB.pushDoubleAsync(&fbdo, path.c_str(), doubleValue);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, double doubleValue, float priority)
{
    return RTDB.pushDouble(&fbdo, path.c_str(), doubleValue, priority);
}

bool FirebaseESP32::pushAsync(FirebaseData &fbdo, const String &path, double doubleValue, float priority)
{
    return RTDB.pushDoubleAsync(&fbdo, path.c_str(), doubleValue, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, bool boolValue)
{
    return RTDB.pushBool(&fbdo, path.c_str(), boolValue);
}

bool FirebaseESP32::pushAsync(FirebaseData &fbdo, const String &path, bool boolValue)
{
    return RTDB.pushBoolAsync(&fbdo, path.c_str(), boolValue);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, bool boolValue, float priority)
{
    return RTDB.pushBool(&fbdo, path.c_str(), boolValue, priority);
}

bool FirebaseESP32::pushAsync(FirebaseData &fbdo, const String &path, bool boolValue, float priority)
{
    return RTDB.pushBoolAsync(&fbdo, path.c_str(), boolValue, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, const char *stringValue)
{
    return RTDB.pushString(&fbdo, path.c_str(), stringValue);
}

bool FirebaseESP32::pushAsync(FirebaseData &fbdo, const String &path, const char *stringValue)
{
    return RTDB.pushStringAsync(&fbdo, path.c_str(), stringValue);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, const String &stringValue)
{
    return RTDB.pushString(&fbdo, path.c_str(), stringValue);
}

bool FirebaseESP32::pushAsync(FirebaseData &fbdo, const String &path, const String &stringValue)
{
    return RTDB.pushStringAsync(&fbdo, path.c_str(), stringValue);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, const char *stringValue, float priority)
{
    return RTDB.pushString(&fbdo, path.c_str(), stringValue, priority);
}

bool FirebaseESP32::pushAsync(FirebaseData &fbdo, const String &path, const char *stringValue, float priority)
{
    return RTDB.pushStringAsync(&fbdo, path.c_str(), stringValue, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, const String &stringValue, float priority)
{
    return RTDB.pushString(&fbdo, path.c_str(), stringValue, priority);
}

bool FirebaseESP32::pushAsync(FirebaseData &fbdo, const String &path, const String &stringValue, float priority)
{
    return RTDB.pushStringAsync(&fbdo, path.c_str(), stringValue, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, FirebaseJson &json)
{
    return RTDB.pushJSON(&fbdo, path.c_str(), &json);
}

bool FirebaseESP32::pushAsync(FirebaseData &fbdo, const String &path, FirebaseJson &json)
{
    return RTDB.pushJSONAsync(&fbdo, path.c_str(), &json);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
    return RTDB.pushJSON(&fbdo, path.c_str(), &json, priority);
}

bool FirebaseESP32::pushAsync(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
    return RTDB.pushJSONAsync(&fbdo, path.c_str(), &json, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr)
{
    return RTDB.pushArray(&fbdo, path.c_str(), &arr);
}

bool FirebaseESP32::pushAsync(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr)
{
    return RTDB.pushArrayAsync(&fbdo, path.c_str(), &arr);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority)
{
    return RTDB.pushArray(&fbdo, path.c_str(), &arr, priority);
}

bool FirebaseESP32::pushAsync(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority)
{
    return RTDB.pushArrayAsync(&fbdo, path.c_str(), &arr, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size)
{
    return RTDB.pushBlob(&fbdo, path.c_str(), blob, size);
}

bool FirebaseESP32::pushAsync(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size)
{
    return RTDB.pushBlobAsync(&fbdo, path.c_str(), blob, size);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority)
{
    return RTDB.pushBlob(&fbdo, path.c_str(), blob, size, priority);
}

bool FirebaseESP32::pushAsync(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority)
{
    return RTDB.pushBlobAsync(&fbdo, path.c_str(), blob, size, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName)
{
    return RTDB.pushFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str());
}

bool FirebaseESP32::pushAsync(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName)
{
    return RTDB.pushFileAsync(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str());
}

bool FirebaseESP32::push(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority)
{
    return RTDB.pushFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), priority);
}

bool FirebaseESP32::pushAsync(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority)
{
    return RTDB.pushFileAsync(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), priority);
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

bool FirebaseESP32::pushIntAsync(FirebaseData &fbdo, const String &path, int intValue)
{
    return RTDB.pushIntAsync(&fbdo, path.c_str(), intValue);
}

bool FirebaseESP32::pushInt(FirebaseData &fbdo, const String &path, int intValue, float priority)
{
    return RTDB.pushInt(&fbdo, path.c_str(), intValue, priority);
}

bool FirebaseESP32::pushIntAsync(FirebaseData &fbdo, const String &path, int intValue, float priority)
{
    return RTDB.pushIntAsync(&fbdo, path.c_str(), intValue, priority);
}

bool FirebaseESP32::pushFloat(FirebaseData &fbdo, const String &path, float floatValue)
{
    return RTDB.pushFloat(&fbdo, path.c_str(), floatValue);
}

bool FirebaseESP32::pushFloatAsync(FirebaseData &fbdo, const String &path, float floatValue)
{
    return RTDB.pushFloatAsync(&fbdo, path.c_str(), floatValue);
}

bool FirebaseESP32::pushFloat(FirebaseData &fbdo, const String &path, float floatValue, float priority)
{
    return RTDB.pushFloat(&fbdo, path.c_str(), floatValue, priority);
}

bool FirebaseESP32::pushFloatAsync(FirebaseData &fbdo, const String &path, float floatValue, float priority)
{
    return RTDB.pushFloat(&fbdo, path.c_str(), floatValue, priority);
}

bool FirebaseESP32::pushDouble(FirebaseData &fbdo, const String &path, double doubleValue)
{
    return RTDB.pushDouble(&fbdo, path.c_str(), doubleValue);
}

bool FirebaseESP32::pushDoubleAsync(FirebaseData &fbdo, const String &path, double doubleValue)
{
    return RTDB.pushDoubleAsync(&fbdo, path.c_str(), doubleValue);
}

bool FirebaseESP32::pushDouble(FirebaseData &fbdo, const String &path, double doubleValue, float priority)
{
    return RTDB.pushDouble(&fbdo, path.c_str(), doubleValue, priority);
}

bool FirebaseESP32::pushDoubleAsync(FirebaseData &fbdo, const String &path, double doubleValue, float priority)
{
    return RTDB.pushDoubleAsync(&fbdo, path.c_str(), doubleValue, priority);
}

bool FirebaseESP32::pushBool(FirebaseData &fbdo, const String &path, bool boolValue)
{
    return RTDB.pushBool(&fbdo, path.c_str(), boolValue);
}

bool FirebaseESP32::pushBoolAsync(FirebaseData &fbdo, const String &path, bool boolValue)
{
    return RTDB.pushBoolAsync(&fbdo, path.c_str(), boolValue);
}

bool FirebaseESP32::pushBool(FirebaseData &fbdo, const String &path, bool boolValue, float priority)
{
    return RTDB.pushBool(&fbdo, path.c_str(), boolValue, priority);
}

bool FirebaseESP32::pushBoolAsync(FirebaseData &fbdo, const String &path, bool boolValue, float priority)
{
    return RTDB.pushBoolAsync(&fbdo, path.c_str(), boolValue, priority);
}

bool FirebaseESP32::pushString(FirebaseData &fbdo, const String &path, const String &stringValue)
{
    return RTDB.pushString(&fbdo, path.c_str(), stringValue);
}

bool FirebaseESP32::pushStringAsync(FirebaseData &fbdo, const String &path, const String &stringValue)
{
    return RTDB.pushStringAsync(&fbdo, path.c_str(), stringValue);
}

bool FirebaseESP32::pushString(FirebaseData &fbdo, const String &path, const String &stringValue, float priority)
{
    return RTDB.pushString(&fbdo, path.c_str(), stringValue, priority);
}

bool FirebaseESP32::pushStringAsync(FirebaseData &fbdo, const String &path, const String &stringValue, float priority)
{
    return RTDB.pushStringAsync(&fbdo, path.c_str(), stringValue, priority);
}

bool FirebaseESP32::pushJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json)
{
    return RTDB.pushJSON(&fbdo, path.c_str(), &json);
}

bool FirebaseESP32::pushJSONAsync(FirebaseData &fbdo, const String &path, FirebaseJson &json)
{
    return RTDB.pushJSONAsync(&fbdo, path.c_str(), &json);
}

bool FirebaseESP32::pushJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
    return RTDB.pushJSON(&fbdo, path.c_str(), &json, priority);
}

bool FirebaseESP32::pushJSONAsync(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
    return RTDB.pushJSONAsync(&fbdo, path.c_str(), &json, priority);
}

bool FirebaseESP32::pushArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr)
{
    return RTDB.pushArray(&fbdo, path.c_str(), &arr);
}

bool FirebaseESP32::pushArrayAsync(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr)
{
    return RTDB.pushArrayAsync(&fbdo, path.c_str(), &arr);
}

bool FirebaseESP32::pushArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority)
{
    return RTDB.pushArray(&fbdo, path.c_str(), &arr, priority);
}

bool FirebaseESP32::pushArrayAsync(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority)
{
    return RTDB.pushArrayAsync(&fbdo, path.c_str(), &arr, priority);
}

bool FirebaseESP32::pushBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size)
{
    return RTDB.pushBlob(&fbdo, path.c_str(), blob, size);
}

bool FirebaseESP32::pushBlobAsync(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size)
{
    return RTDB.pushBlobAsync(&fbdo, path.c_str(), blob, size);
}

bool FirebaseESP32::pushBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority)
{
    return RTDB.pushBlob(&fbdo, path.c_str(), blob, size, priority);
}

bool FirebaseESP32::pushBlobAsync(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority)
{
    return RTDB.pushBlobAsync(&fbdo, path.c_str(), blob, size, priority);
}

bool FirebaseESP32::pushFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName)
{
    return RTDB.pushFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str());
}

bool FirebaseESP32::pushFileAsync(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName)
{
    return RTDB.pushFileAsync(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str());
}

bool FirebaseESP32::pushFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority)
{
    return RTDB.pushFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), priority);
}

bool FirebaseESP32::pushFileAsync(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority)
{
    return RTDB.pushFileAsync(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), priority);
}

bool FirebaseESP32::pushTimestamp(FirebaseData &fbdo, const String &path)
{
    return RTDB.pushTimestamp(&fbdo, path.c_str());
}

bool FirebaseESP32::pushTimestampAsync(FirebaseData &fbdo, const String &path)
{
    return RTDB.pushTimestampAsync(&fbdo, path.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, int intValue)
{
    return RTDB.setInt(&fbdo, path.c_str(), intValue);
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, int intValue)
{
    return RTDB.setIntAsync(&fbdo, path.c_str(), intValue);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, int intValue, float priority)
{
    return RTDB.setInt(&fbdo, path.c_str(), intValue, priority);
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, int intValue, float priority)
{
    return RTDB.setIntAsync(&fbdo, path.c_str(), intValue, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, int intValue, const String &ETag)
{
    return RTDB.setInt(&fbdo, path.c_str(), intValue, ETag.c_str());
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, int intValue, const String &ETag)
{
    return RTDB.setIntAsync(&fbdo, path.c_str(), intValue, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, int intValue, float priority, const String &ETag)
{
    return RTDB.setInt(&fbdo, path.c_str(), intValue, priority, ETag.c_str());
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, int intValue, float priority, const String &ETag)
{
    return RTDB.setIntAsync(&fbdo, path.c_str(), intValue, priority, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, float floatValue)
{
    return RTDB.setFloat(&fbdo, path.c_str(), floatValue);
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, float floatValue)
{
    return RTDB.setFloatAsync(&fbdo, path.c_str(), floatValue);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, float floatValue, float priority)
{
    return RTDB.setFloat(&fbdo, path.c_str(), floatValue, priority);
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, float floatValue, float priority)
{
    return RTDB.setFloatAsync(&fbdo, path.c_str(), floatValue, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, float floatValue, const String &ETag)
{
    return RTDB.setFloat(&fbdo, path.c_str(), floatValue, ETag.c_str());
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, float floatValue, const String &ETag)
{
    return RTDB.setFloatAsync(&fbdo, path.c_str(), floatValue, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, float floatValue, float priority, const String &ETag)
{
    return RTDB.setFloat(&fbdo, path.c_str(), floatValue, priority, ETag.c_str());
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, float floatValue, float priority, const String &ETag)
{
    return RTDB.setFloatAsync(&fbdo, path.c_str(), floatValue, priority, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, double doubleValue)
{
    return RTDB.setDouble(&fbdo, path.c_str(), doubleValue);
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, double doubleValue)
{
    return RTDB.setDoubleAsync(&fbdo, path.c_str(), doubleValue);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, double doubleValue, float priority)
{
    return RTDB.setDouble(&fbdo, path.c_str(), doubleValue, priority);
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, double doubleValue, float priority)
{
    return RTDB.setDoubleAsync(&fbdo, path.c_str(), doubleValue, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, double doubleValue, const String &ETag)
{
    return RTDB.setDouble(&fbdo, path.c_str(), doubleValue, ETag.c_str());
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, double doubleValue, const String &ETag)
{
    return RTDB.setDoubleAsync(&fbdo, path.c_str(), doubleValue, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, double doubleValue, float priority, const String &ETag)
{
    return RTDB.setDouble(&fbdo, path.c_str(), doubleValue, priority, ETag.c_str());
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, double doubleValue, float priority, const String &ETag)
{
    return RTDB.setDoubleAsync(&fbdo, path.c_str(), doubleValue, priority, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, bool boolValue)
{
    return RTDB.setBool(&fbdo, path.c_str(), boolValue);
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, bool boolValue)
{
    return RTDB.setBoolAsync(&fbdo, path.c_str(), boolValue);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, bool boolValue, float priority)
{
    return RTDB.setBool(&fbdo, path.c_str(), boolValue, priority);
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, bool boolValue, float priority)
{
    return RTDB.setBoolAsync(&fbdo, path.c_str(), boolValue, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, bool boolValue, const String &ETag)
{
    return RTDB.setBool(&fbdo, path.c_str(), boolValue, ETag.c_str());
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, bool boolValue, const String &ETag)
{
    return RTDB.setBoolAsync(&fbdo, path.c_str(), boolValue, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, bool boolValue, float priority, const String &ETag)
{
    return RTDB.setBool(&fbdo, path.c_str(), boolValue, priority, ETag.c_str());
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, bool boolValue, float priority, const String &ETag)
{
    return RTDB.setBoolAsync(&fbdo, path.c_str(), boolValue, priority, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const char *stringValue)
{
    return RTDB.setString(&fbdo, path.c_str(), stringValue);
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, const char *stringValue)
{
    return RTDB.setStringAsync(&fbdo, path.c_str(), stringValue);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const String &stringValue)
{
    return RTDB.setString(&fbdo, path.c_str(), stringValue);
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, const String &stringValue)
{
    return RTDB.setStringAsync(&fbdo, path.c_str(), stringValue);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const char *stringValue, float priority)
{
    return RTDB.setString(&fbdo, path.c_str(), stringValue, priority);
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, const char *stringValue, float priority)
{
    return RTDB.setStringAsync(&fbdo, path.c_str(), stringValue, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const String &stringValue, float priority)
{
    return RTDB.setString(&fbdo, path.c_str(), stringValue, priority);
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, const String &stringValue, float priority)
{
    return RTDB.setStringAsync(&fbdo, path.c_str(), stringValue, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const char *stringValue, const String &ETag)
{
    return RTDB.setString(&fbdo, path.c_str(), stringValue, ETag.c_str());
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, const char *stringValue, const String &ETag)
{
    return RTDB.setStringAsync(&fbdo, path.c_str(), stringValue, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const String &stringValue, const String &ETag)
{
    return RTDB.setString(&fbdo, path.c_str(), stringValue, ETag.c_str());
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, const String &stringValue, const String &ETag)
{
    return RTDB.setStringAsync(&fbdo, path.c_str(), stringValue, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const char *stringValue, float priority, const String &ETag)
{
    return RTDB.setString(&fbdo, path.c_str(), stringValue, priority, ETag.c_str());
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, const char *stringValue, float priority, const String &ETag)
{
    return RTDB.setStringAsync(&fbdo, path.c_str(), stringValue, priority, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const String &stringValue, float priority, const String &ETag)
{
    return RTDB.setString(&fbdo, path.c_str(), stringValue, priority, ETag.c_str());
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, const String &stringValue, float priority, const String &ETag)
{
    return RTDB.setStringAsync(&fbdo, path.c_str(), stringValue, priority, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJson &json)
{
    return RTDB.setJSON(&fbdo, path.c_str(), &json);
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, FirebaseJson &json)
{
    return RTDB.setJSONAsync(&fbdo, path.c_str(), &json);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr)
{
    return RTDB.setArray(&fbdo, path.c_str(), &arr);
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr)
{
    return RTDB.setArrayAsync(&fbdo, path.c_str(), &arr);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
    return RTDB.setJSON(&fbdo, path.c_str(), &json, priority);
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
    return RTDB.setJSONAsync(&fbdo, path.c_str(), &json, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority)
{
    return RTDB.setArray(&fbdo, path.c_str(), &arr, priority);
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority)
{
    return RTDB.setArrayAsync(&fbdo, path.c_str(), &arr, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJson &json, const String &ETag)
{
    return RTDB.setJSON(&fbdo, path.c_str(), &json, ETag.c_str());
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, FirebaseJson &json, const String &ETag)
{
    return RTDB.setJSONAsync(&fbdo, path.c_str(), &json, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, const String &ETag)
{
    return RTDB.setArray(&fbdo, path.c_str(), &arr, ETag.c_str());
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, const String &ETag)
{
    return RTDB.setArrayAsync(&fbdo, path.c_str(), &arr, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority, const String &ETag)
{
    return RTDB.setJSON(&fbdo, path.c_str(), &json, priority, ETag.c_str());
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority, const String &ETag)
{
    return RTDB.setJSONAsync(&fbdo, path.c_str(), &json, priority, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority, const String &ETag)
{
    return RTDB.setArray(&fbdo, path.c_str(), &arr, priority, ETag.c_str());
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority, const String &ETag)
{
    return RTDB.setArrayAsync(&fbdo, path.c_str(), &arr, priority, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size)
{
    return RTDB.setBlob(&fbdo, path.c_str(), blob, size);
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size)
{
    return RTDB.setBlobAsync(&fbdo, path.c_str(), blob, size);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority)
{
    return RTDB.setBlob(&fbdo, path.c_str(), blob, size, priority);
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority)
{
    return RTDB.setBlobAsync(&fbdo, path.c_str(), blob, size, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, const String &ETag)
{
    return RTDB.setBlob(&fbdo, path.c_str(), blob, size, ETag.c_str());
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, const String &ETag)
{
    return RTDB.setBlobAsync(&fbdo, path.c_str(), blob, size, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority, const String &ETag)
{
    return RTDB.setBlob(&fbdo, path.c_str(), blob, size, priority, ETag.c_str());
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority, const String &ETag)
{
    return RTDB.setBlobAsync(&fbdo, path.c_str(), blob, size, priority, ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName)
{
    return RTDB.setFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str());
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName)
{
    return RTDB.setFileAsync(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority)
{
    return RTDB.setFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), priority);
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority)
{
    return RTDB.setFileAsync(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, const String &ETag)
{
    return RTDB.setFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), ETag.c_str());
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, const String &ETag)
{
    return RTDB.setFileAsync(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), ETag.c_str());
}

bool FirebaseESP32::set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority, const String &ETag)
{
    return RTDB.setFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), priority, ETag.c_str());
}

bool FirebaseESP32::setAsync(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority, const String &ETag)
{
    return RTDB.setFileAsync(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), priority, ETag.c_str());
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

bool FirebaseESP32::setIntAsync(FirebaseData &fbdo, const String &path, int intValue)
{
    return RTDB.setIntAsync(&fbdo, path.c_str(), intValue);
}

bool FirebaseESP32::setInt(FirebaseData &fbdo, const String &path, int intValue, float priority)
{
    return RTDB.setInt(&fbdo, path.c_str(), intValue, priority);
}

bool FirebaseESP32::setIntAsync(FirebaseData &fbdo, const String &path, int intValue, float priority)
{
    return RTDB.setIntAsync(&fbdo, path.c_str(), intValue, priority);
}

bool FirebaseESP32::setInt(FirebaseData &fbdo, const String &path, int intValue, const String &ETag)
{
    return RTDB.setInt(&fbdo, path.c_str(), intValue, ETag.c_str());
}

bool FirebaseESP32::setIntAsync(FirebaseData &fbdo, const String &path, int intValue, const String &ETag)
{
    return RTDB.setIntAsync(&fbdo, path.c_str(), intValue, ETag.c_str());
}

bool FirebaseESP32::setInt(FirebaseData &fbdo, const String &path, int intValue, float priority, const String &ETag)
{
    return RTDB.setInt(&fbdo, path.c_str(), intValue, priority, ETag.c_str());
}

bool FirebaseESP32::setIntAsync(FirebaseData &fbdo, const String &path, int intValue, float priority, const String &ETag)
{
    return RTDB.setIntAsync(&fbdo, path.c_str(), intValue, priority, ETag.c_str());
}

bool FirebaseESP32::setFloat(FirebaseData &fbdo, const String &path, float floatValue)
{
    return RTDB.setFloat(&fbdo, path.c_str(), floatValue);
}

bool FirebaseESP32::setFloatAsync(FirebaseData &fbdo, const String &path, float floatValue)
{
    return RTDB.setFloatAsync(&fbdo, path.c_str(), floatValue);
}

bool FirebaseESP32::setFloat(FirebaseData &fbdo, const String &path, float floatValue, float priority)
{
    return RTDB.setFloat(&fbdo, path.c_str(), floatValue, priority);
}

bool FirebaseESP32::setFloatAsync(FirebaseData &fbdo, const String &path, float floatValue, float priority)
{
    return RTDB.setFloatAsync(&fbdo, path.c_str(), floatValue, priority);
}

bool FirebaseESP32::setFloat(FirebaseData &fbdo, const String &path, float floatValue, const String &ETag)
{
    return RTDB.setFloat(&fbdo, path.c_str(), floatValue, ETag.c_str());
}

bool FirebaseESP32::setFloatAsync(FirebaseData &fbdo, const String &path, float floatValue, const String &ETag)
{
    return RTDB.setFloatAsync(&fbdo, path.c_str(), floatValue, ETag.c_str());
}

bool FirebaseESP32::setFloat(FirebaseData &fbdo, const String &path, float floatValue, float priority, const String &ETag)
{
    return RTDB.setFloat(&fbdo, path.c_str(), floatValue, priority, ETag.c_str());
}

bool FirebaseESP32::setFloatAsync(FirebaseData &fbdo, const String &path, float floatValue, float priority, const String &ETag)
{
    return RTDB.setFloatAsync(&fbdo, path.c_str(), floatValue, priority, ETag.c_str());
}

bool FirebaseESP32::setDouble(FirebaseData &fbdo, const String &path, double doubleValue)
{
    return RTDB.setDouble(&fbdo, path.c_str(), doubleValue);
}

bool FirebaseESP32::setDoubleAsync(FirebaseData &fbdo, const String &path, double doubleValue)
{
    return RTDB.setDoubleAsync(&fbdo, path.c_str(), doubleValue);
}

bool FirebaseESP32::setDouble(FirebaseData &fbdo, const String &path, double doubleValue, float priority)
{
    return RTDB.setDouble(&fbdo, path.c_str(), doubleValue, priority);
}

bool FirebaseESP32::setDoubleAsync(FirebaseData &fbdo, const String &path, double doubleValue, float priority)
{
    return RTDB.setDoubleAsync(&fbdo, path.c_str(), doubleValue, priority);
}

bool FirebaseESP32::setDouble(FirebaseData &fbdo, const String &path, double doubleValue, const String &ETag)
{
    return RTDB.setDouble(&fbdo, path.c_str(), doubleValue, ETag.c_str());
}

bool FirebaseESP32::setDoubleAsync(FirebaseData &fbdo, const String &path, double doubleValue, const String &ETag)
{
    return RTDB.setDoubleAsync(&fbdo, path.c_str(), doubleValue, ETag.c_str());
}

bool FirebaseESP32::setDouble(FirebaseData &fbdo, const String &path, double doubleValue, float priority, const String &ETag)
{
    return RTDB.setDouble(&fbdo, path.c_str(), doubleValue, priority, ETag.c_str());
}

bool FirebaseESP32::setDoubleAsync(FirebaseData &fbdo, const String &path, double doubleValue, float priority, const String &ETag)
{
    return RTDB.setDoubleAsync(&fbdo, path.c_str(), doubleValue, priority, ETag.c_str());
}

bool FirebaseESP32::setBool(FirebaseData &fbdo, const String &path, bool boolValue)
{
    return RTDB.setBool(&fbdo, path.c_str(), boolValue);
}

bool FirebaseESP32::setBoolAsync(FirebaseData &fbdo, const String &path, bool boolValue)
{
    return RTDB.setBoolAsync(&fbdo, path.c_str(), boolValue);
}

bool FirebaseESP32::setBool(FirebaseData &fbdo, const String &path, bool boolValue, float priority)
{
    return RTDB.setBool(&fbdo, path.c_str(), boolValue, priority);
}

bool FirebaseESP32::setBoolAsync(FirebaseData &fbdo, const String &path, bool boolValue, float priority)
{
    return RTDB.setBoolAsync(&fbdo, path.c_str(), boolValue, priority);
}

bool FirebaseESP32::setBool(FirebaseData &fbdo, const String &path, bool boolValue, const String &ETag)
{
    return RTDB.setBool(&fbdo, path.c_str(), boolValue, ETag.c_str());
}

bool FirebaseESP32::setBoolAsync(FirebaseData &fbdo, const String &path, bool boolValue, const String &ETag)
{
    return RTDB.setBoolAsync(&fbdo, path.c_str(), boolValue, ETag.c_str());
}

bool FirebaseESP32::setBool(FirebaseData &fbdo, const String &path, bool boolValue, float priority, const String &ETag)
{
    return RTDB.setBool(&fbdo, path.c_str(), boolValue, priority, ETag.c_str());
}

bool FirebaseESP32::setBoolAsync(FirebaseData &fbdo, const String &path, bool boolValue, float priority, const String &ETag)
{
    return RTDB.setBoolAsync(&fbdo, path.c_str(), boolValue, priority, ETag.c_str());
}

bool FirebaseESP32::setString(FirebaseData &fbdo, const String &path, const String &stringValue)
{
    return RTDB.setString(&fbdo, path.c_str(), stringValue);
}

bool FirebaseESP32::setStringAsync(FirebaseData &fbdo, const String &path, const String &stringValue)
{
    return RTDB.setStringAsync(&fbdo, path.c_str(), stringValue);
}

bool FirebaseESP32::setString(FirebaseData &fbdo, const String &path, const String &stringValue, float priority)
{
    return RTDB.setString(&fbdo, path.c_str(), stringValue, priority);
}

bool FirebaseESP32::setStringAsync(FirebaseData &fbdo, const String &path, const String &stringValue, float priority)
{
    return RTDB.setStringAsync(&fbdo, path.c_str(), stringValue, priority);
}

bool FirebaseESP32::setString(FirebaseData &fbdo, const String &path, const String &stringValue, const String &ETag)
{
    return RTDB.setString(&fbdo, path.c_str(), stringValue, ETag.c_str());
}

bool FirebaseESP32::setStringAsync(FirebaseData &fbdo, const String &path, const String &stringValue, const String &ETag)
{
    return RTDB.setStringAsync(&fbdo, path.c_str(), stringValue, ETag.c_str());
}

bool FirebaseESP32::setString(FirebaseData &fbdo, const String &path, const String &stringValue, float priority, const String &ETag)
{
    return RTDB.setString(&fbdo, path.c_str(), stringValue, priority, ETag.c_str());
}

bool FirebaseESP32::setStringAsync(FirebaseData &fbdo, const String &path, const String &stringValue, float priority, const String &ETag)
{
    return RTDB.setStringAsync(&fbdo, path.c_str(), stringValue, priority, ETag.c_str());
}

bool FirebaseESP32::setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json)
{
    return RTDB.setJSON(&fbdo, path.c_str(), &json);
}

bool FirebaseESP32::setJSONAsync(FirebaseData &fbdo, const String &path, FirebaseJson &json)
{
    return RTDB.setJSONAsync(&fbdo, path.c_str(), &json);
}

bool FirebaseESP32::setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
    return RTDB.setJSON(&fbdo, path.c_str(), &json, priority);
}

bool FirebaseESP32::setJSONAsync(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
    return RTDB.setJSONAsync(&fbdo, path.c_str(), &json, priority);
}

bool FirebaseESP32::setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, const String &ETag)
{
    return RTDB.setJSON(&fbdo, path.c_str(), &json, ETag.c_str());
}

bool FirebaseESP32::setJSONAsync(FirebaseData &fbdo, const String &path, FirebaseJson &json, const String &ETag)
{
    return RTDB.setJSONAsync(&fbdo, path.c_str(), &json, ETag.c_str());
}

bool FirebaseESP32::setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority, const String &ETag)
{
    return RTDB.setJSON(&fbdo, path.c_str(), &json, priority, ETag.c_str());
}

bool FirebaseESP32::setJSONAsync(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority, const String &ETag)
{
    return RTDB.setJSONAsync(&fbdo, path.c_str(), &json, priority, ETag.c_str());
}

bool FirebaseESP32::setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr)
{
    return RTDB.setArray(&fbdo, path.c_str(), &arr);
}

bool FirebaseESP32::setArrayAsync(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr)
{
    return RTDB.setArrayAsync(&fbdo, path.c_str(), &arr);
}

bool FirebaseESP32::setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority)
{
    return RTDB.setArray(&fbdo, path.c_str(), &arr, priority);
}

bool FirebaseESP32::setArrayAsync(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority)
{
    return RTDB.setArrayAsync(&fbdo, path.c_str(), &arr, priority);
}

bool FirebaseESP32::setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, const String &ETag)
{
    return RTDB.setArray(&fbdo, path.c_str(), &arr, ETag.c_str());
}

bool FirebaseESP32::setArrayAsync(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, const String &ETag)
{
    return RTDB.setArrayAsync(&fbdo, path.c_str(), &arr, ETag.c_str());
}

bool FirebaseESP32::setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority, const String &ETag)
{
    return RTDB.setArray(&fbdo, path.c_str(), &arr, priority, ETag.c_str());
}

bool FirebaseESP32::setArrayAsync(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority, const String &ETag)
{
    return RTDB.setArrayAsync(&fbdo, path.c_str(), &arr, priority, ETag.c_str());
}

bool FirebaseESP32::setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size)
{
    return RTDB.setBlob(&fbdo, path.c_str(), blob, size);
}

bool FirebaseESP32::setBlobAsync(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size)
{
    return RTDB.setBlobAsync(&fbdo, path.c_str(), blob, size);
}

bool FirebaseESP32::setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority)
{
    return RTDB.setBlob(&fbdo, path.c_str(), blob, size, priority);
}

bool FirebaseESP32::setBlobAsync(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority)
{
    return RTDB.setBlobAsync(&fbdo, path.c_str(), blob, size, priority);
}

bool FirebaseESP32::setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, const String &ETag)
{
    return RTDB.setBlob(&fbdo, path.c_str(), blob, size, ETag.c_str());
}

bool FirebaseESP32::setBlobAsync(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, const String &ETag)
{
    return RTDB.setBlobAsync(&fbdo, path.c_str(), blob, size, ETag.c_str());
}

bool FirebaseESP32::setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority, const String &ETag)
{
    return RTDB.setBlob(&fbdo, path.c_str(), blob, size, priority, ETag.c_str());
}

bool FirebaseESP32::setBlobAsync(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority, const String &ETag)
{
    return RTDB.setBlobAsync(&fbdo, path.c_str(), blob, size, priority, ETag.c_str());
}

bool FirebaseESP32::setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName)
{
    return RTDB.setFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str());
}

bool FirebaseESP32::setFileAsync(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName)
{
    return RTDB.setFileAsync(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str());
}

bool FirebaseESP32::setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority)
{
    return RTDB.setFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), priority);
}

bool FirebaseESP32::setFileAsync(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority)
{
    return RTDB.setFileAsync(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), priority);
}

bool FirebaseESP32::setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, const String &ETag)
{
    return RTDB.setFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), ETag.c_str());
}

bool FirebaseESP32::setFileAsync(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, const String &ETag)
{
    return RTDB.setFileAsync(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), ETag.c_str());
}

bool FirebaseESP32::setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority, const String &ETag)
{
    return RTDB.setFile(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), priority, ETag.c_str());
}

bool FirebaseESP32::setFileAsync(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority, const String &ETag)
{
    return RTDB.setFileAsync(&fbdo, getMemStorageType(storageType), path.c_str(), fileName.c_str(), priority, ETag.c_str());
}

bool FirebaseESP32::setTimestamp(FirebaseData &fbdo, const String &path)
{
    return RTDB.setTimestamp(&fbdo, path.c_str());
}

bool FirebaseESP32::setTimestampAsync(FirebaseData &fbdo, const String &path)
{
    return RTDB.setTimestampAsync(&fbdo, path.c_str());
}

bool FirebaseESP32::updateNode(FirebaseData &fbdo, const String path, FirebaseJson &json)
{
    return RTDB.updateNode(&fbdo, path.c_str(), &json);
}

bool FirebaseESP32::updateNodeAsync(FirebaseData &fbdo, const String path, FirebaseJson &json)
{
    return RTDB.updateNodeAsync(&fbdo, path.c_str(), &json);
}

bool FirebaseESP32::updateNode(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
    return RTDB.updateNode(&fbdo, path.c_str(), &json, priority);
}

bool FirebaseESP32::updateNodeAsync(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
    return RTDB.updateNodeAsync(&fbdo, path.c_str(), &json, priority);
}

bool FirebaseESP32::updateNodeSilent(FirebaseData &fbdo, const String &path, FirebaseJson &json)
{
    return RTDB.updateNodeSilent(&fbdo, path.c_str(), &json);
}

bool FirebaseESP32::updateNodeSilentAsync(FirebaseData &fbdo, const String &path, FirebaseJson &json)
{
    return RTDB.updateNodeSilentAsync(&fbdo, path.c_str(), &json);
}

bool FirebaseESP32::updateNodeSilent(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
    return RTDB.updateNodeSilent(&fbdo, path.c_str(), &json, priority);
}

bool FirebaseESP32::updateNodeSilentAsync(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
    return RTDB.updateNodeSilentAsync(&fbdo, path.c_str(), &json, priority);
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

bool FirebaseESP32::deleteNodesByTimestamp(FirebaseData &fbdo, const String &path, const String &timestampNode, size_t limit, unsigned long dataRetentionPeriod)
{
    return RTDB.deleteNodesByTimestamp(&fbdo, path.c_str(), timestampNode.c_str(), limit, dataRetentionPeriod);
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

#endif

#ifdef ENABLE_FCM
bool FirebaseESP32::handleFCMRequest(FirebaseData &fbdo, fb_esp_fcm_msg_type messageType)
{
    if (!fbdo.reconnect())
        return false;

    if (!ut)
        ut = new UtilsClass(nullptr);

    if (!ut->waitIdle(fbdo._ss.http_code))
        return false;

    FirebaseJsonData data;

    FirebaseJson json(fbdo.fcm.raw);

    std::string s;
    ut->appendP(s, fb_esp_pgm_str_577, true);

    json.get(data, s.c_str());

    if (data.stringValue.length() == 0)
    {
        fbdo._ss.http_code = FIREBASE_ERROR_HTTPC_NO_FCM_SERVER_KEY_PROVIDED;
        return false;
    }

    if (fbdo.fcm.idTokens.length() == 0 && messageType == fb_esp_fcm_msg_type::msg_single)
    {
        fbdo._ss.http_code = FIREBASE_ERROR_HTTPC_NO_FCM_DEVICE_TOKEN_PROVIDED;
        return false;
    }

    FirebaseJsonArray arr;
    arr.setJsonArrayData(fbdo.fcm.idTokens.c_str());

    if (messageType == fb_esp_fcm_msg_type::msg_single && fbdo.fcm.idTokens.length() > 0 && fbdo.fcm._index > arr.size() - 1)
    {
        fbdo._ss.http_code = FIREBASE_ERROR_HTTPC_NO_FCM_INDEX_NOT_FOUND_IN_DEVICE_TOKEN_PROVIDED;
        return false;
    }

    ut->appendP(s, fb_esp_pgm_str_576, true);

    json.get(data, s.c_str());

    if (messageType == fb_esp_fcm_msg_type::msg_topic && data.stringValue.length() == 0)
    {
        fbdo._ss.http_code = FIREBASE_ERROR_HTTPC_NO_FCM_TOPIC_PROVIDED;
        return false;
    }

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

#endif

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
#if defined(CARD_TYPE_SD)
        return SD_FS.begin(ss, SPI);
#endif
        return false;
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

bool FirebaseESP32::sdMMCBegin(const String &mountpoint, bool mode1bit, bool format_if_mount_failed)
{
#if defined(ESP32)
#if defined(CARD_TYPE_SD_MMC)
    if (Signer.getCfg())
    {
        Signer.getCfg()->_int.sd_config.sd_mmc_mountpoint = mountpoint;
        Signer.getCfg()->_int.sd_config.sd_mmc_mode1bit = mode1bit;
        Signer.getCfg()->_int.sd_config.sd_mmc_format_if_mount_failed = format_if_mount_failed;
    }
    return SD_FS.begin(mountpoint, mode1bit, format_if_mount_failed);
#endif
#endif
    return false;
}

fb_esp_mem_storage_type FirebaseESP32::getMemStorageType(uint8_t old_type)
{
    return (fb_esp_mem_storage_type)(old_type);
}

bool FirebaseESP32::setSystemTime(time_t ts)
{
    return ut->setTimestamp(ts) == 0;
}

FirebaseESP32 Firebase = FirebaseESP32();

#endif /* ESP32 */

#endif /* FirebaseESP32_CPP */