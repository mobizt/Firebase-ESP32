/**
 * Created by K. Suwatchai (Mobizt)
 *
 * Email: k_suwatchai@hotmail.com
 *
 * Github: https://github.com/mobizt/Firebase-ESP-Client
 *
 * Copyright (c) 2023 mobizt
 *
 */

// This example shows how to retain the past RTDB data within the period from present.

#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

// For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "API_KEY"

/* 3. Define the RTDB URL */
#define DATABASE_URL "URL" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* database secret used in Firebase.setQueryIndex function */
#define DATABASE_SECRET "DATABASE_SECRET"

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long deleteDataMillis = 0, pushDataMillis = 0;

int count = 0;
bool indexing = false;

void setup()
{

    Serial.begin(115200);
    Serial.println();
    Serial.println();

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    // For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

    /* Assign the api key (required) */
    config.api_key = API_KEY;

    /* Assign the user sign in credentials */
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    /* Assign the RTDB URL (required) */
    config.database_url = DATABASE_URL;

    /* Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

    // Comment or pass false value when WiFi reconnection will control by your code or third party library e.g. WiFiManager
    Firebase.reconnectNetwork(true);

    // Since v4.4.x, BearSSL engine was used, the SSL buffer need to be set.
    // Large data transmission may require larger RX buffer, otherwise connection issue or data read time out can be occurred.
    fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

    Firebase.begin(&config, &auth);

    // NTP time sync
    configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
}

void loop()
{

    // Firebase.ready() should be called repeatedly to handle authentication tasks.

    if (Firebase.ready())
    {
        if (!indexing)
        {
            indexing = true;
            Serial.print("Set query index in database rules... ");

            // If sign in with OAuth2.0 token, the database secret pass to this function can be empty string
            if (Firebase.setQueryIndex(fbdo, "test/log", "ts", DATABASE_SECRET))
                Serial.println("ok");
            else
                Serial.println(fbdo.errorReason());
        }

        // push data every 30 sec
        if (millis() - pushDataMillis > 30 * 1000)
        {
            pushDataMillis = millis();

            count++;

            FirebaseJson json;

            json.add("ts", (uint32_t)time(nullptr));
            json.add("count", count);

            Serial.print("Push data... ");

            if (Firebase.push(fbdo, "test/log", json))
                Serial.println("ok");
            else
                Serial.println(fbdo.errorReason());
        }

        // delete old data every 1 min
        if (time(nullptr) > 1618971013 /* timestamp should be valid */ && millis() - deleteDataMillis > 60 * 1000)
        {
            deleteDataMillis = millis();

            Serial.print("Delete history data older than 10 minutes... ");

            if (Firebase.deleteNodesByTimestamp(fbdo, "test/log", "ts", 10 /* delete 10 nodes at once */, 10 * 60 /* retain data within 10 minutes */))
                Serial.println("ok");
            else
                Serial.println(fbdo.errorReason());
        }
    }
}