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


//This example shows how to construct queries to filter data.


#include <WiFi.h>
#include <FirebaseESP32.h>

#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"

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

 FirebaseJson json;

  Serial.println("------------------------------------");
  Serial.println("Push JSON test...");

  for (uint8_t i = 0; i < 30; i++)
  {
    json.clear().addInt("Data1",i + 1).addInt("Data2",i + 100);

    if (Firebase.pushJSON(firebaseData, "/Test/Int", json))
    {
      Serial.println("PASSED");
      Serial.println("PATH: " + firebaseData.dataPath());
      Serial.print("PUSH NAME: ");
      Serial.println(firebaseData.pushName());
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

  QueryFilter query;

  query.orderBy("Data2");
  //query.orderBy("Data1");
  query.startAt("110");
  //query.startAt(5);
  query.endAt("115");
  //query.endAt(20);
  query.limitToLast(3);

  /*

  Begin data filtering test

  Add the following rules in Firebase Database Rules Dashboard

  "rules": {
    ...
    ...
    ,

    "Test":{
      "Int":{
        ".indexOn":"Data2"
        //".indexOn":"Data1"
      }
    }
  }

  */

  Serial.println("------------------------------------");
  Serial.println("Data Filtering test...");

  if (Firebase.getJSON(firebaseData, "/Test/Int", query))
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

  //Clear all query parameters
  query.clear();
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
