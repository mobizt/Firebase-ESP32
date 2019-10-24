
/*
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt
 * 
 * Copyright (c) 2019 mobizt
 * 
 * This example is for FirebaseESP8266 Arduino library v 2.6.0 and later
 *
*/

//This example shows how to set array data through FirebaseJsonArray object then read the data back and parse them.

//FirebaseESP8266.h must be included before ESP8266WiFi.h
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>

#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"


//Define Firebase Data Object
FirebaseData firebaseData;


FirebaseJsonArray arr;

void printResult(FirebaseData &data);


unsigned long sendDataPrevMillis = 0;

String path = "/Test/Array";

uint16_t count = 0;

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

    if (!Firebase.beginStream(firebaseData, path))
    {
        Serial.println("------------------------------------");
        Serial.println("Can't begin stream connection...");
        Serial.println("REASON: " + firebaseData.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
    }
}

void loop()
{

    if (millis() - sendDataPrevMillis > 15000)
    {
        sendDataPrevMillis = millis();
        count++;

        Serial.println("------------------------------------");
        Serial.println("Set Array...");

        arr.clear();
        arr.set("/[0]", count);
        arr.set("/[1]", "hello");
        arr.set("/[4]", 76.54);
        if (Firebase.set(firebaseData, path + "/Data", arr))
        {
            Serial.println("PASSED");
            Serial.println("PATH: " + firebaseData.dataPath());
            Serial.println("TYPE: " + firebaseData.dataType());
            Serial.print("VALUE: ");
            printResult(firebaseData);
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
        Serial.println("Get Array...");
        if (Firebase.get(firebaseData,  path + "/Data"))
        {   
            Serial.println("PASSED");
            Serial.println("PATH: " + firebaseData.dataPath());
            Serial.println("TYPE: " + firebaseData.dataType());
            Serial.print("VALUE: ");
            printResult(firebaseData);               
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

    if (!Firebase.readStream(firebaseData))
    {
        Serial.println("------------------------------------");
        Serial.println("Can't read stream data...");
        Serial.println("REASON: " + firebaseData.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
    }

    if (firebaseData.streamTimeout())
    {
        Serial.println("Stream timeout, resume streaming...");
        Serial.println();
    }

    if (firebaseData.streamAvailable())
    {
        Serial.println("------------------------------------");
        Serial.println("Stream Data available...");
        Serial.println("STREAM PATH: " + firebaseData.streamPath());
        Serial.println("EVENT PATH: " + firebaseData.dataPath());
        Serial.println("DATA TYPE: " + firebaseData.dataType());
        Serial.println("EVENT TYPE: " + firebaseData.eventType());
        Serial.print("VALUE: ");
        printResult(firebaseData);
        Serial.println("------------------------------------");
        Serial.println();
    }
}

void printResult(FirebaseData &data)
{

    if (data.dataType() == "int")
        Serial.println(data.intData());
    else if (data.dataType() == "float")
        Serial.println(data.floatData(), 5);
    else if (data.dataType() == "double")
        printf("%.9lf\n", data.doubleData());
    else if (data.dataType() == "boolean")
        Serial.println(data.boolData() == 1 ? "true" : "false");
    else if (data.dataType() == "string")
        Serial.println(data.stringData());
    else if (data.dataType() == "json")
    {
        Serial.println();
        FirebaseJson &json = data.jsonObject();
        //Print all object data
        Serial.println("Pretty printed JSON data:");
        String jsonStr;
        json.toString(jsonStr, true);
        Serial.println(jsonStr);
        Serial.println();
        Serial.println("Iterate JSON data:");
        Serial.println();
        size_t len = json.iteratorBegin();
        String key, value = "";
        int type = 0;
        for (size_t i = 0; i < len; i++)
        {
            json.iteratorGet(i, type, key, value);
            Serial.print(i);
            Serial.print(", ");
            Serial.print("Type: ");
            Serial.print(type == JSON_OBJECT ? "object" : "array");
            if (type == JSON_OBJECT)
            {
                Serial.print(", Key: ");
                Serial.print(key);
            }
            Serial.print(", Value: ");
            Serial.println(value);
        }
        json.iteratorEnd();
    }
    else if (data.dataType() == "array")
    {
        Serial.println();
        //get array data from FirebaseData using FirebaseJsonArray object
        FirebaseJsonArray &arr = data.jsonArray();
        //Print all array values
        Serial.println("Pretty printed Array:");
        String arrStr;
        arr.toString(arrStr, true);
        Serial.println(arrStr);
        Serial.println();
        Serial.println("Iterate array values:");
        Serial.println();
        for (size_t i = 0; i < arr.size(); i++)
        {
            Serial.print(i);
            Serial.print(", Value: ");

            FirebaseJsonData &jsonData = data.jsonData();
            //Get the result data from FirebaseJsonArray object
            arr.get(jsonData, i);
            if (jsonData.typeNum == JSON_BOOL)
                Serial.println(jsonData.boolValue ? "true" : "false");
            else if (jsonData.typeNum == JSON_INT)
                Serial.println(jsonData.intValue);
            else if (jsonData.typeNum == JSON_DOUBLE)
                printf("%.9lf\n", jsonData.doubleValue);
            else if (jsonData.typeNum == JSON_STRING ||
                     jsonData.typeNum == JSON_NULL ||
                     jsonData.typeNum == JSON_OBJECT ||
                     jsonData.typeNum == JSON_ARRAY)
                Serial.println(jsonData.stringValue);
        }
    }
}