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


//This example shows how to store and read binary data from file on Flash memory to database.

#include <WiFi.h>
#include <FirebaseESP32.h>


#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"
#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

//Define Firebase Data object
FirebaseData firebaseData;

String path = "/ESP32_Test";

File file;

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

  if (!SPIFFS.begin(true))
  {
    Serial.println("SPIFFS initialization failed.");
    return;
  }
  

  //Delete demo files
  if (SPIFFS.exists("/file1.txt"))
    SPIFFS.remove("/file1.txt");

  if (SPIFFS.exists("/file2.txt"))
    SPIFFS.remove("/file2.txt");

  if (SPIFFS.exists("/file3.txt"))
    SPIFFS.remove("/file3.txt");

  Serial.println("------------------------------------");
  Serial.println("Set file data test...");

  //Write demo data to file
  file = SPIFFS.open("/file1.txt", "w");
  for (int i = 0; i < 256; i++)
    file.write((uint8_t)i);

  file.close();


  //Set file (read file from Flash memory and set to database)
  if (Firebase.setFile(firebaseData, StorageType::SPIFFS, path + "/Binary/File/data", "/file1.txt"))
  {
    Serial.println("PASSED");
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.fileTransferError());
    Serial.println("------------------------------------");
    Serial.println();
  }

  Serial.println("------------------------------------");
  Serial.println("Get file data test...");

  //Get file (download file to Flash memory)
  if (Firebase.getFile(firebaseData, StorageType::SPIFFS, path + "/Binary/File/data", "/file2.txt"))
  {

    Serial.println("PASSED");
    Serial.println("DATA");

    //Readout the downloaded file
    file = SPIFFS.open("/file2.txt", "r");
    int i = 0;

    while (file.available())
    {
      if (i > 0 && i % 16 == 0)
        Serial.println();

      if (i < 16)
        Serial.print("0");

      Serial.print(file.read(), HEX);
      Serial.print(" ");
      i++;
    }
    Serial.println();
    Serial.println("------------------------------------");
    Serial.println();
    file.close();
  }
  else
  {

    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.fileTransferError());
    Serial.println("------------------------------------");
    Serial.println();
  }

  Serial.println("------------------------------------");
  Serial.println("Append file data test...");

  if (SPIFFS.exists("/file1.txt"))
    SPIFFS.remove("/file1.txt");

  //Write demo data to file
  file = SPIFFS.open("/file1.txt", "w");
  for (int i = 255; i >= 0; i--)
    file.write((uint8_t)i);

  file.close();

  //Append file data to database
  if (Firebase.pushFile(firebaseData, StorageType::SPIFFS, path + "/Binary/File/Logs", "/file1.txt"))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("PUSH NAME: " + firebaseData.pushName());
    Serial.println("------------------------------------");

    Serial.println();

    Serial.println("------------------------------------");
    Serial.println("Get appended file data test...");

    //Get the recently appended file (download file to Flash memory)
    if (Firebase.getFile(firebaseData, StorageType::SPIFFS, path + "/Binary/File/Logs/" + firebaseData.pushName(), "/file3.txt"))
    {

      Serial.println("PASSED");
      Serial.println("DATA");

      //Readout the downloaded file
      file = SPIFFS.open("/file3.txt", "r");
      int i = 0;
      int idx = 0;

      while (file.available())
      {
        i = file.read();
        if (i < 16)
          Serial.print("0");

        Serial.print(i, HEX);
        Serial.print(" ");

        if (idx > 0 && (idx + 1) % 16 == 0)
          Serial.println();
        idx++;
      }
      Serial.println();
      Serial.println("------------------------------------");
      Serial.println();
      file.close();
    }
    else
    {

      Serial.println("FAILED");
      Serial.println("REASON: " + firebaseData.fileTransferError());
      Serial.println("------------------------------------");
      Serial.println();
    }
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.fileTransferError());
    Serial.println("------------------------------------");
    Serial.println();
  }
}

void loop()
{
}
