
/**
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt
 * 
 * Copyright (c) 2020 mobizt
 *
*/

/*
 * This shows how to pause the Firbase and send LINE Notify.
 * 1. Install HTTPClientESP32Ex library for use with Line Notify library from https://github.com/mobizt/HTTPClientESP32Ex
 * 2. Install Line Notify Arduino library for ESP32 https://github.com/mobizt/LINE-Notify-ESP32
 *
 * More about Line Notify service https://notify-bot.line.me/en/
 */

#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>
#include <FirebaseESP32.h>
#include <LineNotifyESP32.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define FIREBASE_HOST "PROJECT_ID.firebaseio.com"

/** The database secret is obsoleted, please use other authentication methods, 
 * see examples in the Authentications folder. 
*/
#define FIREBASE_AUTH "DATABASE_SECRET"
#define LINE_TOKEN "LINE_NOTIFY_TOKEN"

//Define Firebase Data object
FirebaseData fbdo;

HTTPClientESP32Ex client;

String path = "/Test";

unsigned long sendDataPrevMillis = 0;

unsigned long sendMessagePrevMillis = 0;

uint16_t count = 0;

void printResult(FirebaseData &data);

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

  lineNotify.init(LINE_TOKEN);

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  Serial.println("------------------------------------");
  Serial.println("Begin stream...");
  if (!Firebase.beginStream(fbdo, path + "/Stream/String"))
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("PASSED");
    Serial.println("------------------------------------");
    Serial.println();
  }
}

void loop()
{

  if (millis() - sendDataPrevMillis > 30000)
  {
    sendDataPrevMillis = millis();
    count++;

    Serial.println("------------------------------------");
    Serial.println("Set Data...");
    if (Firebase.setString(fbdo, path + "/Stream/String", "Hello World! " + String(count)))
    {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
      Serial.print("VALUE: ");
      printResult(fbdo);
      Serial.println("--------------------------------");
      Serial.println();
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
      Serial.println("--------------------------------");
      Serial.println();
    }

    Serial.println("------------------------------------");
    Serial.println("Send Line Message...");

    //Pause Firebase and use WiFiClient accessed through client
    uint8_t status = lineNotify.sendLineMessage(client, "Instant sending message after call!");
    if (status == LineNotifyESP32::LineStatus::SENT_COMPLETED)
      Serial.println("PASSED");
    else if (status == LineNotifyESP32::LineStatus::SENT_FAILED)
      Serial.println("FAILED");
    else if (status == LineNotifyESP32::LineStatus::CONNECTION_FAILED)
      Serial.println("FAILED");
    Serial.println("--------------------------------");
    Serial.println();
  }

  if (millis() - sendMessagePrevMillis > 60000)
  {
    sendMessagePrevMillis = millis();

    Serial.println("------------------------------------");
    Serial.println("Send Line Message...");

    uint8_t status = lineNotify.sendLineMessage(client, "Schedule message sending!");
    if (status == LineNotifyESP32::LineStatus::SENT_COMPLETED)
      Serial.println("PASSED");
    else if (status == LineNotifyESP32::LineStatus::SENT_FAILED)
      Serial.println("FAILED");
    else if (status == LineNotifyESP32::LineStatus::CONNECTION_FAILED)
      Serial.println("FAILED");
    Serial.println("--------------------------------");
    Serial.println();
  }

  if (!Firebase.readStream(fbdo))
  {
    Serial.println("------------------------------------");
    Serial.println("Read stream...");
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  if (fbdo.streamTimeout())
  {
    Serial.println("Stream timeout, resume streaming...");
    Serial.println();
  }

  if (fbdo.streamAvailable())
  {
    Serial.println("------------------------------------");
    Serial.println("Stream Data available...");
    Serial.println("STREAM PATH: " + fbdo.streamPath());
    Serial.println("EVENT PATH: " + fbdo.dataPath());
    Serial.println("DATA TYPE: " + fbdo.dataType());
    Serial.println("EVENT TYPE: " + fbdo.eventType());
    Serial.print("VALUE: ");
    printResult(fbdo);
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
      Serial.print(type == FirebaseJson::JSON_OBJECT ? "object" : "array");
      if (type == FirebaseJson::JSON_OBJECT)
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
      if (jsonData.typeNum == FirebaseJson::JSON_BOOL)
        Serial.println(jsonData.boolValue ? "true" : "false");
      else if (jsonData.typeNum == FirebaseJson::JSON_INT)
        Serial.println(jsonData.intValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_FLOAT)
        Serial.println(jsonData.floatValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_DOUBLE)
        printf("%.9lf\n", jsonData.doubleValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_STRING ||
               jsonData.typeNum == FirebaseJson::JSON_NULL ||
               jsonData.typeNum == FirebaseJson::JSON_OBJECT ||
               jsonData.typeNum == FirebaseJson::JSON_ARRAY)
        Serial.println(jsonData.stringValue);
    }
  }
}
