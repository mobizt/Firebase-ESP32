/*
 * Customized version of ESP32 HTTPClient Library. 
 * Allow custom header and payload with STARTTLS support
 * 
 * v 1.0.0
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

#include <Arduino.h>
#include <HTTPClient.h>

#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include "FS.h"
#include "SPIFFS.h"
#include "SD.h"

class FirebaseESP32HTTPClient : public HTTPClient
{

friend class FirebaseESP8266;
friend class FirebaseData;
friend class FCMObject;

public:
  FirebaseESP32HTTPClient();
  ~FirebaseESP32HTTPClient();

  /**
    * Initialization of new http connection.
    * \param host - Host name without protocols.
    * \param port - Server's port.
    * \param uri - The URI of resource.
    * \return True as default.
    * If no certificate string provided, use (const char*)NULL to CAcert param 
    */
  bool begin(const char *host, uint16_t port, const char *uri);

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

  uint16_t tcpTimeout = HTTPCLIENT_DEFAULT_TCP_TIMEOUT;
  bool connect(void);
  void setRootCA(const char *rootCA);
  void setRootCAFile(std::string &rootCAFile, uint8_t storageType);

  int _certType = -1;
  std::string _rootCAFile = "";
  uint8_t _rootCAFileStoreageType = 0;

protected:
  TransportTraitsPtr transportTraits;
  std::unique_ptr<WiFiClient> _client;
  std::shared_ptr<char> _cer;

  std::string _host = "";
  std::string _uri = "";
  uint16_t _port = 0;
};

#endif /* FirebaseESP32HTTPClient_H_ */
