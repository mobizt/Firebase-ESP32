
/**
 * Created by K. Suwatchai (Mobizt)
 *
 * Email: k_suwatchai@hotmail.com
 *
 * Github: https://github.com/mobizt/Firebase-ESP32
 *
 * Copyright (c) 2022 mobizt
 *
 */

/** This example shows the basic RTDB usage with external Client.
 * This example used ESP32 device and WiFiClientSecure as the client.
 */

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

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;

WiFiClientSecure ssl_client;

void networkConnection()
{
    // Reset the network connection
    WiFi.disconnect();

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    unsigned long ms = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial_Printf("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();
}

// Define the callback function to handle server status acknowledgement
void networkStatusRequestCallback()
{
    // Set the network status
    fbdo.setNetworkStatus(WiFi.status() == WL_CONNECTED);
}

// Define the callback function to handle server connection
void tcpConnectionRequestCallback(const char *host, int port)
{

    // You may need to set the system timestamp to use for
    // auth token expiration checking.

    Serial.print("Connecting to server via external Client... ");
    if (!ssl_client.connect(host, port))
    {
        Serial.println("failed.");
        return;
    }
    Serial.println("success.");
}

void setup()
{

    Serial.begin(115200);

    networkConnection();

    Serial_Printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    ssl_client.setInsecure();

    /* Assign the api key (required) */
    config.api_key = API_KEY;

    /* Assign the user sign in credentials */
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    /* Assign the RTDB URL (required) */
    config.database_url = DATABASE_URL;

    /* Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

    /* fbdo.setExternalClient and fbdo.setExternalClientCallbacks must be called before Firebase.begin */

    /* Assign the pointer to global defined external SSL Client object */
    fbdo.setExternalClient(&ssl_client);

    /* Assign the required callback functions */
    fbdo.setExternalClientCallbacks(tcpConnectionRequestCallback, networkConnection, networkStatusRequestCallback);

    // Comment or pass false value when WiFi reconnection will control by your code or third party library
    Firebase.reconnectWiFi(true);

    Firebase.setDoubleDigits(5);

    Firebase.begin(&config, &auth);
}

void loop()
{

    // Firebase.ready() should be called repeatedly to handle authentication tasks.

    if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
    {
        sendDataPrevMillis = millis();

        Serial_Printf("Set bool... %s\n", Firebase.setBool(fbdo, F("/test/bool"), count % 2 == 0) ? "ok" : fbdo.errorReason().c_str());

        count++;
    }
}
