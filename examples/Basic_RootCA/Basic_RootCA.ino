
//Required HTTPClientESP32Ex library to be installed  https://github.com/mobizt/HTTPClientESP32Ex

#include <WiFi.h>
#include "FirebaseESP32.h"


#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com"
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"
#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"



const char* root_ca =  "-----BEGIN CERTIFICATE-----\n" \
                       "MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4G\n" \
                       "A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNp\n" \
                       "Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1\n" \
                       "MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEG\n" \
                       "A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI\n" \
                       "hvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6+Lm8omUVCxKs+IVSbC9N/hHD6ErPL\n" \
                       "v4dfxn+G07IwXNb9rfF73OX4YJYJkhD10FPe+3t+c4isUoh7SqbKSaZeqKeMWhG8\n" \
                       "eoLrvozps6yWJQeXSpkqBy+0Hne/ig+1AnwblrjFuTosvNYSuetZfeLQBoZfXklq\n" \
                       "tTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzd\n" \
                       "C9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ/gkwpRl4pa\n" \
                       "zq+r1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCB\n" \
                       "mTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUm+IH\n" \
                       "V2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5n\n" \
                       "bG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG\n" \
                       "3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4Gs\n" \
                       "J0/WwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO\n" \
                       "291xNBrBVNpGP+DTKqttVCL1OmLNIG+6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavS\n" \
                       "ot+3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h+u/N5GJG79G+dwfCMNYxd\n" \
                       "AfvDbbnvRG15RjF+Cv6pgsH/76tuIMRQyV+dTZsXjAzlAcmgQWpzU/qlULRuJQ/7\n" \
                       "TBj0/VLZjmmx6BEP3ojY+x1J96relc8geMJgEtslQIxq/H5COEBkEveegeGTLg==\n" \
                       "-----END CERTIFICATE-----\n";




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


  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, root_ca);
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
