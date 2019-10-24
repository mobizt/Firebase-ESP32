/*
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt
 * 
 * Copyright (c) 2019 mobizt
 * 
 * This example is for FirebaseESP32 Arduino library v 3.5.0 and later
 *
*/

//This example shows how to store and read binary data from memory to database.


#include <WiFi.h>
#include <FirebaseESP32.h>
#include <SD.h>


#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"
#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

//Define Firebase Data object
FirebaseData firebaseData;

String path = "/Test";

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
  Serial.println("Set BLOB data test...");

  //Create demo data
  uint8_t data[256];
  for (int i = 0; i < 256; i++)
    data[i] = i;

  //Set binary data to database (also can use Firebase.set)
  if (Firebase.setBlob(firebaseData, path + "/Binary/Blob/data", data, sizeof(data)))
  {
    Serial.println("PASSED");
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  Serial.println("------------------------------------");
  Serial.println("Get BLOB data test...");

  //Get binary data from database (also can use Firebase.get)
  if (Firebase.getBlob(firebaseData, path + "/Binary/Blob/data"))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.print("VALUE: ");
    if (firebaseData.dataType() == "blob")
    {

      std::vector<uint8_t> blob = firebaseData.blobData();

      Serial.println();

      for (int i = 0; i < blob.size(); i++)
      {
        if (i > 0 && i % 16 == 0)
          Serial.println();

        if (i < 16)
          Serial.print("0");

        Serial.print(blob[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
    }
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  Serial.println("------------------------------------");
  Serial.println("Append BLOB data test...");

  //Create demo data
  for (int i = 0; i < 256; i++)
    data[i] = 255 - i;

  //Append binary data to database (also can use Firebase.push)
  if (Firebase.pushBlob(firebaseData, path + "/Binary/Blob/Logs", data, sizeof(data)))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("PUSH NAME: " + firebaseData.pushName());
    Serial.println("------------------------------------");
    Serial.println();

    Serial.println("------------------------------------");
    Serial.println("Get appended BLOB data test...");

    //Get appended binary data from database (also can use Firebase.get)
    if (Firebase.getBlob(firebaseData, path + "/Binary/Blob/Logs/" + firebaseData.pushName()))
    {
      Serial.println("PASSED");
      Serial.println("PATH: " + firebaseData.dataPath());
      Serial.println("TYPE: " + firebaseData.dataType());
      Serial.print("VALUE: ");
      if (firebaseData.dataType() == "blob")
      {

        std::vector<uint8_t> blob = firebaseData.blobData();
        Serial.println();
        for (int i = 0; i < blob.size(); i++)
        {
          if (i > 0 && i % 16 == 0)
            Serial.println();
          if (blob[i] < 16)
            Serial.print("0");
          Serial.print(blob[i], HEX);
          Serial.print(" ");
        }
        Serial.println();
      }
      Serial.println("------------------------------------");
      Serial.println();
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
}

void loop()
{
}