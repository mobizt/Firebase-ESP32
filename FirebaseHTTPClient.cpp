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

#ifndef FirebaseHTTPClient_H_
#define FirebaseHTTPClient_H_

#include <HTTPClient.h>
#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>

class FirebaseHTTPClient:public HTTPClient
{
  public:
    FirebaseHTTPClient();
    ~FirebaseHTTPClient();
    
    bool http_begin(const char* host, uint16_t port, const char* uri, const char* CAcert);
    bool http_connected(void);
    int http_sendRequest(const char* header, const char* payload);
    WiFiClient* http_getStreamPtr(void);
    uint16_t tcpTimeout = HTTPCLIENT_DEFAULT_TCP_TIMEOUT;


  protected:
    bool http_connect(void);
    bool http_sendHeader(const char* header);
    TransportTraitsPtr http_transportTraits;
    std::unique_ptr<WiFiClient> _tcp;

    char _host[200];
    char _uri[200];
    uint16_t _port = 0;
};



#endif /* FirebaseHTTPClient_H_ */
