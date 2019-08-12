
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
#include "FirebaseESP32.h"

#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"

//Define Firebase Data objects
FirebaseData firebaseData1;
FirebaseData firebaseData2;

unsigned long sendDataPrevMillis1;
unsigned long sendDataPrevMillis2;

uint16_t count1;
uint16_t count2;

String json = "";

String path = "/ESP32_Test";

void streamCallback1(StreamData data)
{

  Serial.println("> Stream Data1 available...");
  Serial.println("> STREAM PATH: " + data.streamPath());
  Serial.println("> EVENT PATH: " + data.dataPath());
  Serial.println("> DATA TYPE: " + data.dataType());
  Serial.println("> EVENT TYPE: " + data.eventType());
  Serial.print("> VALUE: ");
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
    Serial.println(data.jsonData());
  else if (data.dataType() == "blob")
  {
    //See blob examples
  }
  Serial.println();
}

void streamTimeoutCallback1(bool timeout)
{
  if (timeout)
  {
    Serial.println();
    Serial.println("> Stream 1 timeout, resume streaming...");
    Serial.println();
  }
}

void streamCallback2(StreamData data)
{

  Serial.println(">> Stream Data2 available...");
  Serial.println(">> STREAM PATH: " + data.streamPath());
  Serial.println(">> EVENT PATH: " + data.dataPath());
  Serial.println(">> DATA TYPE: " + data.dataType());
  Serial.println(">> EVENT TYPE: " + data.eventType());
  Serial.print(">> VALUE: ");
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
    Serial.println(data.jsonData());
  else if (data.dataType() == "blob")
  {
    //See blob examples
  }
  Serial.println();
}

void streamTimeoutCallback2(bool timeout)
{
  if (timeout)
  {
    Serial.println();
    Serial.println(">> Stream 2 timeout, resume streaming...");
    Serial.println();
  }
}

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

  Serial.println("------------------------------------");
  Serial.println("> Begin stream 1...");

  if (!Firebase.beginStream(firebaseData1, path + "/Stream/data1"))
  {
    Serial.println("> FAILED");
    Serial.println("> REASON: " + firebaseData1.errorReason());
    Serial.println();
  }
  else
  {
    Serial.println("> PASSED");
    Serial.println("------------------------------------");
    Serial.println();
  }

  Firebase.setStreamCallback(firebaseData1, streamCallback1, streamTimeoutCallback1);

  //When stream timeou callback function is omitted, use Firebase.setStreamCallback(firebaseData1, streamCallback1);

  Serial.println("------------------------------------");
  Serial.println(">> Begin stream 2...");

  if (!Firebase.beginStream(firebaseData2, path + "/Stream/data2"))
  {
    Serial.println(">> FAILED");
    Serial.println(">> REASON: " + firebaseData2.errorReason());
    Serial.println();
  }
  else
  {
    Serial.println(">> PASSED");
    Serial.println("------------------------------------");
    Serial.println();
  }

  Firebase.setStreamCallback(firebaseData2, streamCallback2, streamTimeoutCallback2);

  //When stream timeout callback function is omitted, use Firebase.setStreamCallback(firebaseData2, streamCallback2);
}

void loop()
{

  //This example uses the same Firebase Data object to read/store data (get, set, update, push and delete) and stream.
  //This causes some delay (for start new SSL connection) for swiching between read/store and stream operation.
  //For no delay, see Different_objects_stream.ino example which uses different Firebase Data object for read/store and stream data.


  if (millis() - sendDataPrevMillis1 > 28000)
  {
    sendDataPrevMillis1 = millis();
    count1++;

    json = "{\"data1-1\":" + String(count1) + ",\"data1-2\":" + String(count1 + 1) + ",\"data1-3\":" + String(count1 + 2) + "}";

    Serial.println("------------------------------------");
    Serial.println("> Update Data 1...");
    if (Firebase.updateNode(firebaseData1, path + "/Stream/data1", json))
    {
      Serial.println("> PASSED");
      Serial.println("> PATH: " + firebaseData1.dataPath());
      Serial.println("> TYPE: " + firebaseData1.dataType());
      Serial.print("> VALUE: ");
      if (firebaseData1.dataType() == "int")
        Serial.println(firebaseData1.intData());
      else if (firebaseData1.dataType() == "float")
        Serial.println(firebaseData1.floatData(), 5);
      else if (firebaseData1.dataType() == "double")
        printf("%.9lf\n", firebaseData1.doubleData());
      else if (firebaseData1.dataType() == "boolean")
        Serial.println(firebaseData1.boolData() == 1 ? "true" : "false");
      else if (firebaseData1.dataType() == "string")
        Serial.println(firebaseData1.stringData());
      else if (firebaseData1.dataType() == "json")
        Serial.println(firebaseData1.jsonData());
      Serial.println("------------------------------------");
      Serial.println();
    }
    else
    {
      Serial.println("> FAILED");
      Serial.println("> REASON: " + firebaseData1.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }
  }

  if (millis() - sendDataPrevMillis2 > 20000)
  {
    sendDataPrevMillis2 = millis();
    count2++;

    json = "{\"data2-1\":" + String(count2) + ",\"data2-2\":" + String(count2 + 1) + ",\"data2-3\":" + String(count2 + 2) + "}";

    Serial.println("------------------------------------");
    Serial.println(">> Update Data 2...");

    if (Firebase.updateNode(firebaseData2, path + "/Stream/data2", json))
    {
      Serial.println(">> PASSED");
      Serial.println(">> PATH: " + firebaseData2.dataPath());
      Serial.println(">> TYPE: " + firebaseData2.dataType());
      Serial.print(">> VALUE: ");
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
        Serial.println(firebaseData2.jsonData());
      Serial.println("------------------------------------");
      Serial.println();
    }
    else
    {
      Serial.println(">> FAILED");
      Serial.println(">> REASON: " + firebaseData2.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }
  }
}
