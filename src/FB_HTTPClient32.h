/**
 * Customized version of ESP32 HTTPClient Library. 
 * Allow custom header and payload with STARTTLS support
 * 
 * v 1.0.5
 * 
 * The MIT License (MIT)
 * Copyright (c) 2021 K. Suwatchai (Mobizt)
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

#ifndef FB_HTTPClient32_H
#define FB_HTTPClient32_H

#ifdef ESP32

#include <Arduino.h>
#include <WiFiClient.h>
#include <FS.h>
#include <SPIFFS.h>
#include <SD.h>
#include "wcs/FB_WCS32.h"
#if __has_include(<WiFiEspAT.h>) || __has_include(<espduino.h>)
#error WiFi UART bridge was not supported.
#endif


#define FLASH_FS SPIFFS

#define FIREBASE_DEFAULT_TCP_TIMEOUT_SEC 5


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
#define FIREBASE_ERROR_BUFFER_OVERFLOW (-12)
#define FIREBASE_ERROR_DATA_TYPE_MISMATCH -13
#define FIREBASE_ERROR_PATH_NOT_EXIST -14
#define FIREBASE_ERROR_HTTPC_ERROR_CONNECTION_INUSED -15
#define FIREBASE_ERROR_HTTPC_NO_FCM_TOPIC_PROVIDED -16
#define FIREBASE_ERROR_HTTPC_NO_FCM_DEVICE_TOKEN_PROVIDED -17
#define FIREBASE_ERROR_HTTPC_NO_FCM_SERVER_KEY_PROVIDED -18
#define FIREBASE_ERROR_HTTPC_NO_FCM_INDEX_NOT_FOUND_IN_DEVICE_TOKEN_PROVIDED -19
#define FIREBASE_ERROR_HTTPC_MAX_REDIRECT_REACHED -20
#define FIREBASE_ERROR_EXPECTED_JSON_DATA -21
#define FIREBASE_ERROR_CANNOT_CONFIG_TIME -22
#define FIREBASE_ERROR_SSL_RX_BUFFER_SIZE_TOO_SMALL -23
#define FIREBASE_ERROR_FILE_IO_ERROR -24
#define FIREBASE_ERROR_UNINITIALIZED -25

#define FIREBASE_ERROR_TOKEN_SET_TIME -26
#define FIREBASE_ERROR_TOKEN_CREATE_HASH -27
#define FIREBASE_ERROR_TOKEN_PARSE_PK -28
#define FIREBASE_ERROR_TOKEN_SIGN -29
#define FIREBASE_ERROR_TOKEN_EXCHANGE -30
#define FIREBASE_ERROR_TOKEN_NOT_READY -31

/// HTTP codes see RFC7231

#define FIREBASE_ERROR_HTTP_CODE_OK 200
#define FIREBASE_ERROR_HTTP_CODE_NON_AUTHORITATIVE_INFORMATION 203
#define FIREBASE_ERROR_HTTP_CODE_NO_CONTENT 204
#define FIREBASE_ERROR_HTTP_CODE_MOVED_PERMANENTLY 301
#define FIREBASE_ERROR_HTTP_CODE_FOUND 302
#define FIREBASE_ERROR_HTTP_CODE_USE_PROXY 305
#define FIREBASE_ERROR_HTTP_CODE_TEMPORARY_REDIRECT 307
#define FIREBASE_ERROR_HTTP_CODE_PERMANENT_REDIRECT 308
#define FIREBASE_ERROR_HTTP_CODE_BAD_REQUEST 400
#define FIREBASE_ERROR_HTTP_CODE_UNAUTHORIZED 401
#define FIREBASE_ERROR_HTTP_CODE_FORBIDDEN 403
#define FIREBASE_ERROR_HTTP_CODE_NOT_FOUND 404
#define FIREBASE_ERROR_HTTP_CODE_METHOD_NOT_ALLOWED 405
#define FIREBASE_ERROR_HTTP_CODE_NOT_ACCEPTABLE 406
#define FIREBASE_ERROR_HTTP_CODE_PROXY_AUTHENTICATION_REQUIRED 407
#define FIREBASE_ERROR_HTTP_CODE_REQUEST_TIMEOUT 408
#define FIREBASE_ERROR_HTTP_CODE_LENGTH_REQUIRED 411
#define FIREBASE_ERROR_HTTP_CODE_PRECONDITION_FAILED 412
#define FIREBASE_ERROR_HTTP_CODE_PAYLOAD_TOO_LARGE 413
#define FIREBASE_ERROR_HTTP_CODE_URI_TOO_LONG 414
#define FIREBASE_ERROR_HTTP_CODE_MISDIRECTED_REQUEST 421
#define FIREBASE_ERROR_HTTP_CODE_UNPROCESSABLE_ENTITY 422
#define FIREBASE_ERROR_HTTP_CODE_TOO_MANY_REQUESTS 429
#define FIREBASE_ERROR_HTTP_CODE_REQUEST_HEADER_FIELDS_TOO_LARGE 431
#define FIREBASE_ERROR_HTTP_CODE_INTERNAL_SERVER_ERROR 500
#define FIREBASE_ERROR_HTTP_CODE_NOT_IMPLEMENTED 501
#define FIREBASE_ERROR_HTTP_CODE_BAD_GATEWAY 502
#define FIREBASE_ERROR_HTTP_CODE_SERVICE_UNAVAILABLE 503
#define FIREBASE_ERROR_HTTP_CODE_GATEWAY_TIMEOUT 504
#define FIREBASE_ERROR_HTTP_CODE_HTTP_VERSION_NOT_SUPPORTED 505
#define FIREBASE_ERROR_HTTP_CODE_LOOP_DETECTED 508
#define FIREBASE_ERROR_HTTP_CODE_NETWORK_AUTHENTICATION_REQUIRED 511

class FB_ESP32_TransportTraits
{
public:
  virtual ~FB_ESP32_TransportTraits() {}

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

class TLSTraits : public FB_ESP32_TransportTraits
{
public:
  TLSTraits(const char *CAcert, const char *clicert = nullptr, const char *clikey = nullptr) : _cacert(CAcert), _clicert(clicert), _clikey(clikey) {}

  std::unique_ptr<WiFiClient> create() override
  {
    return std::unique_ptr<WiFiClient>(new FB_WCS32());
  }

  bool verify(WiFiClient &client, const char *host) override
  {
    FB_WCS32 &wcs = static_cast<FB_WCS32 &>(client);
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

typedef std::unique_ptr<FB_ESP32_TransportTraits> FB_ESP32_TransportTraitsPtr;

class FB_HTTPClient32
{

friend class FirebaseESP32;
friend class FirebaseData;
friend class FCMObject;

public:
  FB_HTTPClient32();
  ~FB_HTTPClient32();

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
  int send(const char *header, const char *payload);

  /**
    * Send extra header without making new http connection (if send with header has been called)
    * \param header - The header string (constant chars array).
    * \return True if header sending success.
    * Need to call send with header first. 
    */
  bool send(const char *header);

  /**
    * Get the WiFi client pointer.
    * \return WiFi client pointer.
    */
  WiFiClient *stream(void);

  uint16_t tcpTimeout = FIREBASE_DEFAULT_TCP_TIMEOUT_SEC * 1000;
  bool connect(void);
  void setCACert(const char *caCert);
  void setCertFile(std::string &caCertFile, uint8_t storageType);


  int _certType = -1;
  std::string _caCertFile = "";
  uint8_t _caCertFileStoreageType = 0;

protected:
  FB_ESP32_TransportTraitsPtr transportTraits;
  std::unique_ptr<WiFiClient> _wcs;
  std::unique_ptr<char> _cacert;

  std::string _host = "";
  uint16_t _port = 0;
};

#endif /* ESP32 */

#endif /* FirebaseESP32HTTPClient_H_ */
