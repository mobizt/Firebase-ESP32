
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


//This example shows how to set stream event callback functions.
//Two events are available from Firebase's Real Time database HTTP stream connection, dataAvailable and streamTimeout.


#include <WiFi.h>
#include <FirebaseESP32.h>

#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"


//Define FirebaseESP8266 data object
FirebaseData firebaseData1;
FirebaseData firebaseData2;

void printJsonObjectContent(FirebaseData &data);
void printJsonObjectContent2(StreamData &data);

unsigned long sendDataPrevMillis = 0;

String path = "/ESP32_Test/Stream";

uint16_t count = 0;

void streamCallback(StreamData data)
{

  Serial.println("Stream Data1 available...");
  Serial.println("STREAM PATH: " + data.streamPath());
  Serial.println("EVENT PATH: " + data.dataPath());
  Serial.println("DATA TYPE: " + data.dataType());
  Serial.println("EVENT TYPE: " + data.eventType());
  Serial.print("VALUE: ");
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
    printJsonObjectContent2(data);
  else if (data.dataType() == "blob")
  {
    //See blob examples
  }
  Serial.println();
}

void streamTimeoutCallback(bool timeout)
{
  if (timeout)
  {
    Serial.println();
    Serial.println("Stream timeout, resume streaming...");
    Serial.println();
  }
}

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

  if (!Firebase.beginStream(firebaseData1, path))
  {
    Serial.println("------------------------------------");
    Serial.println("Can't begin stream connection...");
    Serial.println("REASON: " + firebaseData1.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  Firebase.setStreamCallback(firebaseData1, streamCallback, streamTimeoutCallback);
}

void loop()
{
  
  if (millis() - sendDataPrevMillis > 15000)
  {
    sendDataPrevMillis = millis();
    count++;

    Serial.println("------------------------------------");
    Serial.println("Set JSON...");

    FirebaseJson json;
    json.addString("data","hello").addInt("num", count);
    if (Firebase.setJSON(firebaseData2, path + "/Json", json))
    {
      Serial.println("PASSED");
      Serial.println("PATH: " + firebaseData2.dataPath());
      Serial.println("TYPE: " + firebaseData2.dataType());
      Serial.print("VALUE: ");
      if (firebaseData2.dataType() == "int")
        Serial.println(firebaseData2.intData());
      else if (firebaseData2.dataType() == "float")
        Serial.println(firebaseData2.floatData(), 5);
      else if (firebaseData2.dataType() == "double")
        printf("%.9lf\n", firebaseData2.doubleData());
      else if (firebaseData2.dataType() == "boolean")
        Serial.println(firebaseData2.boolData() == 1 ? "true" : "false");
      else if (firebaseData2.dataType() == "string")
        Serial.println(firebaseData2.stringData());
      else if (firebaseData2.dataType() == "json")
        printJsonObjectContent(firebaseData2);
      Serial.println("------------------------------------");
      Serial.println();
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + firebaseData2.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }

  }
}

void printJsonObjectContent(FirebaseData &data){
  size_t tokenCount = data.jsonObject().parse(false).getJsonObjectIteratorCount();
  Serial.println(data.jsonData());
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

void printJsonObjectContent2(StreamData &data){
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

