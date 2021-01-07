/**
 * Google's Firebase Realtime Database Arduino Library for ESP32, version 3.8.13
 * 
 * January 7, 2021
 * 
 *   Updates:
 * - Rename the macro name, struct and class name to prevent the naming conflicts with other libraries.
 * 
 * 
 * This library provides ESP32 to perform REST API by GET PUT, POST, PATCH, DELETE data from/to with Google's Firebase database using get, set, update
 * and delete calls. 
 * 
 * The library was test and work well with ESP32s based module and add support for multiple stream event path.
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
  if (_caCert)
    _caCert.reset();
  _caCert = nullptr;
}

void FirebaseESP32::begin(FirebaseConfig *config, FirebaseAuth *auth)
{
  _auth = auth;
  _cfg = config;

  if (_cfg == nullptr)
    _cfg = &_cfg_;

  if (_auth == nullptr)
    _auth = &_auth_;

  _cfg->signer.signup = false;
  _cfg_.signer.signup = false;
  std::string().swap(_cfg_.signer.tokens.error.message);

  if (_cfg->service_account.json.path.length() > 0)
  {
    if (!parseSAFile())
      _cfg->signer.tokens.status = token_status_uninitialized;
  }

  if (tokenSigninDataReady())
  {

    if (_auth->token.uid.length() == 0)
      setTokenType(token_type_oauth2_access_token);
    else
      setTokenType(token_type_custom_token);
  }
  else if (userSigninDataReady())
    setTokenType(token_type_id_token);
  else if (_cfg->signer.tokens.legacy_token.length() > 0)
    setTokenType(token_type_legacy_token);

  struct fb_esp_url_info_t uinfo;
  getUrlInfo(_cfg->host.c_str(), uinfo);
  _cfg->host = uinfo.host;

  if (strlen_P(_cfg->cert.data))
    _caCert = std::shared_ptr<const char>(_cfg->cert.data);

  if (_cfg->cert.file.length() > 0)
  {
    if (_cfg->cert.file_storage == StorageType::SD && !_sdOk)
      _sdOk = sdTest();
    else if (_cfg->cert.file_storage == StorageType::FLASH && !_flashOk)
      _flashOk = FLASH_FS.begin();
  }

  hanldeToken();
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
  if (strlen_P(caCert))
  {
    _cfg_.cert.data = caCert;
    setClock(_gmtOffset);
  }
  begin(&_cfg_, &_auth_);
}

void FirebaseESP32::begin(const String &host, const String &auth, const String &caCertFile, uint8_t storageType)
{
  _cfg_.host = host.c_str();
  _cfg_.signer.tokens.legacy_token = auth.c_str();
  if (caCertFile.length() > 0)
  {
    _cfg_.cert.file = caCertFile.c_str();
    _cfg_.cert.file_storage = storageType;
    setClock(_gmtOffset);
  }
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

  return getIdToken(true, email, password);
}

bool FirebaseESP32::sendEmailVerification(FirebaseConfig *config, const char *idToken)
{
  _cfg = config;
  if (_cfg == nullptr)
    _cfg = &_cfg_;
  return handleEmailSending(idToken, fb_esp_user_email_sending_type_verify);
}

bool FirebaseESP32::sendResetPassword(FirebaseConfig *config, const char *email)
{
  _cfg = config;
  if (_cfg == nullptr)
    _cfg = &_cfg_;
  return handleEmailSending(email, fb_esp_user_email_sending_type_reset_psw);
}

bool FirebaseESP32::tokenSigninDataReady()
{
  if (!_cfg)
    return false;
  return (strlen_P(_cfg->service_account.data.private_key) > 0 || _cfg->signer.pk.length() > 0) && _cfg->service_account.data.client_email.length() > 0 && _cfg->service_account.data.project_id.length() > 0;
}

bool FirebaseESP32::userSigninDataReady()
{
  if (!_auth || !_auth)
    return false;
  return _cfg->api_key.length() > 0 && _auth->user.email.length() > 0 && _auth->user.password.length() > 0;
}

void FirebaseESP32::clearSA()
{
  _cfg->service_account.data.private_key = "";
  _cfg->service_account.data.project_id.clear();
  _cfg->service_account.data.private_key_id.clear();
  _cfg->service_account.data.client_email.clear();
  std::string().swap(_cfg->signer.pk);
}

bool FirebaseESP32::parseSAFile()
{
  if (_cfg->signer.pk.length() > 0)
    return false;

  if (_cfg->service_account.json.storage_type == StorageType::SD && !_sdOk)
    _sdOk = sdTest();
  else if (_cfg->service_account.json.storage_type == StorageType::FLASH && !_flashOk)
    _flashOk = FLASH_FS.begin();

  if (_sdOk || _flashOk)
  {
    if (_cfg->service_account.json.storage_type == StorageType::FLASH)
    {
      if (FLASH_FS.exists(_cfg->service_account.json.path.c_str()))
        file = FLASH_FS.open(_cfg->service_account.json.path.c_str(), FILE_READ);
    }
    else
    {
      if (SD.exists(_cfg->service_account.json.path.c_str()))
        file = SD.open(_cfg->service_account.json.path.c_str(), FILE_READ);
    }

    if (file)
    {
      clearSA();
      _cfg->signer.json = new FirebaseJson();
      _cfg->signer.data = new FirebaseJsonData();
      char *tmp = nullptr;

      size_t len = file.size();
      char *buf = newS(len + 10);
      if (file.available())
      {
        file.readBytes(buf, len);
        _cfg->signer.json->setJsonData(buf);
      }
      file.close();
      delS(buf);

      tmp = strP(fb_esp_pgm_str_243);
      _cfg->signer.json->get(*_cfg->signer.data, (const char *)tmp);
      delS(tmp);
      if (_cfg->signer.data->success)
      {
        if (strposP(_cfg->signer.data->stringValue.c_str(), fb_esp_pgm_str_244, 0) > -1)
        {
          tmp = strP(fb_esp_pgm_str_245);
          _cfg->signer.json->get(*_cfg->signer.data, (const char *)tmp);
          delS(tmp);
          if (_cfg->signer.data->success)
            _cfg->service_account.data.project_id = _cfg->signer.data->stringValue.c_str();
          tmp = strP(fb_esp_pgm_str_246);
          _cfg->signer.json->get(*_cfg->signer.data, (const char *)tmp);
          delS(tmp);
          if (_cfg->signer.data->success)
            _cfg->service_account.data.private_key_id = _cfg->signer.data->stringValue.c_str();
          tmp = strP(fb_esp_pgm_str_247);
          _cfg->signer.json->get(*_cfg->signer.data, (const char *)tmp);
          delS(tmp);
          if (_cfg->signer.data->success)
          {
            tmp = newS(_cfg->signer.data->stringValue.length());
            size_t c = 0;
            for (size_t i = 0; i < _cfg->signer.data->stringValue.length(); i++)
            {
              if (_cfg->signer.data->stringValue[i] == '\\')
              {
                delay(0);
                tmp[c++] = '\n';
                i++;
              }
              else
                tmp[c++] = _cfg->signer.data->stringValue[i];
            }
            _cfg->signer.pk = tmp;
            _cfg->signer.data->stringValue.clear();
            delS(tmp);
          }

          tmp = strP(fb_esp_pgm_str_248);
          _cfg->signer.json->get(*_cfg->signer.data, (const char *)tmp);
          delS(tmp);
          if (_cfg->signer.data->success)
            _cfg->service_account.data.client_email = _cfg->signer.data->stringValue.c_str();
          tmp = strP(fb_esp_pgm_str_253);
          _cfg->signer.json->get(*_cfg->signer.data, (const char *)tmp);
          delS(tmp);
          if (_cfg->signer.data->success)
            _cfg->service_account.data.client_id = _cfg->signer.data->stringValue.c_str();

          delete _cfg->signer.json;
          delete _cfg->signer.data;
          return true;
        }
      }

      delete _cfg->signer.json;
      delete _cfg->signer.data;
    }
  }

  return false;
}

void FirebaseESP32::setTokenType(fb_esp_auth_token_type type)
{
  _cfg->signer.tokens.token_type = type;

  switch (type)
  {

  case token_type_custom_token:
  case token_type_oauth2_access_token:
    break;
  case token_type_id_token:
    break;

  case token_type_legacy_token:
    _cfg->signer.tokens.status = token_status_ready;
    break;

  default:
    break;
  }
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
  _multipleRequests = enable;
}

bool FirebaseESP32::handleSignerError(int code)
{

  if (code > 0)
  {
    if (_cfg->signer.attempts >= MAX_EXCHANGE_TOKEN_ATTEMPTS)
    {
      _cfg->signer.attempts = 0;
      _cfg->signer.step = fb_esp_jwt_generation_step_begin;
    }
  }

  switch (code)
  {

  case 1:
    setTokenError(FIREBASE_ERROR_HTTPC_ERROR_NOT_CONNECTED);
    break;
  case 2:
    _cfg->signer.wcs->stream()->stop();
    setTokenError(FIREBASE_ERROR_HTTPC_ERROR_CONNECTION_LOST);
    break;
  case 3:
    _cfg->signer.wcs->stream()->stop();
    setTokenError(FIREBASE_ERROR_HTTP_CODE_REQUEST_TIMEOUT);
    break;

  default:
    break;
  }

  std::string().swap(_cfg->signer.tokens.jwt);

  if (_cfg->signer.wcs)
    delete _cfg->signer.wcs;
  if (_cfg->signer.json)
    delete _cfg->signer.json;
  if (_cfg->signer.data)
    delete _cfg->signer.data;

  processing = false;

  if (code > 0 && code < 4)
  {
    _cfg->signer.tokens.status = token_status_error;
    _cfg->signer.tokens.error.code = code;
    return false;
  }
  else if (code == 0)
  {
    _cfg->signer.tokens.status = token_status_ready;
    _cfg->signer.attempts = 0;
    _cfg->signer.step = fb_esp_jwt_generation_step_begin;
    return true;
  }

  return false;
}

bool FirebaseESP32::getIdToken(bool createUser, const char *email, const char *password)
{

  _cfg->signer.attempts++;
  _cfg->signer.signup = false;
  delay(0);

  if (_auth == nullptr)
    return false;

  if (_cfg->signer.tokens.status == token_status_on_request || _cfg->signer.tokens.status == token_status_on_refresh || processing)
    return false;

  _cfg->signer.tokens.status = token_status_on_request;
  processing = true;

  setTokenError(FIREBASE_ERROR_TOKEN_NOT_READY);

  _cfg->signer.wcs = new FB_HTTPClient32();
  _cfg->signer.json = new FirebaseJson();
  _cfg->signer.data = new FirebaseJsonData();
  _cfg->signer.wcs->setCACert(nullptr);

  std::string host;
  if (createUser)
    appendP(host, fb_esp_pgm_str_250);
  else
    appendP(host, fb_esp_pgm_str_193);
  appendP(host, fb_esp_pgm_str_4);
  appendP(host, fb_esp_pgm_str_120);
  _cfg->signer.wcs->begin(host.c_str(), 443);

  char *tmp = strP(fb_esp_pgm_str_196);
  if (createUser)
  {
    std::string().swap(_cfg->signer.signupError.message);
    if (strlen(email) > 0 && strlen(password) > 0)
      _cfg->signer.json->add(tmp, email);
  }
  else
    _cfg->signer.json->add(tmp, _auth->user.email.c_str());
  delS(tmp);
  tmp = strP(fb_esp_pgm_str_197);
  if (createUser)
  {
    if (strlen(email) > 0 && strlen(password) > 0)
      _cfg->signer.json->add(tmp, password);
  }
  else
    _cfg->signer.json->add(tmp, _auth->user.password.c_str());
  delS(tmp);
  tmp = strP(fb_esp_pgm_str_198);
  _cfg->signer.json->add(tmp, true);
  delS(tmp);

  String s;
  _cfg->signer.json->toString(s);

  std::string req;
  appendP(req, fb_esp_pgm_str_24);
  appendP(req, fb_esp_pgm_str_6);

  if (createUser)
    appendP(req, fb_esp_pgm_str_259);
  else
  {
    appendP(req, fb_esp_pgm_str_194);
    appendP(req, fb_esp_pgm_str_195);
  }

  req += _cfg->api_key;
  appendP(req, fb_esp_pgm_str_30);

  appendP(req, fb_esp_pgm_str_31);
  if (createUser)
    appendP(req, fb_esp_pgm_str_250);
  else
    appendP(req, fb_esp_pgm_str_193);
  appendP(req, fb_esp_pgm_str_4);
  appendP(req, fb_esp_pgm_str_120);
  appendP(req, fb_esp_pgm_str_21);
  appendP(req, fb_esp_pgm_str_32);
  appendP(req, fb_esp_pgm_str_12);
  tmp = intStr(s.length());
  req += tmp;
  delS(tmp);
  appendP(req, fb_esp_pgm_str_21);
  appendP(req, fb_esp_pgm_str_8);
  appendP(req, fb_esp_pgm_str_129);
  appendP(req, fb_esp_pgm_str_21);
  appendP(req, fb_esp_pgm_str_21);

  req += s.c_str();
  if (_cfg->signer.wcs->send(req.c_str(), "") < 0)
    return handleSignerError(2);

  _cfg->signer.json->clear();

  if (handleTokenResponse(_cfg->signer.wcs, _cfg->signer.json))
  {
    struct fb_esp_auth_token_error_t error;

    tmp = strP(fb_esp_pgm_str_257);
    _cfg->signer.json->get(*_cfg->signer.data, tmp);
    delS(tmp);

    if (_cfg->signer.data->success)
    {
      error.code = _cfg->signer.data->intValue;
      if (!createUser)
        _cfg->signer.tokens.status = token_status_error;

      tmp = strP(fb_esp_pgm_str_258);
      _cfg->signer.json->get(*_cfg->signer.data, tmp);
      delS(tmp);
      if (_cfg->signer.data->success)
        error.message = _cfg->signer.data->stringValue.c_str();
    }
    if (createUser)
      _cfg->signer.signupError = error;
    else
      _cfg->signer.tokens.error = error;

    if (error.code == 0)
    {
      if (createUser)
      {
        _cfg->signer.signup = true;
        _cfg->signer.tokens.token_type = token_type_id_token;
        _auth->user.email = email;
        _auth->user.password = password;
      }

      tmp = strP(fb_esp_pgm_str_200);
      _cfg->signer.json->get(*_cfg->signer.data, tmp);
      delS(tmp);
      if (_cfg->signer.data->success)
        _cfg->signer.tokens.id_token = _cfg->signer.data->stringValue.c_str();
      tmp = strP(fb_esp_pgm_str_201);
      _cfg->signer.json->get(*_cfg->signer.data, tmp);
      delS(tmp);
      if (_cfg->signer.data->success)
        _cfg->signer.tokens.refresh_token = _cfg->signer.data->stringValue.c_str();

      tmp = strP(fb_esp_pgm_str_202);
      _cfg->signer.json->get(*_cfg->signer.data, tmp);
      delS(tmp);
      if (_cfg->signer.data->success)
        _cfg->signer.tokens.expires = millis() + (1000 * atoi(_cfg->signer.data->stringValue.c_str()));

      tmp = strP(fb_esp_pgm_str_175);
      _cfg->signer.json->get(*_cfg->signer.data, tmp);
      delS(tmp);
      if (_cfg->signer.data->success)
        _auth->token.uid = _cfg->signer.data->stringValue.c_str();

      _cfg->signer.json->clear();
      return handleSignerError(0);
    }

    return handleSignerError(4);
  }

  return handleSignerError(3);
}

bool FirebaseESP32::handleEmailSending(const char *payload, fb_esp_user_email_sending_type type)
{

  delay(0);

  if (processing)
    return false;

  processing = true;

  _cfg->signer.wcs = new FB_HTTPClient32();
  _cfg->signer.json = new FirebaseJson();
  _cfg->signer.data = new FirebaseJsonData();
  _cfg->signer.wcs->setCACert(nullptr);

  std::string host;
  appendP(host, fb_esp_pgm_str_193);
  appendP(host, fb_esp_pgm_str_4);
  appendP(host, fb_esp_pgm_str_120);
  _cfg->signer.wcs->begin(host.c_str(), 443);

  char *tmp = strP(fb_esp_pgm_str_260);
  char *tmp2 = nullptr;
  if (type == fb_esp_user_email_sending_type_verify)
  {
    std::string().swap(_cfg->signer.verificationError.message);
    tmp2 = strP(fb_esp_pgm_str_261);
    _cfg->signer.json->add(tmp, (const char *)tmp2);
  }
  else if (type == fb_esp_user_email_sending_type_reset_psw)
  {
    std::string().swap(_cfg->signer.resetPswError.message);
    tmp2 = strP(fb_esp_pgm_str_263);
    _cfg->signer.json->add(tmp, (const char *)tmp2);
  }
  delS(tmp);

  if (type == fb_esp_user_email_sending_type_verify)
  {
    tmp = strP(fb_esp_pgm_str_200);
    if (strlen(payload) > 0)
      _cfg->signer.json->add(tmp, payload);
    else
      _cfg->signer.json->add(tmp, _cfg->signer.tokens.id_token.c_str());
    delS(tmp);
  }
  else if (type == fb_esp_user_email_sending_type_reset_psw)
  {
    tmp = strP(fb_esp_pgm_str_196);
    _cfg->signer.json->add(tmp, payload);
    delS(tmp);
  }

  String s;
  _cfg->signer.json->toString(s);

  std::string req;
  appendP(req, fb_esp_pgm_str_24);
  appendP(req, fb_esp_pgm_str_6);

  appendP(req, fb_esp_pgm_str_194);
  appendP(req, fb_esp_pgm_str_262);

  req += _cfg->api_key;
  appendP(req, fb_esp_pgm_str_30);

  appendP(req, fb_esp_pgm_str_31);

  appendP(req, fb_esp_pgm_str_193);
  appendP(req, fb_esp_pgm_str_4);
  appendP(req, fb_esp_pgm_str_120);
  appendP(req, fb_esp_pgm_str_21);
  appendP(req, fb_esp_pgm_str_32);
  appendP(req, fb_esp_pgm_str_12);
  tmp = intStr(s.length());
  req += tmp;
  delS(tmp);
  appendP(req, fb_esp_pgm_str_21);
  appendP(req, fb_esp_pgm_str_8);
  appendP(req, fb_esp_pgm_str_129);
  appendP(req, fb_esp_pgm_str_21);
  appendP(req, fb_esp_pgm_str_21);

  req += s.c_str();

  if (_cfg->signer.wcs->send(req.c_str(), "") < 0)
    return handleSignerError(2);

  _cfg->signer.json->clear();

  if (handleTokenResponse(_cfg->signer.wcs, _cfg->signer.json))
  {

    struct fb_esp_auth_token_error_t error;

    tmp = strP(fb_esp_pgm_str_257);
    _cfg->signer.json->get(*_cfg->signer.data, tmp);
    delS(tmp);

    if (_cfg->signer.data->success)
    {
      error.code = _cfg->signer.data->intValue;
      tmp = strP(fb_esp_pgm_str_258);
      _cfg->signer.json->get(*_cfg->signer.data, tmp);
      delS(tmp);
      if (_cfg->signer.data->success)
        error.message = _cfg->signer.data->stringValue.c_str();
    }
    if (type == fb_esp_user_email_sending_type_verify)
      _cfg->signer.verificationError = error;
    else if (type == fb_esp_user_email_sending_type_reset_psw)
      _cfg->signer.resetPswError = error;

    if (error.code == 0)
    {
      _cfg->signer.json->clear();
      return handleSignerError(0);
    }

    return handleSignerError(4);
  }

  return handleSignerError(3);
}

bool FirebaseESP32::refreshToken()
{

  _cfg->signer.attempts++;
  delay(0);

  if (_auth == nullptr)
    return false;

  if (_cfg->signer.tokens.status == token_status_on_request || _cfg->signer.tokens.status == token_status_on_refresh || processing)
    return false;

  if (_cfg->signer.tokens.legacy_token.length() > 0 || (_cfg->signer.tokens.refresh_token.length() == 0 && _cfg->signer.tokens.id_token.length() == 0 && _cfg->signer.tokens.access_token.length() == 0))
    return false;

  _cfg->signer.tokens.status = token_status_on_refresh;
  processing = true;

  _cfg->signer.tokens.id_token.clear();
  _cfg->signer.tokens.access_token.clear();

  _cfg->signer.wcs = new FB_HTTPClient32();
  _cfg->signer.json = new FirebaseJson();
  _cfg->signer.data = new FirebaseJsonData();
  _cfg->signer.wcs->setCACert(nullptr);

  std::string host;
  appendP(host, fb_esp_pgm_str_203);
  appendP(host, fb_esp_pgm_str_4);
  appendP(host, fb_esp_pgm_str_120);
  _cfg->signer.wcs->begin(host.c_str(), 443);

  char *tmp = strP(fb_esp_pgm_str_205);
  char *tmp2 = strP(fb_esp_pgm_str_206);
  _cfg->signer.json->add(tmp, (const char *)tmp2);
  delS(tmp);
  delS(tmp2);
  tmp = strP(fb_esp_pgm_str_207);
  _cfg->signer.json->add(tmp, _cfg->signer.tokens.refresh_token.c_str());
  delS(tmp);

  String s;
  _cfg->signer.json->toString(s);

  std::string req;
  appendP(req, fb_esp_pgm_str_24);
  appendP(req, fb_esp_pgm_str_6);
  appendP(req, fb_esp_pgm_str_204);
  req += _cfg->api_key;
  appendP(req, fb_esp_pgm_str_30);

  appendP(req, fb_esp_pgm_str_31);
  appendP(req, fb_esp_pgm_str_203);
  appendP(req, fb_esp_pgm_str_4);
  appendP(req, fb_esp_pgm_str_120);
  appendP(req, fb_esp_pgm_str_21);
  appendP(req, fb_esp_pgm_str_32);
  appendP(req, fb_esp_pgm_str_12);
  tmp = intStr(s.length());
  req += tmp;
  delS(tmp);
  appendP(req, fb_esp_pgm_str_21);
  appendP(req, fb_esp_pgm_str_8);
  appendP(req, fb_esp_pgm_str_129);
  appendP(req, fb_esp_pgm_str_21);
  appendP(req, fb_esp_pgm_str_21);

  req += s.c_str();
  if (_cfg->signer.wcs->send(req.c_str(), "") < 0)
    return handleSignerError(2);
  struct fb_esp_auth_token_error_t error;

  if (handleTokenResponse(_cfg->signer.wcs, _cfg->signer.json))
  {
    tmp = strP(fb_esp_pgm_str_257);
    _cfg->signer.json->get(*_cfg->signer.data, tmp);
    delS(tmp);

    if (_cfg->signer.data->success)
    {
      error.code = _cfg->signer.data->intValue;
      _cfg->signer.tokens.status = token_status_error;

      tmp = strP(fb_esp_pgm_str_258);
      _cfg->signer.json->get(*_cfg->signer.data, tmp);
      delS(tmp);
      if (_cfg->signer.data->success)
        error.message = _cfg->signer.data->stringValue.c_str();
    }

    _cfg->signer.tokens.error = error;

    if (error.code == 0)
    {
      if (_cfg->signer.tokens.token_type == token_type_id_token || _cfg->signer.tokens.token_type == token_type_custom_token)
      {
        tmp = strP(fb_esp_pgm_str_208);
        _cfg->signer.json->get(*_cfg->signer.data, tmp);
        delS(tmp);
        if (_cfg->signer.data->success)
          _cfg->signer.tokens.id_token = _cfg->signer.data->stringValue.c_str();

        tmp = strP(fb_esp_pgm_str_209);
        _cfg->signer.json->get(*_cfg->signer.data, tmp);
        delS(tmp);
        if (_cfg->signer.data->success)
          _cfg->signer.tokens.refresh_token = _cfg->signer.data->stringValue.c_str();

        tmp = strP(fb_esp_pgm_str_210);
        _cfg->signer.json->get(*_cfg->signer.data, tmp);
        delS(tmp);
        if (_cfg->signer.data->success)
          _cfg->signer.tokens.expires = millis() + (1000 * atoi(_cfg->signer.data->stringValue.c_str()));

        tmp = strP(fb_esp_pgm_str_175);
        _cfg->signer.json->get(*_cfg->signer.data, tmp);
        delS(tmp);
        if (_cfg->signer.data->success)
          _auth->token.uid = _cfg->signer.data->stringValue.c_str();
      }
      return handleSignerError(0);
    }

    return handleSignerError(4);
  }

  return handleSignerError(3);
}

struct token_info_t FirebaseESP32::authTokenInfo()
{
  token_info_t info;
  info.status = _cfg->signer.tokens.status;
  info.type = _cfg->signer.tokens.token_type;
  info.error = _cfg->signer.tokens.error;
  return info;
}

bool FirebaseESP32::requestTokens()
{

  _cfg->signer.attempts++;
  delay(0);

  if (_auth == nullptr)
    return false;

  if (_cfg->signer.tokens.status == token_status_on_request || _cfg->signer.tokens.status == token_status_on_refresh || time(nullptr) < default_ts || processing)
    return false;

  _cfg->signer.tokens.status = token_status_on_request;
  processing = true;
  setTokenError(FIREBASE_ERROR_TOKEN_NOT_READY);

  _cfg->signer.wcs = new FB_HTTPClient32();
  _cfg->signer.json = new FirebaseJson();
  _cfg->signer.data = new FirebaseJsonData();
  _cfg->signer.wcs->setCACert(nullptr);

  std::string host;
  appendP(host, fb_esp_pgm_str_193);
  appendP(host, fb_esp_pgm_str_4);
  appendP(host, fb_esp_pgm_str_120);
  _cfg->signer.wcs->begin(host.c_str(), 443);

  String s;
  std::string req;
  appendP(req, fb_esp_pgm_str_24);
  appendP(req, fb_esp_pgm_str_6);

  if (_cfg->signer.tokens.token_type == token_type_custom_token)
  {
    char *tmp = strP(fb_esp_pgm_str_233);
    _cfg->signer.json->add(tmp, _cfg->signer.tokens.jwt.c_str());
    delS(tmp);
    tmp = strP(fb_esp_pgm_str_198);
    _cfg->signer.json->add(tmp, true);
    delS(tmp);

    appendP(req, fb_esp_pgm_str_194);
    appendP(req, fb_esp_pgm_str_232);
    req += _cfg->api_key;
    appendP(req, fb_esp_pgm_str_30);
    appendP(req, fb_esp_pgm_str_31);
    appendP(req, fb_esp_pgm_str_193);
  }
  else if (_cfg->signer.tokens.token_type == token_type_oauth2_access_token)
  {
    char *tmp = strP(fb_esp_pgm_str_227);
    char *tmp2 = strP(fb_esp_pgm_str_228);
    _cfg->signer.json->add(tmp, (const char *)tmp2);
    delS(tmp);
    delS(tmp2);
    tmp = strP(fb_esp_pgm_str_229);
    _cfg->signer.json->add(tmp, _cfg->signer.tokens.jwt.c_str());
    delS(tmp);

    appendP(req, fb_esp_pgm_str_1);
    appendP(req, fb_esp_pgm_str_233);
    appendP(req, fb_esp_pgm_str_30);
    appendP(req, fb_esp_pgm_str_31);
    appendP(req, fb_esp_pgm_str_251);
  }

  appendP(req, fb_esp_pgm_str_4);
  appendP(req, fb_esp_pgm_str_120);

  appendP(req, fb_esp_pgm_str_21);
  appendP(req, fb_esp_pgm_str_32);
  appendP(req, fb_esp_pgm_str_12);
  _cfg->signer.json->toString(s);
  char *tmp = intStr(s.length());
  req += tmp;
  delS(tmp);
  appendP(req, fb_esp_pgm_str_21);
  appendP(req, fb_esp_pgm_str_8);
  appendP(req, fb_esp_pgm_str_129);
  appendP(req, fb_esp_pgm_str_21);
  appendP(req, fb_esp_pgm_str_21);

  req += s.c_str();
  if (_cfg->signer.wcs->send(req.c_str(), "") < 0)
    return handleSignerError(2);

  struct fb_esp_auth_token_error_t error;

  if (handleTokenResponse(_cfg->signer.wcs, _cfg->signer.json))
  {
    tmp = strP(fb_esp_pgm_str_257);
    _cfg->signer.json->get(*_cfg->signer.data, tmp);
    delS(tmp);
    if (_cfg->signer.data->success)
    {
      error.code = _cfg->signer.data->intValue;
      _cfg->signer.tokens.status = token_status_error;

      tmp = strP(fb_esp_pgm_str_258);
      _cfg->signer.json->get(*_cfg->signer.data, tmp);
      delS(tmp);
      if (_cfg->signer.data->success)
        error.message = _cfg->signer.data->stringValue.c_str();
    }

    _cfg->signer.tokens.error = error;

    if (error.code == 0)
    {
      if (_cfg->signer.tokens.token_type == token_type_custom_token)
      {
        tmp = strP(fb_esp_pgm_str_200);
        _cfg->signer.json->get(*_cfg->signer.data, tmp);
        delS(tmp);
        if (_cfg->signer.data->success)
          _cfg->signer.tokens.id_token = _cfg->signer.data->stringValue.c_str();

        tmp = strP(fb_esp_pgm_str_201);
        _cfg->signer.json->get(*_cfg->signer.data, tmp);
        delS(tmp);
        if (_cfg->signer.data->success)
          _cfg->signer.tokens.refresh_token = _cfg->signer.data->stringValue.c_str();

        tmp = strP(fb_esp_pgm_str_202);
        _cfg->signer.json->get(*_cfg->signer.data, tmp);
        delS(tmp);
        if (_cfg->signer.data->success)
          _cfg->signer.tokens.expires = millis() + (1000 * atoi(_cfg->signer.data->stringValue.c_str()));
      }
      else if (_cfg->signer.tokens.token_type == token_type_oauth2_access_token)
      {
        tmp = strP(fb_esp_pgm_str_235);
        _cfg->signer.json->get(*_cfg->signer.data, tmp);
        delS(tmp);
        if (_cfg->signer.data->success)
          _cfg->signer.tokens.access_token = _cfg->signer.data->stringValue.c_str();

        tmp = strP(fb_esp_pgm_str_236);
        _cfg->signer.json->get(*_cfg->signer.data, tmp);
        delS(tmp);
        if (_cfg->signer.data->success)
          _cfg->signer.tokens.auth_type = _cfg->signer.data->stringValue.c_str();

        tmp = strP(fb_esp_pgm_str_210);
        _cfg->signer.json->get(*_cfg->signer.data, tmp);
        delS(tmp);
        if (_cfg->signer.data->success)
          _cfg->signer.tokens.expires = millis() + (1000 * atoi(_cfg->signer.data->stringValue.c_str()));
      }
      return handleSignerError(0);
    }
    return handleSignerError(4);
  }

  return handleSignerError(3);
}

size_t FirebaseESP32::base64EncLen(size_t len)
{
  return ((len + 2) / 3 * 4) + 1;
}

void FirebaseESP32::encodeBase64Url(char *encoded, unsigned char *string, size_t len)
{
  size_t i;
  char *p = encoded;

  unsigned char *b64enc = new unsigned char[65];
  strcpy_P((char *)b64enc, (char *)fb_esp_base64_table);
  b64enc[62] = '-';
  b64enc[63] = '_';

  for (i = 0; i < len - 2; i += 3)
  {
    *p++ = b64enc[(string[i] >> 2) & 0x3F];
    *p++ = b64enc[((string[i] & 0x3) << 4) | ((int)(string[i + 1] & 0xF0) >> 4)];
    *p++ = b64enc[((string[i + 1] & 0xF) << 2) | ((int)(string[i + 2] & 0xC0) >> 6)];
    *p++ = b64enc[string[i + 2] & 0x3F];
  }

  if (i < len)
  {
    *p++ = b64enc[(string[i] >> 2) & 0x3F];
    if (i == (len - 1))
    {
      *p++ = b64enc[((string[i] & 0x3) << 4)];
    }
    else
    {
      *p++ = b64enc[((string[i] & 0x3) << 4) | ((int)(string[i + 1] & 0xF0) >> 4)];
      *p++ = b64enc[((string[i + 1] & 0xF) << 2)];
    }
  }

  *p++ = '\0';

  delete[] b64enc;
}

bool FirebaseESP32::createJWT(int step)
{
  if (time(nullptr) < default_ts)
    return false;

  setTokenError(FIREBASE_ERROR_TOKEN_NOT_READY);

  if (step == fb_esp_jwt_generation_step_encode_header_payload)
  {
    _cfg->signer.tokens.status = token_status_on_signing;

    _cfg->signer.json = new FirebaseJson();
    _cfg->signer.data = new FirebaseJsonData();

    unsigned long now = time(nullptr);

    _cfg->signer.tokens.jwt.clear();

    //header
    char *tmp = strP(fb_esp_pgm_str_239);
    char *tmp2 = strP(fb_esp_pgm_str_242);
    _cfg->signer.json->add(tmp, (const char *)tmp2);
    delS(tmp);
    delS(tmp2);
    tmp2 = strP(fb_esp_pgm_str_234);
    tmp = strP(fb_esp_pgm_str_240);
    _cfg->signer.json->add(tmp, (const char *)tmp2);
    delS(tmp);
    delS(tmp2);

    _cfg->signer.json->_tostr(_cfg->signer.header);
    size_t len = base64EncLen(_cfg->signer.header.length());
    char *buf = newS(len);
    encodeBase64Url(buf, (unsigned char *)_cfg->signer.header.c_str(), _cfg->signer.header.length());
    _cfg->signer.encHeader = buf;
    delS(buf);
    _cfg->signer.header.clear();
    _cfg->signer.encHeadPayload = _cfg->signer.encHeader;

    //payload
    _cfg->signer.json->clear();
    tmp = strP(fb_esp_pgm_str_212);
    _cfg->signer.json->add(tmp, _cfg->service_account.data.client_email.c_str());
    delS(tmp);
    tmp = strP(fb_esp_pgm_str_213);
    _cfg->signer.json->add(tmp, _cfg->service_account.data.client_email.c_str());
    delS(tmp);
    tmp = strP(fb_esp_pgm_str_214);
    std::string t;
    appendP(t, fb_esp_pgm_str_112);
    if (_cfg->signer.tokens.token_type == token_type_custom_token)
    {
      appendP(t, fb_esp_pgm_str_250);
      appendP(t, fb_esp_pgm_str_4);
      appendP(t, fb_esp_pgm_str_120);
      appendP(t, fb_esp_pgm_str_231);
    }
    else if (_cfg->signer.tokens.token_type == token_type_oauth2_access_token)
    {
      appendP(t, fb_esp_pgm_str_251);
      appendP(t, fb_esp_pgm_str_4);
      appendP(t, fb_esp_pgm_str_120);
      appendP(t, fb_esp_pgm_str_1);
      appendP(t, fb_esp_pgm_str_233);
    }

    _cfg->signer.json->add(tmp, t.c_str());
    delS(tmp);

    tmp = strP(fb_esp_pgm_str_218);
    _cfg->signer.json->add(tmp, (int)now);
    delS(tmp);

    tmp = strP(fb_esp_pgm_str_215);
    _cfg->signer.json->add(tmp, (int)(now + 60 * 60));
    delS(tmp);

    if (_cfg->signer.tokens.token_type == token_type_oauth2_access_token)
    {
      std::string buri;
      appendP(buri, fb_esp_pgm_str_112);
      appendP(buri, fb_esp_pgm_str_193);
      appendP(buri, fb_esp_pgm_str_4);
      appendP(buri, fb_esp_pgm_str_120);
      appendP(buri, fb_esp_pgm_str_1);
      appendP(buri, fb_esp_pgm_str_219);
      appendP(buri, fb_esp_pgm_str_1);

      std::string s = buri;

      appendP(s, fb_esp_pgm_str_221);
      appendP(s, fb_esp_pgm_str_6);
      s += buri;
      appendP(s, fb_esp_pgm_str_222);
      appendP(s, fb_esp_pgm_str_6);
      s += buri;

      appendP(s, fb_esp_pgm_str_223);
      appendP(s, fb_esp_pgm_str_6);
      s += buri;
      appendP(s, fb_esp_pgm_str_224);
      appendP(s, fb_esp_pgm_str_6);
      s += buri;
      appendP(s, fb_esp_pgm_str_225);

      tmp = strP(fb_esp_pgm_str_220);
      _cfg->signer.json->add(tmp, s.c_str());
      delS(tmp);
    }
    else if (_cfg->signer.tokens.token_type == token_type_custom_token)
    {
      tmp = strP(fb_esp_pgm_str_254);
      _cfg->signer.json->add(tmp, _auth->token.uid.c_str());
      delS(tmp);

      std::string s;
      _auth->token.claims._tostr(s);
      if (s.length() > 2)
      {
        tmp = strP(fb_esp_pgm_str_255);
        _cfg->signer.json->add(tmp, _auth->token.claims);
        delS(tmp);
      }
    }

    _cfg->signer.json->_tostr(_cfg->signer.payload);
    len = base64EncLen(_cfg->signer.payload.length());
    buf = newS(len);
    encodeBase64Url(buf, (unsigned char *)_cfg->signer.payload.c_str(), _cfg->signer.payload.length());
    _cfg->signer.encPayload = buf;
    delS(buf);
    _cfg->signer.payload.clear();

    appendP(_cfg->signer.encHeadPayload, fb_esp_pgm_str_4);
    _cfg->signer.encHeadPayload += _cfg->signer.encPayload;

    std::string().swap(_cfg->signer.encHeader);
    std::string().swap(_cfg->signer.encPayload);

    //create message digest from encoded header and payload
    _cfg->signer.hash = new uint8_t[_cfg->signer.hashSize];
    int ret = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (const unsigned char *)_cfg->signer.encHeadPayload.c_str(), _cfg->signer.encHeadPayload.length(), _cfg->signer.hash);
    if (ret != 0)
    {
      char *tmp = newS(100);
      mbedtls_strerror(ret, tmp, 100);
      _cfg->signer.tokens.error.message = tmp;
      delS(tmp);
      setTokenError(FIREBASE_ERROR_TOKEN_CREATE_HASH);
      delete[] _cfg->signer.hash;
      return false;
    }

    _cfg->signer.tokens.jwt = _cfg->signer.encHeadPayload;
    appendP(_cfg->signer.tokens.jwt, fb_esp_pgm_str_4);
    std::string().swap(_cfg->signer.encHeadPayload);

    delete _cfg->signer.json;
    delete _cfg->signer.data;
  }
  else if (step == fb_esp_jwt_generation_step_sign)
  {

    _cfg->signer.tokens.status = token_status_on_signing;

    _cfg->signer.pk_ctx = new mbedtls_pk_context();
    mbedtls_pk_init(_cfg->signer.pk_ctx);

    //parse priv key
    int ret = 0;
    if (_cfg->signer.pk.length() > 0)
      ret = mbedtls_pk_parse_key(_cfg->signer.pk_ctx, (const unsigned char *)_cfg->signer.pk.c_str(), _cfg->signer.pk.length() + 1, NULL, 0);
    else if (strlen_P(_cfg->service_account.data.private_key) > 0)
      ret = mbedtls_pk_parse_key(_cfg->signer.pk_ctx, (const unsigned char *)_cfg->service_account.data.private_key, strlen_P(_cfg->service_account.data.private_key) + 1, NULL, 0);

    if (ret != 0)
    {
      char *tmp = newS(100);
      mbedtls_strerror(ret, tmp, 100);
      _cfg->signer.tokens.error.message = tmp;
      delS(tmp);
      setTokenError(FIREBASE_ERROR_TOKEN_PARSE_PK);
      mbedtls_pk_free(_cfg->signer.pk_ctx);
      delete[] _cfg->signer.hash;
      delete _cfg->signer.pk_ctx;
      return false;
    }

    //generate RSA signature from private key and message digest
    _cfg->signer.signature = new unsigned char[_cfg->signer.signatureSize];
    size_t retSize = 0;
    _cfg->signer.entropy_ctx = new mbedtls_entropy_context();
    _cfg->signer.ctr_drbg_ctx = new mbedtls_ctr_drbg_context();
    mbedtls_entropy_init(_cfg->signer.entropy_ctx);
    mbedtls_ctr_drbg_init(_cfg->signer.ctr_drbg_ctx);
    mbedtls_ctr_drbg_seed(_cfg->signer.ctr_drbg_ctx, mbedtls_entropy_func, _cfg->signer.entropy_ctx, NULL, 0);

    ret = mbedtls_pk_sign(_cfg->signer.pk_ctx, MBEDTLS_MD_SHA256, (const unsigned char *)_cfg->signer.hash, sizeof(_cfg->signer.hash), _cfg->signer.signature, &retSize, mbedtls_ctr_drbg_random, _cfg->signer.ctr_drbg_ctx);
    if (ret != 0)
    {
      char *tmp = newS(100);
      mbedtls_strerror(ret, tmp, 100);
      _cfg->signer.tokens.error.message = tmp;
      delS(tmp);
      setTokenError(FIREBASE_ERROR_TOKEN_SIGN);
    }
    else
    {
      _cfg->signer.encSignature.clear();
      size_t len = base64EncLen(_cfg->signer.signatureSize);
      char *buf = newS(len);
      encodeBase64Url(buf, _cfg->signer.signature, _cfg->signer.signatureSize);
      _cfg->signer.encSignature = buf;
      delS(buf);

      _cfg->signer.tokens.jwt += _cfg->signer.encSignature;
      std::string().swap(_cfg->signer.pk);
      std::string().swap(_cfg->signer.encSignature);
    }

    delete[] _cfg->signer.signature;
    delete[] _cfg->signer.hash;
    mbedtls_pk_free(_cfg->signer.pk_ctx);
    mbedtls_entropy_free(_cfg->signer.entropy_ctx);
    mbedtls_ctr_drbg_free(_cfg->signer.ctr_drbg_ctx);
    delete _cfg->signer.pk_ctx;
    delete _cfg->signer.entropy_ctx;
    delete _cfg->signer.ctr_drbg_ctx;

    if (ret != 0)
      return false;
  }

  _cfg->signer.step++;
  return true;
}

void FirebaseESP32::setTokenError(int code)
{
  if (code != 0)
    _cfg->signer.tokens.status = token_status_error;
  else
  {
    _cfg->signer.tokens.error.message.clear();
    _cfg->signer.tokens.status = token_status_ready;
  }

  _cfg->signer.tokens.error.code = code;

  if (_cfg->signer.tokens.error.message.length() == 0)
  {
    switch (code)
    {

    case FIREBASE_ERROR_TOKEN_SET_TIME:
      appendP(_cfg->signer.tokens.error.message, fb_esp_pgm_str_211, true);
      break;
    case FIREBASE_ERROR_TOKEN_CREATE_HASH:

      break;
    case FIREBASE_ERROR_TOKEN_PARSE_PK:
      //appendP(_cfg->signer.tokens.error.message, fb_esp_pgm_str_179, true);
      break;
    case FIREBASE_ERROR_TOKEN_SIGN:
      //appendP(_cfg->signer.tokens.error.message, fb_esp_pgm_str_178, true);
      break;
    case FIREBASE_ERROR_TOKEN_EXCHANGE:
      appendP(_cfg->signer.tokens.error.message, fb_esp_pgm_str_177, true);
      break;
    case FIREBASE_ERROR_TOKEN_NOT_READY:
      appendP(_cfg->signer.tokens.error.message, fb_esp_pgm_str_252, true);
      break;
    case FIREBASE_ERROR_HTTPC_ERROR_NOT_CONNECTED:
      appendP(_cfg->signer.tokens.error.message, fb_esp_pgm_str_42);
      break;
    case FIREBASE_ERROR_HTTPC_ERROR_CONNECTION_LOST:
      appendP(_cfg->signer.tokens.error.message, fb_esp_pgm_str_43);
      break;
    case FIREBASE_ERROR_HTTP_CODE_REQUEST_TIMEOUT:
      appendP(_cfg->signer.tokens.error.message, fb_esp_pgm_str_58);
      break;

    default:
      break;
    }
  }
}

bool FirebaseESP32::handleTokenResponse(FB_HTTPClient32 *wcs, FirebaseJson *json)
{
  struct server_response_data_t response;
  int chunkIdx = 0;
  int pChunkIdx = 0;
  int payloadLen = 3200;
  int pBufPos = 0;
  int hBufPos = 0;
  int chunkBufSize = 0;
  int hstate = 0;
  int pstate = 0;
  int chunkedDataState = 0;
  int chunkedDataSize = 0;
  int chunkedDataLen = 0;
  int defaultChunkSize = 2048;
  char *header = nullptr;
  char *payload = nullptr;
  char *pChunk = nullptr;
  char *tmp = nullptr;
  bool isHeader = false;

  WiFiClient *stream = wcs->stream();

  while (stream->connected() && stream->available() == 0)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      stream->stop();
      return false;
    }
    delay(0);
  }

  bool complete = false;
  int readLen = 0;
  unsigned long datatime = millis();

  while (!complete)
  {

    chunkBufSize = stream->available();

    if (chunkBufSize > 1 || !complete)
    {
      while (!complete)
      {

        if (WiFi.status() != WL_CONNECTED)
        {
          stream->stop();
          return false;
        }

        chunkBufSize = stream->available();

        if (chunkBufSize > 0)
        {
          chunkBufSize = defaultChunkSize;

          if (chunkIdx == 0)
          {
            header = newS(chunkBufSize);
            hstate = 1;
            int readLen = readLine(stream, header, chunkBufSize);
            int pos = 0;

            tmp = getHeader(header, fb_esp_pgm_str_5, fb_esp_pgm_str_6, pos, 0);
            delay(0);
            if (tmp)
            {
              isHeader = true;
              hBufPos = readLen;
              response.httpCode = atoi(tmp);
              delS(tmp);
            }
          }
          else
          {
            delay(0);
            if (isHeader)
            {
              tmp = newS(chunkBufSize);
              int readLen = readLine(stream, tmp, chunkBufSize);
              bool headerEnded = false;

              if (readLen == 1)
                if (tmp[0] == '\r')
                  headerEnded = true;

              if (readLen == 2)
                if (tmp[0] == '\r' && tmp[1] == '\n')
                  headerEnded = true;

              if (headerEnded)
              {
                isHeader = false;
                parseRespHeader(header, response);
                if (hstate == 1)
                  delS(header);
                hstate = 0;
              }
              else
              {
                memcpy(header + hBufPos, tmp, readLen);
                hBufPos += readLen;
              }

              delS(tmp);
            }
            else
            {
              if (!response.noContent)
              {
                pChunkIdx++;

                pChunk = newS(chunkBufSize + 1);

                if (!payload || pstate == 0)
                {
                  pstate = 1;
                  payload = newS(payloadLen + 1);
                }
                readLen = 0;
                if (response.isChunkedEnc)
                  readLen = readChunkedData(stream, pChunk, chunkedDataState, chunkedDataSize, chunkedDataLen, chunkBufSize);
                else
                  readLen = stream->readBytes(pChunk, chunkBufSize);

                if (readLen > -1)
                  memcpy(payload + pBufPos, pChunk, readLen);

                delS(pChunk);
                pBufPos += readLen;
                if ((response.isChunkedEnc && readLen < 0) || (!response.isChunkedEnc && stream->available() <= 0))
                  complete = true;
              }
              else
              {
                while (stream->available() > 0)
                  stream->read();
                if (stream->available() <= 0)
                  break;
              }
            }
          }
          chunkIdx++;
        }
      }

      if (millis() - datatime > 5000)
        complete = true;
    }
  }

  if (hstate == 1)
    delS(header);

  if (stream->connected())
    stream->stop();

  if (payload && !response.noContent)
  {
    json->setJsonData(payload);
    delS(payload);
    return true;
  }

  return false;
}

void FirebaseESP32::reconnectWiFi(bool reconnect)
{
  _reconnectWiFi = reconnect;
  WiFi.setAutoReconnect(reconnect);
}

void FirebaseESP32::setFloatDigits(uint8_t digits)
{
  if (digits < 7)
    _floatDigits = digits;
}

void FirebaseESP32::setDoubleDigits(uint8_t digits)
{
  if (digits < 9)
    _doubleDigits = digits;
}

void FirebaseESP32::setReadTimeout(FirebaseData &fbdo, int millisec)
{
  if (millisec <= 900000)
    fbdo._readTimeout = millisec;
}

void FirebaseESP32::setwriteSizeLimit(FirebaseData &fbdo, const String &size)
{
  fbdo._writeLimit = size.c_str();
}

bool FirebaseESP32::getRules(FirebaseData &fbdo)
{
  fbdo.queryFilter.clear();
  std::string path;
  appendP(path, fb_esp_pgm_str_103, true);
  bool ret = handleRequest(fbdo, 0, path, fb_esp_method::m_read_rules, fb_esp_data_type::d_json, "", "", "");
  std::string().swap(path);
  return ret;
}

bool FirebaseESP32::setRules(FirebaseData &fbdo, const String &rules)
{
  fbdo.queryFilter.clear();
  std::string path;
  appendP(path, fb_esp_pgm_str_103, true);
  bool ret = handleRequest(fbdo, 0, path, fb_esp_method::m_set_rules, fb_esp_data_type::d_json, rules.c_str(), "", "");
  std::string().swap(path);
  return ret;
}

void FirebaseESP32::getUrlInfo(const std::string url, struct fb_esp_url_info_t &info)
{
  size_t p1;
  size_t p2;
  int scheme = 0;
  char *tmp = nullptr;
  std::string _h;
  tmp = strP(fb_esp_pgm_str_111);
  p1 = url.find(tmp, 0);
  delS(tmp);
  if (p1 == std::string::npos)
  {
    tmp = strP(fb_esp_pgm_str_112);
    p1 = url.find(tmp, 0);
    delS(tmp);
    if (p1 != std::string::npos)
      scheme = 2;
  }
  else
    scheme = 1;

  if (scheme == 1)
    p1 += strlen_P(fb_esp_pgm_str_111);
  else if (scheme == 2)
    p1 += strlen_P(fb_esp_pgm_str_112);
  else
    p1 = 0;

  if (p1 + 3 < url.length())
    if (url[p1] == 'w' && url[p1 + 1] == 'w' && url[p1 + 2] == 'w' && url[p1 + 3] == '.')
      p1 += 4;

  tmp = strP(fb_esp_pgm_str_1);
  p2 = url.find(tmp, p1 + 1);
  delS(tmp);
  if (p2 == std::string::npos)
  {
    tmp = strP(fb_esp_pgm_str_173);
    p2 = url.find(tmp, p1 + 1);
    delS(tmp);
    if (p2 == std::string::npos)
    {
      tmp = strP(fb_esp_pgm_str_174);
      p2 = url.find(tmp, p1 + 1);
      delS(tmp);
      if (p2 == std::string::npos)
        p2 = url.length();
    }
  }

  _h = url.substr(p1, p2 - p1);

  if (_h[0] == '/')
    info.uri = _h;
  else
    info.host = _h;

  if (_cfg->signer.tokens.token_type == token_type_legacy_token || _cfg->signer.tokens.token_type == token_type_id_token)
  {
    //get uri
    if (p2 != _h.length())
    {
      info.uri = url.substr(p2, url.length() - p2);
      tmp = strP(fb_esp_pgm_str_170);
      p1 = _h.find(tmp, url.length());
      delS(tmp);

      if (p1 == std::string::npos)
      {
        tmp = strP(fb_esp_pgm_str_171);
        p1 = _h.find(tmp, url.length());
        delS(tmp);
      }

      if (p1 != std::string::npos)
      {
        p1 += 6;
        tmp = strP(fb_esp_pgm_str_172);
        p2 = _h.find(tmp, p1);
        delS(tmp);
        if (p2 == std::string::npos)
          p2 = _h.length();
        info.auth = _h.substr(p1, p2 - p1);
      }
    }
  }
}

bool FirebaseESP32::processRequest(FirebaseData &fbdo, fb_esp_method method, fb_esp_data_type dataType, const std::string &path, const char *buff, bool queue, const std::string &priority, const std::string &etag)
{

  if (!reconnect(fbdo))
    return false;

  if (!tokenReady(fbdo))
    return false;

  bool ret = false;
  fbdo.queryFilter.clear();

  uint8_t errCount = 0;
  uint8_t maxAttempt = fbdo._maxAttempt;
  if (maxAttempt == 0)
    maxAttempt = 1;

  for (int i = 0; i < maxAttempt; i++)
  {
    ret = handleRequest(fbdo, 0, path.c_str(), method, dataType, buff, priority, etag);
    if (ret)
      break;

    if (fbdo._maxAttempt > 0)
      if (!ret && connectionError(fbdo))
        errCount++;
  }

  fbdo._qID = 0;

  if (!queue && !ret && errCount == maxAttempt && fbdo._qMan._maxQueue > 0)
    if (method == fb_esp_method::m_put || method == fb_esp_method::m_put_nocontent || method == fb_esp_method::m_post || method == fb_esp_method::m_patch || method == fb_esp_method::m_patch_nocontent)
      fbdo.addQueue(method, 0, dataType, path.c_str(), "", buff, false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

  return ret;
}

bool FirebaseESP32::processRequestFile(FirebaseData &fbdo, uint8_t storageType, fb_esp_method method, const std::string &path, const std::string &fileName, bool queue, const std::string &priority, const std::string &etag)
{

  if (!reconnect(fbdo))
    return false;

  if (!tokenReady(fbdo))
    return false;

  fbdo.queryFilter.clear();
  fbdo._fileName = fileName.c_str();

  bool ret = false;

  uint8_t errCount = 0;
  uint8_t maxAttempt = fbdo._maxAttempt;
  if (maxAttempt == 0)
    maxAttempt = 1;

  for (int i = 0; i < maxAttempt; i++)
  {

    ret = handleRequest(fbdo, storageType, path.c_str(), method, fb_esp_data_type::d_file, "", priority, etag);
    if (ret)
      break;

    if (fbdo._maxAttempt > 0)
      if (!ret && connectionError(fbdo))
        errCount++;
  }

  fbdo._qID = 0;

  if (!queue && !ret && errCount == maxAttempt && fbdo._qMan._maxQueue > 0)
    fbdo.addQueue(method, storageType, fb_esp_data_type::d_file, path.c_str(), fileName.c_str(), "", false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

  return ret;
}

bool FirebaseESP32::pathExist(FirebaseData &fbdo, const String &path)
{
  fbdo.queryFilter.clear();
  if (handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_get_nocontent, fb_esp_data_type::d_string, "", "", ""))
    return !fbdo._pathNotExist;
  else
    return false;
}

String FirebaseESP32::getETag(FirebaseData &fbdo, const String &path)
{
  fbdo.queryFilter.clear();
  if (handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_get_nocontent, fb_esp_data_type::d_string, "", "", ""))
    return fbdo._resp_etag.c_str();
  else
    return String();
}

bool FirebaseESP32::getShallowData(FirebaseData &fbdo, const String &path)
{
  bool ret = false;
  ret = handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_get_shallow, fb_esp_data_type::d_string, "", "", "");
  return ret;
}

void FirebaseESP32::enableClassicRequest(FirebaseData &fbdo, bool ret)
{
  fbdo._classicRequest = ret;
}

bool FirebaseESP32::setPriority(FirebaseData &fbdo, const String &path, float priority)
{
  char *num = floatStr(priority);
  trimDigits(num);
  std::string _path = path.c_str();
  appendP(_path, fb_esp_pgm_str_156);
  bool ret = processRequest(fbdo, fb_esp_method::m_set_priority, fb_esp_data_type::d_float, _path.c_str(), num, false, "", "");
  delS(num);
  return ret;
}

bool FirebaseESP32::getPriority(FirebaseData &fbdo, const String &path)
{
  char *tmp = strP(fb_esp_pgm_str_156);
  bool ret = processRequest(fbdo, fb_esp_method::m_get_priority, fb_esp_data_type::d_float, tmp, "", false, "", "");
  delS(tmp);
  return ret;
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, int intValue)
{
  return pushInt(fbdo, path, intValue);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, int intValue, float priority)
{
  return pushInt(fbdo, path, intValue, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, float floatValue)
{
  return pushFloat(fbdo, path, floatValue);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, float floatValue, float priority)
{
  return pushFloat(fbdo, path, floatValue, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, double doubleValue)
{
  return pushDouble(fbdo, path, doubleValue);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, double doubleValue, float priority)
{
  return pushDouble(fbdo, path, doubleValue, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, bool boolValue)
{
  return pushBool(fbdo, path, boolValue);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, bool boolValue, float priority)
{
  return pushBool(fbdo, path, boolValue, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, const char *stringValue)
{
  return pushString(fbdo, path, stringValue);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, const String &stringValue)
{
  return pushString(fbdo, path, stringValue);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, const StringSumHelper &stringValue)
{
  return pushString(fbdo, path, stringValue);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, const char *stringValue, float priority)
{
  return pushString(fbdo, path, stringValue, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, const String &stringValue, float priority)
{
  return pushString(fbdo, path, stringValue, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, const StringSumHelper &stringValue, float priority)
{
  return pushString(fbdo, path, stringValue, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, FirebaseJson &json)
{
  return pushJSON(fbdo, path, json);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
  return pushJSON(fbdo, path, json, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr)
{
  return pushArray(fbdo, path, arr);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority)
{
  return pushArray(fbdo, path, arr, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size)
{
  return pushBlob(fbdo, path, blob, size);
}

bool FirebaseESP32::push(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority)
{
  return pushBlob(fbdo, path, blob, size, priority);
}

bool FirebaseESP32::push(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName)
{
  return pushFile(fbdo, storageType, path, fileName);
}

bool FirebaseESP32::push(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority)
{
  return pushFile(fbdo, storageType, path, fileName, priority);
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
  else if (std::is_same<T, const StringSumHelper &>::value)
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
    return pushBlob(fbdo, path, value, size);
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
  else if (std::is_same<T, const StringSumHelper &>::value)
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
    return pushBlob(fbdo, path, value, size, priority);
}

bool FirebaseESP32::pushInt(FirebaseData &fbdo, const String &path, int intValue)
{
  return pushInt(fbdo, path.c_str(), intValue, false, "");
}

bool FirebaseESP32::pushInt(FirebaseData &fbdo, const String &path, int intValue, float priority)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  bool ret = pushInt(fbdo, path.c_str(), intValue, false, "");
  delS(buf);
  return ret;
}

bool FirebaseESP32::pushInt(FirebaseData &fbdo, const std::string &path, int intValue, bool queue, const std::string &priority)
{
  char *buf = intStr(intValue);
  bool ret = processRequest(fbdo, fb_esp_method::m_post, fb_esp_data_type::d_integer, path, buf, queue, priority, "");
  delS(buf);
  return ret;
}

bool FirebaseESP32::pushFloat(FirebaseData &fbdo, const String &path, float floatValue)
{
  return pushFloat(fbdo, path.c_str(), floatValue, false, "");
}

bool FirebaseESP32::pushFloat(FirebaseData &fbdo, const String &path, float floatValue, float priority)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  bool ret = pushFloat(fbdo, path.c_str(), floatValue, false, buf);
  delS(buf);
  return ret;
}

bool FirebaseESP32::pushFloat(FirebaseData &fbdo, const std::string &path, float floatValue, bool queue, const std::string &priority)
{
  char *buf = floatStr(floatValue);
  trimDigits(buf);
  bool ret = processRequest(fbdo, fb_esp_method::m_post, fb_esp_data_type::d_float, path, buf, queue, priority, "");
  delS(buf);
  return ret;
}

bool FirebaseESP32::pushDouble(FirebaseData &fbdo, const String &path, double doubleValue)
{
  return pushDouble(fbdo, path.c_str(), doubleValue, false, "");
}

bool FirebaseESP32::pushDouble(FirebaseData &fbdo, const String &path, double doubleValue, float priority)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  bool ret = pushDouble(fbdo, path.c_str(), doubleValue, false, buf);
  delS(buf);
  return ret;
}

bool FirebaseESP32::pushDouble(FirebaseData &fbdo, const std::string &path, double doubleValue, bool queue, const std::string &priority)
{
  char *buf = doubleStr(doubleValue);
  trimDigits(buf);
  bool ret = processRequest(fbdo, fb_esp_method::m_post, fb_esp_data_type::d_double, path, buf, queue, priority, "");
  delS(buf);
  return ret;
}

bool FirebaseESP32::pushBool(FirebaseData &fbdo, const String &path, bool boolValue)
{
  return pushBool(fbdo, path.c_str(), boolValue, false, "");
}

bool FirebaseESP32::pushBool(FirebaseData &fbdo, const String &path, bool boolValue, float priority)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  bool ret = pushBool(fbdo, path.c_str(), boolValue, false, buf);
  delS(buf);
  return ret;
}

bool FirebaseESP32::pushBool(FirebaseData &fbdo, const std::string &path, bool boolValue, bool queue, const std::string &priority)
{
  char *tmp = boolStr(boolValue);
  bool ret = processRequest(fbdo, fb_esp_method::m_post, fb_esp_data_type::d_boolean, path, tmp, queue, priority, "");
  delS(tmp);
  return ret;
}

bool FirebaseESP32::pushString(FirebaseData &fbdo, const String &path, const String &stringValue)
{
  bool ret = processRequest(fbdo, fb_esp_method::m_post, fb_esp_data_type::d_string, path.c_str(), stringValue.c_str(), false, "", "");
  return ret;
}

bool FirebaseESP32::pushString(FirebaseData &fbdo, const String &path, const String &stringValue, float priority)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  bool ret = processRequest(fbdo, fb_esp_method::m_post, fb_esp_data_type::d_string, path.c_str(), stringValue.c_str(), false, buf, "");
  delS(buf);
  return ret;
}

bool FirebaseESP32::pushJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json)
{
  std::string s;
  json._toStdString(s);
  bool ret = processRequest(fbdo, fb_esp_method::m_post, fb_esp_data_type::d_json, path.c_str(), s.c_str(), false, "", "");
  std::string().swap(s);
  return ret;
}

bool FirebaseESP32::pushJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  std::string s;
  json._toStdString(s);
  bool ret = processRequest(fbdo, fb_esp_method::m_post, fb_esp_data_type::d_json, path.c_str(), s.c_str(), false, buf, "");
  std::string().swap(s);
  delS(buf);
  return ret;
}

bool FirebaseESP32::pushArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr)
{
  std::string s;
  arr._toStdString(s);
  bool ret = processRequest(fbdo, fb_esp_method::m_post, fb_esp_data_type::d_array, path.c_str(), s.c_str(), false, "", "");
  std::string().swap(s);
  return ret;
}

bool FirebaseESP32::pushArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  std::string s;
  arr._toStdString(s);
  bool ret = processRequest(fbdo, fb_esp_method::m_post, fb_esp_data_type::d_array, path.c_str(), s.c_str(), false, buf, "");
  std::string().swap(s);
  delS(buf);
  return ret;
}

bool FirebaseESP32::pushBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size)
{
  return pushBlob(fbdo, path.c_str(), blob, size, false, "");
}

bool FirebaseESP32::pushBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  bool ret = pushBlob(fbdo, path.c_str(), blob, size, false, buf);
  delS(buf);
  return ret;
}

bool FirebaseESP32::pushBlob(FirebaseData &fbdo, const std::string &path, uint8_t *blob, size_t size, bool queue, const std::string &priority)
{
  if (fbdo._maxBlobSize < size)
  {
    fbdo._httpCode = FIREBASE_ERROR_HTTP_CODE_PAYLOAD_TOO_LARGE;
    return false;
  }

  std::string base64Str;
  appendP(base64Str, fb_esp_pgm_str_92, true);
  base64Str += encodeBase64Str(blob, size);
  appendP(base64Str, fb_esp_pgm_str_3);
  bool ret = processRequest(fbdo, fb_esp_method::m_post, fb_esp_data_type::d_blob, path, base64Str.c_str(), queue, priority, "");
  std::string().swap(base64Str);
  return ret;
}

bool FirebaseESP32::pushFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName)
{
  return processRequestFile(fbdo, storageType, fb_esp_method::m_post, path.c_str(), fileName.c_str(), false, "", "");
}

bool FirebaseESP32::pushFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  bool ret = processRequestFile(fbdo, storageType, fb_esp_method::m_post, path.c_str(), fileName.c_str(), false, buf, "");
  delS(buf);
  return ret;
}

bool FirebaseESP32::pushTimestamp(FirebaseData &fbdo, const String &path)
{
  char *tmp = strP(fb_esp_pgm_str_154);
  bool ret = processRequest(fbdo, fb_esp_method::m_post, fb_esp_data_type::d_timestamp, path.c_str(), tmp, false, "", "");
  delS(tmp);
  return ret;
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, int intValue)
{
  return setInt(fbdo, path, intValue);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, int intValue, float priority)
{
  return setInt(fbdo, path, intValue, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, int intValue, const String &ETag)
{
  return setInt(fbdo, path, intValue, ETag);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, int intValue, float priority, const String &ETag)
{
  return setInt(fbdo, path, intValue, priority, ETag);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, float floatValue)
{
  return setFloat(fbdo, path, floatValue);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, float floatValue, float priority)
{
  return setFloat(fbdo, path, floatValue, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, float floatValue, const String &ETag)
{
  return setFloat(fbdo, path, floatValue, ETag);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, float floatValue, float priority, const String &ETag)
{
  return setFloat(fbdo, path, floatValue, priority, ETag);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, double doubleValue)
{
  return setDouble(fbdo, path, doubleValue);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, double doubleValue, float priority)
{
  return setDouble(fbdo, path, doubleValue, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, double doubleValue, const String &ETag)
{
  return setDouble(fbdo, path, doubleValue, ETag);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, double doubleValue, float priority, const String &ETag)
{
  return setDouble(fbdo, path, doubleValue, priority, ETag);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, bool boolValue)
{
  return setBool(fbdo, path, boolValue);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, bool boolValue, float priority)
{
  return setBool(fbdo, path, boolValue, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, bool boolValue, const String &ETag)
{
  return setBool(fbdo, path, boolValue, ETag);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, bool boolValue, float priority, const String &ETag)
{
  return setBool(fbdo, path, boolValue, priority, ETag);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const char *stringValue)
{
  return setString(fbdo, path, stringValue);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const String &stringValue)
{
  return setString(fbdo, path, stringValue);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const StringSumHelper &stringValue)
{
  return setString(fbdo, path, stringValue);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const char *stringValue, float priority)
{
  return setString(fbdo, path, stringValue, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const String &stringValue, float priority)
{
  return setString(fbdo, path, stringValue, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const StringSumHelper &stringValue, float priority)
{
  return setString(fbdo, path, stringValue, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const char *stringValue, const String &ETag)
{
  return setString(fbdo, path, stringValue, ETag);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const String &stringValue, const String &ETag)
{
  return setString(fbdo, path, stringValue, ETag);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const StringSumHelper &stringValue, const String &ETag)
{
  return setString(fbdo, path, stringValue, ETag);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const char *stringValue, float priority, const String &ETag)
{
  return setString(fbdo, path, stringValue, priority, ETag);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const String &stringValue, float priority, const String &ETag)
{
  return setString(fbdo, path, stringValue, priority, ETag);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, const StringSumHelper &stringValue, float priority, const String &ETag)
{
  return setString(fbdo, path, stringValue, priority, ETag);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJson &json)
{
  return setJSON(fbdo, path, json);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr)
{
  return setArray(fbdo, path, arr);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
  return setJSON(fbdo, path, json, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority)
{
  return setArray(fbdo, path, arr, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJson &json, const String &ETag)
{
  return setJSON(fbdo, path, json, ETag);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, const String &ETag)
{
  return setArray(fbdo, path, arr, ETag);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority, const String &ETag)
{
  return setJSON(fbdo, path, json, priority, ETag);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority, const String &ETag)
{
  return setArray(fbdo, path, arr, priority, ETag);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size)
{
  return setBlob(fbdo, path, blob, size);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority)
{
  return setBlob(fbdo, path, blob, size, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, const String &ETag)
{
  return setBlob(fbdo, path, blob, size, ETag);
}

bool FirebaseESP32::set(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority, const String &ETag)
{
  return setBlob(fbdo, path, blob, size, priority, ETag);
}

bool FirebaseESP32::set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName)
{
  return setFile(fbdo, storageType, path, fileName);
}

bool FirebaseESP32::set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority)
{
  return setFile(fbdo, storageType, path, fileName, priority);
}

bool FirebaseESP32::set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, const String &ETag)
{
  return setFile(fbdo, storageType, path, fileName, ETag);
}

bool FirebaseESP32::set(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority, const String &ETag)
{
  return setFile(fbdo, storageType, path, fileName, priority, ETag);
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
  else if (std::is_same<T, const StringSumHelper &>::value)
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
    return setBlob(fbdo, path, value, size);
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
  else if (std::is_same<T, const StringSumHelper &>::value)
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
    return setBlob(fbdo, path, value, size, priority);
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
  else if (std::is_same<T, const StringSumHelper &>::value)
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
    return setBlob(fbdo, path, value, size, ETag);
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
  else if (std::is_same<T, const StringSumHelper &>::value)
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
    return setBlob(fbdo, path, value, size, priority, ETag);
}

bool FirebaseESP32::setInt(FirebaseData &fbdo, const String &path, int intValue)
{
  return setInt(fbdo, path.c_str(), intValue, false, "", "");
}

bool FirebaseESP32::setInt(FirebaseData &fbdo, const String &path, int intValue, float priority)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  bool ret = setInt(fbdo, path.c_str(), intValue, false, buf, "");
  delS(buf);
  return ret;
}

bool FirebaseESP32::setInt(FirebaseData &fbdo, const String &path, int intValue, const String &ETag)
{
  return setInt(fbdo, path.c_str(), intValue, false, "", ETag.c_str());
}

bool FirebaseESP32::setInt(FirebaseData &fbdo, const String &path, int intValue, float priority, const String &ETag)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  bool ret = setInt(fbdo, path.c_str(), intValue, false, buf, ETag.c_str());
  delS(buf);
  return ret;
}

bool FirebaseESP32::setInt(FirebaseData &fbdo, const std::string &path, int intValue, bool queue, const std::string &priority, const std::string &etag)
{
  char *buf = intStr(intValue);
  bool ret = processRequest(fbdo, fb_esp_method::m_put, fb_esp_data_type::d_integer, path, buf, queue, priority, etag);
  delS(buf);
  return ret;
}

bool FirebaseESP32::setFloat(FirebaseData &fbdo, const String &path, float floatValue)
{
  return setFloat(fbdo, path.c_str(), floatValue, false, "", "");
}

bool FirebaseESP32::setFloat(FirebaseData &fbdo, const String &path, float floatValue, float priority)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  bool ret = setFloat(fbdo, path.c_str(), floatValue, false, buf, "");
  delS(buf);
  return ret;
}

bool FirebaseESP32::setFloat(FirebaseData &fbdo, const String &path, float floatValue, const String &ETag)
{
  return setFloat(fbdo, path.c_str(), floatValue, false, "", ETag.c_str());
}

bool FirebaseESP32::setFloat(FirebaseData &fbdo, const String &path, float floatValue, float priority, const String &ETag)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  bool ret = setFloat(fbdo, path.c_str(), floatValue, false, buf, ETag.c_str());
  delS(buf);
  return ret;
}

bool FirebaseESP32::setFloat(FirebaseData &fbdo, const std::string &path, float floatValue, bool queue, const std::string &priority, const std::string &etag)
{
  char *buf = floatStr(floatValue);
  trimDigits(buf);
  bool ret = processRequest(fbdo, fb_esp_method::m_put, fb_esp_data_type::d_float, path, buf, queue, priority, etag);
  delS(buf);
  return ret;
}

bool FirebaseESP32::setDouble(FirebaseData &fbdo, const String &path, double doubleValue)
{
  return setDouble(fbdo, path.c_str(), doubleValue, false, "", "");
}

bool FirebaseESP32::setDouble(FirebaseData &fbdo, const String &path, double doubleValue, float priority)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  bool ret = setDouble(fbdo, path.c_str(), doubleValue, false, buf, "");
  delS(buf);
  return ret;
}

bool FirebaseESP32::setDouble(FirebaseData &fbdo, const String &path, double doubleValue, const String &ETag)
{
  return setDouble(fbdo, path.c_str(), doubleValue, false, "", ETag.c_str());
}

bool FirebaseESP32::setDouble(FirebaseData &fbdo, const String &path, double doubleValue, float priority, const String &ETag)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  bool ret = setDouble(fbdo, path.c_str(), doubleValue, false, buf, ETag.c_str());
  delS(buf);
  return ret;
}

bool FirebaseESP32::setDouble(FirebaseData &fbdo, const std::string &path, double doubleValue, bool queue, const std::string &priority, const std::string &etag)
{
  char *buf = doubleStr(doubleValue);
  trimDigits(buf);
  bool ret = processRequest(fbdo, fb_esp_method::m_put, fb_esp_data_type::d_double, path, buf, false, priority, etag);
  delS(buf);
  return ret;
}

bool FirebaseESP32::setBool(FirebaseData &fbdo, const String &path, bool boolValue)
{
  return setBool(fbdo, path.c_str(), boolValue, false, "", "");
}

bool FirebaseESP32::setBool(FirebaseData &fbdo, const String &path, bool boolValue, float priority)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  bool ret = setBool(fbdo, path.c_str(), boolValue, false, buf, "");
  delS(buf);
  return ret;
}

bool FirebaseESP32::setBool(FirebaseData &fbdo, const String &path, bool boolValue, const String &ETag)
{
  return setBool(fbdo, path.c_str(), boolValue, false, "", ETag.c_str());
}

bool FirebaseESP32::setBool(FirebaseData &fbdo, const String &path, bool boolValue, float priority, const String &ETag)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  bool ret = setBool(fbdo, path.c_str(), boolValue, false, buf, ETag.c_str());
  delS(buf);
  return ret;
}

bool FirebaseESP32::setBool(FirebaseData &fbdo, const std::string &path, bool boolValue, bool queue, const std::string &priority, const std::string &etag)
{
  char *buf = boolStr(boolValue);
  bool ret = processRequest(fbdo, fb_esp_method::m_put, fb_esp_data_type::d_boolean, path, buf, false, priority, etag);
  delS(buf);
  return ret;
}

bool FirebaseESP32::setString(FirebaseData &fbdo, const String &path, const String &stringValue)
{
  return processRequest(fbdo, fb_esp_method::m_put, fb_esp_data_type::d_string, path.c_str(), stringValue.c_str(), false, "", "");
}

bool FirebaseESP32::setString(FirebaseData &fbdo, const String &path, const String &stringValue, float priority)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  bool ret = processRequest(fbdo, fb_esp_method::m_put, fb_esp_data_type::d_string, path.c_str(), stringValue.c_str(), false, buf, "");
  delS(buf);
  return ret;
}

bool FirebaseESP32::setString(FirebaseData &fbdo, const String &path, const String &stringValue, const String &ETag)
{
  return processRequest(fbdo, fb_esp_method::m_put, fb_esp_data_type::d_string, path.c_str(), stringValue.c_str(), false, "", ETag.c_str());
}

bool FirebaseESP32::setString(FirebaseData &fbdo, const String &path, const String &stringValue, float priority, const String &ETag)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  bool ret = processRequest(fbdo, fb_esp_method::m_put, fb_esp_data_type::d_string, path.c_str(), stringValue.c_str(), false, buf, ETag.c_str());
  delS(buf);
  return ret;
}

bool FirebaseESP32::setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json)
{
  std::string s;
  json._toStdString(s);
  bool ret = processRequest(fbdo, fb_esp_method::m_put, fb_esp_data_type::d_json, path.c_str(), s.c_str(), false, "", "");
  std::string().swap(s);
  return ret;
}

bool FirebaseESP32::setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  std::string s;
  json._toStdString(s);
  bool ret = processRequest(fbdo, fb_esp_method::m_put, fb_esp_data_type::d_json, path.c_str(), s.c_str(), false, buf, "");
  delS(buf);
  std::string().swap(s);
  return ret;
}

bool FirebaseESP32::setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, const String &ETag)
{
  std::string s;
  json._toStdString(s);
  bool ret = processRequest(fbdo, fb_esp_method::m_put, fb_esp_data_type::d_json, path.c_str(), s.c_str(), false, "", ETag.c_str());
  std::string().swap(s);
  return ret;
}

bool FirebaseESP32::setJSON(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority, const String &ETag)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  std::string s;
  json._toStdString(s);
  bool ret = processRequest(fbdo, fb_esp_method::m_put, fb_esp_data_type::d_json, path.c_str(), s.c_str(), false, buf, ETag.c_str());
  delS(buf);
  std::string().swap(s);
  return ret;
}

bool FirebaseESP32::setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr)
{
  String arrStr;
  arr.toString(arrStr);
  return processRequest(fbdo, fb_esp_method::m_put, fb_esp_data_type::d_array, path.c_str(), arrStr.c_str(), false, "", "");
}

bool FirebaseESP32::setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  String arrStr;
  arr.toString(arrStr);
  bool ret = processRequest(fbdo, fb_esp_method::m_put, fb_esp_data_type::d_array, path.c_str(), arrStr.c_str(), false, buf, "");
  delS(buf);
  return ret;
}

bool FirebaseESP32::setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, const String &ETag)
{
  String arrStr;
  arr.toString(arrStr);
  return processRequest(fbdo, fb_esp_method::m_put, fb_esp_data_type::d_array, path.c_str(), arrStr.c_str(), false, "", ETag.c_str());
}

bool FirebaseESP32::setArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray &arr, float priority, const String &ETag)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  String arrStr;
  arr.toString(arrStr);
  bool ret = processRequest(fbdo, fb_esp_method::m_put, fb_esp_data_type::d_array, path.c_str(), arrStr.c_str(), false, buf, ETag.c_str());
  delS(buf);
  return ret;
}

bool FirebaseESP32::setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size)
{
  return setBlob(fbdo, path.c_str(), blob, size, false, "", "");
}

bool FirebaseESP32::setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  bool ret = setBlob(fbdo, path.c_str(), blob, size, false, buf, "");
  delS(buf);
  return ret;
}

bool FirebaseESP32::setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, const String &ETag)
{
  return setBlob(fbdo, path.c_str(), blob, size, false, "", ETag.c_str());
}

bool FirebaseESP32::setBlob(FirebaseData &fbdo, const String &path, uint8_t *blob, size_t size, float priority, const String &ETag)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  bool ret = setBlob(fbdo, path.c_str(), blob, size, false, buf, ETag.c_str());
  delS(buf);
  return ret;
}

bool FirebaseESP32::setBlob(FirebaseData &fbdo, const std::string &path, uint8_t *blob, size_t size, bool queue, const std::string &priority, const std::string &etag)
{
  if (fbdo._maxBlobSize < size)
  {
    fbdo._httpCode = FIREBASE_ERROR_HTTP_CODE_PAYLOAD_TOO_LARGE;
    return false;
  }

  std::string base64Str;
  appendP(base64Str, fb_esp_pgm_str_92, true);
  base64Str += encodeBase64Str(blob, size);
  appendP(base64Str, fb_esp_pgm_str_3);
  bool ret = processRequest(fbdo, fb_esp_method::m_put_nocontent, fb_esp_data_type::d_blob, path, base64Str.c_str(), false, priority, etag);
  std::string().swap(base64Str);
  return ret;
}

bool FirebaseESP32::setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName)
{
  return processRequestFile(fbdo, storageType, fb_esp_method::m_put_nocontent, path.c_str(), fileName.c_str(), false, "", "");
}

bool FirebaseESP32::setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  bool ret = processRequestFile(fbdo, storageType, fb_esp_method::m_put_nocontent, path.c_str(), fileName.c_str(), false, buf, "");
  delS(buf);
  return ret;
}

bool FirebaseESP32::setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, const String &ETag)
{
  return processRequestFile(fbdo, storageType, fb_esp_method::m_put_nocontent, path.c_str(), fileName.c_str(), false, "", ETag.c_str());
}

bool FirebaseESP32::setFile(FirebaseData &fbdo, uint8_t storageType, const String &path, const String &fileName, float priority, const String &ETag)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  bool ret = processRequestFile(fbdo, storageType, fb_esp_method::m_put_nocontent, path.c_str(), fileName.c_str(), false, buf, ETag.c_str());
  delS(buf);
  return ret;
}

bool FirebaseESP32::setTimestamp(FirebaseData &fbdo, const String &path)
{
  char *tmp = strP(fb_esp_pgm_str_154);
  bool ret = processRequest(fbdo, fb_esp_method::m_put, fb_esp_data_type::d_timestamp, path.c_str(), tmp, false, "", "");
  delS(tmp);
  return ret;
}

bool FirebaseESP32::updateNode(FirebaseData &fbdo, const String path, FirebaseJson &json)
{
  std::string s;
  json._toStdString(s);
  bool ret = processRequest(fbdo, fb_esp_method::m_patch, fb_esp_data_type::d_json, path.c_str(), s.c_str(), false, "", "");
  std::string().swap(s);
  return ret;
}

bool FirebaseESP32::updateNode(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  std::string s;
  json._toStdString(s);
  bool ret = processRequest(fbdo, fb_esp_method::m_patch, fb_esp_data_type::d_json, path.c_str(), s.c_str(), false, buf, "");
  delS(buf);
  std::string().swap(s);
  return ret;
}

bool FirebaseESP32::updateNodeSilent(FirebaseData &fbdo, const String &path, FirebaseJson &json)
{
  std::string s;
  json._toStdString(s);
  bool ret = processRequest(fbdo, fb_esp_method::m_patch_nocontent, fb_esp_data_type::d_json, path.c_str(), s.c_str(), false, "", "");
  std::string().swap(s);
  return ret;
}

bool FirebaseESP32::updateNodeSilent(FirebaseData &fbdo, const String &path, FirebaseJson &json, float priority)
{
  char *buf = floatStr(priority);
  trimDigits(buf);
  std::string s;
  json._toStdString(s);
  bool ret = processRequest(fbdo, fb_esp_method::m_patch_nocontent, fb_esp_data_type::d_json, path.c_str(), s.c_str(), false, buf, "");
  delS(buf);
  std::string().swap(s);
  return ret;
}

bool FirebaseESP32::get(FirebaseData &fbdo, const String &path)
{
  fbdo.queryFilter.clear();
  bool ret = handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_get, fb_esp_data_type::d_any, "", "", "");
  return ret;
}

bool FirebaseESP32::getInt(FirebaseData &fbdo, const String &path)
{
  return getFloat(fbdo, path);
}

bool FirebaseESP32::getInt(FirebaseData &fbdo, const String &path, int &target)
{

  bool ret = false;
  fbdo.queryFilter.clear();

  uint8_t errCount = 0;
  uint8_t maxAttempt = fbdo._maxAttempt;
  if (maxAttempt == 0)
    maxAttempt = 1;

  for (int i = 0; i < maxAttempt; i++)
  {
    ret = handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_get, fb_esp_data_type::d_integer, "", "", "");
    target = fbdo.intData();
    if (ret)
      break;

    if (fbdo._maxAttempt > 0)
      if (!ret && connectionError(fbdo))
        errCount++;
  }

  if (!ret && errCount == maxAttempt && fbdo._qMan._maxQueue > 0)
    fbdo.addQueue(fb_esp_method::m_get, 0, fb_esp_data_type::d_integer, path.c_str(), "", "", false, &target, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

  if (ret && fbdo.resp_dataType != fb_esp_data_type::d_integer && fbdo.resp_dataType != fb_esp_data_type::d_float && fbdo.resp_dataType != fb_esp_data_type::d_double)
    ret = false;

  return ret;
}

bool FirebaseESP32::getFloat(FirebaseData &fbdo, const String &path)
{
  fbdo.queryFilter.clear();
  bool ret = handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_get, fb_esp_data_type::d_float, "", "", "");
  if (fbdo.resp_dataType != fb_esp_data_type::d_integer && fbdo.resp_dataType != fb_esp_data_type::d_float && fbdo.resp_dataType != fb_esp_data_type::d_double)
    ret = false;
  return ret;
}

bool FirebaseESP32::getFloat(FirebaseData &fbdo, const String &path, float &target)
{

  bool ret = false;
  fbdo.queryFilter.clear();

  uint8_t errCount = 0;
  uint8_t maxAttempt = fbdo._maxAttempt;
  if (maxAttempt == 0)
    maxAttempt = 1;

  for (int i = 0; i < maxAttempt; i++)
  {
    ret = handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_get, fb_esp_data_type::d_float, "", "", "");
    target = fbdo.floatData();
    if (ret)
      break;

    if (fbdo._maxAttempt > 0)
      if (!ret && connectionError(fbdo))
        errCount++;
  }

  if (!ret && errCount == maxAttempt && fbdo._qMan._maxQueue > 0)
    fbdo.addQueue(fb_esp_method::m_get, 0, fb_esp_data_type::d_float, path.c_str(), "", "", false, nullptr, &target, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

  if (ret && fbdo.resp_dataType != fb_esp_data_type::d_integer && fbdo.resp_dataType != fb_esp_data_type::d_float && fbdo.resp_dataType != fb_esp_data_type::d_double)
    ret = false;

  return ret;
}

bool FirebaseESP32::getDouble(FirebaseData &fbdo, const String &path)
{
  fbdo.queryFilter.clear();
  bool ret = handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_get, fb_esp_data_type::d_double, "", "", "");
  if (fbdo.resp_dataType != fb_esp_data_type::d_integer && fbdo.resp_dataType != fb_esp_data_type::d_float && fbdo.resp_dataType != fb_esp_data_type::d_double)
    ret = false;
  return ret;
}

bool FirebaseESP32::getDouble(FirebaseData &fbdo, const String &path, double &target)
{

  bool ret = false;
  fbdo.queryFilter.clear();

  uint8_t errCount = 0;
  uint8_t maxAttempt = fbdo._maxAttempt;
  if (maxAttempt == 0)
    maxAttempt = 1;

  for (int i = 0; i < maxAttempt; i++)
  {
    ret = handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_get, fb_esp_data_type::d_double, "", "", "");
    target = fbdo.floatData();
    if (ret)
      break;

    if (fbdo._maxAttempt > 0)
      if (!ret && connectionError(fbdo))
        errCount++;
  }

  if (!ret && errCount == maxAttempt && fbdo._qMan._maxQueue > 0)
    fbdo.addQueue(fb_esp_method::m_get, 0, fb_esp_data_type::d_double, path.c_str(), "", "", false, nullptr, nullptr, &target, nullptr, nullptr, nullptr, nullptr, nullptr);

  if (ret && fbdo.resp_dataType != fb_esp_data_type::d_integer && fbdo.resp_dataType != fb_esp_data_type::d_float && fbdo.resp_dataType != fb_esp_data_type::d_double)
    ret = false;

  return ret;
}

bool FirebaseESP32::getBool(FirebaseData &fbdo, const String &path)
{
  fbdo.queryFilter.clear();
  bool ret = handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_get, fb_esp_data_type::d_boolean, "", "", "");
  if (fbdo.resp_dataType != fb_esp_data_type::d_boolean)
    ret = false;
  return ret;
}

bool FirebaseESP32::getBool(FirebaseData &fbdo, const String &path, bool &target)
{

  bool ret = false;
  fbdo.queryFilter.clear();

  uint8_t errCount = 0;
  uint8_t maxAttempt = fbdo._maxAttempt;
  if (maxAttempt == 0)
    maxAttempt = 1;

  for (int i = 0; i < maxAttempt; i++)
  {
    ret = handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_get, fb_esp_data_type::d_boolean, "", "", "");
    target = fbdo.boolData();
    if (ret)
      break;

    if (fbdo._maxAttempt > 0)
      if (!ret && connectionError(fbdo))
        errCount++;
  }

  if (!ret && errCount == maxAttempt && fbdo._qMan._maxQueue > 0)
    fbdo.addQueue(fb_esp_method::m_get, 0, fb_esp_data_type::d_boolean, path.c_str(), "", "", false, nullptr, nullptr, nullptr, &target, nullptr, nullptr, nullptr, nullptr);

  if (ret && fbdo.resp_dataType != fb_esp_data_type::d_boolean)
    ret = false;

  return ret;
}

bool FirebaseESP32::getString(FirebaseData &fbdo, const String &path)
{
  fbdo.queryFilter.clear();
  bool ret = handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_get, fb_esp_data_type::d_string, "", "", "");
  if (fbdo.resp_dataType != fb_esp_data_type::d_string)
    ret = false;
  return ret;
}

bool FirebaseESP32::getString(FirebaseData &fbdo, const String &path, String &target)
{

  bool ret = false;
  fbdo.queryFilter.clear();

  uint8_t errCount = 0;
  uint8_t maxAttempt = fbdo._maxAttempt;
  if (maxAttempt == 0)
    maxAttempt = 1;

  for (int i = 0; i < maxAttempt; i++)
  {
    ret = handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_get, fb_esp_data_type::d_string, "", "", "");
    target = fbdo.stringData();
    if (ret)
      break;

    if (fbdo._maxAttempt > 0)
      if (!ret && connectionError(fbdo))
        errCount++;
  }

  if (!ret && errCount == maxAttempt && fbdo._qMan._maxQueue > 0)
    fbdo.addQueue(fb_esp_method::m_get, 0, fb_esp_data_type::d_string, path.c_str(), "", "", false, nullptr, nullptr, nullptr, nullptr, &target, nullptr, nullptr, nullptr);

  if (ret && fbdo.resp_dataType != fb_esp_data_type::d_string)
    ret = false;

  return ret;
}

bool FirebaseESP32::getJSON(FirebaseData &fbdo, const String &path)
{
  fbdo.queryFilter.clear();
  bool ret = handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_get, fb_esp_data_type::d_json, "", "", "");
  if (fbdo.resp_dataType != fb_esp_data_type::d_json && fbdo.resp_dataType != fb_esp_data_type::d_null)
    ret = false;
  return ret;
}

bool FirebaseESP32::getJSON(FirebaseData &fbdo, const String &path, FirebaseJson *target)
{

  bool ret;
  fbdo.queryFilter.clear();

  uint8_t errCount = 0;
  uint8_t maxAttempt = fbdo._maxAttempt;
  if (maxAttempt == 0)
    maxAttempt = 1;

  for (int i = 0; i < maxAttempt; i++)
  {
    ret = handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_get, fb_esp_data_type::d_json, "", "", "");
    target = fbdo.jsonObjectPtr();
    if (ret)
      break;

    if (fbdo._maxAttempt > 0)
      if (!ret && connectionError(fbdo))
        errCount++;
  }

  if (!ret && errCount == maxAttempt && fbdo._qMan._maxQueue > 0)
    fbdo.addQueue(fb_esp_method::m_get, 0, fb_esp_data_type::d_json, path.c_str(), "", "", false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, target, nullptr);

  if (ret && fbdo.resp_dataType != fb_esp_data_type::d_json && fbdo.resp_dataType != fb_esp_data_type::d_null)
    ret = false;

  return ret;
}

bool FirebaseESP32::getJSON(FirebaseData &fbdo, const String &path, QueryFilter &query)
{
  fbdo.queryFilter.clear();
  if (query._orderBy != "")
    fbdo.setQuery(query);

  bool ret = handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_get, fb_esp_data_type::d_json, "", "", "");
  if (fbdo.resp_dataType != fb_esp_data_type::d_json && fbdo.resp_dataType != fb_esp_data_type::d_null)
    ret = false;
  return ret;
}

bool FirebaseESP32::getJSON(FirebaseData &fbdo, const String &path, QueryFilter &query, FirebaseJson *target)
{
  fbdo.queryFilter.clear();
  if (query._orderBy.length() > 0)
    fbdo.setQuery(query);

  bool ret;

  uint8_t errCount = 0;
  uint8_t maxAttempt = fbdo._maxAttempt;
  if (maxAttempt == 0)
    maxAttempt = 1;

  for (int i = 0; i < maxAttempt; i++)
  {
    ret = handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_get, fb_esp_data_type::d_json, "", "", "");
    target = fbdo.jsonObjectPtr();
    if (ret)
      break;

    if (fbdo._maxAttempt > 0)
      if (!ret && connectionError(fbdo))
        errCount++;
  }

  if (!ret && errCount == maxAttempt && fbdo._qMan._maxQueue > 0)
    fbdo.addQueue(fb_esp_method::m_get, 0, fb_esp_data_type::d_json, path.c_str(), "", "", true, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, target, nullptr);

  if (ret && fbdo.resp_dataType != fb_esp_data_type::d_json && fbdo.resp_dataType != fb_esp_data_type::d_null)
    ret = false;

  return ret;
}

bool FirebaseESP32::getArray(FirebaseData &fbdo, const String &path)
{
  fbdo.queryFilter.clear();
  bool ret = handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_get, fb_esp_data_type::d_array, "", "", "");
  if (fbdo.resp_dataType != fb_esp_data_type::d_array)
    ret = false;
  return ret;
}

bool FirebaseESP32::getArray(FirebaseData &fbdo, const String &path, FirebaseJsonArray *target)
{

  bool ret;
  fbdo.queryFilter.clear();

  uint8_t errCount = 0;
  uint8_t maxAttempt = fbdo._maxAttempt;
  if (maxAttempt == 0)
    maxAttempt = 1;

  for (int i = 0; i < maxAttempt; i++)
  {
    ret = handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_get, fb_esp_data_type::d_array, "", "", "");
    target = fbdo.jsonArrayPtr();
    if (ret)
      break;

    if (fbdo._maxAttempt > 0)
      if (!ret && connectionError(fbdo))
        errCount++;
  }

  if (!ret && errCount == maxAttempt && fbdo._qMan._maxQueue > 0)
    fbdo.addQueue(fb_esp_method::m_get, 0, fb_esp_data_type::d_array, path.c_str(), "", "", false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, target);

  if (ret && fbdo.resp_dataType != fb_esp_data_type::d_array && fbdo.resp_dataType != fb_esp_data_type::d_null)
    ret = false;

  return ret;
}

bool FirebaseESP32::getArray(FirebaseData &fbdo, const String &path, QueryFilter &query)
{
  fbdo.queryFilter.clear();
  if (query._orderBy != "")
    fbdo.setQuery(query);

  bool ret = handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_get, fb_esp_data_type::d_array, "", "", "");
  if (fbdo.resp_dataType != fb_esp_data_type::d_array && fbdo.resp_dataType != fb_esp_data_type::d_null)
    ret = false;
  return ret;
}

bool FirebaseESP32::getArray(FirebaseData &fbdo, const String &path, QueryFilter &query, FirebaseJsonArray *target)
{
  fbdo.queryFilter.clear();
  if (query._orderBy.length() > 0)
    fbdo.setQuery(query);

  bool ret;

  uint8_t errCount = 0;
  uint8_t maxAttempt = fbdo._maxAttempt;
  if (maxAttempt == 0)
    maxAttempt = 1;

  for (int i = 0; i < maxAttempt; i++)
  {
    ret = handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_get, fb_esp_data_type::d_array, "", "", "");
    target = fbdo.jsonArrayPtr();
    if (ret)
      break;

    if (fbdo._maxAttempt > 0)
      if (!ret && connectionError(fbdo))
        errCount++;
  }

  if (!ret && errCount == maxAttempt && fbdo._qMan._maxQueue > 0)
    fbdo.addQueue(fb_esp_method::m_get, 0, fb_esp_data_type::d_json, path.c_str(), "", "", true, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, target);

  if (ret && fbdo.resp_dataType != fb_esp_data_type::d_array && fbdo.resp_dataType != fb_esp_data_type::d_null)
    ret = false;

  return ret;
}

bool FirebaseESP32::getBlob(FirebaseData &fbdo, const String &path)
{
  fbdo.queryFilter.clear();
  bool ret = handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_get, fb_esp_data_type::d_blob, "", "", "");
  if (fbdo.resp_dataType != fb_esp_data_type::d_blob)
    ret = false;
  return ret;
}

bool FirebaseESP32::getBlob(FirebaseData &fbdo, const String &path, std::vector<uint8_t> &target)
{

  fbdo.queryFilter.clear();

  bool ret = false;

  uint8_t errCount = 0;
  uint8_t maxAttempt = fbdo._maxAttempt;
  if (maxAttempt == 0)
    maxAttempt = 1;

  for (int i = 0; i < maxAttempt; i++)
  {
    ret = handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_get, fb_esp_data_type::d_blob, "", "", "");
    target = fbdo.blobData();
    if (ret)
      break;

    if (fbdo._maxAttempt > 0)
      if (!ret && connectionError(fbdo))
        errCount++;
  }

  if (!ret && errCount == maxAttempt && fbdo._qMan._maxQueue > 0)
    fbdo.addQueue(fb_esp_method::m_get, 0, fb_esp_data_type::d_blob, path.c_str(), "", "", false, nullptr, nullptr, nullptr, nullptr, nullptr, &target, nullptr, nullptr);

  if (ret && fbdo.resp_dataType != fb_esp_data_type::d_blob)
    ret = false;

  return ret;
}

bool FirebaseESP32::getFile(FirebaseData &fbdo, uint8_t storageType, const String &nodePath, const String &fileName)
{
  fbdo.queryFilter.clear();
  fbdo._fileName = fileName.c_str();

  bool ret = false;

  uint8_t errCount = 0;
  uint8_t maxAttempt = fbdo._maxAttempt;
  if (maxAttempt == 0)
    maxAttempt = 1;

  for (int i = 0; i < maxAttempt; i++)
  {
    ret = handleRequest(fbdo, storageType, nodePath.c_str(), fb_esp_method::m_get, fb_esp_data_type::d_file, "", "", "");
    if (ret)
      break;

    if (fbdo._maxAttempt > 0)
      if (!ret && (connectionError(fbdo) || fbdo._file_transfer_error.length() > 0))
        errCount++;
  }

  if (!ret && errCount == maxAttempt && fbdo._qMan._maxQueue > 0)
    fbdo.addQueue(fb_esp_method::m_get, storageType, fb_esp_data_type::d_file, nodePath.c_str(), fileName.c_str(), "", false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

  return ret;
}

bool FirebaseESP32::deleteNode(FirebaseData &fbdo, const String &path)
{
  fbdo.queryFilter.clear();
  return handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_delete, fb_esp_data_type::d_string, "", "", "");
}

bool FirebaseESP32::deleteNode(FirebaseData &fbdo, const String &path, const String &ETag)
{
  fbdo.queryFilter.clear();
  return handleRequest(fbdo, 0, path.c_str(), fb_esp_method::m_delete, fb_esp_data_type::d_string, "", "", ETag.c_str());
}

bool FirebaseESP32::beginStream(FirebaseData &fbdo, const String &path)
{
  fbdo._pause = false;
  fbdo.clearNodeList();

  if (!reconnect(fbdo))
    return false;

  closeHTTP(fbdo);

  fbdo._pause = false;
  fbdo._streamStop = false;
  fbdo._isDataTimeout = false;
  fbdo._streamPath = path.c_str();

  if (!handleStreamRequest(fbdo, path.c_str()))
  {
    if (!tokenReady(fbdo))
      return true;

    return false;
  }

  clearDataStatus(fbdo);

  return waitResponse(fbdo);
}

bool FirebaseESP32::beginMultiPathStream(FirebaseData &fbdo, const String &parentPath, const String *childPath, size_t size)
{
  fbdo.addNodeList(childPath, size);

  if (!reconnect(fbdo))
    return false;

  fbdo._pause = false;
  fbdo._streamStop = false;
  fbdo._isDataTimeout = false;

  if (!handleStreamRequest(fbdo, parentPath.c_str()))
  {
    if (!tokenReady(fbdo))
      return true;

    return false;
  }

  clearDataStatus(fbdo);
  return waitResponse(fbdo);
}

bool FirebaseESP32::readStream(FirebaseData &fbdo)
{
  return handleStreamRead(fbdo);
}

bool FirebaseESP32::endStream(FirebaseData &fbdo)
{
  fbdo._pause = true;
  fbdo._streamStop = true;
  fbdo._isStream = false;
  closeHTTP(fbdo);
  clearDataStatus(fbdo);
  return true;
}

int FirebaseESP32::sendRequest(FirebaseData &fbdo, const std::string &path, fb_esp_method method, fb_esp_data_type dataType, const std::string &payload, const std::string &priority)
{

  fbdo._fbError.clear();

  if (fbdo._pause)
    return 0;

  if (!reconnect(fbdo))
    return FIREBASE_ERROR_HTTPC_ERROR_CONNECTION_LOST;

  if (!tokenReady(fbdo))
    return FIREBASE_ERROR_TOKEN_NOT_READY;

  if (!validRequest(fbdo, path))
    return FIREBASE_ERROR_HTTP_CODE_BAD_REQUEST;

  uint8_t attempts = 0;
  uint8_t maxAttempt = 5;

  size_t buffSize = 32;
  char *tmp = nullptr;
  char *buf = nullptr;

  int len = 0;
  size_t toRead = 0;
  int ret = -1;

  std::string payloadBuf = "";
  std::string header = "";

  if (method == fb_esp_method::m_stream)
  {
    //stream path change? reset the current (keep alive) connection
    if (path != fbdo._streamPath)
      fbdo._streamPathChanged = true;

    if (!fbdo._isStream || fbdo._streamPathChanged || fbdo._isFCM)
      closeHTTP(fbdo);

    if (fbdo._isFCM || !fbdo._isStream)
      setSecure(fbdo);

    fbdo._streamPath.clear();

    if (path.length() > 0)
      if (path[0] != '/')
        appendP(fbdo._streamPath, fb_esp_pgm_str_1, true);

    fbdo._streamPath += path;

    fbdo._isStream = true;
    fbdo._isRTDB = false;
    fbdo._isFCM = false;
  }
  else
  {
    //last requested method was stream or fcm?, close the connection
    if (fbdo._isStream || fbdo._isFCM)
      closeHTTP(fbdo);

    if (fbdo._isFCM || !fbdo._isRTDB)
      setSecure(fbdo);

    fbdo._isRTDB = true;
    fbdo._isStream = false;
    fbdo._isFCM = false;

    fbdo._path.clear();
    fbdo._resp_etag.clear();

    if (method != fb_esp_method::m_download && method != fb_esp_method::m_restore)
    {
      fbdo._path.clear();
      if (path.length() > 0)
        if (path[0] != '/')
          appendP(fbdo._path, fb_esp_pgm_str_1, true);
      fbdo._path += path;
    }

    fbdo._isDataTimeout = false;
  }

  fbdo.httpClient.begin(_cfg->host.c_str(), FIREBASE_PORT);

  //Prepare for string and JSON payloads
  preparePayload(method, dataType, priority, payload, payloadBuf);

  //Prepare request header
  if (method != fb_esp_method::m_download && method != fb_esp_method::m_restore && dataType != fb_esp_data_type::d_file)
  {
    //for non-file data
    bool sv = false;
    if (dataType == fb_esp_data_type::d_json)
    {
      tmp = strP(fb_esp_pgm_str_166);
      sv = payloadBuf.find(tmp) != std::string::npos;
      delS(tmp);
    }
    prepareHeader(fbdo, method, dataType, path, payloadBuf.length(), header, sv);
  }
  else
  {
    //for file data payload
    if (fbdo._storageType == StorageType::FLASH)
    {
      if (!FLASH_FS.begin(true))
      {
        appendP(fbdo._file_transfer_error, fb_esp_pgm_str_164, true);
        return FIREBASE_ERROR_FILE_IO_ERROR;
      }
    }
    else if (fbdo._storageType == StorageType::SD)
    {
      if (_sdInUse)
      {
        appendP(fbdo._file_transfer_error, fb_esp_pgm_str_84, true);
        return FIREBASE_ERROR_FILE_IO_ERROR;
      }

      if (!_sdOk)
        _sdOk = sdTest();

      if (!_sdOk)
      {
        appendP(fbdo._file_transfer_error, fb_esp_pgm_str_85, true);
        return FIREBASE_ERROR_FILE_IO_ERROR;
      }

      _sdInUse = true;
    }

    if (method == fb_esp_method::m_download || method == fb_esp_method::m_restore)
    {

      fbdo._backupFilename = fbdo._backupDir;

      if (fbdo._backupFilename.length() == 0)
        appendP(fbdo._backupFilename, fb_esp_pgm_str_1);
      else if (fbdo._backupFilename[fbdo._backupFilename.length() - 1] != '/')
        appendP(fbdo._backupFilename, fb_esp_pgm_str_1);
      else if (fbdo._backupFilename[0] != '/')
      {
        char *b = strP(fb_esp_pgm_str_1);
        std::string t = b;
        fbdo._backupFilename = t + fbdo._backupFilename;
        delS(b);
        std::string().swap(t);
      }

      for (int i = 1; i < fbdo._backupNodePath.length(); i++)
      {
        if (fbdo._backupNodePath[i] == '/')
          appendP(fbdo._backupFilename, fb_esp_pgm_str_4);
        else
          fbdo._backupFilename += fbdo._backupNodePath[i];
      }

      std::string s = fbdo._backupDir;
      appendP(s, fb_esp_pgm_str_1);
      if (fbdo._backupFilename == s)
        appendP(fbdo._backupFilename, fb_esp_pgm_str_90, true);
      else
        appendP(fbdo._backupFilename, fb_esp_pgm_str_89, false);
      std::string().swap(s);

      if (method == fb_esp_method::m_download)
      {
        if (fbdo._storageType == StorageType::FLASH)
        {
          if (FLASH_FS.exists(fbdo._backupFilename.c_str()))
            FLASH_FS.remove(fbdo._backupFilename.c_str());
          file = FLASH_FS.open(fbdo._backupFilename.c_str(), "w");
        }
        else if (fbdo._storageType == StorageType::SD)
        {
          if (SD.exists(fbdo._backupFilename.c_str()))
            SD.remove(fbdo._backupFilename.c_str());
          file = SD.open(fbdo._backupFilename.c_str(), FILE_WRITE);
        }
      }
      else if (method == fb_esp_method::m_restore)
      {

        if (fbdo._storageType == StorageType::FLASH && FLASH_FS.exists(fbdo._backupFilename.c_str()))
          file = FLASH_FS.open(fbdo._backupFilename.c_str(), "r");
        else if (fbdo._storageType == StorageType::SD && SD.exists(fbdo._backupFilename.c_str()))
          file = SD.open(fbdo._backupFilename.c_str(), FILE_READ);
        else
        {
          appendP(fbdo._file_transfer_error, fb_esp_pgm_str_83, true);
          return FIREBASE_ERROR_FILE_IO_ERROR;
        }
        len = file.size();
      }

      if (!file)
      {
        appendP(fbdo._file_transfer_error, fb_esp_pgm_str_86, true);
        return FIREBASE_ERROR_FILE_IO_ERROR;
      }
    }

    if (dataType == fb_esp_data_type::d_file)
    {

      if (method == fb_esp_method::m_put_nocontent || method == fb_esp_method::m_post)
      {
        if (fbdo._storageType == StorageType::FLASH)
        {
          if (FLASH_FS.exists(fbdo._fileName.c_str()))
            file = FLASH_FS.open(fbdo._fileName.c_str(), "r");
          else
          {
            appendP(fbdo._file_transfer_error, fb_esp_pgm_str_83, true);
            return FIREBASE_ERROR_FILE_IO_ERROR;
          }
        }
        else if (fbdo._storageType == StorageType::SD)
        {
          if (SD.exists(fbdo._fileName.c_str()))
            file = SD.open(fbdo._fileName.c_str(), FILE_READ);
          else
          {
            appendP(fbdo._file_transfer_error, fb_esp_pgm_str_83, true);
            return FIREBASE_ERROR_FILE_IO_ERROR;
          }
        }

        len = (4 * ceil(file.size() / 3.0)) + strlen_P(fb_esp_pgm_str_93) + 1;
      }
      else if (method == fb_esp_method::m_get)
      {

        tmp = strP(fb_esp_pgm_str_1);
        int p1 = fbdo._fileName.find_last_of(tmp);
        delS(tmp);
        std::string folder = "/";

        if (p1 != std::string::npos && p1 != 0)
          folder = fbdo._fileName.substr(p1 - 1);

        if (fbdo._storageType == StorageType::FLASH)
        {
          file = FLASH_FS.open(fbdo._fileName.c_str(), "w");
        }
        else if (fbdo._storageType == StorageType::SD)
        {
          if (!SD.exists(folder.c_str()))
            createDirs(folder, fbdo._storageType);

          SD.remove(fbdo._fileName.c_str());

          file = SD.open(fbdo._fileName.c_str(), FILE_WRITE);
        }
        std::string().swap(folder);
      }

      if (!file)
      {
        appendP(fbdo._file_transfer_error, fb_esp_pgm_str_86, true);
        return FIREBASE_ERROR_FILE_IO_ERROR;
      }
    }

    if (dataType == fb_esp_data_type::d_file)
      prepareHeader(fbdo, method, dataType, fbdo._path.c_str(), len, header, false);
    else
      prepareHeader(fbdo, method, dataType, fbdo._backupNodePath.c_str(), len, header, false);
  }

  if (method == fb_esp_method::m_get_nocontent || method == fb_esp_method::m_patch_nocontent || (method == fb_esp_method::m_put_nocontent && dataType == fb_esp_data_type::d_blob))
    fbdo._reqNoContent = true;

  if (dataType == fb_esp_data_type::d_blob)
    std::vector<uint8_t>().swap(fbdo._blob);

  if (!reconnect(fbdo))
    return FIREBASE_ERROR_HTTPC_ERROR_CONNECTION_LOST;

  //Send request
  ret = fbdo.httpClient.send(header.c_str(), payloadBuf.c_str());

  //Retry
  if (method != fb_esp_method::m_stream)
  {
    attempts = 0;
    while (ret != 0)
    {
      attempts++;
      if (attempts > maxAttempt)
        break;

      if (!reconnect(fbdo))
        return FIREBASE_ERROR_HTTPC_ERROR_CONNECTION_LOST;

      ret = fbdo.httpClient.send(header.c_str(), payloadBuf.c_str());
    }
  }

  //handle send file data
  if (method == fb_esp_method::m_restore || (dataType == fb_esp_data_type::d_file && (method == fb_esp_method::m_put_nocontent || method == fb_esp_method::m_post)))
  {
    if (dataType == fb_esp_data_type::d_file && (method == fb_esp_method::m_put_nocontent || method == fb_esp_method::m_post))
    {

      buf = strP(fb_esp_pgm_str_93);
      ret = fbdo.httpClient.send("", buf);
      delS(buf);
      if (ret == 0)
        sendBase64Stream(fbdo.httpClient.stream(), fbdo._fileName, fbdo._storageType);
      else
        return FIREBASE_ERROR_HTTPC_ERROR_SEND_PAYLOAD_FAILED;

      buf = newS(2);
      buf[0] = '"';
      buf[1] = '\0';
      ret = fbdo.httpClient.send("", buf);
      delS(buf);

      if (ret != 0)
        return FIREBASE_ERROR_HTTPC_ERROR_SEND_PAYLOAD_FAILED;
    }
    else
    {
      while (len)
      {
        if (!reconnect(fbdo))
          return FIREBASE_ERROR_HTTPC_ERROR_CONNECTION_LOST;

        toRead = len;
        if (toRead > buffSize)
          toRead = buffSize - 1;

        buf = newS(buffSize);
        memset(buf, 0, buffSize);
        file.read((uint8_t *)buf, toRead);
        buf[toRead] = '\0';

        ret = fbdo.httpClient.send("", buf);
        delS(buf);

        if (ret != 0)
          return FIREBASE_ERROR_HTTPC_ERROR_SEND_PAYLOAD_FAILED;

        len -= toRead;

        if (len <= 0)
          break;
      }
    }

    closeFileHandle(fbdo);
  }

  fbdo._httpConnected = ret == 0;
  return ret;
}

void FirebaseESP32::delS(char *p)
{
  if (p != nullptr)
    delete[] p;
}

char *FirebaseESP32::newS(size_t len)
{
  char *p = new char[len];
  memset(p, 0, len);
  return p;
}

char *FirebaseESP32::newS(char *p, size_t len)
{
  delS(p);
  p = newS(len);
  return p;
}

char *FirebaseESP32::newS(char *p, size_t len, char *d)
{
  delS(p);
  p = newS(len);
  strcpy(p, d);
  return p;
}

bool FirebaseESP32::waitIdle(FirebaseData &fbdo)
{
  if (_multipleRequests)
    return true;

  unsigned long wTime = millis();
  while (processing)
  {
    if (millis() - wTime > 3000)
    {
      fbdo._httpCode = FIREBASE_ERROR_HTTPC_ERROR_CONNECTION_INUSED;
      return false;
    }
    delay(0);
  }

  return true;
}

bool FirebaseESP32::handleRequest(FirebaseData &fbdo, uint8_t storageType, const std::string &path, fb_esp_method method, fb_esp_data_type dataType, const std::string &payload, const std::string &priority, const std::string &etag)
{

  fbdo._fbError.clear();

  if (fbdo._pause)
    return true;

  if (!reconnect(fbdo))
    return false;

  if (!tokenReady(fbdo))
    return false;

  if (!validRequest(fbdo, path))
    return false;

  if ((method == fb_esp_method::m_put || method == fb_esp_method::m_post || method == fb_esp_method::m_patch || method == fb_esp_method::m_patch_nocontent || method == fb_esp_method::m_set_rules) && payload.length() == 0 && dataType != fb_esp_data_type::d_string && dataType != fb_esp_data_type::d_blob && dataType != fb_esp_data_type::d_file)
  {
    fbdo._httpCode = FIREBASE_ERROR_HTTP_CODE_BAD_REQUEST;
    return false;
  }

  if (!waitIdle(fbdo))
    return false;

  fbdo._qID = 0;
  fbdo._req_etag = etag;
  fbdo._priority = priority;
  fbdo._storageType = storageType;

  if (method != fb_esp_method::m_stream)
    if (!fbdo._keepAlive)
      closeHTTP(fbdo);

  fbdo._redirectURL.clear();
  fbdo._req_method = method;
  fbdo._req_dataType = dataType;
  fbdo._mismatchDataType = false;

  int ret = sendRequest(fbdo, path, method, dataType, payload, priority);

  if (ret == 0)
  {
    fbdo._httpConnected = true;

    if (method == fb_esp_method::m_stream)
    {
      if (!waitResponse(fbdo))
      {
        closeHTTP(fbdo);
        return false;
      }
    }
    else if (method == fb_esp_method::m_download || (dataType == fb_esp_data_type::d_file && method == fb_esp_method::m_get))
    {
      fbdo._req_method = method;
      fbdo._req_dataType = dataType;
      if (!waitResponse(fbdo))
      {
        closeHTTP(fbdo);
        return false;
      }
    }
    else if (method == fb_esp_method::m_restore || (dataType == fb_esp_data_type::d_file && method == fb_esp_method::m_put_nocontent))
    {
      fbdo._req_method = method;
      fbdo._req_dataType = dataType;
      if (!waitResponse(fbdo))
      {
        closeHTTP(fbdo);
        return false;
      }
    }
    else
    {
      fbdo._path = path;
      if (!waitResponse(fbdo))
      {
        closeHTTP(fbdo);
        return false;
      }
      fbdo._dataAvailable = fbdo._data.length() > 0 || fbdo._blob.size() > 0;
    }
  }
  else
  {
    fbdo._httpCode = ret;
    if (ret != FIREBASE_ERROR_HTTPC_ERROR_CONNECTION_INUSED)
      fbdo._httpConnected = false;
    return false;
  }

  return true;
}

void FirebaseESP32::parseRespHeader(const char *buf, server_response_data_t &response)
{
  int beginPos = 0, pmax = 0, payloadPos = 0;

  char *tmp = nullptr;

  if (response.httpCode != -1)
  {
    payloadPos = beginPos;
    pmax = beginPos;
    tmp = getHeader(buf, fb_esp_pgm_str_10, fb_esp_pgm_str_21, beginPos, 0);
    if (tmp)
    {
      response.connection = tmp;
      delS(tmp);
    }
    if (pmax < beginPos)
      pmax = beginPos;
    beginPos = payloadPos;
    tmp = getHeader(buf, fb_esp_pgm_str_8, fb_esp_pgm_str_21, beginPos, 0);
    if (tmp)
    {
      response.contentType = tmp;
      delS(tmp);
    }

    if (pmax < beginPos)
      pmax = beginPos;
    beginPos = payloadPos;
    tmp = getHeader(buf, fb_esp_pgm_str_12, fb_esp_pgm_str_21, beginPos, 0);
    if (tmp)
    {
      response.contentLen = atoi(tmp);
      delS(tmp);
    }

    if (pmax < beginPos)
      pmax = beginPos;
    beginPos = payloadPos;
    tmp = getHeader(buf, fb_esp_pgm_str_167, fb_esp_pgm_str_21, beginPos, 0);
    if (tmp)
    {
      response.transferEnc = tmp;
      if (stringCompare(tmp, 0, fb_esp_pgm_str_168))
        response.isChunkedEnc = true;
      delS(tmp);
    }

    if (pmax < beginPos)
      pmax = beginPos;
    beginPos = payloadPos;
    tmp = getHeader(buf, fb_esp_pgm_str_150, fb_esp_pgm_str_21, beginPos, 0);
    if (tmp)
    {
      response.etag = tmp;
      delS(tmp);
    }

    if (pmax < beginPos)
      pmax = beginPos;
    beginPos = payloadPos;
    tmp = getHeader(buf, fb_esp_pgm_str_10, fb_esp_pgm_str_21, beginPos, 0);
    if (tmp)
    {
      response.connection = tmp;
      delS(tmp);
    }

    if (pmax < beginPos)
      pmax = beginPos;
    beginPos = payloadPos;
    tmp = getHeader(buf, fb_esp_pgm_str_12, fb_esp_pgm_str_21, beginPos, 0);
    if (tmp)
    {

      response.payloadLen = atoi(tmp);
      delS(tmp);
    }

    if (response.httpCode == FIREBASE_ERROR_HTTP_CODE_TEMPORARY_REDIRECT || response.httpCode == FIREBASE_ERROR_HTTP_CODE_PERMANENT_REDIRECT || response.httpCode == FIREBASE_ERROR_HTTP_CODE_MOVED_PERMANENTLY || response.httpCode == FIREBASE_ERROR_HTTP_CODE_FOUND)
    {
      if (pmax < beginPos)
        pmax = beginPos;
      beginPos = payloadPos;
      tmp = getHeader(buf, fb_esp_pgm_str_95, fb_esp_pgm_str_21, beginPos, 0);
      if (tmp)
      {
        response.location = tmp;
        delS(tmp);
      }
    }

    if (response.httpCode == FIREBASE_ERROR_HTTP_CODE_NO_CONTENT)
      response.noContent = true;
  }
}

void FirebaseESP32::parseRespPayload(const char *buf, server_response_data_t &response, bool getOfs)
{

  int payloadPos = 0;
  int payloadOfs = 0;

  char *tmp = nullptr;

  if (!response.isEvent && !response.noEvent)
  {

    tmp = getHeader(buf, fb_esp_pgm_str_13, fb_esp_pgm_str_180, payloadPos, 0);
    if (tmp)
    {
      response.isEvent = true;
      response.eventType = tmp;
      delS(tmp);

      payloadOfs = payloadPos;

      tmp = getHeader(buf, fb_esp_pgm_str_14, fb_esp_pgm_str_180, payloadPos, 0);
      if (tmp)
      {

        payloadOfs += strlen_P(fb_esp_pgm_str_14);
        payloadPos = payloadOfs;
        response.hasEventData = true;

        delS(tmp);

        tmp = getHeader(buf, fb_esp_pgm_str_17, fb_esp_pgm_str_3, payloadPos, 0);

        if (tmp)
        {
          payloadOfs = payloadPos;
          response.eventPath = tmp;
          delS(tmp);

          tmp = getHeader(buf, fb_esp_pgm_str_18, fb_esp_pgm_str_180, payloadPos, 0);

          if (tmp)
          {
            tmp[strlen(tmp) - 1] = 0;
            response.payloadLen = strlen(tmp);
            response.eventData = tmp;
            payloadOfs += strlen_P(fb_esp_pgm_str_18) + 1;
            response.payloadOfs = payloadOfs;
            delS(tmp);
          }
        }
      }
    }
  }

  if (strlen(buf) < payloadOfs)
    return;

  if (response.dataType == 0)
  {

    tmp = getHeader(buf, fb_esp_pgm_str_20, fb_esp_pgm_str_3, payloadPos, 0);
    if (tmp)
    {
      response.pushName = tmp;
      delS(tmp);
    }

    tmp = getHeader(buf, fb_esp_pgm_str_102, fb_esp_pgm_str_3, payloadPos, 0);
    if (tmp)
    {
      delS(tmp);
      FirebaseJson js;
      FirebaseJsonData d;
      js.setJsonData(buf);
      tmp = strP(fb_esp_pgm_str_176);
      js.get(d, tmp);
      delS(tmp);
      if (d.success)
        response.fbError = d.stringValue.c_str();
    }

    if (stringCompare(buf, payloadOfs, fb_esp_pgm_str_92))
    {

      response.dataType = fb_esp_data_type::d_blob;
      if ((response.isEvent && response.hasEventData) || getOfs)
      {
        int dlen = response.eventData.length() - strlen_P(fb_esp_pgm_str_92) - 1;
        response.payloadLen = dlen;
        response.payloadOfs += strlen_P(fb_esp_pgm_str_92);
        response.eventData.clear();
      }
    }
    else if (stringCompare(buf, payloadOfs, fb_esp_pgm_str_93))
    {
      response.dataType = fb_esp_data_type::d_file;
      if ((response.isEvent && response.hasEventData) || getOfs)
      {
        int dlen = response.eventData.length() - strlen_P(fb_esp_pgm_str_93) - 1;
        response.payloadLen = dlen;
        response.payloadOfs += strlen_P(fb_esp_pgm_str_93);
        response.eventData.clear();
      }
    }
    else if (stringCompare(buf, payloadOfs, fb_esp_pgm_str_3))
    {
      response.dataType = fb_esp_data_type::d_string;
    }
    else if (stringCompare(buf, payloadOfs, fb_esp_pgm_str_163))
    {
      response.dataType = fb_esp_data_type::d_json;
    }
    else if (stringCompare(buf, payloadOfs, fb_esp_pgm_str_182))
    {
      response.dataType = fb_esp_data_type::d_array;
    }
    else if (stringCompare(buf, payloadOfs, fb_esp_pgm_str_106) || stringCompare(buf, payloadOfs, fb_esp_pgm_str_107))
    {
      response.dataType = fb_esp_data_type::d_boolean;
      response.boolData = stringCompare(buf, payloadOfs, fb_esp_pgm_str_107);
    }
    else if (stringCompare(buf, payloadOfs, fb_esp_pgm_str_19))
    {
      response.dataType = fb_esp_data_type::d_null;
    }
    else
    {
      tmp = strP(fb_esp_pgm_str_4);
      int p1 = strpos(buf, tmp, payloadOfs);
      delS(tmp);
      setNumDataType(buf, payloadOfs, response, p1 != -1);
    }
  }
}

void FirebaseESP32::setNumDataType(const char *buf, int ofs, server_response_data_t &response, bool dec)
{
  if (response.payloadLen > 0 && response.payloadLen <= (int)strlen(buf) && ofs < (int)strlen(buf) && ofs + response.payloadLen <= (int)strlen(buf))
  {
    char *tmp = newS(response.payloadLen + 1);
    memcpy(tmp, &buf[ofs], response.payloadLen);
    tmp[response.payloadLen] = 0;
    double d = atof(tmp);
    delS(tmp);

    if (dec)
    {
      if (response.payloadLen <= 7)
      {
        response.floatData = d;
        response.dataType = fb_esp_data_type::d_float;
      }
      else
      {
        response.doubleData = d;
        response.dataType = fb_esp_data_type::d_double;
      }
    }
    else
    {
      if (d > 0x7fffffff)
      {
        response.doubleData = d;
        response.dataType = fb_esp_data_type::d_double;
      }
      else
      {
        response.intData = (int)d;
        response.dataType = fb_esp_data_type::d_integer;
      }
    }
  }
}

char *FirebaseESP32::getHeader(const char *buf, PGM_P beginH, PGM_P endH, int &beginPos, int endPos)
{

  char *tmp = strP(beginH);
  int p1 = strpos(buf, tmp, beginPos);
  int ofs = 0;
  delS(tmp);
  if (p1 != -1)
  {
    tmp = strP(endH);
    int p2 = -1;
    if (endPos > 0)
      p2 = endPos;
    else if (endPos == 0)
    {
      ofs = strlen_P(endH);
      p2 = strpos(buf, tmp, p1 + strlen_P(beginH) + 1);
    }
    else if (endPos == -1)
    {
      beginPos = p1 + strlen_P(beginH);
    }

    if (p2 == -1)
      p2 = strlen(buf);

    delS(tmp);

    if (p2 != -1)
    {
      beginPos = p2 + ofs;
      int len = p2 - p1 - strlen_P(beginH);
      tmp = newS(len + 1);
      memcpy(tmp, &buf[p1 + strlen_P(beginH)], len);
      return tmp;
    }
  }

  return nullptr;
}

bool FirebaseESP32::stringCompare(const char *buf, int ofs, PGM_P beginH)
{
  char *tmp = strP(beginH);
  char *tmp2 = newS(strlen_P(beginH) + 1);
  memcpy(tmp2, &buf[ofs], strlen_P(beginH));
  tmp2[strlen_P(beginH)] = 0;
  bool ret = (strcmp(tmp, tmp2) == 0);
  delS(tmp);
  delS(tmp2);
  return ret;
}

int FirebaseESP32::readLine(WiFiClient *stream, char *buf, int bufLen)
{
  int res = -1;
  char c = 0;
  int idx = 0;
  while (stream->available() && idx <= bufLen)
  {
    res = stream->read();
    if (res > -1)
    {
      c = (char)res;
      strcat_c(buf, c);
      idx++;
      if (c == '\n')
        return idx;
    }
  }
  return idx;
}

int FirebaseESP32::readChunkedData(WiFiClient *stream, char *out, int &chunkState, int &chunkedSize, int &dataLen, int bufLen)
{

  char *tmp = nullptr;
  char *buf = nullptr;
  int p1 = 0;
  int olen = 0;

  if (chunkState == 0)
  {
    chunkState = 1;
    chunkedSize = -1;
    dataLen = 0;
    buf = newS(bufLen);
    int readLen = readLine(stream, buf, bufLen);
    if (readLen)
    {
      tmp = strP(fb_esp_pgm_str_79);
      p1 = strpos(buf, tmp, 0);
      delS(tmp);
      if (p1 == -1)
      {
        tmp = strP(fb_esp_pgm_str_21);
        p1 = strpos(buf, tmp, 0);
        delS(tmp);
      }

      if (p1 != -1)
      {
        tmp = newS(p1 + 1);
        memcpy(tmp, buf, p1);
        chunkedSize = hex2int(tmp);
        delS(tmp);
      }

      //last chunk
      if (chunkedSize < 1)
        olen = -1;
    }
    else
      chunkState = 0;

    delS(buf);
  }
  else
  {

    if (chunkedSize > -1)
    {
      buf = newS(bufLen);
      int readLen = readLine(stream, buf, bufLen);

      if (readLen > 0)
      {
        //chunk may contain trailing
        if (dataLen + readLen - 2 < chunkedSize)
        {
          dataLen += readLen;
          memcpy(out, buf, readLen);
          olen = readLen;
        }
        else
        {
          if (chunkedSize - dataLen > 0)
            memcpy(out, buf, chunkedSize - dataLen);
          dataLen = chunkedSize;
          chunkState = 0;
          olen = readLen;
        }
      }
      else
      {
        olen = -1;
      }

      delS(buf);
    }
  }

  return olen;
}

bool FirebaseESP32::waitResponse(FirebaseData &fbdo)
{

  if (processing && fbdo._isStream)
    return true;

  processing = true;
  bool ret = handleResponse(fbdo);
  processing = false;

  return ret;
}

void FirebaseESP32::checkOvf(FirebaseData &fbdo, size_t len, server_response_data_t &resp)
{
  if (fbdo._responseBufSize < len && !fbdo._bufOvf)
  {
    if (fbdo._req_method == fb_esp_method::m_get && !fbdo._isDataTimeout && !fbdo._isFCM && resp.dataType != fb_esp_data_type::d_file && fbdo._req_method != fb_esp_method::m_download && fbdo._req_dataType != fb_esp_data_type::d_file)
    {
      fbdo._bufOvf = true;
      fbdo._httpCode = FIREBASE_ERROR_BUFFER_OVERFLOW;
    }
  }
}

bool FirebaseESP32::handleResponse(FirebaseData &fbdo)
{

  if (fbdo._pause)
    return true;

  if (!reconnect(fbdo))
    return false;

  WiFiClient *stream = fbdo.httpClient.stream();

  if (!fbdo._httpConnected || stream == nullptr)
  {
    fbdo._httpCode = FIREBASE_ERROR_HTTPC_ERROR_NOT_CONNECTED;
    return false;
  }

  unsigned long dataTime = millis();

  char *pChunk = nullptr;
  char *tmp = nullptr;
  char *header = nullptr;
  char *payload = nullptr;
  bool isHeader = false;

  server_response_data_t response;

  int chunkIdx = 0;
  int pChunkIdx = 0;
  int payloadLen = fbdo._responseBufSize;
  int pBufPos = 0;
  int hBufPos = 0;
  int chunkBufSize = stream->available();
  int hstate = 0;
  int pstate = 0;
  bool redirect = false;
  int chunkedDataState = 0;
  int chunkedDataSize = 0;
  int chunkedDataLen = 0;
  int defaultChunkSize = fbdo._responseBufSize;

  fbdo._httpCode = FIREBASE_ERROR_HTTP_CODE_OK;
  fbdo._contentLength = -1;
  fbdo._pushName.clear();
  fbdo._mismatchDataType = false;
  fbdo._isChunkedEnc = false;
  fbdo._bufOvf = false;

  if (fbdo._isFCM)
    defaultChunkSize = 768;

  if (!fbdo._isStream)
  {
    while (fbdo._httpConnected && chunkBufSize <= 0)
    {
      if (!reconnect(fbdo, dataTime) || stream == nullptr)
      {
        fbdo._httpCode = FIREBASE_ERROR_HTTPC_ERROR_NOT_CONNECTED;
        return false;
      }
      chunkBufSize = stream->available();
      delay(0);
    }
  }

  dataTime = millis();

  if (chunkBufSize > 1)
  {
    while (chunkBufSize > 0)
    {

      if (!reconnect(fbdo, dataTime) || stream == nullptr)
      {
        fbdo._httpCode = FIREBASE_ERROR_HTTPC_ERROR_NOT_CONNECTED;
        return false;
      }

      chunkBufSize = stream->available();

      if (chunkBufSize <= 0)
        break;

      if (chunkBufSize > 0)
      {
        if (!fbdo._isFCM)
        {
          if (pChunkIdx == 0)
          {
            if (chunkBufSize > defaultChunkSize + strlen_P(fb_esp_pgm_str_93))
              chunkBufSize = defaultChunkSize + strlen_P(fb_esp_pgm_str_93); //plus file header length for later base64 decoding
          }
          else
          {
            if (chunkBufSize > defaultChunkSize)
              chunkBufSize = defaultChunkSize;
          }
        }
        else
        {
          chunkBufSize = defaultChunkSize;
        }

        if (chunkIdx == 0)
        {
          //the first chunk can be stream event data (no header) or http response header
          header = newS(chunkBufSize);
          hstate = 1;
          int readLen = readLine(stream, header, chunkBufSize);
          int pos = 0;

          response.noEvent = !fbdo._isStream;

          tmp = getHeader(header, fb_esp_pgm_str_5, fb_esp_pgm_str_6, pos, 0);
          delay(0);
          dataTime = millis();
          if (tmp)
          {
            //http response header with http response code
            isHeader = true;
            hBufPos = readLen;
            response.httpCode = atoi(tmp);
            fbdo._httpCode = response.httpCode;
            delS(tmp);
          }
          else
          {
            //stream payload data
            payload = newS(payloadLen);
            pstate = 1;
            memcpy(payload, header, readLen);
            pBufPos = readLen;
            delS(header);
            hstate = 0;
          }
        }
        else
        {
          delay(0);
          dataTime = millis();
          //the next chunk data can be the remaining http header
          if (isHeader)
          {
            //read one line of next header field until the empty header has found
            tmp = newS(chunkBufSize);
            int readLen = readLine(stream, tmp, chunkBufSize);
            bool headerEnded = false;

            //check is it the end of http header (\n or \r\n)?
            if (readLen == 1)
              if (tmp[0] == '\r')
                headerEnded = true;

            if (readLen == 2)
              if (tmp[0] == '\r' && tmp[1] == '\n')
                headerEnded = true;

            if (headerEnded)
            {
              //parse header string to get the header field
              isHeader = false;
              parseRespHeader(header, response);
              fbdo._resp_etag = response.etag;

              if (hstate == 1)
                delS(header);
              hstate = 0;

              //error in request or server
              if (response.httpCode >= 400)
              {
                if (strposP(response.contentType.c_str(), fb_esp_pgm_str_74, 0) < 0)
                {
                  closeHTTP(fbdo);
                  break;
                }
              }

              if (fbdo._httpCode == FIREBASE_ERROR_HTTP_CODE_TEMPORARY_REDIRECT || fbdo._httpCode == FIREBASE_ERROR_HTTP_CODE_PERMANENT_REDIRECT || fbdo._httpCode == FIREBASE_ERROR_HTTP_CODE_MOVED_PERMANENTLY || fbdo._httpCode == FIREBASE_ERROR_HTTP_CODE_FOUND)
              {
                if (response.location.length() > 0)
                {
                  fbdo._redirectURL = response.location;
                  redirect = true;
                  if (fbdo._httpCode == FIREBASE_ERROR_HTTP_CODE_TEMPORARY_REDIRECT || fbdo._httpCode == FIREBASE_ERROR_HTTP_CODE_FOUND)
                    fbdo._redirect = 1;
                  else
                    fbdo._redirect = 2;
                }
              }

              if (stringCompare(response.connection.c_str(), 0, fb_esp_pgm_str_11))
                fbdo._keepAlive = true;
              else
                fbdo._keepAlive = false;

              fbdo._isChunkedEnc = response.isChunkedEnc;

              if (fbdo._req_method == fb_esp_method::m_download)
                fbdo._backupzFileSize = response.contentLen;
            }
            else
            {
              //accumulate the remaining header field
              memcpy(header + hBufPos, tmp, readLen);
              hBufPos += readLen;
            }
            delS(tmp);
          }
          else
          {

            //the next chuunk data is the payload
            if (!response.noContent && !fbdo._bufOvf)
            {
              pChunkIdx++;

              pChunk = newS(chunkBufSize + 1);

              if (!payload || pstate == 0)
              {
                pstate = 1;
                payload = newS(payloadLen + 1);
              }

              //read the avilable data
              int readLen = 0;

              //chunk transfer encoding?
              if (response.isChunkedEnc)
                readLen = readChunkedData(stream, pChunk, chunkedDataState, chunkedDataSize, chunkedDataLen, chunkBufSize);
              else
                readLen = stream->readBytes(pChunk, chunkBufSize);

              if (readLen > 0)
              {
                checkOvf(fbdo, pBufPos + readLen, response);
                if (!fbdo._bufOvf)
                {
                  if (pBufPos + readLen <= payloadLen)
                    memcpy(payload + pBufPos, pChunk, readLen);
                  else
                  {
                    //in case of the accumulated payload size is bigger than the char array
                    //reallocate the char array

                    char *buf = newS(pBufPos + readLen + 1);
                    memcpy(buf, payload, pBufPos);

                    memcpy(buf + pBufPos, pChunk, readLen);

                    payloadLen = pBufPos + readLen;
                    delS(payload);
                    payload = newS(payloadLen + 1);
                    memcpy(payload, buf, payloadLen);
                    delS(buf);
                  }
                }
              }

              if (!fbdo._isDataTimeout && !fbdo._isFCM && !fbdo._bufOvf)
              {

                //try to parse the payload for stream event data
                if (response.dataType == 0 || (response.isEvent && !response.hasEventData))
                {
                  if (fbdo._req_dataType == fb_esp_data_type::d_blob || fbdo._req_method == fb_esp_method::m_download || (fbdo._req_dataType == fb_esp_data_type::d_file && fbdo._req_method == fb_esp_method::m_get))
                    parseRespPayload(payload, response, true);
                  else
                    parseRespPayload(payload, response, false);

                  fbdo._fbError = response.fbError;
                  if (fbdo._req_method == fb_esp_method::m_download || fbdo._req_method == fb_esp_method::m_restore)
                    fbdo._file_transfer_error = response.fbError;

                  if (fbdo._req_method == fb_esp_method::m_download && response.dataType != fb_esp_data_type::d_json)
                  {
                    fbdo._httpCode = FIREBASE_ERROR_EXPECTED_JSON_DATA;
                    tmp = strP(fb_esp_pgm_str_185);
                    fbdo._file_transfer_error = tmp;
                    delS(tmp);

                    if (hstate == 1)
                      delS(header);
                    if (pstate == 1)
                      delS(payload);
                    closeFileHandle(fbdo);
                    closeHTTP(fbdo);
                    return false;
                  }
                }

                //in case of the payload data type is file, decode and write stream to temp file
                if (response.dataType == fb_esp_data_type::d_file || (fbdo._req_method == fb_esp_method::m_download || (fbdo._req_dataType == fb_esp_data_type::d_file && fbdo._req_method == fb_esp_method::m_get)))
                {
                  int ofs = 0;
                  int len = readLen;

                  if (fbdo._req_method == fb_esp_method::m_download || (fbdo._req_dataType == fb_esp_data_type::d_file && fbdo._req_method == fb_esp_method::m_get))
                  {

                    if (fbdo._fileName.length() == 0)
                    {
                      file.write((uint8_t *)payload, readLen);
                    }
                    else
                    {

                      if (pChunkIdx == 1)
                      {
                        len = payloadLen - response.payloadOfs;
                        ofs = response.payloadOfs;
                      }

                      if (payload[len - 1] == '"')
                        payload[len - 1] = 0;

                      if (fbdo._storageType == StorageType::FLASH)
                        decodeBase64Flash(file, payload + ofs, len);
                      else if (fbdo._storageType == StorageType::SD)
                        decodeBase64Stream(file, payload + ofs, len);
                    }
                  }
                  else
                  {
                    if (!file)
                    {
                      tmp = strP(fb_esp_pgm_str_184);
                      if (FLASH_FS.begin(true))
                        file = FLASH_FS.open(tmp, FILE_WRITE);
                      delS(tmp);

                      readLen = payloadLen - response.payloadOfs;
                      ofs = response.payloadOfs;
                    }

                    if (file)
                    {
                      if (payload[len - 1] == '"')
                        payload[len - 1] = 0;

                      if (fbdo._storageType == StorageType::FLASH)
                        decodeBase64Flash(file, payload + ofs, len);
                      else if (fbdo._storageType == StorageType::SD)
                        decodeBase64Stream(file, payload + ofs, len);
                    }
                  }

                  if (response.dataType > 0)
                  {
                    if (pstate == 1)
                      delS(payload);
                    pstate = 0;
                    pBufPos = 0;
                    readLen = 0;
                  }
                }
              }

              delS(pChunk);
              if (readLen < 0)
                break;
              pBufPos += readLen;
            }
            else
            {
              //read all the rest data
              while (stream->available() > 0)
                stream->read();
            }
          }
        }

        chunkIdx++;
      }
    }

    if (hstate == 1)
      delS(header);

    if (!fbdo._isDataTimeout && !fbdo._isFCM && !fbdo._bufOvf)
    {

      //parse the payload
      if (payload && !response.noContent)
      {

        //the payload ever parsed?
        if (response.dataType == 0 || (response.isEvent && !response.hasEventData))
        {
          if (fbdo._req_dataType == fb_esp_data_type::d_blob || fbdo._req_method == fb_esp_method::m_download || (fbdo._req_dataType == fb_esp_data_type::d_file && fbdo._req_method == fb_esp_method::m_get))
            parseRespPayload(payload, response, true);
          else
            parseRespPayload(payload, response, false);

          fbdo._fbError = response.fbError;
        }

        fbdo.resp_dataType = response.dataType;
        fbdo._contentLength = response.payloadLen;

        fbdo._json.clear();
        fbdo._jsonArr.clear();

        if (fbdo.resp_dataType == fb_esp_data_type::d_blob)
        {
          std::vector<uint8_t>().swap(fbdo._blob);
          fbdo._data.clear();
          fbdo._data2.clear();
          decodeBase64Str((const char *)payload + response.payloadOfs, fbdo._blob);
        }

        if (fbdo.resp_dataType == fb_esp_data_type::d_file || file)
        {
          closeFileHandle(fbdo);
          fbdo._data.clear();
          fbdo._data2.clear();
        }

        if (fbdo._httpCode == FIREBASE_ERROR_HTTP_CODE_OK || fbdo._httpCode == FIREBASE_ERROR_HTTP_CODE_PRECONDITION_FAILED || response.isEvent)
        {

          if (fbdo._req_method == fb_esp_method::m_set_rules)
          {
            if (stringCompare(payload, 0, fb_esp_pgm_str_104))
            {
              if (pstate == 1)
                delS(payload);
              pstate = 0;
            }
          }

          //stream payload?
          if (response.isEvent)
          {

            if (stringCompare(response.eventType.c_str(), 0, fb_esp_pgm_str_15) || stringCompare(response.eventType.c_str(), 0, fb_esp_pgm_str_16))
            {
              handlePayload(fbdo, response, payload);

              response.eventData.clear();

              //Any stream update?
              //based on BLOB or File event data changes (no old data available for comparision or inconvenient for large data)
              //event path changes
              //event data changes without the path changes
              if (fbdo.resp_dataType == fb_esp_data_type::d_blob ||
                  fbdo.resp_dataType == fb_esp_data_type::d_file ||
                  response.eventPathChanged ||
                  (!response.eventPathChanged && response.dataChanged && !fbdo._streamPathChanged))
              {
                fbdo._streamDataChanged = true;
                fbdo._data2.clear();
                fbdo._data2 = fbdo._data;
              }
              else
                fbdo._streamDataChanged = false;

              fbdo._dataAvailable = true;
              fbdo._streamPathChanged = false;
            }
            else
            {
              //Firebase keep alive event
              if (stringCompare(response.eventType.c_str(), 0, fb_esp_pgm_str_11))
              {
                if (fbdo._timeoutCallback)
                  fbdo._timeoutCallback(false);
              }

              //Firebase cancel and auth_revoked events
              else if (stringCompare(response.eventType.c_str(), 0, fb_esp_pgm_str_109) || stringCompare(response.eventType.c_str(), 0, fb_esp_pgm_str_110))
              {

                fbdo._eventType = response.eventType;
                //make stream available status
                fbdo._streamDataChanged = true;
                fbdo._dataAvailable = true;
              }
            }
          }
          else if (fbdo._req_method != fb_esp_method::m_set_rules)
          {
            if (fbdo.resp_dataType != fb_esp_data_type::d_blob && fbdo.resp_dataType != fb_esp_data_type::d_file && payload)
            {
              handlePayload(fbdo, response, payload);

              if (fbdo._priority_val_flag)
              {
                char *path = newS(fbdo._path.length());
                strncpy(path, fbdo._path.c_str(), fbdo._path.length() - strlen_P(fb_esp_pgm_str_156));
                fbdo._path = path;
                delS(path);
              }

              //Push (POST) data?
              if (fbdo._req_method == fb_esp_method::m_post)
              {
                if (response.pushName.length() > 0)
                {
                  fbdo._pushName = response.pushName;
                  fbdo.resp_dataType = fb_esp_data_type::d_any;
                  fbdo._data.clear();
                }
              }
            }
          }
        }
      }

      if (fbdo._reqNoContent || response.noContent)
      {
        if (fbdo._httpCode == FIREBASE_ERROR_HTTP_CODE_NO_CONTENT)
        {
          fbdo._httpCode = FIREBASE_ERROR_HTTP_CODE_OK;
          fbdo._path.clear();
          fbdo._data.clear();
          fbdo._json.clear();
          fbdo._jsonArr.clear();
          fbdo._jsonData.stringValue.clear();
          fbdo._jsonData.success = false;
          fbdo._jsonData.boolValue = false;
          fbdo._jsonData.intValue = 0;
          fbdo._jsonData.floatValue = 0;
          fbdo._jsonData.doubleValue = 0;
          fbdo._pushName.clear();
          fbdo.resp_dataType = fb_esp_data_type::d_any;
          fbdo._dataAvailable = false;
        }
      }

      if (stringCompare(fbdo._resp_etag.c_str(), 0, fb_esp_pgm_str_151) && response.noContent)
      {
        fbdo._httpCode = FIREBASE_ERROR_PATH_NOT_EXIST;
        fbdo._pathNotExist = true;
      }
      else
        fbdo._pathNotExist = false;

      if (fbdo.resp_dataType != fb_esp_data_type::d_null && fbdo._req_dataType != fb_esp_data_type::d_timestamp && !response.noContent && fbdo._req_method != fb_esp_method::m_post && fbdo._req_method != fb_esp_method::m_get_shallow && response.httpCode < 400)
      {

        bool _reqType = fbdo._req_dataType == fb_esp_data_type::d_integer || fbdo._req_dataType == fb_esp_data_type::d_float || fbdo._req_dataType == fb_esp_data_type::d_double;
        bool _respType = fbdo.resp_dataType == fb_esp_data_type::d_integer || fbdo.resp_dataType == fb_esp_data_type::d_float || fbdo.resp_dataType == fb_esp_data_type::d_double;

        if (fbdo._req_dataType == fbdo.resp_dataType || (_reqType && _respType))
          fbdo._mismatchDataType = false;
        else if (fbdo._req_dataType != fb_esp_data_type::d_any)
        {
          fbdo._mismatchDataType = true;
          fbdo._httpCode = FIREBASE_ERROR_DATA_TYPE_MISMATCH;
        }
      }
    }

    if (fbdo._isStream)
    {
      fbdo._dataMillis = millis();
      fbdo._isDataTimeout = false;
    }
    else if (!fbdo._isFCM)
    {
      closeFileHandle(fbdo);
    }

    if (fbdo._isFCM && payload)
      fbdo._data = payload;

    if (pstate == 1)
      delS(payload);

    if (!redirect)
    {

      if (fbdo._redirect == 1 && fbdo._redirectCount > 1)
        fbdo._redirectURL.clear();
    }
    else
    {

      fbdo._redirectCount++;

      if (fbdo._redirectCount > MAX_REDIRECT)
      {
        fbdo._redirect = 0;
        fbdo._httpCode = FIREBASE_ERROR_HTTPC_MAX_REDIRECT_REACHED;
      }
      else
      {
        struct fb_esp_url_info_t uinfo;
        getUrlInfo(fbdo._redirectURL, uinfo);
        if (sendRequest(fbdo, uinfo.uri, fbdo._req_method, fbdo._req_dataType, "", fbdo._priority) == 0)
          return waitResponse(fbdo);
      }
    }

    return fbdo._httpCode == FIREBASE_ERROR_HTTP_CODE_OK;
  }
  else
  {

    while (stream->available() > 0)
      stream->read();

    if (!fbdo._isStream)
    {
      fbdo._httpCode = FIREBASE_ERROR_SSL_RX_BUFFER_SIZE_TOO_SMALL;
      if (fbdo._req_method == fb_esp_method::m_get && fbdo._req_dataType == fb_esp_data_type::d_file)
        Firebase.errorToString(fbdo._httpCode, fbdo._file_transfer_error);

      return false;
    }
  }

  if (fbdo._isStream && fbdo._httpConnected)
  {
    fbdo._httpCode = FIREBASE_ERROR_HTTP_CODE_OK;
    return true;
  }
  else
  {
    fbdo._httpCode = FIREBASE_ERROR_HTTPC_ERROR_NOT_CONNECTED;
    return false;
  }
}

void FirebaseESP32::handlePayload(FirebaseData &fbdo, server_response_data_t &response, char *payload)
{

  String rawArr;
  String rawJson;

  fbdo._json.toString(rawJson);
  fbdo._jsonArr.toString(rawArr);

  fbdo._data.clear();
  fbdo._jsonData.stringValue.clear();
  fbdo._json.clear();
  fbdo._jsonArr.clear();

  if (response.isEvent)
  {
    response.eventPathChanged = strcmp(response.eventPath.c_str(), fbdo._path.c_str()) != 0;
    fbdo._path = response.eventPath;
    fbdo._eventType = response.eventType;
  }

  if (fbdo.resp_dataType == fb_esp_data_type::d_json)
  {
    if (response.isEvent)
      fbdo._json.setJsonData(response.eventData.c_str());
    else
      fbdo._json.setJsonData(payload);
    String test;
    fbdo._json.toString(test);
    if (test != rawJson)
      response.dataChanged = true;
    test.clear();
  }
  else if (fbdo.resp_dataType == fb_esp_data_type::d_array)
  {
    if (response.isEvent)
      fbdo._jsonArr.setJsonArrayData(response.eventData.c_str());
    else
      fbdo._jsonArr.setJsonArrayData(payload);
    String test;
    fbdo._jsonArr.toString(test);
    if (test != rawArr)
      response.dataChanged = true;
    test.clear();
  }
  else if (fbdo.resp_dataType != fb_esp_data_type::d_blob && fbdo.resp_dataType != fb_esp_data_type::d_file)
  {
    if (response.isEvent)
      fbdo._data = response.eventData;
    else
      fbdo._data = payload;

    response.dataChanged = fbdo._data2 != fbdo._data;
  }

  rawJson.clear();
  rawArr.clear();
  fbdo._data2.clear();
}

void FirebaseESP32::closeFileHandle(FirebaseData &fbdo)
{
  if (file)
    file.close();
  if (fbdo._storageType == StorageType::SD)
  {
    _sdInUse = false;
    _sdOk = false;
    SD.end();
  }
}

bool FirebaseESP32::validRequest(FirebaseData &fbdo, const std::string &path)
{
  if (path.length() == 0 || _cfg->host.length() == 0 || (_cfg->signer.tokens.legacy_token.length() == 0 && _cfg->signer.tokens.id_token.length() == 0 && _cfg->signer.tokens.access_token.length() == 0))
  {
    fbdo._httpCode = FIREBASE_ERROR_HTTP_CODE_BAD_REQUEST;
    return false;
  }
  return true;
}

bool FirebaseESP32::tokenReady(FirebaseData &fbdo)
{
  if (!_auth || !_cfg)
  {
    fbdo._httpCode = FIREBASE_ERROR_UNINITIALIZED;
    return false;
  }
  checkToken(fbdo);

  if (_cfg->signer.tokens.status != token_status_ready)
  {
    fbdo._httpCode = FIREBASE_ERROR_TOKEN_NOT_READY;
    closeHTTP(fbdo);
    return false;
  }
  return true;
};

bool FirebaseESP32::handleStreamRequest(FirebaseData &fbdo, const std::string &path)
{

  if (fbdo._pause)
    return true;

  if (!reconnect(fbdo))
    return false;

  if (!tokenReady(fbdo))
    return false;

  if (!validRequest(fbdo, path))
    return false;

  bool ret = false;

  if (fbdo._redirectURL.length() > 0)
  {
    struct fb_esp_url_info_t uinfo;
    getUrlInfo(fbdo._redirectURL, uinfo);
    ret = sendRequest(fbdo, uinfo.uri, fb_esp_method::m_stream, fb_esp_data_type::d_string, "", "") == 0;
  }
  else
    ret = sendRequest(fbdo, path, fb_esp_method::m_stream, fb_esp_data_type::d_string, "", "") == 0;

  if (!ret)
    fbdo._httpCode = FIREBASE_ERROR_HTTPC_ERROR_NOT_CONNECTED;

  return ret;
}

bool FirebaseESP32::handleStreamRead(FirebaseData &fbdo)
{

  if (fbdo._pause || fbdo._streamStop)
    return true;

  if (!reconnect(fbdo))
  {
    fbdo._isDataTimeout = true;
    return true;
  }

  if (!tokenReady(fbdo))
    return true;

  bool ret = false;
  bool reconnectStream = false;

  //trying to reconnect the stream when required at some interval as running in the loop
  if (millis() - STREAM_RECONNECT_INTERVAL > fbdo._streamReconnectMillis)
  {
    reconnectStream = (fbdo._isDataTimeout && !fbdo._httpConnected) || fbdo._isFCM;

    if (fbdo._isDataTimeout)
      closeHTTP(fbdo);
    fbdo._streamReconnectMillis = millis();
  }
  else
    ret = true;

  if (fbdo.httpClient.stream())
  {
    if (!fbdo.httpClient.stream()->connected())
      reconnectStream = true;
  }
  else
    reconnectStream = true;

  //Stream timeout
  if (millis() - fbdo._dataMillis > KEEP_ALIVE_TIMEOUT)
  {
    fbdo._dataMillis = millis();
    fbdo._isDataTimeout = true;
    reconnectStream = true;
  }

  if (reconnectStream)
  {
    if (!waitIdle(fbdo))
      return false;

    closeHTTP(fbdo);

    if (!tokenReady(fbdo))
      return false;

    std::string path = fbdo._streamPath;
    if (fbdo._redirectURL.length() > 0)
    {
      struct fb_esp_url_info_t uinfo;
      getUrlInfo(fbdo._redirectURL, uinfo);
      path = uinfo.uri;
    }

    //mode changed, non-stream -> stream, reset the data timeout state to allow data available to be notified.
    fbdo._isDataTimeout = false;

    if (!handleStreamRequest(fbdo, path))
    {
      fbdo._isDataTimeout = true;
      return ret;
    }

    fbdo._isFCM = false;
  }

  if (!waitResponse(fbdo))
  {
    fbdo._isDataTimeout = true;
    if (!fbdo._httpConnected) //not connected in fcm shared object usage
      return true;
    else
      return ret;
  }

  return true;
}

void FirebaseESP32::closeHTTP(FirebaseData &fbdo)
{
  //close the socket and free the resources used by the mbedTLS data
  if (fbdo._httpConnected)
  {
    if (fbdo.httpClient.stream())
    {
      if (fbdo.httpClient.stream()->connected())
        fbdo.httpClient.stream()->stop();
    }
    _lastReconnectMillis = millis();
  }
  if (fbdo._isStream)
  {
    fbdo._streamTimeoutMillis = millis();
    fbdo._dataMillis = millis();
    fbdo._isDataTimeout = false;
  }
  fbdo._httpConnected = false;
}

void FirebaseESP32::processErrorQueue(FirebaseData &fbdo, QueueInfoCallback callback)
{

  if (!reconnect(fbdo))
    return;

  if (fbdo._qMan._queueCollection.size() > 0)
  {

    for (uint8_t i = 0; i < fbdo._qMan._queueCollection.size(); i++)
    {
      QueueItem item = fbdo._qMan._queueCollection[i];

      if (callback)
      {
        QueueInfo qinfo;
        qinfo._isQueue = true;
        qinfo._dataType = fbdo.getDataType(item.dataType);
        qinfo._path = item.path;
        qinfo._currentQueueID = item.qID;
        qinfo._method = fbdo.getMethod(item.method);
        qinfo._totalQueue = fbdo._qMan._queueCollection.size();
        qinfo._isQueueFull = fbdo._qMan._queueCollection.size() == fbdo._qMan._maxQueue;
        callback(qinfo);
      }

      if (item.method == fb_esp_method::m_get)
      {

        switch (item.dataType)
        {

        case fb_esp_data_type::d_integer:

          if (Firebase.getInt(fbdo, item.path.c_str()))
          {

            if (item.intPtr)
              *item.intPtr = fbdo.intData();

            fbdo.clearQueueItem(item);
            fbdo._qMan.remove(i);
          }

          break;

        case fb_esp_data_type::d_float:

          if (Firebase.getFloat(fbdo, item.path.c_str()))
          {

            if (item.floatPtr)
              *item.floatPtr = fbdo.floatData();

            fbdo.clearQueueItem(item);
            fbdo._qMan.remove(i);
          }

          break;

        case fb_esp_data_type::d_double:

          if (Firebase.getDouble(fbdo, item.path.c_str()))
          {

            if (item.doublePtr)
              *item.doublePtr = fbdo.doubleData();

            fbdo.clearQueueItem(item);
            fbdo._qMan.remove(i);
          }

          break;

        case fb_esp_data_type::d_boolean:

          if (Firebase.getBool(fbdo, item.path.c_str()))
          {

            if (item.boolPtr)
              *item.boolPtr = fbdo.boolData();

            fbdo.clearQueueItem(item);
            fbdo._qMan.remove(i);
          }

          break;

        case fb_esp_data_type::d_string:

          if (Firebase.getString(fbdo, item.path.c_str()))
          {

            if (item.stringPtr)
              *item.stringPtr = fbdo.stringData();

            fbdo.clearQueueItem(item);
            fbdo._qMan.remove(i);
          }

          break;

        case fb_esp_data_type::d_json:

          if (item.queryFilter._orderBy.length() > 0)
          {
            if (Firebase.getJSON(fbdo, item.path.c_str(), item.queryFilter))
            {

              if (item.jsonPtr)
                item.jsonPtr = fbdo.jsonObjectPtr();

              fbdo.clearQueueItem(item);
              fbdo._qMan.remove(i);
            }
          }
          else
          {
            if (Firebase.getJSON(fbdo, item.path.c_str()))
            {

              if (item.jsonPtr)
                item.jsonPtr = fbdo.jsonObjectPtr();

              fbdo.clearQueueItem(item);
              fbdo._qMan.remove(i);
            }
          }

          break;
        case fb_esp_data_type::d_array:

          if (item.queryFilter._orderBy.length() > 0)
          {
            if (Firebase.getArray(fbdo, item.path.c_str(), item.queryFilter))
            {

              if (item.arrPtr)
                item.arrPtr = fbdo.jsonArrayPtr();

              fbdo.clearQueueItem(item);
              fbdo._qMan.remove(i);
            }
          }
          else
          {
            if (Firebase.getArray(fbdo, item.path.c_str()))
            {
              if (item.arrPtr)
                item.arrPtr = fbdo.jsonArrayPtr();

              fbdo.clearQueueItem(item);
              fbdo._qMan.remove(i);
            }
          }

          break;

        case fb_esp_data_type::d_blob:

          if (Firebase.getBlob(fbdo, item.path.c_str()))
          {
            if (item.blobPtr)
              *item.blobPtr = fbdo.blobData();

            fbdo.clearQueueItem(item);
            fbdo._qMan.remove(i);
          }

          break;

        case fb_esp_data_type::d_file:

          if (Firebase.getFile(fbdo, item.storageType, item.path.c_str(), item.filename.c_str()))
          {
            fbdo.clearQueueItem(item);
            fbdo._qMan.remove(i);
          }

          break;
        case fb_esp_data_type::d_any:

          if (Firebase.get(fbdo, item.path.c_str()))
          {
            fbdo.clearQueueItem(item);
            fbdo._qMan.remove(i);
          }

          break;

        default:
          break;
        }
      }
      else if (item.method == fb_esp_method::m_post || item.method == fb_esp_method::m_put || item.method == fb_esp_method::m_put_nocontent || item.method == fb_esp_method::m_patch || item.method == fb_esp_method::m_patch_nocontent)
      {

        if (item.dataType == fb_esp_data_type::d_file)
        {
          if (processRequestFile(fbdo, item.storageType, item.method, item.path.c_str(), item.filename.c_str(), true, "", ""))
          {
            fbdo.clearQueueItem(item);
            fbdo._qMan.remove(i);
          }
        }
        else
        {
          if (processRequest(fbdo, item.method, item.dataType, item.path.c_str(), item.payload.c_str(), true, "", ""))
          {
            fbdo.clearQueueItem(item);
            fbdo._qMan.remove(i);
          }
        }
      }
    }
  }
}

uint32_t FirebaseESP32::getErrorQueueID(FirebaseData &fbdo)
{
  return fbdo._qID;
}

bool FirebaseESP32::isErrorQueueExisted(FirebaseData &fbdo, uint32_t errorQueueID)
{

  for (uint8_t i = 0; i < fbdo._qMan._queueCollection.size(); i++)
  {
    QueueItem q = fbdo._qMan._queueCollection[i];
    if (q.qID == errorQueueID)
      return true;
  }
  return false;
}

void FirebaseESP32::preparePayload(fb_esp_method method, fb_esp_data_type dataType, const std::string &priority, const std::string &payload, std::string &buf)
{
  char *tmp = nullptr;

  if (method != fb_esp_method::m_get && method != fb_esp_method::m_read_rules && method != fb_esp_method::m_get_shallow && method != fb_esp_method::m_get_priority && method != fb_esp_method::m_get_nocontent && method != fb_esp_method::m_stream &&
      method != fb_esp_method::m_delete && method != fb_esp_method::m_restore)
  {

    if (priority.length() > 0)
    {
      if (dataType == fb_esp_data_type::d_json)
      {
        if (payload.length() > 0)
        {
          buf.clear();
          buf = payload;
          tmp = strP(fb_esp_pgm_str_127);
          size_t x = buf.find_last_of(tmp);
          delS(tmp);
          if (x != std::string::npos && x != 0)
            for (size_t i = 0; i < buf.length() - x; i++)
              buf.pop_back();

          appendP(buf, fb_esp_pgm_str_157);
          buf += priority;
          appendP(buf, fb_esp_pgm_str_127);
        }
      }
      else
      {
        appendP(buf, fb_esp_pgm_str_161, true);
        if (dataType == fb_esp_data_type::d_string)
          appendP(buf, fb_esp_pgm_str_3);
        buf += payload;
        if (dataType == fb_esp_data_type::d_string)
          appendP(buf, fb_esp_pgm_str_3);
        appendP(buf, fb_esp_pgm_str_157);
        buf += priority;
        appendP(buf, fb_esp_pgm_str_127);
      }
    }
    else
    {
      buf.clear();
      if (dataType == fb_esp_data_type::d_string)
        appendP(buf, fb_esp_pgm_str_3);
      buf += payload;
      if (dataType == fb_esp_data_type::d_string)
        appendP(buf, fb_esp_pgm_str_3);
    }
  }
}

void FirebaseESP32::prepareHeader(FirebaseData &fbdo, fb_esp_method method, fb_esp_data_type dataType, const std::string &path, int payloadLength, std::string &header, bool sv)
{
  fb_esp_method http_method = fb_esp_method::m_put;
  char *tmp = nullptr;
  fbdo._shallow_flag = false;
  fbdo._priority_val_flag = false;

  header.clear();
  if (method == fb_esp_method::m_stream)
  {
    appendP(header, fb_esp_pgm_str_22, true);
  }
  else
  {
    if (method == fb_esp_method::m_put || method == fb_esp_method::m_put_nocontent || method == fb_esp_method::m_set_priority || method == fb_esp_method::m_set_rules)
    {
      http_method = fb_esp_method::m_put;
      if (fbdo._classicRequest)
        appendP(header, fb_esp_pgm_str_24, true);
      else
        appendP(header, fb_esp_pgm_str_23, true);
    }
    else if (method == fb_esp_method::m_post)
    {
      http_method = fb_esp_method::m_post;
      appendP(header, fb_esp_pgm_str_24, true);
    }
    else if (method == fb_esp_method::m_get || method == fb_esp_method::m_get_nocontent || method == fb_esp_method::m_get_shallow || method == fb_esp_method::m_get_priority || method == fb_esp_method::m_download || method == fb_esp_method::m_read_rules)
    {
      http_method = fb_esp_method::m_get;
      appendP(header, fb_esp_pgm_str_25, true);
    }
    else if (method == fb_esp_method::m_patch || method == fb_esp_method::m_patch_nocontent || method == fb_esp_method::m_restore)
    {
      http_method = fb_esp_method::m_patch;
      appendP(header, fb_esp_pgm_str_26, true);
    }
    else if (method == fb_esp_method::m_delete)
    {
      http_method = fb_esp_method::m_delete;
      if (fbdo._classicRequest)
        appendP(header, fb_esp_pgm_str_24, true);
      else
        appendP(header, fb_esp_pgm_str_27, true);
    }

    appendP(header, fb_esp_pgm_str_6);
  }

  if (path.length() > 0)
    if (path[0] != '/')
      appendP(header, fb_esp_pgm_str_1);

  header += path;

  if (method == fb_esp_method::m_patch || method == fb_esp_method::m_patch_nocontent)
    appendP(header, fb_esp_pgm_str_1);

  bool appendAuth = false;

  if (fbdo._redirectURL.length() > 0)
  {
    struct fb_esp_url_info_t uinfo;
    getUrlInfo(fbdo._redirectURL, uinfo);
    if (uinfo.auth.length() == 0)
      appendAuth = true;
  }
  else
    appendAuth = true;

  if (appendAuth)
  {

    if (_cfg->signer.tokens.token_type == token_type_oauth2_access_token)
      appendP(header, fb_esp_pgm_str_238);
    else
      appendP(header, fb_esp_pgm_str_2);

    if (_cfg->signer.tokens.token_type == token_type_legacy_token)
      header += _cfg->signer.tokens.legacy_token;

    if (_cfg->signer.tokens.token_type == token_type_id_token || _cfg->signer.tokens.token_type == token_type_custom_token)
      header += _cfg->signer.tokens.id_token;
  }

  if (fbdo._readTimeout > 0)
  {
    appendP(header, fb_esp_pgm_str_158);
    tmp = intStr(fbdo._readTimeout);
    header += tmp;
    delS(tmp);
    appendP(header, fb_esp_pgm_str_159);
  }

  if (fbdo._writeLimit.length() > 0)
  {
    appendP(header, fb_esp_pgm_str_160);
    header += fbdo._writeLimit;
  }

  if (method == fb_esp_method::m_get_shallow)
  {
    appendP(header, fb_esp_pgm_str_155);
    fbdo._shallow_flag = true;
  }

  if (method == fb_esp_method::m_get && fbdo.queryFilter._orderBy.length() > 0)
  {
    appendP(header, fb_esp_pgm_str_96);
    header += fbdo.queryFilter._orderBy;

    if (method == fb_esp_method::m_get && fbdo.queryFilter._limitToFirst.length() > 0)
    {
      appendP(header, fb_esp_pgm_str_97);
      header += fbdo.queryFilter._limitToFirst;
    }

    if (method == fb_esp_method::m_get && fbdo.queryFilter._limitToLast.length() > 0)
    {
      appendP(header, fb_esp_pgm_str_98);
      header += fbdo.queryFilter._limitToLast;
    }

    if (method == fb_esp_method::m_get && fbdo.queryFilter._startAt.length() > 0)
    {
      appendP(header, fb_esp_pgm_str_99);
      header += fbdo.queryFilter._startAt;
    }

    if (method == fb_esp_method::m_get && fbdo.queryFilter._endAt.length() > 0)
    {
      appendP(header, fb_esp_pgm_str_100);
      header += fbdo.queryFilter._endAt;
    }

    if (method == fb_esp_method::m_get && fbdo.queryFilter._equalTo.length() > 0)
    {
      appendP(header, fb_esp_pgm_str_101);
      header += fbdo.queryFilter._equalTo;
    }
  }

  if (method == fb_esp_method::m_download)
  {
    appendP(header, fb_esp_pgm_str_162);
    appendP(header, fb_esp_pgm_str_28);
    std::string filename = "";

    for (int i = 0; i < fbdo._backupNodePath.length(); i++)
    {
      if (fbdo._backupNodePath.c_str()[i] == '/')
        appendP(filename, fb_esp_pgm_str_4);
      else
        filename += fbdo._backupNodePath.c_str()[i];
    }

    header += filename;
    std::string().swap(filename);
  }

  if (method == fb_esp_method::m_get && fbdo._fileName.length() > 0)
  {
    appendP(header, fb_esp_pgm_str_28);
    header += fbdo._fileName;
  }

  if (method == fb_esp_method::m_get_nocontent || method == fb_esp_method::m_restore || method == fb_esp_method::m_put_nocontent || method == fb_esp_method::m_patch_nocontent)
    appendP(header, fb_esp_pgm_str_29);

  appendP(header, fb_esp_pgm_str_30);
  appendP(header, fb_esp_pgm_str_31);
  header += _cfg->host;
  appendP(header, fb_esp_pgm_str_21);
  appendP(header, fb_esp_pgm_str_32);

  if (_cfg->signer.tokens.token_type == token_type_oauth2_access_token)
  {
    appendP(header, fb_esp_pgm_str_237);
    header += _cfg->signer.tokens.auth_type;
    appendP(header, fb_esp_pgm_str_6);
    header += _cfg->signer.tokens.access_token;
    appendP(header, fb_esp_pgm_str_21);
  }

  //Timestamp cannot use with ETag header, otherwise cases internal server error
  if (!sv && fbdo.queryFilter._orderBy.length() == 0 && dataType != fb_esp_data_type::d_timestamp && (method == fb_esp_method::m_delete || method == fb_esp_method::m_get || method == fb_esp_method::m_get_nocontent || method == fb_esp_method::m_put || method == fb_esp_method::m_put_nocontent || method == fb_esp_method::m_post))
    appendP(header, fb_esp_pgm_str_148);

  if (fbdo._req_etag.length() > 0 && (method == fb_esp_method::m_put || method == fb_esp_method::m_put_nocontent || method == fb_esp_method::m_delete))
  {
    appendP(header, fb_esp_pgm_str_149);
    header += fbdo._req_etag;
    appendP(header, fb_esp_pgm_str_21);
  }

  if (fbdo._classicRequest && http_method != fb_esp_method::m_get && http_method != fb_esp_method::m_post && http_method != fb_esp_method::m_patch)
  {
    appendP(header, fb_esp_pgm_str_153);

    if (http_method == fb_esp_method::m_put)
      appendP(header, fb_esp_pgm_str_23);
    else if (http_method == fb_esp_method::m_delete)
      appendP(header, fb_esp_pgm_str_27);

    appendP(header, fb_esp_pgm_str_21);
  }

  if (method == fb_esp_method::m_stream)
  {
    appendP(header, fb_esp_pgm_str_34);
    appendP(header, fb_esp_pgm_str_35);
  }
  else if (method == fb_esp_method::m_download || method == fb_esp_method::m_restore)
  {
    appendP(header, fb_esp_pgm_str_34);
  }
  else
  {
    appendP(header, fb_esp_pgm_str_36);
    appendP(header, fb_esp_pgm_str_37);
  }

  if (method != fb_esp_method::m_download && method != fb_esp_method::m_restore)
    appendP(header, fb_esp_pgm_str_38);

  if (method == fb_esp_method::m_get_priority || method == fb_esp_method::m_set_priority)
    fbdo._priority_val_flag = true;

  if (method == fb_esp_method::m_put || method == fb_esp_method::m_put_nocontent || method == fb_esp_method::m_post || method == fb_esp_method::m_patch || method == fb_esp_method::m_patch_nocontent || method == fb_esp_method::m_restore || method == fb_esp_method::m_set_rules || method == fb_esp_method::m_set_priority)
  {
    appendP(header, fb_esp_pgm_str_12);
    tmp = intStr(payloadLength);
    header += tmp;
    delS(tmp);
  }
  appendP(header, fb_esp_pgm_str_21);
  appendP(header, fb_esp_pgm_str_21);
}

void FirebaseESP32::setSecure(FirebaseData &fbdo)
{
  if (fbdo.httpClient._certType == -1)
  {
    if (_cfg->cert.file.length() == 0)
    {
      if (_caCert)
        fbdo.httpClient.setCACert(_caCert.get());
      else
        fbdo.httpClient.setCACert(nullptr);
    }
    else
    {
      if (_cfg->cert.file_storage == StorageType::SD)
      {
        if (!_sdOk)
          _sdOk = sdTest();

        if (!_sdOk)
          return;
      }
      else if (_cfg->cert.file_storage == StorageType::FLASH)
      {
        if (!_flashOk)
          _flashOk = FLASH_FS.begin(true);

        if (!_flashOk)
          return;
      }

      fbdo.httpClient.setCertFile(_cfg->cert.file, _cfg->cert.file_storage);
    }
  }
}

bool FirebaseESP32::connectionError(FirebaseData &fbdo)
{
  return fbdo._httpCode == FIREBASE_ERROR_HTTPC_ERROR_CONNECTION_REFUSED || fbdo._httpCode == FIREBASE_ERROR_HTTPC_ERROR_CONNECTION_LOST ||
         fbdo._httpCode == FIREBASE_ERROR_HTTPC_ERROR_SEND_PAYLOAD_FAILED || fbdo._httpCode == FIREBASE_ERROR_HTTPC_ERROR_SEND_HEADER_FAILED ||
         fbdo._httpCode == FIREBASE_ERROR_HTTPC_ERROR_NOT_CONNECTED || fbdo._httpCode == FIREBASE_ERROR_HTTPC_ERROR_READ_TIMEOUT;
}

void FirebaseESP32::clearDataStatus(FirebaseData &fbdo)
{
  fbdo._streamDataChanged = false;
  fbdo._streamPathChanged = false;
  fbdo._dataAvailable = false;
  fbdo._isDataTimeout = false;
  fbdo._contentLength = -1;
  fbdo.resp_dataType = fb_esp_data_type::d_any;
  fbdo._httpCode = -1000;
  fbdo._data.clear();
  fbdo._data2.clear();
  fbdo._backupNodePath.clear();
  fbdo._json.clear();
  fbdo._jsonArr.clear();
  fbdo._blob.clear();
  fbdo._path.clear();
  fbdo._pushName.clear();
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

bool FirebaseESP32::reconnect(FirebaseData &fbdo, unsigned long dataTime)
{

  bool status = WiFi.status() == WL_CONNECTED;

  if (dataTime > 0)
  {
    if (millis() - dataTime > fbdo.httpClient.tcpTimeout)
    {
      fbdo._httpCode = FIREBASE_ERROR_HTTPC_ERROR_READ_TIMEOUT;
      char *tmp = strP(fb_esp_pgm_str_69);
      if (fbdo._req_method == fb_esp_method::m_download || fbdo._req_method == fb_esp_method::m_restore)
        fbdo._file_transfer_error = tmp;
      else
        fbdo._fbError = tmp;
      delS(tmp);
      closeHTTP(fbdo);
      return false;
    }
  }

  if (!status)
  {
    if (fbdo._httpConnected)
      closeHTTP(fbdo);

    fbdo._httpCode = FIREBASE_ERROR_HTTPC_ERROR_CONNECTION_LOST;

    if (_reconnectWiFi)
    {
      if (millis() - _lastReconnectMillis > _reconnectTimeout && !fbdo._httpConnected)
      {
        esp_wifi_connect();
        _lastReconnectMillis = millis();
      }
    }

    status = WiFi.status() == WL_CONNECTED;
  }

  return status;
}

void FirebaseESP32::checkToken(FirebaseData &fbdo)
{
  if (!_auth || !_cfg)
    return;

  if ((_cfg->signer.tokens.token_type == token_type_id_token || _cfg->signer.tokens.token_type == token_type_custom_token || _cfg->signer.tokens.token_type == token_type_oauth2_access_token) && (millis() > _cfg->signer.tokens.expires - preRefreshMillis || _cfg->signer.tokens.expires == 0))
  {
    closeHTTP(fbdo);
    hanldeToken();
  }
}

bool FirebaseESP32::hanldeToken()
{

  if (!_auth || !_cfg)
    return false;

  if ((_cfg->signer.tokens.token_type == token_type_id_token || _cfg->signer.tokens.token_type == token_type_custom_token || _cfg->signer.tokens.token_type == token_type_oauth2_access_token) && (millis() > _cfg->signer.tokens.expires - preRefreshMillis || _cfg->signer.tokens.expires == 0))
  {
    if (_cfg->signer.tokens.expires > 0 && (_cfg->signer.tokens.token_type == token_type_id_token || _cfg->signer.tokens.token_type == token_type_custom_token))
    {
      if (millis() - _cfg->signer.lastReqMillis > _cfg->signer.reqTO || _cfg->signer.lastReqMillis == 0)
      {
        _cfg->signer.lastReqMillis = millis();
        if (processing)
          return false;
        return refreshToken();
      }
      return false;
    }
    else
    {
      if (_cfg->signer.tokens.token_type == token_type_id_token)
      {
        if (millis() - _cfg->signer.lastReqMillis > _cfg->signer.reqTO || _cfg->signer.lastReqMillis == 0)
        {
          _cfg->signer.lastReqMillis = millis();
          if (processing)
            return false;
          return getIdToken(false, "", "");
        }
        return false;
      }
      else
      {
        if (_cfg->signer.step == fb_esp_jwt_generation_step_begin)
        {
          if (_cfg->service_account.json.path.length() > 0 && _cfg->signer.pk.length() == 0)
          {
            if (!parseSAFile())
              _cfg->signer.tokens.status = token_status_uninitialized;
          }
        }

        if (setClock(_cfg->time_zone))
        {
          setTokenError(FIREBASE_ERROR_TOKEN_NOT_READY);
          _cfg->signer.step++;
          createJWT(_cfg->signer.step);
          if (_cfg->signer.step == fb_esp_jwt_generation_step_sign)
          {
            createJWT(_cfg->signer.step);
            if (_cfg->signer.step == fb_esp_jwt_generation_step_exchange)
            {
              while (_cfg->signer.step == fb_esp_jwt_generation_step_exchange && _cfg->signer.attempts < MAX_EXCHANGE_TOKEN_ATTEMPTS)
                requestTokens();
              _cfg->signer.attempts = 0;
            }
          }
        }
        else
        {
          setTokenError(FIREBASE_ERROR_TOKEN_SET_TIME);
        }
      }
    }
  }

  if (_cfg->signer.tokens.token_type == token_type_legacy_token)
  {
    setTokenError(0);
    return true;
  }
  else
    return _cfg->signer.tokens.status == token_status_ready;
}

void FirebaseESP32::errorToString(int httpCode, std::string &buff)
{

  buff.clear();

  if (_cfg)
  {
    if (_cfg->signer.tokens.status == token_status_error || _cfg->signer.tokens.error.code != 0)
    {
      buff = _cfg->signer.tokens.error.message;
      return;
    }
  }

  switch (httpCode)
  {
  case FIREBASE_ERROR_HTTPC_ERROR_CONNECTION_REFUSED:
    appendP(buff, fb_esp_pgm_str_39);
    return;
  case FIREBASE_ERROR_HTTPC_ERROR_SEND_HEADER_FAILED:
    appendP(buff, fb_esp_pgm_str_40);
    return;
  case FIREBASE_ERROR_HTTPC_ERROR_SEND_PAYLOAD_FAILED:
    appendP(buff, fb_esp_pgm_str_41);
    return;
  case FIREBASE_ERROR_HTTPC_ERROR_NOT_CONNECTED:
    appendP(buff, fb_esp_pgm_str_42);
    return;
  case FIREBASE_ERROR_HTTPC_ERROR_CONNECTION_LOST:
    appendP(buff, fb_esp_pgm_str_43);
    return;
  case FIREBASE_ERROR_HTTPC_ERROR_NO_HTTP_SERVER:
    appendP(buff, fb_esp_pgm_str_44);
    return;
  case FIREBASE_ERROR_HTTP_CODE_BAD_REQUEST:
    appendP(buff, fb_esp_pgm_str_45);
    return;
  case FIREBASE_ERROR_HTTP_CODE_NON_AUTHORITATIVE_INFORMATION:
    appendP(buff, fb_esp_pgm_str_46);
    return;
  case FIREBASE_ERROR_HTTP_CODE_NO_CONTENT:
    appendP(buff, fb_esp_pgm_str_47);
    return;
  case FIREBASE_ERROR_HTTP_CODE_MOVED_PERMANENTLY:
    appendP(buff, fb_esp_pgm_str_48);
    return;
  case FIREBASE_ERROR_HTTP_CODE_USE_PROXY:
    appendP(buff, fb_esp_pgm_str_49);
    return;
  case FIREBASE_ERROR_HTTP_CODE_TEMPORARY_REDIRECT:
    appendP(buff, fb_esp_pgm_str_50);
    return;
  case FIREBASE_ERROR_HTTP_CODE_PERMANENT_REDIRECT:
    appendP(buff, fb_esp_pgm_str_51);
    return;
  case FIREBASE_ERROR_HTTP_CODE_UNAUTHORIZED:
    appendP(buff, fb_esp_pgm_str_52);
    return;
  case FIREBASE_ERROR_HTTP_CODE_FORBIDDEN:
    appendP(buff, fb_esp_pgm_str_53);
    return;
  case FIREBASE_ERROR_HTTP_CODE_NOT_FOUND:
    appendP(buff, fb_esp_pgm_str_54);
    return;
  case FIREBASE_ERROR_HTTP_CODE_METHOD_NOT_ALLOWED:
    appendP(buff, fb_esp_pgm_str_55);
    return;
  case FIREBASE_ERROR_HTTP_CODE_NOT_ACCEPTABLE:
    appendP(buff, fb_esp_pgm_str_56);
    return;
  case FIREBASE_ERROR_HTTP_CODE_PROXY_AUTHENTICATION_REQUIRED:
    appendP(buff, fb_esp_pgm_str_57);
    return;
  case FIREBASE_ERROR_HTTP_CODE_REQUEST_TIMEOUT:
    appendP(buff, fb_esp_pgm_str_58);
    return;
  case FIREBASE_ERROR_HTTP_CODE_LENGTH_REQUIRED:
    appendP(buff, fb_esp_pgm_str_59);
    return;
  case FIREBASE_ERROR_HTTP_CODE_TOO_MANY_REQUESTS:
    appendP(buff, fb_esp_pgm_str_60);
    return;
  case FIREBASE_ERROR_HTTP_CODE_REQUEST_HEADER_FIELDS_TOO_LARGE:
    appendP(buff, fb_esp_pgm_str_61);
    return;
  case FIREBASE_ERROR_HTTP_CODE_INTERNAL_SERVER_ERROR:
    appendP(buff, fb_esp_pgm_str_62);
    return;
  case FIREBASE_ERROR_HTTP_CODE_BAD_GATEWAY:
    appendP(buff, fb_esp_pgm_str_63);
    return;
  case FIREBASE_ERROR_HTTP_CODE_SERVICE_UNAVAILABLE:
    appendP(buff, fb_esp_pgm_str_64);
    return;
  case FIREBASE_ERROR_HTTP_CODE_GATEWAY_TIMEOUT:
    appendP(buff, fb_esp_pgm_str_65);
    return;
  case FIREBASE_ERROR_HTTP_CODE_HTTP_VERSION_NOT_SUPPORTED:
    appendP(buff, fb_esp_pgm_str_66);
    return;
  case FIREBASE_ERROR_HTTP_CODE_NETWORK_AUTHENTICATION_REQUIRED:
    appendP(buff, fb_esp_pgm_str_67);
    return;
  case FIREBASE_ERROR_HTTP_CODE_PRECONDITION_FAILED:
    appendP(buff, fb_esp_pgm_str_152);
    return;
  case FIREBASE_ERROR_HTTPC_ERROR_READ_TIMEOUT:
    appendP(buff, fb_esp_pgm_str_69);
    return;
  case FIREBASE_ERROR_DATA_TYPE_MISMATCH:
    appendP(buff, fb_esp_pgm_str_70);
    return;
  case FIREBASE_ERROR_PATH_NOT_EXIST:
    appendP(buff, fb_esp_pgm_str_71);
    return;
  case FIREBASE_ERROR_HTTPC_ERROR_CONNECTION_INUSED:
    appendP(buff, fb_esp_pgm_str_94);
    return;
  case FIREBASE_ERROR_HTTPC_MAX_REDIRECT_REACHED:
    appendP(buff, fb_esp_pgm_str_169);
    return;
  case FIREBASE_ERROR_BUFFER_OVERFLOW:
    appendP(buff, fb_esp_pgm_str_68);
    return;
  case FIREBASE_ERROR_HTTPC_NO_FCM_TOPIC_PROVIDED:
    appendP(buff, fb_esp_pgm_str_144);
    return;
  case FIREBASE_ERROR_HTTPC_NO_FCM_DEVICE_TOKEN_PROVIDED:
    appendP(buff, fb_esp_pgm_str_145);
    return;
  case FIREBASE_ERROR_HTTPC_NO_FCM_SERVER_KEY_PROVIDED:
    appendP(buff, fb_esp_pgm_str_146);
    return;
  case FIREBASE_ERROR_HTTPC_NO_FCM_INDEX_NOT_FOUND_IN_DEVICE_TOKEN_PROVIDED:
    appendP(buff, fb_esp_pgm_str_147);
    return;
  case FIREBASE_ERROR_EXPECTED_JSON_DATA:
    appendP(buff, fb_esp_pgm_str_185);
    return;
  case FIREBASE_ERROR_HTTP_CODE_PAYLOAD_TOO_LARGE:
    appendP(buff, fb_esp_pgm_str_189);
    return;
  case FIREBASE_ERROR_CANNOT_CONFIG_TIME:
    appendP(buff, fb_esp_pgm_str_190);
    return;
  case FIREBASE_ERROR_SSL_RX_BUFFER_SIZE_TOO_SMALL:
    appendP(buff, fb_esp_pgm_str_191);
    return;
  case FIREBASE_ERROR_FILE_IO_ERROR:
    appendP(buff, fb_esp_pgm_str_192);
    return;
  case FIREBASE_ERROR_TOKEN_NOT_READY:
    appendP(buff, fb_esp_pgm_str_252);
    return;
  case FIREBASE_ERROR_UNINITIALIZED:
    appendP(buff, fb_esp_pgm_str_256);
    return;

  default:
    return;
  }
}

bool FirebaseESP32::handleFCMRequest(FirebaseData &fbdo, fb_esp_fcm_msg_type messageType)
{
  if (!Firebase.reconnect(fbdo))
    return false;

  if (!Firebase.waitIdle(fbdo))
    return false;

  if (fbdo.fcm._server_key.length() == 0)
  {
    fbdo._httpCode = FIREBASE_ERROR_HTTPC_NO_FCM_SERVER_KEY_PROVIDED;
    return false;
  }

  if (fbdo.fcm._deviceToken.size() == 0 && messageType == fb_esp_fcm_msg_type::msg_single)
  {
    fbdo._httpCode = FIREBASE_ERROR_HTTPC_NO_FCM_DEVICE_TOKEN_PROVIDED;
    return false;
  }

  if (messageType == fb_esp_fcm_msg_type::msg_single && fbdo.fcm._deviceToken.size() > 0 && fbdo.fcm._index > fbdo.fcm._deviceToken.size() - 1)
  {
    fbdo._httpCode = FIREBASE_ERROR_HTTPC_NO_FCM_INDEX_NOT_FOUND_IN_DEVICE_TOKEN_PROVIDED;
    return false;
  }

  if (messageType == fb_esp_fcm_msg_type::msg_topic && fbdo.fcm._topic.length() == 0)
  {
    fbdo._httpCode = FIREBASE_ERROR_HTTPC_NO_FCM_TOPIC_PROVIDED;
    return false;
  }

  if (!fbdo._isFCM || fbdo._isStream || fbdo._isRTDB)
  {
    closeHTTP(fbdo);
    setSecure(fbdo);
  }

  fbdo.fcm.fcm_begin(fbdo);

  fbdo._isFCM = true;

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

void FirebaseESP32::setStreamCallback(FirebaseData &fbdo, StreamEventCallback dataAvailablecallback, StreamTimeoutCallback timeoutCallback, size_t streamTaskStackSize)
{

  fbdo._streamTaskEnable = false;
  removeMultiPathStreamCallback(fbdo);

  int index = fbdo._idx;
  std::string taskName;
  appendP(taskName, fb_esp_pgm_str_72, true);

  bool hasHandle = false;

  if (fbdo._handle)
  {
    hasHandle = true;
    vTaskDelete(fbdo._handle);
    fbdo._handle = NULL;
  }

  if (fbdo._q_handle)
    hasHandle = true;

  if (index == -1)
  {
    index = fbsoCount;
    fbsoCount++;
  }

  char *tmp = intStr(index);
  appendP(taskName, fb_esp_pgm_str_113);
  taskName += tmp;
  delS(tmp);

  if (streamTaskStackSize > STREAM_TASK_STACK_SIZE)
    fbdo._streamTaskStackSize = streamTaskStackSize;
  else
    fbdo._streamTaskStackSize = STREAM_TASK_STACK_SIZE;

  fbdo._idx = index;
  fbsoIdx = index;
  fbdo._dataAvailableCallback = dataAvailablecallback;
  fbdo._timeoutCallback = timeoutCallback;
  fbdo._streamTaskEnable = true;

  //object created
  if (hasHandle)
    fbso[index] = fbdo;
  else
    fbso.push_back(fbdo);

  runStreamTask(fbdo, taskName);
}

void FirebaseESP32::setMultiPathStreamCallback(FirebaseData &fbdo, MultiPathStreamEventCallback multiPathDataCallback, StreamTimeoutCallback timeoutCallback, size_t streamTaskStackSize)
{

  fbdo._streamTaskEnable = false;
  removeStreamCallback(fbdo);

  int index = fbdo._idx;
  std::string taskName;
  appendP(taskName, fb_esp_pgm_str_72, true);

  bool hasHandle = false;

  if (fbdo._q_handle)
    hasHandle = true;

  if (index == -1)
  {
    index = fbsoCount;
    fbsoCount++;
  }

  char *tmp = intStr(index);
  appendP(taskName, fb_esp_pgm_str_113);
  taskName += tmp;
  delS(tmp);

  if (streamTaskStackSize > STREAM_TASK_STACK_SIZE)
    fbdo._streamTaskStackSize = streamTaskStackSize;
  else
    fbdo._streamTaskStackSize = STREAM_TASK_STACK_SIZE;

  fbdo._idx = index;
  fbsoIdx = index;
  fbdo._multiPathDataCallback = multiPathDataCallback;
  fbdo._timeoutCallback = timeoutCallback;
  fbdo._streamTaskEnable = true;

  //object created
  if (hasHandle)
    fbso[index] = fbdo;
  else
    fbso.push_back(fbdo);

  runStreamTask(fbdo, taskName);
}

void FirebaseESP32::runStreamTask(FirebaseData &fbdo, const std::string &taskName)
{

  TaskFunction_t taskCode = [](void *param) {
    bool res = false;

    while (fbso[fbsoIdx].get()._streamTaskEnable)
    {

      if ((fbso[fbsoIdx].get()._dataAvailableCallback || fbso[fbsoIdx].get()._timeoutCallback))
      {

        res = Firebase.readStream(fbso[fbsoIdx].get());

        if (fbso[fbsoIdx].get().streamTimeout() && fbso[fbsoIdx].get()._timeoutCallback)
          fbso[fbsoIdx].get()._timeoutCallback(true);

        if (Firebase.reconnect(fbso[fbsoIdx].get()))
        {

          if (res && fbso[fbsoIdx].get().streamAvailable() && (fbso[fbsoIdx].get()._dataAvailableCallback || fbso[fbsoIdx].get()._multiPathDataCallback))
          {
            if (fbso[fbsoIdx].get()._dataAvailableCallback)
            {
              StreamData s;
              s._json = &fbso[fbsoIdx].get()._json;
              s._jsonArr = &fbso[fbsoIdx].get()._jsonArr;
              s._jsonData = &fbso[fbsoIdx].get()._jsonData;
              s._streamPath = fbso[fbsoIdx].get()._streamPath;
              s._data = fbso[fbsoIdx].get()._data;
              s._path = fbso[fbsoIdx].get()._path;

              s._dataType = fbso[fbsoIdx].get().resp_dataType;
              s._dataTypeStr = fbso[fbsoIdx].get().getDataType(s._dataType);
              s._eventTypeStr = fbso[fbsoIdx].get()._eventType;
              s._idx = fbsoIdx;

              if (fbso[fbsoIdx].get().resp_dataType == fb_esp_data_type::d_blob)
              {
                s._blob = fbso[fbsoIdx].get()._blob;
                //Free ram in case of the callback data was used
                fbso[fbsoIdx].get()._blob.clear();
              }
              fbso[fbsoIdx].get()._dataAvailableCallback(s);
              s.empty();
            }
            else if (fbso[fbsoIdx].get()._multiPathDataCallback)
            {

              MultiPathStreamData mdata;
              mdata._type = fbso[fbsoIdx].get().resp_dataType;
              mdata._path = fbso[fbsoIdx].get()._path;
              mdata._typeStr = fbso[fbsoIdx].get().getDataType(mdata._type);

              if (mdata._type == fb_esp_data_type::d_json)
                mdata._json = &fbso[fbsoIdx].get()._json;
              else
              {
                if (mdata._type == fb_esp_data_type::d_string)
                  mdata._data = fbso[fbsoIdx].get()._data.substr(1, fbso[fbsoIdx].get()._data.length() - 2).c_str();
                else
                  mdata._data = fbso[fbsoIdx].get()._data;
              }

              fbso[fbsoIdx].get()._multiPathDataCallback(mdata);
              mdata.empty();
            }
          }
        }
      }

      yield();
      vTaskDelay(30 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);

    fbso[fbsoIdx].get()._handle = NULL;
  };

  xTaskCreatePinnedToCore(taskCode, taskName.c_str(), fbdo._streamTaskStackSize, NULL, 3, &fbdo._handle, 1);
}

void FirebaseESP32::removeStreamCallback(FirebaseData &fbdo)
{
  int index = fbdo._idx;

  if (index != -1)
  {
    fbsoCount--;

    bool hasOherHandles = false;

    if (fbdo._q_handle)
    {
      hasOherHandles = true;
    }

    if (!hasOherHandles)
      fbdo._idx = -1;

    if (fbdo._handle)
      vTaskDelete(fbdo._handle);

    fbdo._handle = NULL;

    fbdo._dataAvailableCallback = NULL;
    fbdo._timeoutCallback = NULL;

    if (!hasOherHandles)
      fbso.erase(fbso.begin() + index);
  }
}

void FirebaseESP32::removeMultiPathStreamCallback(FirebaseData &fbdo)
{
  int index = fbdo._idx;

  if (index != -1)
  {
    fbsoCount--;

    bool hasOherHandles = false;

    if (fbdo._q_handle)
    {
      hasOherHandles = true;
    }

    if (!hasOherHandles)
      fbdo._idx = -1;

    if (fbdo._handle)
      vTaskDelete(fbdo._handle);

    fbdo._handle = NULL;

    fbdo._multiPathDataCallback = NULL;
    fbdo._timeoutCallback = NULL;

    if (!hasOherHandles)
      fbso.erase(fbso.begin() + index);
  }
}

void FirebaseESP32::beginAutoRunErrorQueue(FirebaseData &fbdo, QueueInfoCallback callback, size_t queueTaskStackSize)
{

  int index = fbdo._idx;

  std::string taskName;
  appendP(taskName, fb_esp_pgm_str_72);
  char *idx = nullptr;

  bool hasHandle = false;

  if (fbdo._handle || fbdo._q_handle)
  {
    hasHandle = true;
  }

  if (index == -1)
  {
    index = fbsoCount;
    fbsoCount++;
  }

  idx = intStr(index);
  appendP(taskName, fb_esp_pgm_str_114);
  taskName += idx;
  delS(idx);

  if (queueTaskStackSize > QUEUE_TASK_STACK_SIZE)
    fbdo._queueTaskStackSize = queueTaskStackSize;
  else
    fbdo._queueTaskStackSize = QUEUE_TASK_STACK_SIZE;

  if (callback)
    fbdo._queueInfoCallback = callback;
  else
    fbdo._queueInfoCallback = NULL;

  fbdo._idx = index;
  errorQueueIndex = index;

  //object created
  if (hasHandle)
    fbso[index] = fbdo;
  else
    fbso.push_back(fbdo);

  TaskFunction_t taskCode = [](void *param) {
    int id = errorQueueIndex;

    for (;;)
    {

      if (fbso[id].get()._queueInfoCallback)
        Firebase.processErrorQueue(fbso[id].get(), fbso[id].get()._queueInfoCallback);
      else
        Firebase.processErrorQueue(fbso[id].get(), NULL);

      yield();
      vTaskDelay(30 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
    fbso[id].get()._q_handle = NULL;
  };

  xTaskCreatePinnedToCore(taskCode, taskName.c_str(), fbdo._queueTaskStackSize, NULL, 1, &fbdo._q_handle, 1);
}

void FirebaseESP32::endAutoRunErrorQueue(FirebaseData &fbdo)
{
  int index = fbdo._idx;

  if (index != -1)
  {
    bool hasOherHandles = false;

    if (fbdo._handle)
    {
      hasOherHandles = true;
    }

    if (!hasOherHandles)
      fbdo._idx = -1;

    if (fbdo._q_handle)
      vTaskDelete(fbdo._q_handle);

    fbdo._q_handle = NULL;
    fbdo._queueInfoCallback = NULL;

    if (!hasOherHandles)
      fbso.erase(fbso.begin() + index);
  }
}

void FirebaseESP32::clearErrorQueue(FirebaseData &fbdo)
{
  for (uint8_t i = 0; i < fbdo._qMan._queueCollection.size(); i++)
  {
    QueueItem item = fbdo._qMan._queueCollection[i];
    fbdo.clearQueueItem(item);
  }
}

bool FirebaseESP32::backup(FirebaseData &fbdo, uint8_t storageType, const String &nodePath, const String &dirPath)
{
  fbdo._backupDir = dirPath.c_str();
  fbdo._backupNodePath = nodePath.c_str();
  fbdo._backupFilename.clear();
  fbdo._fileName.clear();
  bool ret = handleRequest(fbdo, storageType, nodePath.c_str(), fb_esp_method::m_download, fb_esp_data_type::d_json, "", "", "");
  return ret;
}

bool FirebaseESP32::restore(FirebaseData &fbdo, uint8_t storageType, const String &nodePath, const String &dirPath)
{
  fbdo._backupDir = dirPath.c_str();
  fbdo._backupNodePath = nodePath.c_str();
  fbdo._backupFilename.clear();
  fbdo._fileName.clear();
  bool ret = handleRequest(fbdo, storageType, nodePath.c_str(), fb_esp_method::m_restore, fb_esp_data_type::d_json, "", "", "");
  return ret;
}

int FirebaseESP32::strposP(const char *buf, PGM_P beginH, int ofs)
{
  char *tmp = strP(beginH);
  int p = strpos(buf, tmp, ofs);
  delS(tmp);
  return p;
}

char *FirebaseESP32::strP(PGM_P pgm)
{
  size_t len = strlen_P(pgm) + 1;
  char *buf = newS(len);
  strcpy_P(buf, pgm);
  buf[len - 1] = 0;
  return buf;
}

char *FirebaseESP32::floatStr(float value)
{
  char *buf = newS(36);
  memset(buf, 0, 36);
  dtostrf(value, 7, _floatDigits, buf);
  return buf;
}

char *FirebaseESP32::intStr(int value)
{
  char *buf = newS(36);
  memset(buf, 0, 36);
  itoa(value, buf, 10);
  return buf;
}

char *FirebaseESP32::boolStr(bool value)
{
  char *buf = nullptr;
  if (value)
    buf = strP(fb_esp_pgm_str_107);
  else
    buf = strP(fb_esp_pgm_str_106);
  return buf;
}

char *FirebaseESP32::doubleStr(double value)
{
  char *buf = newS(36);
  memset(buf, 0, 36);
  dtostrf(value, 12, _doubleDigits, buf);
  return buf;
}

void FirebaseESP32::appendP(std::string &buf, PGM_P p, bool empty)
{
  if (empty)
    buf.clear();
  char *b = strP(p);
  buf += b;
  delS(b);
}

void FirebaseESP32::trimDigits(char *buf)
{
  size_t i = strlen(buf) - 1;
  while (buf[i] == '0' && i > 0)
  {
    if (buf[i - 1] == '.')
    {
      i--;
      break;
    }
    if (buf[i - 1] != '0')
      break;
    i--;
  }
  if (i < strlen(buf) - 1)
    buf[i] = '\0';
}

void FirebaseESP32::strcat_c(char *str, char c)
{
  for (; *str; str++)
    ;
  *str++ = c;
  *str++ = 0;
}

int FirebaseESP32::strpos(const char *haystack, const char *needle, int offset)
{
  size_t len = strlen(haystack);
  size_t len2 = strlen(needle);
  if (len == 0 || len < len2 || len2 == 0 || offset >= (int)len)
    return -1;
  char *_haystack = newS(len - offset + 1);
  _haystack[len - offset] = 0;
  strncpy(_haystack, haystack + offset, len - offset);
  char *p = strstr(_haystack, needle);
  int r = -1;
  if (p)
    r = p - _haystack + offset;
  delS(_haystack);
  return r;
}

char *FirebaseESP32::rstrstr(const char *haystack, const char *needle)
{
  size_t needle_length = strlen(needle);
  const char *haystack_end = haystack + strlen(haystack) - needle_length;
  const char *p;
  size_t i;
  for (p = haystack_end; p >= haystack; --p)
  {
    for (i = 0; i < needle_length; ++i)
    {
      if (p[i] != needle[i])
        goto next;
    }
    return (char *)p;
  next:;
  }
  return 0;
}

int FirebaseESP32::rstrpos(const char *haystack, const char *needle, int offset)
{
  size_t len = strlen(haystack);
  size_t len2 = strlen(needle);
  if (len == 0 || len < len2 || len2 == 0 || offset >= (int)len)
    return -1;
  char *_haystack = newS(len - offset + 1);
  _haystack[len - offset] = 0;
  strncpy(_haystack, haystack + offset, len - offset);
  char *p = rstrstr(_haystack, needle);
  int r = -1;
  if (p)
    r = p - _haystack + offset;
  delS(_haystack);
  return r;
}

bool FirebaseESP32::sdTest()
{

  if (_sdConfigSet)
    sdBegin(_sck, _miso, _mosi, _ss);
  else
    sdBegin();

  char *tmp = strP(fb_esp_pgm_str_73);

  File file = SD.open(tmp, FILE_WRITE);
  delS(tmp);
  if (!file)
    return false;

  if (!file.write(32))
    return false;
  file.close();

  tmp = strP(fb_esp_pgm_str_73);
  file = SD.open(tmp);
  delS(tmp);
  if (!file)
    return false;

  while (file.available())
  {
    if (file.read() != 32)
      return false;
  }
  file.close();
  tmp = strP(fb_esp_pgm_str_73);
  SD.remove(tmp);
  delS(tmp);

  return true;
}

bool FirebaseESP32::setClock(float gmtOffset)
{
  if (time(nullptr) > default_ts && gmtOffset == _gmtOffset)
    return true;

  if (WiFi.status() != WL_CONNECTED)
    WiFi.reconnect();

  time_t now = time(nullptr);

  _clockReady = now > default_ts;

  if (!_clockReady || gmtOffset != _gmtOffset)
  {
    char *server1 = strP(fb_esp_pgm_str_187);
    char *server2 = strP(fb_esp_pgm_str_188);
    configTime(gmtOffset * 3600, 0, server1, server2);

    now = time(nullptr);
    unsigned long timeout = millis();
    while (now < default_ts)
    {
      now = time(nullptr);
      if (now > default_ts || millis() - timeout > ntpTimeout)
        break;
      delay(200);
    }

    delS(server1);
    delS(server2);
  }

  _clockReady = now > default_ts;
  if (_clockReady)
    _gmtOffset = gmtOffset;
  return _clockReady;
}

void FirebaseESP32::setMaxRetry(FirebaseData &fbdo, uint8_t num)
{
  fbdo._maxAttempt = num;
}

void FirebaseESP32::setMaxErrorQueue(FirebaseData &fbdo, uint8_t num)
{
  fbdo._qMan._maxQueue = num;

  if (fbdo._qMan._queueCollection.size() > num)
  {
    for (uint8_t i = fbdo._qMan._queueCollection.size() - 1; i >= num; i--)
    {
      QueueItem item = fbdo._qMan._queueCollection[i];
      fbdo.clearQueueItem(item);
    }
  }
}

bool FirebaseESP32::saveErrorQueue(FirebaseData &fbdo, const String &filename, uint8_t storageType)
{
  if (storageType == StorageType::SD)
  {
    if (!sdTest())
      return false;
    file = SD.open(filename.c_str(), FILE_WRITE);
  }
  else if (storageType == StorageType::FLASH)
  {
    FLASH_FS.begin(true);
    file = FLASH_FS.open(filename.c_str(), "w");
  }

  if (!file)
    return false;

  uint8_t idx = 0;
  std::string buff = "";

  char *nbuf = nullptr;

  for (uint8_t i = 0; i < fbdo._qMan._queueCollection.size(); i++)
  {
    QueueItem item = fbdo._qMan._queueCollection[i];

    if (item.method != fb_esp_method::m_get)
    {
      if (idx > 0)
        buff.append("\r");

      nbuf = Firebase.intStr(item.dataType);
      buff.append(nbuf);
      delS(nbuf);
      buff.append("~");

      nbuf = Firebase.intStr(item.method);
      buff.append(nbuf);
      delS(nbuf);
      buff.append("~");

      buff += item.path.c_str();
      buff.append("~");

      buff += item.payload.c_str();
      buff.append("~");

      for (int j = 0; j < item.blob.size(); j++)
      {
        nbuf = Firebase.intStr(item.blob[j]);
        delS(nbuf);
      }
      buff.append("~");

      buff += item.filename.c_str();

      nbuf = Firebase.intStr(item.storageType);
      buff.append(nbuf);
      delS(nbuf);
      buff.append("~");

      idx++;
    }
  }

  file.print(buff.c_str());
  file.close();

  delS(nbuf);
  std::string().swap(buff);

  return true;
}

bool FirebaseESP32::restoreErrorQueue(FirebaseData &fbdo, const String &filename, uint8_t storageType)
{
  return openErrorQueue(fbdo, filename, storageType, 1) != 0;
}

uint8_t FirebaseESP32::errorQueueCount(FirebaseData &fbdo, const String &filename, uint8_t storageType)
{
  return openErrorQueue(fbdo, filename, storageType, 0);
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
    FLASH_FS.begin(true);
    return FLASH_FS.remove(filename.c_str());
  }
}

uint8_t FirebaseESP32::openErrorQueue(FirebaseData &fbdo, const String &filename, uint8_t storageType, uint8_t mode)
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

    FLASH_FS.begin(true);

    file = FLASH_FS.open(filename.c_str(), "r");
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

  std::vector<std::string> p = splitString(fbdo._maxBlobSize, t.c_str(), '\r');

  for (int i = 0; i < p.size(); i++)
  {

    std::vector<std::string> q = splitString(fbdo._maxBlobSize, p[i].c_str(), '~');

    if (q.size() == 6)
    {
      count++;

      if (mode == 1)
      {

        //Restore Firebase Error Queues
        QueueItem item;

        item.dataType = (fb_esp_data_type)atoi(q[0].c_str());
        item.method = (fb_esp_method)atoi(q[1].c_str());
        item.path.append(q[2].c_str());
        item.payload.append(q[3].c_str());

        for (int j = 0; j < q[4].length(); j++)
          item.blob.push_back(atoi(q[4].c_str()));

        item.filename.append(q[5].c_str());

        //backwards compatibility to old APIs
        if (q.size() == 7)
          item.storageType = atoi(q[6].c_str());

        fbdo._qMan._queueCollection.push_back(item);
      }
    }
  }
  std::string().swap(t);

  return count;
}

bool FirebaseESP32::isErrorQueueFull(FirebaseData &fbdo)
{
  if (fbdo._qMan._maxQueue > 0)
    return fbdo._qMan._queueCollection.size() >= fbdo._qMan._maxQueue;
  return false;
}

uint8_t FirebaseESP32::errorQueueCount(FirebaseData &fbdo)
{
  return fbdo._qMan._queueCollection.size();
}

std::vector<std::string> FirebaseESP32::splitString(int size, const char *str, const char delim)
{
  uint16_t index = 0;
  uint16_t len = strlen(str);
  int buffSize = (int)(size * 1.4f);
  char *buff = newS(buffSize);
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

  delS(buff);

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

std::string FirebaseESP32::encodeBase64Str(const unsigned char *src, size_t len)
{
  return encodeBase64Str((uint8_t *)src, len);
}

std::string FirebaseESP32::encodeBase64Str(uint8_t *src, size_t len)
{
  unsigned char *out, *pos;
  const unsigned char *end, *in;

  unsigned char *b64enc = new unsigned char[65];
  strcpy_P((char *)b64enc, (char *)fb_esp_base64_table);

  size_t olen;

  olen = 4 * ((len + 2) / 3); /* 3-byte blocks to 4-byte */

  if (olen < len)
  {
    delete[] b64enc;
    return std::string();
  }

  std::string outStr;
  outStr.resize(olen);
  out = (unsigned char *)&outStr[0];

  end = src + len;
  in = src;
  pos = out;

  while (end - in >= 3)
  {
    *pos++ = b64enc[in[0] >> 2];
    *pos++ = b64enc[((in[0] & 0x03) << 4) | (in[1] >> 4)];
    *pos++ = b64enc[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
    *pos++ = b64enc[in[2] & 0x3f];
    in += 3;
    yield();
  }

  if (end - in)
  {

    *pos++ = b64enc[in[0] >> 2];

    if (end - in == 1)
    {
      *pos++ = b64enc[(in[0] & 0x03) << 4];
      *pos++ = '=';
    }
    else
    {
      *pos++ = b64enc[((in[0] & 0x03) << 4) | (in[1] >> 4)];
      *pos++ = b64enc[(in[1] & 0x0f) << 2];
    }

    *pos++ = '=';
  }
  delete[] b64enc;
  return outStr;
}

