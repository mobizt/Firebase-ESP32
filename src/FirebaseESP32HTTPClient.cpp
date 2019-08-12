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

#ifndef FirebaseESP32HTTPClient_CPP
#define FirebaseESP32HTTPClient_CPP

#include "FirebaseESP32HTTPClient.h"

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
        return std::unique_ptr<WiFiClient>(new WiFiClientSecure());
    }

    bool verify(WiFiClient &client, const char *host) override
    {
        WiFiClientSecure &wcs = static_cast<WiFiClientSecure &>(client);
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

FirebaseESP32HTTPClient::FirebaseESP32HTTPClient()
{
    transportTraits = TransportTraitsPtr(new TLSTraits(nullptr));
}

FirebaseESP32HTTPClient::~FirebaseESP32HTTPClient()
{
    if (_client)
        _client->stop();
    std::string().swap(_host);
    std::string().swap(_uri);
    std::string().swap(_rootCAFile);
    _cer.reset(new char);
    _cer = nullptr;
    transportTraits.reset(nullptr);
}

bool FirebaseESP32HTTPClient::begin(const char *host, uint16_t port, const char *uri)
{
    _host = host;
    _port = port;
    _uri = uri;

    return true;
}

bool FirebaseESP32HTTPClient::connected()
{
    if (_client)
        return ((_client->available() > 0) || _client->connected());
    return false;
}

bool FirebaseESP32HTTPClient::sendHeader(const char *header)
{
    if (!connected())
        return false;
    return (_client->write(header, strlen(header)) == strlen(header));
}

int FirebaseESP32HTTPClient::sendRequest(const char *header, const char *payload)
{
    size_t size = strlen(payload);
    if (strlen(header) > 0)
    {
        if (!connect())
            return HTTPC_ERROR_CONNECTION_REFUSED;
        if (!sendHeader(header))
            return HTTPC_ERROR_SEND_HEADER_FAILED;
    }
    if (size > 0)
        if (_client->write(&payload[0], size) != size)
            return HTTPC_ERROR_SEND_PAYLOAD_FAILED;

    return 0;
}

WiFiClient *FirebaseESP32HTTPClient::getStreamPtr(void)
{
    if (connected())
        return _client.get();
    return nullptr;
}

bool FirebaseESP32HTTPClient::connect(void)
{
    if (connected())
    {
        while (_client->available() > 0)
            _client->read();
        return true;
    }

    if (!transportTraits)
        return false;

    _client = transportTraits->create();

    if (!transportTraits->verify(*_client, _host.c_str()))
    {
        _client->stop();
        return false;
    }

    if (!_client->connect(_host.c_str(), _port))
        return false;

    return connected();
}

void FirebaseESP32HTTPClient::setRootCA(const char *rootCA)
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

void FirebaseESP32HTTPClient::setRootCAFile(std::string &rootCAFile, uint8_t storageType)
{

    if (rootCAFile.length() > 0)
    {
        bool t = false;
        _certType = 2;

        if (storageType == 0)
            t = SPIFFS.begin();
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
            _cer.reset(new char);
            _cer = nullptr;
            _cer = std::shared_ptr<char>(new char[len]);

            if (f.available())
                f.readBytes(_cer.get(), len);

            f.close();

            transportTraits.reset(nullptr);
            transportTraits = TransportTraitsPtr(new TLSTraits(_cer.get()));
        }
    }
}

#endif
