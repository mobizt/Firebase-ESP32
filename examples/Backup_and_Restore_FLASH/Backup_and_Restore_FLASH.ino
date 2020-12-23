/**
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt
 * 
 * Copyright (c) 2020 mobizt
 *
*/

//This example shows how to backup and restore database data

#include <WiFi.h>
#include <FirebaseESP32.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define FIREBASE_HOST "PROJECT_ID.firebaseio.com"

/** The database secret is obsoleted, please use other authentication methods, 
 * see examples in the Authentications folder. 
*/
#define FIREBASE_AUTH "DATABASE_SECRET"

//Define Firebase Data object
FirebaseData fbdo;

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

  Serial.println("------------------------------------");
  Serial.println("Backup test...");

  //Download and save data to Flash memory.
  //{TARGET_NODE_PATH} is the full path of database to backup and restore.
  //{FILE_NAME} is file name included path to save to Flash meory

  if (!Firebase.backup(fbdo, StorageType::FLASH, "/{TARGET_NODE_PATH}", "/{FILE_NAME}"))
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.fileTransferError());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("PASSED");
    Serial.println("BACKUP FILE: " + fbdo.getBackupFilename());
    Serial.println("FILE SIZE: " + String(fbdo.getBackupFileSize()));
    Serial.println("------------------------------------");
    Serial.println();
  }

  Serial.println("------------------------------------");
  Serial.println("Restore test...");

  //Restore data to defined database path using backup file on Flash memory.
  //{TARGET_NODE_PATH} is the full path of database to restore
  //{FILE_NAME} is file name included path of backed up file.

  if (!Firebase.restore(fbdo, StorageType::FLASH, "/{TARGET_NODE_PATH}", "/{FILE_NAME}"))
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.fileTransferError());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("PASSED");
    Serial.println("BACKUP FILE: " + fbdo.getBackupFilename());
    Serial.println("------------------------------------");
    Serial.println();
  }
}

void loop()
{
}
