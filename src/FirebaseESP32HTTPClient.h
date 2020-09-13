/*
 * Customized version of ESP32 HTTPClient Library. 
 * Allow custom header and payload with STARTTLS support
 * 
 * v 1.0.1
 * 
 * The MIT License (MIT)
 * Copyright (c) 2019 K. Suwatchai (Mobizt)
 * 
 * HTTPClient Arduino library for ESP32
 *
 * Copyright (c) 2015 Markus Sattler. All rights reserved.
 * This file is part of the HTTPClient for Arduino.
 * Port to ESP32 by Evandro Luis Copercini (2017), 
 * changed fingerprints to CA verification. 	
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
*/

#ifndef FirebaseESP32HTTPClient_H_
#define FirebaseESP32HTTPClient_H_

#ifdef ESP32

#include <Arduino.h>
#include <WiFiClient.h>
#include <FS.h>
#include <SPIFFS.h>
#include <SD.h>
#include "WiFiClientSecureESP32.h"
#if __has_include(<WiFiEspAT.h>) || __has_include(<espduino.h>)
#error WiFi UART bridge was not supported.
#endif

#define FIREBASE_DEFAULT_TCP_TIMEOUT 5000 //5 seconds

/// HTTP client errors
#define FIREBASE_ERROR_HTTPC_ERROR_CONNECTION_REFUSED (-1)
#define FIREBASE_ERROR_HTTPC_ERROR_SEND_HEADER_FAILED (-2)
#define FIREBASE_ERROR_HTTPC_ERROR_SEND_PAYLOAD_FAILED (-3)
#define FIREBASE_ERROR_HTTPC_ERROR_NOT_CONNECTED (-4)
#define FIREBASE_ERROR_HTTPC_ERROR_CONNECTION_LOST (-5)
#define FIREBASE_ERROR_HTTPC_ERROR_NO_STREAM (-6)
#define FIREBASE_ERROR_HTTPC_ERROR_NO_HTTP_SERVER (-7)
#define FIREBASE_ERROR_HTTPC_ERROR_TOO_LESS_RAM (-8)
#define FIREBASE_ERROR_HTTPC_ERROR_ENCODING (-9)
#define FIREBASE_ERROR_HTTPC_ERROR_STREAM_WRITE (-10)
#define FIREBASE_ERROR_HTTPC_ERROR_READ_TIMEOUT (-11)

#define FIREBASE_ERROR_HTTP_CODE_CONTINUE 100
#define FIREBASE_ERROR_HTTP_CODE_SWITCHING_PROTOCOLS 101
#define FIREBASE_ERROR_HTTP_CODE_PROCESSING 102
#define FIREBASE_ERROR_HTTP_CODE_OK 200
#define FIREBASE_ERROR_HTTP_CODE_CREATED 201
#define FIREBASE_ERROR_HTTP_CODE_ACCEPTED 202
#define FIREBASE_ERROR_HTTP_CODE_NON_AUTHORITATIVE_INFORMATION 203
#define FIREBASE_ERROR_HTTP_CODE_NO_CONTENT 204
#define FIREBASE_ERROR_HTTP_CODE_RESET_CONTENT 205
#define FIREBASE_ERROR_HTTP_CODE_PARTIAL_CONTENT 206
#define FIREBASE_ERROR_HTTP_CODE_MULTI_STATUS 207
#define FIREBASE_ERROR_HTTP_CODE_ALREADY_REPORTED 208
#define FIREBASE_ERROR_HTTP_CODE_IM_USED 226
#define FIREBASE_ERROR_HTTP_CODE_MULTIPLE_CHOICES 300
#define FIREBASE_ERROR_HTTP_CODE_MOVED_PERMANENTLY 301
#define FIREBASE_ERROR_HTTP_CODE_FOUND 302
#define FIREBASE_ERROR_HTTP_CODE_SEE_OTHER 303
#define FIREBASE_ERROR_HTTP_CODE_NOT_MODIFIED 304
#define FIREBASE_ERROR_HTTP_CODE_USE_PROXY 305
#define FIREBASE_ERROR_HTTP_CODE_TEMPORARY_REDIRECT 307
#define FIREBASE_ERROR_HTTP_CODE_PERMANENT_REDIRECT 308
#define FIREBASE_ERROR_HTTP_CODE_BAD_REQUEST 400
#define FIREBASE_ERROR_HTTP_CODE_UNAUTHORIZED 401
#define FIREBASE_ERROR_HTTP_CODE_PAYMENT_REQUIRED 402
#define FIREBASE_ERROR_HTTP_CODE_FORBIDDEN 403
#define FIREBASE_ERROR_HTTP_CODE_NOT_FOUND 404
#define FIREBASE_ERROR_HTTP_CODE_METHOD_NOT_ALLOWED 405
#define FIREBASE_ERROR_HTTP_CODE_NOT_ACCEPTABLE 406
#define FIREBASE_ERROR_HTTP_CODE_PROXY_AUTHENTICATION_REQUIRED 407
#define FIREBASE_ERROR_HTTP_CODE_REQUEST_TIMEOUT 408
#define FIREBASE_ERROR_HTTP_CODE_CONFLICT 409
#define FIREBASE_ERROR_HTTP_CODE_GONE 410
#define FIREBASE_ERROR_HTTP_CODE_LENGTH_REQUIRED 411
#define FIREBASE_ERROR_HTTP_CODE_PRECONDITION_FAILED 412
#define FIREBASE_ERROR_HTTP_CODE_PAYLOAD_TOO_LARGE 413
#define FIREBASE_ERROR_HTTP_CODE_URI_TOO_LONG 414
#define FIREBASE_ERROR_HTTP_CODE_UNSUPPORTED_MEDIA_TYPE 415
#define FIREBASE_ERROR_HTTP_CODE_RANGE_NOT_SATISFIABLE 416
#define FIREBASE_ERROR_HTTP_CODE_EXPECTATION_FAILED 417
#define FIREBASE_ERROR_HTTP_CODE_MISDIRECTED_REQUEST 421
#define FIREBASE_ERROR_HTTP_CODE_UNPROCESSABLE_ENTITY 422
#define FIREBASE_ERROR_HTTP_CODE_LOCKED 423
#define FIREBASE_ERROR_HTTP_CODE_FAILED_DEPENDENCY 424
#define FIREBASE_ERROR_HTTP_CODE_UPGRADE_REQUIRED 426
#define FIREBASE_ERROR_HTTP_CODE_PRECONDITION_REQUIRED 428
#define FIREBASE_ERROR_HTTP_CODE_TOO_MANY_REQUESTS 429
#define FIREBASE_ERROR_HTTP_CODE_REQUEST_HEADER_FIELDS_TOO_LARGE 431
#define FIREBASE_ERROR_HTTP_CODE_INTERNAL_SERVER_ERROR 500
#define FIREBASE_ERROR_HTTP_CODE_NOT_IMPLEMENTED 501
#define FIREBASE_ERROR_HTTP_CODE_BAD_GATEWAY 502
#define FIREBASE_ERROR_HTTP_CODE_SERVICE_UNAVAILABLE 503
#define FIREBASE_ERROR_HTTP_CODE_GATEWAY_TIMEOUT 504
#define FIREBASE_ERROR_HTTP_CODE_HTTP_VERSION_NOT_SUPPORTED 505
#define FIREBASE_ERROR_HTTP_CODE_VARIANT_ALSO_NEGOTIATES 506
#define FIREBASE_ERROR_HTTP_CODE_INSUFFICIENT_STORAGE 507
#define FIREBASE_ERROR_HTTP_CODE_LOOP_DETECTED 508
#define FIREBASE_ERROR_HTTP_CODE_NOT_EXTENDED 510
#define FIREBASE_ERROR_HTTP_CODE_NETWORK_AUTHENTICATION_REQUIRED 511