void FirebaseESP32::sendBase64Stream(WiFiClient *client, const std::string &filePath, uint8_t storageType)
{

  if (storageType == StorageType::FLASH)
    file = FLASH_FS.open(filePath.c_str(), "r");
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

      buff[byteAdd++] = fb_esp_base64_table[fbuff[0] >> 2];
      buff[byteAdd++] = fb_esp_base64_table[((fbuff[0] & 0x03) << 4) | (fbuff[1] >> 4)];
      buff[byteAdd++] = fb_esp_base64_table[((fbuff[1] & 0x0f) << 2) | (fbuff[2] >> 6)];
      buff[byteAdd++] = fb_esp_base64_table[fbuff[2] & 0x3f];

      if (byteAdd >= chunkSize - 4)
      {
        byteSent += byteAdd;
        client->write(buff, byteAdd);
        memset(buff, 0, chunkSize);
        byteAdd = 0;
      }

      fbuffIndex += 3;
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
    client->write(buff, byteAdd);

  if (len - fbuffIndex > 0)
  {

    memset(buff, 0, chunkSize);
    byteAdd = 0;

    buff[byteAdd++] = fb_esp_base64_table[fbuff[0] >> 2];
    if (len - fbuffIndex == 1)
    {
      buff[byteAdd++] = fb_esp_base64_table[(fbuff[0] & 0x03) << 4];
      buff[byteAdd++] = '=';
    }
    else
    {
      buff[byteAdd++] = fb_esp_base64_table[((fbuff[0] & 0x03) << 4) | (fbuff[1] >> 4)];
      buff[byteAdd++] = fb_esp_base64_table[(fbuff[1] & 0x0f) << 2];
    }
    buff[byteAdd++] = '=';

    client->write(buff, byteAdd);
  }

  delete[] buff;
  delete[] fbuff;
}

