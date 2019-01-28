/*
 * Google's Firebase real time database - ESP32 library for Arduino, version 2.0
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
#ifndef HTTPClientEx_CPP
#define HTTPClientEx_CPP


#include "HTTPClientEx.h"

class TransportTraits
{
  public:
    virtual ~TransportTraits(){}

    virtual std::unique_ptr<WiFiClient> create()
    {
      return std::unique_ptr<WiFiClient>(new WiFiClient());
    }

    virtual bool verify(WiFiClient& client, const char* host)
    {
      return true;
    }
};

class TLSTraits : public TransportTraits
{
  public:
    TLSTraits(const char* CAcert, const char* clicert = nullptr, const char* clikey = nullptr):
      _cacert(CAcert), _clicert(clicert), _clikey(clikey){}

    std::unique_ptr<WiFiClient> create() override
    {
      return std::unique_ptr<WiFiClient>(new WiFiClientSecure());
    }

    bool verify(WiFiClient& client, const char* host) override
    {
      WiFiClientSecure& wcs = static_cast<WiFiClientSecure&>(client);
      wcs.setCACert(_cacert);
      wcs.setCertificate(_clicert);
      wcs.setPrivateKey(_clikey);
      return true;
    }

  protected:
    const char* _cacert;
    const char* _clicert;
    const char* _clikey;
};


HTTPClientEx::HTTPClientEx()
{
}

HTTPClientEx::~HTTPClientEx()
{
  if (_tcp) _tcp->stop();
}

bool HTTPClientEx::http_begin(const char* host, uint16_t port, const char* uri, const char* CAcert)
{
  http_transportTraits.reset(nullptr);
  memset(_host, 0, sizeof _host);
  strcpy(_host, host);
  _port = port;
  memset(_uri, 0, sizeof _uri);
  strcpy(_uri, uri);
  http_transportTraits = TransportTraitsPtr(new TLSTraits(CAcert));
  return true;
}


bool HTTPClientEx::http_connected()
{
  if (_tcp) return ((_tcp->available() > 0) || _tcp->connected());
  return false;
}


bool HTTPClientEx::http_sendHeader(const char* header)
{
  if (!http_connected())return false;
  return (_tcp->write(header, strlen(header)) == strlen(header));
}

int HTTPClientEx::http_sendRequest(const char* header, const char* payload)
{
  size_t size = strlen(payload);
  if(strlen(header)>0){
	if (!http_connect()) return HTTPC_ERROR_CONNECTION_REFUSED;  
	if (!http_sendHeader(header))return HTTPC_ERROR_SEND_HEADER_FAILED;
  }
  if (size > 0)
    if (_tcp->write(&payload[0], size) != size)
      return HTTPC_ERROR_SEND_PAYLOAD_FAILED;
  return 0;
}


WiFiClient* HTTPClientEx::http_getStreamPtr(void)
{
  if (http_connected()) return _tcp.get();
  return nullptr;
}

bool HTTPClientEx::http_connect(void)
{
  if (http_connected()) {
    while (_tcp->available() > 0) _tcp->read();
    return true;
  }

  if (!http_transportTraits)
     return false;

  _tcp = http_transportTraits->create();

  if (!http_transportTraits->verify(*_tcp, _host)) {
    _tcp->stop();
    return false;
  }

  if (!_tcp->connect(_host, _port)) return false;
  
  return http_connected();
}
#endif
