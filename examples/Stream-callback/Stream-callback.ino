

//Required HTTPClientESP32Ex library to be installed  https://github.com/mobizt/HTTPClientESP32Ex


#include <WiFi.h>
#include "FirebaseESP32.h"

#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com"
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"

//Use multiple FirebaseData may lead to low memory and reboot from exception error

//Define FirebaseESP32 data object
FirebaseData firebaseData1;
FirebaseData firebaseData2;



unsigned long sendDataPrevMillis1;
unsigned long sendDataPrevMillis2;

uint16_t count1;
uint16_t count2;

String json;

void streamCallback1(streamData data)
{

  Serial.println("-------Stream Data1 available-------");
  Serial.println("STREAM PATH: " + data.streamPath());
  Serial.println("PATH: " + data.dataPath());
  Serial.println("TYPE: " + data.dataType());
  Serial.print("VALUE: ");
  if (data.dataType() == "int")
    Serial.println(data.intData());
  else if (data.dataType() == "float")
    Serial.println(data.floatData());
  else if (data.dataType() == "string")
    Serial.println(data.stringData());
  else if (data.dataType() == "json")
    Serial.println(data.jsonData());
  Serial.println();
}

void streamTimeoutCallback1(){
  Serial.println("Stream 1 timeout, resume streaming...");
}

void streamCallback2(streamData data)
{

  Serial.println("-------Stream Data2 available-------");
  Serial.println("STREAM PATH: " + data.streamPath());
  Serial.println("PATH: " + data.dataPath());
  Serial.println("TYPE: " + data.dataType());
  Serial.print("VALUE: ");
  if (data.dataType() == "int")
    Serial.println(data.intData());
  else if (data.dataType() == "float")
    Serial.println(data.floatData());
  else if (data.dataType() == "string")
    Serial.println(data.stringData());
  else if (data.dataType() == "json")
    Serial.println(data.jsonData());
  Serial.println();
}

void streamTimeoutCallback2(){
  Serial.println("Stream 2 timeout, resume streaming...");
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

  

  if (!Firebase.beginStream(firebaseData1, "/test/data1"))
  {
    Serial.println("------Can't begin stream 1 connection------");
    Serial.println("REASON: " + firebaseData1.errorReason());
    Serial.println();
  }

  Firebase.setStreamCallback(firebaseData1, streamCallback1,streamTimeoutCallback1);
  //Firebase.setStreamCallback(firebaseData1, streamCallback1);
 

  if (!Firebase.beginStream(firebaseData2, "/test/data2"))
  {
    Serial.println("------Can't begin stream 2 connection------");
    Serial.println("REASON: " + firebaseData2.errorReason());
    Serial.println();
  }
  
  Firebase.setStreamCallback(firebaseData2, streamCallback2, streamTimeoutCallback2);
  //Firebase.setStreamCallback(firebaseData2, streamCallback2);


}

void loop()
{

  if (millis() - sendDataPrevMillis1 > 28000)
  {
    sendDataPrevMillis1 = millis();
    count1++;

    json = "{\"data1-1\":" + String(count1) + ",\"data1-2\":" + String(count1 + 1) + ",\"data1-3\":" + String(count1 + 2) + "}";

    if (Firebase.updateNode(firebaseData1, "/test/data1", json))
    {
      Serial.println("----------Update Data 1 result-----------");
      Serial.println("PATH: " + firebaseData1.dataPath());
      Serial.println("TYPE: " + firebaseData1.dataType());
      Serial.print("VALUE: ");
      if (firebaseData1.dataType() == "int")
        Serial.println(firebaseData1.intData());
      else if (firebaseData1.dataType() == "float")
        Serial.println(firebaseData1.floatData());
      else if (firebaseData1.dataType() == "string")
        Serial.println(firebaseData1.stringData());
      else if (firebaseData1.dataType() == "json")
        Serial.println(firebaseData1.jsonData());
      Serial.println("--------------------------------");
      Serial.println();
    }
    else
    {
      Serial.println("----------Can't update data 1--------");
      Serial.println("REASON: " + firebaseData1.errorReason());
      Serial.println("--------------------------------");
      Serial.println();
    }
  }

  if (millis() - sendDataPrevMillis2 > 20000)
  {
    sendDataPrevMillis2 = millis();
    count2++;

    json = "{\"data2-1\":" + String(count2) + ",\"data2-2\":" + String(count2 + 1) + ",\"data2-3\":" + String(count2 + 2) + "}";

    if (Firebase.updateNode(firebaseData2, "/test/data2", json))
    {
      Serial.println("----------Update Data 2 result-----------");
      Serial.println("PATH: " + firebaseData2.dataPath());
      Serial.println("TYPE: " + firebaseData2.dataType());
      Serial.print("VALUE: ");
      if (firebaseData2.dataType() == "int")
        Serial.println(firebaseData2.intData());
      else if (firebaseData2.dataType() == "float")
        Serial.println(firebaseData2.floatData());
      else if (firebaseData2.dataType() == "string")
        Serial.println(firebaseData2.stringData());
      else if (firebaseData2.dataType() == "json")
        Serial.println(firebaseData2.jsonData());
      Serial.println("--------------------------------");
      Serial.println();
    }
    else
    {
      Serial.println("----------Can't update data 2--------");
      Serial.println("REASON: " + firebaseData2.errorReason());
      Serial.println("--------------------------------");
      Serial.println();
    }
  }
}