bool FirebaseESP32::decodeBase64Str(const std::string src, std::vector<uint8_t> &out)
{
  unsigned char *dtable = new unsigned char[256];
  memset(dtable, 0x80, 256);
  for (size_t i = 0; i < sizeof(fb_esp_base64_table) - 1; i++)
    dtable[fb_esp_base64_table[i]] = (unsigned char)i;
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

bool FirebaseESP32::decodeBase64Stream(File &file, const char *src, size_t len)
{
  unsigned char *dtable = new unsigned char[256];
  memset(dtable, 0x80, 256);
  for (size_t i = 0; i < sizeof(fb_esp_base64_table) - 1; i++)
    dtable[fb_esp_base64_table[i]] = (unsigned char)i;
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

bool FirebaseESP32::decodeBase64Flash(File &file, const char *src, size_t len)
{
  unsigned char *dtable = new unsigned char[256];
  memset(dtable, 0x80, 256);
  for (size_t i = 0; i < sizeof(fb_esp_base64_table) - 1; i++)
    dtable[fb_esp_base64_table[i]] = (unsigned char)i;
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

uint32_t FirebaseESP32::hex2int(const char *hex)
{
  uint32_t val = 0;
  while (*hex)
  {
    // get current character then increment
    uint8_t byte = *hex++;
    // transform hex character to the 4bit equivalent number, using the ascii table indexes
    if (byte >= '0' && byte <= '9')
      byte = byte - '0';
    else if (byte >= 'a' && byte <= 'f')
      byte = byte - 'a' + 10;
    else if (byte >= 'A' && byte <= 'F')
      byte = byte - 'A' + 10;
    // shift 4 to make space for new digit, and add the 4 bits of the new digit
    val = (val << 4) | (byte & 0xF);
  }
  return val;
}

FirebaseData::FirebaseData()
{
  _idx = -1;
}

FirebaseData::~FirebaseData()
{
  clear();
}

void FirebaseData::clear()
{
  _dataAvailableCallback = NULL;
  _multiPathDataCallback = NULL;
  _timeoutCallback = NULL;
  _queueInfoCallback = NULL;
  _handle = NULL;
  _q_handle = NULL;

  std::string().swap(_path);
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
  std::string().swap(_fbError);
  std::string().swap(_eventType);
  std::string().swap(_resp_etag);
  std::string().swap(_req_etag);
  std::string().swap(_priority);
  std::string().swap(_writeLimit);
  _json.clear();
  _jsonArr.clear();
  _jsonData.stringValue.clear();
  _qMan.clear();
  _blob.clear();
  for (uint8_t i = 0; i < _childNodeList.size(); i++)
    std::string().swap(_childNodeList[i]);
  _childNodeList.clear();
  if (httpClient.stream())
  {
    if (httpClient.stream()->connected())
      httpClient.stream()->stop();
  }
}

void FirebaseData::addQueue(fb_esp_method method,
                            uint8_t storageType,
                            fb_esp_data_type dataType,
                            const std::string path,
                            const std::string filename,
                            const std::string payload,
                            bool isQuery,
                            int *intTarget,
                            float *floatTarget,
                            double *doubleTarget,
                            bool *boolTarget,
                            String *stringTarget,
                            std::vector<uint8_t> *blobTarget,
                            FirebaseJson *jsonTarget,
                            FirebaseJsonArray *arrTarget)
{
  if (_qMan._queueCollection.size() < _qMan._maxQueue && payload.length() <= _maxBlobSize)
  {
    QueueItem item;
    item.method = method;
    item.dataType = dataType;
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
    item.jsonPtr = jsonTarget;
    item.arrPtr = arrTarget;
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

void FirebaseData::clearNodeList()
{
  for (size_t i = 0; i < _childNodeList.size(); i++)
    std::string().swap(_childNodeList[i]);
  _childNodeList.clear();
}

void FirebaseData::addNodeList(const String childPath[], size_t size)
{
  clearNodeList();
  for (size_t i = 0; i < size; i++)
    if (childPath[i].length() > 0 && childPath[i] != "/")
      _childNodeList.push_back(childPath[i].c_str());
}

void FirebaseData::setResponseSize(size_t len)
{
  if (len >= 4096)
    _responseBufSize = 4 * (1 + (len / 4));
}

WiFiClient &FirebaseData::getWiFiClient()
{
  return *httpClient._wcs.get();
}

void FirebaseData::stopWiFiClient()
{
  if (httpClient.stream())
  {
    if (httpClient.stream()->connected())
      httpClient.stream()->stop();
  }
  _httpConnected = false;
}

bool FirebaseData::pauseFirebase(bool pause)
{
  if (WiFi.status() != WL_CONNECTED)
    return false;

  if (httpClient.connected() && pause != _pause)
  {
    if (httpClient.stream()->connected())
      httpClient.stream()->stop();

    if (!httpClient.connected())
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
  return getDataType(resp_dataType).c_str();
}

String FirebaseData::eventType()
{
  return _eventType.c_str();
}

String FirebaseData::ETag()
{
  return _resp_etag.c_str();
}

std::string FirebaseData::getDataType(uint8_t type)
{
  std::string res = "";
  switch (type)
  {
  case fb_esp_data_type::d_json:
    Firebase.appendP(res, fb_esp_pgm_str_74);
    break;
  case fb_esp_data_type::d_array:
    Firebase.appendP(res, fb_esp_pgm_str_165);
    break;
  case fb_esp_data_type::d_string:
    Firebase.appendP(res, fb_esp_pgm_str_75);
    break;
  case fb_esp_data_type::d_float:
    Firebase.appendP(res, fb_esp_pgm_str_76);
    break;
  case fb_esp_data_type::d_double:
    Firebase.appendP(res, fb_esp_pgm_str_108);
    break;
  case fb_esp_data_type::d_boolean:
    Firebase.appendP(res, fb_esp_pgm_str_105);
    break;
  case fb_esp_data_type::d_integer:
    Firebase.appendP(res, fb_esp_pgm_str_77);
    break;
  case fb_esp_data_type::d_blob:
    Firebase.appendP(res, fb_esp_pgm_str_91);
    break;
  case fb_esp_data_type::d_file:
    Firebase.appendP(res, fb_esp_pgm_str_183);
    break;
  case fb_esp_data_type::d_null:
    Firebase.appendP(res, fb_esp_pgm_str_78);
    break;
  default:
    break;
  }
  return res;
}

std::string FirebaseData::getMethod(uint8_t method)
{
  std::string res = "";
  switch (method)
  {
  case fb_esp_method::m_get:
    Firebase.appendP(res, fb_esp_pgm_str_115);
    break;
  case fb_esp_method::m_put:
  case fb_esp_method::m_put_nocontent:
    Firebase.appendP(res, fb_esp_pgm_str_116);
    break;
  case fb_esp_method::m_post:
    Firebase.appendP(res, fb_esp_pgm_str_117);
    break;
  case fb_esp_method::m_patch:
  case fb_esp_method::m_patch_nocontent:
    Firebase.appendP(res, fb_esp_pgm_str_118);
    break;
  case fb_esp_method::m_delete:
    Firebase.appendP(res, fb_esp_pgm_str_119);
    break;
  default:
    break;
  }
  return res;
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
  if ((_data.length() > 0 && (resp_dataType == fb_esp_data_type::d_integer || resp_dataType == fb_esp_data_type::d_float || resp_dataType == fb_esp_data_type::d_double)))
  {
    if (_req_dataType == fb_esp_data_type::d_timestamp)
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
  if (_data.length() > 0 && (resp_dataType == fb_esp_data_type::d_integer || resp_dataType == fb_esp_data_type::d_float || resp_dataType == fb_esp_data_type::d_double))
    return atof(_data.c_str());
  else
    return 0.0;
}

double FirebaseData::doubleData()
{
  if (_data.length() > 0 && (resp_dataType == fb_esp_data_type::d_integer || resp_dataType == fb_esp_data_type::d_float || resp_dataType == fb_esp_data_type::d_double))
    return atof(_data.c_str());
  else
    return 0.0;
}

bool FirebaseData::boolData()
{
  bool res = false;
  char *str = Firebase.boolStr(true);
  if (_data.length() > 0 && resp_dataType == fb_esp_data_type::d_boolean)
    res = strcmp(_data.c_str(), str) == 0;
  Firebase.delS(str);
  return res;
}

String FirebaseData::stringData()
{
  if (_data.length() > 0 && resp_dataType == fb_esp_data_type::d_string)
    return _data.substr(1, _data.length() - 2).c_str();
  else
    return std::string().c_str();
}

String FirebaseData::jsonString()
{
  if (resp_dataType == fb_esp_data_type::d_json)
  {
    String s;
    _json.toString(s);
    return s;
  }
  else
    return String();
}

FirebaseJson *FirebaseData::jsonObjectPtr()
{
  return &_json;
}

FirebaseJson &FirebaseData::jsonObject()
{
  return *jsonObjectPtr();
}

FirebaseJsonArray *FirebaseData::jsonArrayPtr()
{
  if (_data.length() > 0 && resp_dataType == fb_esp_data_type::d_array)
  {
    char *tmp = Firebase.newS(20);

    std::string().swap(_jsonArr._json._jsonData._dbuf);
    std::string().swap(_jsonArr._json._tbuf);

    strcpy_P(tmp, FirebaseJson_STR_21);
    _jsonArr._json._toStdString(_jsonArr._jbuf, false);
    _jsonArr._json._rawbuf = tmp;
    _jsonArr._json._rawbuf += _data;

    tmp = Firebase.newS(tmp, 20);
    strcpy_P(tmp, FirebaseJson_STR_26);

    _jsonArr._json._parse(tmp, FirebaseJson::PRINT_MODE_PLAIN);

    std::string().swap(_jsonArr._json._tbuf);
    std::string().swap(_jsonArr._jbuf);
    _jsonArr._json.clearPathTk();
    _jsonArr._json._tokens.reset();
    _jsonArr._json._tokens = nullptr;
    Firebase.delS(tmp);
    if (_jsonArr._json._jsonData._dbuf.length() > 2)
      _jsonArr._json._rawbuf = _jsonArr._json._jsonData._dbuf.substr(1, _jsonArr._json._jsonData._dbuf.length() - 2);
    _jsonArr._arrLen = _jsonArr._json._jsonData._len;
  }
  return &_jsonArr;
}

FirebaseJsonArray &FirebaseData::jsonArray()
{
  return *jsonArrayPtr();
}

FirebaseJsonData &FirebaseData::jsonData()
{
  return _jsonData;
}

FirebaseJsonData *FirebaseData::jsonDataPtr()
{
  return &_jsonData;
}

std::vector<uint8_t> FirebaseData::blobData()
{
  if (_blob.size() > 0 && resp_dataType == fb_esp_data_type::d_blob)
  {
    return _blob;
  }
  else
    return std::vector<uint8_t>();
}

File FirebaseData::fileStream()
{
  File file;
  if (resp_dataType == fb_esp_data_type::d_file)
  {
    char *tmp = Firebase.strP(fb_esp_pgm_str_184);
    if (FLASH_FS.begin(true))
      file = FLASH_FS.open(tmp, FILE_READ);
    Firebase.delS(tmp);
  }
  return file;
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
  if (millis() - STREAM_ERROR_NOTIFIED_INTERVAL > _streamTimeoutMillis || _streamTimeoutMillis == 0)
  {
    _streamTimeoutMillis = millis();
    if (_isDataTimeout)
      Firebase.closeHTTP(*this);
    return _isDataTimeout;
  }
  return false;
}

bool FirebaseData::dataAvailable()
{
  return _dataAvailable;
}

bool FirebaseData::streamAvailable()
{
  bool ret = _httpConnected && _dataAvailable && _streamDataChanged;
  _dataAvailable = false;
  _streamDataChanged = false;
  return ret;
}

bool FirebaseData::mismatchDataType()
{
  return _mismatchDataType;
}

bool FirebaseData::bufferOverflow()
{
  return _bufOvf;
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
  std::string buf = "";
  if (_fbError.length() > 0)
    return _fbError.c_str();
  else
    Firebase.errorToString(_httpCode, buf);
  return buf.c_str();
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
  if (_data.length() > 0 && (_dataType == fb_esp_data_type::d_integer || _dataType == fb_esp_data_type::d_float || _dataType == fb_esp_data_type::d_double))
    return atoi(_data.c_str());
  else
    return 0;
}

float StreamData::floatData()
{
  if (_data.length() > 0 && (_dataType == fb_esp_data_type::d_integer || _dataType == fb_esp_data_type::d_float || _dataType == fb_esp_data_type::d_double))
    return atof(_data.c_str());
  else
    return 0;
}

double StreamData::doubleData()
{
  if (_data.length() > 0 && (_dataType == fb_esp_data_type::d_integer || _dataType == fb_esp_data_type::d_float || _dataType == fb_esp_data_type::d_double))
    return atof(_data.c_str());
  else
    return 0.0;
}

bool StreamData::boolData()
{
  bool res = false;
  char *str = Firebase.boolStr(true);
  if (_data.length() > 0 && _dataType == fb_esp_data_type::d_boolean)
    res = strcmp(_data.c_str(), str) == 0;
  Firebase.delS(str);
  return res;
}

String StreamData::stringData()
{
  if (_dataType == fb_esp_data_type::d_string)
    return _data.substr(1, _data.length() - 2).c_str();
  else
    return std::string().c_str();
}

String StreamData::jsonString()
{
  if (_dataType == fb_esp_data_type::d_json)
  {
    String res = "";
    _json->toString(res);
    return res;
  }
  else
    return std::string().c_str();
}

FirebaseJson *StreamData::jsonObjectPtr()
{
  return _json;
}

FirebaseJson &StreamData::jsonObject()
{
  return *jsonObjectPtr();
}

FirebaseJsonArray *StreamData::jsonArrayPtr()
{
  if (_data.length() > 0 && _dataType == fb_esp_data_type::d_array)
  {

    char *tmp = Firebase.newS(20);

    std::string().swap(_jsonArr->_json._jsonData._dbuf);
    std::string().swap(_jsonArr->_json._tbuf);

    strcpy_P(tmp, FirebaseJson_STR_21);
    _jsonArr->_json._toStdString(_jsonArr->_jbuf, false);
    _jsonArr->_json._rawbuf = tmp;
    _jsonArr->_json._rawbuf += _data;
    Firebase.delS(tmp);

    tmp = Firebase.newS(tmp, 20);
    strcpy_P(tmp, FirebaseJson_STR_26);

    _jsonArr->_json._parse(tmp, FirebaseJson::PRINT_MODE_PLAIN);
    Firebase.delS(tmp);

    std::string().swap(_jsonArr->_json._tbuf);
    std::string().swap(_jsonArr->_jbuf);
    _jsonArr->_json.clearPathTk();
    _jsonArr->_json._tokens.reset();
    _jsonArr->_json._tokens = nullptr;

    if (_jsonArr->_json._jsonData._dbuf.length() > 2)
      _jsonArr->_json._rawbuf = _jsonArr->_json._jsonData._dbuf.substr(1, _jsonArr->_json._jsonData._dbuf.length() - 2);
    _jsonArr->_arrLen = _jsonArr->_json._jsonData._len;
  }
  return _jsonArr;
}

FirebaseJsonArray &StreamData::jsonArray()
{
  return *jsonArrayPtr();
}

FirebaseJsonData *StreamData::jsonDataPtr()
{
  return _jsonData;
}

FirebaseJsonData &StreamData::jsonData()
{
  return *_jsonData;
}

std::vector<uint8_t> StreamData::blobData()
{
  if (_blob.size() > 0 && _dataType == fb_esp_data_type::d_blob)
  {
    return _blob;
  }
  else
    return std::vector<uint8_t>();
}

File StreamData::fileStream()
{
  File file;
  if (_dataType == fb_esp_data_type::d_file)
  {

    char *tmp = Firebase.strP(fb_esp_pgm_str_184);

    if (FLASH_FS.begin(true))
      file = FLASH_FS.open(tmp, FILE_READ);

    Firebase.delS(tmp);
  }

  return file;
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
  if (_json)
    _json->clear();
  if (_jsonArr)
    _jsonArr->clear();
  if (_jsonData)
    _jsonData->stringValue.clear();
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
  Firebase.appendP(_orderBy, fb_esp_pgm_str_3, true);
  _orderBy += val.c_str();
  Firebase.appendP(_orderBy, fb_esp_pgm_str_3);
  return *this;
}
QueryFilter &QueryFilter::limitToFirst(int val)
{

  char *num = Firebase.intStr(val);
  _limitToFirst = num;
  Firebase.delS(num);
  return *this;
}

QueryFilter &QueryFilter::limitToLast(int val)
{
  char *num = Firebase.intStr(val);
  _limitToLast = num;
  Firebase.delS(num);
  return *this;
}

QueryFilter &QueryFilter::startAt(float val)
{
  char *num = Firebase.floatStr(val);
  _startAt = num;
  Firebase.delS(num);
  return *this;
}

QueryFilter &QueryFilter::endAt(float val)
{
  char *num = Firebase.floatStr(val);
  _endAt = num;
  Firebase.delS(num);
  return *this;
}

QueryFilter &QueryFilter::startAt(const String &val)
{
  Firebase.appendP(_startAt, fb_esp_pgm_str_3, true);
  _startAt += val.c_str();
  Firebase.appendP(_startAt, fb_esp_pgm_str_3);
  return *this;
}

QueryFilter &QueryFilter::endAt(const String &val)
{
  Firebase.appendP(_endAt, fb_esp_pgm_str_3, true);
  _startAt += val.c_str();
  Firebase.appendP(_endAt, fb_esp_pgm_str_3);
  return *this;
}

QueryFilter &QueryFilter::equalTo(int val)
{
  char *num = Firebase.intStr(val);
  _equalTo = num;
  Firebase.delS(num);
  return *this;
}

QueryFilter &QueryFilter::equalTo(const String &val)
{
  Firebase.appendP(_equalTo, fb_esp_pgm_str_3, true);
  _equalTo += val.c_str();
  Firebase.appendP(_equalTo, fb_esp_pgm_str_3);
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
    item.jsonPtr = nullptr;
    item.arrPtr = nullptr;
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
  char *key = Firebase.strP(fb_esp_pgm_str_123);
  _fcmPayload.set(key, title);
  Firebase.delS(key);
  key = Firebase.strP(fb_esp_pgm_str_124);
  _fcmPayload.set(key, body);
  Firebase.delS(key);
}

void FCMObject::setNotifyMessage(const String &title, const String &body, const String &icon)
{
  setNotifyMessage(title, body);
  char *key = Firebase.strP(fb_esp_pgm_str_125);
  _fcmPayload.set(key, icon);
  Firebase.delS(key);
}

void FCMObject::setNotifyMessage(const String &title, const String &body, const String &icon, const String &click_action)
{
  setNotifyMessage(title, body, icon);
  char *key = Firebase.strP(fb_esp_pgm_str_126);
  _fcmPayload.set(key, click_action);
  Firebase.delS(key);
}

void FCMObject::addCustomNotifyMessage(const String &key, const String &value)
{
  std::string s;
  Firebase.appendP(s, fb_esp_pgm_str_122, true);
  Firebase.appendP(s, fb_esp_pgm_str_1, false);
  s += key.c_str();
  _fcmPayload.set(s.c_str(), value);
  std::string().swap(s);
}

void FCMObject::clearNotifyMessage()
{
  char *key = Firebase.strP(fb_esp_pgm_str_122);
  _fcmPayload.remove(key);
  Firebase.delS(key);
}

void FCMObject::setDataMessage(const String &jsonString)
{
  char *key = Firebase.strP(fb_esp_pgm_str_135);
  FirebaseJson js;
  js.setJsonData(jsonString);
  _fcmPayload.set(key, js);
  Firebase.delS(key);
  js.clear();
}

void FCMObject::setDataMessage(FirebaseJson &json)
{
  char *key = Firebase.strP(fb_esp_pgm_str_135);
  _fcmPayload.set(key, json);
  Firebase.delS(key);
}

void FCMObject::clearDataMessage()
{
  char *key = Firebase.strP(fb_esp_pgm_str_135);
  _fcmPayload.remove(key);
  Firebase.delS(key);
}

void FCMObject::setPriority(const String &priority)
{
  char *key = Firebase.strP(fb_esp_pgm_str_136);
  _fcmPayload.set(key, priority);
  Firebase.delS(key);
}

void FCMObject::setCollapseKey(const String &key)
{
  char *_key = Firebase.strP(fb_esp_pgm_str_138);
  _fcmPayload.set(_key, key);
  Firebase.delS(_key);
}

void FCMObject::setTimeToLive(uint32_t seconds)
{
  if (seconds <= 2419200)
    _ttl = seconds;
  else
    _ttl = -1;
  char *key = Firebase.strP(fb_esp_pgm_str_137);
  _fcmPayload.set(key, _ttl);
  Firebase.delS(key);
}

void FCMObject::setTopic(const String &topic)
{
  Firebase.appendP(_topic, fb_esp_pgm_str_134, true);
  _topic += topic.c_str();
}

String FCMObject::getSendResult()
{
  return _sendResult.c_str();
}

void FCMObject::fcm_begin(FirebaseData &fbdo)
{
  std::string host;
  Firebase.appendP(host, fb_esp_pgm_str_249);
  Firebase.appendP(host, fb_esp_pgm_str_4);
  Firebase.appendP(host, fb_esp_pgm_str_120);
  fbdo.httpClient.begin(host.c_str(), _port);
}
void FCMObject::fcm_prepareHeader(std::string &header, size_t payloadSize)
{

  char *len = nullptr;

  Firebase.appendP(header, fb_esp_pgm_str_24, true);
  Firebase.appendP(header, fb_esp_pgm_str_6);
  Firebase.appendP(header, fb_esp_pgm_str_121);
  Firebase.appendP(header, fb_esp_pgm_str_30);

  Firebase.appendP(header, fb_esp_pgm_str_31);
  Firebase.appendP(header, fb_esp_pgm_str_249);
  Firebase.appendP(header, fb_esp_pgm_str_4);
  Firebase.appendP(header, fb_esp_pgm_str_120);
  Firebase.appendP(header, fb_esp_pgm_str_21);

  Firebase.appendP(header, fb_esp_pgm_str_131);
  header += _server_key;
  Firebase.appendP(header, fb_esp_pgm_str_21);

  Firebase.appendP(header, fb_esp_pgm_str_32);

  Firebase.appendP(header, fb_esp_pgm_str_8);
  Firebase.appendP(header, fb_esp_pgm_str_129);
  Firebase.appendP(header, fb_esp_pgm_str_21);

  Firebase.appendP(header, fb_esp_pgm_str_12);
  len = Firebase.intStr(payloadSize);
  header += len;
  Firebase.delS(len);
  Firebase.appendP(header, fb_esp_pgm_str_21);
  Firebase.appendP(header, fb_esp_pgm_str_36);
  Firebase.appendP(header, fb_esp_pgm_str_21);
}

void FCMObject::fcm_preparePayload(std::string &msg, fb_esp_fcm_msg_type messageType)
{
  char *tmp = nullptr;
  if (messageType == fb_esp_fcm_msg_type::msg_single)
  {
    tmp = Firebase.strP(fb_esp_pgm_str_128);
    _fcmPayload.add(tmp, _deviceToken[_index].c_str());
    Firebase.delS(tmp);
  }
  else if (messageType == fb_esp_fcm_msg_type::msg_multicast)
  {
    FirebaseJsonArray arr;
    for (uint16_t i = 0; i < _deviceToken.size(); i++)
      arr.add(_deviceToken[i].c_str());
    tmp = Firebase.strP(fb_esp_pgm_str_130);
    _fcmPayload.add(tmp, arr);
    Firebase.delS(tmp);
    arr.clear();
  }
  else if (messageType == fb_esp_fcm_msg_type::msg_topic)
  {
    tmp = Firebase.strP(fb_esp_pgm_str_128);
    _fcmPayload.add(tmp, _topic.c_str());
    Firebase.delS(tmp);
  }

  String s;
  _fcmPayload.toString(s);
  msg = s.c_str();
  s.clear();
}

bool FCMObject::fcm_send(FirebaseData &fbdo, fb_esp_fcm_msg_type messageType)
{
  std::string msg = "";
  std::string header = "";

  fcm_preparePayload(msg, messageType);
  fcm_prepareHeader(header, msg.length());
  int ret = fbdo.httpClient.send(header.c_str(), msg.c_str());
  std::string().swap(msg);
  std::string().swap(header);
  _fcmPayload.clear();
  if (ret != 0)
  {
    Firebase.closeHTTP(fbdo);
    return false;
  }
  else
    fbdo._httpConnected = true;

  ret = Firebase.waitResponse(fbdo);
  _sendResult.clear();

  if (ret)
    _sendResult = fbdo._data;
  else
    Firebase.closeHTTP(fbdo);

  fbdo._data.clear();
  fbdo._data = fbdo._data2;
  return ret;
}

void FCMObject::clear()
{
  _fcmPayload.clear();
  std::string().swap(_topic);
  std::string().swap(_server_key);
  std::string().swap(_sendResult);
  _ttl = -1;
  _index = 0;
  clearDeviceToken();
  std::vector<std::string>().swap(_deviceToken);
}

MultiPathStreamData::MultiPathStreamData()
{
}

MultiPathStreamData::~MultiPathStreamData()
{
}

bool MultiPathStreamData::get(const String &path)
{
  value.clear();
  type.clear();
  dataPath.clear();
  bool res = false;
  if (_type == fb_esp_data_type::d_json)
  {
    char *tmp = Firebase.strP(fb_esp_pgm_str_1);
    if (strcmp(_path.c_str(), tmp) == 0)
    {
      FirebaseJsonData data;
      _json->get(data, path);
      if (data.success)
      {
        type = data.type.c_str();
        char *buf = Firebase.strP(fb_esp_pgm_str_186);
        if (strcmp(type.c_str(), buf) == 0)
          type = _typeStr.c_str();
        value = data.stringValue;
        dataPath = path;
        Firebase.delS(buf);
        res = true;
      }
    }
    else
    {
      std::string p1 = _path;
      if (path.length() < _path.length())
        p1 = _path.substr(0, path.length());
      std::string p2 = path.c_str();
      if (p2[0] != '/')
        p2 = "/" + p2;
      if (strcmp(p1.c_str(), p2.c_str()) == 0)
      {
        _json->toString(value, true);
        type = _typeStr.c_str();
        dataPath = _path.c_str();
        res = true;
      }
      std::string().swap(p1);
      std::string().swap(p2);
    }
  }
  else
  {
    std::string p1 = _path;
    if (path.length() < _path.length())
      p1 = _path.substr(0, path.length());
    std::string p2 = path.c_str();
    if (p2[0] != '/')
      p2 = "/" + p2;
    if (strcmp(p1.c_str(), p2.c_str()) == 0)
    {
      value = _data.c_str();
      dataPath = _path.c_str();
      type = _typeStr.c_str();
      res = true;
    }
    std::string().swap(p1);
    std::string().swap(p2);
  }
  return res;
}

void MultiPathStreamData::empty()
{
  std::string().swap(_data);
  std::string().swap(_path);
  std::string().swap(_typeStr);
  dataPath.clear();
  value.clear();
  type.clear();
  _json = nullptr;
}

FirebaseESP32 Firebase = FirebaseESP32();

#endif /* ESP32 */

#endif /* FirebaseESP32_CPP */