class TransportTraits
{
public:
  virtual ~TransportTraits() {}

  virtual std::unique_ptr<WiFiClient> create()
  {
    return std::unique_ptr<WiFiClient>(new WiFiClient());
  }

  virtual bool
  verify(WiFiClient &client, const char *host)
  {
    return true;
  }
};

class TLSTraits : public TransportTraits
{
public:
  TLSTraits(const char *CAcert, const char *clicert = nullptr, const char *clikey = nullptr) : _cacert(CAcert), _clicert(clicert), _clikey(clikey) {}

  std::unique_ptr<WiFiClient> create() override
  {
    return std::unique_ptr<WiFiClient>(new WiFiClientSecureESP32());
  }

  bool verify(WiFiClient &client, const char *host) override
  {
    WiFiClientSecureESP32 &wcs = static_cast<WiFiClientSecureESP32 &>(client);
    wcs.setCACert(_cacert);
    wcs.setCertificate(_clicert);
    wcs.setPrivateKey(_clikey);
    return true;
  }

protected:
  const char *_cacert;
  const char *_clicert;
  const char *_clikey;
};

typedef std::unique_ptr<TransportTraits> TransportTraitsPtr;

class FirebaseESP32HTTPClient
{

friend class FirebaseESP32;
friend class FirebaseData;
friend class FCMObject;

public:
  FirebaseESP32HTTPClient();
  ~FirebaseESP32HTTPClient();

  /**
    * Initialization of new http connection.
    * \param host - Host name without protocols.
    * \param port - Server's port.
    * \return True as default.
    * If no certificate string provided, use (const char*)NULL to CAcert param 
    */
  bool begin(const char *host, uint16_t port);

  /**
    * Check the http connection status.
    * \return True if connected.
    */
  bool connected();

  /**
    * Establish http connection if header provided and send it, send payload if provided.
    * \param header - The header string (constant chars array).
    * \param payload - The payload string (constant chars array), optional.
    * \return http status code, Return zero if new http connection and header and/or payload sent 
    * with no error or no header and payload provided. If obly payload provided, no new http connection was established.
    */
  int sendRequest(const char *header, const char *payload);

  /**
    * Send extra header without making new http connection (if sendRequest has been called)
    * \param header - The header string (constant chars array).
    * \return True if header sending success.
    * Need to call sendRequest with header first. 
    */
  bool sendHeader(const char *header);

  /**
    * Get the WiFi client pointer.
    * \return WiFi client pointer.
    */
  WiFiClient *getStreamPtr(void);

  uint16_t tcpTimeout = FIREBASE_DEFAULT_TCP_TIMEOUT;
  bool connect(void);
  void setRootCA(const char *rootCA);
  void setRootCAFile(std::string &rootCAFile, uint8_t storageType);

  int _certType = -1;
  std::string _rootCAFile = "";
  uint8_t _rootCAFileStoreageType = 0;

protected:
  TransportTraitsPtr transportTraits;
  std::unique_ptr<WiFiClient> _client;
  std::unique_ptr<char> _cer;

  std::string _host = "";
  uint16_t _port = 0;
};

#endif /* ESP32 */

#endif /* FirebaseESP32HTTPClient_H_ */
