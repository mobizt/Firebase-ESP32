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

//This example shows how to store and read binary data from file on SD card to database.

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

  //Mount SD card

  if (!Firebase.sdBegin())
  {
    Serial.println("SD Card mounted failed");
    return;
  }

  /*
  if (!Firebase.sdBegin(14, 2, 15, 13)) //SCK, MISO, MOSI,SS for TTGO T8 v1.7 or 1.8
  {
    Serial.println("SD Card mounted failed");
    return;
  }
  */

  //Delete demo files
  if (SD.exists("/source.txt"))
    SD.remove("/source.txt");

  if (SD.exists("/target_1.txt"))
    SD.remove("/target_1.txt");

  if (SD.exists("/push_in.txt"))
    SD.remove("/push_in.txt");

  if (SD.exists("/push_out.txt"))
    SD.remove("/push_out.txt");

  //Write demo data to file (8192 bytes)
  file = SD.open("/source.txt", FILE_WRITE);
  uint8_t v = 0;
  for (int i = 0; i < 400000; i++)
  {
    file.write(v);
    v++;
  }

  file.close();

  Serial.println("-----------------------------------");
  Serial.println("Set file data 1 test...");

  //Set file (read file from SD card and set to database)
  if (Firebase.setFile(firebaseData, StorageType::SD, path + "/Binary/File/data1", "/source.txt"))
  {
    Serial.println("PASSED");
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.fileTransferError());
    Serial.println("-------------------------------------");
    Serial.println();
  }

  Serial.println("-----------------------------------");
  Serial.println("Get file data 1 test...");

  //Get file (download file to SD card)
  if (Firebase.getFile(firebaseData, StorageType::SD, path + "/Binary/File/data1", "/target_1.txt"))
  {

    //Need to begin SD card again due to File system closed by library
    SD.begin(); //or use Firebase.sdBegin();
    //Firebase.sdBegin(14, 2, 15, 13); //SCK, MISO, MOSI,SS for TTGO T8 v1.7 or 1.8

    Serial.println("PASSED");
    Serial.println("DATA");

    //Readout the downloaded file
    file = SD.open("/target_1.txt", FILE_READ);
    int i = 0;

    while (file.available())
    {
      if (i > 0 && i % 16 == 0)
        Serial.println();

      v = file.read();

      if (v < 16)
        Serial.print("0");

      Serial.print(v, HEX);
      Serial.print(" ");
      i++;
    }
    Serial.println();
    Serial.println("--------------------------------");
    Serial.println();
    file.close();
  }
  else
  {

    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.fileTransferError());
    Serial.println("--------------------------------");
    Serial.println();
  }

  Serial.println("-----------------------------------");
  Serial.println("Append file data test...");

  //Need to begin SD card again due to File system closed by library
  SD.begin(); //or use Firebase.sdBegin();
  //Firebase.sdBegin(14, 2, 15, 13); //SCK, MISO, MOSI,SS for TTGO T8 v1.7 or 1.8

  //Write demo data to file
  file = SD.open("/push_in.txt", FILE_WRITE);
  for (int i = 255; i >= 0; i--)
    file.write(i);

  file.close();

  //Append file data to database
  if (Firebase.pushFile(firebaseData, StorageType::SD, path + "/Binary/File/Logs", "/push_in.txt"))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("PUSH NAME: " + firebaseData.pushName());
    Serial.println("-------------------------------------");

    Serial.println();

    Serial.println("-----------------------------------");
    Serial.println("Get appended file data test...");

    //Get the recently appended file (download file to SD card)
    if (Firebase.getFile(firebaseData, StorageType::SD, path + "/Binary/File/Logs/" + firebaseData.pushName(), "/push_out.txt"))
    {

      Serial.println("PASSED");
      Serial.println("DATA");

      //Need to begin SD card again due to File system closed by library
      SD.begin(); //or use Firebase.sdBegin();
      //Firebase.sdBegin(14, 2, 15, 13); //SCK, MISO, MOSI,SS for TTGO T8 v1.7 or 1.8

      //Readout the downloaded file
      file = SD.open("/push_out.txt", FILE_READ);
      int i = 0;

      while (file.available())
      {
        i = file.read();
        if (i < 16)
          Serial.print("0");

        Serial.print(i, HEX);
        Serial.print(" ");
        if (i > 0 && i % 16 == 0)
          Serial.println();
      }
      Serial.println();
      Serial.println("-------------------------------------");
      Serial.println();
      file.close();
    }
    else
    {

      Serial.println("FAILED");
      Serial.println("REASON: " + firebaseData.fileTransferError());
      Serial.println("--------------------------------");
      Serial.println();
    }
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.fileTransferError());
    Serial.println("--------------------------------");
    Serial.println();
  }
}

void loop()
{
}
