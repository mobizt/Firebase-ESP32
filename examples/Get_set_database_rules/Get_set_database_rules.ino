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


//This example shows how to read and write database rules


#include <WiFi.h>
#include <FirebaseESP32.h>

#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"

//Define Firebase Data object
FirebaseData firebaseData;

void printRulesContent(FirebaseData &data);

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

  String rules = "";

  Serial.println("------------------------------------");
  Serial.println("Read Database Rules test...");

  if (Firebase.getRules(firebaseData))
  {
    Serial.println("PASSED");
    Serial.println("DATABASE RULES: ");
    Serial.println(firebaseData.jsonData());
    rules = firebaseData.jsonData();
    Serial.println("------------------------------------");
    Serial.println("PARSE: ");
    printRulesContent(firebaseData);
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  return;

  Serial.println("------------------------------------");
  Serial.println("Write Database Rules test...");

  if (Firebase.setRules(firebaseData, rules))
  {
    Serial.println("PASSED");
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

  //Quit Firebase and release all resources
  Firebase.end(firebaseData);
  
}

void loop()
{
}


void printRulesContent(FirebaseData &data){
  size_t tokenCount = data.jsonObject().parse(false).getJsonObjectIteratorCount();
  String key;
  String value;
  FirebaseJsonObject jsonParseResult;
  Serial.println();
  for (size_t i = 0; i < tokenCount; i++)
  {
    data.jsonObject().jsonObjectiterator(i, key, value);
    value.replace("\n","");
    value.replace(" ","");
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
