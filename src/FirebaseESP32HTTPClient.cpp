/*
 * Customized version of ESP32 HTTPClient Library. 
 * Allow custom header and payload with STARTTLS support
 * 
 * v 1.0.2
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

#ifndef FirebaseESP32HTTPClient_CPP
#define FirebaseESP32HTTPClient_CPP

#ifdef ESP32

#include "FirebaseESP32HTTPClient.h"

FirebaseESP32HTTPClient::FirebaseESP32HTTPClient()
{
    transportTraits = TransportTraitsPtr(new TLSTraits(nullptr));
}

FirebaseESP32HTTPClient::~FirebaseESP32HTTPClient()
{
    if (_wcs)
    {
        _wcs->stop();
        _wcs.reset();
        _wcs.release();
    }
    std::string().swap(_host);
    std::string().swap(_rootCAFile);
    _cacert.reset(new char);
    _cacert = nullptr;
    transportTraits.reset(nullptr);
}

bool FirebaseESP32HTTPClient::begin(const char *host, uint16_t port)
{
    _host = host;
    _port = port;
    return true;
}

bool FirebaseESP32HTTPClient::connected()
{
    if (_wcs)
        return (_wcs->connected());
    return false;
}

bool FirebaseESP32HTTPClient::send(const char *header)
{
    if (!connected())
        return false;
    return (_wcs->write(header, strlen(header)) == strlen(header));
}

int FirebaseESP32HTTPClient::send(const char *header, const char *payload)
{
    size_t size = strlen(payload);
    if (strlen(header) > 0)
    {
        if (!connect())
        {
            return FIREBASE_ERROR_HTTPC_ERROR_CONNECTION_REFUSED;
        }

        if (!send(header))
        {
            return FIREBASE_ERROR_HTTPC_ERROR_SEND_HEADER_FAILED;
        }
    }

    if (size > 0)
    {
        if (_wcs->write(&payload[0], size) != size)
        {
            return FIREBASE_ERROR_HTTPC_ERROR_SEND_PAYLOAD_FAILED;
        }
    }

    return 0;
}

WiFiClient *FirebaseESP32HTTPClient::stream(void)
{
    if (connected())
        return _wcs.get();
    return nullptr;
}

bool FirebaseESP32HTTPClient::connect(void)
{

    if (!transportTraits)
        return false;

    if (!_wcs)
        _wcs = transportTraits->create();
    else
    {
        if (connected())
            _wcs->stop();
    }

    if (_wcs)
    {
        transportTraits->verify(*_wcs, _host.c_str());
        if (!_wcs->connect(_host.c_str(), _port))
            return false;
    }

    return connected();
}

void FirebaseESP32HTTPClient::setCACert(const char *rootCA)
{
    if (rootCA)
    {
        transportTraits.reset(nullptr);
        transportTraits = TransportTraitsPtr(new TLSTraits(rootCA));
        _certType = 1;
    }
    else
        _certType = 0;
}

void FirebaseESP32HTTPClient::setCertFile(std::string &rootCAFile, uint8_t storageType)
{

    if (rootCAFile.length() > 0)
    {
        bool t = false;
        _certType = 2;

        if (storageType == 0)
            t = SPIFFS.begin(true);
        else
            t = SD.begin();
        if (!t)
            return;

        File f;
        if (storageType == 0)
        {
            if (SPIFFS.exists(rootCAFile.c_str()))
                f = SPIFFS.open(rootCAFile.c_str(), FILE_READ);
        }
        else
        {
            if (SD.exists(rootCAFile.c_str()))
                f = SD.open(rootCAFile.c_str(), FILE_READ);
        }

        if (f)
        {
            size_t len = f.size();
            _cacert.reset(new char);
            _cacert = nullptr;
            _cacert = std::unique_ptr<char>(new char[len]);

            if (f.available())
                f.readBytes(_cacert.get(), len);

            f.close();

            transportTraits.reset(nullptr);
            transportTraits = TransportTraitsPtr(new TLSTraits(_cacert.get()));
        }
    }
}



#endif /* ESP32 */

#endif /* FirebaseESP32HTTPClient_CPP */
