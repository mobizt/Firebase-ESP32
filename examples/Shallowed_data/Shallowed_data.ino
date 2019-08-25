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

//This example shows how to retrieve all database data using shallowed data.


#include <WiFi.h>
#include <FirebaseESP32.h>


#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com"
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"
#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"


//Define Firebase Data object
FirebaseData firebaseData;

String path = "/";

void getNode(String &key);

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

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

  //The following test will print all your data in database
  //The absolute path of nested child may exceed the path buffer and lead to null result. 

  Serial.println("------------------------------------");
  Serial.println("Shallowed Data test...");
  Serial.println();

  getNode(path);

  Serial.println();
  Serial.println("FINISHED");
  Serial.println("------------------------------------");
}

void loop()
{
}

void getNode(String &key)
{

  if (Firebase.getShallowData(firebaseData, key))
  {
    if (firebaseData.dataType() != "json")
    {
      Serial.print(key);
      Serial.print("->");

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
      else if (firebaseData.dataType() == "null")
        Serial.println("null");
    }
    else
    {

      FirebaseJson json;

      json.setJsonData(firebaseData.jsonData());

      json.parse();
      size_t count =json.getJsonObjectIteratorCount();
      String _key;
      String _val;

      for (size_t i = 0; i< count; i++)
      {
        
        json.jsonObjectiterator(i,_key,_val);

        if (_val == "true")
        {
          if (key == "/")
          {
            getNode(_key);
          }
          else
          {
            getNode(key + "/" + _key);
          }
        }
      }
    }
  }
  else
  {
    Serial.println(firebaseData.errorReason());
  }
}