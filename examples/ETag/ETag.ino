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

//This example shows how to set and delete data with checking the matching between node path ETag (unique identifier string)
//and provided Etag


#include <WiFi.h>
#include "FirebaseESP32.h"

#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"

//Define Firebase Data object
FirebaseData firebaseData;

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

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);

    String path = "/ESP32_Test";

    String ETag = "";
    String wrong_ETag = "ANY_WRONG_ETag";

    Serial.println("------------------------------------");
    Serial.println("Set integer without ETag test...");

    if (Firebase.setInt(firebaseData, path + "/Int/Data", 100))
    {
        Serial.println("PASSED");
        Serial.println("PATH: " + firebaseData.dataPath());
        Serial.println("TYPE: " + firebaseData.dataType());
        Serial.println("CURRENT ETag: " + firebaseData.ETag());
        ETag = firebaseData.ETag();
        Serial.print("VALUE: ");
        if (firebaseData.dataType() == "int")
            Serial.println(firebaseData.intData());
        else if (firebaseData.dataType() == "float")
            Serial.println(firebaseData.floatData(), 5);
        else if (firebaseData.dataType() == "double")
            printf("%.9lf\n", firebaseData.doubleData());
        else if (firebaseData.dataType() == "boolean")
            Serial.println(firebaseData.boolData() == 1 ? "true" : "false");
        else if (firebaseData.dataType() == "string")
            Serial.println(firebaseData.stringData());
        else if (firebaseData.dataType() == "json")
            Serial.println(firebaseData.jsonData());
        Serial.println("------------------------------------");
        Serial.println();
    }
    else
    {
        Serial.println("FAILED");
        Serial.println("REASON: " + firebaseData.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
    }

    Serial.println("------------------------------------");
    Serial.println("Set integer with valid ETag test...");

    if (Firebase.setInt(firebaseData, path + "/Int/Data", 200))
    {
        Serial.println("PASSED");
        Serial.println("PATH: " + firebaseData.dataPath());
        Serial.println("TYPE: " + firebaseData.dataType());
        Serial.println("CURRENT ETag: " + firebaseData.ETag());
        ETag = firebaseData.ETag();
        Serial.print("VALUE: ");
        if (firebaseData.dataType() == "int")
            Serial.println(firebaseData.intData());
        else if (firebaseData.dataType() == "float")
            Serial.println(firebaseData.floatData(), 5);
        else if (firebaseData.dataType() == "double")
            printf("%.9lf\n", firebaseData.doubleData());
        else if (firebaseData.dataType() == "boolean")
            Serial.println(firebaseData.boolData() == 1 ? "true" : "false");
        else if (firebaseData.dataType() == "string")
            Serial.println(firebaseData.stringData());
        else if (firebaseData.dataType() == "json")
            Serial.println(firebaseData.jsonData());
        Serial.println("------------------------------------");
        Serial.println();
    }
    else
    {
        Serial.println("FAILED");
        Serial.println("REASON: " + firebaseData.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
    }

    Serial.println("------------------------------------");
    Serial.println("Set integer with wrong ETag test...");

    if (Firebase.setInt(firebaseData, path + "/Int/Data", 300, wrong_ETag))
    {
        Serial.println("PASSED");
        Serial.println("PATH: " + firebaseData.dataPath());
        Serial.println("TYPE: " + firebaseData.dataType());
        Serial.println("ETag: " + firebaseData.ETag());
        ETag = firebaseData.ETag();
        Serial.print("VALUE: ");
        if (firebaseData.dataType() == "int")
            Serial.println(firebaseData.intData());
        else if (firebaseData.dataType() == "float")
            Serial.println(firebaseData.floatData(), 5);
        else if (firebaseData.dataType() == "double")
            printf("%.9lf\n", firebaseData.doubleData());
        else if (firebaseData.dataType() == "boolean")
            Serial.println(firebaseData.boolData() == 1 ? "true" : "false");
        else if (firebaseData.dataType() == "string")
            Serial.println(firebaseData.stringData());
        else if (firebaseData.dataType() == "json")
            Serial.println(firebaseData.jsonData());
        Serial.println("------------------------------------");
        Serial.println();
    }
    else
    {
        Serial.println("FAILED");
        Serial.println("REASON: " + firebaseData.errorReason());

        //If provided ETag is not match to current ETag (httpCode 412)
        if (firebaseData.httpCode() == 412)
        {
            Serial.println("PATH: " + firebaseData.dataPath());
            Serial.println("TYPE: " + firebaseData.dataType());
            Serial.println("PROVIDED ETag: " + wrong_ETag);
            Serial.println("CURRENT ETag: " + firebaseData.ETag());
            ETag = firebaseData.ETag();
            Serial.print("CURRENT VALUE: ");
            if (firebaseData.dataType() == "int")
                Serial.println(firebaseData.intData());
            else if (firebaseData.dataType() == "float")
                Serial.println(firebaseData.floatData(), 5);
            else if (firebaseData.dataType() == "double")
                printf("%.9lf\n", firebaseData.doubleData());
            else if (firebaseData.dataType() == "boolean")
                Serial.println(firebaseData.boolData() == 1 ? "true" : "false");
            else if (firebaseData.dataType() == "string")
                Serial.println(firebaseData.stringData());
            else if (firebaseData.dataType() == "json")
                Serial.println(firebaseData.jsonData());
        }

        Serial.println("------------------------------------");
        Serial.println();
    }

    Serial.println("------------------------------------");
    Serial.println("Delete node with wrong ETag test...");
    if (Firebase.deleteNode(firebaseData, path + "/Int/Data", wrong_ETag))
    {

        Serial.println("PASSED");
        Serial.println("CURRENT ETag: " + firebaseData.ETag());
        Serial.println("------------------------------------");
        Serial.println();
    }
    else
    {

        Serial.println("FAILED");
        Serial.println("REASON: " + firebaseData.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
    }
}

void loop()
{
}
