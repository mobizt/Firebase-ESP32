
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

#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"
#define LINE_TOKEN "YOUR_LINE_NOTIFY_TOKEN"

//Define Firebase Data object
FirebaseData firebaseData;

HTTPClientESP32Ex client;

String path = "/ESP32_Test";

unsigned long sendDataPrevMillis = 0;

unsigned long sendMessagePrevMillis = 0;

uint16_t count = 0;

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
  if (!Firebase.beginStream(firebaseData, path + "/Stream/String"))
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
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
    if (Firebase.setString(firebaseData, path + "/Stream/String", "Hello World! " + String(count)))
    {
      Serial.println("PASSED");
      Serial.println("PATH: " + firebaseData.dataPath());
      Serial.println("TYPE: " + firebaseData.dataType());
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
      Serial.println("--------------------------------");
      Serial.println();
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + firebaseData.errorReason());
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

  if (!Firebase.readStream(firebaseData))
  {
    Serial.println("--------------------------------");
    Serial.println("Read stream...");
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("--------------------------------");
    Serial.println();
  }

  if (firebaseData.streamTimeout(bool timeout))
  {
    if (timeout)
    {
      Serial.println("Stream timeout, resume streaming...");
      Serial.println();
    }
  }

  if (firebaseData.streamAvailable())
  {
    Serial.println("Stream Data available...");
    Serial.println("STREAM PATH: " + firebaseData.streamPath());
    Serial.println("EVENT PATH: " + firebaseData.dataPath());
    Serial.println("DATA TYPE: " + firebaseData.dataType());
    Serial.println("EVENT TYPE: " + firebaseData.eventType());
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
    else if (firebaseData.dataType() == "blob")
    {
      //See blob examples
    }
    Serial.println();
  }
}
