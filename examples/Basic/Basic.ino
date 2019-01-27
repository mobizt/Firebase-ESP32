#include <WiFi.h>
#include "FirebaseESP32.h"


#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com"
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"
#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

//Define FirebaseESP32 data object
FirebaseData firebaseData;



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

  String path = "/ESP32_Test";
  String jsonStr;

  //Firebase.deleteNode(firebaseData, path);

  Serial.println("-----------------------------------");
  Serial.println("----------Path Exist Test----------");
  Serial.println("-----------------------------------");


  Serial.println();
  if (Firebase.pathExist(firebaseData, path)) {
    Serial.println("Path " + path + " exists");
  } else {
    Serial.println("Path " + path + " is not exist");
  }
  Serial.println();

  Serial.println("-----------------------------------");
  Serial.println("----------Begin Set Test-----------");
  Serial.println("-----------------------------------");
  Serial.println();

  for (uint8_t i = 0; i < 10; i++) {

    if (Firebase.setInt(firebaseData, path + "/Int/Data" + (i + 1), (i + 1) * 10)) {
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

  Serial.println("-----------------------------------");
  Serial.println("----------Begin Get Test-----------");
  Serial.println("-----------------------------------");
  Serial.println();

  for (uint8_t i = 0; i < 10; i++) {

    if (Firebase.getInt(firebaseData, path + "/Int/Data" + (i + 1))) {
      Serial.println("----------Get result-----------");
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
      Serial.println("----------Can't get data--------");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("--------------------------------");
      Serial.println();
    }

  }

  Serial.println("-----------------------------------");
  Serial.println("----------Begin Push Test----------");
  Serial.println("-----------------------------------");
  Serial.println();

  for (uint8_t i = 0; i < 5; i++) {

    if (Firebase.pushInt(firebaseData, path + "/Push/Int", (i + 1))) {
      Serial.println("----------Push result-----------");
      Serial.println("PATH: " + firebaseData.dataPath());
      Serial.print("PUSH NAME: ");
      Serial.println(firebaseData.pushName());
      Serial.println("--------------------------------");
      Serial.println();
    } else {
      Serial.println("----------Can't push data--------");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("--------------------------------");
      Serial.println();
    }

  }

  for (uint8_t i = 5; i < 10; i++) {

    jsonStr = "{\"Data" +  String(i + 1) + "\":" + String(i + 1) + "}";

    if (Firebase.pushJSON(firebaseData, path + "/Push/Int", jsonStr)) {
      Serial.println("----------Push result-----------");
      Serial.println("PATH: " + firebaseData.dataPath());
      Serial.print("PUSH NAME: ");
      Serial.println(firebaseData.pushName());
      Serial.println("--------------------------------");
      Serial.println();
    } else {
      Serial.println("----------Can't push data--------");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("--------------------------------");
      Serial.println();
    }

  }


  Serial.println("-----------------------------------");
  Serial.println("---------Begin Update Test----------");
  Serial.println("-----------------------------------");
  Serial.println();

  for (uint8_t i = 0; i < 5; i++) {

    jsonStr = "{\"Data" +  String(i + 1) + "\":" + String(i + 5.5) + "}";

    if (Firebase.updateNode(firebaseData, path + "/Int" , jsonStr)) {
      Serial.println("----------Update result-----------");
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
      Serial.println("----------Can't Update data--------");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("--------------------------------");
      Serial.println();
    }

  }

  if (Firebase.getJSON(firebaseData, path)) {
    Serial.println("----------Get result-----------");
    if (firebaseData.bufferOverflow())
      Serial.println("Data Buffer overflow, increase FIREBASE_DATA_SIZE and FIREBASE_RESPONSE_SIZE in FirebaseESP32.h");

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
    Serial.println("----------Can't get data--------");
    Serial.println("REASON: " + firebaseData.errorReason());
    if (firebaseData.bufferOverflow())
      Serial.println("Data Buffer overflow, increase FIREBASE_DATA_SIZE and FIREBASE_RESPONSE_SIZE in FirebaseESP32.h");

    Serial.println("--------------------------------");
    Serial.println();
  }


}



void loop() {


}
