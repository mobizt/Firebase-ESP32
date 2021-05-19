/**
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt
 * 
 * Copyright (c) 2021 mobizt
 *
*/

//This example shows how to send Firebase Cloud Messaging.

#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define FIREBASE_FCM_SERVER_KEY "FIREBASE_PROJECT_CLOUD_MESSAGING_SERVER_KEY"
#define FIREBASE_FCM_DEVICE_TOKEN_1 "RECIPIENT_DEVICE_TOKEN"
#define FIREBASE_FCM_DEVICE_TOKEN_2 "ANOTHER_RECIPIENT_DEVICE_TOKEN"

//Define Firebase Data object
FirebaseData fbdo;

unsigned long lastTime = 0;

int count = 0;

void sendMessage();

void setup()
{

    Serial.begin(115200);

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

    Firebase.reconnectWiFi(true);

    fbdo.fcm.begin(FIREBASE_FCM_SERVER_KEY);

    fbdo.fcm.addDeviceToken(FIREBASE_FCM_DEVICE_TOKEN_1);

    fbdo.fcm.addDeviceToken(FIREBASE_FCM_DEVICE_TOKEN_2);

    fbdo.fcm.setPriority("high");

    fbdo.fcm.setTimeToLive(1000);

    sendMessage();
}

void loop()
{
    if (millis() - lastTime > 60 * 1000)
    {
        lastTime = millis();
        sendMessage();
    }
}

void sendMessage()
{

    fbdo.fcm.setNotifyMessage("Notification", "Hello World! " + String(count));

    fbdo.fcm.setDataMessage("{\"myData\":" + String(count) + "}");

    //Firebase.broadcastMessage(fbdo)
    //Firebase.sendTopic(fbdo)
    Serial.printf("Send message... %s\n", Firebase.sendMessage(fbdo, 0) ? "ok" : fbdo.errorReason().c_str());

    if (fbdo.httpCode() == FIREBASE_ERROR_HTTP_CODE_OK)
        Serial.println(fbdo.fcm.getSendResult());
        
    Serial.println();

    count++;
}
