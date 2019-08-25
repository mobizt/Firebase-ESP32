/*
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt
 * 
 * Copyright (c) 2019 mobizt
 *
*/

//This example shows how to send Firebase Cloud Messaging.

#include <WiFi.h>
#include <FirebaseESP32.h>

#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"

#define FIREBASE_FCM_SERVER_KEY "YOUR_FIREBASE_PROJECT_CLOUD_MESSAGING_SERVER_KEY"
#define FIREBASE_FCM_DEVICE_TOKEN_1 "RECIPIENT_DEVICE_TOKEN"
#define FIREBASE_FCM_DEVICE_TOKEN_2 "ANOTHER_RECIPIENT_DEVICE_TOKEN"

FirebaseData firebaseData1;

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

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);

    firebaseData1.fcm.begin(FIREBASE_FCM_SERVER_KEY);

    firebaseData1.fcm.addDeviceToken(FIREBASE_FCM_DEVICE_TOKEN_1);

    firebaseData1.fcm.addDeviceToken(FIREBASE_FCM_DEVICE_TOKEN_2);

    firebaseData1.fcm.setPriority("high");

    firebaseData1.fcm.setTimeToLive(1000);

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

    Serial.println("------------------------------------");
    Serial.println("Send Firebase Cloud Messaging...");

    firebaseData1.fcm.setNotifyMessage("Notification", "Hello World! " + String(count));

    firebaseData1.fcm.setDataMessage("{\"myData\":" + String(count) + "}");

    //if (Firebase.broadcastMessage(firebaseData1))
    //if (Firebase.sendTopic(firebaseData1))
    if (Firebase.sendMessage(firebaseData1, 0))//send message to recipient index 0
    {

        Serial.println("PASSED");
        Serial.println(firebaseData1.fcm.getSendResult());
        Serial.println("------------------------------------");
        Serial.println();
    }
    else
    {
        Serial.println("FAILED");
        Serial.println("REASON: " + firebaseData1.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
    }

    count++;
}
