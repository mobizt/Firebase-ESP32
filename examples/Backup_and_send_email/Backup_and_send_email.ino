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


//This example shows how to backup database and send the Email


#include <WiFi.h>
#include "FirebaseESP32.h"

/*
   Required ESP32 MailClient library for Arduino
   https://github.com/mobizt/ESP32-Mail-Client
*/

#include "ESP32_MailClient.h"

#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com" //Without http:// or https://
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"
#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

//Define Firebase Data object
FirebaseData firebaseData;

//The Email Sending data object contains config and data to send
SMTPData smtpData;

//Callback function to get the Email sending status
void sendCallback(SendStatus info);

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

  //Print to see stack size and free memory
  UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
  Serial.print("Stack: ");
  Serial.println(uxHighWaterMark);
  Serial.print("Heap: ");
  Serial.println(esp_get_free_heap_size());

  Serial.println();

  Serial.println("------------------------------------");
  Serial.println("Backup test...");

  //Provide specific SD card interface
  //Firebase.sdBegin(14, 2, 15, 13); //SCK, MISO, MOSI,SS for TTGO T8 v1.7 or 1.8

  //Download and save data at defined database path to SD card.
  //{TARGET_NODE_PATH} is the full path of database to backup.

  if (!Firebase.backup(firebaseData, "/TARGET_NODE_PATH", "/PATH_IN_SD_CARD"))
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.fileTransferError());
	Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("PASSED");
    Serial.println("SAVE PATH: " + firebaseData.getBackupFilename());
    Serial.println("FILE SIZE: " + String(firebaseData.getBackupFileSize()));
	Serial.println("------------------------------------");
    Serial.println();

    //
    if (firebaseData.pauseFirebase(true)) {

      //Send backup file via Email

      Serial.println("------------------------------------");
      Serial.println("Send Email...");

      //Set the Email host, port, account and password
      smtpData.setLogin("smtp.gmail.com", 465, "YOUR_EMAIL_ACCOUNT@gmail.com", "YOUR_EMAIL_PASSWORD");

      //Set the sender name and Email
      smtpData.setSender("ESP32", "SOME_EMAIL_ACCOUNT@SOME_EMAIL.com");

      //Set Email priority or importance High, Normal, Low or 1 to 5 (1 is highest)
      smtpData.setPriority("High");

      //Set the subject
      smtpData.setSubject("Firebase Database Backup File");

      //Set the message - normal text or html format
      smtpData.setMessage("<div style=\"color:#cc3399;font-size:14px;\">Firebase Database Backup File<br/><br/>Sent from ESP32</div>", true);

      //Add recipients, can add more than one recipient
      smtpData.addRecipient("someone@somemail.com");

      //Add attachment file (backup file) from SD card
      smtpData.addAttachFile(firebaseData.getBackupFilename());

      smtpData.setSendCallback(sendCallback);

      //Start sending Email, can be set callback function to track the status
      if (!MailClient.sendMail(firebaseData.http, smtpData))
        Serial.println("Error sending Email, " + MailClient.smtpErrorReason());

      //Clear all data from Email object to free memory
      smtpData.empty();

      Serial.println();
    } else {
      Serial.println("Could not pause Firebase");
    }

  }

  //Quit Firebase and release all resources
  Firebase.end(firebaseData);
}

void loop()
{
}

//Callback function to get the Email sending status
void sendCallback(SendStatus msg)
{
  //Print the current status
  Serial.println(msg.info());

}
