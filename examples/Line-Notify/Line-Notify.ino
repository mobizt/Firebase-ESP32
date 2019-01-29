
/*
 * This shows how to pause the Firbase call and send LINE Notify message using the same shared SSL WiFi client.
 * 1. Install HTTPClientESP32Ex library from https://github.com/mobizt/HTTPClientESP32Ex
 * 2. Install Line notify Arduino library for ESP32 https://github.com/mobizt/LINE-Notify-ESP32
 * 
 */


#include <WiFi.h>
#include "FS.h"
#include "SPIFFS.h"
#include "FirebaseESP32.h"
#include "LineNotifyESP32.h"


//Include for use in this demo only
#include "image.h"




#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com"
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"
#define LINE_TOKEN "YOUR_LINE_NOTIFY_TOKEN"

FirebaseData firebaseData;

String path = "/ESP32_Test/Stream";

unsigned long sendDataPrevMillis = 0;

unsigned long sendMessagePrevMillis = 0;

uint16_t count = 0;

void setup() {

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
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

  if (!Firebase.beginStream(firebaseData, path)) {
    Serial.println("------Can't begin stream connection------");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println();
  }


}



void loop() {


  if (millis() - sendDataPrevMillis > 30000) {
    sendDataPrevMillis = millis();
    count++;

    if (Firebase.setString(firebaseData, path + "/String", "Hello World! " + String(count)) ) {
      Serial.println("----------Set result-----------");
      Serial.println("PATH: " + firebaseData.dataPath());
      Serial.println("TYPE: " + firebaseData.dataType());
      Serial.print("VALUE: ");
      if (firebaseData.dataType() == "int") Serial.println(firebaseData.intData());
      else if (firebaseData.dataType() == "float") Serial.println(firebaseData.floatData());
      else if (firebaseData.dataType() == "string") Serial.println(firebaseData.stringData());
      else if (firebaseData.dataType() == "json") Serial.println(firebaseData.jsonData());
      Serial.println("--------------------------------");
      Serial.println();
    } else {
      Serial.println("----------Can't set data--------");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("--------------------------------");
      Serial.println();
    }



    if (firebaseData.pauseFirebase(true)) {

      // Use firebaseData.http to get shared WiFi client
      uint8_t status =  lineNotify.sendLineMessage(firebaseData.http, "Instant sending message after call!");
      if (status == LineNotifyESP32::LineStatus::SENT_COMPLETED) Serial.println("Send text message completed");
      else if (status == LineNotifyESP32::LineStatus::SENT_FAILED) Serial.println("Send text message was failed!");
      else if (status == LineNotifyESP32::LineStatus::CONNECTION_FAILED) Serial.println("Connection to LINE sevice faild!");
      Serial.println();

      //Unpause WiFi client
      firebaseData.pauseFirebase(false);

    } else {
      Serial.println("----------Can't pause the WiFi client--------");
    }



  }

  if (millis() - sendMessagePrevMillis > 60000) {
    sendMessagePrevMillis = millis();
    if (firebaseData.pauseFirebase(true)) {

      // Use firebaseData.http to get shared WiFi client

      uint8_t status =  lineNotify.sendLineMessage(firebaseData.http, "Schedule message sending!");
      if (status == LineNotifyESP32::LineStatus::SENT_COMPLETED) Serial.println("Send text message completed");
      else if (status == LineNotifyESP32::LineStatus::SENT_FAILED) Serial.println("Send text message was failed!");
      else if (status == LineNotifyESP32::LineStatus::CONNECTION_FAILED) Serial.println("Connection to LINE sevice faild!");
      Serial.println();

      //Unpause WiFi client
      firebaseData.pauseFirebase(false);

    } else {
      Serial.println("----------Can't pause the WiFi client--------");
    }


  }



  if (!Firebase.readStream(firebaseData)) {
    Serial.println("Can't read stream data");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println();
  }




  if (firebaseData.streamTimeout()) {
    Serial.println("Stream timeout, resume streaming...");
    Serial.println();
  }

  if (firebaseData.streamAvailable()) {
    Serial.println("-------Stream Data available-------");
    Serial.println("STREAM PATH: " + firebaseData.streamPath());
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.print("VALUE: ");
    if (firebaseData.dataType() == "int") Serial.println(firebaseData.intData());
    else if (firebaseData.dataType() == "float") Serial.println(firebaseData.floatData());
    else if (firebaseData.dataType() == "string") Serial.println(firebaseData.stringData());
    else if (firebaseData.dataType() == "json") Serial.println(firebaseData.jsonData());
    Serial.println();
  }


}
