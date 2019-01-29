
//Required HTTPClientESP32Ex library to be installed  https://github.com/mobizt/HTTPClientESP32Ex

#include <WiFi.h>
#include "FirebaseESP32.h"


#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com"
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"
#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

//Define FirebaseESP32 data object
FirebaseData firebaseData;

unsigned long sendDataPrevMillis = 0;

String path = "/ESP32_Test/Stream";

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


  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  if (!Firebase.beginStream(firebaseData, path)) {
    Serial.println("------Can't begin stream connection------");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println();
  }


}



void loop() {

  if (millis() - sendDataPrevMillis > 15000) {
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
