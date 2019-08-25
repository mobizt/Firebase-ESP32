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

//This example shows how set node's priority and filtering the data based on priority of child nodes.
//The priority is virtual node with the key ".priority" and can't see in Console.

//Since data ordering is not supported in Firebase's REST APIs, then the query result will not sorted.


#include <WiFi.h>
#include <FirebaseESP32.h>

#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com"
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"
#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"


//Define Firebase Data object
FirebaseData firebaseData;

void printJsonObjectContent(FirebaseData &data);

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

    Serial.println("------------------------------------");
    Serial.println("Set priority test...");

    for (int i = 0; i < 15; i++)
    {

        float priority = 15 - i;
        FirebaseJson json;
        json.addString("item_" + String(i + 1),"value_" + String(i + 1));
        String Path = path + "/Items/priority_" + String(15 - i);

        if (Firebase.setJSON(firebaseData, Path, json, priority))
        {
            Serial.println("PASSED");
            Serial.println("PATH: " + firebaseData.dataPath());
            Serial.println("TYPE: " + firebaseData.dataType());
            Serial.println("CURRENT ETag: " + firebaseData.ETag());
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
                printJsonObjectContent(firebaseData);
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

    //Qury child nodes under "/ESP32_Test/Item" with priority between 3.0 and 8.0
    //Since data ordering is not supported in Firebase's REST APIs, then the query result will not sorted.
    QueryFilter query;
    query.orderBy("$priority").startAt(3.0).endAt(8.0);

    Serial.println("------------------------------------");
    Serial.println("Filtering based on priority test...");

    if (Firebase.getJSON(firebaseData, path + "/Items", query))
    {

        Serial.println("PASSED");
        Serial.println("JSON DATA: ");
        printJsonObjectContent(firebaseData);
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

void printJsonObjectContent(FirebaseData &data){
  size_t tokenCount = data.jsonObject().parse(false).getJsonObjectIteratorCount();
  String key;
  String value;
  FirebaseJsonObject jsonParseResult;
  Serial.println();
  for (size_t i = 0; i < tokenCount; i++)
  {
    data.jsonObject().jsonObjectiterator(i,key,value);
    jsonParseResult = data.jsonObject().parseResult();
    Serial.print("KEY: ");
    Serial.print(key);
    Serial.print(", ");
    Serial.print("VALUE: ");
    Serial.print(value); 
    Serial.print(", ");
    Serial.print("TYPE: ");
    Serial.println(jsonParseResult.type);        

  }
}