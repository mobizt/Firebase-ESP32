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
#include "FirebaseESP32.h"
#include "SD.h"


#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"
#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

//Define Firebase Data object
FirebaseData firebaseData;


String path = "/ESP32_Test";

File file;


void setup() {

  Serial.begin(115200);
  Serial.println();
  Serial.println();

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

  if (SD.exists("/target_2.txt"))
    SD.remove("/target_2.txt");

  if (SD.exists("/target_3.txt"))
    SD.remove("/target_3.txt");

  if (SD.exists("/target_4.txt"))
    SD.remove("/target_4.txt");


  if (SD.exists("/target_5.txt"))
    SD.remove("/target_5.txt");


  if (SD.exists("/target_6.txt"))
    SD.remove("/target_6.txt");

  if (SD.exists("/target_7.txt"))
    SD.remove("/target_7.txt");

  if (SD.exists("/target_8.txt"))
    SD.remove("/target_8.txt");

  if (SD.exists("/target_9.txt"))
    SD.remove("/target_9.txt");

  if (SD.exists("/target_10.txt"))
    SD.remove("/target_10.txt");

  if (SD.exists("/push_in.txt"))
    SD.remove("/push_in.txt");

  if (SD.exists("/push_out.txt"))
    SD.remove("/push_out.txt");

 

  /*/////////////////////////////////////////////////////

  Sending large file ( > 10k) leads to unexpected result

  /////////////////////////////////////////////////////*/


  //Write demo data to file (8192 bytes)
  file = SD.open("/source.txt", FILE_WRITE);
  for (int i = 0; i < 8192; i++)
    file.write(0xf);

  file.close();

  
  
  Serial.println("-----------------------------------");
  Serial.println("Set file data 1 test...");


  //Set file (read file from SD card and set to database)
  if (Firebase.setFile(firebaseData, path + "/Binary/File/data1", "/source.txt"))
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
  Serial.println("Set file data 2 test...");


  //Set file (read file from SD card and set to database)
  if (Firebase.setFile(firebaseData, path + "/Binary/File/data2", "/source.txt"))
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
  Serial.println("Set file data 3 test...");


  //Set file (read file from SD card and set to database)
  if (Firebase.setFile(firebaseData, path + "/Binary/File/data3", "/source.txt"))
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
  Serial.println("Set file data 4 test...");


  //Set file (read file from SD card and set to database)
  if (Firebase.setFile(firebaseData, path + "/Binary/File/data4", "/source.txt"))
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
  Serial.println("Set file data 5 test...");


  //Set file (read file from SD card and set to database)
  if (Firebase.setFile(firebaseData, path + "/Binary/File/data5", "/source.txt"))
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
  Serial.println("Set file data 6 test...");


  //Set file (read file from SD card and set to database)
  if (Firebase.setFile(firebaseData, path + "/Binary/File/data6", "/source.txt"))
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
  Serial.println("Set file data 7 test...");


  //Set file (read file from SD card and set to database)
  if (Firebase.setFile(firebaseData, path + "/Binary/File/data7", "/source.txt"))
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
  Serial.println("Set file data 8 test...");


  //Set file (read file from SD card and set to database)
  if (Firebase.setFile(firebaseData, path + "/Binary/File/data8", "/source.txt"))
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
  Serial.println("Set file data 9 test...");


  //Set file (read file from SD card and set to database)
  if (Firebase.setFile(firebaseData, path + "/Binary/File/data9", "/source.txt"))
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
  Serial.println("Set file data 10 test...");


  //Set file (read file from SD card and set to database)
  if (Firebase.setFile(firebaseData, path + "/Binary/File/data10", "/source.txt"))
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
  if (Firebase.getFile(firebaseData, path + "/Binary/File/data1", "/target_1.txt"))
  {

    Serial.println("PASSED");
    Serial.println("------------------------------------");
    Serial.println();

  }
  else
  {

    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.fileTransferError());
    Serial.println("--------------------------------");
    Serial.println();
  }



  Serial.println("-----------------------------------");
  Serial.println("Get file data 2 test...");

  //Get file (download file to SD card)
  if (Firebase.getFile(firebaseData, path + "/Binary/File/data2", "/target_2.txt"))
  {

    Serial.println("PASSED");
    Serial.println("------------------------------------");
    Serial.println();

  }
  else
  {

    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.fileTransferError());
    Serial.println("--------------------------------");
    Serial.println();
  }



  Serial.println("-----------------------------------");
  Serial.println("Get file data 3 test...");

  //Get file (download file to SD card)
  if (Firebase.getFile(firebaseData, path + "/Binary/File/data3", "/target_3.txt"))
  {

    Serial.println("PASSED");
    Serial.println("------------------------------------");
    Serial.println();

  }
  else
  {

    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.fileTransferError());
    Serial.println("--------------------------------");
    Serial.println();
  }


  Serial.println("-----------------------------------");
  Serial.println("Get file data 4 test...");

  //Get file (download file to SD card)
  if (Firebase.getFile(firebaseData, path + "/Binary/File/data4", "/target_4.txt"))
  {

    Serial.println("PASSED");
    Serial.println("------------------------------------");
    Serial.println();

  }
  else
  {

    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.fileTransferError());
    Serial.println("--------------------------------");
    Serial.println();
  }



  Serial.println("-----------------------------------");
  Serial.println("Get file data 5 test...");

  //Get file (download file to SD card)
  if (Firebase.getFile(firebaseData, path + "/Binary/File/data5", "/target_5.txt"))
  {

    Serial.println("PASSED");
    Serial.println("------------------------------------");
    Serial.println();

  }
  else
  {

    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.fileTransferError());
    Serial.println("--------------------------------");
    Serial.println();
  }



  Serial.println("-----------------------------------");
  Serial.println("Get file data 6 test...");

  //Get file (download file to SD card)
  if (Firebase.getFile(firebaseData, path + "/Binary/File/data6", "/target_6.txt"))
  {

    Serial.println("PASSED");
    Serial.println("------------------------------------");
    Serial.println();

  }
  else
  {

    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.fileTransferError());
    Serial.println("--------------------------------");
    Serial.println();
  }


  Serial.println("-----------------------------------");
  Serial.println("Get file data 7 test...");

  //Get file (download file to SD card)
  if (Firebase.getFile(firebaseData, path + "/Binary/File/data7", "/target_7.txt"))
  {

    Serial.println("PASSED");
    Serial.println("------------------------------------");
    Serial.println();

  }
  else
  {

    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.fileTransferError());
    Serial.println("--------------------------------");
    Serial.println();
  }


  Serial.println("-----------------------------------");
  Serial.println("Get file data 8 test...");

  //Get file (download file to SD card)
  if (Firebase.getFile(firebaseData, path + "/Binary/File/data8", "/target_8.txt"))
  {

    Serial.println("PASSED");
    Serial.println("------------------------------------");
    Serial.println();

  }
  else
  {

    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.fileTransferError());
    Serial.println("--------------------------------");
    Serial.println();
  }



  Serial.println("-----------------------------------");
  Serial.println("Get file data 9 test...");

  //Get file (download file to SD card)
  if (Firebase.getFile(firebaseData, path + "/Binary/File/data9", "/target_9.txt"))
  {

    Serial.println("PASSED");
    Serial.println("------------------------------------");
    Serial.println();

  }
  else
  {

    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.fileTransferError());
    Serial.println("--------------------------------");
    Serial.println();
  }


  Serial.println("-----------------------------------");
  Serial.println("Get file data 10 test...");

  //Get file (download file to SD card)
  if (Firebase.getFile(firebaseData, path + "/Binary/File/data10", "/target_10.txt"))
  {

    Serial.println("PASSED");
    Serial.println("------------------------------------");
    Serial.println();

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
  if (Firebase.pushFile(firebaseData, path + "/Binary/File/Logs", "/push_in.txt"))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("PUSH NAME: " + firebaseData.pushName());
    Serial.println("-------------------------------------");

    Serial.println();

    Serial.println("-----------------------------------");
    Serial.println("Get appended file data test...");

    //Get the recently appended file (download file to SD card)
    if (Firebase.getFile(firebaseData, path + "/Binary/File/Logs/" + firebaseData.pushName(), "/push_out.txt"))
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



void loop() {

}
